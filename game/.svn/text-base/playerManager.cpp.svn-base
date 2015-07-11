#include "playerManager.h"
#include "mongoDB.h"
#include "game_helper.hpp"
#include "json/json.h"
#include <boost/thread/thread.hpp>
#include "game_server.h"
#include "mongoDB.h"
#include "core_helper.h"
#include <boost/bind.hpp>
#include "gate_game_protocol.h"
#include "record_system.h"
#include "war_story.h"
#include "guild_battle_system.h"
#include "starwar_system.h"
#include "refresh_tick.h"
#include "helper.h"
#include "guild_system.h"
#include "timmer.hpp"
#ifndef WIN32
#include "DebugInfo.h"
#endif
namespace gg
{
	playerManager* const playerManager::playerMgr = new playerManager();
	const unsigned CheckHours = 14400;
	const unsigned LogOnlineTimes  = 300;
	const unsigned removeHours = 7200;
	playerManager::playerManager()
	{
	}

	playerManager::~playerManager()
	{

	}

	void playerManager::initPlayerManager()
	{
		ensureIndex();
		m_playerMap.clear();
		m_NameMap.clear();

		Timer::AddEventSeconds(boostBind(playerManager::playerOnlineLog, this), LogOnlineTimes);
		Timer::AddEventSeconds(boostBind(playerManager::playerManagerKick, this), 300);//5钟钟
		Timer::AddEventSeconds(boostBind(playerManager::playerManagerUpdate, this), CheckHours);
	}

	const static string mysqlLogOnlinePlayer = "log_playing";
	void playerManager::playerOnlineLog()
	{
		Timer::AddEventSeconds(boostBind(playerManager::playerOnlineLog, this), LogOnlineTimes);
		unsigned now = na::time_helper::get_current_time();
		Json::Value seasonJson = season_sys.serverOpenPackage();
		unsigned num = 0;
		for (BHIDMAP::iterator it = m_playerMap.begin(); it != m_playerMap.end(); ++it)
		{
			playerDataPtr player = it->second;
			if( player == playerDataPtr() )continue;
			if (player->netID >= 0)
			{
				++num;
// 				if (now > player->tickServerTime)
// 				{
// 					player->sendToClient(gate_client::server_info_resp, seasonJson);
// 					player->tickServerTime = now + 240;
// 				}
			}
		}
		StreamLog::Log(mysqlLogOnlinePlayer, -1, "system", -1, "online", num);
	}

	void playerManager::playerManagerKick()
	{
		Timer::AddEventSeconds(boostBind(playerManager::playerManagerKick, this), 300);//5钟钟

		if(m_playerMap.size() < 10000)return;
		playerManagerClearRB();
	}

	void playerManager::showPlayerManagerInfo()
	{
		unsigned total = m_playerMap.size();
		unsigned online = 0;
		for (BHIDMAP::iterator it = m_playerMap.begin(); it != m_playerMap.end(); ++it)
		{
			playerDataPtr player = it->second;
			if(player == NULL)continue;
			if(player->netID < 0)continue;
			++online;
		}
		LogS << "Player Manager Infomation ..." << LogEnd;
		LogS << online  << color_pink(" player online ~") << LogEnd;
		LogS << total - online << color_pink(" player offline ~") << LogEnd;
		LogS << total << color_pink(" player total ~") << LogEnd;
	}

	void playerManager::gateResetReq()
	{
		for (BHIDMAP::iterator it = m_playerMap.begin(); it != m_playerMap.end(); ++it)
		{
			playerDataPtr player = it->second;
			if(player == NULL)continue;
			playerOffline(player);
		}
	}

	void playerManager::playerManagerClearRB()
	{
		int num = 0;
		for (BHIDMAP::iterator it = m_playerMap.begin(); it != m_playerMap.end();)
		{
			playerDataPtr player = it->second;
			BHIDMAP::iterator cPlayerIt = it;
			++it;
			if( player == playerDataPtr() || (player->netID < 0 && !player->isBattleState()) )
			{
				++num;
				m_playerMap.erase(cPlayerIt);
				if(player == NULL)continue;
				m_NameMap.erase(player->Base.getName());
				if (!player.unique())
				{
					LogW << "player" << player->playerID << " is not unique ..." << LogEnd;
				}
			}
		}
#ifdef DEBUG_INFO
		LogS << num  << color_pink(" player memory cache has been free ~") << LogEnd;
#endif
	}

