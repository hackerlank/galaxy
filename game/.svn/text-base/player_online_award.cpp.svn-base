#include "player_online_award.h"
#include "playerManager.h"
#include "mongoDB.h"
#include "helper.h"
#include "email_system.h"
#include "action_def.h"
#include "player_item.h"
#include "action_instance.hpp"
#include "action_format.hpp"
#include <vector>

using namespace std;

namespace gg
{
	playerOnlineAward::playerOnlineAward(playerData &own):Block(own)
	{
		m_PlayerAwardContent.clear();
		//m_DbUpdateTime = 0;
		goodsID = 13001;
		limitBase = 0;
	}


	playerOnlineAward::~playerOnlineAward(void)
	{

	}
	//判断CD时间
	bool playerOnlineAward::judgeCD(unsigned now){
		//计算出累积在线时间
		unsigned int onlineTime = now - m_PlayerMsg.initialTime;
		//比较累积时间是否大于剩余CD时间
		if(onlineTime>=m_PlayerMsg.awardCD){
			//清除CD时间
			m_PlayerMsg.awardCD = 0;
			return true;
		}
		return false;
	}

	//领取奖励
	int playerOnlineAward::getAward(playerDataPtr player, int typeIndicate, double ratio, Json::Value& fail_value)
	{
		int resultCode=0;
		if(typeIndicate!=m_PlayerMsg.awardTypeIndicate)
			return 1;
		unsigned int now = updateTime = na::time_helper::get_current_time();
		int length = (unsigned)m_PlayerAwardContent.size();
		if(judgeCD(now)&&length!=0){
			int type = m_PlayerMsg.awardTypeIndicate;
			if(type!=-1&&m_PlayerMsg.awardNum!=type&&m_PlayerMsg.awardNum<length){
				m_PlayerMsg.awardNum +=1;
				if (updatePlayer(player, ratio, fail_value))
					resultCode = 2;

			}
			if(type!=-1&&type<=length){
				m_PlayerMsg.awardCD = 0;  //改变现在奖励的剩余CD时间
				//激活下一个奖励,如果type=m_AwardContent.size()则没有下一个奖励了	
				if(type==m_PlayerAwardContent.size()){
					m_PlayerMsg.awardTypeIndicate = -1;
					m_PlayerMsg.awardCD = 0;		//改变剩余CD时间
				}
				else{
					type +=1;
					m_PlayerMsg.awardTypeIndicate = m_PlayerAwardContent[type].awardType;
					m_PlayerMsg.awardCD = m_PlayerAwardContent[type].awardTotalCD;		//改变剩余CD时间
				}
				m_PlayerMsg.initialTime = now;	//设置奖励领取的计时时间为当前时间
				//保存现有的奖励信息到数据库中
				helper_mgr.insertSaveAndUpdate(this);
				update();
			}
		}
		return resultCode;
	}

	int playerOnlineAward::packageActionDoJson(int type,double ratio,Json::Value& msg)
	{
		AwardContentMap::iterator it = m_PlayerAwardContent.find(type);
		if(it==m_PlayerAwardContent.end())
			return 1;
		//判断系数范围
		unsigned base = 0;
		for (;base<it->second.levelLimit.size();base++)
		{
			if(it->second.levelLimit[base]>=own.Base.getLevel())
				break;
		}
		for(unsigned i  = 0;i<it->second.m_Award.size();i++){
			Award& r = it->second.m_Award[i];
			int ID = r.actionID;
			switch(ID){
			case action_add_gold_ticket:	//金票
			case action_add_gold:			//金币
			case action_add_junling:		//补给
				msg[i][ACTION::strActionID] = r.actionID;
				if(base<r.addNum.size())
					msg[i][ACTION::strValue] = (int)r.addNum[base]*(1+ratio);
			case action_add_item:			//物品
				msg[i][ACTION::strActionID] = r.actionID;
				msg[i][ACTION::addItemIDStr] = r.itemID;
				if(base<r.addNum.size())
					msg[i][ACTION::addNumStr] = (int)r.addNum[base]*(1+ratio);
				break;
			case action_active_pilot:		//武将，暂未跟后台
				msg[i][ACTION::strActionID] = r.actionID;
				msg[i][ACTION::strPilotActiveList] = r.itemID;
				//msg[i][ACTION::addNumStr] = r.addNum[base];
				break;
			default:
				msg[i][ACTION::strActionID] = r.actionID;
				if (base < r.addNum.size())
					msg[i][ACTION::strValue] = (int)r.addNum[base] * (1 + ratio);//*own.Base.getLevel();			//改为固定值
				break;
			}
		}
		return 0;
	}

