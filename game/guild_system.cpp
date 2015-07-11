#include "guild_system.h"
#include "Glog.h"
#include "response_def.h"
#include "gate_game_protocol.h"
#include "record_system.h"
#include "task_system.h"
#include "chat_system.h"
#include "gate_game_protocol.h"
#include "email_system.h"
#include "season_system.h"
#include "mine_resource_system.h"
#include "guild_battle_system.h"
#include "world_boss_system.h"
#include "activity_system.h"

using namespace mongo;

namespace gg
{
	const static string strEventID = "eID";
	const static string strEventData = "eD";
	const static string strEventTrriger = "eT";

	static bool removeHelpTick = false;

	const static string logSystemGuild = "log_sys_guild";
	enum{
		log_tag_create,
		log_tag_dismiss,
		log_tag_kick,
		log_tag_add,
		log_tag_apply,
		log_tag_leave,
		log_tag_upgrade_member,
		log_tag_words,
		log_tag_donate,
		log_tag_science,
		log_tag_change_leader,
		log_tag_reject_join,
		log_tag_allow_join,
		log_set_first_science,
		log_set_flag_icon,
		log_guild_rank,
		log_tag_motify_name,
		log_member_contribute,
	};

	enum
	{
		guild_event_create,
		guild_event_trun_leader,
		guild_event_replace_leader,
		guild_event_member_upgrade,
		guild_event_donate,
		guild_event_hold_place,
		guild_event_add_member,
		guild_event_remove_member,
		guild_event_change_flag_icon,
		guild_event_won_field,
		guild_event_change_announce,
	};

namespace Guild
{
	std::map<int, GuildScience::Config> GuildScience::configMap;

	Json::Value JoinLimit::packageJLimit()
	{
		Json::Value updateJson;
		updateJson.append(levelLimit);
		updateJson.append(rankLimit);
		updateJson.append(storyLimit);
		updateJson.append(autoApply);
		return updateJson;
	}

	void JoinLimit::setLimit(const int level, const int rank, const int war, const bool aApp)
	{
		levelLimit = level;
		rankLimit = rank;
		storyLimit = war;
		autoApply = aApp;
	}

	void JoinLimit::initLimit(mongo::BSONObj& obj)
	{
		checkNotEoo(obj[joinLimitStr])
		{
			vector<mongo::BSONElement> elems = obj[joinLimitStr].Array();
			unsigned size = elems.size();
			if(size > 0)levelLimit = elems[0].Int();
			if(size > 1)rankLimit = elems[1].Int();
			if(size > 2)storyLimit = elems[2].Int();
			if(size > 3)autoApply = elems[3].Bool();
		}
	}

	bool JoinLimit::passLimit(playerDataPtr player)
	{
		if(levelLimit > 0 && player->Base.getLevel() < levelLimit)return false;
		if(rankLimit > 0 && player->Arena.getRank() > rankLimit) return false;//竞技场
		if(storyLimit > 0 && !player->Warstory.isProcessMap(storyLimit))return false;
		return true;
	}

	mongo::BSONArray JoinLimit::packageBLimit()
	{
		mongo::BSONArrayBuilder build;
		build << levelLimit << rankLimit << storyLimit << autoApply;
		return build.arr();
	}


	GuildScience::GuildScience()
	{
		scienceMap.clear();
		flagIcon = -1;
		flagChange = 0;
	}

	void GuildScience::formatBson(mongo::BSONObj& obj)
	{
		checkNotEoo(obj[guildScienceStr])
		{
			mongo::BSONElement elem = obj[guildScienceStr];
			checkNotEoo(elem[guildFlagIconStr]){flagIcon = elem[guildFlagIconStr].Int();}
			checkNotEoo(elem[guildFlagChangeStr]){flagChange = elem[guildFlagChangeStr].Int();}
			checkNotEoo(elem[guildScienceStr])
			{
				vector<mongo::BSONElement> vec = elem[guildScienceStr].Array();
				for (unsigned i = 0; i < vec.size(); ++i)
				{
					science sc;
					sc.id = vec[i][guildScienceIDStr].Int();
					sc.level = vec[i][guildScienceLevelStr].Int();
					sc.exp = vec[i][guildScienceExpStr].Int();
					scienceMap[sc.id] = sc;
				}
			}
		}
	}
	
	const static string strGuildScience = "./instance/guild/sc/";
	void GuildScience::initConfig()
	{
		na::file_system::json_value_vec vec;
		na::file_system::load_jsonfiles_from_dir(strGuildScience, vec);
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			Json::Value& scJson = vec[i];
			Config c;
			c.ID = scJson["guildid"].asInt();
			c.LimitID = scJson["openIndex"].asInt();
			c.Type = scJson["type"].asInt();
			c.LimitLevel = scJson["openlevel"].asInt();
			c.LimitEffectIdx = scJson["limitIndex"].asInt();
			for (unsigned n = 0; n < scJson["guildcost"].size(); ++n)
			{
				c.Exp.push_back(scJson["guildcost"][n].asInt());
			}
			for (unsigned n = 0; n < scJson["guildeffect"].size(); ++n)
			{
				c.Add.push_back(scJson["guildeffect"][n].asInt());
			}
			for (unsigned n = 0; n < scJson["costModule"].size(); ++n)
			{
				c.Vip.push_back(scJson["costModule"][n][0u].asInt());
				c.Silver.push_back(scJson["costModule"][n][1u].asInt());
				c.Gold.push_back(scJson["costModule"][n][2u].asInt());
			}
			configMap[c.ID] = c;
		}
	}

	GuildScience::Config GuildScience::getConfig(const int sID)
	{
		static Config cNull;
		std::map<int, Config>::iterator it = configMap.find(sID);
		if(it == configMap.end())return cNull;
		return it->second;
	}

	GuildScience::ScienceData GuildScience::getScienceData(const int sID)
	{
		ScienceData sD;
		const science& sc = getScience(sID);
		Config cig = getConfig(sID);
		sD.ID = sID;
		sD.LV = sc.level;
		sD.EXP = sc.exp;
		sD.ADD = cig.getADD(sD.LV);
		return sD;
	}

	int GuildScience::addScienceExp(const int sID, const int aExp)
	{
		science& sc = getScience(sID);
		if(sc.isVaild())
		{
			sc.exp += aExp;
			upgrade(sc);
			return 0;
		}
		return -1;
	}

	int GuildScience::setScienceLevel(int sID, int sLevel)
	{
		science& sc = getScience(sID);
		if(!sc.isVaild())return -1;
		Config cig = getConfig(sID);
		sLevel = sLevel < 0 ? 0 : sLevel;
		sLevel = sLevel > (int)cig.Exp.size() ? (int)cig.Exp.size() : sLevel;
		sc.level = sLevel;
		return 0;
	}

	GuildScience::science& GuildScience::getScience(const int sID)
	{
		static science scNull;
		std::map<int, science>::iterator it = scienceMap.find(sID);
		if(it == scienceMap.end())
		{
			Config c = getConfig(sID);
			if(!c.isVaild())return scNull;
			science nsc;
			nsc.id = sID;
			scienceMap[sID] = nsc;
			it = scienceMap.find(sID);
		}
		return it->second;
	}

	void GuildScience::upgrade(science& sc)
	{
		Config c = getConfig(sc.id);
		vector<int> expVec = c.Exp;
		int exp = sc.exp;
		int level = sc.level;
		double rateCut = 1.0 - (double)getScienceData(playerGuild::financial_planning).ADD / 10000.0;
		int lmLV = -1;
		if(c.LimitEffectIdx != -1)
			lmLV = getScienceData(c.LimitEffectIdx).LV;
		for (unsigned i = (unsigned)level; i < expVec.size();++i)
		{
			int uExp = expVec[i];
			if(c.Type == 1)
			{
				uExp = int(uExp * rateCut);
			}
			if(exp < uExp)break;
			++level;
			exp -= uExp;
		}
		if(lmLV != -1 && level > lmLV)
		{
			level =lmLV;
			exp = 0;
		}
		sc.level = level;
		sc.exp = exp;
		if((unsigned)level >= expVec.size())sc.exp = 0;
	}

	mongo::BSONObj GuildScience::packageBScience()
	{
		mongo::BSONObjBuilder builder;
		builder << guildFlagIconStr << flagIcon << guildFlagChangeStr << flagChange;
		mongo::BSONArrayBuilder arrBuild;
		for (std::map<int, science>::iterator it = scienceMap.begin(); it != scienceMap.end(); ++it)
		{
			const science& sc = it->second;
			mongo::BSONObjBuilder sB;
			sB << guildScienceLevelStr << sc.level <<
				guildScienceExpStr << sc.exp << 
				guildScienceIDStr << sc.id;
			arrBuild << sB.obj();
		}
		builder << guildScienceStr << arrBuild.arr();
		return builder.obj();
	}

	Json::Value GuildScience::packageJScience()
	{
		Json::Value updateJson;
		updateJson[guildFlagLevelStr] = getScienceData(playerGuild::flag_level).LV;
		updateJson[guildFlagIconStr] = flagIcon;
		updateJson[guildFlagChangeStr] = flagChange;
		Json::Value& scienceJson = updateJson[guildScienceStr];
		for (std::map<int, Config>::iterator it = configMap.begin(); it != configMap.end(); ++it)
		{
			const ScienceData sD = getScienceData(it->second.ID);
			Json::Value scJson;
			scJson[guildScienceIDStr] = sD.ID;
			scJson[guildScienceLevelStr] = sD.LV;
			scJson[guildScienceExpStr] = sD.EXP;
			scienceJson.append(scJson);
		}
		return updateJson;
	}

	GuildJoin::GuildJoin()
	{
		JoinVec.clear();
	}

	bool GuildJoin::hasJoin(const int playerID)
	{
		for (unsigned i = 0; i < JoinVec.size(); ++i)
		{
			if(playerID == JoinVec[i]->getPlayerID())return true;
		}
		return false;
	}

	void GuildJoin::addJoinMember(memberPtr mem)
	{
		removeJoinMember(mem->getPlayerID());
		JoinVec.push_back(mem);
	}

	memberPtr GuildJoin::getJoinMember(const int playerID)
	{
		for (unsigned i = 0; i < JoinVec.size(); ++i)
		{
			if(playerID == JoinVec[i]->getPlayerID())return JoinVec[i];
		}
		return memberPtr();
	}

	bool GuildJoin::removeJoinMember(const int playerID)
	{
		for (unsigned i = 0; i < JoinVec.size(); ++i)
		{
			if(playerID != JoinVec[i]->getPlayerID())continue;
			JoinVec.erase(JoinVec.begin() + i);
			return true;
		}
		return false;
	}

	void GuildJoin::initGuildJoin(mongo::BSONObj& obj, const int gID)
	{
		checkNotEoo(obj[joinMemberStr])
		{
			JoinVec.clear();
			vector<mongo::BSONElement> elems = obj[joinMemberStr].Array();
			for (unsigned i = 0; i < elems.size(); ++i)
			{
				BSONElement& elem = elems[i];
				memberPtr ptr = guild_system::CreateMember(elem, gID);
				addJoinMember(ptr);
			}
		}
	}

	Json::Value GuildJoin::packageJoinMember()
	{
		Json::Value updateJson;
		updateJson = Json::arrayValue;
		for (unsigned i = 0;i < JoinVec.size(); ++i)updateJson.append(JoinVec[i]->packageUpdate());
		return updateJson;
	}

	mongo::BSONArray GuildJoin::packageBJoin()
	{
		mongo::BSONArrayBuilder builder;
		for (unsigned i = 0; i < JoinVec.size(); ++i)
		{
			builder << JoinVec[i]->package();
		}
		return builder.arr();
	}

	guildMember::guildMember(playerDataPtr player, const int gID) : guildID(gID)
	{
		initialData();
		playerID =  player->playerID;
		playerName = player->Base.getName();
		playerLevel = player->Base.getLevel();
		faceID = player->Base.getFaceID();
		lastLoginTime = player->getLoginTime();
		warProcess = player->Warstory.currentProcess;
		arenaRank = player->Arena.getRank();
	}

	void guildMember::alterContribute(const int num)
	{
		int tmp = contribute;
		contribute += num;
		contribute = contribute < 0 ? 0 : contribute;
		if (num > 0)
		{
			history += num;
		}
		guildPtr ptr = guild_sys.getGuild(guildID);
		vector<string> logV;
		if (ptr == NULL)
			logV.push_back(boost::lexical_cast<string, int>(-1));
		else
			logV.push_back(boost::lexical_cast<string, int>(ptr->getKingdom()));
		logV.push_back(boost::lexical_cast<string, int>(tmp));
		logV.push_back(boost::lexical_cast<string, int>(contribute));
		if (ptr == NULL)
		{
			StreamLog::Log(logSystemGuild, playerID, playerName, playerLevel, "-1", "NONE", logV, log_member_contribute);
			return;
		}
		StreamLog::Log(logSystemGuild, playerID, playerName, playerLevel, ptr->strLogGID(), ptr->guildName, logV, log_member_contribute);
	}

