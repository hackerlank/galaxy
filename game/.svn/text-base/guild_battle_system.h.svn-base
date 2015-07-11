#pragma once
#include "json/json.h"
#include "msg_base.h"
#include "war_story.h"
#include <map>

#define guild_battle_sys (*gg::guild_battle_system::guildBattleSys)
//是否达到开放城池的条件，退团等处理方式
using namespace na::msg;

namespace gg
{
	namespace guild_battle_def
	{
		const string static strGuildConfigDir = "./instance/guildbattle/";
		const string static strLimit = "lm";
		const string static strGuildBattleDBData = "gl.guild_battle_area";
		const string static strOccupier = "op";
		const string static strRemainLeveyTimes = "rlt";
		const string static strContender1 = "c1";
		const string static strContender2 = "c2";
		const string static strFightResult1 = "fr1";
		const string static strFightResult2 = "fr2";
		const string static strFighterNum = "fn";
		const string static strLevyValue = "ly";
		const string static strApplyState = "as";
		const string static strAtkDefWinStreak = "adw";

		const string static strInspireGoldCost = "igc";
		const string static strInspireKejiCost = "ikc";
		
		const string static strFighterList1 = "fl1";
		const string static strFighterList2 = "fl2";
		const string static strStamp = "st";
		const string static strAreaID = "aid";
		const string static strNextAreaSeasonUpdateTime = "ut";
		const string static strNextAreaDayUpdateTime = "dut";
		const string static strStartLevyTime_ = "slt";
		const string static strReportLink = "rp";
		const string static strHasInvite = "hi";
		const string static strMysqlLogGuildBattle = "log_guild_battle";
	}

	enum
	{
		log_guild_battle_inspire_keji,
		log_guild_battle_inspire_gold,
		log_guild_battle_apply,
		log_guild_battle_atk,//每个玩家的战斗
		log_guild_battle_total,//每场战斗的情况
		log_guild_battle_final_result//最终占领情况
	};

	enum
	{
		guild_battle_state_null,
		guild_battle_state_ing,
	};

	struct starAreaConfig
	{
		static starAreaConfig* sNullPoint;
		starAreaConfig();
		~starAreaConfig()
		{

		}
		starAreaConfig(int aid);
		bool operator == (starAreaConfig& s);
		int areaID_;
		int canLeveyBaseNumber_;
		int levelCondition_;
		int guildLevelCondition_;
		teamMapDataPtr teamMapData_;

		static teamMapDataPtr createTeam()
		{
			void* p = GGNew(sizeof(teamMapData));
			return teamMapDataPtr(new(p) teamMapData(), destoryTeam);
		}

		static void destoryTeam(teamMapData* point)
		{
			//静态变量无法通过内存池申请内存..//根治办法是不用静态变量或者在之后再创建实例,而不是在在构造函数中申请内存
			if(point != NULL)
			{
				point->~teamMapData();
				GGDelete(point);
			}
		}
	};

	struct fighterData 
	{
		fighterData();
		~fighterData();
		int sumAll();
		int atkValue_;
		int defValue_;
		int winnerStreak_;
	};

	struct enterLimit
	{
		enterLimit();
		void reset();
		bool isPermit(playerDataPtr pdata);
		int level_;
		int process_;
		int arenaRank_;
		bool valid_;
	};

	struct guildBattleRollBroadcast 
	{
		guildBattleRollBroadcast();
		bool br_19_30_;
		bool br_20_00_;
		bool br_20_11_;
		bool winterReset_;
		bool afterBattleClear_;
		bool stateNullClear_;
		unsigned nextUpdateTime_;
		void reset();
		void refresh();
	};

	struct annouceData
	{
		annouceData();
		void refresh();
		bool apply_;
		bool first_join_team_;
		bool second_join_team_;
		bool ad_19_30_;
		unsigned nextUpdateTime_;
	};

	typedef boost::shared_ptr<fighterData> fighterDataPtr;

