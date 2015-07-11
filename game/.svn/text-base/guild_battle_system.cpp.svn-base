#include "guild_battle_system.h"
#include "season_system.h"
#include "one_to_one.h"
#include "response_def.h"
#include "battle_def.h"
#include "guild_system.h"
#include "email_system.h"
#include "chat_system.h"
#include "record_system.h"
#include "translate.h"
#include "task_system.h"
#include "activity_system.h"
namespace gg
{

#define starAreaNullConfig (*starAreaConfig::sNullPoint)

	static fighterDataPtr fdata = fighterDataPtr();

	guild_battle_system* const guild_battle_system::guildBattleSys = new guild_battle_system();
	starAreaConfig* starAreaConfig::sNullPoint = NULL;
	starAreaConfig::starAreaConfig()
	{
		teamMapData_ = createTeam();
	}

	starAreaConfig::starAreaConfig(int aid)
		: areaID_(aid)
	{
		teamMapData_ = createTeam();
	}

	bool starAreaConfig::operator == (starAreaConfig& s)
	{
		return (areaID_ == s.areaID_);
	}

	starAreaData starAreaData::sNull = starAreaData();
	starAreaData::starAreaData()
	{
		reportLink_ = 1;
		areaID_ = -1;
		nextSeasonUpdateTime_ = 0;
		nextDayUpdateTime_ = 0;
		startLevyTime_ = 0;
		fighterData_.resize(3);
		playerData_.resize(3);
		battleResult_[0] = 0;
		battleResult_[1] = 0;
		inviteTime_ = 0;
		hasInvite_ = false;
		finishTheFirstBattle_ = guild_battle_state_null;
		finishTheSecondBattle_ = guild_battle_state_null;
	}

	bool starAreaData::operator == (starAreaData& s)
	{
		return areaID_ == s.areaID_;
	}

	enterLimit::enterLimit()
	{
		level_ = 0;
		process_ = 0;
		arenaRank_ = 9000000;
		valid_ = false;
	}

	void enterLimit::reset()
	{
		level_ = 0;
		process_ = 0;
		arenaRank_ = 9000000;
		valid_ = false;
	}

	fighterData::fighterData()
	{
		atkValue_ = 0;
		defValue_ = 0;
		winnerStreak_ = 0;
	}

	fighterData::~fighterData()
	{
	}

	int fighterData::sumAll()
	{
		return atkValue_ + defValue_ + winnerStreak_;
	}

	bool enterLimit::isPermit(playerDataPtr pdata)
	{
		if (!valid_)
		{
			return true;
		}
		if (pdata->Arena.getRank() > arenaRank_
			|| pdata->Warstory.currentProcess < process_
			|| pdata->Base.getLevel() < level_)
		{
			return false;
		}
		
		return true;
	}

	guild_battle_system::guild_battle_system()
	{
//		levyNextUpdateTime_ = 0;
		state_ = state_null;
	}

	guild_battle_system::~guild_battle_system()
	{

	}

	void guild_battle_system::initData()
	{
		starAreaConfig::sNullPoint = new starAreaConfig();
		initAreaConfig();
		initAreaData();
	}

	void guild_battle_system::initAreaConfig()
	{
		vector<Json::Value> allConfigFile;
		na::file_system::load_jsonfiles_from_dir(guild_battle_def::strGuildConfigDir, allConfigFile);
		for (unsigned i = 0; i < allConfigFile.size(); i++)
		{
			Json::Value& singleConfigJson = allConfigFile[i];
			starAreaConfig singleConfig;
			singleConfig.areaID_ = singleConfigJson["id"].asInt();
			singleConfig.canLeveyBaseNumber_ = singleConfigJson["output"].asInt();
			singleConfig.levelCondition_ = singleConfigJson["lv"].asInt();
			singleConfig.guildLevelCondition_ = singleConfigJson["guildLevel"].asInt();
			Json::Value &dataJson = singleConfigJson["armyDatas"];
			singleConfig.teamMapData_->teamWinMax = singleConfigJson[WAR::teamNpcMaxWinNumStr].asInt();
			singleConfig.teamMapData_->teamMax = singleConfigJson[WAR::teamNpcMaxTeamMemberStr].asInt();
			singleConfig.teamMapData_->battleSize = singleConfigJson[WAR::teamNpcStepStr].asUInt();
			singleConfig.teamMapData_->battleSize =  singleConfig.teamMapData_->battleSize < 2 ?  2 : singleConfig.teamMapData_->battleSize; 
			for (unsigned i = 0; i < dataJson.size(); ++i)
			{
				Json::Value mapDataJson = dataJson[i];
				mapDataPtr mDP = warStory::create();
				if(mDP == NULL)continue;

				string mNa = mapDataJson[WAR::warStoryNameStr].asString();
				//mDP->mapName = TransLate::getTranslate(mNa);
				mDP->mapName = mNa;
				mDP->localID = mapDataJson[WAR::warStoryLocalIDStr].asInt();
				mDP->mapLevel = mapDataJson[WAR::warStoryLevelStr].asInt();
				Json::Value& troop = mapDataJson["trd"];
				mDP->npcArmy = war_story.formatNpcArmy(troop);
				singleConfig.teamMapData_->npcArmy.push_back(mDP);
			}

			starAreaConfig_.insert(make_pair(singleConfig.areaID_, singleConfig));
		}

		starAreaConfig_[1].levelCondition_ = 10;
		starAreaConfig_[2].levelCondition_ = 20;
		starAreaConfig_[3].levelCondition_ = 30;
		starAreaConfig_[4].levelCondition_ = 40;
		starAreaConfig_[5].levelCondition_ = 50;
		starAreaConfig_[6].levelCondition_ = 60;
	}

	void guild_battle_system::initAreaData()
	{
		db_mgr.ensure_index(guild_battle_def::strGuildBattleDBData, BSON(guild_battle_def::strAreaID << 1));
		objCollection areaList = db_mgr.Query(guild_battle_def::strGuildBattleDBData);

		const static string teamReportGuildDir = "./report/temp/teamGuild/";
		string areaReportLinkDir;
		for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
		{
			starAreaData aData;
			aData.areaID_ = it->first;
			starAreaData_.insert(make_pair(aData.areaID_, aData));
			areaReportLinkDir = teamReportGuildDir+boost::lexical_cast<string,int>(aData.areaID_)+"/";

			boost::filesystem::path path_dir(areaReportLinkDir);
			if(!boost::filesystem::exists(path_dir))
				boost::filesystem::create_directory(path_dir);
		}

		int cur_time = na::time_helper::get_current_time();
		int cur_season = season_sys.getSeason();
		for (objCollection::iterator it = areaList.begin(); it != areaList.end(); it++)
		{
			starAreaData aData;
			checkNotEoo((*it)[guild_battle_def::strAreaID])
				aData.areaID_ = (*it)[guild_battle_def::strAreaID].Int();
			checkNotEoo((*it)[guild_battle_def::strOccupier])
				aData.occupier_ = (*it)[guild_battle_def::strOccupier].String();
			checkNotEoo((*it)[guild_battle_def::strContender1])
				aData.contender_[0] = (*it)[guild_battle_def::strContender1].String();
			checkNotEoo((*it)[guild_battle_def::strContender2])
				aData.contender_[1] = (*it)[guild_battle_def::strContender2].String();
			checkNotEoo((*it)[guild_battle_def::strFightResult1])
				aData.battleResult_[0] = (*it)[guild_battle_def::strFightResult1].Int();
			checkNotEoo((*it)[guild_battle_def::strFightResult2])
				aData.battleResult_[1] = (*it)[guild_battle_def::strFightResult2].Int();
			checkNotEoo((*it)[guild_battle_def::strNextAreaSeasonUpdateTime])
				aData.nextSeasonUpdateTime_ = (*it)[guild_battle_def::strNextAreaSeasonUpdateTime].Int();
			checkNotEoo((*it)[guild_battle_def::strNextAreaDayUpdateTime])
				aData.nextDayUpdateTime_ = (*it)[guild_battle_def::strNextAreaDayUpdateTime].Int();
			checkNotEoo((*it)[guild_battle_def::strReportLink])
				aData.reportLink_ = (*it)[guild_battle_def::strReportLink].Int();
			checkNotEoo((*it)[guild_battle_def::strStartLevyTime_])
				aData.startLevyTime_ = (*it)[guild_battle_def::strStartLevyTime_].Int();
			starAreaData_[aData.areaID_] = aData;

			if (cur_time > aData.nextSeasonUpdateTime_)
			{
				boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
				tm t = boost::posix_time::to_tm(p);

				unsigned update_time;
				if (t.tm_hour < 5)
					update_time = cur_time - 86400*cur_season - 3600*(t.tm_hour+24-5) - 60*t.tm_min - t.tm_sec + 3*86400;
				else
					update_time = cur_time - 86400*cur_season - 3600*(t.tm_hour-5) - 60*t.tm_min - t.tm_sec + 3*86400;

				starAreaData tempData;
				tempData.areaID_ = aData.areaID_;
				tempData.nextSeasonUpdateTime_ = update_time;
				starAreaData_[tempData.areaID_] = tempData;
				saveAreaData(tempData.areaID_);
			}
			starAreaData_[aData.areaID_].maintainDailyData();
		}
	}