	void guildMember::initialData()
	{
		playerID =  -1;
		playerName = "";
		faceID = -1;
		playerLevel = 1;
		offcial = guild_comon_member;
		rank = -1;
		contribute = 0;
		history = 0;
		lastLoginTime = 0;
		warProcess =  0;
		arenaRank = -1;
	}

	guildMember::guildMember(BSONElement& el, const int gID) : guildID(gID)
	{
		initialData();
		checkNotEoo(el[memberIDStr]){playerID= el[memberIDStr].Int();}
		checkNotEoo(el[memberNameStr]){playerName= el[memberNameStr].String();}
		checkNotEoo(el[memberFaceIDStr]){ faceID = el[memberFaceIDStr].Int(); }
		checkNotEoo(el[memberLevelStr]){playerLevel= el[memberLevelStr].Int();}
		checkNotEoo(el[memberOffcialStr]){offcial= el[memberOffcialStr].Int();}
		checkNotEoo(el[memberContributeStr]){contribute= el[memberContributeStr].Int();}
		checkNotEoo(el[memberHistoryContributeStr]){ history = el[memberHistoryContributeStr].Int(); }
		else{history = contribute;}
		checkNotEoo(el[memberLastLoginTimeStr]){lastLoginTime= (unsigned)el[memberLastLoginTimeStr].Int();}
		checkNotEoo(el[memberJoinWordStr]){joinWord = el[memberJoinWordStr].String();}
		checkNotEoo(el[memberArenaRankStr]){arenaRank = el[memberArenaRankStr].Int();}
		checkNotEoo(el[memberWarProcessStr]){warProcess = el[memberWarProcessStr].Int();}
	}

	BSONObj guildMember::package()
	{
		BSONObjBuilder obj;
		obj << memberIDStr << playerID << memberNameStr << playerName << memberLevelStr << playerLevel
			<< memberOffcialStr << offcial << memberContributeStr << contribute << memberLastLoginTimeStr << 
			lastLoginTime << memberJoinWordStr << joinWord << memberFaceIDStr << faceID <<
			memberArenaRankStr << arenaRank << memberWarProcessStr << warProcess << memberHistoryContributeStr<<
			history;
		return obj.obj();
	}

	Json::Value guildMember::packageUpdate()
	{
		Json::Value updateJson;
		updateJson[memberIDStr] = playerID;
		updateJson[memberNameStr] = playerName;
		updateJson[memberLevelStr] = playerLevel;
		updateJson[memberFaceIDStr] = faceID;
		updateJson[memberOffcialStr] = offcial;
		updateJson[memberContributeStr] = contribute;
		updateJson[memberHistoryContributeStr] = history;
		updateJson[memberLastLoginTimeStr] = lastLoginTime;
		updateJson[memberArenaRankStr] = arenaRank;
		updateJson[memberctiRank] = rank;
		updateJson[memberWarProcessStr] = warProcess;
		updateJson[memberJoinWordStr] = joinWord;
		updateJson[memberIsOnlineStr] = false;
		if(player_mgr.IsOnline(playerID))updateJson[memberIsOnlineStr] = true;
		return updateJson;
	}

	bool guildMember::formatBaseInfomation(playerDataPtr player)
	{
		bool update = false;
		do 
		{
			if(playerID != player->playerID)break;
			if(playerName != player->Base.getName()){
				update = true;
				playerName = player->Base.getName();
			}
			if (faceID != player->Base.getFaceID()){
				update = true;
				faceID = player->Base.getFaceID();
			}
			if(playerLevel != player->Base.getLevel()){
				update = true;
				playerLevel = player->Base.getLevel();
			}
			if(lastLoginTime != player->getLoginTime()){
				update = true;
				lastLoginTime = player->getLoginTime();
			}
			if(warProcess < player->Warstory.currentProcess){
				update = true;
				warProcess = player->Warstory.currentProcess;
			}
			if(arenaRank != player->Arena.getRank()){
				update = true;
				arenaRank = player->Arena.getRank();
			}
		} while (false);
		return update;
	}

	const static string HelpTypeStr = "ht";
	const static string HelpKeyStr = "key";// just update
	const static string HelpNumStr = "hn";
	const static string TotalHelpNumStr = "thn";
	const static string HelpCreateTimeStr = "hct";//just save
	const static string HelpDeadTimeStr = "hdt";//just save
	const static string HelpHelperListStr = "hpl";//just save

	vector< vector<int> > HelpData::TotalHelpVec;
	const static int HelpEventNum = 4;
	const static unsigned HelpLastTime[HelpEventNum] =
	{
		1800,
		86400,
		1200,
		600
	};
	HelpData::HelpData(GuildHelp* h, memberPtr m, const int t) : M_(m), Father(h)
	{
		_over = false;
		TYPE_ = t;
		H_NUM_ = 0;
		T_H_NUM_ = TotalHelpVec[_GET_TYPE()][M_->offcial];
		C_TIME = na::time_helper::get_current_time();
		int type = _GET_TYPE();
		if (type < 4)
			D_TIME = C_TIME + HelpLastTime[type];
		else
			D_TIME = C_TIME + na::time_helper::ONEDAY;
		P_LIST_.clear();
		_SH_KEY = boost::lexical_cast<string, int>(M_->getPlayerID()) + boost::lexical_cast<string, int>(TYPE_);
	}

	HelpData::HelpData(GuildHelp* h, memberPtr m, mongo::BSONObj& obj) : M_(m), Father(h)
	{
		_over = false;
		TYPE_ = obj[HelpTypeStr].Int();
		H_NUM_ = obj[HelpNumStr].Int();
		T_H_NUM_ = obj[TotalHelpNumStr].Int();
		C_TIME = (unsigned)obj[HelpCreateTimeStr].Int();
		checkNotEoo(obj[HelpDeadTimeStr])
		{
			D_TIME = (unsigned)obj[HelpDeadTimeStr].Int();
		}
		vector<mongo::BSONElement> vec = obj[HelpHelperListStr].Array();
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			P_LIST_.insert(vec[i].Int());
		}
		_SH_KEY = boost::lexical_cast<string, int>(M_->getPlayerID()) + boost::lexical_cast<string, int>(TYPE_);
	}

	void HelpData::initData()
	{
		TotalHelpVec.clear();
		Json::Value configJson = na::file_system::load_jsonfile_val("./instance/guild/guildHelp.json");
		for (unsigned i = 0; i < configJson.size(); ++i)
		{
			Json::Value& ofJson = configJson[i];
			vector<int> vec;
			for (unsigned n = 0; n < ofJson.size(); ++n)
			{
				vec.push_back(ofJson[n].asInt());
			}
			TotalHelpVec.push_back(vec);
		}
	}

	Json::Value HelpData::_JSON()
	{
		Json::Value sJD;
		sJD[playerIDStr] = M_->getPlayerID();
		sJD[playerNameStr] = M_->getPlayerName();
		sJD[playerFaceIDStr] = M_->getFaceID();
		sJD[playerLevelStr] = M_->getPlayerLv();
		sJD[HelpTypeStr] = _GET_TYPE();
		sJD[HelpKeyStr] = _SH_KEY;
		sJD[HelpNumStr] = H_NUM_;
		sJD[TotalHelpNumStr] = T_H_NUM_;
		return sJD;
	}

	mongo::BSONObj HelpData::_BSON()
	{
		mongo::BSONArrayBuilder aBuilder;
		for (HELPERLIST::iterator it = P_LIST_.begin(); it != P_LIST_.end(); ++it)
		{
			aBuilder << *it;
		}
		return BSON(playerIDStr << M_->getPlayerID() << HelpNumStr << H_NUM_ <<
			HelpTypeStr << TYPE_ << TotalHelpNumStr << T_H_NUM_ << HelpHelperListStr <<
			aBuilder.arr() << HelpCreateTimeStr << C_TIME << HelpDeadTimeStr << D_TIME);
	}

	bool HelpData::_IN_HELPER(const int HELPER)
	{
		bool ret = P_LIST_.find(HELPER) != P_LIST_.end();
		return ret;
	}

	bool HelpData::_CanTick(const int From)
	{
		return P_LIST_.find(From) == P_LIST_.end();
	}

	bool HelpData::_TICK(const int From)
	{
		if (!P_LIST_.insert(From).second)return false;
		++H_NUM_;
		if (H_NUM_ >= T_H_NUM_)_DEAD();
		return true;
	}

	void HelpData::_DEAD()
	{
		Father->removeHelp(_KEY());
		_over = true;
	}