	//更新玩家信息 true:发了邮件 false;没法邮件
	bool playerOnlineAward::updatePlayer(playerDataPtr player, double ratio, Json::Value& fail_value)
	{
		Json::Value msg;
		Json::Value Param;
		if(packageActionDoJson(m_PlayerMsg.awardNum,ratio,msg)==1)
			return false;
		vector<actionResult> vecRetCode = actionFormat::actionDoJump(own.shared_from_this(), msg, Param);
		email_sys.sendDoJumpFailEmail(player, msg, vecRetCode, email_team_system_bag_full_online, fail_value);
		for (unsigned i = 0;i<vecRetCode.size();i++)
		{
			if(vecRetCode[i].resultCode!=0)
				return true;
		}
		return false;

	}


	//重置奖励信息
	void playerOnlineAward::resetAward(const AwardContentMap& contentMap){
		unsigned int now = updateTime = na::time_helper::get_current_time();
		{
			//重新设置当前奖励的领奖记录时间
			int temp;
			if(m_PlayerMsg.initialTime!=m_PlayerMsg.playerLogoutTime)
				temp = m_PlayerMsg.awardCD - (now-m_PlayerMsg.initialTime);
			else
				temp = m_PlayerMsg.awardCD;
			m_PlayerMsg.awardCD = temp > 0 ? temp : 0 ;
			m_PlayerMsg.initialTime = now;
			m_PlayerAwardContent = contentMap;
		}
		helper_mgr.insertSaveAndUpdate(this);
	}


	void playerOnlineAward::playerLogin()
	{
		unsigned now = na::time_helper::get_current_time();
		if(now>=m_PlayerMsg.playerLoginTime)
		{
			//把奖励指示器指示奖励一，累积CD时间设为总累积时间
			m_PlayerMsg.awardTypeIndicate = 1;
			m_PlayerMsg.awardNum = 0;
			m_PlayerMsg.playerContentIndicate = 0;
			//m_PlayerAwardContent = contentMap;
			m_PlayerMsg.awardCD = m_PlayerAwardContent[1].awardTotalCD;
			m_PlayerMsg.playerLoginTime = na::time_helper::get_next_update_time(now);	
		}	
		m_PlayerMsg.initialTime = now;
		helper_mgr.insertSaveAndUpdate(this);
	}


	//玩家退出信息处理
	bool playerOnlineAward::playerLogout(){
		unsigned int now = updateTime = na::time_helper::get_current_time();
		//判断退出时是否过完更新点
		/*if(now>=m_PlayerMsg.playerLogoutTime)
		{
			m_PlayerMsg.playerLogoutTime = na::time_helper::get_next_update_time(now);
		}else{*/
			//计算出累积在线时间
			unsigned int onlineTime = now - m_PlayerMsg.initialTime;
			//比较累积时间是否大于剩余CD时间
			if(onlineTime>=m_PlayerMsg.awardCD)
				m_PlayerMsg.awardCD = 0;			//累积CD时间清零
			else
				m_PlayerMsg.awardCD = m_PlayerMsg.awardCD-onlineTime;        //设置剩余CD时间
			this->save();	
		//}
		return false;
	}

	void playerOnlineAward::autoUpdate(){
		this->update();
	}

	void playerOnlineAward::update()
	{
		playerLogin();

		Json::Value msg;
		this->package(msg);
		player_mgr.sendToPlayer(own.playerID,own.netID,gate_client::online_award_reset_resp,msg);
		//LogW<<"now:"<<updateTime<<"\n"<<"value:" << msg.toStyledString() << LogEnd; 
	}

	bool playerOnlineAward::save(){
		//保存key值
		mongo::BSONObj key = BSON(playerIDStr<<own.playerID);
		mongo::BSONObjBuilder obj;
		obj<<playerIDStr<<own.playerID;
		//保存用户信息
		obj<<player_award_login_time<<m_PlayerMsg.playerLoginTime<<player_award_logout_time<<m_PlayerMsg.playerLogoutTime
			<<player_award_content_indicate<<m_PlayerMsg.playerContentIndicate<<player_award_type_indicate<<m_PlayerMsg.awardTypeIndicate
			<<player_get_award_num<<m_PlayerMsg.awardNum<<player_initial_time<<m_PlayerMsg.initialTime<<player_award_cd_time
			<<m_PlayerMsg.awardCD;
		//写入数据库
		return db_mgr.save_mongo(player_onlineaward_db_name,key,obj.obj());
	}