	void guild_battle_system::guildBattleUpdate(boost::system_time& tmp)
	{
		if((tmp - tick).total_seconds() < duration_)return;

		tick = tmp;
		duration_ = 20;

		br_.refresh();
		annouce_.refresh();
		int sea = season_sys.getSeason();
		if (sea == season_quarter_four)
		{
			if (!br_.winterReset_)
			{
				br_.winterReset_ = true;
				for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
				{//冬天重置
					starAreaData aData;
					aData.areaID_ = it->first;
					starAreaData_[aData.areaID_] = aData;
					saveAreaData(aData.areaID_);
				}
			}
			return;
		}

		unsigned cur_time = na::time_helper::get_current_time();
		boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
		tm t = boost::posix_time::to_tm(p);

		if (state_ != state_apply && t.tm_hour >= 12 && t.tm_hour < 20)
		{
			annouce_.apply_ = true;
			state_ = state_apply;
			if (t.tm_hour == 12 && t.tm_min < 2)
			{
				Json::Value chatContent;
				chat_sys.sendToAllBroadCastCommon(BROADCAST::guild_battle_to_all_can_apply, chatContent);
			}
		}
		if (state_ != state_first_join_team && t.tm_hour == 20 && t.tm_min < 10)
		{
			state_ = state_first_join_team;
			chatBroadCastBeginJoinTeam(1);
			checkAdvanceEnd();
		}
		if (state_ != state_second_join_team && t.tm_hour == 20 && (t.tm_min >= 11 && t.tm_min < 20))
		{
			state_ = state_second_join_team;
			for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
			{
				starAreaData &aData = getAreaData(it->first);
				if (aData.battleResult_[0] == 0)//遇到当机等特殊情况时跳过战斗的话，就认为第一场失败了
					aData.battleResult_[0] = 2;

				saveAreaData(aData.areaID_);
			}
			chatBroadCastBeginJoinTeam(2);
		}
		if (state_ == state_first_join_team && t.tm_hour == 20 && t.tm_min == 9)
		{
			duration_ = 1;
		}
		if (state_ == state_second_join_team && t.tm_hour == 20 && t.tm_min == 19)
		{
			duration_ = 1;
		}
		if (state_ == state_first_join_team && t.tm_hour == 20 && t.tm_min == 10)
		{
			state_ = state_first_fight;
			fight(1);
		}
		if (state_ == state_second_join_team && t.tm_hour == 20 && t.tm_min == 20)
		{
			state_ = state_second_fight;
			fight(2);
		}
		if (state_ == state_second_fight && t.tm_hour == 20 && (t.tm_min == 21 || t.tm_min == 22))
		{
			if (t.tm_hour == 20 && t.tm_min == 20 && isSecondBattleEnd()
				|| (t.tm_min == 21 || t.tm_min == 22))
			{
				state_ = state_after_battle;
			}
		}
		if (!br_.afterBattleClear_ && (t.tm_hour == 20 && t.tm_min > 22 || t.tm_hour > 21))
		{
			state_ = state_null;
			br_.afterBattleClear_ = true;
			clearAreaDataDaily();
		}
		if (!br_.stateNullClear_ && state_ == state_null)
		{
			br_.stateNullClear_ = true;
			clearAreaDataDaily();
		}

		//普通广播
		if (!annouce_.ad_19_30_ && t.tm_hour == 19 && t.tm_min == 30)
		{
			annouce_.ad_19_30_ = true;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::guild_battle_19_30);
		}

		//滚动公告
		if (!br_.br_19_30_ && t.tm_hour == 19 && t.tm_min == 30 && t.tm_sec >= 20)
		{
			br_.br_19_30_ = true;
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_guild_battle_is_coming_19_30);
		}
		if (!br_.br_20_00_ && t.tm_hour == 20 && t.tm_min == 0)
		{
			br_.br_20_00_ = true;
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_guild_battle_is_coming_20_00);
		}
		if (!br_.br_20_11_ && t.tm_hour == 20 && t.tm_min == 11)
		{
			br_.br_20_11_ = true;
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_guild_battle_is_coming_20_10);
		}

// 		if (cur_time > levyNextUpdateTime_)//延迟30秒
// 			levyNextUpdateTime_ = cur_time - t.tm_hour * 3600 - t.tm_min * 60 - t.tm_sec + 20 * 3600 + 20 * 60 + 30;
	}

	bool guild_battle_system::getActTime(unsigned &start_time, unsigned &end_time)
	{
		unsigned cur_time = na::time_helper::get_current_time();

		boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
		tm t = boost::posix_time::to_tm(p);

		unsigned stand_time1 = cur_time - 3600 * t.tm_hour - 60 * t.tm_min - t.tm_sec;
		unsigned stand_time2 = stand_time1 + 5 * 3600;

		unsigned start_time1 = stand_time1 + 20 * 3600;
		unsigned end_time1 = start_time1 + 10 * 60;
		unsigned start_time2 = stand_time1 + 20 * 3600 + 11 * 60;
		unsigned end_time2 = start_time2 + 9 * 60;

		bool ret = false;
		start_time = 0;
		end_time = 0;

		if (season_sys.getSeason() == season_quarter_four)
		{
			return false;
		}

		if (cur_time < stand_time2)
		{
			start_time = start_time2 - 86400;
			end_time = end_time2 - 86400;
		}
		else if (cur_time < start_time1)
		{
			start_time = start_time1;
			end_time = end_time1;
		}
		else if (cur_time < end_time1)
		{
			start_time = start_time1;
			end_time = end_time1;
			ret = true;
		}
		else if (cur_time < start_time2)
		{
			start_time = start_time2;
			end_time = end_time2;
		}
		else if (cur_time < end_time2)
		{
			start_time = start_time2;
			end_time = end_time2;
			ret = true;
		}
		else
		{
			start_time = start_time2;
			end_time = end_time2;
		}

		return ret;
	}

	void guild_battle_system::recordTest()
	{
		int gid1 = 5;
		int gid2 = 6;
		int gid3 = 7;

		string gna1 = guild_sys.getGuildName(gid1);
		string gna2 = guild_sys.getGuildName(gid2);
		string gna3 = guild_sys.getGuildName(gid3);
		starAreaData& aData = getAreaData(1);
		aData.contender_[0] = gna1;
		aData.occupier_ = gna2;
		saveAreaData(1);
		recordGuildBattleEvent(1);
	}

	void guild_battle_system::clearAreaDataDaily()
	{
		for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
		{
			clearSingleAreaDataDaily(it->first);
		}
	}

	void guild_battle_system::clearSingleAreaDataDaily(int areaID)
	{
		starAreaData& aData = getAreaData(areaID);
		if (aData == starAreaData::sNull)
			return;
		aData.contender_[0] = "";
		aData.contender_[1] = "";
		aData.finishTheFirstBattle_ = guild_battle_state_null;
		aData.finishTheSecondBattle_ = guild_battle_state_null;
		for (unsigned i = 0; i < 3; i++)
		{
			aData.limit_->reset();
			aData.fighterData_[i].clear();
			aData.playerData_[i].clear();
		}
		aData.battleResult_[0] = 0;
		aData.battleResult_[1] = 0;
		saveAreaData(aData.areaID_);
	}

	void guild_battle_system::renameGuild(string oldName, string newName)
	{
		for (starAreaConfigMap::iterator itCfg = starAreaConfig_.begin(); itCfg != starAreaConfig_.end(); itCfg++)
		{
			starAreaData &adata = getAreaData(itCfg->first);
			bool isModify = false;
			if (adata.contender_[0] == oldName)
			{
				adata.contender_[0] = newName;
				isModify = true;
			}				
			if (adata.contender_[1] == oldName)
			{
				adata.contender_[1] = newName;
				isModify = true;
			}
			if (adata.occupier_ == oldName)
			{
				adata.occupier_ = newName;
				isModify = true;
			}
			if (isModify)
			{
				saveAreaData(itCfg->first);
			}
		}
	}

	void guild_battle_system::guildBattleTotalUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int myAreaID = -1;
		int guildID = d->Guild.getGuildID();
		if (guildID == -1)
			myAreaID = -1;
		string guildName = guild_sys.getGuildName(guildID);
		int contenderID, contenderIndex;
		getGuildContender(guildName, contenderID, contenderIndex);
		if (contenderID != -1)
			myAreaID = contenderID;

		int idx = 0;
		Json::Value updateJson;
		Json::Value updateExtraJson;
		for (starAreaConfigMap::iterator itCfg = starAreaConfig_.begin(); itCfg != starAreaConfig_.end(); itCfg++)
		{
			starAreaData &adata = getAreaData(itCfg->first);
			string strExtraGuild = adata.contender_[0];
			if (adata.occupier_ != "")
				strExtraGuild = adata.occupier_;
			Guild::guildPtr gExtra = guild_sys.getGuild(strExtraGuild);
			int extraFlagLevel = 0;
			int extraFlagIcon = 0;

			if (gExtra != Guild::guildPtr())
			{
				extraFlagLevel = gExtra->getScienceData(playerGuild::flag_level).LV;
				extraFlagIcon = gExtra->getFlagIcon();
			}

			updateJson[idx].append(itCfg->first);
			updateExtraJson[idx].append(itCfg->first);
			if (adata == starAreaData::sNull)
			{
				updateJson[idx].append("");
				updateJson[idx].append(-1);
				updateExtraJson[idx].append("");
			}
			else
			{
				updateExtraJson[idx].append(adata.occupier_);
				if (state_ >= state_first_join_team && state_ <= state_second_join_team)
				{
					if (adata.contender_[1] == "" 
						&& (state_ == state_second_join_team 
						|| state_ == state_first_fight && adata.finishTheFirstBattle_ == guild_battle_state_null))
					{//第一场结束，如果没有挑战者，会认为该城池战斗结束
						updateJson[idx].append(adata.occupier_);
						Guild::guildPtr gptr = guild_sys.getGuild(adata.occupier_);
						if (gptr != Guild::guildPtr())
							updateJson[idx].append(gptr->getScienceData(playerGuild::flag_level).LV);
						else
							updateJson[idx].append(-1);
					}
					else if (state_ >= state_first_join_team && adata.canLevyInAdvance(guildName))
					{
						updateJson[idx].append(adata.occupier_);
						Guild::guildPtr gptr = guild_sys.getGuild(adata.occupier_);
						if (gptr != Guild::guildPtr())
							updateJson[idx].append(gptr->getScienceData(playerGuild::flag_level).LV);
						else
							updateJson[idx].append(-1);
					}
					else
					{
						updateJson[idx].append("");
						updateJson[idx].append(-1);
					}
				}
				else
				{
					updateJson[idx].append(adata.occupier_);
					Guild::guildPtr gptr = guild_sys.getGuild(adata.occupier_);
					if (gptr != Guild::guildPtr())
						updateJson[idx].append(gptr->getScienceData(playerGuild::flag_level).LV);
					else
						updateJson[idx].append(-1);
				}
			}
			updateExtraJson[idx].append(strExtraGuild);
			updateExtraJson[idx].append(extraFlagLevel);
			updateExtraJson[idx].append(extraFlagIcon);
			idx++;
		}

		r[msgStr][1] = updateJson;
		r[msgStr][2] = myAreaID;
		r[msgStr][3] = updateExtraJson;

		starAreaData &adata = getAreaData(myAreaID);
		bool hasEnter = false;
		bool isRedPoint = false;
		
		int occupyAreaID = -1;
		getGuildOccupyArea(guildName, occupyAreaID);
		if (!(adata == starAreaData::sNull))
		{
			if (state_ == state_first_join_team && adata.enterFirstList_.find(d->playerID) == adata.enterFirstList_.end())
			{
				if (adata.contender_[0] != "" && (contenderIndex == 0 || occupyAreaID > 0))
				{
					isRedPoint = true;
				}
			}
			if ((state_ == state_first_fight || state_ == state_second_join_team) && adata.enterSecondList_.find(d->playerID) == adata.enterSecondList_.end())
			{
				if (adata.contender_[1] != "" && (contenderIndex == 1 || occupyAreaID > 0))
				{
					isRedPoint = true;
				}
			}
		}