	void playerManager::playerManagerUpdate()
	{
		Timer::AddEventSeconds(boostBind(playerManager::playerManagerUpdate, this), CheckHours);

		unsigned now = na::time_helper::get_current_time();
		int num = 0;
		for (BHIDMAP::iterator it = m_playerMap.begin(); it != m_playerMap.end();)
		{
			playerDataPtr player = it->second;
			BHIDMAP::iterator cPlayerIt = it;
			++it;
			if( player == playerDataPtr() || 
				(player->netID < 0 && !player->isBattleState() && now > player->getOffineTime() && now - player->getOffineTime()> removeHours))
			{
				++num;
				m_playerMap.erase(cPlayerIt);
				if(player == NULL)continue;
				m_NameMap.erase(player->Base.getName());
				if (!player.unique())
				{
					LogW << "player" << player->playerID << " is not unique ..." << LogEnd;
				}
			}
		}
#ifdef DEBUG_INFO
		LogS << num  << color_pink(" player memory cache has been free ~") << LogEnd;
#endif
	}

	bool playerManager::hasRegisterName(const string name)
	{
		return findPlayerIDByName(name) > 0;
	}

	int playerManager::findPlayerIDByName(const string name)
	{
		int playerID = -1;
		mongo::BSONObj key = BSON(playerNameStr << name);
		mongo::BSONObj obj = db_mgr.FindOne(playerBaseDBStr, key);
		if(!obj.isEmpty())	{
			checkNotEoo(obj[playerIDStr]){playerID = obj[playerIDStr].Int();}
		}
		return playerID;
	}

	void playerManager::removeOldRelate(const string oldName)
	{
		m_NameMap.erase(oldName);
	}

	void playerManager::asyncPlayerData(playerDataPtr player)
	{
		if(player == playerDataPtr() || !player->isVaild())return;
		m_NameMap[player->Base.getName()] = player;
	}

	void playerManager::asyncPlayerData(const int playerID)
	{
		playerDataPtr ptr = getPlayer(playerID);
		asyncPlayerData(ptr);
	}

	const static string mysqlLogOfflinePlayer = "log_online";
	void playerManager::playerOffline(const int playerID)
	{
		playerDataPtr player = getPlayer(playerID);
		playerOffline(player);
	}

	void playerManager::playerOffline(playerDataPtr player)
	{
		if(player == NULL)return;
		player->netID = -1;
		player->offlineTime = na::time_helper::get_current_time();
		player->firstLogin = false;
		unsigned now = na::time_helper::get_current_time() - na::time_helper::timeZone() * 3600;
		StreamLog::Log(mysqlLogOfflinePlayer, player, (int)(player->getLoginTime() - na::time_helper::timeZone() * 3600), (int)now);
		//用户退出，在线奖励处理
		player->onlineAward.playerLogout();
		//event_sys.deletePlayerEventData(player->playerID);
		war_story.playerLogout(player->playerID);
		guild_battle_sys.playerLogout(player->playerID);
		player->Email.playerLogout();
		player->Ally.playerLogout();
		player->Sign.playerLogout();
		starwar_sys.kickPlayer(player, true);
	}

	playerDataPtr playerManager::getOnlinePlayer(const int player_id, const int net_id /* = -1 */, const bool alive /* = true */)
	{
		playerDataPtr player = getPlayer(player_id);
		if(player != playerDataPtr() && net_id >= 0)player->netID = net_id;
		if(player != playerDataPtr() && player->netID < 0)return playerDataPtr();
		if(alive && player != playerDataPtr() && !player->isVaild())return playerDataPtr();
		return player;
	}

