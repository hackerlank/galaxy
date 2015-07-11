#pragma once

#include "battle_def.h"
#include "json/json.h"
#include "core_helper.h"
#include "player_data.h"
#include "game_server.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>

#define battle_sys (*gg::battle_system::battleSys)

namespace gg
{
	const static int _AddEnergy = 34;

	enum
	{
		battle_player_vs_player_type,
		battle_player_vs_npc_type,
		battle_arena_type,
		battle_ruler_type,
		battle_ruler_final_type,
		battle_mine_res_type,
		battle_starwar_with_player_type,
		battle_starwar_with_npc_type,
		battle_world_boss_type,

		battle_sts_end,
		team_battle_begin = 1000,
		
		team_battle_npc_type,
		team_battle_guild_type,

		team_battle_end,
	};



	//临时战报
	const static string ShareReportPublic = "./report/temp/sharePublic/";

	namespace BATTLE
	{
		const static string strBattleReward = "brw";//战斗奖励
		const static string strBattleType = "tp"; //战斗类型
		const static string strBattleResult = "brs";//战斗结果
		const static string strBattleStar = "bst";
		const static string strArmyFormation = "f";//布阵信息
		const static string strInfoAtk = "an";
		const static string strInfoDef = "dn";
		const static string strBigRound = "br";//当前大回合数
		const static string strTeamDo = "td";//团战行动指南
		const static string strTeamBattleReport = "tbr";//团战资料

		//配置字段
		const static string strArenaParam = "ap";//竞技场附带参数 
		const static string strStarwarParam = "swp";//国战附带参数 
		const static string strBackGround = "bg";//背景
		const static string strTeamStep = "ts";//对阵数量
		const static string strTeamReportDir = "trd";//战报地址
		const static string strWarMapID = "wmid";//星区id
		const static string strWarStoryTeamMemberType = "mt";//要塞战队友类型
	}

	class battle_system
	{
	public:
		void DealStarwarWithNpcReport(Battlefield& field, Json::Value& report);
		void DealStarwarWithPlayerReport(Battlefield& field, Json::Value& report, bool flag);
		void DealArenaReport(Battlefield& field, Json::Value& report, bool flag);
		void DealStrategyReport(string filename, Json::Value report);
		void DealPVEReport(Battlefield field, Json::Value report, const bool defeat = false);
		void DealPVPReport(Battlefield field, const string file, Json::Value report);
		void DealTeamPVEReport(BattlefieldMuilt fields, Json::Value report);
		void DealTeamGuildReport(BattlefieldMuilt fields, Json::Value report);
		void MineResRespon(const int battleResult, Battlefield field, playerDataPtr atkData, Json::Value report);
		void WorldBossRespon(const int battleResult, Battlefield field, playerDataPtr atkData, Json::Value &report);
	public:
		static battle_system* const battleSys;
		battle_system();
		void initBattleSystem();
		static void PostBattle(Battlefield field, const int type);
		static void PostTeamBattle(BattlefieldMuilt fields, const int type);
	private:
		//////////////////////////////////////////////////////////////////////////
		//single battleData
		battleSidePtr currentAtkSide;//当前攻击阵营
		battleSidePtr currentDefSide;//当前防守阵营
		battleMemPtr currentAtk;//当前攻击部队
		battleMemPtr currentDef;//当前攻击部队
		Params::VarPtr currentSkill;//当前使用的技能
		bool useSkillEnergy;//当前是否为使用士气的技能
		bool isCounter;//当前行动是不是反击
		int currentRound;//当前回合
		battleMemPtr appointTarget;//当前指定目标

		//////////////////////////////////////////////////////////////////////////
		//战斗相关
		void _TeamBattle(BattlefieldMuilt fields, const int type);
		int _Battle(Battlefield field, const int type);
		int SingleBattle(Battlefield field, const int type, Json::Value& report);
		void _ClearOTOBattle();

		int attack(Json::Value& report);//战前清理
		int pick_skill(Json::Value& report);//选择技能
		int use_skill(Json::Value& report);//选择使用技能的阶段
		battleMemPtr effect_skill(Json::Value& report);//技能发生效果的阶段
		void run_skill(Params::Var& skillPart, Json::Value& report);
		//////////////////////////////////////////////////////////////////////////其他
		//其他
		void createDir(string dir);

		bool isGoOn(battleMemPtr atk, Json::Value& report);
		string getBattleFileName(const int bType);
		void writeReport(Battlefield& field, Json::Value& report);
		void wirteTeamReport(BattlefieldMuilt& field, Json::Value& report);

		void packageArmy(battleSidePtr side, Json::Value& armyJson);

		///伤害计算公式
		typedef boost::function< int ( Params::Var&, Json::Value& ) > DamageFunction;
		typedef boost::unordered_map<int, DamageFunction> DamageFMap;
		DamageFMap _DFMap;

		int reduceCurrentHPRate(Params::Var& sk, Json::Value& report);
		int doingRate(Params::Var& sk, Json::Value& report);
		int absorbEnergy(Params::Var& sk, Json::Value& report);
		int exchangeHPFromCurrentDamage(Params::Var& sk, Json::Value& report);
		int setBuff(Params::Var& sk, Json::Value& report);
		int setEnergy(Params::Var& sk, Json::Value& report);
		int alterEnergy(Params::Var& sk, Json::Value& report);
		int setSelfBackUpEnergy(Params::Var& sk, Json::Value& report);
		int magicAddHP(Params::Var& sk, Json::Value& report);
		int reduceDefEnergyFormule(Params::Var& sk, Json::Value& report);
		int atk2def(Params::Var& sk, Json::Value& report);
		int war2wardef(Params::Var& sk, Json::Value& report);
		int magic2magicdef(Params::Var& sk, Json::Value& report);
		void CalculateAtk2Def(Params::Var& sk, battleMemPtr atk, const int round, bool& isCri, bool isSelf = false);
		void CalculateWar2WarDef(Params::Var& sk, battleMemPtr atk, const int round, bool& isCri, bool isSelf = false);
		void CalculateMagic2MagicDef(Params::Var& sk, battleMemPtr atk, const int round, bool& isCri, bool isSelf = false);

		double getBuffValue(int buff_type, vector<battleBuff>& buffs);
		void formatRule(int t[3], const int s[3]);
		void getTarget(Params::Var& skill, battleMemPtr atk, battleSidePtr atkArmy, battleSidePtr defArmy, battleMemPtr def = battleMemPtr());
		void getSingleTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk);
		void getLineTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk);
		void getRowTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk);
		void getTTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk);
		void getAllTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk, const int Type, const bool selfin = true);
		void getRandom(battleSidePtr targetSide, battleMemPtr atk, int signNum, const int Type);
		battleMemPtr singleTarget(battleMemPtr atk, battleSidePtr targetSide, const int type);
		battleMemPtr singletargetOPP(battleMemPtr atk, battleSidePtr targetSide, const int type);

		struct reportTick{
			reportTick(){
				tempPve = 0;
				tempPvp = 0;
				tempTeam = 0;
				tempTeamGuild = 0;
			}
			int tempPve;
			int tempPvp;
			int tempTeam;
			int tempTeamGuild;
		};
		reportTick rTick_;
		bool criTag_;
	};
}