#define AddHelpReg(HelpFunction) boost::bind(&GuildHelp::HelpFunction, this, _1, _2, _3, _4)
#define AddHelpTick(HelpFunction) boost::bind(&GuildHelp::HelpFunction, this, _1, _2)
#define AddHelpUpdate(HelpFunction) boost::bind(&GuildHelp::HelpFunction, this, _1, _2)
	GuildHelp::GuildHelp(guild* hand) : Father(hand)
	{
		ownMap.clear();
		ownVec.clear();

		//注册函数
		_regMap.clear();
		_regMap[pilot_core] = AddHelpReg(pilot_core_reg);
		_regMap[mine_battle] = AddHelpReg(mine_battle_reg);
		_regMap[world_boss] = AddHelpReg(world_boss_reg);
		_regMap[war_story_cd] = AddHelpReg(war_story_reg);

		//处理函数
		_tickMap.clear();
		_tickMap[pilot_core] = AddHelpTick(pilot_core_tick);
		_tickMap[mine_battle] = AddHelpTick(mine_battle_tick);
		_tickMap[world_boss] = AddHelpTick(world_boss_tick);
		_tickMap[war_story_cd] = AddHelpTick(war_story_tick);

		//判断函数
		_updateMap[world_boss] = AddHelpUpdate(world_boss_update);
	}

	bool GuildHelp::mine_battle_tick(playerDataPtr player, HelpDataPtr help)
	{
		mineItem mItem;
		if (mine_res_sys.getPlayerMineInfo(player, mItem))
		{
			mItem.alterBeHelpMineTimes(1);
			return true;
		}
		return false;
	}

	bool GuildHelp::pilot_core_tick(playerDataPtr player, HelpDataPtr help)
	{
		int pilotID = help->_GET_EXTRA();
		if (player->Pilots.addCoreTime(pilotID, 300))return true;
		return false;
	}

	bool GuildHelp::world_boss_tick(playerDataPtr player, HelpDataPtr help)
	{
		if (world_boss_sys.isBossActValid())
		{
			player->WorldBoss.alterBeHelpBossTimes(1);
			Json::Value chatContent;
			chatContent[playerNameStr] = help->M_->getPlayerName();
//			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_guild_inspire, chatContent);
			world_boss_sys.updateInfoSingleClient(player);
			return true;
		}
		return false;
	}

	bool GuildHelp::war_story_tick(playerDataPtr player, HelpDataPtr help)
	{
		playerCD& CD = player->Campaign.getCD();
		if (!CD.getCD().Lock)return false;
		unsigned now = na::time_helper::get_current_time();
		if (now > CD.getCD().CD)return false;
		if (CD.getCD().CD - now < 300)return false;
		CD.reduceCD(30);//30sCD
		if (CD.getCD().CD - now < 300)
		{
			help->_DEAD();
		}
		helper_mgr.insertSaveAndUpdate(&player->Campaign);
		return true;
	}

	int GuildHelp::TickHelp(playerDataPtr player, memberPtr player_m, playerManager::BHIDMAP playerList, vector<string> keyList, int* cri)
	{
		int num = 0;
		boost::unordered_set<memberPtr> mL;
		for (unsigned i = 0; i < keyList.size(); ++i)
		{
			HelpDataPtr helpPtr = getHelpPtr(keyList[i]);
			if (helpPtr == NULL)continue;
			playerManager::BHIDMAP::iterator playerIt = playerList.find(helpPtr->M_->getPlayerID());
			if (playerIt == playerList.end())continue;
			playerDataPtr target = playerIt->second;
			if (!helpPtr->_CanTick(player->playerID))
			{
				if (helpPtr->_Over())mL.insert(helpPtr->M_);
				continue;
			}
			mL.insert(helpPtr->M_);
			if (helpPtr->_Over())continue;
			//增加对应的效果
			bool sucess = false;
			if (_tickMap.find(helpPtr->_GET_TYPE()) != _tickMap.end())
			{
				sucess = _tickMap[helpPtr->_GET_TYPE()](target, helpPtr);
			}
			if (sucess)
			{
				//玩家贡献
				if (player->Guild.getHelpTick() < 200)
				{
					player_m->alterContribute(player->Base.getLevel());
				}
				if (player->Guild.getHelpTick() < 20)
				{
					int silver = 1000;
					if (commom_sys.randomOk(0.3))
					{
						if (cri)++*cri;
						silver *= 3;
					}
					player->Base.alterSilver(silver);
				}
				player->Guild.tickHelp();
			}
			else
			{
				helpPtr->_DEAD();
				continue;
			}

			helpPtr->_TICK(player->playerID);

			Json::Value bCast;
			bCast[msgStr][0u] = 0;
			bCast[msgStr][1u][senderChannelStr] = chat_server;
			bCast[msgStr][1u][chatBroadcastID] = BROADCAST::guild_help_announce;
			const static string ParamListStr = "pl";
			bCast[msgStr][1u][ParamListStr].append(player_m->getPlayerName());
			bCast[msgStr][1u][ParamListStr].append(helpPtr->_GET_TYPE());
			target->sendToClient(gate_client::chat_broadcast_resp, bCast);

			++num;
		}
		if (num > 0 && Father != NULL)
		{
			Father->sortMember();
		}
		for (boost::unordered_set<memberPtr>::iterator it = mL.begin(); it != mL.end(); ++it)
		{
			saveHelp(*it);
		}
		return num;
	}

	void GuildHelp::AddHelp(HelpDataPtr helpPtr, bool save)
	{
		if (helpPtr == NULL)return;
		bool t_sort = getHelpPtr(helpPtr->_KEY()) == NULL ? false : true;
		ownMap[helpPtr->_KEY()] = helpPtr;
		if (t_sort)
		{
			sortHelp();
		}
		else
		{
			ownVec.insert(ownVec.begin(), helpPtr);
		}
		helpPtr->M_->ownMap[helpPtr->_KEY()] = helpPtr;
		if (save)
		{
			saveHelp(helpPtr);
		}
		if (save && ownVec.size() > SET_LIFE)
		{
			HelpDataPtr ptr = ownVec.back();
			ownVec.back()->M_->ownMap.erase(ownVec.back()->_KEY());
			ownMap.erase(ownVec.back()->_KEY());
			ownVec.pop_back();
			saveHelp(ptr);
		}
	}

	bool GuildHelp::mine_battle_reg(playerDataPtr player, memberPtr mem, const int type, const int seed)
	{
		if (seed != 0)return false;
		mineItem mItem;
		if (!mine_res_sys.getPlayerMineInfo(player, mItem))return false;
		if (mItem.getHasSeekMineHelp())return false;
		mItem.setHasSeekMineHelp(true);
		return true;
	}

	bool GuildHelp::pilot_core_reg(playerDataPtr player, memberPtr mem, const int type, const int seed)
	{
		const playerPilot& cPilot = player->Pilots.getPlayerPilotExtert(seed);
		if (cPilot == playerPilots::NullPilotExtert)return false;
		if (cPilot.coreMode < 0 || cPilot.askForHelp)return false;
		player->Pilots.setPilotHelp(seed);
		return true;
	}

	bool GuildHelp::world_boss_reg(playerDataPtr player, memberPtr mem, const int type, const int seed)
	{
		if (seed != 0)return false;
		if (!world_boss_sys.isBossActValid()) return false;
		if (player->WorldBoss.getHasSeekBossHelp())return false;
		if (player->Base.getLevel() < WORLD_BOSS_OPEN_LEVEL || player->Base.getSphereID() == -1)return false;
		player->WorldBoss.setHasSeekBossHelp(true);
		return true;
	}

	bool GuildHelp::war_story_reg(playerDataPtr player, memberPtr mem, const int type, const int seed)
	{
		if (seed != 0)return false;
		if (!player->Campaign.getCD().getCD().Lock)return false;
		if (player->Campaign.getHelpTick())return false;
		player->Campaign.setHelpTick(true);
		return true;
	}

	bool GuildHelp::AddHelp(playerDataPtr player, memberPtr mem, const int type, const int seed)
	{
		if (player == NULL || mem == NULL || mem->offcial < 0)return false;

		bool ret = false;
		if (_regMap.find(type) != _regMap.end())
		{
			ret = _regMap[type](player, mem, type, seed);
		}
		if (ret)
		{
			HelpDataPtr helpPtr = HelpData::Create(this, mem, HelpData::TYPECAL(type, seed));
			AddHelp(helpPtr);
		}
		return ret;
	}

	void GuildHelp::formatHelp(memberPtr mem, mongo::BSONObj& obj)
	{
		if (mem == NULL)return;
		checkNotEoo(obj[guildHelpStr])
		{
			vector<mongo::BSONElement> vec = obj[guildHelpStr].Array();
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				mongo::BSONObj data = vec[i].Obj();
				HelpDataPtr helpPtr = HelpData::Create(this, mem, data);
				if (helpPtr->_Over())
				{
					saveHelp(mem);
					continue;
				}
				AddHelp(helpPtr, false);
			}
		}
		sortHelp();
	}

	void GuildHelp::saveHelp(memberPtr mem)
	{
		if (mem == NULL)return;
		HelpMap& m = mem->ownMap;
		mongo::BSONObj key = BSON(guildIDStr << mem->guildID <<
			playerIDStr << mem->getPlayerID());
		if (m.empty())
		{
			db_mgr.remove_collection(dbGuildHelpList, key);
			return;
		}
		mongo::BSONArrayBuilder aBuilder;
		for (HelpMap::iterator it = m.begin(); it != m.end(); ++it)
		{
			aBuilder << it->second->_BSON();
		}
		mongo::BSONObj obj = BSON(guildIDStr << mem->guildID <<
			playerIDStr << mem->getPlayerID() << guildHelpStr << aBuilder.arr());
		db_mgr.save_mongo(dbGuildHelpList, key, obj);
	}

	void GuildHelp::saveHelp(HelpDataPtr help)
	{
		if (help == NULL)return;
		saveHelp(help->M_);
	}

	bool HelpsortDo(HelpDataPtr h1, HelpDataPtr h2)
	{
		return h1->_COMEOUT() < h2->_COMEOUT();
	}

	void GuildHelp::sortHelp()
	{
		ownVec.clear();
		for (HelpMap::iterator it = ownMap.begin(); it != ownMap.end(); ++it)
		{
			ownVec.push_back(it->second);
		}
		std::sort(ownVec.begin(), ownVec.end(), HelpsortDo);
	}

	HelpDataPtr GuildHelp::getHelpPtr(const string key)
	{
		HelpMap::iterator  it = ownMap.find(key);
		if (it == ownMap.end())return HelpDataPtr();
		return it->second;
	}

	HelpDataPtr GuildHelp::reomveHelpD(const string key)
	{
		HelpMap::iterator it = ownMap.find(key);
		if (it == ownMap.end())return HelpDataPtr();
		HelpDataPtr hdPtr = it->second;
		hdPtr->M_->ownMap.erase(key);
		ownMap.erase(it);
		return hdPtr;
	}

	bool GuildHelp::removeHelp(const string key)
	{
		HelpDataPtr ptr = reomveHelpD(key);
		bool s = ptr == NULL ? false : true;
		if (s)
		{
			sortHelp();
			saveHelp(ptr);
		}
		return s;
	}

	bool GuildHelp::removeHelp(const int playerID, const int TYPE)
	{
		return removeHelp(boost::lexical_cast<string, int>(playerID)+
			boost::lexical_cast<string, int>(TYPE));
	}

	void GuildHelp::removeHelp(memberPtr mem)   //some one leave guild
	{
		HelpMap& m = mem->ownMap;
		for (HelpMap::iterator it = m.begin(); it != m.end();)
		{
			HelpDataPtr ptr = it->second;
			++it;
			ptr->_DEAD();
		}
		sortHelp();
		saveHelp(mem);
	}

	bool GuildHelp::world_boss_update(const int helper, HelpDataPtr help)
	{
		return world_boss_sys.isBossActValid();
	}

	Json::Value GuildHelp::packageGHelp(const int helper)
	{
		Json::Value jsonData = Json::arrayValue;
		for (int i = 0; i < (int)ownVec.size() && i < 20; i++)
		{
			HelpDataPtr cD = ownVec[i];
			if (cD->_Over())
			{
				--i;
				saveHelp(cD->M_);
				continue;
			}
			updateMap::iterator it = _updateMap.find(cD->_GET_TYPE());
			if (it != _updateMap.end() && !it->second(helper, cD))
			{
				--i;
				cD->_DEAD();
				saveHelp(cD->M_);
				continue;
			}
			if (cD->M_->getPlayerID() == helper)continue;
			if (cD->_IN_HELPER(helper))continue;
			jsonData.append(cD->_JSON());
		}
		return jsonData;
	}

	GuildHistory::GuildHistory()
	{
		eventVec.clear();
	}
	
	void GuildHistory::addEvent(Json::Value& eventJson)
	{
		//int eventID = eventJson[strEventID].asInt();
		eventJson[strEventTrriger] = na::time_helper::get_current_time();
		eventVec.insert(eventVec.begin(), eventJson);
		if(eventVec.size() > 99)eventVec.pop_back();
	}

	void GuildHistory::initGuildHistory(mongo::BSONObj& obj)
	{
		checkNotEoo(obj[guildHistorystr])
		{
			eventVec.clear();
			vector<mongo::BSONElement> vec = obj[guildHistorystr].Array();
			for(unsigned i = 0; i < vec.size(); ++i)
			{
				mongo::BSONElement& elem = vec[i];
				int eID = elem[strEventID].Int();
				Json::Value dataJson;
				dataJson[strEventID] = eID;
				dataJson[strEventTrriger] = (unsigned)elem[strEventTrriger].Int();
				checkNotEoo(elem[strEventData])
				{
					vector<mongo::BSONElement> datas = elem[strEventData].Array();
					for (unsigned n = 0; n < datas.size(); ++n)
					{
						mongo::BSONElement e = datas[n];
						if (e.type() == mongo::NumberInt)dataJson[strEventData].append(e.Int());
						else if (e.type() == mongo::NumberLong)dataJson[strEventData].append(e.Long());
						else if (e.type() == mongo::NumberDouble)dataJson[strEventData].append(e.Double());
						else if (e.type() == mongo::String)dataJson[strEventData].append(e.String());
					}
				}
				//addEvent(dataJson);
				eventVec.push_back(dataJson);
			}
		}
	}

	mongo::BSONArray GuildHistory::packageBHis()
	{
		mongo::BSONArrayBuilder builder;
		for (unsigned i = 0; i < eventVec.size(); ++i)
		{
			string str = commom_sys.json2string(eventVec[i]);
// 			string str = eventVec[i].toStyledString();
// 			str = commom_sys.tighten(str);
			builder << mongo::fromjson(str);
		}
		return builder.arr();
	}

	Json::Value GuildHistory::packageHistory(unsigned beginIdx, unsigned endIdx)
	{
		Json::Value updateJson = Json::arrayValue;
		unsigned size = endIdx - beginIdx + 1;
		for (unsigned i = beginIdx; i < eventVec.size() && i <= endIdx; ++i)
		{
			updateJson.append(eventVec[i]);
		}
		return updateJson;
	}

	guild::guild(const int gID) : guildID(gID), GuildHelp(this)
	{
		freeAllData();
		createTime  = na::time_helper::get_current_time();
	}

	int guild::addMember(playerDataPtr player, const int of /* = guild_comon_member */, bool sotrM /* = true */)
	{
		if(members.size() >= (unsigned)getScienceData(playerGuild::flag_level).ADD
			|| getMember(player->playerID) != NULL)return 1;
		memberPtr mem = guild_system::CreateMember(player, guildID);
		if(mem == memberPtr())return -1;
		mem->offcial = of;
		addMember(mem);
		player->Guild.setGuildID(guildID);
		if(sotrM)sortMember();
		if(of != guild_leader)
		{
			Json::Value addJson;
			addJson[strEventID] = guild_event_add_member;
			addJson[strEventTrriger] = na::time_helper::get_current_time();
			addJson[strEventData].append(player->Base.getName());
			addEvent(addJson);
		}
		return 0;
	}

	int guild::alterMemContribute(playerDataPtr player, const int num)
	{
		memberPtr mem = getMember(player->playerID);
		if(mem == memberPtr())return -1;
		mem->alterContribute(num);
		return 0;
	}

	int guild::memberDonate(playerDataPtr player, const int num, const int scID, bool& GL)
	{
		memberPtr mem = getMember(player->playerID);
		if(mem == memberPtr())return -1;
		
		int tmpLv = getScienceData(scID).LV;
		int aExp = num;
		unsigned now = na::time_helper::get_current_time();
		if (now > player->Guild.getJoinTime() && now - player->Guild.getJoinDayTime() < 3 * na::time_helper::ONEDAY)aExp /= 3;
		if (0 != addScienceExp(scID, aExp))return -1;
		int nowLv = getScienceData(scID).LV;
		mem->alterContribute(num);
		if(tmpLv != nowLv)
		{
			if(scID == playerGuild::guild_level)GL = true;
			Json::Value upgradeJson;
			upgradeJson[strEventID] = guild_event_donate;
			upgradeJson[strEventTrriger] = na::time_helper::get_current_time();
			upgradeJson[strEventData].append(mem->getPlayerName());
			upgradeJson[strEventData].append(num);
			upgradeJson[strEventData].append(scID);
			upgradeJson[strEventData].append(nowLv);
			addEvent(upgradeJson);
			StreamLog::LogV(logSystemGuild ,player, strLogGID(), guildName, log_tag_science, boost::lexical_cast<string, int>(getKingdom()), boost::lexical_cast<string, int>(scID), boost::lexical_cast<string, int>(nowLv));
		}
		sortMember();
		return 0;
	}

	int guild::getOffcial(int player_id)
	{
		memberPtr mem = getMember(player_id);
		if(mem == memberPtr())return -1;
		return mem->offcial;
	}

	string guild::strLogGID()
	{
		//static const string logGID = boost::lexical_cast<string, int>(guildID) + boost::lexical_cast<string, unsigned>(createTime);
		//return logGID;
		return boost::lexical_cast<string, int>(guildID)+boost::lexical_cast<string, unsigned>(createTime);
	}

	int guild::memberUpgrade(playerDataPtr player)
	{
		memberPtr mem = getMember(player->playerID);
		if(mem == memberPtr())return -1;
		if(!canUpgrade(mem))return 1;
		int checkOffcial = mem->offcial - 1;
		vector<memberPtr> vec = getOffcialRank(checkOffcial);
		if(checkOffcial == guild_leader && !vec.empty()){
			vec.back()->offcial = guild_comon_member;
		}else if(checkOffcial == guild_vice_leader && vec.size() >= 2){
			vec.back()->offcial = mem->offcial;
		}else if(checkOffcial == guild_old_member && vec.size() >= 4){
			vec.back()->offcial = mem->offcial;
		}else if(checkOffcial == guild_senior_member && vec.size() >= 8){
			vec.back()->offcial = mem->offcial;
		}else if(checkOffcial == guild_middle_member && vec.size() >= 16){
			vec.back()->offcial = mem->offcial;
		}
		mem->offcial = checkOffcial;
		sortMember();
		if(checkOffcial == guild_leader)
		{
			Json::Value replaceJson;
			replaceJson[strEventID] = guild_event_replace_leader;
			replaceJson[strEventTrriger] = na::time_helper::get_current_time();
			replaceJson[strEventData].append(mem->getPlayerName());
			addEvent(replaceJson);
			Json::Value bCast;
			bCast[msgStr][0u] = 0;
			bCast[msgStr][1u][senderChannelStr] =chat_guild;
			bCast[msgStr][1u][chatBroadcastID] = BROADCAST::guild_leader_replace;
			const static string ParamListStr = "pl";
			bCast[msgStr][1u][ParamListStr].append(mem->getPlayerName());
			sendToAllOnlineMember(gate_client::chat_broadcast_resp, bCast);

			//群发邮件
			Json::Value leaderJson;
			leaderJson[msgStr][0u] = mem->getPlayerName();
			string s = leaderJson.toStyledString();
			na::msg::msg_json mj(gate_client::player_guild_upgrade_to_leader_notice_req, s);
			for (memberMap::iterator it = members.begin(); it != members.end(); ++it)
			{
				memberPtr currentMem = it->second;
				mj._player_id = currentMem->getPlayerID();
				playerDataPtr currentPlayer = player_mgr.getPlayerMain(currentMem->getPlayerID());
				if(currentPlayer == playerDataPtr()){
					player_mgr.postMessage(mj);
					continue;
				}

				Json::Value leaderReplaceEmail;
				string guildLeaderName = "gln";
				leaderReplaceEmail[guildLeaderName] = mem->getPlayerName();
				email_sys.sendSystemEmailCommon(currentPlayer, email_type_system_ordinary, 
					email_team_system_replace_guild_leader, leaderReplaceEmail);
			}
		}else if(checkOffcial == guild_vice_leader || checkOffcial == guild_old_member){
			Json::Value upgradeJson;
			upgradeJson[strEventID] = guild_event_member_upgrade;
			upgradeJson[strEventTrriger] = na::time_helper::get_current_time();
			upgradeJson[strEventData].append(mem->getPlayerName());
			upgradeJson[strEventData].append(checkOffcial);
			addEvent(upgradeJson);
		}
		StreamLog::LogV(logSystemGuild, player, strLogGID(), guildName, log_tag_upgrade_member, boost::lexical_cast<string, int>(getKingdom()), boost::lexical_cast<string, int>(checkOffcial));
		task_sys.updateBranchTaskInfo(player, _task_guild_position_primary, mem->offcial);
		return 0;
	}

	int guild::memberJoin(playerDataPtr player, string words)
	{
		if(words.length() > 150)return 1;
		if(player->Guild.getJoinList().size() >= 10)return 2;
		if(player->Guild.hasApplyGuild(guildID))return 3;
		memberPtr mem = guild_system::CreateMember(player, guildID);
		if(mem == memberPtr())return -1;
		if(isFullList())return 4;//申请名单已经满了
		if(hasJoin(player->playerID))return 5;//已经提交申请
		mem->joinWord = words;
		addJoinMember(mem);
		player->Guild.pushGuild(guildID);
		return 0;
	}

	//0否决,其他接受
	int guild::applyMember(playerDataPtr player, vector<playerDataPtr> targetVec, const int type)
	{
		memberPtr mem = getMember(player->playerID);
		if(mem == memberPtr())return -1;
		if(mem->offcial < guild_leader || mem->offcial > guild_old_member)return 1;//权限不够
		if(type == 0)	{
			for (unsigned i = 0; i < targetVec.size(); ++i)
			{
				playerDataPtr target = targetVec[i];
				removeJoinMember(target->playerID);
				target->Guild.removeGuild(guildID);
				StreamLog::LogV(logSystemGuild, player, strLogGID(), guildName, log_tag_reject_join, boost::lexical_cast<string, int>(getKingdom()), boost::lexical_cast<string, int>(target->playerID), target->Base.getName());
			}
		}
		else{
			int num = (int)targetVec.size();
			if(getRemainPlace() < num)return 3;
			if(mem->offcial == guild_old_member){
				int leave = 10  - player->Guild.getApplyTimes();
				if(leave < num)return 2;
			}
			player->Guild.alterApplyTimes(num);
			for (unsigned i = 0; i < targetVec.size(); ++i)
			{
				playerDataPtr target = targetVec[i];
				memberPtr Jmem = getJoinMember(target->playerID);
				if(Jmem == memberPtr())continue;
				if(addMember(target, guild_comon_member, false) != 0)continue;
				removeJoinMember(target->playerID);
				target->Base.update();
				Json::Value bCast;
				bCast[msgStr][0u] = 0;
				bCast[msgStr][1u][senderChannelStr] =chat_guild;
				bCast[msgStr][1u][chatBroadcastID] = BROADCAST::guild_member_join;
				const static string ParamListStr = "pl";
				bCast[msgStr][1u][ParamListStr].append(target->Base.getName());
				sendToAllOnlineMember(gate_client::chat_broadcast_resp, bCast);
				StreamLog::LogV(logSystemGuild, player, strLogGID(), guildName, log_tag_allow_join, boost::lexical_cast<string, int>(getKingdom()), boost::lexical_cast<string, int>(target->playerID), target->Base.getName());
			}
			sortMember();
		}
		return 0;
	}

	int guild::motifyWords(playerDataPtr player, string word)
	{
		if( words.length() < 1 || commom_sys.UTF8Len(word) > 75 )return 3;
		memberPtr ptr = getMember(player->playerID);
		if(ptr == memberPtr())return -1;
		if(ptr->offcial < guild_leader || ptr->offcial > guild_vice_leader)return 1;
		if(ptr->offcial == guild_vice_leader && player->Guild.getDeclareTimes() > 2)return 2;
		player->Guild.alterDeclareTimes(1);
		words = word;
		return 0;
	}

	bool guild::canKick(playerDataPtr player, playerDataPtr target)
	{
		memberPtr playerPtr = getMember(player->playerID);
		memberPtr targetPtr = getMember(target->playerID);
		if(playerPtr == memberPtr() || targetPtr == memberPtr())return false;
		if(playerPtr->offcial == guild_leader && playerPtr->offcial < targetPtr->offcial && player->Guild.getKickTimes() < 10)return true;
		if(playerPtr->offcial == guild_vice_leader && playerPtr->offcial < targetPtr->offcial && player->Guild.getKickTimes() < 5)return true;
		return false;
	}

	bool guild::initGuildMember(mongo::BSONObj& obj)
	{
		bool ret = false;
		checkNotEoo(obj[guildNameStr]){guildName = obj[guildNameStr].String();}
		checkNotEoo(obj[guildKingdomStr]){guildKingdom = obj[guildKingdomStr].Int();}
		checkNotEoo(obj[guildWordsStr]){words = obj[guildWordsStr].String();}
		checkNotEoo(obj[guildCreateTimeStr]){createTime = (unsigned)obj[guildCreateTimeStr].Int();}
		checkNotEoo(obj[guildServerIDStr]){ serverID = obj[guildServerIDStr].Int(); }
		else{ serverID = commom_sys.serverID(); ret = true; }
		checkNotEoo(obj[guildMemberStr])
		{
			vector<mongo::BSONElement> arr = obj[guildMemberStr].Array();
			for (unsigned i = 0;  i < arr.size(); ++i)
			{
				BSONElement& el = arr[i];
				memberPtr ptr = guild_system::CreateMember(el, guildID);
				if(ptr != memberPtr())addMember(ptr);
			}
		}
		return true;
	}

	memberPtr guild::getMember(const int playerID)
	{
		memberMap::iterator it = members.find(playerID);
		if(it == members.end())return memberPtr();
		return it->second;
	}

	memberPtr guild::getMember(const string name)
	{
		memNaMap::iterator it = memNas.find(name);
		if(it == memNas.end())return memberPtr();
		return it->second;
	}

	Json::Value guild::packageMember(unsigned beginIdx, unsigned endIdx)
	{
		Json::Value updateJson;
		updateJson = Json::arrayValue;
		for (unsigned i = beginIdx; i <= endIdx && i < memberRank.size(); ++i)
			updateJson.append(memberRank[i]->packageUpdate());
		return updateJson;
	}

	void guild::sendToAllOnlineMember(const short protocol, Json::Value& msg)
	{
		for (memberMap::iterator it = members.begin(); it != members.end(); ++it)
			player_mgr.sendToPlayer(it->second->getPlayerID(), protocol, msg);
	}

	void guild::getOnline(playerManager::playerDataVec& vec)
	{
		for (memberMap::iterator it = members.begin(); it != members.end(); ++it)
		{
			playerDataPtr player = player_mgr.getOnlinePlayer(it->second->getPlayerID());
			if(player == NULL)continue;
			vec.push_back(player);
		}
	}

	Json::Value guild::packageBase(playerDataPtr player)
	{
		Json::Value updateJson;
		updateJson[guildIDStr] = guildID;
		updateJson[guildRankStr] = rank;
		updateJson[guildNameStr] = guildName;
		updateJson[guildKingdomStr] = guildKingdom;
		updateJson[guildWordsStr] = words;
		updateJson[applyMemberStr] = getJoinMemberNum();
		updateJson[guildLeaderInfoStr] = Json::arrayValue;
		if(!memberRank.empty())
		{
			updateJson[guildLeaderInfoStr].append(memberRank[0]->getPlayerID());
			updateJson[guildLeaderInfoStr].append(memberRank[0]->getPlayerName());
		}
		updateJson[guildViceLeaderInfoStr] = Json::arrayValue;
		for (unsigned i = 1; i < memberRank.size(); ++i)
		{
			if(memberRank[i]->offcial != guild_vice_leader)break;
			Json::Value viceJson;
			viceJson.append(memberRank[i]->getPlayerID());
			viceJson.append(memberRank[i]->getPlayerName());
			updateJson[guildViceLeaderInfoStr].append(viceJson);
		}
		updateJson[memberYourSelfStr] = Json::objectValue;
		memberPtr mine = getMember(player->playerID);
		if(mine != NULL)updateJson[memberYourSelfStr] = mine->packageUpdate();
		updateJson[guildMemberNumStr] = (int)members.size();
		GuildScience::ScienceData sD  = getScienceData(playerGuild::flag_level);
		updateJson[guildFlagLevelStr] = sD.LV;
		updateJson[guildMemberContainStr] = sD.ADD;
		updateJson[guildHistorystr] = packageHistory(0, 2);
		memberPtr mem = getMember(player->playerID);
		updateJson[canUpgradeStr] = canUpgrade(mem);
		updateJson[joinLimitStr] = packageJLimit();
		updateJson[guildFlagIconStr] = getFlagIcon();
		updateJson[guildLeagueLevelStr] = getScienceData(playerGuild::guild_level).LV;
		updateJson[guildIsApplyStr] = player->Guild.hasApplyGuild(guildID);
		return updateJson;
	}

	Json::Value guild::gmPackageBase()
	{
		Json::Value updateJson;
		updateJson[guildIDStr] = guildID;
		updateJson[guildNameStr] = guildName;
		updateJson[guildCreateTimeStr] = createTime;
		updateJson[guildKingdomStr] = guildKingdom;
		updateJson[guildLeaderInfoStr] = Json::arrayValue;
		if(!memberRank.empty())
		{
			updateJson[guildLeaderInfoStr].append(memberRank[0]->getPlayerID());
			updateJson[guildLeaderInfoStr].append(memberRank[0]->getPlayerName());
		}
		updateJson[guildMemberNumStr] = (int)members.size();
		GuildScience::ScienceData sD  = getScienceData(playerGuild::flag_level);
		updateJson[guildFlagLevelStr] = sD.LV;
		updateJson[guildMemberContainStr] = sD.ADD;
		updateJson[guildLeagueLevelStr] = getScienceData(playerGuild::guild_level).LV;
		updateJson[guildScienceStr] = packageJScience();
		return updateJson;
	}

	bool guild::asyncPlayerData(playerDataPtr player)
	{
		memberPtr mem = getMember(player->playerID);
		if(mem == memberPtr())return false;
		if(mem->getPlayerName() != player->Base.getName())
		{
			memNas.erase(mem->getPlayerName());
			memNas[player->Base.getName()] = mem;
		}
		return mem->formatBaseInfomation(player);
	}

	int guild::getRemainPlace()
	{
		return getScienceData(playerGuild::flag_level).ADD - (int)members.size();
	}

	void guild::getAllMembers(vector<int> &memList)
	{
		for (memberMap::iterator it = members.begin(); it != members.end(); it++)
		{
			memList.push_back(it->second->getPlayerID());
		}
	}

	BSONObj guild::package()
	{
		BSONObjBuilder objBuild;
		objBuild << guildIDStr << guildID << guildNameStr << guildName << 
			guildWordsStr<< words << guildCreateTimeStr << createTime << 
			guildKingdomStr << guildKingdom << guildServerIDStr << serverID;
		{
			BSONArrayBuilder arr;
			for (memberMap::iterator it = members.begin(); it != members.end(); ++it)
				arr << it->second->package();
			objBuild << guildMemberStr << arr.arr();
		}
		return objBuild.obj();
	}

	int guild::kickMember(playerDataPtr player, playerDataPtr target)
	{
		if(!canKick(player, target))return 2;//没有权限或者今天已经不能再t人了
		int res = memberLeave(target);
		if(res != 0)return res;
		memberPtr playerPtr = getMember(player->playerID);
		if(playerPtr == memberPtr())return 0;
		player->Guild.alterKickTimes(1);
		//这里通知玩家已经被踢出
		Json::Value kickJson;
		kickJson[strEventID] = guild_event_remove_member;
		kickJson[strEventTrriger] = na::time_helper::get_current_time();
		kickJson[strEventData].append(playerPtr->getPlayerName());
		kickJson[strEventData].append(target->Base.getName());
		addEvent(kickJson);
		return 0;
	}

	int guild::memberLeave(playerDataPtr target)
	{
		memberPtr ptr = getMember(target->playerID);
		if(ptr == memberPtr())return 1;
		if(ptr->offcial == guild_leader)return 2;
		target->Guild.setGuildID(-1, ptr->getContribute());
		members.erase(ptr->getPlayerID());
		memNas.erase(ptr->getPlayerName());
		removeHelp(ptr);
		sortMember();
		return 0;
	}

	int guild::cancelJoin(playerDataPtr player)
	{
		removeJoinMember(player->playerID);
		player->Guild.removeGuild(guildID);
		return 0;
	}

	int guild::abdication(playerDataPtr player, playerDataPtr target)
	{
		if(player->playerID == target->playerID)return -1;
		memberPtr leader = getMember(player->playerID);
		if(leader == memberPtr() || leader->offcial != guild_leader)return -1;
		memberPtr tMem = getMember(target->playerID);
		if(tMem == memberPtr())return 1;
		tMem->offcial = guild_leader;
		int tmp = target->Guild.getEmailTimes();
		target->Guild.setApplyTimes(player->Guild.getApplyTimes());
		target->Guild.setDeclareTimes(player->Guild.getDeclareTimes());
		target->Guild.setEmailTimes(player->Guild.getEmailTimes());
		target->Guild.setKickTimes(player->Guild.getKickTimes());
		leader->offcial = guild_comon_member;
		sortMember();
		Json::Value transJson;
		transJson[strEventID] = guild_event_trun_leader;
		transJson[strEventTrriger] = na::time_helper::get_current_time();
		transJson[strEventData].append(leader->getPlayerName());
		transJson[strEventData].append(tMem->getPlayerName());
		addEvent(transJson);
		return 0;
	}

	int guild::dismissGuild(const int playerID)
	{
		memberPtr mem = getMember(playerID);
		if(mem == memberPtr())return -1;
		if(mem->offcial != guild_leader)return 1;//没有权限
		//package deal 
		for (memberMap::iterator it = members.begin(); it != members.end(); ++it)
		{
			memberPtr mem = it->second;
			playerDataPtr currentPlayer = player_mgr.getPlayerMain(mem->getPlayerID());
			if(currentPlayer == playerDataPtr()){
				string message = "\"msg\":[" + boost::lexical_cast<string, int>(mem->getContribute()) + "]";
				na::msg::msg_json mj(playerID, -1, gate_client::player_guild_notice_req, message);
				player_mgr.postMessage(mj);
				continue;
			}
			currentPlayer->Guild.setGuildID(-1, mem->getContribute());
		}
		freeAllData();
		return 0;
	}

	vector<memberPtr> guild::getOffcialRank(const int offcial)
	{
		vector<memberPtr> vec;
		vec.clear();
		for (memberList::const_iterator it = memberRank.begin(); it != memberRank.end(); ++it)
		{
			if(offcial == (*it)->offcial) vec.push_back(*it);
			else if(offcial < (*it)->offcial)break;
		}
		return vec;
	}

	void guild::addMember(memberPtr mem)
	{
		members[mem->getPlayerID()] = mem;
		memNas[mem->getPlayerName()] = mem;
	}

	bool guild::canUpgrade(memberPtr mem)
	{
		if(mem == memberPtr())return false;
		int checkOffcial = mem->offcial - 1;
		if(checkOffcial < 0)return false;
		vector<memberPtr> vec = getOffcialRank(checkOffcial);
		if(checkOffcial == guild_vice_leader && mem->rank > 0 && mem->rank < 3 
			&& (vec.size() < 2 || vec.back()->getHistory() < mem->getHistory()))return true;
		if(checkOffcial == guild_old_member && mem->rank > 0 && mem->rank < 7
			&& (vec.size() < 4 || vec.back()->getHistory() < mem->getHistory()))return true;
		if(checkOffcial == guild_senior_member && mem->rank > 0 && mem->rank < 15
			&& (vec.size() < 8 || vec.back()->getHistory() < mem->getHistory()))return true;
		if(checkOffcial == guild_middle_member && mem->rank > 0 && mem->rank < 31
			&& (vec.size() < 16 || vec.back()->getHistory() < mem->getHistory()))return true;
		if (checkOffcial == guild_lower_member && mem->getHistory() >= 1000)return true;
		if(checkOffcial == guild_leader && !memberRank.empty() && 
			na::time_helper::get_current_time() - memberRank[0]->lastLoginTime > na::time_helper::ONEDAY * 5)return true;
		return false;
	}

	void guild::freeAllData()
	{
		guildName = "";
		guildKingdom = -1;
		rank = -1;
		words = "";
		ownMap.clear();
		ownVec.clear();
		members.clear();
		memNas.clear();
		memberRank.clear();
		createTime = 0;
		serverID = commom_sys.serverID();
	}

	void guild::sortMember()
	{
		memberRank.clear();
		memberPtr leader;
		for (memberMap::iterator it = members.begin(); it != members.end(); ++it){
			it->second->rank = -1;
			if(it->second->offcial == guild_leader){leader = it->second;continue;}
			memberRank.push_back(it->second);
		}
		std::sort(memberRank.begin(), memberRank.end(), guild::descend);
		for (unsigned i = 0; i < memberRank.size(); ++i)
		{
			if(memberRank[i] == memberPtr())continue;
			memberRank[i]->rank = (int)(i + 1);
		}
		std::sort(memberRank.begin(), memberRank.end(), guild::ascendByOffcial);
		if(leader != NULL)memberRank.insert(memberRank.begin(), leader);
	}
}

