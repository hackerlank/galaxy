#include "player_explore.h"
#include "gate_game_protocol.h"
#include "mongoDB.h"
#include "helper.h"
#include "record_system.h"
#include "action_def.h"
#include "email_system.h"
#include "time_helper.h"
#include "action_format.hpp"
#include "action_instance.hpp"
#include "explore_manager.h"


using namespace mongo;
namespace gg{


	player_explore::player_explore(playerData& own) : Block(own)
	{
		//m_OrdinaryMaterialsMax = 50000;
	}


	player_explore::~player_explore(void)
	{
	}

	void player_explore::autoUpdate()
	{
		//this->update();
	}

	bool player_explore::save()
	{
		mongo::BSONObj key = BSON(playerIDStr<<own.playerID);
		mongo::BSONObjBuilder obj;
		obj<<playerIDStr<<own.playerID;
		obj<<ordinary_materials<<m_OrdinaryMaterials;
		obj<<hq_materials<<m_HQMaterials;
		//obj<<ordinary_materials_max<<m_OrdinaryMaterialsMax;
		obj<<get_ordinary_materials<<m_GetOrdinaryMaterials;
		obj<<interplanetary_update_time<<m_UpdateTime;
		return db_mgr.save_mongo(player_explore_db_name,key,obj.obj());
	}

	bool player_explore::get()
	{
		mongo::BSONObj key = BSON(playerIDStr<<own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(player_explore_db_name,key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[ordinary_materials]){
				m_OrdinaryMaterials = obj[ordinary_materials].Int();
			}
			checkNotEoo(obj[hq_materials]){
				m_HQMaterials = obj[hq_materials].Int();
			}
			/*checkNotEoo(obj[ordinary_materials_max]){
			m_OrdinaryMaterialsMax = obj[ordinary_materials_max].Int();
			}*/
			checkNotEoo(obj[get_ordinary_materials]){
				m_GetOrdinaryMaterials = obj[get_ordinary_materials].Int();
			}
			checkNotEoo(obj[interplanetary_update_time]){
				m_UpdateTime = obj[interplanetary_update_time].Int();
			}
			return true;
		}
		return false;
	}
	
	void player_explore::update()
	{
		/*Json::Value noticeMsg;
		noticeMsg[msgStr][0u] = 0;
		noticeMsg[msgStr][1u] = State::getState();
		noticeMsg[msgStr][1u][ordinary_materials] = m_OrdinaryMaterials;
		noticeMsg[msgStr][1u][hq_materials] = m_HQMaterials;
		noticeMsg[msgStr][1u][ordinary_materials_max] = m_OrdinaryMaterialsMax;
		noticeMsg[msgStr][1u][hq_materials_max] = m_HQMaterialsMax;
		player_mgr.sendToPlayer(own.playerID,own.netID,0,noticeMsg);*/
	}

	void player_explore::addPlayerPackage(Json::Value& msg)
	{
		//msg[updateFromStr] = State::getState();
		msg[ordinary_materials] = m_OrdinaryMaterials;
		msg[ordinary_materials_max] = explore_sys.getMaterialsMax();
		msg[hq_materials] = m_HQMaterials;
		msg[get_ordinary_materials] = m_GetOrdinaryMaterials;
		//player_mgr.sendToPlayer(own.playerID,own.netID,gate_client::interplanetary_exploration_update_req,noticeMsg);
	}

	int player_explore::getOrdinaryMaterials()
	{
		return m_OrdinaryMaterials;
	}

	/*int player_explore::getOrdinaryMaterialsMax()
	{
		return m_OrdinaryMaterialsMax;
	}*/



	int player_explore::getHQMaterials()
	{
		return m_HQMaterials;
	}



	int player_explore::getGetOrdinaryMaterials()
	{
		return m_GetOrdinaryMaterials;
	}


	void player_explore::alterOrdinaryMaterials(int num)
	{
		int	previousValue = m_OrdinaryMaterials;
		unsigned now = na::time_helper::get_current_time();
		unsigned t = na::time_helper::get_next_update_time(m_UpdateTime);
		int mMax = explore_sys.getMaterialsMax();
		if(now>t)
			m_GetOrdinaryMaterials = 0;
		m_UpdateTime = now;	
		if(num>0){
			int temp = m_GetOrdinaryMaterials+num;
			int temp1 = m_GetOrdinaryMaterials;
			m_GetOrdinaryMaterials = (temp > mMax? mMax:temp);
			m_OrdinaryMaterials +=(m_GetOrdinaryMaterials-temp1);
		}else{
			int temp = m_OrdinaryMaterials+num;
			m_OrdinaryMaterials = (temp<0?0:temp);
		}
		StreamLog::Log(mysqlLogExplore, own,previousValue, m_OrdinaryMaterials,0);
		this->save();
	}

	void player_explore::alterHQMaterials(int num)
	{
		int	previousValue = m_HQMaterials;
		int temp = m_HQMaterials + num;
		m_HQMaterials = (temp < 0	? 0:temp);
		StreamLog::Log(mysqlLogExplore, own,previousValue, m_HQMaterials,1);
		this->save();
	}

	int player_explore::GetAReward(Json::Value& jv, Json::Value& fail_json)
	{
		Json::Value param;
		vector<actionResult> vecRetCode = actionFormat::actionDoJump(own.shared_from_this(), jv, param);
		email_sys.sendDoJumpFailEmail(own.shared_from_this(), jv, vecRetCode, email_team_system_bag_full_online, fail_json);
		for (unsigned i = 0; i < vecRetCode.size(); i++)
		{
			if (vecRetCode[i].resultCode != 0)
				return 1;
		}
		return 0;
	}



	void player_explore::clearMaterials()
	{
		m_GetOrdinaryMaterials = 0;
		m_OrdinaryMaterials = 0;
		m_HQMaterials = 0;
		m_UpdateTime = na::time_helper::get_current_time();
		this->save();
	}

	unsigned player_explore::getUpdateTime()
	{
		return m_UpdateTime;
	}

}