//		r[msgStr][4] = hasEnter;
		r[msgStr][4] = isRedPoint;

		Return(r, 0);
	}

	void guild_battle_system::guildBattleSingleUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int areaID = js_msg[0u].asInt();
		starAreaConfig cfg = getAreaConfig(areaID);
		if (cfg == starAreaNullConfig)
		{
			Return(r, -1);
		}

		starAreaData &adata = getAreaData(areaID);
		if (adata == starAreaData::sNull)
		{
			Return(r, -1);
		}

		Json::Value updateJson;
		int leagueLevel1 = 0;
		int flagLevel1 = 0;
		int flagID1 = 0;
		string guild1 = adata.contender_[0];
		Guild::guildPtr gPtr1 = guild_sys.getGuild(guild1);
		if (gPtr1 != Guild::guildPtr())
		{
			leagueLevel1 = gPtr1->getScienceData(playerGuild::guild_level).LV;
			flagLevel1 = gPtr1->getScienceData(playerGuild::flag_level).LV;
			flagID1 = gPtr1->getFlagIcon();
		}
		updateJson[guildLeagueLevelStr+"1"] = leagueLevel1;
		updateJson[guildFlagLevelStr+"1"] = flagLevel1;
		updateJson[guildFlagIconStr+"1"] = flagID1;

		int leagueLevel2 = 0;
		int flagLevel2 = 0;
		int flagID2 = 0;
		string guild2 = adata.contender_[1];
		Guild::guildPtr gPtr2 = guild_sys.getGuild(guild2);
		if (gPtr2 != Guild::guildPtr())
		{
			leagueLevel2 = gPtr2->getScienceData(playerGuild::guild_level).LV;
			flagLevel2 = gPtr2->getScienceData(playerGuild::flag_level).LV;
			flagID2 = gPtr2->getFlagIcon();
		}
		updateJson[guildLeagueLevelStr+"2"] = leagueLevel2;
		updateJson[guildFlagLevelStr+"2"] = flagLevel2;
		updateJson[guildFlagIconStr+"2"] = flagID2;
		
		int leagueLevel3 = 0;
		int flagLevel3 = 0;
		int flagID3 = 0;
		string guild3 = adata.occupier_;
		Guild::guildPtr gPtr3 = guild_sys.getGuild(guild3);
		if (gPtr3 != Guild::guildPtr())
		{
			leagueLevel3 = gPtr3->getScienceData(playerGuild::guild_level).LV;
			flagLevel3 = gPtr3->getScienceData(playerGuild::flag_level).LV;
			flagID3 = gPtr3->getFlagIcon();
		}
		updateJson[guildLeagueLevelStr+"3"] = leagueLevel3;
		updateJson[guildFlagLevelStr+"3"] = flagLevel3;
		updateJson[guildFlagIconStr+"3"] = flagID3;
		
		if (0/*state_ >= state_first_join_team && state_ <= state_second_join_team*/)//total在活动期间置空，single在活动期间不置空
		{
			updateJson[guild_battle_def::strOccupier] = "";
		}
		else
		{
			updateJson[guild_battle_def::strOccupier] = adata.occupier_;
		}
		updateJson[guild_battle_def::strContender1] = adata.contender_[0];
		updateJson[guild_battle_def::strContender2] = adata.contender_[1];
		updateJson[guild_battle_def::strFightResult1] = adata.battleResult_[0];
		updateJson[guild_battle_def::strFightResult2] = adata.battleResult_[1];
		updateJson[guild_battle_def::strRemainLeveyTimes] = getRemainLevyTimes(d, adata);

		int fighterNum = 0;
		string guildName = "";
		Json::Value limitJson;
		limitJson.append(false);
		limitJson.append(0);
		limitJson.append(0);
		limitJson.append(-1);
		int gid = d->Guild.getGuildID();
		int contenderAreaID = -1;
		int occupyAreaID = -1;
		int contenderIndex = -1;
		if (gid != -1)
		{
			guildName = guild_sys.getGuildName(gid);
			getGuildContender(guildName, contenderAreaID, contenderIndex);
			getGuildOccupyArea(guildName, occupyAreaID);

			int curAreaID = -1;
			int curIndex = -1;
			curAreaID = contenderAreaID > 0 ? contenderAreaID : occupyAreaID;
			curIndex = contenderIndex > -1 ? contenderIndex : 2;

			if (occupyAreaID > -1)
				curIndex = 2;//报名了并且第一场胜利了

			if (curIndex != -1)
			{
				fighterNum = adata.playerData_[curIndex].size();
				limitJson[0u] = adata.limit_[curIndex].valid_;
				limitJson[1] = adata.limit_[curIndex].level_;
				limitJson[2] = adata.limit_[curIndex].process_;
				limitJson[3] = adata.limit_[curIndex].arenaRank_;
			}
		}

		updateJson[guild_battle_def::strFighterNum] = fighterNum;
		updateJson[guild_battle_def::strLimit] = limitJson;

		int levyValue = cfg.canLeveyBaseNumber_;
