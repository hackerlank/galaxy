#pragma once 

#include <vector>
#include "pilotManager.h"
#include <boost/unordered_map.hpp>
#include <vector>
#include "playerManager.h"
#include "msg_base.h"
#include "action_format.hpp"

using namespace na::msg;
using namespace std;

#define war_story  (*gg::warStory::warSys)

namespace gg
{
	namespace WAR{
		const static string warStoryDataDirStr = "./instance/warMap/";
		const static string teamWarMapDirStr = "./instance/corps/team/tm_";
		const static string teamNpcDataDirStr = "./instance/corps/troop/tp_";
		const static string teamRobotFileStr = "./instance/corps/robot/robot.json";
// 		const static string teamRobotBoysNameStr = "./instance/name/Englishboy.json";
// 		const static string teamRobotGirlsNameStr = "./instance/name/Englishgirl.json";
// 		const static string teamRobotFamilyNameStr = "./instance/name/familyname.json";

		const static string teamNpcMaxWinNumStr = "continuousWinMaxNum";
		const static string teamNpcMaxTeamMemberStr = "teamMemberMaxNum";
		const static string teamNpcArmyNumStr = "armyNum";
		const static string teamNpcStepStr = "battleListSize";

		const static string warStoryMapIDStr = "mapId";
		const static string warStoryEndMapStr = "completeKeyArmyId";
		const static string warStroyMapDataStr = "MapData";
		const static string warStoryLocalIDStr = "localID";
		const static string warStoryNameStr = "name";
		const static string warStoryLevelStr = "lev";
		const static string warStoryValueStr = "battleValue";
		const static string warStoryFaceIDStr = "faceID";
		const static string warStoryFrontIDStr = "frontID";
		const static string warStoryPlayerExpStr = "playerExp";
		const static string warStoryPilotExpStr = "pilotExp";
		const static string warStoryArmyDataStr = "armyData";
		const static string warStoryRewardStr = "reward";

		const static string warStoryBackGroundIDStr = "bgId";
		const static string warStoryLeaderGeneralRawIdStr = "leaderGeneralRawId";
		const static string warStoryRewardJunGongStr = "rewardJunGong";
		const static string warStoryMapTypeStr = "type";
		const static string warStoryDropItemIdStr = "dropItemId";
		const static string warStoryDropRateStr = "dropRate";

		const static string warStoryArmyRawIDStr = "rawID";
		const static string warStoryArmyCoorStr = "coorIndex";
		const static string warStoryArmyArmsLevelStr = "Level";// "armsLevel";
		const static string warStoryArmyHPStr = "HP";

		const static string warStoryTeamJoinCondition = "jc";
		const static string warStoryTeamMemberNum = "mn";
		const static string warStoryTeamLevelLimit = "jlv";

		const static string pilotPuGongStr = "PuGong";
		const static string pilotPuFangStr = "PuFang";
		const static string pilotZhanGongStr = "ZhanGong";
		const static string pilotZhanFangStr = "ZhanFang";
		const static string pilotMoGongStr = "MoGong";
		const static string pilotMoFangStr = "MoFang";
		const static string pilotShanBiStr = "ShanBi";
		const static string pilotGeDangStr = "GeDang";
		const static string pilotBaoJiStr = "BaoJi";
		const static string pilotFanJiStr = "FanJi";
		const static string pilotWuLiZengShangStr = "WuLiZengShang";
		const static string pilotWuLiJianShangStr = "WuLiJianShang";
		const static string pilotZhanFaZengShangStr = "ZhanFaZengShang";
		const static string pilotZhanFaJianShangStr = "ZhanFaJianShang";
		const static string pilotMoFaZengShangStr = "MoFaZengShang";
		const static string pilotMoFaJianShangStr = "MoFaJianShang";
		const static string pilotZhiLiaoXiShuStr = "ZhiLiaoXiShu";
		const static string pilotBeiZhiLiaoXiShuStr = "BeiZhiLiaoXiShu";
		const static string pilotEWaiTongStr = "EWaiTong";
		const static string pilotEWaiYongStr = "EWaiYong";
		const static string pilotEWaiZhiStr = "EWaiZhi";

		enum
		{
			state_null,
			state_bonus
		};
	}

	struct npcData
	{
		npcData(){
			memset(this, 0x0, sizeof(npcData));
		}
		int rawID;
		int armyIndex;
		int armsLevel;
		int Hp;
		int initialAttribute[attributeNum];
		double initialCharacter[characterNum];
	};

	typedef vector<npcData> npcDataList;
	struct mapData{
		mapData(){
			faceID = -1;
			localID = -1;
			frontLocalID = -1;
			mapName = "";
			playerExp = 0;
			pilotExp = 0;
			mapLevel = 1;
			mapValue = 999;
			backGround = -1;
			mapType = 0;
			sphereID = -1;
			firstBlood = false;
			actionVec.clear();
		}
		~mapData(){}
		int localID;
		int frontLocalID;
		int faceID;
		int sphereID;
		string mapName;
		int mapLevel;
		int mapValue;
		int playerExp;
		int pilotExp;
		int backGround;
		bool firstBlood;
		int mapType;
		npcDataList npcArmy;
		actionVector actionVec;
	};

