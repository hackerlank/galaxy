#include "player_crystal_exchange.h"
#include "email_system.h"
#include "mongoDB.h"
#include "helper.h"
#include <vector>
#include "action_instance.hpp"
#include "action_format.hpp"
namespace gg{

	ExItemsMap player_crystal_exchange::exItemsMsg;
	player_crystal_exchange::player_crystal_exchange(playerData &own):Block(own)
	{
		for (ExItemsMap::iterator it = exItemsMsg.begin(); it != exItemsMsg.end(); it++)
		{
			PlayerExItemsMsg itMsg;
			itMsg.ID = it->second.ID;
			itMsg.remainingNum = it->second.maxNum;
			itMsg.maxNum = it->second.maxNum;
			//itMsg.exItemsJson = it->second.exItemsJson;
			m_PlayerExItemMsg[itMsg.ID] = itMsg;
		}

	}


	player_crystal_exchange::~player_crystal_exchange()
	{
	}

	void player_crystal_exchange::autoUpdate()
	{
		update();
	}

	bool player_crystal_exchange::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID;
		obj << last_update_time_str << m_LastUpdateTime;
		mongo::BSONArrayBuilder arry;
		for (PlayerExItemMap::iterator it = m_PlayerExItemMsg.begin(); it != m_PlayerExItemMsg.end(); it++)
		{
			mongo::BSONObjBuilder o;
			o << exchange_items_id_str << it->second.ID << remaining_num_str <<
				it->second.remainingNum << exchange_max_num_str << it->second.maxNum;
			arry.append(o.obj());
		}
		obj << player_exchange_items_msg_str << arry.arr();
		return db_mgr.save_mongo(player_crystal_exchange_db_name, key, obj.obj());
	}
	using namespace mongo;
	bool player_crystal_exchange::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(player_crystal_exchange_db_name, key);
		if (!obj.isEmpty()){

			checkNotEoo(obj[last_update_time_str]){
				m_LastUpdateTime = obj[last_update_time_str].Int();
			}
			checkNotEoo(obj[player_exchange_items_msg_str]){
				m_PlayerExItemMsg.clear();
				vector<BSONElement> els = obj[player_exchange_items_msg_str].Array();
				for (unsigned i = 0; i < els.size(); ++i)
				{
					PlayerExItemsMsg itMsg;
					itMsg.ID = els[i][exchange_items_id_str].Int();
					itMsg.remainingNum = els[i][remaining_num_str].Int();
					itMsg.maxNum = els[i][exchange_max_num_str].Int();
					m_PlayerExItemMsg[itMsg.ID] = itMsg;
				}
			}
			return true;
		}
		return false;
	}

	void player_crystal_exchange::update()
	{
		Json::Value msg;
		//resetData();
		package(msg);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::update_crystal_exchange_resp, msg);
	}

	int player_crystal_exchange::exchageItems(int ID, Json::Value itemsJson)
	{
		PlayerExItemMap::iterator it = m_PlayerExItemMsg.find(ID);
		if (it == m_PlayerExItemMsg.end())
			return -1;
		if (it->second.remainingNum <= 0)
			return -2;
		it->second.remainingNum--;
		m_LastUpdateTime = na::time_helper::get_next_update_time(na::time_helper::get_current_time());
		Json::Value param;
		vector<actionResult> vecRetCode = actionFormat::actionDoJump(own.shared_from_this(), itemsJson, param);

		Json::Value failSimpleJson;
		email_sys.sendDoJumpFailEmail(own.shared_from_this(), itemsJson, vecRetCode, email_team_system_bag_full_online, failSimpleJson);
		for (unsigned i = 0; i < vecRetCode.size(); i++)
		{
			if (vecRetCode[i].resultCode != 0)
				return 1;
		}
		save();
		return 0;
	}

	int player_crystal_exchange::getRemainingNum(int itemsID)
	{
		return m_PlayerExItemMsg[itemsID].remainingNum;
	}

	const ExItemsMsg& player_crystal_exchange::getExItemsMsg(int exItemsID)
	{
			ExItemsMap::iterator it = exItemsMsg.find(exItemsID);
			if (it == exItemsMsg.end())
				return nullExItem;
			return (it->second);
	}

	void player_crystal_exchange::setExItemsMsg(ExItemsMap& ex)
	{
		exItemsMsg = ex;
	}


	void player_crystal_exchange::package(Json::Value& msg)
	{
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		Json::Value& award_json = msg[msgStr][1u][player_exchange_items_msg_str];
		unsigned i = 0;
		for (PlayerExItemMap::iterator it = m_PlayerExItemMsg.begin(); it != m_PlayerExItemMsg.end(); it++, ++i)
		{
			award_json[i][exchange_items_id_str] = it->second.ID;
			award_json[i][remaining_num_str] = it->second.remainingNum;
			award_json[i][exchange_max_num_str] = it->second.maxNum;
		}
	}

	void player_crystal_exchange::resetData()
	{
		unsigned now = na::time_helper::get_current_time();
		if (now >= m_LastUpdateTime){
			for (ExItemsMap::iterator it = exItemsMsg.begin(); it != exItemsMsg.end(); it++)
			{
				PlayerExItemsMsg itMsg;
				itMsg.ID = it->second.ID;
				itMsg.remainingNum = it->second.maxNum;
				itMsg.maxNum = it->second.maxNum;
				//itMsg.exItemsJson = it->second.exItemsJson;
				m_PlayerExItemMsg[itMsg.ID] = itMsg;
			}
		}
	}

}
