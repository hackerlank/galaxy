#pragma once

#include <boost/unordered_map.hpp>
#include <map>
#include <string>
#include "msg_base.h"
#include "playerManager.h"
#include "json/json.h"
using namespace na::msg;
using namespace std;

#define world_sys (*gg::world::worldSys)

namespace gg
{
	struct playerAreaData
	{
		playerAreaData(){
			playerID = -1;
			planetID = -1;
			playerLv = 1;
			playerName = "";
			flagName = "";
			leaveMessage = "";
			hostileValue[0] = 0;
			hostileValue[1] = 0;
			hostileValue[2] = 0;
			protectCD = 0;
			failedToday = 0;
			battleToday = 0;
			BeHeatToday = 0;
			faceID = -1;
			population = 0;
		}
		bool operator ==(const playerAreaData& area) const{
			return (playerID == area.playerID && planetID == area.planetID && playerName == area.playerName);
		}
		bool operator !=(const playerAreaData& area) const{
			return !(*this == area);
		}
		void clearOldData()
		{
			failedToday = 0;
			battleToday = 0;
			BeHeatToday = 0;
		}
		int population;
		int playerID;
		string playerName;
		int playerLv;
		int faceID;
		int planetID;
		string flagName;
		string leaveMessage;
		int hostileValue[3];
		unsigned protectCD;
		int failedToday;
		int battleToday;
		int BeHeatToday;
		const static playerAreaData NullPlayerAreaData;
	};

	struct areaData
	{
		const static int singlePageSize = 10;
		areaData(){
			areaID = -1;
			areaPage = -1;
			areaVec.clear();
			areaVec.resize(singlePageSize);
			freePos = singlePageSize;
			nextUpdateTime = 0;
		}
		bool operator ==(const areaData& aD)const{return areaID == aD.areaID && areaPage == aD.areaPage;}
		Json::Value package()const;
		int areaID;
		int areaPage;
		int freePos;
		unsigned nextUpdateTime;
		typedef vector<playerAreaData> areaVector;
		areaVector areaVec;
		const static areaData NULLAreaData;
	};
	
	struct areaConfig
	{
		const static areaConfig NullConfig;
		areaConfig(){
			memset(this, 0x0, sizeof(areaConfig));
			areaID = -1;
		}
		bool operator==(const areaConfig& config)const{
			return (findIndex == config.findIndex && areaID == config.areaID &&
				minLevelLimit == config.minLevelLimit && subjectCamp == config.subjectCamp);
		}
		int findIndex;
		int areaID;
		int minLevelLimit;//等级限制
		int subjectCamp;//所属阵营
	};

	struct areaKey
	{
		areaKey() : areaID(-1), areaPage(-1){};
		areaKey(const int ID, const int Page) : areaID(ID), areaPage(Page){};
		areaKey(std::pair<int, int> position) : areaID(position.first),
			areaPage(position.second){}
		int areaID;
		int areaPage;
		bool operator< (const areaKey& k) const{
			if(areaID != k.areaID)return areaID < k.areaID;
			return areaPage < k.areaPage;
		}
	};

	class world
	{
	public:
		static world* const worldSys;
		
		world();
		void initData();
		void updateMapPage(msg_json& m, Json::Value& r);
		void updateClient(msg_json& m, Json::Value& r);
		void changePlanet(msg_json& m, Json::Value& r);
		void motifyFlagName(msg_json& m, Json::Value& r);
		void motifyLeaveMessage(msg_json& m, Json::Value& r);
//		void investedMyself(msg_json& m, Json::Value& r);
		void battleAreaPlayer(msg_json& m, Json::Value& r);

		const areaConfig& getConfig(const int areaID);
		void checkMoving(playerDataPtr player);
		void wPVPDeal(playerDataPtr player, playerDataPtr target, const int battleResult);
		void asyncPlayerData(playerDataPtr player);
		int getPlayerPlanetID(playerDataPtr player);
	private:
		int movingAreaSystem(playerDataPtr player, const int areaID);
		typedef pair<areaData*, playerAreaData*> PADPair;
		PADPair getPlayerAreaData(playerDataPtr player);
		areaConfig& getConfigPrivate(const int areaID);
		areaData& getAreaData(const areaKey& key);
		void insertNewAddress(playerDataPtr player, areaConfig& config, playerAreaData& pA);
		void removeOldAddress(playerDataPtr player, areaConfig& config, playerAreaData& pA);
		bool saveSingleArea(areaData& aD);
		void loadAllAreaData();		
		typedef boost::unordered_map<int, areaConfig> areaConfigMap;
		typedef boost::unordered_map<int, int> areaTotalPage;
		typedef map<areaKey, areaData> areaDataMap;
		areaConfigMap areaConfigs;
		areaDataMap areaDatas;
		areaTotalPage areaPages;
		vector<int> Vector;
		Params::ArrayValue PvPPopulation;
	};
}