using namespace Guild;

	guild_system* const guild_system::guildSys = new guild_system();

	void guild_system::createGuild(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		if (js_msg[3u].isNull() || js_msg[3u].asBool())Return(r, 5);//非法字符
		if (d->Base.getLevel() < 17)Return(r, -1);
		int Vip = d->Vip.getVipLevel();
		if (! (Vip > 0 || d->Base.getLevel() >= 35) )Return(r, 4);//vip等级不足
		int kindom = d->Base.getSphereID();
		{			
			if(kindom < 0 || kindom > 2)Return(r, -1);
			int guildID = d->Guild.getGuildID();
			if(guildID >= 0)Return(r, -1);
			guildPtr gPtr = getGuild(guildID);
			if(gPtr != guildPtr())Return(r, -1);
		}
		if(d->Base.getAllGold() < 200)Return(r, 3);//金币不足
		string gName = js_msg[0u].asString();
		int flagIcon = js_msg[1u].asInt();
		string words = js_msg[2u].asString();
		if (commom_sys.UTF8Len(gName) > 7 || commom_sys.UTF8Len(gName) < 2)Return(r, -1);
		if(commom_sys.UTF8Len(words) > 75)Return(r, -1);
		if(hasSameName(gName))Return(r, 2);//相同名字
		int gID = createNewGuildID();
		guildPtr ptr = CreateGuild(gID);
		if(ptr == guildPtr())Return(r, -1);
		int res = ptr->addMember(d, guild_leader);
		if(res == 0)
		{
			d->Base.alterBothGold(-200);
			ptr->guildName = gName;
			ptr->setKingdom(kindom);
			ptr->setFlagIcon(flagIcon);
			ptr->words = words;
			addGuild(ptr);
			Json::Value createJson;
			createJson[strEventID] = guild_event_create;
			unsigned now = na::time_helper::get_current_time();
			createJson[strEventTrriger] = now;
			createJson[strEventData].append(now);
			createJson[strEventData].append(d->Base.getName());
			createJson[strEventData].append(ptr->guildName);
			ptr->addEvent(createJson);
			saveHistory(ptr);
			d->Base.update();
			StreamLog::LogV(logSystemGuild, d, ptr->strLogGID(), gName, log_tag_create, boost::lexical_cast<string, int>(ptr->getKingdom()));
		}
		Return(r, res);
	}

	int guild_system::createNewGuildID()
	{
// 		const static int minID = config_ins.get_config_prame("server_id").asInt() << 20;
// 		const static int maxID = (config_ins.get_config_prame("server_id").asInt() + 1) << 20;
// 		const static int maxLoop = maxID - minID;
// 		static int currentGID = minID;
// 		int loop = 0;
// 		do 
// 		{
// 			guildPtr ptr = getGuild(currentGID);
// 			if(ptr == NULL)return currentGID;
// 			++currentGID;
// 			currentGID = currentGID >= maxID ? minID : currentGID;
// 			++loop;
// 		} while (loop < maxLoop);
// 		return -1;
		return ++currentGID;
	}

	void guild_system::memScientFirst(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if(ptr == guildPtr())Return(r, -1);
		ReadJsonArray;
		int sID = js_msg[0u].asInt();
		memberPtr mem = ptr->getMember(d->playerID);
		if(mem == NULL)Return(r, -1);
		GuildScience::Config c = GuildScience::getConfig(sID);
		if(!c.isVaild())Return(r, -1);
		if(c.Type != 1)Return(r, -1);
		int lmLV = ptr->getScienceData(c.LimitID).LV;
		if(lmLV < c.LimitLevel)Return(r, -1);
		d->Guild.setFirst(sID);
		sendScience(d, ptr);
		StreamLog::LogV(logSystemGuild, d, ptr->strLogGID(), ptr->guildName, log_set_first_science, boost::lexical_cast<string, int>(ptr->getKingdom()), boost::lexical_cast<string, int>(sID));
		Return(r, 0);
	}

	void guild_system::changeFlagIcon(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if(ptr == guildPtr())Return(r, -1);
		int cost = 5000;
		if(ptr->getFlagChange() == 0)cost = 500;
		if(d->Base.getAllGold() < cost)Return(r, 2);
		ReadJsonArray;
		int iID = js_msg[0u].asInt();
		memberPtr mem = ptr->getMember(d->playerID);
		int res = 1;
		if(mem->offcial == guild_leader || mem->offcial == guild_vice_leader)
		{
			ptr->setFlagIcon(iID);
			saveScience(ptr);
			res = 0;

			StreamLog::LogV(logSystemGuild, d, ptr->strLogGID(), ptr->guildName, log_set_flag_icon, boost::lexical_cast<string, int>(ptr->getKingdom()), boost::lexical_cast<string, int>(iID));
			Json::Value flagJson;
			flagJson[strEventID] = guild_event_change_flag_icon;
			flagJson[strEventTrriger] = na::time_helper::get_current_time();
			flagJson[strEventData].append(d->Base.getName());
			ptr->addEvent(flagJson);
			saveHistory(ptr);

			d->Base.alterBothGold(-cost);
		}
		Return(r, res);
	}	

	void guild_system::gmAddGuildScienceExpReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;

		string guildName = js_msg[0u].asString();
		Json::Value configJson = js_msg[1];

		Guild::guildPtr gPtr = getGuild(guildName);
		if (gPtr == Guild::guildPtr())
			Return(r, 1);
		int ret = 0;

		for (unsigned i = 0; i < configJson.size(); i++)
		{
			int sID = configJson[i][0u].asInt();
			int sExp = configJson[i][1u].asInt();

			gPtr->addScienceExp(sID, sExp);
		}
		saveScience(gPtr);

		Return(r, 0);
	}

	void guild_system::addGuild(guildPtr gPtr)
	{
		guilds[gPtr->guildID] = gPtr;
		guildsName[gPtr->guildName] = gPtr;
		sortRank();
		saveMember(gPtr);
	}