//		int levyValue = getLevyValue(cfg, d);
		updateJson[guild_battle_def::strLevyValue] = levyValue;

		int curState = 0; //0
		if (guildName != "" && (adata.contender_[0] == guildName || adata.contender_[1] == guildName))
			curState = 1; //1
		if (state_ == state_first_join_team || state_ == state_second_join_team || state_ == state_first_fight)
		{
			if (state_ >= state_first_fight && contenderIndex == 0 && occupyAreaID < 1)
				curState = 1; //1 //第一场参战的队伍，如果是在第一场已经输了，那就不能继续参战
			else if (state_ >= state_first_fight && occupyAreaID >= 1 && adata.contender_[1] == "")
				curState = 0; //1 //第一场参战的队伍，如果是在第一场已经赢了并且第二场没有人报名的情况下，红利界面
			else if (adata.contender_[0] == "" && adata.contender_[1] == "" && areaID == occupyAreaID)
				curState = 0;//如果两场都没人报名，并且自己队伍占领了城池，可以直接进入征税界面
			else if (areaID == contenderAreaID || areaID == occupyAreaID)
				curState = 2; //2
			else if (adata.canLevyInAdvance(guildName))
				curState = 0;
			else
				curState = 1; //1
		}
		updateJson[guild_battle_def::strStamp] = curState;
		r[msgStr][1u] = updateJson;
		Return(r, 0);
	}

	int guild_battle_system::getRemainLevyTimes(playerDataPtr pdata, starAreaData& adata)
	{
		int gid = pdata->Guild.getGuildID();
		if (gid == -1)
			return 0;
		string guildName = guild_sys.getGuildName(gid);
		int occupyAreaID;
		getGuildOccupyArea(guildName, occupyAreaID);
		if (occupyAreaID == adata.areaID_)
		{
			return pdata->Guild.getRemainLeveyTimes(adata.startLevyTime_);
		}

		return 0;
	}
	
	const starAreaConfig guild_battle_system::getAreaConfig(int areaID)
	{
		starAreaConfigMap::iterator it = starAreaConfig_.find(areaID);
		if (it == starAreaConfig_.end())
		{
			return starAreaNullConfig;
		}
		return it->second;
	}

	starAreaData& guild_battle_system::getAreaData(int areaID)
	{
		map<int, starAreaData>::iterator it = starAreaData_.find(areaID);
		if (it == starAreaData_.end())
		{
			return starAreaData::sNull;
		}

		it->second.maintainDailyData();
		it->second.maintainSeasonData();
//		unsigned cur_time = na::time_helper::get_current_time();
// 		if (cur_time > it->second.nextSeasonUpdateTime_)
// 		{
// 			boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
// 			tm t = boost::posix_time::to_tm(p);
// 
// 			unsigned update_time;
// 			if (t.tm_hour < 5)
// 				update_time = cur_time - 86400*cur_season - 3600*(t.tm_hour+24-5) - 60*t.tm_min - t.tm_sec + 3*86400;
// 			else
// 				update_time = cur_time - 86400*cur_season - 3600*(t.tm_hour-5) - 60*t.tm_min - t.tm_sec + 3*86400;
// 
// 			starAreaData tempData;
// 			tempData.areaID_ = aData.areaID_;
// 			tempData.nextSeasonUpdateTime_ = update_time;
// 			starAreaData_[tempData.areaID_] = tempData;
// 			saveAreaData(tempData.areaID_);
// 		}
		return it->second;
	}

	void starAreaData::maintainDailyData()
	{
		int cur_time = int(na::time_helper::get_current_time());
		if (cur_time > this->nextDayUpdateTime_)
		{
			starAreaData aData;
			aData.nextDayUpdateTime_ = na::time_helper::get_next_update_time(cur_time);
			aData.areaID_ = this->areaID_;
			aData.occupier_ = this->occupier_;
			aData.reportLink_ = this->reportLink_;
			aData.nextSeasonUpdateTime_ = this->nextSeasonUpdateTime_;
			*this = aData;
		}
	}

	void starAreaData::maintainSeasonData()
	{
		int cur_time = int(na::time_helper::get_current_time());
		if (cur_time > this->nextSeasonUpdateTime_)
		{
			starAreaData aData;
			aData.nextSeasonUpdateTime_ = season_sys.getNextSeasonTime(season_quarter_four, 5);
			aData.nextDayUpdateTime_ = this->nextDayUpdateTime_;
			aData.areaID_ = this->areaID_;
			aData.reportLink_ = this->reportLink_;
			*this = aData;
		}
	}

	bool starAreaData::canLevyInAdvance(string guildName)
	{
		if (occupier_ != "" && occupier_ == guildName && contender_[0] == "" && contender_[1] == "")
			return true;

		return false;
	}

	bool guild_battle_system::setAreaData(int areaID, starAreaData& adata)
	{
		adata.areaID_ = areaID;
		starAreaData_[areaID] = adata;
		return true;
	}

	void guild_battle_system::guildBattleLevyReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int areaID = js_msg[0u].asInt();
		starAreaConfig cfg = getAreaConfig(areaID);
		if (cfg == starAreaNullConfig)
		{
			Return(r, -1);
		}

		starAreaData &adata = getAreaData(areaID);
		if (adata == starAreaData::sNull)
		{
			Return(r, -1);
		}

		int gid = d->Guild.getGuildID();
		if (gid == -1)
			Return(r, -1);

		if (guild_sys.getGuildName(gid) != adata.occupier_)
			Return(r, 1);
		
		int levyTimes = getRemainLevyTimes(d, adata);
		if (levyTimes <= 0)
		{
			Return(r, 2);
		}

		if (state_first_join_team <= state_ && state_ <= state_second_fight)
		{
			if (adata.contender_[1] == "" 
				&& (state_ == state_second_join_team || 
				state_ == state_first_fight && adata.finishTheFirstBattle_ == guild_battle_state_null))
			{//第二场挑战者为空的情况下认为是结束了战斗
			}
			else if (state_ == state_second_fight && adata.finishTheSecondBattle_ == guild_battle_state_null)
			{
			}
			else if (adata.canLevyInAdvance(adata.occupier_))
			{
			}
			else
			{
				Return(r, 3);
			}
		}
			

		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time - d->Guild.getJoinTime() < 2*3600)
			Return(r, 4);

		int levyValue = getLevyValue(cfg, d);
		int doubleRate = commom_sys.randomBetween(1, 10);
		bool isDoubleLevey = false;
		if (doubleRate <= 3)
		{
			levyValue *= 2;
			isDoubleLevey = true;
		}
		d->Base.alterSilver(levyValue);
		d->Guild.alterRemainLeveyTimes(-1);
		r[msgStr][1] = isDoubleLevey;
		r[msgStr][2] = levyValue;

		Json::Value updateSingle;
		updateSingle[msgStr][0u] = areaID;
		string s = updateSingle.toStyledString();
		na::msg::msg_json mj(gate_client::guild_battle_single_update_req, s);
		mj._player_id = d->playerID;
		mj._net_id = d->netID;
		player_mgr.postMessage(mj);
		activity_sys.updateActivity(d, 0, activity::_guild_reward);
		Return(r, 0);
	}

	int guild_battle_system::getLevyValue(starAreaConfig &cfg, playerDataPtr player)
	{
		int levyValue = cfg.canLeveyBaseNumber_;
//		levyValue = int(levyValue*(player->Base.getLevel()+40)/100);//去掉
		levyValue = int(levyValue * (1 + activity_sys.getRate(activity::_guild_battle, player)));
		levyValue = int(levyValue * (1 + activity_sys.getRate(activity::_cai_yuan_guang_jin, player)));

		return levyValue;
	}

	void insert_sort( vector<int>& vec, vector<unsigned>& vec_idx )
	{
		unsigned i, j;
		int temp_val, temp_idx;
		for (i = 1; i < vec.size(); ++i)
		{
			temp_val = vec[i];
			temp_idx = vec_idx[i];
			for (j = i; j>0 && temp_val < vec[j - 1]; --j)
			{
				vec[j] = vec[j - 1];
				vec_idx[j] = vec_idx[j - 1];
			}
			vec[j] = temp_val;
			vec_idx[j] = temp_idx;
		}
	}

	void get_score( vector<unsigned>& bef_idx, vector<unsigned>& aft_idx, vector<int>& score, int coef )
	{
		for (unsigned b = 0; b < bef_idx.size(); b++)
		{
			for (unsigned a = 0; a < aft_idx.size(); a++)
			{
				if (aft_idx[a] == bef_idx[b])
				{
					score[b] += (bef_idx[a] + 1) * coef;
					break;
				}
			}
		}
	}

	int chooseGuild(Guild::guildPtr gPtr, Guild::guildPtr gPtr1, Guild::guildPtr gPtr2)
	{//星盟等级高——>军徽等级高——>星盟当前人数多——>星盟创建早
		if (gPtr1 == Guild::guildPtr() && gPtr2 == Guild::guildPtr())
			return 1;
		if (gPtr1 == Guild::guildPtr())
			return 1;
		if (gPtr2 == Guild::guildPtr())
			return 2;

		vector<int> vGuildLevel, vFlagLevel, vMemberNum, vCreateTime;
		vector<unsigned> vGuildLevelIdxBef, vFlagLevelIdxBef, vMemberNumIdxBef, vCreateTimeIdxBef;
		vector<unsigned> vGuildLevelIdxAft, vFlagLevelIdxAft, vMemberNumIdxAft, vCreateTimeIdxAft;

		for (unsigned i = 0; i < 3; i++)
		{
			vGuildLevelIdxBef.push_back(vGuildLevelIdxBef.size());
			vFlagLevelIdxBef.push_back(vFlagLevelIdxBef.size());
			vMemberNumIdxBef.push_back(vMemberNumIdxBef.size());
			vCreateTimeIdxBef.push_back(vCreateTimeIdxBef.size());
		}

		unsigned cur_time = na::time_helper::get_current_time();
		vGuildLevel.push_back(gPtr->getScienceData(playerGuild::guild_level).LV);
		vFlagLevel.push_back(gPtr->getScienceData(playerGuild::flag_level).LV);
		vCreateTime.push_back(cur_time - gPtr->getCreateTime());
		vector<int> memList;
		gPtr->getAllMembers(memList);
		vMemberNum.push_back(memList.size());

		vGuildLevel.push_back(gPtr1->getScienceData(playerGuild::guild_level).LV);
		vFlagLevel.push_back(gPtr1->getScienceData(playerGuild::flag_level).LV);
		vCreateTime.push_back(cur_time - gPtr1->getCreateTime());
		memList.clear();
		gPtr1->getAllMembers(memList);
		vMemberNum.push_back(memList.size());

		vGuildLevel.push_back(gPtr2->getScienceData(playerGuild::guild_level).LV);
		vFlagLevel.push_back(gPtr2->getScienceData(playerGuild::flag_level).LV);
		vCreateTime.push_back(cur_time - gPtr2->getCreateTime());
		memList.clear();
		gPtr2->getAllMembers(memList);
		vMemberNum.push_back(memList.size());

		vGuildLevelIdxAft = vGuildLevelIdxBef;
		vFlagLevelIdxAft = vFlagLevelIdxBef;
		vMemberNumIdxAft = vMemberNumIdxBef;
		vCreateTimeIdxAft = vCreateTimeIdxBef;

		insert_sort(vGuildLevel, vGuildLevelIdxAft);
		insert_sort(vFlagLevel, vFlagLevelIdxAft);
		insert_sort(vMemberNum, vMemberNumIdxAft);
		insert_sort(vCreateTime, vCreateTimeIdxAft);

		vector<int> scores(3);
		get_score(vGuildLevelIdxBef, vGuildLevelIdxAft, scores, 1000);
		get_score(vFlagLevelIdxBef, vFlagLevelIdxAft, scores, 100);
		get_score(vMemberNumIdxBef, vMemberNumIdxAft, scores, 10);
		get_score(vCreateTimeIdxBef, vCreateTimeIdxAft, scores, 1);

		int worstGuild = scores[0];
		unsigned worstIndex = 0;
		for (unsigned idxGuild = 1; idxGuild < scores.size(); idxGuild++)
		{
			if (scores[idxGuild] < worstGuild)
			{
				worstGuild = scores[idxGuild];
				worstIndex = idxGuild;
			}
		}

		return worstIndex;
	}

	void guild_battle_system::guildBattleApplyReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int areaID = js_msg[0u].asInt();

		if (state_ != state_apply)
		{//非报名时间
			Return(r, 1);
		}

		int guildID = d->Guild.getGuildID();
		if (guildID == -1)
			Return(r, -1);

		Guild::guildPtr gPtr = guild_sys.getGuild(guildID);
		int gOfficial = gPtr->getOffcial(d->playerID);
		if (gOfficial == -1 || gOfficial > Guild::guild_vice_leader)
		{//不是副会长或会长
			Return(r, 2);
		}

		starAreaConfig cfg = getAreaConfig(areaID);
		if (cfg == starAreaNullConfig)
		{
			Return(r, -1);
		}

		starAreaData &aData = getAreaData(areaID);
		if (aData == starAreaData::sNull)
		{
			Return(r, -1);
		}

		int contenderAreaID, contenderIndex;
		getGuildContender(gPtr->guildName, contenderAreaID, contenderIndex);
		if (contenderIndex != -1)
		{//已经报名了的就不能再报名了
			Return(r, 3);
		}

		if (gPtr->getScienceData(playerGuild::guild_level).LV < cfg.guildLevelCondition_)
		{//星盟等级不够
			Return(r, 4);
		}

		int occupyAreaID;
		getGuildOccupyArea(gPtr->guildName, occupyAreaID);
		if (occupyAreaID == aData.areaID_)
		{//不可报名已经被自己星盟占领的星域
			Return(r, 5);
		}

// 		if (d->Base.getLevel() < cfg.levelCondition_)
// 			Return(r, 7);//星球开放等级限制
		
		if (aData.contender_[0] != "" && aData.contender_[1] != "")
		{
			Guild::guildPtr gPtr1 = guild_sys.getGuild(aData.contender_[0]);
			Guild::guildPtr gPtr2 = guild_sys.getGuild(aData.contender_[1]);
			int badGuild = chooseGuild(gPtr, gPtr1, gPtr2);

			Guild::guildPtr tempGPtr;
			if (badGuild == 2)
				tempGPtr = gPtr2;
			else if(badGuild == 1)
				tempGPtr = gPtr1;
			else//星盟不够给力
				Return(r, 6);

			if (badGuild == 2)
				aData.contender_[1] = gPtr->guildName;
			else if(badGuild == 1)
				aData.contender_[0] = gPtr->guildName;
			else
				Return(r, 6);

			if (gPtr1 != Guild::guildPtr() && gPtr2 != Guild::guildPtr())
			{
				Json::Value chatContent;
				chatContent["gna"] = gPtr->guildName;
				chatContent[guild_battle_def::strAreaID] = areaID;
				chat_sys.sendToGuildBroadCastCommon(tempGPtr->guildID, BROADCAST::guild_battle_apply_replace, chatContent);
			}
		}
		else if (aData.contender_[0] != "")
		{
			aData.contender_[1] = gPtr->guildName;
		}
		else if (aData.contender_[1] != "")
		{
			aData.contender_[0] = gPtr->guildName;
		}
		else
		{
			aData.contender_[0] = gPtr->guildName;
		}

		if (occupyAreaID != -1)
		{
			starAreaData &occupyData = getAreaData(occupyAreaID);
			occupyData.occupier_ = "";
			saveAreaData(occupyAreaID);
		}

		saveAreaData(areaID);

		Json::Value updateJson;
		updateJson[msgStr][0u] = areaID;
		string strUpdate = updateJson.toStyledString();
		na::msg::msg_json mj(m._player_id, m._net_id, gate_client::guild_battle_single_update_req, strUpdate);
		player_mgr.postMessage(mj);

		Json::Value chatContent;
		chatContent[guild_battle_def::strAreaID] = areaID;
		chat_sys.sendToGuildBroadCastCommon(guildID, BROADCAST::guild_battle_apply_success, chatContent);

		{
			//每次成功报名的结果记录（玩家ID、星盟身份，格子1，格子2）
			unsigned cur_time = na::time_helper::get_current_time();
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(areaID));
			fields.push_back(gPtr->guildName);
			fields.push_back(aData.contender_[0]);
			fields.push_back(aData.contender_[1]);
			StreamLog::Log(guild_battle_def::strMysqlLogGuildBattle, d, boost::lexical_cast<string, int>(cur_time), 
				"", fields, log_guild_battle_apply);
		}

		Return(r, 0);
	}

	void guild_battle_system::guildBattleEnterReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		int gid = d->Guild.getGuildID();
		if (gid == -1)
			Return(r, -1);

		Guild::guildPtr gPtr = guild_sys.getGuild(gid);
		string guildName = gPtr->guildName;
		int contenderAreaID, contenderIndex, fighterNum = 0;
		getGuildContender(guildName, contenderAreaID, contenderIndex);

		int occupyAreaID;
		getGuildOccupyArea(gPtr->guildName, occupyAreaID);

		if (contenderAreaID == -1 && occupyAreaID == -1)//没报名没占领
			Return(r, 2);

		int curAreaID = contenderAreaID > 0 ? contenderAreaID : occupyAreaID;
		int curIndex = contenderIndex > -1 ? contenderIndex : 2;

		if (occupyAreaID > -1)
			curIndex = 2;//报名了并且第一场胜利了

		starAreaConfig cfg = getAreaConfig(curAreaID);
		if (cfg == starAreaNullConfig)
			Return(r, -1);

		starAreaData& aData = getAreaData(curAreaID);
		fighterNum = aData.playerData_[curIndex].size();

		if (fighterNum == 50)//人数满
			Return(r, 3);

		if (state_ >= state_first_fight && contenderIndex == 0 && occupyAreaID < 1)
		{//第一场参战的队伍在第一场参战失败后提示“您所在的星盟首战失利，无法继续争夺领地”
			Return(r, 13);
		}

		if (state_ != state_first_join_team
			&& state_ != state_first_fight
			&& state_ != state_second_join_team)//时间
			Return(r, 1);

		if (state_ == state_first_join_team && (curIndex != 0 && curIndex != 2)) //第一场只能1队和占领者入场
			Return(r, 4);
		if (state_ == state_second_join_team && (curIndex != 1 && curIndex != 2)) //第二场只能2队和占领者入场
			Return(r, 5);
		if ((state_ == state_second_join_team || state_ == state_first_fight) && aData.contender_[1] == "")
			Return(r, 12);//第二场当没有挑战者的时候不能入场 || “没有其它星盟争夺该星球，无需参战”

		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time - d->Guild.getJoinTime() < 2 * 3600)
			Return(r, 6); //新人满2小时才能入场

		if (d->Embattle.isNullFormation())
			Return(r, 7); //阵上必须有舰长

		if (!aData.limit_[curIndex].isPermit(d))
			Return(r, 8); //限制条件