	bool playerOnlineAward::get(){
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		//从数据库取数据
		mongo::BSONObj obj = db_mgr.FindOne(player_onlineaward_db_name, key);
		if(!obj.isEmpty()){
			//登录时间
			checkNotEoo(obj[player_award_login_time]){
				m_PlayerMsg.playerLoginTime = obj[player_award_login_time].Int();
			}
			//登出时间
			checkNotEoo(obj[player_award_logout_time]){
				m_PlayerMsg.playerLogoutTime = obj[player_award_logout_time].Int();
			}
			//内容指示
			checkNotEoo(obj[player_award_content_indicate]){
				m_PlayerMsg.playerContentIndicate = obj[player_award_content_indicate].Int();
			}
			//奖励类型
			checkNotEoo(obj[player_award_type_indicate]){
				m_PlayerMsg.awardTypeIndicate = obj[player_award_type_indicate].Int();
			}
			//已领奖的数量
			checkNotEoo(obj[player_get_award_num]){
				m_PlayerMsg.awardNum = obj[player_get_award_num].Int();
			}
			//领奖时间
			checkNotEoo(obj[player_initial_time]){
				m_PlayerMsg.initialTime = obj[player_initial_time].Int();
			}
			//CD
			checkNotEoo(obj[player_award_cd_time]){
				m_PlayerMsg.awardCD = obj[player_award_cd_time].Int();
			}
			//读取奖励内容
		}
		
		return false;
	}


	int playerOnlineAward::packageClientJson(int type,Json::Value& msg)
	{
		AwardContentMap::iterator it = m_PlayerAwardContent.find(type);
		if(it==m_PlayerAwardContent.end())
			return 1;
		//判断系数范围
		unsigned base = 0;
		for (;base<it->second.levelLimit.size();base++)
		{
			if(it->second.levelLimit[base]>=own.Base.getLevel())
				break;
		}
		for(unsigned i  = 0;i<it->second.m_Award.size();i++){
			Award& r = it->second.m_Award[i];
			int ID = r.actionID;
			switch(ID){
			case action_add_gold_ticket:	//金票
			case action_add_gold:			//金币
			case action_add_junling:		//补给
				msg[i][email::actionID] = r.actionID;
				if(base<r.addNum.size())
					msg[i][email::value] = r.addNum[base];
				break;
			case action_add_item:			//物品
				msg[i][email::actionID] = r.actionID;
				msg[i][email::itemID] = r.itemID;
				if(base<r.addNum.size())
					msg[i][email::addNum] = r.addNum[base];
				break;
			case action_active_pilot:		//武将，暂未跟后台
				msg[i][email::actionID] = r.actionID;
				msg[i][email::pilotActiveList] = r.itemID;
				//msg[i][ACTION::addNumStr] = r.addNum[base];
				break;
			default:
				msg[i][email::actionID] = r.actionID;
				if (base < r.addNum.size())
					msg[i][email::value] = r.addNum[base];// *own.Base.getLevel();
				break;
			}
		}
		return 0;
	}

	void playerOnlineAward::package(Json::Value &pack){
		unsigned int now = updateTime;
		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();
		pack[msgStr][1u][player_award_type_indicate] = m_PlayerMsg.awardTypeIndicate;	//奖励类型指示
		unsigned int temp = m_PlayerMsg.awardCD;
		if (temp != 0)
			temp += now;
		pack[msgStr][1u][player_award_cd_time] = temp;		//剩余cd时间
		{
			Json::Value& award_json = pack[msgStr][1u][player_award_mongoDB];
			award_json = Json::arrayValue;
			unsigned i = 0;
			for (AwardContentMap::iterator it = m_PlayerAwardContent.begin(); it != m_PlayerAwardContent.end(); ++it,++i)
			{
					award_json[i][player_award_type] = it->second.awardType;	//类型
					//award_json[i][player_award_total_cd] = it->second.awardTotalCD;	//总CD时间
					packageClientJson(it->second.awardType,award_json[i][player_award_content]);
			}
		}
	}

	void playerOnlineAward::packagePlayerMsg(Json::Value &pack)
	{
		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();
		pack[msgStr][1u][player_award_type_indicate] = m_PlayerMsg.awardTypeIndicate;	//奖励类型指示
	}


	
}