	typedef boost::shared_ptr<mapData> mapDataPtr;
	struct teamMapData{
		teamMapData(){
			localID = -1;
			faceID = -1;
			frontMapID = -1;
			mapName = "";
			playerExp = 0;
			pilotExp = 0;
			mapLevel = 1;
			mapValue = 999;
			actionVec.clear();
			battleSize = 3;
		}
		~teamMapData(){}
		int teamMax;
		int teamWinMax;
		int teamAmryNum;
		unsigned battleSize;
		int localID;
		int faceID;
		int frontMapID;
		string mapName;
		int mapLevel;
		int mapValue;
		int playerExp;
		int pilotExp;
		vector<mapDataPtr> npcArmy;
		actionVector actionVec;
	};

	struct npcHelpConfig 
	{
		npcHelpConfig(){};
		~npcHelpConfig(){};
		int localID_;
		int cost_;
		int warStoryLimit_;
		int fortLimit_;
		int minPassNumber_;
	};
	typedef boost::shared_ptr<npcHelpConfig> npcHelpConfigPtr;

	enum
	{
		condition_null,
		condition_legion,
		condition_sphere
	};

	struct memberUnion
	{
		playerDataPtr player_;
		mapDataPtr npc_;
		battleSidePtr npcFromPlayer_;
		bool isPlayer()
		{
			return player_ != playerDataPtr();
		};
		bool isNpc()
		{
			return npc_ != mapDataPtr();
		};
		bool isNpcFromPlayer()
		{
			return npcFromPlayer_ != battleSidePtr();
		};
	};

	struct teamItem
	{
		teamItem();
		vector<memberUnion> members_;
		int joinCondition_;
		int levelLimit_;
		int aliveTime_;
		unsigned createTime_;
		bool isNewbie_;
		bool is5secondAdd_;
		bool hasSeekHelp_;
		string prefixName_;;
		unsigned lastAddTime_;
		unsigned lastNpcHelpAddTime_;
		vector<string> robotNameList_;
		void package(Json::Value &pack);
		bool addNewRobot();
		bool addNewNpcFromPlayer(battleSidePtr addSide);
	};

	struct memberInfoItem
	{
		int localID_;
		int leaderPlayer_;
	};

	typedef boost::shared_ptr<teamItem> teamItemPtr;
	typedef boost::unordered_map< string , teamItemPtr > teamItemMap;
	struct teamList
	{
		teamList(){localID = -1;}
		teamList(int lid){localID = lid;}
		~teamList(){}
		vector<teamItemPtr> teams;
		teamItemPtr getTeamItem(int leader);
		int localID;
		int package(int startIndex, int teamOffset, Json::Value &pack, int &maxIndex);
		void leaderAliveCheck();
		void addRobot();
//		void addNpcFromPlayer(int passMapID);

		static teamItemPtr createTeamItem()
		{
			void* p = GGNew(sizeof(teamItem));
			return teamItemPtr(new(p) teamItem(), destoryTeamItem);
		}

		static void destoryTeamItem(teamItem* point)
		{
			point->~teamItem();
			GGDelete(point);
		}

	};

	typedef boost::shared_ptr<teamMapData> teamMapDataPtr;
	typedef boost::shared_ptr<teamList> teamListPtr;
	const static int mapIDOffset = 100;// 1000 -> 100 2014/7/3

	struct warStoryRollBroadCast 
	{//19:55 ¡¢20:55¡¢21:55
		warStoryRollBroadCast();
		bool br_12_25_;
		bool br_12_55_;
		bool br_13_25_;
		bool br_19_55_;
		bool br_20_55_;
		bool br_21_55_;

		bool nbr_12_30_;
		bool nbr_20_00_;
		unsigned nextUpdateTime_;
		void reset();
		void refresh();
	};