	struct starAreaData
	{
		static starAreaData sNull;
		starAreaData();
		bool operator == (starAreaData& s);
		int areaID_;
		string occupier_;
		string contender_[2];
		//
		vector<map <int, fighterDataPtr> > fighterData_;
		vector<vector <playerDataPtr> > playerData_;
		set<int> enterFirstList_;
		set<int> enterSecondList_;
		enterLimit limit_[3];
		int battleResult_[2];
		int nextSeasonUpdateTime_;
		int nextDayUpdateTime_;
		int startLevyTime_;
		int reportLink_;
		int finishTheFirstBattle_;
		int finishTheSecondBattle_;
		int inviteTime_;
		bool hasInvite_;

		void maintainDailyData();
		void maintainSeasonData();
		bool canLevyInAdvance(string guildName);
	};

	enum
	{
		state_null,
		state_apply,
		state_first_join_team,
		state_first_fight,
		state_second_join_team,
		state_second_fight,
		state_after_battle
	};

	class guild_battle_system
	{
	public:
		static guild_battle_system* const guildBattleSys;
		guild_battle_system();
		~guild_battle_system();
		void guildBattleTotalUpdateReq(msg_json& m, Json::Value& r);
		void guildBattleSingleUpdateReq(msg_json& m, Json::Value& r);
		void guildBattleLevyReq(msg_json& m, Json::Value& r);
		void guildBattleApplyReq(msg_json& m, Json::Value& r);
		void guildBattleEnterReq(msg_json& m, Json::Value& r);
		void guildBattleEnterLimitReq(msg_json& m, Json::Value& r);
		void guildBattleEnterCancelLimitReq(msg_json& m, Json::Value& r);
		void guildBattleEnterUpdateReq(msg_json& m, Json::Value& r);
		void guildBattleExitReq(msg_json& m, Json::Value& r);
		void guildBattleInviteReq(msg_json& m, Json::Value& r);
		void guildBattleInspireReq(msg_json& m, Json::Value& r);
		void initData();
		void guildBattleUpdate(boost::system_time& tmp);
		void fight(int opt);
		void checkAdvanceEnd();
		void addBattleBuff(playerDataPtr pdata, battleSide& side);
		int getState(){return state_;};
		void teamGuildRespon(BattlefieldMuilt field, Json::Value report);
		void playerLogout(int player_id);
		starAreaData& getAreaData(int areaID);
		void getGuildContender(string guildName, int &contenderAreaID, int &contenderIndex);
		void getGuildOccupyArea(string guildName, int &occupyAreaID);
		void recordTest();
		bool getActTime(unsigned &start_time, unsigned &end_time);
		void processRemoveGuild(string gName);
		void renameGuild(string oldName, string newName);
	private:
		int getInspireGoldCost();
		int getInspireKejiCost(playerDataPtr player);
		int getLevyValue(starAreaConfig &cfg, playerDataPtr player);
		bool isSecondBattleEnd();
		void recordGuildBattleEvent(int areaID);
		static fighterDataPtr createFighterData()
		{
			void* p = GGNew(sizeof(fighterData));
			return fighterDataPtr(new(p) fighterData(), destoryFighterData);
		}

		static void destoryFighterData(fighterData* point)
		{
			point->~fighterData();
			GGDelete(point);
		}

		void initAreaConfig();
		void initAreaData();
		const starAreaConfig getAreaConfig(int areaID);
		bool setAreaData(int areaID, starAreaData& adata);
		int getRemainLevyTimes(playerDataPtr pdata, starAreaData& adata);
		bool saveAreaData(int areaID);
		void enterUpdate(int curIndex, int curAreaID, playerDataPtr pdata, int updateSide = -1);
		void chatBroadCastBeginJoinTeam(int opt);
		void clearAreaDataDaily();
		void clearSingleAreaDataDaily(int areaID);
		int getBattleTime(int curState);
		
		typedef boost::unordered_map< int , starAreaConfig> starAreaConfigMap;

		boost::system_time tick;
		int duration_;
		starAreaConfigMap starAreaConfig_;
		map<int, starAreaData> starAreaData_;
		int state_;
		annouceData annouce_;
//		unsigned levyNextUpdateTime_;
		guildBattleRollBroadcast br_;
	};
}