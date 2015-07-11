#include "one_to_one.h"
#include "commom.h"
#include "skill_def.h"
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "mineHeader.h"
#include <boost/filesystem/operations.hpp>
#include "system_response.h"
#include "arena_system.h"
#include "ruler_system.h"
#include "mine_resource_system.h"
#include "guild_system.h"
#include "email_system.h"
#include "chat_system.h"
#include "action_def.h"
#include "record_system.h"
#include <boost/lexical_cast.hpp>
#include "starwar_system.h"
#include "world_boss_system.h"
#include "task_system.h"

using namespace std;

#pragma warning(once:4244)

namespace gg
{
	using namespace BATTLE;
	const static unsigned _BigRound = 30;
	const static unsigned _LittleRound = 9;
	const static int _PriorityXY[3][3] = { {0,1,2} , {1,0,2} , {2,1,0}};
	const static int _MaxEnergy = 300;

#define CheckVectorEffectiveRange(v, i)\
		if(i >= 0 && (unsigned)(i) < v.size())

#define FormatRule(INDEX, VARNAME)\
	int VARNAME[3];\
	formatRule(VARNAME, _PriorityXY[INDEX]);\


	battle_system* const battle_system::battleSys = new battle_system();

	const static string reportMainDir = "./report/";
	const static string reportTempDir = "./report/temp/";
	const static string reportEngrossDir = "./report/engross/";
	const static string reportBugDir = "./report/temp/Bug/";
	const static string reportTeamBugDir = "./report/temp/teamBug/";
	const static string reportTempPVEDir = "./report/temp/tempPVE/";
	const static string reportTempPVPDir = "./report/temp/tempPVP/";
	const static string teamReportTempDir = "./report/temp/teamTemp/";
	const static string teamReportGuildDir = "./report/temp/teamGuild/";
	const static string worldBossDir = "./report/temp/worldBoss/";

#define CRITAG(LOGIC) criTag_ = LOGIC ? LOGIC : criTag_;

	namespace Team
	{
		struct fieldValue
		{
			fieldValue(){
				atkValue = 0;
				defValue = 0;
			}
			int atkValue;
			int defValue;
		};

		namespace HELPER
		{
			static battleSidePtr Pop(vector<battleSidePtr>& vec)
			{
				battleSidePtr side = battleSidePtr();
				if(!vec.empty()){
					side = vec[0];
					vec.erase(vec.begin());
				}
				return side;
			}
		}

		static fieldValue LookAlive(vector<Battlefield>& vec)
		{
			fieldValue alive;
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				if(vec[i].atkSide != battleSidePtr())++alive.atkValue;
				if(vec[i].defSide != battleSidePtr())++alive.defValue;
			}
			return alive;
		}