// 		if (d->Base.getLevel() < cfg.levelCondition_)
// 			Return(r, 9); //没达到开放条件

		if (aData.finishTheFirstBattle_ == guild_battle_state_ing)
			Return(r, 11);//“战斗还在继续中，请稍后再来参战”

		if (aData.finishTheSecondBattle_ == guild_battle_state_ing)
			Return(r, 11);

		if (aData.canLevyInAdvance(guildName))
			Return(r, 14);

		bool isInList = false;
		for (vector<playerDataPtr>::iterator it = aData.playerData_[curIndex].begin(); it != aData.playerData_[curIndex].end(); it++)
		{
			if (*it == d)
			{
				isInList = true;
				break;
			}
		}

		if (isInList)
			Return(r, 10);

		aData.playerData_[curIndex].push_back(d);
		map<int, fighterDataPtr>::iterator itFighter = aData.fighterData_[curIndex].find(d->playerID);
		if (itFighter == aData.fighterData_[curIndex].end())
		{
			fighterDataPtr tempPtr = createFighterData();
			aData.fighterData_[curIndex].insert(make_pair(d->playerID, tempPtr));
		}
		if (state_ == state_first_join_team)
		{
			aData.enterFirstList_.insert(d->playerID);
		}
		if (state_ == state_second_join_team)
		{
			aData.enterSecondList_.insert(d->playerID);
		}
		enterUpdate(curIndex, curAreaID, d);
		Return(r, 0);
	}

	void guild_battle_system::guildBattleInviteReq( msg_json& m, Json::Value& r )
	{
		AsyncGetPlayerData(m);

		if (state_ != state_first_join_team && state_ != state_second_join_team)//时间
			Return(r, 1);

		int gid = d->Guild.getGuildID();
		if (gid == -1)
			Return(r, -1);

		Guild::guildPtr gPtr = guild_sys.getGuild(gid);
		string guildName = gPtr->guildName;
		int contenderAreaID, contenderIndex, fighterNum = 0;
		getGuildContender(guildName, contenderAreaID, contenderIndex);

		int occupyAreaID;
		getGuildOccupyArea(gPtr->guildName, occupyAreaID);

		if (contenderAreaID == -1 && occupyAreaID == -1)//没报名没占领
			Return(r, 2);

		int curAreaID = contenderAreaID > 0 ? contenderAreaID : occupyAreaID;
		int curIndex = contenderIndex > -1 ? contenderIndex : 2;

		if (occupyAreaID > -1)
			curIndex = 2;//报名了并且第一场胜利了

		starAreaConfig cfg = getAreaConfig(curAreaID);
		if (cfg == starAreaNullConfig)
			Return(r, -1);

		int official = gPtr->getOffcial(d->playerID);
		if (official != Guild::guild_leader && official != Guild::guild_vice_leader)
			Return(r, 3);

		starAreaData& aData = getAreaData(curAreaID);
		int curTime = na::time_helper::get_current_time();
		if (curTime < aData.inviteTime_)
			Return(r, 4);

		if (aData.canLevyInAdvance(guildName))
			Return(r, 5);

//		aData.hasInvite_ = true;
		aData.inviteTime_ = curTime + 60;
		fighterNum = aData.playerData_[curIndex].size();
		Json::Value chatContent;
		string strGuildName = "gna";
		chatContent[guild_battle_def::strFighterNum] = fighterNum;
		chatContent[strGuildName] = gPtr->guildName;
		chatContent[guild_battle_def::strAreaID] = curAreaID;
		chat_sys.sendToGuildBroadCastCommon(gid, BROADCAST::guild_battle_to_guild_invite, chatContent);
		enterUpdate(curIndex, curAreaID, d);
		Return(r, 0);
	}

	void guild_battle_system::guildBattleExitReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