	playerDataPtr playerManager::getOnlinePlayer(const string player_name, const int net_id /* = -1 */, const bool alive /* = true */)
	{
		playerDataPtr player = getPlayer(player_name);
		if(player != playerDataPtr() && net_id >= 0)player->netID = net_id;
		if(player != playerDataPtr() && player->netID < 0)return playerDataPtr();
		if(alive && player != playerDataPtr() && !player->isVaild())return playerDataPtr();
		return player;
	}

	playerDataPtr playerManager::getPlayer(const int player_id)
	{
		BHIDMAP::iterator it = m_playerMap.find(player_id);
		if(it != m_playerMap.end())return it->second;
		return playerDataPtr();
	}

	playerDataPtr playerManager::getPlayer(const string name)
	{
		BHNMAP::iterator it = m_NameMap.find(name);
		if(it != m_NameMap.end())return it->second;
		return playerDataPtr();
	}

	playerManager::playerDataVec playerManager::onlineSpherePlayer()
	{
		playerDataVec vec;
		for (BHIDMAP::iterator it = m_playerMap.begin(); it != m_playerMap.end(); ++it)
		{
			if (it->second->netID >= 0 && it->second->Base.SphereVaild())vec.push_back(it->second);
		}
		return vec;
	}

	playerManager::playerDataVec playerManager::onlinePlayer()
	{
		playerDataVec vec;
		for (BHIDMAP::iterator it = m_playerMap.begin(); it != m_playerMap.end(); ++it)
		{
			if(it->second->netID >= 0)vec.push_back(it->second);
		}
		return vec;
	}

	playerManager::playerDataVec playerManager::onlinePlayerSphereID(const int sphereID)
	{
		playerDataVec vec;
		if(sphereID < 0 || sphereID > 2)return vec;
		for (BHIDMAP::iterator it = m_playerMap.begin(); it != m_playerMap.end(); ++it)
		{
			playerDataPtr player = it->second;
			if(player == NULL)continue;
			if(player->Base.getSphereID() == sphereID && player->netID >= 0)
				vec.push_back(player);
		}
		return vec;
	}

	playerManager::playerDataVec playerManager::onlinePlayerAreaID(const int areaID)
	{
		playerDataVec vec;
		for (BHIDMAP::iterator it = m_playerMap.begin(); it != m_playerMap.end(); ++it)
		{
			playerDataPtr player = it->second;
			if(player == NULL)continue;
			if(player->Base.getPosition().first == areaID && player->netID >= 0)
				vec.push_back(player);
		}
		return vec;
	}

	//getPlayerDataAndPostMessage
	playerDataPtr playerManager::getPlayerMain(const int player_id, const int net_id /* = -1 */, const bool alive /* = true */)
	{
		playerDataPtr player = getPlayer(player_id);
		if(player != playerDataPtr() && net_id >= 0 && player->netID != net_id){player->netID = net_id;}
		if(alive && player != playerDataPtr() && !player->isVaild())return playerDataPtr();
		if(player == playerDataPtr())asyncLoadData(player_id, net_id);
		return player;
	}

	playerDataPtr playerManager::getPlayerMainByName(const string playerName)
	{
		playerDataPtr player = getPlayer(playerName);
		if(player == playerDataPtr()){
			int playerID = findPlayerIDByName(playerName);
			player = getPlayer(playerID);
			if(player == playerDataPtr())asyncLoadData(playerID, -1);
			else asyncPlayerData(player);
		}
		return player;
	}

	void playerManager::postMessage(na::msg::msg_json& mj)
	{
		msg_json::ptr msg_ptr = msg_json::create(mj);
		_Db_Post(boost::bind(&playerManager::asyncRecallMsg, playerMgr, msg_ptr));
	}

	void playerManager::asyncLoadData(const int player_id, const int net_id)
	{
		if( player_id <= 0 || hasPostLoad(player_id) || getPlayer(player_id) != NULL )return;
		playerDataPtr p = playerData::CreatplayerData(player_id, net_id);
		if(p == playerDataPtr())return;
		m_PostList.insert(player_id);
		_Db_Post(boost::bind(&playerManager::postToLoadData, playerMgr, p));
	}