	class warStory
	{
	public:
		static warStory* const warSys;
		void initData();
		mapDataPtr getMapData(const int localID);
		teamMapDataPtr getTeamMapData(const int localID);
		vector<int> getAllMapID();
		vector<int> getAllTeamMapID();
		vector<int> getAllMapLocalID();
		//void noticeClientBattleReport(playerDataPtr d,const string report_name,const int battles_res);
		void warStoryUpdate(msg_json& m, Json::Value& r);
		void battleWithNpc(msg_json& m, Json::Value& r);
		void teamUpdateReq(msg_json& m, Json::Value& r);
		void teamManualUpdateReq(msg_json& m, Json::Value& r);
		void teamMemberUpdateReq(msg_json& m, Json::Value& r);
		void teamCreateReq(msg_json& m, Json::Value& r);
		void teamQuitInterfaceReq(msg_json& m, Json::Value& r);
		void teamBonusUpdateReq(msg_json& m, Json::Value& r);
		void teamInviteReq(msg_json& m, Json::Value& r);
		void teamChangeMemberPosReq(msg_json& m, Json::Value& r);
		void teamRemoveMemberReq(msg_json& m, Json::Value& r);
		void teamDismissReq(msg_json& m, Json::Value& r);
		void teamAttackReq(msg_json& m, Json::Value& r);
		void teamJoinReq(msg_json& m, Json::Value& r);
		void teamQuitReq(msg_json& m, Json::Value& r);
		void teamMapUpdateReq(msg_json& m, Json::Value& r);
		void chaper1RewardReq(msg_json& m, Json::Value& r);
		void chaper2RewardReq(msg_json& m, Json::Value& r);
		void chaper3RewardReq(msg_json& m, Json::Value& r);
		void teamLeaderAliveReq(msg_json& m, Json::Value& r);
		void teamNpcHelpUpdateReq(msg_json& m, Json::Value& r);
		void teamNpcHelpReq(msg_json& m, Json::Value& r);
		void teamAddNpcFromPlayerReq(msg_json& m, Json::Value& r);
		void teamSweepReq(msg_json& m, Json::Value& r);

		void pushBattleNpc(const int localID, battleSidePtr npc);
		bool isProcessMap(playerDataPtr player, const int mapID);
		npcDataList formatNpcArmy(Json::Value& dataJson);
		actionVector formationAction(Json::Value& dataJson, int actionOpt);
		void playerLogout(int player_id);
		bool getRewardTime(unsigned &start_time, unsigned &end_time);
		static mapDataPtr create()
		{
			void* p = GGNew(sizeof(mapData));
			return mapDataPtr(new(p) mapData(), destory);
		}
		void sendToTeamList(int localID, msg_json& m);
		void warStoryTeamLoopUpdate(boost::system_time& tmp);
		int calTeamKejiReward(playerDataPtr player, playerDataPtr leader);
		int getCDCreateMap(){ return CDCreateMap; };
		string getNewRandomName(vector<string> allNames = vector<string>(0), vector<string> exceptNames = vector<string>(0));
		int getNewRobotID();
		void removeRobot(int lid);
		const npcDataList getRobotInst(){ return robotItem_; };
		void updateTeamMem2Client(teamItemPtr teamItemData);
	private:
		playerDataPtr getTeamLeader(teamItemPtr teamItemData);
		teamListPtr getTeamList(int localID);
		npcHelpConfigPtr getNpcHelpConfig(int localID);
		bool isMemberIn(int player_id);

		static teamMapDataPtr createTeam()
		{
			void* p = GGNew(sizeof(teamMapData));
			return teamMapDataPtr(new(p) teamMapData(), destoryTeam);
		}

		static teamListPtr createTeamList(int localID)
		{
			void* p = GGNew(sizeof(teamList));
			return teamListPtr(new(p) teamList(localID), destoryTeamList);
		}

		static void destoryTeam(teamMapData* point)
		{
			point->~teamMapData();
			GGDelete(point);
		}

		static void destory(mapData* point)
		{
			point->~mapData();
			GGDelete(point);
		}

		static void destoryTeamList(teamList* point)
		{
			point->~teamList();
			GGDelete(point);
		}

		static npcHelpConfigPtr createNpcHelpPtr()
		{
			void* p = GGNew(sizeof(npcHelpConfig));
			return npcHelpConfigPtr(new(p)npcHelpConfig(), destoryNpcHelp);
		}

		static void destoryNpcHelp(npcHelpConfig* point)
		{
			point->~npcHelpConfig();
			GGDelete(point);
		}

		typedef boost::unordered_map< int , int > completeMap;
		typedef boost::unordered_map< int , set<playerDataPtr> > refreshMap;
		typedef boost::unordered_map< int , mapDataPtr > warStoryMap;
		typedef boost::unordered_map< int , teamMapDataPtr > teamWarStoryMap;
		typedef boost::unordered_map< int , teamListPtr > teamListMap;
		typedef vector<int> warStoryVector;
		typedef boost::unordered_map<int, memberInfoItem> memberInfoMap;
		typedef boost::unordered_map< int, npcHelpConfigPtr > npcHelpconfigMap;
		warStoryMap map_;
		warStoryMap chaperRewardMap[3];
		teamWarStoryMap tmap_;
		completeMap cmap_;
		warStoryVector mapIDVec_;
		warStoryVector mapIDTeamVec_;
		teamListMap teamListMap_;
		memberInfoMap memberInfo_;
		int CDCreateMap;
		refreshMap refreshList_;
		boost::system_time tick;
		int duration_;
		int state_;
		warStoryRollBroadCast br_;
		npcDataList robotItem_;
		string strPoint_;
		vector<int> robotIDUsed_;
		typedef boost::unordered_set<int> SignSet;
		SignSet NpcIDList;
		bool isMatchNpcList(const int localID);
		typedef std::vector<battleSidePtr> OneVec;
		typedef boost::unordered_map<int, OneVec> NpcMaps;
		npcHelpconfigMap npcHelpConfigMap_;
		NpcMaps npcMap;
	};
}