//		LogI << __FUNCTION__ << LogEnd;
		if (state_ != state_first_join_team && state_ != state_second_join_team
			&& state_ != state_first_fight && state_ != state_second_fight)//时间
			Return(r, -1);

		int gid = d->Guild.getGuildID();
		if (gid == -1)
			Return(r, -1);
		
		Guild::guildPtr gPtr = guild_sys.getGuild(gid);
		string guildName = gPtr->guildName;
		int contenderAreaID, contenderIndex, fighterNum = 0;
		getGuildContender(guildName, contenderAreaID, contenderIndex);

		int occupyAreaID;
		getGuildOccupyArea(gPtr->guildName, occupyAreaID);

		if (contenderAreaID == -1 && occupyAreaID == -1)//没报名没占领
			Return(r, -1);

		int curAreaID = contenderAreaID > 0 ? contenderAreaID : occupyAreaID;
		int curIndex = contenderIndex > -1 ? contenderIndex : 2;

		if (occupyAreaID > -1)
			curIndex = 2;

		starAreaConfig cfg = getAreaConfig(curAreaID);
		if (cfg == starAreaNullConfig)
			Return(r, -1);

		bool isInList = false;
		starAreaData& aData = getAreaData(curAreaID);
		for (vector<playerDataPtr>::iterator it = aData.playerData_[curIndex].begin(); it != aData.playerData_[curIndex].end(); it++)
		{
			if (*it == d)
			{
				isInList = true;
				aData.playerData_[curIndex].erase(it);
				break;
			}
		}

		if (!isInList)
			Return(r, 1);

		Return(r, 0);
	}

	void guild_battle_system::guildBattleEnterUpdateReq(msg_json& m, Json::Value& r)
	{//如果是第一场-只能1队入场，第二场-2队入场，2对变攻方，1队变守方，1队可能是npc，
		AsyncGetPlayerData(m);

		if (state_ != state_first_join_team && state_ != state_second_join_team
			&& state_ != state_first_fight && state_ != state_second_fight)//时间
			Return(r, 1);

		int gid = d->Guild.getGuildID();
		if (gid == -1)
			Return(r, -1);

		Guild::guildPtr gPtr = guild_sys.getGuild(gid);
		string guildName = gPtr->guildName;
		int contenderAreaID, contenderIndex;
		getGuildContender(guildName, contenderAreaID, contenderIndex);

		int occupyAreaID;
		getGuildOccupyArea(gPtr->guildName, occupyAreaID);

		if (contenderAreaID == -1 && occupyAreaID == -1)//没报名
			Return(r, 2);

		int curAreaID = contenderAreaID > 0 ? contenderAreaID : occupyAreaID;
		int curIndex = contenderIndex > -1 ? contenderIndex : 2;

		if (occupyAreaID > -1)
			curIndex = 2;

		if (state_ == state_first_join_team && (curIndex != 0 && curIndex != 2)) //第一场只能1队和占领者入场
			Return(r, 4);
		if (state_ == state_second_join_team && (curIndex != 1 && curIndex != 2)) //第一场只能2队和占领者入场
			Return(r, 5);

		starAreaData& aData = getAreaData(curAreaID);
		if (aData.finishTheFirstBattle_ == guild_battle_state_ing)
			Return(r, 6);//战斗还在进行中

		if (aData.finishTheSecondBattle_ == guild_battle_state_ing)
			Return(r, 6);

		if (aData.canLevyInAdvance(guildName))
			Return(r, 7);

		bool isInList = false;
		for (vector<playerDataPtr>::iterator it = aData.playerData_[curIndex].begin(); 
			it != aData.playerData_[curIndex].end(); it++)
		{
			if (*it == d)
			{
				isInList = true;
				break;
			}
		}
		if (!isInList)
			Return(r, -1);

		enterUpdate(curIndex, curAreaID, d);
	}

	void guild_battle_system::playerLogout(int player_id)
	{
		string s = "";
		na::msg::msg_json mj(gate_client::guild_battle_exit_req, s);
		mj._player_id = player_id;
		Json::Value jresp;
		guildBattleExitReq(mj, jresp);
	}

	void guild_battle_system::enterUpdate( int curIndex, int curAreaID, playerDataPtr pdata, int updateSide /*= -1*/ )
	{//攻防连状态,vector<玩家名string>玩家列表,有玩家入场的时候，推送一条入场update_resp协议
		Json::Value updateJson;
		starAreaData &adata = getAreaData(curAreaID);
		map<int, fighterDataPtr>::iterator it = adata.fighterData_[curIndex].find(pdata->playerID);

		int leftSide = -1;
		if (state_ == state_first_join_team || state_ == state_first_fight)
			leftSide = 0;
		else if (state_ == state_second_join_team || state_ == state_second_fight)
			leftSide = 1;
		else
			return;

		if (state_ == state_first_fight && adata.finishTheFirstBattle_ == guild_battle_state_null)
			leftSide = 1;//第一场打完之后能马上进入第二场

		updateJson[guild_battle_def::strInspireGoldCost] = getInspireGoldCost();
		updateJson[guild_battle_def::strInspireKejiCost] = getInspireKejiCost(pdata);
		updateJson[guild_battle_def::strAtkDefWinStreak] = Json::arrayValue;
		if (it != adata.fighterData_[curIndex].end())
		{
			updateJson[guild_battle_def::strAtkDefWinStreak].append(it->second->atkValue_);
			updateJson[guild_battle_def::strAtkDefWinStreak].append(it->second->defValue_);
			updateJson[guild_battle_def::strAtkDefWinStreak].append(it->second->winnerStreak_);
			updateJson[guild_battle_def::strHasInvite] = adata.hasInvite_; //保留，虽然用不到
		}

		if (updateSide == -1 || updateSide == 0)
		{
			updateJson["g1"] = adata.contender_[leftSide];
			updateJson[guild_battle_def::strFighterList1] = Json::arrayValue;
			for (vector<playerDataPtr>::iterator itPlayer1 = adata.playerData_[leftSide].begin(); itPlayer1 != adata.playerData_[leftSide].end(); itPlayer1++)
			{
				Json::Value pItem;
				map<int, fighterDataPtr>::iterator itFighter = adata.fighterData_[leftSide].find((*itPlayer1)->playerID);
				if (itFighter == adata.fighterData_[leftSide].end())
					return;
				pItem[playerNameStr] = (*itPlayer1)->Base.getName();
				pItem["t"] = itFighter->second->sumAll();
				updateJson[guild_battle_def::strFighterList1].append(pItem);
			}
		}

		if (updateSide == -1 || updateSide == 1)
		{
			updateJson["g2"] = adata.occupier_;
			updateJson[guild_battle_def::strFighterList2] = Json::arrayValue;
			if (adata.occupier_ == "")
			{
				updateJson[guild_battle_def::strStamp] = 0;
			}
			else
			{
				updateJson[guild_battle_def::strStamp] = 1;
				for (vector<playerDataPtr>::iterator itPlayer2 = adata.playerData_[2].begin(); itPlayer2 != adata.playerData_[2].end(); itPlayer2++)
				{
					Json::Value pItem;
					map<int, fighterDataPtr>::iterator itFighter = adata.fighterData_[2].find((*itPlayer2)->playerID);
					if (itFighter == adata.fighterData_[2].end())
						return;
					pItem[playerNameStr] = (*itPlayer2)->Base.getName();
					pItem["t"] = itFighter->second->sumAll();
					updateJson[guild_battle_def::strFighterList2].append(pItem);
				}
			}
		}

		Json::Value r;
		r[msgStr][0u] = 0;
		r[msgStr][1u] = updateJson;
		r[msgStr][2u] = getBattleTime(state_);
		string s = r.toStyledString();
		msg_json mj(gate_client::guild_battle_enter_update_resp, s);
//		cout << __FUNCTION__ << ":s" << endl;
		mj._player_id = pdata->playerID;
		mj._net_id = pdata->netID;
		player_mgr.sendToPlayer(mj);
	}

	void guild_battle_system::guildBattleEnterLimitReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int limitLevel = js_msg[0u].asInt();
		int limitProcess = js_msg[1].asInt();
		int limitRank = js_msg[2].asInt();

		int guildID = d->Guild.getGuildID();
		if (guildID == -1)
			Return(r, -1);
		string guildName = guild_sys.getGuildName(guildID);
		int contenderAreaID, contenderIndex;
		getGuildContender(guildName, contenderAreaID, contenderIndex);
		
		Guild::guildPtr gPtr = guild_sys.getGuild(guildID);
		if (gPtr == Guild::guildPtr())
			Return(r, -1);
		int occupyAreaID;
		getGuildOccupyArea(gPtr->guildName, occupyAreaID);

		if (contenderAreaID == -1 && occupyAreaID == -1)//没报名
			Return(r, 3);

		//根据策划的要求，取消部分时间限制
		if (state_ != state_apply/* && state_ != state_first_join_team && state_ != state_second_join_team*/)
			Return(r, 2);//没到时间
		
		int gOfficial = gPtr->getOffcial(d->playerID);
		if (gOfficial == -1 || gOfficial > Guild::guild_vice_leader)			
			Return(r, 1);//是副会长或会长

		int curAreaID = contenderAreaID > 0 ? contenderAreaID : occupyAreaID;
		int curIndex = contenderIndex > -1 ? contenderIndex : 2;

		if (occupyAreaID > -1)
			curIndex = 2;

		starAreaData& aData = getAreaData(curAreaID);

		aData.limit_[curIndex].level_ = limitLevel;
		aData.limit_[curIndex].process_ = limitProcess;
		aData.limit_[curIndex].arenaRank_ = limitRank;
		aData.limit_[curIndex].valid_ = true;

		Return(r, 0);
	}

	void guild_battle_system::guildBattleEnterCancelLimitReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		int guildID = d->Guild.getGuildID();
		if (guildID == -1)
			Return(r, -1);

		Guild::guildPtr gPtr = guild_sys.getGuild(guildID);
		int gOfficial = gPtr->getOffcial(d->playerID);
		if (gOfficial == -1 || gOfficial > Guild::guild_vice_leader)			
			Return(r, 1);//是副会长或会长
		if (state_ != state_apply/* && state_ != state_first_join_team && state_ != state_second_join_team*/)
			Return(r, 5);//不在时间内没提示

		string guildName = guild_sys.getGuildName(guildID);
		int contenderAreaID, contenderIndex;
		getGuildContender(guildName, contenderAreaID, contenderIndex);

		int occupyAreaID;
		getGuildOccupyArea(gPtr->guildName, occupyAreaID);

		if (contenderAreaID == -1 && occupyAreaID == -1)//没报名
			Return(r, 4);

		int curAreaID = contenderAreaID > 0 ? contenderAreaID : occupyAreaID;
		int curIndex = contenderIndex > -1 ? contenderIndex : 2;

		if (occupyAreaID > -1)
			curIndex = 2;

		starAreaData& aData = getAreaData(curAreaID);
		if (!aData.limit_[curIndex].valid_)
			Return(r, 3);//已经是取消状态
		aData.limit_[curIndex].valid_ = false;

		Return(r, 0);
	}

	void guild_battle_system::guildBattleInspireReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int option_gold = 0;
		int option_keji = 1;
		int inspireOption = js_msg[0].asInt();
		r[msgStr][1] = 0;
		if (inspireOption != option_gold && inspireOption != option_keji)
			Return(r, -1);
		if (state_ != state_first_join_team && state_ != state_second_join_team)
			Return(r, 1);//没到时间
		int gid = d->Guild.getGuildID();
		if (gid == -1)
			Return(r, -1);
		string guildName = guild_sys.getGuildName(gid);
		int contenderAreaID, contenderIndex;
		getGuildContender(guildName, contenderAreaID, contenderIndex);

		int occupyAreaID;
		getGuildOccupyArea(guildName, occupyAreaID);

		if (contenderAreaID == -1 && occupyAreaID == -1)//没报名
			Return(r, -1);

		int curAreaID = contenderAreaID > 0 ? contenderAreaID : occupyAreaID;
		int curIndex = contenderIndex > -1 ? contenderIndex : 2;

		if (occupyAreaID > -1)
			curIndex = 2;

		if (state_ == state_first_join_team && (curIndex != 0 && curIndex != 2)) //第一场只能1队和占领者入场
			Return(r, 5);
		if (state_ == state_second_join_team && (curIndex != 1 && curIndex != 2)) //第二场只能2队和占领者入场
			Return(r, 6);

		starAreaData& aData = getAreaData(curAreaID);
		if (aData.fighterData_[curIndex][d->playerID] == fighterDataPtr())
			Return(r, -1);
		if (aData.fighterData_[curIndex][d->playerID]->sumAll() >= 30)
			Return(r, 2);//满
		if (aData.canLevyInAdvance(guildName))
			Return(r, -1);
		int successRate = 0;
		int costResource = 0;
		if (inspireOption == option_gold)
		{
			int costGold = getInspireGoldCost();
			if (d->Base.getAllGold() < costGold)
				Return(r, 3);
			d->Base.alterBothGold(-costGold);
			successRate = 80;
			costResource = costGold;
		}
		else if (inspireOption == option_keji)
		{
			int costKeji = getInspireKejiCost(d);
			if (d->Base.getKeJi() < costKeji)
				Return(r, 3);
			d->Base.alterKeJi(-costKeji);
			successRate = 65;
			costResource = costKeji;
		}
		else
		{
		}
		successRate = int(successRate*(1-(aData.fighterData_[curIndex][d->playerID]->sumAll()-1)*0.03));
		int rp = commom_sys.randomBetween(1, 100);
		int opt = commom_sys.randomBetween(1,3);
		int chValue = 0;
		int ret = 0;
		int isSuccess = 0;
		if (rp <= successRate)
		{//成功
			int count = 0;
			while (1)
			{
				if (aData.fighterData_[curIndex][d->playerID]->atkValue_ < 10)
				{
					count++;
					if (count == opt)
					{
						aData.fighterData_[curIndex][d->playerID]->atkValue_++;
						chValue = 1;
						break;
					}
				}
				if (aData.fighterData_[curIndex][d->playerID]->defValue_ < 10)
				{
					count++;
					if (count == opt)
					{
						aData.fighterData_[curIndex][d->playerID]->defValue_++;
						chValue = 2;
						break;
					}
				}
				if (aData.fighterData_[curIndex][d->playerID]->winnerStreak_ < 10)
				{
					count++;
					if (count == opt)
					{
						aData.fighterData_[curIndex][d->playerID]->winnerStreak_++;
						chValue = 3;
						break;
					}
				}
			}
			r[msgStr][1] = chValue;
			ret = 0;
			isSuccess = 0;
		}
		else
		{//鼓舞失败
			ret = 4;
			isSuccess = 1;
		}

		{
			vector<string> fields;
			unsigned cur_time = na::time_helper::get_current_time();
			fields.push_back(boost::lexical_cast<string, int>(costResource));
			fields.push_back(boost::lexical_cast<string, int>(isSuccess));
			if (inspireOption == option_gold)
			{//信用点鼓舞的流水（时间、玩家ID、消耗军功数量、鼓舞结果）
				StreamLog::Log(guild_battle_def::strMysqlLogGuildBattle, d, boost::lexical_cast<string, int>(cur_time), 
					"", fields, log_guild_battle_inspire_gold);
			}
			else
			{//科技点鼓舞的流水（时间、玩家ID、消耗军功数量、鼓舞结果）
				StreamLog::Log(guild_battle_def::strMysqlLogGuildBattle, d, boost::lexical_cast<string, int>(cur_time), 
					"", fields, log_guild_battle_inspire_keji);
			}
		}

		Return(r, ret);
	}

	void guild_battle_system::checkAdvanceEnd()
	{
		unsigned curTime = na::time_helper::get_current_time();
		for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
		{
			starAreaData &aData = getAreaData(it->first);
			if (aData.canLevyInAdvance(aData.occupier_))
			{
				aData.startLevyTime_ = na::time_helper::get_time_zero(curTime) + 20 * 3600 + 00 * 60;
				aData.finishTheFirstBattle_ = guild_battle_state_null;
				saveAreaData(it->first);
			}
		}
	}

	void guild_battle_system::fight(int opt)
	{
		if (opt != 1 && opt != 2)
			return;
		int dataIndex = opt - 1;

		bSideReCall f;
		f = BindSideReCall(guild_battle_system::guildBattleSys, guild_battle_system::addBattleBuff);

		unsigned curTime = na::time_helper::get_current_time();
		for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
		{
			starAreaData &aData = getAreaData(it->first);
			if (aData.canLevyInAdvance(aData.occupier_))
			{
				continue;
			}
			if (aData.contender_[dataIndex] == "")
			{//没人报名，跳过战斗
				bool hasModify = false;
				if (opt == 1)
				{
					aData.battleResult_[0] = 0;
					aData.startLevyTime_ = na::time_helper::get_time_zero(curTime) + 20 * 3600 + 10 * 60;
					aData.finishTheFirstBattle_ = guild_battle_state_null;
					hasModify = true;
				}
				if (opt == 2)
				{
					if (aData.contender_[0] !=  "")
					{
						aData.battleResult_[1] = 0;
						aData.startLevyTime_ = na::time_helper::get_time_zero(curTime) + 20 * 3600 + 20 * 60;
						hasModify = true;
					}
					aData.finishTheSecondBattle_ = guild_battle_state_null;
				}
				if (hasModify)
					saveAreaData(it->first);
				continue;
			}

			teamMapDataPtr tptr = it->second.teamMapData_;
			vector<mapDataPtr>& vec = tptr->npcArmy;

			//攻击方
			BattlefieldMuilt mu;
			for (unsigned playerIndex = 0; playerIndex < starAreaData_[it->first].playerData_[dataIndex].size(); playerIndex++)
			{
				int curPlayerId = starAreaData_[it->first].playerData_[dataIndex][playerIndex]->playerID;
				task_sys.updateBranchTaskInfo(starAreaData_[it->first].playerData_[dataIndex][playerIndex], _task_guild_battle_times);
				fdata = starAreaData_[it->first].fighterData_[dataIndex][curPlayerId];

				mu.atkSide.push_back(battleSide::Create(starAreaData_[it->first].playerData_[dataIndex][playerIndex], tptr, f));
				fdata = fighterDataPtr();
			}

			//防守方
			if (starAreaData_[it->first].occupier_ == "")
			{
				for (unsigned npcIndex = 0; npcIndex < vec.size(); npcIndex++)
				{
					mu.defSide.push_back(battleSide::Create(tptr, vec[npcIndex]));
				}
			}
			else
			{
				for (unsigned playerIndex = 0; playerIndex < starAreaData_[it->first].playerData_[2].size(); playerIndex++)
				{
					int curPlayerId = starAreaData_[it->first].playerData_[2][playerIndex]->playerID;
					task_sys.updateBranchTaskInfo(starAreaData_[it->first].playerData_[2][playerIndex], _task_guild_battle_times);
					fdata = starAreaData_[it->first].fighterData_[2][curPlayerId];

					battleSidePtr bsPtr = battleSide::Create(starAreaData_[it->first].playerData_[2][playerIndex], tptr, f);
					mu.defSide.push_back(bsPtr);
					fdata = fighterDataPtr();
				}
			}

			//清数据，战斗
			for (unsigned i = 0; i < 3; i++)
			{
				starAreaData_[it->first].playerData_[i].clear();
				starAreaData_[it->first].fighterData_[i].clear();
				starAreaData_[it->first].limit_->reset();
				starAreaData_[it->first].hasInvite_ = false;
			}
			mu.ParamJson[BATTLE::strWarMapID] = it->first;
			mu.ParamJson[BATTLE::strTeamStep] = it->second.teamMapData_->battleSize;
			mu.ParamJson["opt"] = opt;
			mu.ParamJson[guild_battle_def::strOccupier] = starAreaData_[it->first].occupier_;
			mu.ParamJson[guild_battle_def::strContender1] = starAreaData_[it->first].contender_[0];
			mu.ParamJson[guild_battle_def::strContender2] = starAreaData_[it->first].contender_[1];
			if (starAreaData_[it->first].reportLink_ > 100)
				starAreaData_[it->first].reportLink_ = 1;
			mu.ParamJson[guild_battle_def::strReportLink] = starAreaData_[it->first].reportLink_++;
			saveAreaData(it->first);
			if (opt == 1)
			{
				starAreaData_[it->first].finishTheFirstBattle_ = guild_battle_state_ing;
			}
			if (opt == 2)
			{
				starAreaData_[it->first].finishTheSecondBattle_ = guild_battle_state_ing;
			}
			battle_sys.PostTeamBattle(mu, team_battle_guild_type);
		}
	}