	void playerManager::postToLoadData(playerDataPtr ptr)
	{
// 		if(! ptr->load()){
// 			Json::Value updateJson;
// 			updateJson[msgStr][0u] = 1;//没有角色
// 			string updateString = updateJson.toStyledString();
// 			msg_json mj(ptr->playerID, ptr->netID, gate_client::player_login_resp, updateString);
// 			game_svr->async_send_to_gate(mj);
// 		}
		ptr->load();
		_Logic_Post(boost::bind(&playerManager::addPlayerCallback, playerMgr, ptr));	
	}

	void playerManager::addPlayerCallback(playerDataPtr ptr)
	{
		m_PostList.erase(ptr->playerID);
		BHIDMAP::iterator it = m_playerMap.find(ptr->playerID);
		if(it == m_playerMap.end()) m_playerMap[ptr->playerID] = ptr;
		ptr->load_end();
		if(!ptr->isVaild())return;//不保存任何数据
		asyncPlayerData(ptr);
	}

	//recall message
	void playerManager::asyncRecallMsg(msg_json::ptr msg_ptr)
	{
		_Logic_Post(boost::bind(&playerManager::asyncCallGameHandler, playerMgr, msg_ptr));
	}

	void playerManager::asyncCallGameHandler(msg_json::ptr msg_ptr)
	{
		game_svr->internalCallback(msg_ptr);
	}

	bool playerManager::IsOnline(const int player_id)
	{
		BHIDMAP::iterator it = m_playerMap.find(player_id);
		if(it != m_playerMap.end() && it->second->netID >= 0)
			return true;

		return false;
	}

	void playerManager::sendToSphere(const short pcl, Json::Value& m, const int spID)
	{
		playerDataVec vec = onlinePlayerSphereID(spID);
		MsgSignOnline(vec, m, pcl);
	}

	void playerManager::sendToGuild(const short pcl, Json::Value& m, const int gID)
	{
		playerDataVec vec = guild_sys.getOnlineMember(gID);
		MsgSignOnline(vec, m, pcl);
	}

	void playerManager::sendToArea(const short pcl, Json::Value& m, const int aID)
	{
		playerDataVec vec = onlinePlayerAreaID(aID);
		MsgSignOnline(vec, m, pcl);
	}

	void playerManager::sendToAll(const short pcl, Json::Value& m)
	{
		playerDataVec vec = onlinePlayer();
		MsgSignOnline(vec, m, pcl);
	}

	void playerManager::sendToPlayer(const int player_id, const short pcl, Json::Value& m)
	{
		playerDataPtr d = getOnlinePlayer(player_id);
		if(d == playerDataPtr() || d->netID < 0)return;
		sendToPlayer(d->playerID, d->netID, pcl, m);
	}

	void playerManager::sendToPlayer(const int player_id, const int net_id, const short pcl, Json::Value& m)
	{
		if(net_id < 0) return;
		string str = m.toStyledString();
		//LogW<<"Test display"<<str<<LogEnd;
		na::msg::msg_json mj(pcl, str);
		sendToPlayer(player_id, net_id, mj);
	}

	void playerManager::sendToPlayer(const int player_id, const int net_id, na::msg::msg_json& mj)
	{
		if(net_id < 0) return;
		mj._player_id = player_id;
		mj._net_id = net_id;
		sendToPlayer(mj);
	}

	void playerManager::sendToPlayer(na::msg::msg_json& mj)
	{
		if(mj._player_id > 0 && mj._net_id >= 0)
			game_svr->async_send_to_gate(mj);	
	}

	void playerManager::ensureIndex()
	{
		db_mgr.ensure_index(playerBaseDBStr, BSON(playerIDStr << 1));
		db_mgr.ensure_index(playerBaseDBStr, BSON(playerNameStr << 1));
		db_mgr.ensure_index(TICK::TickMongoDBStr, BSON(playerIDStr << 1 << TICK::strTickKey << 1));
	}

	bool playerManager::hasPostLoad(const int player_id)
	{
		BHIDSET::const_iterator it = m_PostList.find(player_id);
		if(it != m_PostList.end())
			return true;
		return false;
	}
}