		static void BattlePanel(vector<Battlefield>& panel, vector<battleSidePtr>& atkVec, vector<battleSidePtr>& defVec)
		{
			for (unsigned i = 0; i < panel.size(); ++i)
			{
				Battlefield& field = panel[i];
				if(field.atkSide == battleSidePtr())field.atkSide = HELPER::Pop(atkVec);
				if(field.defSide == battleSidePtr())field.defSide = HELPER::Pop(defVec);
			}

			for (unsigned i = 0; i < panel.size(); ++i)
			{
				Battlefield& current = panel[i];
				if(current.atkSide != battleSidePtr() && current.defSide == battleSidePtr()){
					for (unsigned n = 0; n < panel.size(); ++n){
						Battlefield& match = panel[n];
						if(match.defSide != battleSidePtr() && match.atkSide == battleSidePtr()){
							current.defSide = match.defSide;
							match.defSide = battleSidePtr();
							break;
						}
					}
				}
			}
		}
	}

	void battle_system::PostBattle(Battlefield field, const int type)
	{
		{
			playerDataPtr player = player_mgr.getPlayerMain(field.atkSide->getPlayerID());
			if (player != NULL)
			{
				player->SetBattleState();
				task_sys.updateBranchTaskInfo(player, _task_battle_polit_num);
			}
		}
		{
			playerDataPtr player = player_mgr.getPlayerMain(field.defSide->getPlayerID());
			if (player != NULL)
			{
				player->SetBattleState();
				task_sys.updateBranchTaskInfo(player, _task_battle_polit_num);
			}
		}
		_Battle_Post(boost::bind(&battle_system::_Battle, battle_system::battleSys, field, type));
	}

	void battle_system::PostTeamBattle(BattlefieldMuilt fields, const int type)
	{
		{
			vector<battleSidePtr>& vec = fields.atkSide;
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				playerDataPtr player = player_mgr.getPlayerMain(vec[i]->getPlayerID());
				if(player != NULL)player->SetBattleState();
			}
		}
		{
			vector<battleSidePtr>& vec = fields.defSide;
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				playerDataPtr player = player_mgr.getPlayerMain(vec[i]->getPlayerID());
				if(player != NULL)player->SetBattleState();
			}
		}
		_Battle_Post(boost::bind(&battle_system::_TeamBattle, battle_system::battleSys, fields, type));
	}

	battle_system::battle_system()
	{
		criTag_ = false;

		_DFMap[atk_to_def_type] = boost::bind(&battle_system::atk2def, this, _1,_2);
		_DFMap[war_to_anti_war_type] = boost::bind(&battle_system::war2wardef, this, _1,_2);
		_DFMap[magic_to_magic_def_type] = boost::bind(&battle_system::magic2magicdef, this, _1,_2);
		_DFMap[magic_to_add_hp_type] = boost::bind(&battle_system::magicAddHP, this, _1,_2);
		_DFMap[set_buff_type] = boost::bind(&battle_system::setBuff, this, _1,_2);
		_DFMap[set_backup_energy_type] = boost::bind(&battle_system::setSelfBackUpEnergy, this, _1,_2);
		_DFMap[alter_energy_formule_type] = boost::bind(&battle_system::reduceDefEnergyFormule, this, _1,_2);
		_DFMap[alter_energy_type] = boost::bind(&battle_system::alterEnergy, this, _1,_2);
		_DFMap[set_energy_type] = boost::bind(&battle_system::setEnergy, this, _1,_2);
		_DFMap[damage_conver_by_hurt_type] = boost::bind(&battle_system::exchangeHPFromCurrentDamage, this, _1,_2);
		_DFMap[absorb_energy_type] = boost::bind(&battle_system::absorbEnergy, this, _1,_2);
		_DFMap[doing_rate_type] = boost::bind(&battle_system::doingRate, this, _1,_2);
		_DFMap[reduce_current_rate_hp_type] = boost::bind(&battle_system::reduceCurrentHPRate, this, _1,_2);
	}

	void battle_system::initBattleSystem()
	{
		srand(na::time_helper::get_current_time());
		createDir(reportMainDir);
		createDir(reportTempDir);
		createDir(reportEngrossDir);
		createDir(reportBugDir);
		createDir(reportTempPVEDir);
		createDir(reportTempPVPDir);
		createDir(teamReportTempDir);
		createDir(teamReportGuildDir);
		createDir(ShareReportPublic);
		createDir(worldBossDir);
	}

	void ADDDEFAULTINFO(battleSidePtr side, Json::Value& report)
	{
		armyVec av = side->getMember(MO::mem_null);
		Json::Value pilotJson;
		pilotJson[0u] = action_add_pilot_exp;
		pilotJson[1u] = 0;
		Json::Value& pilotExp = pilotJson[2u];
		pilotExp = Json::arrayValue;
		for (unsigned  i = 0; i < av.size(); ++i)
		{
			battleMemPtr mem = av[i];
			Json::Value single;
			single.append(mem->getID());
			single.append(mem->getLv());
			single.append(0);
			single.append(mem->getLv());
			single.append(0);
			single.append(false);
			pilotExp.append(single);
		}
		report.append(pilotJson);
	}

	const static unsigned bigBuffSize = 1024 * 1024 * 10;
	void writeToFile(string& filePath, Json::Value& report)
	{
		static unsigned char* bigBuff = (unsigned char*)GGNew(bigBuffSize);
		memset(bigBuff, 0x0, bigBuffSize);
		try
		{
			std::ofstream f(filePath.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
// 			string writeData = report.toStyledString();
// 			writeData = commom_sys.tighten(writeData);
			string writeData = commom_sys.json2string(report);
			unsigned long lsize = bigBuffSize;
			int res = compress(bigBuff, &lsize, (const unsigned char*)writeData.c_str(), writeData.length());
			if (res != Z_OK){
				LogE << "zlib compress failed ... error code :" << res << LogEnd;
			}
			else{
				f.write((const char*)bigBuff, lsize);
				f.flush();
			}
			f.close();
		}
		catch (std::exception& e)
		{
			LogE << e.what() << LogEnd;
		}
	}

#define PUSHID(VEC, ID)\
	{\
		if(ID > 0)VEC.push_back(ID);\
	}


	void battle_system::wirteTeamReport(BattlefieldMuilt& field, Json::Value& report)
	{
		int battleType = report[strBattleType].asInt();
		int battleResult = report[strBattleResult].asInt();
		vector<int> IDList;
		{
			vector<battleSidePtr>& vec = field.atkSide;
			for (unsigned  i = 0; i < vec.size(); ++i)
			{
				PUSHID(IDList, vec[i]->getPlayerID());
			}
		}
		{
			vector<battleSidePtr>& vec = field.defSide;
			for (unsigned  i = 0; i < vec.size(); ++i)
			{
				PUSHID(IDList, vec[i]->getPlayerID());
			}
		}
		if(battleType == team_battle_npc_type)
		{
			_Logic_Post(boost::bind(&system_response::teamPVERespon, system_response::respSys, 
				field, report));
		}
		else if(battleType == team_battle_guild_type)
		{
			_Logic_Post(boost::bind(&guild_battle_system::teamGuildRespon, guild_battle_system::guildBattleSys, 
				field, report));
		}
		_Logic_Post(boost::bind(&system_response::OverBattle, system_response::respSys, IDList));
	}

	void battle_system::writeReport(Battlefield& field, Json::Value& report)
	{
		int battleType = report[strBattleType].asInt();
		int battleResult = report[strBattleResult].asInt();
		do{
				vector<int> IDList;
				PUSHID(IDList, field.atkSide->getPlayerID());
				PUSHID(IDList, field.defSide->getPlayerID());
				if(battleType == battle_arena_type)
				{
					DealArenaReport(field, report, true);
					string atkFileName = reportArenaPVPDir + field.atkSide->getFileName();
					writeToFile(atkFileName, report);
					bool isfake = field.ParamJson[BATTLE::strArenaParam][2u].asBool();
					if (!isfake)
					{
						DealArenaReport(field, report, false);
						string defFileName = reportArenaPVPDir + field.defSide->getFileName();
						writeToFile(defFileName, report);
					}
		
					_Logic_Post(boost::bind(&arena_system::arenaChallegeResp, arena_system::arenaSys, battleResult, field));
				}else
				if(battleType == battle_player_vs_npc_type)
				{
					_Logic_Post(boost::bind(&system_response::PVERespon, 
						system_response::respSys,	field, report));
				}else
				if(battleType == battle_player_vs_player_type)
				{
					string fileName = getBattleFileName(battleType);
					Json::Value& rw = report[BATTLE::strBattleReward];
					ADDDEFAULTINFO(field.atkSide, rw);
					_Logic_Post(boost::bind(&system_response::PVPRespon, system_response::respSys, 
						fileName, report, field));
				}else
				if(battleType == battle_ruler_type)
				{
					string fileName = reportRulerPVPDir + field.atkSide->getFileName();
					writeToFile(fileName, report);
		
					_Logic_Post(boost::bind(&ruler_system::rulerChallengeResp, ruler_system::rulerSys, battleResult, field));
				}else
				if(battleType == battle_ruler_final_type)
				{
					string fileName = reportRulerPVPDir + field.atkSide->getFileName();
					writeToFile(fileName, report);
		
					_Logic_Post(boost::bind(&ruler_system::rulerFinalBattleResp, ruler_system::rulerSys, battleResult, field));
				}else
				if (battleType == battle_mine_res_type)
				{
					_Logic_Post(boost::bind(&mine_resource_system::mineResFightCallback, mine_resource_system::mineResSys, battleResult, field, report));
					return;
				}else
				if (battleType == battle_starwar_with_npc_type)
				{
					string atkFileName = reportStarwarDir + field.atkSide->getFileName();
					DealStarwarWithNpcReport(field, report);
					writeToFile(atkFileName, report);

					_Logic_Post(boost::bind(&starwar_system::attackWithNpcInnerResp, starwar_system::starwarSys, battleResult, field));
				}
				else
				if (battleType == battle_starwar_with_player_type)
				{
					string atkFileName = reportStarwarDir + field.atkSide->getFileName();
					DealStarwarWithPlayerReport(field, report, true);
					writeToFile(atkFileName, report);
					string defFileName = reportStarwarDir + field.defSide->getFileName();
					DealStarwarWithPlayerReport(field, report, false);
					writeToFile(defFileName, report);
					_Logic_Post(boost::bind(&starwar_system::attackWithPlayerInnerResp, starwar_system::starwarSys, battleResult, field));
				}
				else 
				if (battleType == battle_world_boss_type)
				{
					_Logic_Post(boost::bind(&world_boss_system::worldBossAttackCallback, world_boss_system::worldBossSys, battleResult, field, report));
					return;
				}
				else
				{
					LogE << "no type match to write report !!" << LogEnd;
					break;
				}
				_Logic_Post(boost::bind(&system_response::OverBattle, system_response::respSys, IDList));
		}while(false);
	}

	string battle_system::getBattleFileName(const int bType)
	{
		string fileName = "bug";
		if(bType == battle_player_vs_player_type){
			fileName = boost::lexical_cast<string , int>(rTick_.tempPvp++);
			rTick_.tempPvp %= 3000;
		}else
		if(bType == battle_player_vs_npc_type){
			fileName =  boost::lexical_cast<string , int>(rTick_.tempPve++);
			rTick_.tempPve %= 3000;
		}else
		if(bType == team_battle_npc_type){
			fileName =  boost::lexical_cast<string , int>(rTick_.tempTeam++);
			rTick_.tempPve %= 3000;
		}else
		if (bType == team_battle_guild_type){
			fileName =  boost::lexical_cast<string , int>(rTick_.tempTeamGuild++);
			rTick_.tempTeamGuild %= 3000;
		}
		return fileName;
	}

	void battle_system::createDir(string dir)
	{
		boost::filesystem::path path_dir(dir);
		if(!boost::filesystem::exists(path_dir))
			boost::filesystem::create_directory(path_dir);
	}
	
	void battle_system::_TeamBattle(BattlefieldMuilt fields, const int type)
	{
		string reportFile = fields.ParamJson[strTeamReportDir].asString();
		if(reportFile == "")reportFile = getBattleFileName(type);
		if(reportFile == "bug"){LogE << "no match team to team battle !!" << LogEnd;return;}
		fields.ParamJson[strTeamReportDir] = reportFile;
		Json::Value report;
		report[strBattleType] = type;
		report[strTeamStep] = fields.ParamJson[strTeamStep];
		report[strInfoAtk] = Json::arrayValue;
		report[strInfoDef] = Json::arrayValue;
		report[strBattleResult] = 2;
		for (unsigned i = 0; i < fields.atkSide.size(); ++i)
		{
			report[strInfoAtk][i].append(fields.atkSide[i]->getName());
			report[strInfoAtk][i].append(fields.atkSide[i]->getInsNum());
			report[strInfoAtk][i].append(fields.atkSide[i]->isPlayerSide());
		}
		for (unsigned i = 0; i < fields.defSide.size(); ++i)
		{
			report[strInfoDef][i].append(fields.defSide[i]->getName());
			report[strInfoDef][i].append(fields.defSide[i]->getInsNum());
			report[strInfoDef][i].append(fields.defSide[i]->isPlayerSide());
		}
		report[strTeamDo] = Json::arrayValue;
		Json::Value& TeamJson = report[strTeamDo];
		report[strTeamBattleReport] = Json::arrayValue;
		Json::Value& BattleReprotJson = report[strTeamBattleReport];
		int battleRes = 2;//守方胜利
		unsigned step = fields.ParamJson[strTeamStep].asUInt();
		step = step < 1 ? 1 : step;
		unsigned reportIdx = 0;
		int loopTimes = 0;
		vector<battleSidePtr> attacker = fields.atkSide;
		vector<battleSidePtr> defender = fields.defSide;
		vector<Battlefield> BattleList;
		BattleList.resize(step);
		while (true)
		{
			++loopTimes;
			Team::BattlePanel(BattleList, attacker, defender);
			for (unsigned i = 0; i < BattleList.size(); ++i)
			{
				Battlefield& current = BattleList[i];
				if(current.atkSide == battleSidePtr() || current.defSide == battleSidePtr())continue;
				Json::Value roundJson;
				roundJson[0u] = i;
				roundJson[1u] = reportIdx;
				Json::Value& atkJson = roundJson[2u];
				atkJson = Json::arrayValue;
				atkJson.append(current.atkSide->getSideID());
				atkJson.append(current.atkSide->getName());
				atkJson.append(current.atkSide->isPlayerSide());
				Json::Value& defJson = roundJson[3u];
				defJson = Json::arrayValue;
				defJson.append(current.defSide->getSideID());
				defJson.append(current.defSide->getName());
				defJson.append(current.defSide->isPlayerSide());
				++reportIdx;
				Json::Value SingleReport;
				int res = SingleBattle(current, type, SingleReport);
				//0 胜利动画, 1失败动画, 2满连胜退场
				if(res == 1){
					if(!current.atkSide->SetWinAndGoOn()){
						atkJson.append(2);
						atkJson.append(current.atkSide->getWinNum());
						atkJson.append(current.atkSide->getInsNum());
						current.atkSide = battleSidePtr();
					}else {
						atkJson.append(0);
						atkJson.append(current.atkSide->getWinNum());
						atkJson.append(current.atkSide->getInsNum());
					}
					defJson.append(1);
					defJson.append(current.defSide->getWinNum());
					defJson.append(current.defSide->getInsNum());
					current.defSide = battleSidePtr();
				}else{
					if(!current.defSide->SetWinAndGoOn()){
						defJson.append(2);
						defJson.append(current.defSide->getWinNum());
						defJson.append(current.defSide->getInsNum());
						current.defSide = battleSidePtr();
					}else {
						defJson.append(0);
						defJson.append(current.defSide->getWinNum());
						defJson.append(current.defSide->getInsNum());
					}
					atkJson.append(1);
					atkJson.append(current.atkSide->getWinNum());
					atkJson.append(current.atkSide->getInsNum());
					current.atkSide = battleSidePtr();
				}
				roundJson.append(res);//结果
				TeamJson.append(roundJson);
				BattleReprotJson.append(SingleReport);
			}
			//判断胜负
			Team::fieldValue alive = Team::LookAlive(BattleList);
			if (attacker.empty() && alive.atkValue < 1)
			{
				report[strBattleResult] = 2;
				break;
			}
			if(defender.empty() && alive.defValue < 1)
			{
				report[strBattleResult] = 1;
				break;
			}

			if (loopTimes > 200)break;
		}

		if(loopTimes > 200)
		{
			LogE << "too long loop for team battle !!" << LogEnd;
			string bugPath = reportTeamBugDir + "bug";
			writeToFile(bugPath, report);
			return;
		}

		wirteTeamReport(fields, report);
	}

	void battle_system::packageArmy(battleSidePtr side, Json::Value& armyJson)
	{
		armyVec memVec = side->getMember();
		for (unsigned i = 0; i < memVec.size(); ++i)
		{
			battleMemPtr mem = memVec[i];
			if(mem == battleMemPtr())continue;
			Json::Value army;
			army.append(mem->getIdx());
			army.append(mem->getID());
			army.append(mem->getLv());
			army.append(mem->getHP());
			army.append((int)mem->getTotalChar(idx_hp));
			if(mem->skill_2 != NULL)
				army.append(mem->getEnergy());
			armyJson.append(army);			
		}
	}

	int battle_system::SingleBattle(Battlefield field, const int type, Json::Value& report)
	{
		_ClearOTOBattle();
		report[strBattleType] = type;
		report[strArmyFormation] = Json::arrayValue;
		report[strInfoAtk].append(field.atkSide->getSideID());
		report[strInfoAtk].append(field.atkSide->getFaceID());
		report[strInfoAtk].append(field.atkSide->getName());
		report[strInfoAtk].append(field.atkSide->getPlayerLevel());
		report[strInfoAtk].append(field.atkSide->getBattleValue());
		report[strInfoAtk].append(field.atkSide->isPlayerSide());
		report[strInfoDef].append(field.defSide->getSideID());
		report[strInfoDef].append(field.defSide->getFaceID());
		report[strInfoDef].append(field.defSide->getName());
		report[strInfoDef].append(field.defSide->getPlayerLevel());
		report[strInfoDef].append(field.defSide->getBattleValue());
		report[strInfoDef].append(field.defSide->isPlayerSide());

		//打包部队信息
		field.atkSide->initBattle(battleSide::atk_side);
		Json::Value& armyFmJson = report[strArmyFormation];
		armyFmJson = Json::arrayValue;
		packageArmy(field.atkSide, armyFmJson);
		field.defSide->initBattle(battleSide::def_side);
		packageArmy(field.defSide, armyFmJson);

// #ifdef WIN32
// 		armyVec atkVec = field.atkSide->getAllMember();
// 		Json::Value& attacker = report["攻击方"];
// 		for (unsigned i = 0; i < atkVec.size(); ++i)
// 		{
// 			battleMemPtr mem = atkVec[i];
// 			Json::Value& armyJson = attacker[i];
// 			armyJson["血量"] = mem->getHP();
// 			armyJson["最大血量"] = mem->getMaxHP();
// 			armyJson["武将ID"] = mem->getID();
// 			armyJson["位置"] = mem->getIdx();
// 			armyJson["基础统"] = mem->getBaseAttri(idx_dominance);
// 			armyJson["基础勇"] = mem->getBaseAttri(idx_power);
// 			armyJson["基础智"] = mem->getBaseAttri(idx_intelligence);
// 			armyJson["额外统"] = mem->getAddAttri(idx_dominance);
// 			armyJson["额外勇"] = mem->getAddAttri(idx_power);
// 			armyJson["额外智"] = mem->getAddAttri(idx_intelligence);
// 			armyJson["基础攻击"] = mem->getBaseChar(idx_atk);
// 			armyJson["基础防御"] = mem->getBaseChar(idx_def);
// 			armyJson["基础战攻"] = mem->getBaseChar(idx_war);
// 			armyJson["基础战防"] = mem->getBaseChar(idx_war_def);
// 			armyJson["基础魔攻"] = mem->getBaseChar(idx_magic);
// 			armyJson["基础魔防"] = mem->getBaseChar(idx_magic_def);
// 			armyJson["基础闪避"] = mem->getBaseChar(idx_dodge);
// 			armyJson["基础格挡"] = mem->getBaseChar(idx_block);
// 			armyJson["基础暴击"] = mem->getBaseChar(idx_crit);
// 			armyJson["基础反击"] = mem->getBaseChar(idx_counter);
// 			armyJson["基础物增"] = mem->getBaseChar(idx_phyAddHurt);
// 			armyJson["基础物免"] = mem->getBaseChar(idx_phyCutHurt);
// 			armyJson["基础战增"] = mem->getBaseChar(idx_warAddHurt);
// 			armyJson["基础战免"] = mem->getBaseChar(idx_warCutHurt);
// 			armyJson["基础魔增"] = mem->getBaseChar(idx_magicAddHurt);
// 			armyJson["基础魔免"] = mem->getBaseChar(idx_magicCutHurt);
// 			armyJson["额外攻击"] = mem->getAddChar(idx_atk);
// 			armyJson["额外防御"] = mem->getAddChar(idx_def);
// 			armyJson["额外战攻"] = mem->getAddChar(idx_war);
// 			armyJson["额外战防"] = mem->getAddChar(idx_war_def);
// 			armyJson["额外魔攻"] = mem->getAddChar(idx_magic);
// 			armyJson["额外魔防"] = mem->getAddChar(idx_magic_def);
// 			armyJson["额外闪避"] = mem->getAddChar(idx_dodge);
// 			armyJson["额外格挡"] = mem->getAddChar(idx_block);
// 			armyJson["额外暴击"] = mem->getAddChar(idx_crit);
// 			armyJson["额外反击"] = mem->getAddChar(idx_counter);
// 			armyJson["额外物增"] = mem->getAddChar(idx_phyAddHurt);
// 			armyJson["额外物免"] = mem->getAddChar(idx_phyCutHurt);
// 			armyJson["额外战增"] = mem->getAddChar(idx_warAddHurt);
// 			armyJson["额外战免"] = mem->getAddChar(idx_warCutHurt);
// 			armyJson["额外魔增"] = mem->getAddChar(idx_magicAddHurt);
// 			armyJson["额外魔免"] = mem->getAddChar(idx_magicCutHurt);
// 		}
// 		armyVec defVec = field.defSide->getAllMember();
// 		Json::Value& defender = report["防御方"];
// 		for (unsigned i = 0; i < defVec.size(); ++i)
// 		{
// 			battleMemPtr mem = defVec[i];
// 			Json::Value& armyJson = defender[i];
// 			armyJson["血量"] = mem->getHP();
// 			armyJson["最大血量"] = mem->getMaxHP();
// 			armyJson["武将ID"] = mem->getID();
// 			armyJson["位置"] = mem->getIdx();
// 			armyJson["基础统"] = mem->getBaseAttri(idx_dominance);
// 			armyJson["基础勇"] = mem->getBaseAttri(idx_power);
// 			armyJson["基础智"] = mem->getBaseAttri(idx_intelligence);
// 			armyJson["额外统"] = mem->getAddAttri(idx_dominance);
// 			armyJson["额外勇"] = mem->getAddAttri(idx_power);
// 			armyJson["额外智"] = mem->getAddAttri(idx_intelligence);
// 			armyJson["基础攻击"] = mem->getBaseChar(idx_atk);
// 			armyJson["基础防御"] = mem->getBaseChar(idx_def);
// 			armyJson["基础战攻"] = mem->getBaseChar(idx_war);
// 			armyJson["基础战防"] = mem->getBaseChar(idx_war_def);
// 			armyJson["基础魔攻"] = mem->getBaseChar(idx_magic);
// 			armyJson["基础魔防"] = mem->getBaseChar(idx_magic_def);
// 			armyJson["基础闪避"] = mem->getBaseChar(idx_dodge);
// 			armyJson["基础格挡"] = mem->getBaseChar(idx_block);
// 			armyJson["基础暴击"] = mem->getBaseChar(idx_crit);
// 			armyJson["基础反击"] = mem->getBaseChar(idx_counter);
// 			armyJson["基础物增"] = mem->getBaseChar(idx_phyAddHurt);
// 			armyJson["基础物免"] = mem->getBaseChar(idx_phyCutHurt);
// 			armyJson["基础战增"] = mem->getBaseChar(idx_warAddHurt);
// 			armyJson["基础战免"] = mem->getBaseChar(idx_warCutHurt);
// 			armyJson["基础魔增"] = mem->getBaseChar(idx_magicAddHurt);
// 			armyJson["基础魔免"] = mem->getBaseChar(idx_magicCutHurt);
// 			armyJson["额外攻击"] = mem->getAddChar(idx_atk);
// 			armyJson["额外防御"] = mem->getAddChar(idx_def);
// 			armyJson["额外战攻"] = mem->getAddChar(idx_war);
// 			armyJson["额外战防"] = mem->getAddChar(idx_war_def);
// 			armyJson["额外魔攻"] = mem->getAddChar(idx_magic);
// 			armyJson["额外魔防"] = mem->getAddChar(idx_magic_def);
// 			armyJson["额外闪避"] = mem->getAddChar(idx_dodge);
// 			armyJson["额外格挡"] = mem->getAddChar(idx_block);
// 			armyJson["额外暴击"] = mem->getAddChar(idx_crit);
// 			armyJson["额外反击"] = mem->getAddChar(idx_counter);
// 			armyJson["额外物增"] = mem->getAddChar(idx_phyAddHurt);
// 			armyJson["额外物免"] = mem->getAddChar(idx_phyCutHurt);
// 			armyJson["额外战增"] = mem->getAddChar(idx_warAddHurt);
// 			armyJson["额外战免"] = mem->getAddChar(idx_warCutHurt);
// 			armyJson["额外魔增"] = mem->getAddChar(idx_magicAddHurt);
// 			armyJson["额外魔免"] = mem->getAddChar(idx_magicCutHurt);
// 		}
// #endif

		int battle_res = 0;//和局
		do{
			report[strBigRound] = Json::arrayValue;
			if(field.atkSide->aliveNum() <= 0){
				battle_res = 2;//defender win
				break;
			}
			if(field.defSide->aliveNum() <= 0){
				battle_res = 1;//attacker win
				break;
			}			

			for (unsigned i = 0; i < _BigRound; ++i)
			{
				bool b = false;
				field.atkSide->resetLoop();
				field.defSide->resetLoop();
				currentRound = i + 1;
				Json::Value& cBR = report[strBigRound][i];
				cBR = Json::arrayValue;
				for (unsigned round = 0; round < _LittleRound; ++round)
				{
					currentAtkSide = field.atkSide;
					currentDefSide = field.defSide;
					int res = attack(cBR); 
					if(res != 0){
						b = true;
						battle_res = res;
						break;
					}

					currentAtkSide = field.defSide;
					currentDefSide = field.atkSide;
					res = attack(cBR);
					if(res != 0){
						b = true;
						if(res == 1)battle_res = 2;
						else if(res == 2)	battle_res = 1;
						break;
					}
				}
				if(b) break;
			}
		}while (false);

		report[strBackGround] = -1;
		if(field.ParamJson[strBackGround].isInt())report[strBackGround] = field.ParamJson[strBackGround].asInt();
		report[strBattleResult] = battle_res;
		if(1 == battle_res)report[strBattleStar] =  field.atkSide->getStar();
		else report[strBattleStar] =  field.defSide->getStar();
		_ClearOTOBattle();
		return battle_res;
	}

	void battle_system::_ClearOTOBattle()
	{
		currentAtkSide = battleSidePtr();
		currentDefSide = battleSidePtr();
		currentAtk = battleMemPtr();
		currentDef = battleMemPtr();
		appointTarget = battleMemPtr();
		currentSkill = Params::VarPtr();
		useSkillEnergy = false;
		isCounter = false;
		currentRound = 1;
	}

	int battle_system::_Battle(Battlefield field, const int type)
	{
		Json::Value report;
		int res = SingleBattle(field, type, report);
		writeReport(field, report);
		return res;
	}

	bool battle_system::isGoOn(battleMemPtr atk, Json::Value& report)
	{
		bool isContinue = true;
		if(atk == battleMemPtr() || atk->isDead() || 
			atk->hasBuff(lock_buff))isContinue = false;
		if( atk != battleMemPtr() )atk->runPreBuff(report);

		return isContinue;
	}

	int battle_system::attack(Json::Value& report)
	{	
		int res = 0;
		Json::Value loopJson = Json::arrayValue;
		do 
		{
			currentAtk = currentAtkSide->getNext();
			if(currentAtk == NULL) break;			
			if(! isGoOn(currentAtk, loopJson)) break;
			res = pick_skill(loopJson);
		} while (false);
		if(! loopJson.empty())report.append(loopJson);
		return res;
	}

	int battle_system::pick_skill(Json::Value& report)
	{
		currentSkill = currentAtk->skill_1;
		useSkillEnergy = false;
		if(currentAtk->getEnergy() >= 100)
		{
			useSkillEnergy = true;
			currentSkill = currentAtk->skill_2;
		}
		return use_skill(report);
	}

	int battle_system::use_skill(Json::Value& report)
	{
		battleMemPtr counter = effect_skill(report);
		if(!isCounter && currentAtkSide->aliveNum() <= 0)return 2;
		if(isCounter && currentDefSide->aliveNum() <= 0)return 2;
		if(!isCounter && currentDefSide->aliveNum() <= 0)return 1;
		if(isCounter && currentAtkSide->aliveNum() <= 0)return 1;
		
		//判断反击
		int res = 0;
		if(!isCounter && counter != NULL &&  ! counter->hasBuff(lock_buff) &&
			!currentAtk->isDead() && !counter->isDead()){
				double ctRate = counter->getCounterRate(currentAtk);
				if(commom_sys.randomOk(ctRate))
				{
					isCounter = true;
					std::swap(currentAtkSide, currentDefSide);
					appointTarget = currentAtk;
					currentAtk = counter;
					res = pick_skill(report);
				}
		}
		if(isCounter)
		{
			isCounter = false;
			appointTarget = battleMemPtr();
		}
		return res;
	}

	battleMemPtr battle_system::effect_skill(Json::Value& report)
	{
		battleMemPtr counter = battleMemPtr();
		if(currentSkill == NULL)
		{
			LogW << "current skill failed ... " << __FUNCTION__ << LogEnd;
			if(currentAtk != NULL)
			{
				LogW << "pilot ID is ..." << currentAtk->getID() << LogEnd;
			}
			return counter;
		}
		Params::Var& skill = *currentSkill.get();
		if(skill[skillDoStr].getType() != Params::Array)
		{
			return counter;
		}

		Json::Value atkDoJson;
		atkDoJson.append(currentAtk->getIdx());
		if(isCounter)atkDoJson.append(battle_action_use_skill_by_counter);
		else atkDoJson.append(battle_action_use_skill);
		atkDoJson.append(skill[skillIDStr].asInt());	
		Json::Value skillDo = Json::arrayValue;
		for (unsigned i = 0; i < skill[skillDoStr].getSize(); ++i)
		{
			criTag_ = false;
			Params::Var& skillPart = skill[skillDoStr][i];
			getTarget(skillPart, currentAtk, currentAtkSide, currentDefSide, appointTarget);
			run_skill(skillPart, skillDo);
			if(currentAtkSide->aliveNum() <= 0 || currentDefSide->aliveNum() <= 0)break;
			if(skillPart[skiilMainTargetStr].asInt() > 0){
				battleMemPtr mem = currentAtk->getMainTarget();
				if(mem != battleMemPtr() && mem->beenBalance)counter = mem;
			}
		}
		atkDoJson.append(skillDo);
		report.append(atkDoJson);
		if(useSkillEnergy)currentAtk->setEnergy(0, report);
		currentAtk->runPostBuff(report);
		return counter;
	}

	void battle_system::run_skill(Params::Var& skillPart, Json::Value& report)
	{
		Json::Value dealJson = Json::arrayValue;
		dealJson[0u] = Json::arrayValue;
		dealJson[1u] = Json::arrayValue;
		dealJson[2u] = Json::arrayValue;
		int idx = currentAtk->getIdx();
		currentAtk->packagetTargetIdx(dealJson[0u]);

		do{
			if(!skillPart.isMember(skillActionStr) || skillPart[skillActionStr].getType() != Params::Array)break;
			Params::Var& skillEf = skillPart[skillActionStr];
			do 
			{
				currentDef = currentAtk->getCurrentTarget();
				if(currentDef == battleMemPtr())break;
				bool is_continue = false;
				bool is_break = false;
				for (unsigned i = 0; i  < skillEf.getSize(); ++i)
				{
					Params::Var& sk = skillEf[i];
					int judgeType = sk[skillJudgeTypeStr].asInt();
					int breakLevel = sk[skillBreakLevelTypeStr].asInt(); 
					if((breakLevel & reset_continue)  == reset_continue)is_continue= false;
					if((breakLevel & reset_break)  == reset_break)is_break= false;
					if(is_continue)continue;
					int targetInclude = sk[skillEfTargetIncludeStr].asInt();
					if(currentAtk->isMainTarget(currentDef) && targetInclude == target_include_only_adds)	continue;
					if(!currentAtk->isMainTarget(currentDef) && targetInclude == target_include_only_main)continue;
					int runType = sk[skillEfRunTypeStr].asInt();
					DamageFMap::iterator it = _DFMap.find(runType);
					if(it == _DFMap.end())continue;
					int runRes = it->second(sk, dealJson[1u]);
					if(runRes > never_break && runRes < reset_continue && runRes == (breakLevel & runRes))is_continue = true;
					if(runRes > never_break && runRes < reset_continue && runRes == (judgeType & runRes))	is_break = true;		
				}
				if(currentAtk->isMainTarget(currentDef) && is_break)break;
				currentAtk->getNextTarget();
			} while (true);
			currentAtkSide->dealDeadAndPackage(dealJson[2u]);
			currentDefSide->dealDeadAndPackage(dealJson[2u]);
		}while (false);
		dealJson[3u] = criTag_;
		if(!dealJson[1u].empty() || !dealJson[2u].empty())report.append(dealJson);
	}

	battleMemPtr battle_system::singleTarget(battleMemPtr atk, battleSidePtr targetSide, const int type)
	{
		FormatRule(atk->getCoor().second, Rule);
		battleMemPtr mem = battleMemPtr();
		bool alive = type & MO::mem_alive;
		bool energy = type & MO::mem_energy;
		bool nfenergy = type & MO::mem_nfenetgy;
		for(int i = 0; i < 3; ++i){
			bool isBreak = false;
			for (int j = 0; j < 3; ++j)	{
				battleMemPtr g = targetSide->getMember(pair<int, int>(j, Rule[i]));
				if(g == battleMemPtr())continue;
				if(alive && g->isDead())continue;
				if(energy && g->skill_2 == NULL)continue;
				if(nfenergy && (g->skill_2 == NULL || g->getEnergy() >= 100))continue;
				mem = g;
				isBreak = true;
				break;
			}
			if(isBreak)break;
		}
		return mem;
	}

	battleMemPtr battle_system::singletargetOPP(battleMemPtr atk, battleSidePtr targetSide, const int type)
	{
		FormatRule(atk->getCoor().second, Rule);
		battleMemPtr mem = battleMemPtr();
		bool alive = type & MO::mem_alive;
		bool energy = type & MO::mem_energy;
		bool nfenergy = type & MO::mem_nfenetgy;
		for(int i = 0; i < 3; ++i){
			bool isBreak = false;
			for (int j = 2; j >= 0; --j){
				battleMemPtr g = targetSide->getMember(pair<int, int>(j, Rule[i]));
				if(g == battleMemPtr())continue;
				if(alive && g->isDead())continue;
				if(energy && g->skill_2 == NULL)continue;
				if(nfenergy && (g->skill_2 == NULL || g->getEnergy() >= 100))continue;
				mem = g;
				isBreak = true;
				break;
			}
			if(isBreak)break;
		}
		return mem;
	}

#define SafePushTarget(X, Y, tSide, vec)\
	{\
		battleMemPtr mem = tSide->getMember(pair<int, int>(X, Y));\
		if(mem != battleMemPtr()) vec.push_back(mem);\
	}\

	void battle_system::getSingleTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk)
	{
		armyVec vec;
		int beginX = mainTarget->getCoor().first;
		int beginY = mainTarget->getCoor().second;
		SafePushTarget(beginX, beginY, targetSide, vec);
		atk->convertTarget(vec);
	}

	void battle_system::getTTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk)
	{
		armyVec vec;
		int beginX = mainTarget->getCoor().first;
		int beginY = mainTarget->getCoor().second;
		SafePushTarget(beginX, beginY, targetSide, vec);
		SafePushTarget(beginX - 1, beginY, targetSide, vec);
		SafePushTarget(beginX, beginY + 1, targetSide, vec);
		SafePushTarget(beginX + 1, beginY, targetSide, vec);
		SafePushTarget(beginX, beginY - 1, targetSide, vec);
		atk->convertTarget(vec);
	}
	
	void battle_system::getLineTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk)
	{
		armyVec vec;
		int beginX = mainTarget->getCoor().first;
		int beginY = mainTarget->getCoor().second;
		SafePushTarget(beginX, beginY, targetSide, vec);
		SafePushTarget(beginX, beginY+1, targetSide, vec);
		SafePushTarget(beginX, beginY+2, targetSide, vec);
		SafePushTarget(beginX, beginY-1, targetSide, vec);
		SafePushTarget(beginX, beginY-2, targetSide, vec);
		atk->convertTarget(vec);
	}

	void battle_system::getRowTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk)
	{
		armyVec vec;
		int beginX = mainTarget->getCoor().first;
		int beginY = mainTarget->getCoor().second;
		SafePushTarget(0, beginY, targetSide, vec);
		SafePushTarget(1, beginY, targetSide, vec);
		SafePushTarget(2, beginY, targetSide, vec);
		atk->convertTarget(vec);
	}

	void battle_system::getAllTarget(battleMemPtr mainTarget, battleSidePtr targetSide, battleMemPtr atk, const int Type, const bool selfin /* = true */)
	{
		armyVec army;
		army.push_back(mainTarget);
		armyVec vec = targetSide->getMember(Type);
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			if(mainTarget == vec[i])continue;
			army.push_back(vec[i]);
		}
		if(!selfin)
		{
			for (unsigned i = 0; i < army.size(); ++i)
			{
				if(atk == army[i])
				{
					army.erase(army.begin() + i);
					break;
				}
			}
		}
		atk->convertTarget(army);
	}

	void battle_system::getRandom(battleSidePtr targetSide, battleMemPtr atk, int signNum, const int Type)
	{
		int aliveMem = targetSide->aliveNum();
		if(signNum < 1 || aliveMem <= 0) return;
		if(signNum > aliveMem) signNum = aliveMem;
		armyVec vec = targetSide->getMember(Type);
		random_shuffle(vec.begin(), vec.end());
		armyVec targets;
		for (unsigned i = 0; i < (unsigned)signNum && i < vec.size(); ++ i)
		{
			targets.push_back(vec[i]);
		}
		atk->convertTarget(targets);
	}

	void battle_system::getTarget(Params::Var& skill, battleMemPtr atk, battleSidePtr atkArmy, battleSidePtr defarmy, battleMemPtr def /* = battleMemPtr() */)
	{
		int targetType = skill[skillTargetStr].asInt();
		battleMemPtr vec = atk;
		if(targetType == target_self)
		{
			armyVec tmp;
			if(def != NULL)
			{
				battleMemPtr ptr = defarmy->getMember(def->getCoor());
				if(ptr != NULL)tmp.push_back(ptr);
			}
			vec->convertTarget(tmp);
			return; 
		}
		if(targetType == target_self + TargetSelfOffset)
		{
			armyVec tmp; 
			if(atk != NULL)
			{
				battleMemPtr ptr = atkArmy->getMember(atk->getCoor());
				if(ptr != NULL)tmp.push_back(ptr);
			}
			vec->convertTarget(tmp);
			return; 
		}

		int signNum = skill[skillSignTargetStr].asInt();
		battleSidePtr targetSide = defarmy;
		if(targetType >= TargetSelfOffset){
			targetType -= TargetSelfOffset;
			targetSide = atkArmy;
		}
		
		if(targetType > target_other_dead_line){
			int tp = MO::mem_alive;
			if(targetType == target_random_energy)tp |= MO::mem_energy;
			if(targetType == target_random_not_full_energy) tp |= MO::mem_nfenetgy;
			getRandom(targetSide, vec, signNum, tp);
			return;
		}

		battleMemPtr mainTarget = def;
		if(mainTarget != battleMemPtr())
		{
			bool ret = targetSide->isMember(mainTarget->getIdx());
			bool dead = mainTarget->isDead();
			if(!ret || dead) mainTarget = battleMemPtr();
		}
		if(mainTarget == battleMemPtr()){
			int tp = MO::mem_alive;
			if(targetType == target_energy_all || targetType == target_energy_all_except_self)tp |= MO::mem_energy;
			if(targetType == target_energy_all_not_full || targetType == target_energy_all_not_full_except_self)tp |= MO::mem_nfenetgy;
			if(targetType < target_opp_dead_line)
				mainTarget = singleTarget(atk, targetSide, tp);
			else if(targetType < target_other_dead_line)
				mainTarget = singletargetOPP(atk, targetSide, tp);
			armyVec tmpNull;
			if(mainTarget == battleMemPtr()){
				vec->convertTarget(tmpNull);
				return;
			}
		}
		if(targetType == target_single || targetType == target_opp_single)
			getSingleTarget(mainTarget, targetSide, vec);
		else if(targetType == target_line || targetType == target_opp_line)
			getLineTarget(mainTarget, targetSide, vec);
		else if(targetType == target_row)
			getRowTarget(mainTarget, targetSide, vec);
		else if(targetType == target_t)
			getTTarget(mainTarget, targetSide, vec);
		else if(targetType == target_all/* || targetType == target_all_opp*/)
			getAllTarget(mainTarget, targetSide, vec, MO::mem_alive);
		else if(targetType == target_energy_all)
			getAllTarget(mainTarget, targetSide, vec, MO::mem_alive | MO::mem_energy);
		else if(targetType == target_energy_all_except_self)
			getAllTarget(mainTarget, targetSide, vec, MO::mem_alive | MO::mem_energy, false);
		else if(targetType == target_energy_all_not_full/* || targetType == target_energy_all_not_full_opp*/)
			getAllTarget(mainTarget, targetSide, vec, MO::mem_alive | MO::mem_nfenetgy);
		else if(targetType == target_energy_all_not_full_except_self)
			getAllTarget(mainTarget, targetSide, vec, MO::mem_alive | MO::mem_nfenetgy, false);
	}

	double battle_system::getBuffValue(int buff_type, vector<battleBuff>& buffs)
	{
		for (unsigned i = 0; i < buffs.size(); ++i){
			if(buffs[i].buffType == buff_type)
				return buffs[i].effectValue;
		}
		return 0.0;
	}

	void battle_system::CalculateAtk2Def(Params::Var& sk, battleMemPtr atk, const int round, bool& isCri, bool isSelf /* = false */)
	{
		battleMemPtr target;
		if(isSelf) target = atk;
		else target = atk->getCurrentTarget();
		if(target == battleMemPtr())return;
		double talent = (1 + 
			(atk->getTotalAttri(idx_dominance) - target->getTotalAttri(idx_dominance))
			* 0.0045 + (atk->getBaseAttri(idx_dominance) - target->getBaseAttri(idx_dominance))*0.0025);
		talent = talent < 0.1 ? 0.1 : talent;

		double atrr_diff = atk->getTotalChar(idx_atk) * 0.5 - 
			target->getTotalChar(idx_def) * 0.5 + 50;
		if(atrr_diff < 0)
			atrr_diff = (atk->getTotalChar(idx_atk) + 50) * 50.0 / 
			(target->getBaseChar(idx_def) +  target->getAddChar(idx_def) * 0.5 + 50.0);

		double arms_diff = atk->getArmsModule(idx_atkModule) - target->getArmsModule(idx_defModule);
		arms_diff =  arms_diff < 0.0 ? 0.0 : arms_diff;
		double rm_percent = (0.5 + 0.5 * (double)atk->getHP() / atk->getTotalChar(idx_hp));
		double round_eff = (round - 1)*0.03 + 1;
		double reduce_damage = 1 - target->getTotalChar(idx_phyCutHurt);
		reduce_damage = reduce_damage < 0.0 ? 0.0 : reduce_damage;
		double add_damage = 1 + atk->getTotalChar(idx_phyAddHurt);
		double times = 1.0;
		double cri_rate = atk->getCriticalRate(target);
		if(commom_sys.randomOk(cri_rate)){
			times = 1.5;
			isCri = true;
		}
		double real_damage_rate = 1.0;
		double get_rate = target->getBuffModule(reduce_phy_buff);
		real_damage_rate = real_damage_rate - get_rate < 0.0 ? 0.0 : real_damage_rate - get_rate; 

		//兵种相克
		double armsR = atk->getArmsR(target); 

		int total_damage = (int)(pow(talent * atrr_diff, arms_diff) * rm_percent * round_eff * 
			reduce_damage * add_damage * times * 
			real_damage_rate * armsR);
		int real_damage = target->alterHP(-total_damage);

		target->totalDamage += abs(real_damage);
		target->beenBalance = true;
	}

	void battle_system::CalculateWar2WarDef(Params::Var& sk, battleMemPtr atk, const int round, bool& isCri, bool isSelf /* = false */)
	{
		battleMemPtr target;
		if(isSelf) target = atk;
		else target = atk->getCurrentTarget();
		if(target == battleMemPtr())return;
		double talent = (1 + 
			(atk->getTotalAttri(idx_power) - target->getTotalAttri(idx_power))
			* 0.0045 + (atk->getBaseAttri(idx_power) - target->getBaseAttri(idx_power))*0.0025);
		talent = talent < 0.1 ? 0.1 : talent;

		double atrr_diff = atk->getTotalChar(idx_war) * 0.5 - 
			target->getTotalChar(idx_war_def) * 0.5 + 50;
		if(atrr_diff < 0)
			atrr_diff = (atk->getTotalChar(idx_war) + 50) * 50.0 / 
			(target->getBaseChar(idx_war_def) +  target->getAddChar(idx_war_def) * 0.5 + 50.0);

		double arms_diff = atk->getArmsModule(idx_warModule) - target->getArmsModule(idx_warDefModule);
		arms_diff =  arms_diff < 0.0 ? 0.0 : arms_diff;
		double rm_percent = (0.5 + 0.5 * (double)atk->getHP() / atk->getTotalChar(idx_hp));
		double round_eff = (round - 1)*0.03 + 1;
		double reduce_damage = 1 - target->getTotalChar(idx_warCutHurt);
		reduce_damage = reduce_damage < 0.0 ? 0.0 : reduce_damage;
		double skill_modules = sk[skillEfWarModuleStr].asDouble();
		if(sk.isMember(skillDecayStr))skill_modules *= sk[skillDecayStr][atk->getLv()].asDouble();
		if(skill_modules < 0.0)skill_modules = 1.0;
		double add_damage = 1 + atk->getTotalChar(idx_warAddHurt);
		double times = 1.0;
		double cri_rate = atk->getCriticalRate(target);
		if(commom_sys.randomOk(cri_rate)){
			times = 1.5;
			isCri = true;
		}
		double backHitModule = 1.0;
		if (sk.isMember(selfModuleStr))
		{
			backHitModule = (1.0 + (1.0 - (double)atk->getHP() / atk->getTotalChar(idx_hp)) * (sk[selfModuleStr].asDouble() * 100));
		}
		double  energyModule = 1.0;
		if(sk[skillUseEnergyStr].asInt() > 0)
			energyModule = 1.0 + (double)(atk->getEnergy() - 100)*0.004;
		double real_damage_rate = 1.0;
		double get_rate = target->getBuffModule(reduce_war_buff);
		real_damage_rate = real_damage_rate - get_rate < 0.0 ? 0.0 : real_damage_rate - get_rate; 

		//兵种相克
		double armsR = atk->getArmsR(target); 

		int total_damage = (int)(pow(talent * atrr_diff, arms_diff) * rm_percent 
			* round_eff * reduce_damage * skill_modules * add_damage * times 
			* real_damage_rate * backHitModule * armsR * energyModule);
		int real_damage = target->alterHP(-total_damage);

		target->totalDamage += abs(real_damage);
		target->beenBalance = true;
	}


	void battle_system::CalculateMagic2MagicDef(Params::Var& sk, battleMemPtr atk, const int round, bool& isCri, bool isSelf /* = false */)
	{
		battleMemPtr target;
		if(isSelf) target = atk;
		else target = atk->getCurrentTarget();
		if(target == battleMemPtr())return;
		double talent = (1 + 
			(atk->getTotalAttri(idx_intelligence) - target->getTotalAttri(idx_intelligence))
			* 0.0045 + (atk->getBaseAttri(idx_intelligence) - target->getBaseAttri(idx_intelligence))*0.0025);
		talent = talent < 0.1 ? 0.1 : talent;

		double atrr_diff = atk->getTotalChar(idx_magic) * 0.5 - 
			target->getTotalChar(idx_magic_def) * 0.5 + 50;
		if(atrr_diff < 0)
			atrr_diff = (atk->getTotalChar(idx_magic) + 50) * 50.0 / 
			(target->getBaseChar(idx_magic_def) +  target->getAddChar(idx_magic_def) * 0.5 + 50.0);

		double arms_diff = atk->getArmsModule(idx_magicModule) - target->getArmsModule(idx_magicDefModule);
		arms_diff =  arms_diff < 0.0 ? 0.0 : arms_diff;
		double rm_percent = (0.5 + 0.5 * (double)atk->getHP() / atk->getTotalChar(idx_hp));
		double round_eff = (round - 1)*0.03 + 1;
		double reduce_damage = 1 - target->getTotalChar(idx_magicCutHurt);
		reduce_damage = reduce_damage < 0.0 ? 0.0 : reduce_damage;
		double add_damage = 1 + atk->getTotalChar(idx_magicAddHurt);
		double times = 1.0;
// 		double cri_rate = atk->getCriticalRate(target);
// 		if(commom_sys.randomOk(cri_rate)){
// 			times = 1.5;
// 			isCri = true;
// 		}
		double real_damage_rate = 1.0;
		double get_rate = target->getBuffModule(reduce_magic_buff);;
		real_damage_rate = real_damage_rate - get_rate < 0 ? 0.0 : real_damage_rate - get_rate; 

		//兵种相克
		double armsR = atk->getArmsR(target); 

		int total_damage = (int)(pow(talent * atrr_diff, arms_diff) * rm_percent * round_eff * 
			reduce_damage * add_damage * times * armsR * real_damage_rate);
		int real_damage = target->alterHP(-total_damage);

		target->totalDamage += abs(real_damage);
		target->beenBalance = true;
	}

	int battle_system::magic2magicdef(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		if(currentDef == battleMemPtr())return res;
		Json::Value cDo;
		do 
		{
			cDo.append(currentDef->getIdx());

			double rate = sk[skillEfRunRateStr].asDouble();
			if(! commom_sys.randomOk(rate)){
				res = break_by_run_failed;
				cDo.append(battle_action_skill_not_success);
				break;
			}
			//先检查是否本身就有buff
			if(currentDef->hasBuff(defend_magic_buff)){
				res = break_by_buff;//buff拦截
				currentDef->alterHP(-1);
				cDo.append(battle_action_buff_resist);
				cDo.append(currentDef->getBuffVal(defend_magic_buff));
				cDo.append(currentDef->getHP());
				break;
			}

			bool isCri = false;
			int before = currentDef->getHP();
			CalculateMagic2MagicDef(sk,currentAtk, currentRound, isCri);
			cDo.append(battle_action_reduce_hp);
			int now = currentDef->getHP();
			cDo.append(now);
			cDo.append(before - now);
			if(isCri) cDo.append(1);
			CRITAG(isCri);
		} while (false);
		if(! cDo.empty() && ! cDo.isNull())report.append(cDo);
		return res;
	}

	int battle_system::war2wardef(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		if(currentDef == battleMemPtr())return res;
		Json::Value cDo;
		do 
		{
			cDo.append(currentDef->getIdx());
			if(currentAtk->isMainTarget(currentDef))
			{
				if(! currentDef->hasBuff(lock_buff))
				{
					bool isDodge = commom_sys.randomOk(currentDef->getDodgeRate(currentAtk));
					if(isDodge){
						res = break_by_dodge;// 闪避
						cDo.append(battle_action_self_resist);
						cDo.append(break_by_dodge);
						break;
					}

					if(sk[skillCanBlockStr].asInt() > 0){
						bool isBlock = commom_sys.randomOk(currentDef->getBlockRate(currentAtk));
						if(isBlock){
							//						target->alterHP(-1);
							res = break_by_block;//格挡
							Json::Value resistJson;
							resistJson.append(currentDef->getIdx());
							resistJson.append(battle_action_self_resist);
							resistJson.append(break_by_block);
							report.append(resistJson);

							{
								bool isCri = false;
								int before = currentAtk->getHP();
								CalculateWar2WarDef(sk, currentAtk, currentRound, isCri, true);
								cDo[0u] = currentAtk->getIdx();
								cDo.append(battle_action_reduce_hp);
								int now = currentAtk->getHP();
								cDo.append(now);
								cDo.append(before - now);
								if(isCri) cDo.append(1);
							}
							break;
						}
					}
				}
			}

			if(currentDef->hasBuff(defend_war_buff)){
				res = break_by_buff;
				currentDef->alterHP(-1);
				cDo.append(battle_action_buff_resist);
				cDo.append(currentDef->getBuffVal(defend_war_buff));
				cDo.append(currentDef->getHP());
				cDo.append(1);
				break;
			}

			{
				bool isCri = false;
				int before = currentDef->getHP();
				CalculateWar2WarDef(sk,currentAtk, currentRound, isCri);
				cDo.append(battle_action_reduce_hp);
				int now = currentDef->getHP();
				cDo.append(now);
				cDo.append(before - now);
				if(isCri) cDo.append(1);
				CRITAG(isCri);
			}
		} while (false);
		if(! cDo.empty() && ! cDo.isNull())report.append(cDo);
		if(res != break_by_dodge && res != break_by_block && sk[skillGetEnergyStr].asInt() > 0 && currentAtk->isMainTarget(currentDef)){
			currentAtk->alterEnergy(_AddEnergy,report);
			currentDef->alterEnergy(_AddEnergy, report);
		}
		return res;
	}

	int battle_system::alterEnergy(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		do 
		{
			if(currentDef == battleMemPtr())break;
			double rate = sk[skillEfRunRateStr].asDouble();
			if(! commom_sys.randomOk(rate)){
				res = break_by_run_failed;
				Json::Value failJson;
				failJson.append(currentDef->getIdx());
				failJson.append(battle_action_skill_not_success);
				report.append(failJson);
				break;
			}

			int alterValue = sk[energyStr].asInt();
			battleMemPtr targetVec = battleMember::CopyForTargetVec(currentAtk);
			if(targetVec == battleMemPtr())break;
			getTarget(sk, targetVec, currentAtkSide, currentDefSide, currentDef);
			armyVec vec = targetVec->getAllTarget();
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				battleMemPtr mem = vec[i];
				if(mem == battleMemPtr())continue;
				if(alterValue < 0) mem->alterEnergy(alterValue, report, battle_action_energy_special_reduce);
				else mem->alterEnergy(alterValue, report, battle_action_energy_special_add);
			}
		} while (false);
		return res;
	}

	int battle_system::setEnergy(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		Json::Value current_do;
		do 
		{
			if(currentDef == battleMemPtr())break;
			double rate = sk[skillEfRunRateStr].asDouble();
			if(! commom_sys.randomOk(rate)){
				res = break_by_run_failed;
				Json::Value failJson;
				failJson.append(currentDef->getIdx());
				failJson.append(battle_action_skill_not_success);
				report.append(failJson);
				break;
			}

			int alterValue = sk[energyStr].asInt();
			battleMemPtr targetVec = battleMember::CopyForTargetVec(currentAtk);
			if(targetVec == battleMemPtr())break;
			getTarget(sk, targetVec, currentAtkSide, currentDefSide, currentDef);
			armyVec vec = targetVec->getAllTarget();
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				battleMemPtr mem = vec[i];
				if(mem == NULL)continue;
				//if(mem == battleMemPtr())continue;
				//if(alterValue < mem->getEnergy())continue;
				mem->setEnergy(alterValue, report, battle_action_energy_special_effect);
			}
		} while (false);
		return res;
	}

	int battle_system::reduceCurrentHPRate(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		do 
		{
			if(currentDef == battleMemPtr())break;
			double runRate = sk[skillEfRunRateStr].asDouble();
			if(!commom_sys.randomOk(runRate)){
				res = break_by_run_failed;
				Json::Value failJson;
				failJson.append(currentDef->getIdx());
				failJson.append(battle_action_skill_not_success);
				report.append(failJson);
				break;
			}
			double selfRate = sk[selfModuleStr].asDouble();
			if(selfRate < 0.0 || selfRate > 1.0)break;
			battleMemPtr  targetVec = battleMember::CopyForTargetVec(currentAtk);
			getTarget(sk, targetVec, currentAtkSide, currentDefSide, currentDef);
			armyVec vec = targetVec->getAllTarget();
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				battleMemPtr mem = vec[i];
				if(mem == battleMemPtr())continue;
				int value = (int)(mem->getHP() *  selfRate);
				mem->alterHP(-value);
				Json::Value dataJson;
				dataJson.append(mem->getIdx());
				dataJson.append(battle_action_reduce_hp);
				dataJson.append(mem->getHP());
				dataJson.append(value);
				report.append(dataJson);
			}
		} while (false);
		return res;
	}

	int battle_system::doingRate(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		if(!commom_sys.randomOk(sk[skillEfRunRateStr].asDouble()))res = break_by_run_failed;
		return res;
	}

	int battle_system::absorbEnergy(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		Json::Value cDo;
		do 
		{
			battleMemPtr target = currentAtk->getCurrentTarget();
			if(target == battleMemPtr() || target->skill_2 == NULL)break;
			if( currentAtk->skill_2 == NULL )break;
			double val = currentAtk->getBuffModule(reconver_backup_energy);
			val += (double)target->getEnergy();
			currentAtk->addPostBuff(reconver_backup_energy, -1, 1, val);
			target->setEnergy(0 ,report, battle_action_energy_special_reduce);
		} while (0);
		return res;
	}

	int battle_system::exchangeHPFromCurrentDamage(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		Json::Value cDO;
		do 
		{
			if(currentDef == battleMemPtr())break;

			double selfModule = sk[selfModuleStr].asDouble();
			//double rateTarget = sk[rateTargetStr].asDouble();
			double fixedValue = sk[fixedValueStr].asInt();

			cDO.append(currentAtk->getIdx());
			cDO.append(battle_action_add_hp_exchange);
			int exchangeHP = (int)(currentDef->totalDamage * selfModule + fixedValue);
			exchangeHP = currentAtk->alterHP(exchangeHP);
			cDO.append(currentAtk->getHP());
			cDO.append(exchangeHP);

		} while (0);
		if(! cDO.empty() && ! cDO.isNull())report.append(cDO);
		return res;
	}

	int battle_system::setBuff(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		do 
		{
			double rate = sk[skillEfRunRateStr].asDouble();
			if(! commom_sys.randomOk(rate)){
				res = break_by_run_failed;
				break;
			}
			int buffID = sk[buffIDStr].asInt();
			int buffVal = sk[buffValStr].asInt();
			if(buffID < 0 || buffID >= end_buff)break;
			int round = sk[lastRoundStr].asInt();
			double selfModule = sk[selfModuleStr].asDouble();
			//计算module
			if(currentDef == battleMemPtr())break;
			battleMemPtr  targetVec = battleMember::CopyForTargetVec(currentAtk);
			getTarget(sk, targetVec, currentAtkSide, currentDefSide, currentDef);
			armyVec vec = targetVec->getAllTarget();
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				battleMemPtr mem = vec[i];
				if(mem == battleMemPtr())continue;
				if (mem->isResist(buffID))
				{
					Json::Value buffJson;
					buffJson.append(mem->getIdx());
					buffJson.append(battle_action_buff_effect_resist);
					buffJson.append(buffID);
					report.append(buffJson);
					continue;//免疫
				}
				mem->addPreBuff(report, buffID, buffVal, round, selfModule);
			}
		} while (false);
		return res;
	}

	int battle_system::setSelfBackUpEnergy(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		if(currentAtk->skill_2 == NULL)return res;
		currentAtk->addPostBuff(reconver_backup_energy, -1, 1, sk[energyStr].asDouble());
		return res;
	}

	int battle_system::magicAddHP(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		Json::Value cDo;
		do 
		{
			if(currentDef == battleMemPtr())break;
			cDo.append(currentDef->getIdx());
			double rate = sk[skillEfRunRateStr].asDouble();
			if(! commom_sys.randomOk(rate)){
				res = break_by_run_failed;
				cDo.append(battle_action_skill_not_success);
				break;
			}
			cDo.append(battle_action_add_hp);
			double module = sk[selfModuleStr].asDouble();
			int add = (int)( (currentAtk->getArmsLevel()*2 + 40 + currentAtk->getTotalChar(idx_magic) * module) * 
				(1 + currentAtk->getTotalChar(idx_cureModule) + currentDef->getTotalChar(idx_beCureModule)));
			add = currentDef->alterHP(add);
			cDo.append(currentDef->getHP());
			cDo.append(add);
		} while (false);
		if(! cDo.empty() && ! cDo.isNull())report.append(cDo);
		return res;
	}

	int battle_system::reduceDefEnergyFormule(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		do 
		{
			if(currentDef == battleMemPtr())break;
			if (!currentDef->isEnergyMember())
			{
				res = break_by_invalid;
				Json::Value failJson;
				failJson.append(currentDef->getIdx());
				failJson.append(battle_action_skill_not_success);
				report.append(failJson);
				break;
			}
			double rate = sk[skillEfRunRateStr].asDouble();
			if(! commom_sys.randomOk(rate)){
				res = break_by_run_failed;
				Json::Value failJson;
				failJson.append(currentDef->getIdx());
				failJson.append(battle_action_skill_not_success);
				report.append(failJson);
				break;
			}

			int reduce = currentAtk->getTotalAttri(idx_power) - currentDef->getTotalAttri(idx_power) + 20;
			reduce = reduce < 20 ? 20 : reduce;
			reduce = reduce > 80 ? 80 : reduce;
			currentDef->alterEnergy(-reduce, report);
		} while (false);
		return res;
	}

	int battle_system::atk2def(Params::Var& sk, Json::Value& report)
	{
		int res = never_break;
		if(currentDef == battleMemPtr())return res;
		Json::Value cDo;
		do 
		{
			cDo.append(currentDef->getIdx());
			if(currentAtk->isMainTarget(currentDef))
			{
				if(!currentDef->hasBuff(lock_buff))
				{
					bool isDodge = commom_sys.randomOk(currentDef->getDodgeRate(currentAtk));
					if(isDodge){
						res = break_by_dodge;// 闪避
						cDo.append(battle_action_self_resist);
						cDo.append(break_by_dodge);
						break;
					}

					bool isBlock = commom_sys.randomOk(currentDef->getBlockRate(currentAtk));
					if(isBlock){
						res = break_by_block;//格挡

						Json::Value resistJson;
						resistJson.append(currentDef->getIdx());
						resistJson.append(battle_action_self_resist);
						resistJson.append(break_by_block);
						report.append(resistJson);

						{
							bool isCri = false;
							int before = currentAtk->getHP();
							CalculateAtk2Def(sk,currentAtk, currentRound, isCri, true);
							cDo[0u] = currentAtk->getIdx();
							cDo.append(battle_action_reduce_hp);
							int now = currentAtk->getHP();
							cDo.append(now);
							cDo.append(before - now);
							if(isCri) cDo.append(1);
						}

						break;
					}
				}
			}

			if(currentDef->hasBuff(defend_phy_buff)){
				res = break_by_buff;//buff拦截
				currentDef->alterHP(-1);
				cDo.append(battle_action_buff_resist);
				cDo.append(currentDef->getBuffVal(defend_phy_buff));
				cDo.append(currentDef->getHP());
				cDo.append(1);
				break;
			}

			{
				bool isCri = false;
				int before = currentDef->getHP();
				CalculateAtk2Def(sk,currentAtk, currentRound, isCri);
				cDo.append(battle_action_reduce_hp);
				int now = currentDef->getHP();
				cDo.append(now);
				cDo.append(before - now);
				if(isCri) cDo.append(1);
				CRITAG(isCri);
			}
		} while (0);
		if(! cDo.empty() && ! cDo.isNull())report.append(cDo);
		if(res != break_by_dodge && res != break_by_block && currentAtk->isMainTarget(currentDef)){
			currentAtk->alterEnergy(_AddEnergy, report);
			currentDef->alterEnergy(_AddEnergy, report);
		}
		return res;
	}

	void battle_system::formatRule(int t[3], const int s[3])
	{
		for (unsigned i = 0 ; i < 3; ++i)t[i] = s[i];
	}

	void battle_system::DealStarwarWithNpcReport(Battlefield& field, Json::Value& report)
	{
		Json::Value& rw = report[BATTLE::strBattleReward];
		ADDDEFAULTINFO(field.atkSide, rw);
		int battleResult = report[strBattleResult].asInt();
		report[starwar_exploit_field_str] = battleResult == 1 ? field.ParamJson[BATTLE::strStarwarParam][5u].asInt() : 0;
		report[starwar_level_field_str] = field.ParamJson[BATTLE::strStarwarParam][6u].asInt();
		Json::Value info;
		info.append(action_add_silver);
		info.append(battleResult == 1 ? starwar::_win_silver : starwar::_lose_silver);
		rw.append(info);
	}
	 
	void battle_system::DealStarwarWithPlayerReport(Battlefield& field, Json::Value& report, bool flag)
	{
		Json::Value& rw = report[BATTLE::strBattleReward];
		rw = Json::arrayValue;
		if (flag)ADDDEFAULTINFO(field.atkSide, rw);
		else ADDDEFAULTINFO(field.defSide, rw);

		int battleResult = report[strBattleResult].asInt();
		int lv = flag ? field.ParamJson[BATTLE::strStarwarParam][4u].asInt() : field.ParamJson[BATTLE::strStarwarParam][3u].asInt();
		bool result = flag ? (battleResult == 1 ? true : false) : (battleResult == 1 ? false : true);
		report[starwar_exploit_field_str] = starwar_sys.getParam().getExploit(lv, result);
		report[starwar_level_field_str] = flag ? field.ParamJson[BATTLE::strStarwarParam][3u].asInt() : field.ParamJson[BATTLE::strStarwarParam][4u].asInt();
		Json::Value info;
		info.append(action_add_silver);
		int silver = flag ? (battleResult == 1 ? starwar::_win_silver : starwar::_lose_silver)
			: (battleResult == 1 ? starwar::_lose_silver : starwar::_win_silver);
		info.append(silver);
		rw.append(info);
	}

	void battle_system::DealArenaReport(Battlefield& field, Json::Value& report, bool flag)
	{
		Json::Value& rw = report[BATTLE::strBattleReward];
		rw = Json::arrayValue;
		if(flag)ADDDEFAULTINFO(field.atkSide, rw);
		else ADDDEFAULTINFO(field.defSide, rw);

		int playerRank = field.ParamJson[BATTLE::strArenaParam][0u].asInt();
		int targetRank = field.ParamJson[BATTLE::strArenaParam][1u].asInt();
		int battleResult = report[strBattleResult].asInt();

		int old_rank = flag? playerRank : targetRank;
		report["ork"] = old_rank;
		report["rk"] = old_rank;
		if(battleResult == 1 && playerRank > targetRank)
			report["rk"] = flag? targetRank : playerRank;

		if(flag)
		{
			int silver = battleResult == 1 ? arena_system::_win_silver : arena_system::_lose_silver;
			Json::Value info;
			info.append(action_add_silver);
			info.append(silver);
			rw.append(info);
		}
	}

	void battle_system::DealStrategyReport(string filename, Json::Value report)
	{
		writeToFile(filename, report);
	}

	void battle_system::DealPVEReport(Battlefield field, Json::Value report, const bool defeat)
	{
		string fileName = getBattleFileName(battle_player_vs_npc_type);
		string fullDir = reportTempPVEDir + fileName;
		writeToFile(fullDir, report);
		int battleResult = report[strBattleResult].asInt();
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = field.defSide->getSideID();
		msg[msgStr][2u] = fileName;
		msg[msgStr][3u] = battleResult;
		msg[msgStr][4u] = defeat;
		msg[msgStr][5u] = report[strBattleReward];
		player_mgr.sendToPlayer(field.atkSide->getSideID(), field.atkSide->netID, gate_client::war_story_battle_with_npc_resp, msg);
	}

	void battle_system::DealPVPReport(Battlefield field, const string file, Json::Value report)
	{
		string fullDir = reportTempPVPDir + file;
		writeToFile(fullDir, report);
		int battleResult = report[BATTLE::strBattleResult].asInt();
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = battleResult;
		msg[msgStr][2u] = file;
		player_mgr.sendToPlayer(field.atkSide->getSideID(), field.atkSide->netID, gate_client::world_area_battle_resp, msg);
	}

	void battle_system::DealTeamPVEReport(BattlefieldMuilt fields, Json::Value report)
	{
		string reportFile = fields.ParamJson[strTeamReportDir].asString();
		string fullDir = teamReportTempDir + reportFile;
		writeToFile(fullDir, report);
		int battleResult = report[strBattleResult].asInt();
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = fields.ParamJson[strWarMapID].asInt();
		msg[msgStr][2u] = reportFile;
		msg[msgStr][3u] = battleResult;
		for (unsigned i = 0; i < fields.atkSide.size(); ++i)
		{
			battleSidePtr ptr = fields.atkSide[i];
			player_mgr.sendToPlayer(ptr->getSideID(), ptr->netID, gate_client::war_story_team_attack_resp, msg);
		}
		
	}

	void battle_system::DealTeamGuildReport(BattlefieldMuilt fields, Json::Value report)
	{
		string reportFile = fields.ParamJson[guild_battle_def::strReportLink].asString();
		int battleResult = report[strBattleResult].asInt();
		int mapID = fields.ParamJson[BATTLE::strWarMapID].asInt();
		reportFile = boost::lexical_cast<string, int>(mapID) + "/" + reportFile;
		string fullDir = teamReportGuildDir + reportFile;
		writeToFile(fullDir, report);
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = fields.ParamJson[strWarMapID].asInt();
		msg[msgStr][2u] = reportFile;
		msg[msgStr][3u] = battleResult;
		msg[msgStr][4u] = (fields.ParamJson[guild_battle_def::strOccupier].asString() == "" ? 0 : 1);
		for (unsigned i = 0; i < fields.atkSide.size(); ++i)
		{
			battleSidePtr ptr = fields.atkSide[i];
			player_mgr.sendToPlayer(ptr->getSideID(), ptr->netID, gate_client::guild_battle_fight_resp, msg);
		}
		if (fields.defSide.size() > 0)
		{
			battleSidePtr firstSide = fields.defSide[0];
			if (firstSide->isPlayerSide())
			{
				for (unsigned i = 0; i < fields.defSide.size(); ++i)
				{
					battleSidePtr ptr = fields.defSide[i];
					player_mgr.sendToPlayer(ptr->getSideID(), ptr->netID, gate_client::guild_battle_fight_resp, msg);
				}
			}
		}

		Json::Value contentJson;
		contentJson["an"] = fields.ParamJson[guild_battle_def::strContender1];
		if (fields.ParamJson["opt"] == 1)
		{
			contentJson["an"] = fields.ParamJson[guild_battle_def::strContender1];
		}
		else
		{
			contentJson["an"] = fields.ParamJson[guild_battle_def::strContender2];
		}
		contentJson["dn"] = fields.ParamJson[guild_battle_def::strOccupier];
		contentJson["res"] = battleResult;
		contentJson[guild_battle_def::strReportLink] = reportFile;
		contentJson[BATTLE::strWarMapID] = mapID;
		contentJson[guild_battle_def::strAreaID] = mapID;

		for (unsigned i =0 ; i < fields.atkSide.size(); ++i)
		{
			battleSidePtr side = fields.atkSide[i];
			playerDataPtr player = player_mgr.getPlayerMain(side->getSideID());
			if(player == playerDataPtr())continue;

			string strAtkReward = BATTLE::strBattleReward+"Atk";
			string strDefReward = BATTLE::strBattleReward+"Def";

			int contributeBase, contributeExtra, contributeTotal;
			contributeBase = report[BATTLE::strBattleReward][i][5].asInt();
			contributeExtra = report[BATTLE::strBattleReward][i][6].asInt();
			contributeTotal = contributeBase + contributeExtra;
			contentJson[memberContributeStr] = contributeTotal;
			int gid = player->Guild.getGuildID();
			contentJson["cgn"] = guild_sys.getGuildName(gid);
			email_sys.sendSystemEmailCommon(player, email_type_system_battle_report, email_team_system_guild_battle, contentJson);

			{
				//每次战斗的结果（玩家ID、战斗胜负、获得的奖励内容和数值）
				vector<string> fields;
				fields.push_back(boost::lexical_cast<string, int>(battleResult));
				fields.push_back(boost::lexical_cast<string, int>(contributeTotal));
				unsigned cur_time = na::time_helper::get_current_time();
				StreamLog::Log(guild_battle_def::strMysqlLogGuildBattle, player, boost::lexical_cast<string, int>(cur_time), 
					"", fields, log_guild_battle_atk);
			}

		}

		int atkNumber = fields.atkSide.size();

		if (fields.defSide.size() > 0)
		{
			battleSidePtr firstSide = fields.defSide[0];
			if (firstSide->isPlayerSide())
			{
				for (unsigned i =0 ; i < fields.defSide.size(); ++i)
				{
					unsigned rewardIndex = atkNumber + i;
					battleSidePtr side = fields.defSide[i];
					playerDataPtr player = player_mgr.getPlayerMain(side->getSideID());
					if(player == playerDataPtr())continue;

					string strAtkReward = BATTLE::strBattleReward+"Atk";
					string strDefReward = BATTLE::strBattleReward+"Def";

					int contributeBase, contributeExtra, contributeTotal;
					contributeBase = report[BATTLE::strBattleReward][rewardIndex][5].asInt();
					contributeExtra = report[BATTLE::strBattleReward][rewardIndex][6].asInt();
					contributeTotal = contributeBase + contributeExtra;
					contentJson[memberContributeStr] = contributeTotal;
					int gid = player->Guild.getGuildID();
					contentJson["cgn"] = guild_sys.getGuildName(gid);
					email_sys.sendSystemEmailCommon(player, email_type_system_battle_report, email_team_system_guild_battle, contentJson);

					{
						//每次战斗的结果（玩家ID、战斗胜负、获得的奖励内容和数值）
						vector<string> fields;
						fields.push_back(boost::lexical_cast<string, int>(battleResult));
						fields.push_back(boost::lexical_cast<string, int>(contributeTotal));
						unsigned cur_time = na::time_helper::get_current_time();
						StreamLog::Log(guild_battle_def::strMysqlLogGuildBattle, player, boost::lexical_cast<string, int>(cur_time), 
							"", fields, log_guild_battle_atk);
					}
				}
			}
		}

		starAreaData& aData = guild_battle_sys.getAreaData(mapID);
		if (fields.ParamJson["opt"] == 1)
		{
			aData.finishTheFirstBattle_ = guild_battle_state_null;
		}
		if (fields.ParamJson["opt"] == 2)
		{
			aData.finishTheSecondBattle_ = guild_battle_state_null;
		}
		
		chat_sys.sendToAllBroadCastCommon(BROADCAST::guild_battle_fight_result, contentJson);
	}

	void battle_system::MineResRespon(const int battleResult, Battlefield field, playerDataPtr atkData, Json::Value report)
	{
		string atkFileName = mine_res::strReportMineResDir + field.atkSide->getFileName();
		writeToFile(atkFileName, report);
		string defFileName = mine_res::strReportMineResDir + field.defSide->getFileName();
		writeToFile(defFileName, report);

		_Logic_Post(boost::bind(&mine_resource_system::mineResFightEnding,mine_resource_system::mineResSys, battleResult, field, atkData));

		vector<int> IDList;
		PUSHID(IDList, field.atkSide->getPlayerID());
		PUSHID(IDList, field.defSide->getPlayerID());
		_Logic_Post(boost::bind(&system_response::OverBattle, system_response::respSys, IDList));
	}

	void battle_system::WorldBossRespon(const int battleResult, Battlefield field, playerDataPtr atkData, Json::Value &report)
	{
		string totalReportDir = worldBossDir + "total/";
		string totalReportLink = boost::lexical_cast<string, int>(world_boss_sys.getReportLinkAndIncrease());
		string totalReportFile = totalReportDir + totalReportLink;
		createDir(totalReportDir);
		writeToFile(totalReportFile, report);

		string playerReportDir = worldBossDir + "player/";
		createDir(playerReportDir);
		string singlePlayerReportDir = playerReportDir + boost::lexical_cast<string, int>(field.atkSide->getSideID()) + "/";
		createDir(singlePlayerReportDir);
		string singlePlayerReportFile = singlePlayerReportDir + field.atkSide->getFileName();
		writeToFile(singlePlayerReportFile, report);

//		LogE << __FUNCTION__ << ",pid:" << atkData->playerID << LogEnd;
		_Logic_Post(boost::bind(&world_boss_system::worldBossEnding, world_boss_system::worldBossSys, battleResult, field, atkData));

		vector<int> IDList;
		PUSHID(IDList, field.atkSide->getPlayerID());
		PUSHID(IDList, field.defSide->getPlayerID());
		_Logic_Post(boost::bind(&system_response::OverBattleBoss, system_response::respSys, IDList));
	}

}