// 	void guild_battle_system::fight2()
// 	{
// 		for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
// 		{
// 			if (starAreaData_[it->first].playerData_[1].size() == 0)
// 			{
// 				continue;
// 			}
// 			BattlefieldMuilt mu;
// 			for (unsigned playerIndex = 0; playerIndex < starAreaData_[it->first].playerData_[1].size(); playerIndex++)
// 			{
// 				mu.atkSide.push_back(battleSide::Create(starAreaData_[it->first].playerData_[1][playerIndex]));
// 			}
// 			if (starAreaData_[it->first].battleResult_[0] == 1)
// 			{
// 				for (unsigned playerIndex = 0; playerIndex < starAreaData_[it->first].playerData_[0].size(); playerIndex++)
// 				{
// 					mu.defSide.push_back(battleSide::Create(starAreaData_[it->first].playerData_[0][playerIndex]));
// 				}
// 			}
// 			else
// 			{
// 				teamMapDataPtr tptr = it->second.teamMapData_;
// 				vector<mapDataPtr>& vec = tptr->npcArmy;
// 				for (unsigned npcIndex = 0; npcIndex < vec.size(); npcIndex++)
// 				{
// 					mu.defSide.push_back(battleSide::Create(tptr, vec[npcIndex]));
// 				}
// 			}
// 
// 			for (unsigned i = 0; i < 3; i++)
// 			{
// 				starAreaData_[it->first].playerData_[i].clear();
// 				starAreaData_[it->first].fighterData_[i].clear();
// 				starAreaData_[it->first].limit_->reset();
// 			}
// 			mu.ParamJson[BATTLE::strWarMapID] = it->first;
// 			battle_sys.PostTeamBattle(mu, team_battle_guild_type);
// 		}
//	}

	void guild_battle_system::addBattleBuff( playerDataPtr pdata, battleSide& side )
	{
		//side.addMaxWin(fdata->winnerStreak_ / 5);
//		side.maxWin = 3;
		side.maxWin += int(fdata->winnerStreak_ / 5);
		side.inspireNum = fdata->sumAll();
		armyVec vec = side.getMember(MO::mem_null);
		for (armyVec::iterator it = vec.begin(); it != vec.end(); it++)
		{
			(*it)->addCharacter[idx_phyAddHurt] += (fdata->atkValue_ * 0.01);
			(*it)->addCharacter[idx_warAddHurt] += (fdata->atkValue_ * 0.01);
			(*it)->addCharacter[idx_magicAddHurt] += (fdata->atkValue_ * 0.01);

			(*it)->addCharacter[idx_phyCutHurt] += (fdata->defValue_ * 0.01);
			(*it)->addCharacter[idx_warCutHurt] += (fdata->defValue_ * 0.01);
			(*it)->addCharacter[idx_magicCutHurt] += (fdata->defValue_ * 0.01);
		}
		fdata = fighterDataPtr();
	}

	void guild_battle_system::teamGuildRespon(BattlefieldMuilt field, Json::Value report)
	{//结算军团贡献
		State::setState(gate_client::guild_battle_fight_req);
		NumberCounter::Step();
		int battleResult = report[BATTLE::strBattleResult].asInt();
		int mapID = field.ParamJson[BATTLE::strWarMapID].asInt();

//		{
//			battleResult = 1;
//			report[BATTLE::strBattleResult] = 1;
//		}

		string strAtkReward = BATTLE::strBattleReward+"Atk";
		string strDefReward = BATTLE::strBattleReward+"Def";
		report[BATTLE::strBattleReward] = Json::arrayValue;
		for (unsigned i =0 ; i < field.atkSide.size(); ++i)
		{
			battleSidePtr side = field.atkSide[i];
			playerDataPtr player = player_mgr.getPlayerMain(side->getSideID());
			if(player == playerDataPtr())continue;
			side->netID = player->netID;
			//奖励军团贡献
			do{
				int contributeRate = (battleResult == 1 ? 2 : 1);
				int contributeBase = int((player->Base.getLevel() + 50) * 100 * contributeRate);
				double winAdd = 0.0;
				if (side->getWinNum() >= 2)
				{
					winAdd = 0.1 + side->getWinNum() * 0.05;
				}
// 				int contributeExtra = int(contributeBase * winAdd);
// 				contributeBase = int(contributeBase * (1 + action_show_sys.get_action_show_par(star_field_battle, player)));
				int contribute = int(contributeBase * (1.0f + winAdd));
				int guildID = player->Guild.getGuildID();
				Guild::guildPtr gPtr = guild_sys.getGuild(guildID);
				gPtr->alterMemContribute(player, contribute);

				Json::Value SingleRW;
				SingleRW.append(0);
				SingleRW.append(player->Warstory.currentProcess);
				SingleRW.append(side->getName());
				SingleRW.append(player->Base.getLevel());
				SingleRW.append(side->getWinNum());
				SingleRW.append(contribute);
				SingleRW.append(0);
				report[BATTLE::strBattleReward].append(SingleRW);

			}while(false);
		}

		if (field.defSide.size() > 0)
		{
			battleSidePtr firstSide = field.defSide[0];
			if (firstSide->isPlayerSide())
			{
				for (unsigned i = 0; i < field.defSide.size(); i++)
				{
					battleSidePtr side = field.defSide[i];
					playerDataPtr player = player_mgr.getPlayerMain(side->getSideID());
					if(player == playerDataPtr())continue;
					side->netID = player->netID;

					//奖励军团贡献
					do{
						int contributeRate = battleResult == 1 ? 2 : 1;
						int contributeBase = int(player->Base.getLevel() * 100 * contributeRate);
						int contributeExtra = int(player->Base.getLevel() * 100 * (0.1+0.05*side->getWinNum()) * contributeRate);
						int contribute = contributeBase + contributeExtra;
						int guildID = player->Guild.getGuildID();
						Guild::guildPtr gPtr = guild_sys.getGuild(guildID);
						gPtr->alterMemContribute(player, contribute);

						Json::Value SingleRW;
						SingleRW.append(1);
						SingleRW.append(player->Warstory.currentProcess);
						SingleRW.append(side->getName());
						SingleRW.append(player->Base.getLevel());
						SingleRW.append(side->getWinNum());
						SingleRW.append(contribute);
						SingleRW.append(0);
						report[BATTLE::strBattleReward].append(SingleRW);
					}while(false);
				}
			}
		}

		helper_mgr.runSaveAndUpdate();

		{
			string atkName = field.ParamJson[guild_battle_def::strContender1].asString();
			if (field.ParamJson["opt"] == 1)
				atkName = field.ParamJson[guild_battle_def::strContender1].asString();
			else
				atkName = field.ParamJson[guild_battle_def::strContender2].asString();
			string defName = field.ParamJson[guild_battle_def::strOccupier].asString();

			//总的战斗：发生时间，游戏时间，军团名，攻守方，参战人数，攻击城池，结果
			//Log(logTable, player.playerID, player.Base.getName(), player.Base.getLevel(), targetID, targetData, Tag);
			for (unsigned i = 0; i < 2; i++)
			{
				vector<string> fds;
				unsigned cur_time = na::time_helper::get_current_time();
				fds.push_back(boost::lexical_cast<string, int>(cur_time));
				fds.push_back(boost::lexical_cast<string, int>(mapID));
				fds.push_back(boost::lexical_cast<string, int>(battleResult));
				fds.push_back(boost::lexical_cast<string, int>(i));
				if (i == 0)
				{
					fds.push_back(atkName);
					fds.push_back(boost::lexical_cast<string, int>(field.atkSide.size()));
				}
				else
				{
					fds.push_back(defName);
					fds.push_back(boost::lexical_cast<string, int>(field.defSide.size()));
				}
				int cur_year = season_sys.getYear(cur_time);
				int cur_season = season_sys.getSeason();
				StreamLog::Log(guild_battle_def::strMysqlLogGuildBattle, 0, "", 0,
					boost::lexical_cast<string, int>(cur_year), boost::lexical_cast<string, int>(cur_season), fds, log_guild_battle_total);
			}
		}

		unsigned cur_time = na::time_helper::get_current_time();
		starAreaData& aData = getAreaData(mapID);
		if (getState() == state_first_fight)
		{
			aData.battleResult_[0] = battleResult;
			if (battleResult == 1)
			{
				aData.occupier_ = aData.contender_[0];
			}
			if (aData.contender_[1] == "")
			{//第一场结束，当下一场没挑战者的时候不需要进行下一场战斗，提前记录星盟记事
				recordGuildBattleEvent(aData.areaID_);
				aData.startLevyTime_ = na::time_helper::get_time_zero(cur_time) + 20 * 3600 + 10 * 60;
				clearSingleAreaDataDaily(aData.areaID_);
			}
		}
		if (getState() == state_second_fight)
		{
			aData.battleResult_[1] = battleResult;
			if (battleResult == 1)
			{
				aData.occupier_ = aData.contender_[1];
			}
			aData.startLevyTime_ = na::time_helper::get_time_zero(cur_time) + 20 * 3600 + 20 * 60;
			recordGuildBattleEvent(aData.areaID_);
			clearSingleAreaDataDaily(aData.areaID_);
		}
		saveAreaData(aData.areaID_);

		_Battle_Post(boost::bind(&battle_system::DealTeamGuildReport, battle_system::battleSys, 
			field, report));
	}

	bool guild_battle_system::saveAreaData(int areaID)
	{
		mongo::BSONObj key = BSON(guild_battle_def::strAreaID << areaID);
		mongo::BSONObjBuilder obj;
		obj << guild_battle_def::strAreaID << areaID;
		obj << guild_battle_def::strOccupier << starAreaData_[areaID].occupier_;
		obj << guild_battle_def::strContender1 << starAreaData_[areaID].contender_[0];
		obj << guild_battle_def::strContender2 << starAreaData_[areaID].contender_[1];
		obj << guild_battle_def::strFightResult1 << starAreaData_[areaID].battleResult_[0];
		obj << guild_battle_def::strFightResult2 << starAreaData_[areaID].battleResult_[1];
		obj << guild_battle_def::strNextAreaSeasonUpdateTime << starAreaData_[areaID].nextSeasonUpdateTime_;
		obj << guild_battle_def::strNextAreaDayUpdateTime << starAreaData_[areaID].nextDayUpdateTime_;
		obj << guild_battle_def::strReportLink << starAreaData_[areaID].reportLink_;
		obj << guild_battle_def::strStartLevyTime_ << starAreaData_[areaID].startLevyTime_;

		return db_mgr.save_mongo(guild_battle_def::strGuildBattleDBData, key, obj.obj());
	}

	void guild_battle_system::getGuildContender(string guildName, int &contenderAreaID, int &contenderIndex)
	{
		contenderAreaID = -1;
		contenderIndex = -1;
		for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
		{
			if (starAreaData_[it->first].contender_[0] == guildName)
			{
				contenderAreaID = it->first;
				contenderIndex = 0;
				break;
			}
			if (starAreaData_[it->first].contender_[1] == guildName)
			{
				contenderAreaID = it->first;
				contenderIndex = 1;
				break;
			}
		}
	}

	void guild_battle_system::getGuildOccupyArea(string guildName, int &occupyAreaID)
	{
		occupyAreaID = -1;
		if (guildName == "")
			return;
		for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
		{
			if (starAreaData_[it->first].occupier_ == guildName)
			{
				occupyAreaID = it->first;
				break;
			}
		}
	}

	void guild_battle_system::chatBroadCastBeginJoinTeam(int opt)
	{
		if (opt != 1 && opt != 2)
			return;

		int dataIndex = opt - 1;
		int bid = 0;
		if (opt == 1)
			bid = BROADCAST::guild_battle_to_guild_fight_will_com_19_50;
		else
			bid = BROADCAST::guild_battle_to_guild_fight_will_com_20_40;
		for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
		{
			Json::Value chatContent;

			starAreaData &aData = getAreaData(it->first);
			chatContent[guild_battle_def::strAreaID] = it->first;
			chatContent["dn"] = aData.occupier_;
			chatContent["an"] = aData.contender_[dataIndex];

			Guild::guildPtr gPtrDef = guild_sys.getGuild(aData.occupier_);
			Guild::guildPtr gPtrAtk = guild_sys.getGuild(aData.contender_[dataIndex]);
			if (gPtrDef != Guild::guildPtr())
			{
				if (opt == 2 && gPtrAtk == Guild::guildPtr())
				{//第二场没有挑战者的时候不发送广播
				}
				else
				{
					Json::Value chatContent;
					chat_sys.sendToGuildBroadCastCommon(gPtrDef->guildID, bid, chatContent);
				}
			}

			if (gPtrAtk != Guild::guildPtr() && gPtrAtk != gPtrDef)
			{
				Json::Value chatContent;
				chat_sys.sendToGuildBroadCastCommon(gPtrAtk->guildID, bid, chatContent);
			}

			if (gPtrDef != Guild::guildPtr() || gPtrAtk != Guild::guildPtr())
			{
				if (opt == 2 && gPtrAtk == Guild::guildPtr())
				{//第二场没有挑战者的时候不发送广播
				}
				else
				{
					chat_sys.sendToAllBroadCastCommon(BROADCAST::guild_battle_begin_join_team, chatContent);
				}
			}
		}
	}

	int guild_battle_system::getBattleTime( int curState )
	{
		unsigned cur_time = na::time_helper::get_current_time();
		boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
		tm t = boost::posix_time::to_tm(p);
		unsigned standard_time = cur_time - 3600 * t.tm_hour - 60 * t.tm_min - t.tm_sec;

		if (curState == state_first_join_team)
			return standard_time + 3600 * 20 + 60 * 10;
		else if (curState == state_second_join_team)
			return standard_time + 3600 * 20 + 60 * 20;
		else
			return 0;
	}

	void guild_battle_system::recordGuildBattleEvent( int areaID )
	{
		starAreaData& aData = getAreaData(areaID);

		if (aData == starAreaData::sNull)
			return;
		if (aData.occupier_ == "")
			return;

		if (aData.contender_[0] == "" && aData.contender_[1] == "")
			return;

		guild_sys.addGuilBattleEvent(aData.occupier_, areaID);
		string s = guild_battle_def::strMysqlLogGuildBattle;

		Guild::guildPtr gPtr = guild_sys.getGuild(aData.occupier_);

		vector<string> fds;
		fds.push_back(aData.occupier_);
		fds.push_back(boost::lexical_cast<string, int>(areaID));
		fds.push_back(boost::lexical_cast<string, int>(gPtr->getKingdom()));
		unsigned cur_time = na::time_helper::get_current_time();
		StreamLog::Log(guild_battle_def::strMysqlLogGuildBattle, 0, "", 0, 
			boost::lexical_cast<string, int>(season_sys.getYear(cur_time)), 
			boost::lexical_cast<string, int>(season_sys.getSeason()), fds, log_guild_battle_final_result);
	}

	bool guild_battle_system::isSecondBattleEnd()
	{
		bool isEnd = true;
		for (starAreaConfigMap::iterator it = starAreaConfig_.begin(); it != starAreaConfig_.end(); it++)
		{
			if (starAreaData_[it->first].finishTheSecondBattle_ == guild_battle_state_ing)
			{
				isEnd = false;
			}
		}
		return isEnd;
	}

	void guild_battle_system::processRemoveGuild(string gName)
	{
		if (gName == "")
			return;

		for (starAreaConfigMap::iterator itCfg = starAreaConfig_.begin(); itCfg != starAreaConfig_.end(); itCfg++)
		{
			starAreaData &adata = getAreaData(itCfg->first);
			if (adata == starAreaData::sNull)
				continue;

			bool isModify = false;
			if (gName == adata.occupier_)
			{
				adata.occupier_ = "";
				isModify = true;
			}

			if (gName == adata.contender_[0])
			{
				adata.contender_[0] = "";
				isModify = true;
			}

			if (gName == adata.contender_[1])
			{
				adata.contender_[1] = "";
				isModify = true;
			}

			if (isModify)
				saveAreaData(itCfg->first);
		}
	}

	int guild_battle_system::getInspireGoldCost()
	{
		return 10;
	}

	int guild_battle_system::getInspireKejiCost(playerDataPtr player)
	{
		if (player == playerDataPtr())
			return 0;

		return player->Base.getLevel() * 3 + 100;
	}

	guildBattleRollBroadcast::guildBattleRollBroadcast()
	{
		br_19_30_ = false;
		br_20_00_ = false;
		br_20_11_ = false;
		winterReset_ = false;
		afterBattleClear_ = false;
		stateNullClear_ = false;
		nextUpdateTime_ = 0;
	}

	void guildBattleRollBroadcast::reset()
	{
		br_19_30_ = false;
		br_20_00_ = false;
		br_20_11_ = false;
		winterReset_ = false;
		afterBattleClear_ = false;
		stateNullClear_ = false;
	}

	void guildBattleRollBroadcast::refresh()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > nextUpdateTime_)
		{
			reset();
			nextUpdateTime_ = na::time_helper::get_next_update_time(cur_time);
		}
	}


	void annouceData::refresh()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > nextUpdateTime_)
		{
			memset(this, 0, sizeof(*this));
			nextUpdateTime_ = na::time_helper::get_next_update_time(cur_time);
		}
	}

	annouceData::annouceData()
	{

	}

}