// 	void guild_system::dismissGuild(msg_json& m, Json::Value& r)
// 	{
// 		AsyncGetPlayerData(m);
// 		guildPtr ptr = getGuild(d->Guild.getGuildID());
// 		if(ptr == guildPtr())Return(r, -1);
// 		string gName = ptr->guildName;
// 		int gID = ptr->guildID;
// 		int res = ptr->dismissGuild(d->playerID);
// 		if(res == 0){
// 			removeGuild(gID, gName);
// 			StreamLog::Log(logSystemGuild, d, "GuildName", gName, log_tag_dismiss);
// 		}
// 		Return(r, res);
// 	}

	bool guild_system::hasSameName(string name)
	{
		guildPtr ptr = getGuild(name);
		if(ptr == guildPtr())return false;
		return true;
	}

	void guild_system::kickMember(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		string targetNa = js_msg[0u].asString();
		playerDataPtr targetP = player_mgr.getPlayerMainByName(targetNa);
		playerDataPtr d = player_mgr.getPlayerMain(m._player_id, m._net_id);
		if(playerDataPtr() == d || playerDataPtr() == targetP)
		{
			if(m._post_times > 0)Return(r, -1);
			player_mgr.postMessage(m);
			return;
		}
		int guildID = d->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())Return(r, -1);
		memberPtr mem = gPtr->getMember(targetP->playerID);
		if(mem == NULL)Return(r, -1);
		int res = gPtr->kickMember(d, targetP);
		if(res == 0){
			saveMember(gPtr);
			saveHistory(gPtr);
			StreamLog::LogV(logSystemGuild, d, gPtr->strLogGID(), gPtr->guildName, log_tag_kick, boost::lexical_cast<string, int>(gPtr->getKingdom()), 
				boost::lexical_cast<string, int>(targetP->playerID), targetP->Base.getName(), boost::lexical_cast<string, int>(mem->getContribute()));
		}
		Return(r, res);
	}

	void guild_system::changeGName(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		if (js_msg[2u].isNull() || js_msg[2u].asBool())Return(r, 1);//非法字符
		string org_gn = js_msg[0u].asString();
		string mf_gn = js_msg[1u].asString();
		guildPtr gPtr = getGuild(org_gn);
		if (gPtr == NULL)Return(r, 2); //不存在对应的公会
		if (hasSameName(mf_gn))Return(r, 3);//存在相同名字
		if (commom_sys.UTF8Len(mf_gn) > 7 ||
			commom_sys.UTF8Len(mf_gn) < 2)Return(r, 4);//名字太长
		gPtr->guildName = mf_gn;
		guildsName.erase(org_gn);
		guildsName[mf_gn] = gPtr;
		saveMember(gPtr);
		guild_battle_sys.renameGuild(org_gn, mf_gn);
		StreamLog::LogV(logSystemGuild, org_gn, mf_gn, log_tag_motify_name, boost::lexical_cast<string, int>(gPtr->guildID));
		Return(r, 0);
	}

	void guild_system::joinGuild(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < 17)Return(r, -1);

		{			
			int guildID = d->Guild.getGuildID();
			if(guildID >= 0)Return(r, -1);
			guildPtr gPtr = getGuild(guildID);
			if(gPtr != guildPtr())Return(r, -1);
		}

		ReadJsonArray;
		int joinID = js_msg[0u].asInt();
		string words = js_msg[1u].asString();
		guildPtr joinG = getGuild(joinID);
		if(joinG == guildPtr())Return(r, -1);
		if(d->Base.getSphereID() != joinG->getKingdom())Return(r, 100);
		int res = -1;
		if(joinG->autoAdd() && joinG->passLimit(d))
		{
			r[msgStr][1u] = true;
			res = joinG->addMember(d);
			if(res == 0){
				saveMember(joinG);
				saveHistory(joinG);
				Json::Value bCast;
				bCast[msgStr][0u] = 0;
				bCast[msgStr][1u][senderChannelStr] =chat_guild;
				bCast[msgStr][1u][chatBroadcastID] = BROADCAST::guild_member_join;
				const static string ParamListStr = "pl";
				bCast[msgStr][1u][ParamListStr].append(d->Base.getName());
				joinG->sendToAllOnlineMember(gate_client::chat_broadcast_resp, bCast);
				StreamLog::LogV(logSystemGuild, d, joinG->strLogGID(), joinG->guildName, log_tag_add, boost::lexical_cast<string, int>(joinG->getKingdom()));
			}
		}else{
			r[msgStr][1u] = false;
			res = joinG->memberJoin(d, words);
			if(res == 0){
				saveJoin(joinG);
				StreamLog::LogV(logSystemGuild, d, joinG->strLogGID(), joinG->guildName, log_tag_apply, boost::lexical_cast<string, int>(joinG->getKingdom()));
			}
		}
		Return(r, res);
	}

	void guild_system::leaveGuild(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int guildID = d->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())Return(r, -1);
		memberPtr mem = gPtr->getMember(d->playerID);
		if(mem == NULL)Return(r, -1);
		int res = gPtr->memberLeave(d);
		if(res == 0){
			saveMember(gPtr);
			Json::Value bCast;
			bCast[msgStr][0u] = 0;
			bCast[msgStr][1u][senderChannelStr] =chat_guild;
			bCast[msgStr][1u][chatBroadcastID] = BROADCAST::guild_member_leave;
			const static string ParamListStr = "pl";
			bCast[msgStr][1u][ParamListStr].append(d->Base.getName());
			gPtr->sendToAllOnlineMember(gate_client::chat_broadcast_resp, bCast);
			StreamLog::LogV(logSystemGuild, d, gPtr->strLogGID(), gPtr->guildName, log_tag_leave, boost::lexical_cast<string, int>(gPtr->getKingdom()),
				boost::lexical_cast<string, int>(mem->getContribute()));
		}else if(res == 2 && gPtr->getMemberNum() < 2){
			string gName = gPtr->guildName;
			string strGID = gPtr->strLogGID();
			if(0 == gPtr->dismissGuild(d->playerID))
			{
				removeGuild(guildID, gName);
				guild_battle_sys.processRemoveGuild(gName);
				StreamLog::LogV(logSystemGuild, d, strGID, gName, log_tag_dismiss, boost::lexical_cast<string, int>(gPtr->getKingdom()));
				res = 0;
			}
		}
		Return(r, res);
	}

	void guild_system::memberUpgrade(msg_json& m, Json::Value& r)
	{
		//todo:
		AsyncGetPlayerData(m);
		int guildID = d->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())Return(r, -1);
		int res = gPtr->memberUpgrade(d);
		if(res == 0){
			saveMember(gPtr);
			saveHistory(gPtr);
		}
		Return(r, res);
	}

	void guild_system::removeHelp(playerDataPtr player, const int TYPE)
	{
		guildPtr gPtr = getGuild(player->Guild.getGuildID());
		if (gPtr == NULL)return;

		if (gPtr->removeHelp(player->playerID, TYPE))
		{
			sendToOnlineHelpList(gPtr);
		}
	}

	Json::Value guild_system::getPersonal(playerDataPtr player)
	{
		Json::Value value = Json::objectValue;
		guildPtr gPtr = getGuild(player->Guild.getGuildID());
		if(gPtr == guildPtr())return value;
		memberPtr mem = gPtr->getMember(player->playerID);
		if(mem == memberPtr())return value;
		value = mem->packageUpdate();
		value[guildNameStr] = gPtr->guildName;
		return value;
	}

	int guild_system::alterContribute(playerDataPtr player, int num)
	{
		int guildID = player->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if (gPtr == guildPtr())return -1;
		memberPtr mem = gPtr->getMember(player->playerID);
		if (mem == NULL)return -1;
		mem->alterContribute(num);
		saveMember(gPtr);
		return 0;
	}

	int guild_system::donateDefault(playerDataPtr player, int num)
	{
		int guildID = player->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())return -1;
		memberPtr mem = gPtr->getMember(player->playerID);
		if(mem == NULL)return -1;
		return donate(player, player->Guild.getFirst(), num);
	}

	int guild_system::donate(playerDataPtr player, int scID, int num)
	{
		if(num <= 0)return -1;
		int guildID = player->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())return -1;
		GuildScience::Config c = GuildScience::getConfig(scID);
		if(!c.isVaild())return -1;
		int cLV = gPtr->getScienceData(scID).LV;
		//if(cLV >= (int)c.Exp.size())return -1;
		{
			int lmLV = gPtr->getScienceData(c.LimitID).LV;
			if(lmLV < c.LimitLevel)return -1;
		}
		if(c.LimitEffectIdx != -1)
		{
			int lmLV =  gPtr->getScienceData(c.LimitEffectIdx).LV;
			if(cLV >= lmLV)return -1;
		}
		bool GL = false;
		int res = gPtr->memberDonate(player, num, scID, GL);
		if(res == 0){
			if(GL)sortRank();
			saveMember(gPtr);
			saveScience(gPtr);
			saveHistory(gPtr);
			sendScience(player, gPtr);
			task_sys.updateBranchTaskInfo(player, _task_guild_donate_times);
			activity_sys.updateActivity(player, 0, activity::_guild_donate);
			StreamLog::LogV(logSystemGuild ,player, gPtr->strLogGID(), gPtr->guildName, log_tag_donate, boost::lexical_cast<string, int>(gPtr->getKingdom()), boost::lexical_cast<string, int>(scID), boost::lexical_cast<string, int>(num));
		}
		return res;
	}

	void guild_system::donateGuild(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
// 		unsigned now = na::time_helper::get_current_time();
// 		if(now < d->Guild.getJoinTime() || now - d->Guild.getJoinTime() < 7200)Return(r,5);//加入不够2个小时
		ReadJsonArray;
		int scID = js_msg[0u].asInt();
		int type = js_msg[1u].asInt();
		int vip = d->Vip.getVipLevel();
		int times = d->Guild.getDonate(scID);
		if(vip < 0 || times < 0)Return(r, -1);
		GuildScience::Config c = GuildScience::getConfig(scID);
		if(!c.isVaild())Return(r, -1);
		if(times >= (int)c.Vip.size())Return(r, 1);//次数用完了
		if(vip < c.Vip[times])Return(r, 2);//vip等级不足
		int num = 0;
		int dn = d->Base.getLevel() * 200;
		if(0 == type)
		{
			num = d->Base.getLevel() * c.Silver[times];
			if(d->Base.getSilver() < num)Return(r, 3);//银币不足
		}
		else
		{
			num = c.Gold[times];
			if(d->Base.getAllGold() < num)Return(r, 4);//金币不足
		}
		if(num < 0 || dn < 0)Return(r,-1);
		int res = donate(d, scID, dn);
		if(res == 0)
		{
			if(0 == type)
			{
				d->Base.alterSilver(-num);
			}
			else
			{
				d->Base.alterBothGold(-num);
			}
			d->Guild.tickDonate(scID);
		}
		r[msgStr][2u] = dn;
		unsigned now = na::time_helper::get_current_time();
		if (now > d->Guild.getJoinTime() && now - d->Guild.getJoinTime() < 3 * na::time_helper::ONEDAY)dn /= 3;
		r[msgStr][1u] = dn;
		//activity_sys.updateActivity(d, 0, activity::_guild_donate);
		Return(r, res);
	}

