#include "OnlineAwardSys.h"
#include "response_def.h"
#include "mongoDB.h"
#include "helper.h"
#include "file_system.h"
#include "activity_system.h"
namespace gg {
	const static string onlineAwardDataDirStr = "./instance/onlineAward/";
	OnlineAwardSys* const OnlineAwardSys::onlineAwardMgr = new OnlineAwardSys();

	//const static string player_award_content_db_name ="gl.player_award_one";	//奖励内容数据库名
	//const static string player_award_content_id = "id";	//标识id
	//const static string player_award_key[] = {"db1","db2"};

	void OnlineAwardSys::initOnlineAward(){
		m_Ratio = 1;
		//m_DBIndicate = 0;
		db_mgr.ensure_index(player_onlineaward_db_name, BSON(playerIDStr << 1));
		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(onlineAwardDataDirStr,jv);
		m_AwardContent.clear();
		if(jv.size()!=0){
			//更新数据
			for (unsigned int i=0;i<jv.size();i++)
			{
				AwardContent award;
				award.awardType = jv[i][player_award_type].asInt();
				award.awardTotalCD = jv[i][player_award_total_cd].asInt();
				for (unsigned k = 0;k<jv[i][player_award_content].size();k++)
				{
					Award r;
					Json::Value& content = jv[i][player_award_content][k];
					if(content.isMember("actionID"))
						r.actionID = content["actionID"].asInt();
					if(content.isMember("itemID"))
						r.itemID = content["itemID"].asInt();
					if(content.isMember("addNum")){
						if(content["addNum"].isArray())
						{
							for(unsigned n = 0;n<content["addNum"].size();n++)
								r.addNum.push_back(content["addNum"][n].asInt());
						}
					}
					if(content.isMember("value")){
						if(content["value"].isArray()){
							for(unsigned n = 0;n<content["value"].size();n++)
								r.addNum.push_back(content["value"][n].asInt());
						}
					}
					award.m_Award.push_back(r);
				}

				for (unsigned k = 0 ;k < jv[i][player_award_limit_level].size();k++)
					award.levelLimit.push_back(jv[i][player_award_limit_level][k].asInt());
				m_AwardContent[award.awardType]=award;
			}
		}
	}


	OnlineAwardSys::OnlineAwardSys(void)
	{
		//this->initOnlineAward();
	}


	OnlineAwardSys::~OnlineAwardSys(void)
	{

	}

	//获取奖励
	void OnlineAwardSys::GetAward(msg_json& m, Json::Value& r){
		AsyncGetPlayerData(m);
		ReadJsonArray
		int typeIndicate = js_msg[0u].asInt();
		int resultCode = 0;
		m_Ratio = activity_sys.getRate(activity::_tian_dao_chou_qin, d);
		Json::Value fail_value;
		resultCode = d->onlineAward.getAward(d, typeIndicate, m_Ratio, fail_value);
		if(resultCode!=1){
			//更新客户端用户信息
			d->onlineAward.packagePlayerMsg(r);
			if (resultCode == 2)
				r[msgStr][2u] = fail_value;
			//记录领奖事件
			activity_sys.updateActivity(d, 0, activity::_online_reward);
			Return(r,resultCode);
		}
		Return(r,resultCode);
	}
	//重置奖励信息
	void OnlineAwardSys::ResetAwardMsg(msg_json& m,Json::Value& r){
		AsyncGetPlayerData(m);
		//action_show_sys.updateActionMsg(m,r);
		//d->onlineAward.setDBIndicate(m_DBIndicate);
		//重置奖励信息
		d->onlineAward.resetAward(m_AwardContent);
		//Return(r, 1);
	}
}