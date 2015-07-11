#pragma once

#include "player_data.h"
#include <boost/unordered_map.hpp>
#include <vector>
#include <boost/thread.hpp>
#include "msg_base.h"
#include "gate_game_protocol.h"
#include <boost/unordered_set.hpp>
#include <iostream>
using namespace std;

#define player_mgr (*gg::playerManager::playerMgr)

namespace gg
{
	class playerManager
	{
	private:
		void playerManagerKick();
		void playerManagerUpdate();
		void playerOnlineLog();
	public:
		static playerManager* const playerMgr;
		typedef boost::unordered_map<int, playerDataPtr> BHIDMAP;
		typedef boost::unordered_map<string, playerDataPtr> BHNMAP;
		typedef boost::unordered_set<int> BHIDSET;
		typedef vector<playerDataPtr> playerDataVec;
		playerManager();
		~playerManager();
		void initPlayerManager();
		void playerManagerClearRB();
		void showPlayerManagerInfo();
		void gateResetReq();
	public:
		playerDataPtr getOnlinePlayer(const int player_id, const int net_id = -1, const bool alive = true);
		playerDataPtr getOnlinePlayer(const string player_name, const int net_id = -1, const bool alive = true);
		bool IsOnline(const int player_id);
		//mj's player_id and ner_id must be a valid value		
		playerDataPtr getPlayerMain(const int player_id, const int net_id = -1, const bool alive = true);
		playerDataPtr getPlayerMainByName(const string playerName);
		void postMessage(na::msg::msg_json& mj);
		bool hasRegisterName(const string name);
		void asyncPlayerData(const int playerID);
		void asyncPlayerData(playerDataPtr player);
		void removeOldRelate(const string oldName);
		
		void playerOffline(const int playerID);
		void playerOffline(playerDataPtr player);
		void sendToPlayer(na::msg::msg_json& mj);
		void sendToPlayer(const int player_id, const short pcl, Json::Value& m);		
		void sendToPlayer(const int player_id, const int net_id, const short pcl, Json::Value& m);
		void sendToAll(const short pcl, Json::Value& m);
		void sendToSphere(const short pcl, Json::Value& m, const int spID);
		void sendToArea(const short pcl, Json::Value& m, const int aID);
		void sendToGuild(const short pcl, Json::Value& m, const int gID);
		void sendToPlayer(const int player_id, const int net_id, na::msg::msg_json& mj);

		playerDataVec onlinePlayer();
		playerDataVec onlineSpherePlayer();
		playerDataVec onlinePlayerSphereID(const int sphereID);
		playerDataVec onlinePlayerAreaID(const int areaID);
	protected:		
		int findPlayerIDByName(const string name);
		playerDataPtr getPlayer(const int player_id);
		playerDataPtr getPlayer(const string name);
		//async load player data
		void asyncLoadData(const int player_id, const int net_id);
		void postToLoadData(playerDataPtr ptr);
		void addPlayerCallback(playerDataPtr ptr);
		bool hasPostLoad(const int player_id);

		//recall player msg
		void asyncCallGameHandler(msg_json::ptr msg_ptr);
		void asyncRecallMsg(msg_json::ptr msg_ptr);
		
		void ensureIndex();

		BHIDSET m_PostList;
		BHIDMAP m_playerMap;
		BHNMAP m_NameMap;
	};

	
}