// 	void guild_system::upgradeGuildFlag(msg_json& m, Json::Value& r)
// 	{
// 		AsyncGetPlayerData(m);
// 		guildPtr ptr = getGuild(d->Guild.getGuildID());
// 		if(ptr == guildPtr())Return(r, -1);
// 		int flagLevel = ptr->getFlagLevel();
// 		if(flagLevel < 1 || flagLevel > 9)Return(r, 1);
// 		int cost = upgradeCost[flagLevel - 1];
// 		if(cost > d->Base.getAllGold())Return(r, 2);
// 		d->Base.alterBothGold(-cost);
// 		ptr->setFlagLevel(flagLevel + 1);
// 		saveScience(ptr);
// 		Json::Value flagJson;
// 		flagJson[strEventID] = guild_event_flag_upgrade;
// 		flagJson[strEventTrriger] = na::time_helper::get_current_time();
// 		flagJson[strEventData].append(d->Base.getName());
// 		flagJson[strEventData].append(cost);
// 		flagJson[strEventData].append(ptr->getFlagLevel());
// 		ptr->addEvent(flagJson);
// 		saveHistory(ptr);
// 		sendGuildBase(d, ptr);
// 		StreamLog::LogV(logSystemGuild, d, ptr->guildID, ptr->guildName, log_upgrade_member_size, boost::lexical_cast<string, int>(ptr->getKingdom()), boost::lexical_cast<string, int>(ptr->getKingdom()), boost::lexical_cast<string, int>(ptr->getFlagLevel()));
// 		Return(r, 0);
// 	}

	void guild_system::sendGuildBase(playerDataPtr player, guildPtr gPtr)
	{
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1u][updateFromStr] = State::getState();
		updateJson[msgStr][1u][guildListStr].append(gPtr->packageBase(player));
		player->sendToClient(gate_client::guild_update_base_resp, updateJson);
	}

	void guild_system::updateGuildHistory(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if(ptr == guildPtr())Return(r, -1);
		ReadJsonArray;
		unsigned beginIdx = js_msg[0u].asUInt();
		unsigned endIdx = js_msg[1u].asUInt();
		if(beginIdx > endIdx || endIdx - beginIdx + 1 > 10)Return(r, -1);
		r[msgStr][0u] = 0;
		r[msgStr][1u][updateFromStr] = m._type;
		Json::Value& list = r[msgStr][1u][guildHistorystr];
		list = Json::arrayValue;
		list = ptr->packageHistory(beginIdx, endIdx);
	}

	void guild_system::setJoinLimit(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if(ptr == guildPtr())Return(r, -1);
		ReadJsonArray;
		int levelLimit = js_msg[0u].asInt();
		int rankLimit = js_msg[1u].asInt();
		int storyLimit = js_msg[2u].asInt();
		bool autoApply = js_msg[3u].asBool();
		memberPtr player = ptr->getMember(d->playerID);
		if (player == memberPtr() || player->offcial < 0 || player->offcial > guild_old_member)Return(r, 1);//没有权限
		ptr->setLimit(levelLimit, rankLimit, storyLimit, autoApply);
		saveLimit(ptr);
		sendGuildBase(d, ptr);
		Return(r, 0);
	}

	void guild_system::abdication(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		string targetNa = js_msg[0u].asString();
		playerDataPtr targetP = player_mgr.getPlayerMainByName(targetNa);
		playerDataPtr d = player_mgr.getPlayerMain(m._player_id, m._net_id);
		if(playerDataPtr() == d || playerDataPtr() == targetP)
		{
			if(m._post_times > 0)Return(r, -1);
			player_mgr.postMessage(m);
			return;
		}
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if(ptr == guildPtr())Return(r, -1);
		int res = ptr->abdication(d, targetP);
		if(res == 0){
			saveMember(ptr);
			Json::Value bCast;
			bCast[msgStr][0u] = 0;
			bCast[msgStr][1u][senderChannelStr] =chat_guild;
			bCast[msgStr][1u][chatBroadcastID] = BROADCAST::guild_leader_change;
			const static string ParamListStr = "pl";
			bCast[msgStr][1u][ParamListStr].append(targetNa);
			ptr->sendToAllOnlineMember(gate_client::chat_broadcast_resp, bCast);
			StreamLog::LogV(logSystemGuild, d, ptr->strLogGID(), ptr->guildName, log_tag_change_leader, boost::lexical_cast<string, int>(ptr->getKingdom()), boost::lexical_cast<string, int>(targetP->playerID), targetP->Base.getName());
		}
		Return(r, res);
	}

	void guild_system::cancelJoin(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int gID = js_msg[0u].asInt();
		if(!d->Guild.hasApplyGuild(gID))Return(r, -1);
		guildPtr ptr = getGuild(gID);
		if(ptr == guildPtr()){
			d->Guild.removeGuild(gID);
			Return(r, 0);
		}
		int res = ptr->cancelJoin(d);
		if(res == 0)saveJoin(ptr);
		Return(r, res);
	}

	void guild_system::updateGuildScience(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if(ptr == guildPtr())Return(r, -1);
		sendScience(d, ptr);
	}

	void guild_system::updateJoinGuild(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		vector<int> gList = d->Guild.getJoinList(); 
		Json::Value nullGuild = Json::objectValue;
		r[msgStr][0u] = 0;
		r[msgStr][1u][guildListStr] = Json::arrayValue;
		Json::Value& pData = r[msgStr][1u][guildListStr];
		for (unsigned i =0; i < gList.size(); ++i)
		{
			guildPtr ptr = getGuild(gList[i]);
			if(ptr == guildPtr())pData.append(nullGuild);
			else pData.append(ptr->packageBase(d));
		}
	}

	const static string strTradeSkillLog = "log_trade_skill";
	void guild_system::upgradeTradeSkillReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if (ptr == guildPtr())Return(r, -1);
		memberPtr player_m = ptr->getMember(d->playerID);
		if (player_m == NULL)Return(r, -1);
		ReadJsonArray;
		int skillID = js_msg[0u].asInt();
		if (skillID < 0 || skillID > 5)Return(r, -1);
		int skillLevel = d->TradeSkill.getSkill(skillID);
		if (skillLevel < 0 || skillLevel >= (int)TradeSkillCostVec.size())Return(r, -1);
		const TradeSkillCost& cost = TradeSkillCostVec[skillLevel];
		int costGX = cost.gongxian;
		int costSL = cost.jinbi;
		int costWW = cost.rongyu;
		if (player_m->getContribute() < costGX)Return(r, 1);//贡献不足
		if (d->Base.getSilver() < costSL)Return(r, 2);//金币不足
		if (d->Base.getWeiWang() < costWW)Return(r, 3);//荣誉值不足
		//科技升级
		if (skillLevel + 1 > ptr->getScienceData(playerGuild::guild_level).LV + 50)Return(r, 5);//星盟等级不足
		if (!d->TradeSkill.setSkill(skillID, skillLevel + 1))Return(r, 4);//满级了
		player_m->alterContribute(-costGX);
		saveMember(ptr);
		d->Base.alterSilver(-costSL);
		d->Base.alterWeiWang(-costWW);
		d->Pilots.recalBattleValue();
		StreamLog::LogV(strTradeSkillLog, d, skillID, skillLevel + 1, -1, boost::lexical_cast<string, int>(costGX),
			boost::lexical_cast<string, int>(costSL), boost::lexical_cast<string, int>(costWW),
			boost::lexical_cast<string, int>(player_m->getContribute()), boost::lexical_cast<string, int>(d->Base.getSilver()),
			boost::lexical_cast<string, int>(d->Base.getWeiWang()));
		Return(r, 0);
	}

	void guild_system::tickHelpReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if (ptr == guildPtr())Return(r, -1);
		memberPtr player_m = ptr->getMember(d->playerID);
		if (player_m == NULL)Return(r, -1);
		ReadJsonArray;
		if (js_msg.size() > 20 || js_msg.size() < 1)Return(r, -1);
		vector<string> strV;
		bool Post = false;
		playerManager::BHIDMAP playerList;
		for (unsigned i = 0; i < js_msg.size(); ++i)
		{
			if (!js_msg[i].isString())continue;
			string key = js_msg[i].asString();
			HelpDataPtr hPtr = ptr->getHelpPtr(key);
			if (hPtr == NULL)continue;
			playerDataPtr target = player_mgr.getPlayerMain(hPtr->M_->getPlayerID());
			if (target == NULL)
			{
				Post = true;
				continue;
			}
			playerList[target->playerID] = target;
			strV.push_back(key);
		}
		if (Post)
		{
			if (m._post_times > 1)Return(r, -1);
			player_mgr.postMessage(m);
			return;
		}

		int oldc = player_m->getContribute();
		int oldsil = d->Base.getSilver();
		int oldtick = d->Guild.getHelpTick();
		int cri = 0;
		int num = ptr->TickHelp(d, player_m, playerList, strV, &cri);
		if (num < 1)
		{
			sendHelpList(d, ptr);
			Return(r, 1);//列表已经更新,请重试
		}
		saveMember(ptr);
		sendHelpList(d, ptr);
		r[msgStr][1u] = player_m->getContribute() - oldc;
		r[msgStr][2u] = num;
		r[msgStr][3u] = d->Base.getSilver() - oldsil;
		r[msgStr][4u] = cri;
		r[msgStr][5u] = oldtick;
		Return(r, 0);
	}

	void guild_system::guildHelpReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int type = js_msg[0u].asInt();
		if (type < 0 || type >= GuildHelp::help_end)Return(r, -1);
		int seed = js_msg[1u].asInt();
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if (ptr == guildPtr())Return(r, -1);
		memberPtr mem = ptr->getMember(d->playerID);
		if (!ptr->AddHelp(d, mem, type, seed))
		{
			Return(r, 1);//重复事件
		}
		sendToOnlineHelpList(ptr);
		Return(r, 0);
	}

	void guild_system::updateGuildHelp(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if (ptr == guildPtr())Return(r, -1);
		sendHelpList(d, ptr);
	}

	void guild_system::updateGuildBase(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		guildPtr ptr = getGuild(d->Guild.getGuildID());
		if(ptr == guildPtr())Return(r, -1);
		sendGuildBase(d, ptr);
	}

	void guild_system::gmUpdateGuildBaseReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		string guildName = js_msg[0u].asString();
		guildPtr ptr = getGuild(guildName);
		if(ptr == guildPtr())Return(r, -1);

		r[msgStr][1u][updateFromStr] = State::getState();
		r[msgStr][1u][guildListStr].append(ptr->gmPackageBase());

		Return(r, 0);
	}

	void guild_system::responseJoin(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int type = js_msg[0u].asInt();
		if(!js_msg[1u].isArray())Return(r, -1);
		vector<playerDataPtr> targetVec, checkVec;
		bool allOnMem = true;
		unsigned sz = js_msg[1u].size();
		if(sz >= 50)Return(r, -1);
		for (unsigned i = 0; i < sz; ++i)
		{
			int target = js_msg[1u][i].asInt();
			playerDataPtr targetP = player_mgr.getPlayerMain(target);
			if(targetP == playerDataPtr()){allOnMem = false;continue;}
			targetVec.push_back(targetP);
		}
		playerDataPtr d = player_mgr.getPlayerMain(m._player_id, m._net_id);
		if(playerDataPtr() == d || !allOnMem)
		{
			if(m._post_times > 0)Return(r, -1);
			player_mgr.postMessage(m);
			return;
		}
		int guildID = d->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())Return(r, -1);
		for (unsigned i = 0; i < targetVec.size(); ++i)
		{
			if(!gPtr->hasJoin(targetVec[i]->playerID))continue;
			checkVec.push_back(targetVec[i]);
		}
		int res = gPtr->applyMember(d, checkVec, type);
		if(res == 0){
			saveMember(gPtr);
			saveJoin(gPtr);
			saveHistory(gPtr);
			sendJoinMember(d, gPtr);
			sendGuildBase(d, gPtr);
		}
		Return(r, res);
	}

	void guild_system::updateGuildList(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		unsigned beginIdx = js_msg[0u].asUInt();
		unsigned endIdx = js_msg[1u].asUInt();
		if(beginIdx > endIdx || endIdx - beginIdx + 1 > 10)Return(r, -1);
		r[msgStr][0u] = 0;
		r[msgStr][1u][updateFromStr] = m._type;
		Json::Value& list = r[msgStr][1u][guildListStr];
		list = Json::arrayValue;
		for (unsigned i = beginIdx; i <= endIdx && i < guildRank.size(); ++i)
			list.append(guildRank[i]->packageBase(d));
	}

	void guild_system::updateGuildMember(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int guildID = d->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())Return(r, -1);
		ReadJsonArray;
		unsigned beginIdx = js_msg[0u].asUInt();
		unsigned endIdx = js_msg[1u].asUInt();		
		if(beginIdx > endIdx || endIdx - beginIdx + 1 > 30)Return(r, -1);
		r[msgStr][0u] = 0;
		r[msgStr][1u][updateFromStr] = m._type;
		r[msgStr][1u][guildMemberStr] = gPtr->packageMember(beginIdx, endIdx);
		r[msgStr][1u][memberYourSelfStr] = Json::objectValue;
		memberPtr player = gPtr->getMember(d->playerID);
		if(player != memberPtr())r[msgStr][1u][memberYourSelfStr] = player->packageUpdate();
	}

	void guild_system::gmUpdateGuildMember(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		string guildName = js_msg[0u].asString();
		guildPtr gPtr = getGuild(guildName);
		if(gPtr == guildPtr())Return(r, -1);
		unsigned beginIdx = js_msg[1].asUInt();
		unsigned endIdx = js_msg[2].asUInt();		
		if(beginIdx > endIdx || endIdx - beginIdx + 1 > 30)Return(r, -1);
		r[msgStr][0u] = 0;
		r[msgStr][1u][guildMemberStr] = gPtr->packageMember(beginIdx, endIdx);
	}

	void guild_system::updateGulildJoinMemBer(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int guildID = d->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())Return(r, -1);
		memberPtr mem = gPtr->getMember(d->playerID);
		if(mem == memberPtr())Return(r, -1);
		if(mem->offcial < guild_leader || mem->offcial > guild_old_member)Return(r, -1);
		sendJoinMember(d, gPtr);
	}

	void guild_system::sendHelpList(playerDataPtr player)
	{
		int guildID = player->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if (gPtr == guildPtr())return;
		sendHelpList(player, gPtr);
	}

	void guild_system::sendHelpList(playerDataPtr player, Guild::guildPtr gPtr)
	{
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1u][updateFromStr] = State::getState();
		updateJson[msgStr][1u][guildHelpStr] = gPtr->packageGHelp(player->playerID);
		player->sendToClient(gate_client::guild_help_update_resp, updateJson);
	}

	void guild_system::sendToOnlineHelpList(Guild::guildPtr gPtr)
	{
		vector<int> pList;
		gPtr->getAllMembers(pList);
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1u][updateFromStr] = State::getState();
		for (unsigned i = 0; i < pList.size(); ++i)
		{
			playerDataPtr player = player_mgr.getOnlinePlayer(pList[i]);
			if (player == NULL)continue;
			updateJson[msgStr][1u][guildHelpStr] = gPtr->packageGHelp(player->playerID);
			player->sendToClient(gate_client::guild_help_update_resp, updateJson);
		}
	}

	void guild_system::sendScience(playerDataPtr player, Guild::guildPtr gPtr)
	{
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1u][updateFromStr] = State::getState();
		updateJson[msgStr][1u][guildScienceStr] = gPtr->packageJScience();
		updateJson[msgStr][1u][memFirstStr] = player->Guild.getFirst();
		player->sendToClient(gate_client::guild_update_science_resp, updateJson);
	}

	void guild_system::sendJoinMember(playerDataPtr player, guildPtr gPtr)
	{
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1u][updateFromStr] = State::getState();
		updateJson[msgStr][1u][guildMemberStr] = gPtr->packageJoinMember();
		player->sendToClient(gate_client::guild_update_join_member_resp, updateJson);
	}

	void guild_system::asyncPlayerData(playerDataPtr player)
	{
		int guildID = player->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())return;
		bool isUpdate = gPtr->asyncPlayerData(player);
		if(isUpdate)saveMember(gPtr);
	}

	void guild_system::addGuildScienceExp(playerDataPtr player, const int sID, const int num)
	{
		int guildID = player->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())return;
		gPtr->addScienceExp(sID, num);
		saveScience(gPtr);
	}

	int guild_system::getGuildOfficial(playerDataPtr player)
	{
		guildPtr gPtr = getGuild(player->Guild.getGuildID());
		if(gPtr == guildPtr())
			return -1;
		return gPtr->getOffcial(player->playerID);
	}

	string guild_system::getGuildName(const int gID)
	{
		guildPtr gPtr = getGuild(gID);
		if(gPtr == guildPtr())return "";
		return gPtr->guildName;
	}

	GuildScience::ScienceData guild_system::getScienceData(const int gID, const int sID)
	{
		guildPtr gPtr = getGuild(gID);
		if(gPtr == guildPtr())return GuildScience::ScienceData();
		return gPtr->getScienceData(sID);
	}

	void guild_system::removeJoinAndBackUpInfo(const int playerID, const int guildID)
	{
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())return;
		if(gPtr->removeJoinMember(playerID))saveJoin(gPtr);
	}

	playerManager::playerDataVec guild_system::getOnlineMember(const int gID)
	{
		playerManager::playerDataVec vec;
		guildPtr gPtr = getGuild(gID);
		if(gPtr == guildPtr())return vec;
		vector<int> mems;
		gPtr->getAllMembers(mems);
		for (unsigned i = 0; i < mems.size(); ++i)
		{
			playerDataPtr player = player_mgr.getOnlinePlayer(mems[i]);
			if(player == NULL)continue;
			vec.push_back(player);
		}
		return vec;
	}

	void guild_system::motifyWords(msg_json& m, Json::Value& r)
	{
		//todo:
		AsyncGetPlayerData(m);
		ReadJsonArray;
		string words = js_msg[0u].asString();
		int guildID = d->Guild.getGuildID();
		guildPtr gPtr = getGuild(guildID);
		if(gPtr == guildPtr())Return(r, -1);
		int res = gPtr->motifyWords(d, words);
		if(res == 0){
			saveMember(gPtr);
			StreamLog::LogV(logSystemGuild, d, gPtr->strLogGID(), gPtr->guildName, log_tag_words, boost::lexical_cast<string, int>(gPtr->getKingdom()), words);
			Json::Value wordJson;
			wordJson[strEventID] = guild_event_change_announce;
			wordJson[strEventTrriger] = na::time_helper::get_current_time();
			wordJson[strEventData].append(d->Base.getName());
			gPtr->addEvent(wordJson);
		}
		Return(r, res);
	}

	void guild_system::addGuilBattleEvent(string guildName, int areaID)
	{
		guildPtr gPtr = getGuild(guildName);
		if(gPtr == guildPtr())
			return;

		unsigned cur_time = na::time_helper::get_current_time();
		Json::Value eventJson;
		eventJson[strEventID] = guild_event_won_field;
		eventJson[strEventTrriger] = cur_time;
		eventJson[strEventData].append(season_sys.getYear(cur_time));
		eventJson[strEventData].append(season_sys.getSeason());
		eventJson[strEventData].append(areaID);
		gPtr->addEvent(eventJson);
		saveHistory(gPtr);
	}

	void guild_system::loadAllGuild()
	{
		db_mgr.ensure_index(dbGuildMember, BSON(guildIDStr << 1));
		db_mgr.ensure_index(dbGuildMember, BSON(guildNameStr << 1));
		db_mgr.ensure_index(dbGuildJoinList, BSON(guildIDStr << 1));
		db_mgr.ensure_index(dbGuildHelpList, BSON(guildIDStr << 1 << playerIDStr << 1));
		db_mgr.ensure_index(dbGuildJoinLimit, BSON(guildIDStr << 1));
		db_mgr.ensure_index(dbGuildScience, BSON(guildIDStr << 1));
		db_mgr.ensure_index(dbGuildHistory, BSON(guildIDStr << 1));

		guildRank.clear();
		guilds.clear();
		guildsName.clear();
		currentGID = 0;
		objCollection objC = db_mgr.Query(dbGuildMember);
		LogS << "load " << color_yellow(objC.size()) << " guild info from DB" << LogEnd;
		for (unsigned  i = 0; i < objC.size(); ++i)
		{
			BSONObj& obj = objC[i];
			int gID = -1;
			checkNotEoo(obj[guildIDStr]){gID = obj[guildIDStr].Int();}
			if(gID < 0)continue;
			currentGID = gID > currentGID ? currentGID : gID;
			guildPtr ptr = guild_system::CreateGuild(gID);
			mongo::BSONObj key = BSON(guildIDStr << gID);
			if(ptr != guildPtr()){
				if (ptr->initGuildMember(obj))
				{
					saveMember(ptr);
				}
				{
					mongo::BSONObj obj = db_mgr.FindOne(dbGuildJoinList, key);
					ptr->initGuildJoin(obj, gID);
				}
				{
					mongo::BSONObj obj = db_mgr.FindOne(dbGuildJoinLimit, key);
					ptr->initLimit(obj);
				}
				{				
					mongo::BSONObj obj = db_mgr.FindOne(dbGuildScience, key);
					ptr->formatBson(obj);
				}
				{
					mongo::BSONObj obj = db_mgr.FindOne(dbGuildHistory, key);
					ptr->initGuildHistory(obj);
				}
				{
					objCollection objs = db_mgr.Query(dbGuildHelpList, key);
					for (unsigned i = 0; i < objs.size(); ++i)
					{
						mongo::BSONObj& obj = objs[i];
						memberPtr mem = ptr->getMember(obj[playerIDStr].Int());
						if (mem == NULL)
						{
							mongo::BSONObj rmKey = BSON(guildIDStr << gID
								<< playerIDStr << obj[playerIDStr].Int());
							db_mgr.remove_collection(dbGuildHelpList, rmKey);
							continue;
						}
						ptr->formatHelp(mem, obj);
					}
				}
				guilds[gID] = ptr;
				guildsName[ptr->guildName] = ptr;
				ptr->sortMember();
			}
		}
		sortRank(false);
	}

	void guild_system::saveMember(guildPtr gPtr)
	{
		mongo::BSONObj key = BSON(guildIDStr << gPtr->guildID);
		mongo::BSONObj value = gPtr->package();
		db_mgr.save_mongo(dbGuildMember, key, value);
	}

	void guild_system::saveJoin(guildPtr gPtr)
	{
		mongo::BSONObj key = BSON(guildIDStr << gPtr->guildID);
		mongo::BSONObj value = BSON(guildIDStr << gPtr->guildID << 
			joinMemberStr << gPtr->packageBJoin());
		db_mgr.save_mongo(dbGuildJoinList, key, value);
	}

	void guild_system::saveLimit(Guild::guildPtr gPtr)
	{
		mongo::BSONObj key = BSON(guildIDStr << gPtr->guildID);
		mongo::BSONObj value = BSON(guildIDStr << gPtr->guildID << 
			joinLimitStr << gPtr->packageBLimit());
		db_mgr.save_mongo(dbGuildJoinLimit, key, value);
	}

	void guild_system::saveHistory(Guild::guildPtr gPtr)
	{
		mongo::BSONObj key = BSON(guildIDStr << gPtr->guildID);
		mongo::BSONObj value = BSON(guildIDStr << gPtr->guildID << 
			guildHistorystr << gPtr->packageBHis());
		db_mgr.save_mongo(dbGuildHistory, key, value);
	}

	void guild_system::saveScience(Guild::guildPtr gPtr)
	{
		if (gPtr == NULL)return;
		mongo::BSONObj key = BSON(guildIDStr << gPtr->guildID);
		mongo::BSONObj value = BSON(guildIDStr << gPtr->guildID << 
			guildScienceStr << gPtr->packageBScience());
		db_mgr.save_mongo(dbGuildScience, key, value);
	}

	void guild_system::removeGuild(const int gID, const string gName)
	{
		guilds.erase(gID);
		guildsName.erase(gName);
		sortRank();
		mongo::BSONObj key = BSON(guildIDStr << gID);
		db_mgr.remove_collection(dbGuildMember, key);
		db_mgr.remove_collection(dbGuildJoinList, key);
		db_mgr.remove_collection(dbGuildJoinLimit, key);
		db_mgr.remove_collection(dbGuildScience, key);
		db_mgr.remove_collection(dbGuildHistory, key);
		db_mgr.remove_collection(dbGuildHelpList, key);
	}

//	const static string strEmblemCostConfig = "./instance/GuildConfig/emblem_cost.json";
	void guild_system::initData()
	{
		db_mgr.ensure_index(playerGuildDBStr, BSON(playerIDStr << 1));

		GuildScience::initConfig();
		HelpData::initData();

// 		upgradeCost.clear();
// 		Json::Value config = na::file_system::load_jsonfile_val(strEmblemCostConfig);
// 		for (unsigned i = 0; i < config.size(); ++i)
// 		{
// 			upgradeCost.push_back(config[i].asInt());
// 		}

		loadAllGuild();
		//currentGID = createNewGuildID();
	}
	
	void guild_system::sortRank(const bool log /* = true */)
	{
		guildRank.clear();
		for (guildMap::iterator it = guilds.begin(); it != guilds.end(); ++it)
			guildRank.push_back(it->second);
		std::sort(guildRank.begin(), guildRank.end(), guild_system::descend);
		for (unsigned i = 0; i < guildRank.size(); ++i)
		{
			if(guildRank[i] == guildPtr())continue;
			int oldr = guildRank[i]->rank; 
			guildRank[i]->rank = (int)i + 1;
			if(log && oldr != guildRank[i]->rank)
			{
				StreamLog::LogV(logSystemGuild, guildRank[i]->strLogGID(), guildRank[i]->guildName,
					log_guild_rank, boost::lexical_cast<string, int>(guildRank[i]->getKingdom()), boost::lexical_cast<string, int>(oldr),
					boost::lexical_cast<string, int>(guildRank[i]->rank));
			}
		}

		{
			TradeSkillCostVec.clear();
			Json::Value costJson = na::file_system::load_jsonfile_val("./instance/trade/skill_cost.json");
			Json::Value& gongxianJson = costJson[0u];
			Json::Value& jinbiJson = costJson[1u];
			Json::Value& rongyuJson = costJson[2u];
			for (unsigned i = 0; i < gongxianJson.size() || i < jinbiJson.size() || i < rongyuJson.size(); ++i)
			{
				TradeSkillCostVec.push_back(TradeSkillCost(gongxianJson[i].asInt(), jinbiJson[i].asInt(), rongyuJson[i].asInt()));
			}
		}
	}

	guildPtr guild_system::getGuild(const int gID)
	{
		guildMap::iterator it = guilds.find(gID);
		if(it == guilds.end())
			return guildPtr();
		return it->second;
	}

	guildPtr guild_system::getGuild(const string name)
	{
		gNameMap::iterator it = guildsName.find(name);
		if(it == guildsName.end())
			return guildPtr();
		return it->second;
	}
}