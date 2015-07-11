#include "player_base.h"
#include "playerManager.h"
#include "workshop_system.h"
#include "helper.h"
#include "file_system.h"
#include "world_system.h"
#include "science_system.h"
#include "record_system.h"
#include "system_response.h"
#include "game_server.h"
#include "service_config.h"
#include "office_system.h"
#include "task_system.h"
#include "kingdom_system.h"
#include "embattle_sys.h"
#include "mine_resource_system.h"
#include "arena_system.h"
#include "ruler_system.h"
#include "chat_system.h"
#include "world_system.h"
#include "ally_system.h"
#include "guild_system.h"
#include "player_trade.h"
#include "level_rank.h"

namespace gg
{
	playerBase::playerBase(playerData& own) : Block(own)
	{
		planetID = -1;
		silver_ = 6000;
		goldTicket_ = 50;
		gold_ = 0;
		level_ = 1;
		name_ = "";
		areaPage_ = -1;
		areaID_ = -1;
		population_ = 50000;
		boom_ = 0;
		sphereID_ = -1;
		invest_ = 3;
		shared_ = 10;
		weiwang_ = 200;
		keji_ = 200;
		exp_ = 0;
		faceID_ = -1;
		setNameTimes = 0;
		createRoleTime_ = 0;
		lastLoginTime_ = 0;
		playerProcess_ = 0;
		gender = playerDefine::other;
	}

	struct Level_s
	{
		Level_s():mID(-1), lLimit(-1)
		{
			memset(this, 0x0, sizeof(Level_s));
		}
		Level_s(const int mapID, const int levelLimit) : mID(mapID), lLimit(levelLimit)
		{

		}
		Level_s& operator =(Level_s& source_)
		{
			memcpy(this, &source_, sizeof(Level_s));
			return *this;
		}

		const int mID;
		const int lLimit;
	};
	vector<int> playerBase::playerUpgradeExp;
	static vector<Level_s> vlLimit;
	const static string playerRoleExpConfigStr = "./instance/playerExp/playerExp.json";
	const static string playerRoleExpLimitConfigStr = "./instance/playerExp/expLimit.json";
	const static string mysqlLogSilver = "log_silver";
	const static string mysqlLogGold = "log_gold";
	const static string mysqlLogKeJi = "log_keji";
	const static string mysqlLogRoleLevel = "log_role_level";
	const static string mysqlLogRoleExp = "log_role_exp";
	const static string mysqlLogPlayerName = "log_player_name";
	const static string mysqlLogPlayerKindom = "log_player_kingdom";
	const static string mysqlLogPlayerLocal = "log_player_local";
	const static string mysqlLogClientProcess = "log_client_process";
	const static string mysqlLogPlayerGender = "log_player_gender";
	const static string mysqlLogWeiWang = "log_player_weiwang";
	const static string mysqlLogPopulation = "log_player_population";

	Params::ArrayValue PopulationModule;
	Params::ArrayValue PlayerPay;
	void playerBase::initPublicData()
	{
		playerUpgradeExp.clear();
		LogS << "load player exp config json..." << LogEnd;
		Json::Value expJson = na::file_system::load_jsonfile_val(playerRoleExpConfigStr);
		for (unsigned i = 0; i < expJson.size(); ++i)
		{
			playerUpgradeExp.push_back(expJson[i].asInt());
		}

		{
			Json::Value val = na::file_system::load_jsonfile_val("./instance/world_extern/population.json");
			Params::ArrayValue tmp(val);
			PopulationModule = tmp;
		}

		{
			Json::Value val = na::file_system::load_jsonfile_val("./instance/playerpay/pay.json");
			Params::ArrayValue tmp(val);
			PlayerPay = tmp;
		}

		vlLimit.clear();
		LogS << "load player exp  limit config json..." << LogEnd;
		Json::Value limitJson = na::file_system::load_jsonfile_val(playerRoleExpLimitConfigStr);
		for (unsigned i = 0; i < limitJson.size(); ++i)
		{
			int mID = limitJson[i][0u].asInt();
			int llm = limitJson[i][1u].asInt();
			vlLimit.push_back(Level_s(mID, llm));
		}
	}

	int playerBase::getCalPopu()
	{
		return population_ + PopulationModule[level_].asInt();
	}

	int playerBase::calMaxLv()
	{
		int cMID = own.Warstory.currentProcess;
		for (unsigned i = 0; i < vlLimit.size(); ++i)
		{
			if(cMID < vlLimit[i].mID)return vlLimit[i].lLimit;
		}
		return (int)playerUpgradeExp.size();
	}

	void GoldAlterHelper(int& first, int& second, const int num)
	{
		first += num;
		if(first  < 0){
			second += first;
			second = second < 0 ? 0 : second;
			first = 0;
		}
	}

	int playerBase::alterBothGold(int num)
	{
		int tmpTicket = goldTicket_;
		int tmpGold = gold_;
// 		if(num < 0)GoldAlterHelper(gold_, goldTicket_, num);
// 		else GoldAlterHelper(goldTicket_, gold_, num);
		GoldAlterHelper(goldTicket_, gold_, num);
		//if(num<0)
		//	own.playerExplore.alterOrdinaryMaterials(-num);
		StreamLog::LogV(mysqlLogGold, own.getOwnDataPtr(), boost::lexical_cast<string, int>(tmpGold), boost::lexical_cast<string, int>(gold_), -1,
			boost::lexical_cast<string, int>(tmpTicket), boost::lexical_cast<string, int>(goldTicket_));
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	int playerBase::alterVipBothGold(int goldNum, int goldTicketNum, string orderID)
	{
		if (goldNum < 0 || goldTicketNum < 0)
			return 0;

		int tmpGold = gold_;
		gold_ += goldNum;
		gold_ = gold_ < 0 ? 0 : gold_;

		int tmpGoldTicket = goldTicket_;
		goldTicket_ += goldTicketNum;
		goldTicket_ = goldTicket_ < 0 ? 0 : goldTicket_;

		StreamLog::LogV(mysqlLogGold, own.getOwnDataPtr(), boost::lexical_cast<string, int>(tmpGold), boost::lexical_cast<string, int>(gold_), -1,
			boost::lexical_cast<string, int>(tmpGoldTicket), boost::lexical_cast<string, int>(goldTicket_), orderID);
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	int playerBase::alterSilver(int num)
	{
		int tmp = silver_;
		silver_ += num;
		silver_ = silver_ < 0 ? 0 : silver_;
		StreamLog::Log(mysqlLogSilver, own, tmp, silver_);
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	int playerBase::alterGold(int num)
	{
		int tmp = gold_;
		gold_ += num;
		gold_ = gold_ < 0 ? 0 : gold_;
		//if(num<0)
		//	own.playerExplore.alterOrdinaryMaterials(-num);
		StreamLog::LogV(mysqlLogGold, own.getOwnDataPtr(), tmp, gold_, -1, boost::lexical_cast<string, int>(goldTicket_), boost::lexical_cast<string, int>(goldTicket_));
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	int playerBase::alterGoldTicket(int num)
	{
		int tmp = goldTicket_;
		goldTicket_ += num;
		goldTicket_ = goldTicket_ < 0 ? 0 : goldTicket_;
		//if(num<0)
		//	own.playerExplore.alterOrdinaryMaterials(-num);
		StreamLog::LogV(mysqlLogGold, own.getOwnDataPtr(), boost::lexical_cast<string, int>(gold_), boost::lexical_cast<string, int>(gold_), -1, boost::lexical_cast<string, int>(tmp)
			, boost::lexical_cast<string, int>(goldTicket_));
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	void playerBase::setPosition(PlayerPosition pos)
	{
		setPosition(pos.first, pos.second);
	}

	enum{
		local_area,
		local_page,
	};
	void playerBase::setPosition(const int aID, const int aPage)
	{
		StreamLog::Log(mysqlLogPlayerLocal, own, areaID_, aID, local_area);
		areaID_ = aID;
		StreamLog::Log(mysqlLogPlayerLocal, own, areaPage_, aPage, local_page);
		areaPage_ = aPage;
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerBase::alterPopulation(int num)
	{
		int tmp = population_;
		population_ += num;
		population_ = population_ < 0 ? 0 : population_;
		const int maxPopu = PopulationModule[level_].asInt();
		if(maxPopu > 0)
		{
			population_ = population_ > maxPopu ? maxPopu : population_;
		}
		StreamLog::Log(mysqlLogPopulation, own, tmp, population_);
		world_sys.asyncPlayerData(own.getOwnDataPtr());
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

// 	int playerBase::alterLoyalty(int num)
// 	{
// 		loyalty_ += num;
// 		loyalty_ = loyalty_ < 0 ? 0 : loyalty_;
// 		helper_mgr.insertSaveSet(this);
// 		return 0;
// 	}

	int playerBase::setSphere(int ID)
	{
		if(ID < 0 || ID > 2)return -1;
		int tmp = sphereID_;
		sphereID_ = ID;
		if(tmp >= 0 && tmp <= 2)kingdom_sys.untickKingdom(tmp);
		kingdom_sys.tickKingdom(ID);
		StreamLog::Log(mysqlLogPlayerKindom, own, tmp, sphereID_);
		upgradeEvent(false);
		own.Trade.Position = 1000 * ID;//设置贸易的初始位置
		own.Trade.Destination = own.Trade.Position();
		helper_mgr.insertSaveAndUpdate(this);

		int curOffLevel = own.Office.getOffcialLevel();//处理武将激活
		for (int i = 1; i <= curOffLevel; i++)
		{
			officeItemConfig off = office_sys.getOfficeItem(i);
			int pilotID = off.addCanRecruitGeneralRawId[sphereID_];
			if (pilotID > 0 && !own.Pilots.checkPilotActive(pilotID))
			{
				own.Pilots.activePilot(pilotID);
			}
		}

		Json::Value bCast;
		bCast[msgStr][0u] = 0;
		bCast[msgStr][1u][senderChannelStr] =chat_kingdom;
		bCast[msgStr][1u][chatBroadcastID] = BROADCAST::join_sphere_event;
		const static string ParamListStr = "pl";
		bCast[msgStr][1u][ParamListStr].append(name_);
		bCast[msgStr][1u][ParamListStr].append(sphereID_);
		player_mgr.sendToSphere(gate_client::chat_broadcast_resp, bCast, sphereID_);
		return 0;
	}

	int playerBase::alterKeJi(int num)
	{
		int tmp = keji_;
		keji_ += num;
		keji_ = keji_ < 0 ? 0 : keji_;
		StreamLog::Log(mysqlLogKeJi, own, tmp, keji_);
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	int playerBase::alterWeiWang(int num)
	{
		int tmp = weiwang_;
		weiwang_ += num;
		weiwang_ = weiwang_ < 0 ? 0 : weiwang_;
		StreamLog::Log(mysqlLogWeiWang, own, tmp, weiwang_, 0);
		int beforeLevel = own.Office.getOffcialLevel();
		while (own.Office.canPromote(weiwang_))
		{
			officeItemConfig off = office_sys.getOfficeItem(own.Office.getOffcialLevel()+1);
			weiwang_ -= off.requireWeiWang;
			own.Office.setOffcialLevel(own.Office.getOffcialLevel()+1);
			if (sphereID_ != -1 && off.addCanRecruitGeneralRawId[sphereID_] > 0)
			{
				own.Pilots.activePilot(off.addCanRecruitGeneralRawId[sphereID_]);
			}
			StreamLog::Log(mysqlLogWeiWang, own, tmp, weiwang_, 1);
		}
		int afterLevel = own.Office.getOffcialLevel();
		if (beforeLevel != afterLevel)
		{
			vector<string> fds;
			fds.push_back(boost::lexical_cast<string, int>(weiwang_));
			StreamLog::Log(office::strMysqlLogOffice, own, 
				boost::lexical_cast<string, int>(beforeLevel), boost::lexical_cast<string, int>(afterLevel), fds);
		}
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	void playerBase::setDefaultFace()
	{
		if (gender == playerDefine::male)
		{
			faceID_ = -1;
		}
		else
		{
			faceID_ = -2;
		}
		world_sys.asyncPlayerData(own.getOwnDataPtr());
		guild_sys.asyncPlayerData(own.getOwnDataPtr());
		arena_sys.updatePlayerInfo(own.getOwnDataPtr(), _arena_alter_face_id);
		ruler_sys.updatePlayerInfo(own.getOwnDataPtr(), _ruler_alter_face_id);
		ally_sys.postToUpdateInfo(own.getOwnDataPtr());
		helper_mgr.insertSaveAndUpdate(this);
	}


	int playerBase::setFaceID(int faceID)
	{
		if (
				(faceID >= 100000 && own.PlayerSecretaries.inSidList(faceID - 100000)) ||
				(faceID >= 0 && own.Pilots.checkPilotActive(faceID)) ||
				(faceID == -1) ||
				(faceID == -2)
			)
		{
			faceID_ = faceID;
			world_sys.asyncPlayerData(own.getOwnDataPtr());
			guild_sys.asyncPlayerData(own.getOwnDataPtr());
			arena_sys.updatePlayerInfo(own.getOwnDataPtr(), _arena_alter_face_id);
			ruler_sys.updatePlayerInfo(own.getOwnDataPtr(), _ruler_alter_face_id);
			ally_sys.postToUpdateInfo(own.getOwnDataPtr());
			helper_mgr.insertSaveAndUpdate(this);
			return 0;
		}
		return 1;
	}

	int playerBase::setName(string name, const bool is_gm /* = false */)
	{
		string tmpName = name_;
		if (tmpName.empty())createRoleTime_ = na::time_helper::get_current_time();
		else if (!is_gm)++setNameTimes;
		name_ = name;
		StreamLog::Log(mysqlLogPlayerName, own, tmpName, name_);
		if(tmpName != name_){
			player_mgr.removeOldRelate(tmpName);
			resp_sys.asyncSystemPlayerData(own.getOwnDataPtr());
			player_mgr.asyncPlayerData(own.getOwnDataPtr());
			Json::Value updateAccount;
			updateAccount[msgStr][0u] = name_;
			string strSend = updateAccount.toStyledString();
			na::msg::msg_json mj(own.playerID, service::process_id::ACCOUNT_NET_ID, gate_client::player_notic_account_name_resp, strSend);
			game_svr->async_send_to_gate(mj);
			arena_sys.updatePlayerInfo(own.getOwnDataPtr(), _arena_alter_name);
			ruler_sys.updatePlayerInfo(own.getOwnDataPtr(), _ruler_alter_name);
			ally_sys.postToUpdateInfo(own.getOwnDataPtr());
			own.Trade.updateRichpName(name_);
		}
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	int playerBase::addExp(int num)
	{
		if(num < 1)return -1;
		int tmp = exp_;
		int tmpLv = level_;
		exp_ += num;
		StreamLog::Log(mysqlLogRoleExp, own, tmp, exp_);
		upgrade();
		if(tmpLv != level_)
		{
			StreamLog::Log(mysqlLogRoleLevel, own, tmpLv, level_);
			resp_sys.asyncSystemPlayerData(own.getOwnDataPtr());
			embattle_sys.activeEMbattle(own.getOwnDataPtr());
			int num = level_ - tmpLv;
			if(num > 0)
			{
				own.Campaign.alterJunling(num);
			}
			arena_sys.updatePlayerInfo(own.getOwnDataPtr(), _arena_alter_lv);
			ruler_sys.updatePlayerInfo(own.getOwnDataPtr(), _ruler_alter_lv);
			ally_sys.postToUpdateInfo(own.getOwnDataPtr());
			own.PlayerSecretaries.openFm();
		}
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	void playerBase::setPlayerProcess(unsigned process)
	{
		StreamLog::Log(mysqlLogClientProcess, own, playerProcess_, process);
		playerProcess_ = process;
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerBase::setGender(int type)
	{
		if(!(type == playerDefine::male || type == playerDefine::female))return -1;
		StreamLog::Log(mysqlLogPlayerGender, own, gender, type);
		gender = type;
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	void playerBase::tickLogin()
	{
		if (season_sys.getSeason(lastLoginTime_) != season_sys.getSeason() ||
			(na::time_helper::get_current_time() > lastLoginTime_ && na::time_helper::get_current_time() - lastLoginTime_ > na::time_helper::ONEDAY))
		{
			own.firstLogin = true;
		}
		lastLoginTime_ = na::time_helper::get_current_time();
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerBase::autoUpdate()
	{
		update();
	}

	int playerBase::getInvest()
	{
		own.TickMgr.Tick();
		return invest_;
	}

	int playerBase::getShared()
	{
		own.TickMgr.Tick();
		return shared_;
	}

	int playerBase::alterInvest(const int num)
	{
		invest_ += num;
		invest_ = invest_ < 0 ? 0 : invest_;
		invest_ = invest_ > 3 ? 3 : invest_;
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	int playerBase::alterShared(const int num)
	{
		shared_ += num;
		shared_ = shared_ < 0 ? 0 : shared_;
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	void playerBase::update()
	{
		if(!isVaild())return;
		Json::Value msg;
		package(msg);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::player_role_update_resp, msg);
	}

	void playerBase::upgrade()
	{
		if(level_ < 1)return;
		int maxLv = calMaxLv();
		for (unsigned i = (unsigned)level_; i < playerUpgradeExp.size(); ++i)
		{
			int tmp = exp_ - playerUpgradeExp[i];
			if(tmp < 0)break;
			if(level_ >= maxLv)break;
			mine_res_sys.levelUp(own.shared_from_this());
			++level_;
			upgradeEvent();
			exp_ = tmp;
		}
		if((sphereID_ < 0 || sphereID_ > 2) && level_ > 29)
		{
			level_ = 30;
			exp_ = 0;
		}
		if(level_ >= maxLv)
		{
//			level_ = maxLv;
			exp_ = 0;
		}
	}

	void playerBase::upgradeEvent(bool pay /* = true */)
	{
		playerDataPtr player = own.getOwnDataPtr();
//		science_sys.playerUpdateEventTrigger(d);
		int level = getLevel();
		if(level == 40)own.Pilots.activeEnlistMode();
		world_sys.checkMoving(player);
		//event_sys.openPlayerEvent(player);
		task_sys.updateBranchTaskInfo(player, _task_role_lv);
		//更新邀请人信息
		own.inviter.updateInviterPeopleNumOfMatchInfo();
		//低级补偿
		int limitPay = levelRank_sys.serverLevel();
		if (pay && level < limitPay)
		{
			double module = season_sys.serverModule();
			if (module > 0.0)
			{
				int sil = int(PlayerPay[0][level_].asInt() * module);
				int keji = int(PlayerPay[1][level_].asInt() * module);
				int jl = PlayerPay[2][level_].asInt();

				alterSilver(sil);
				alterKeJi(keji);
				own.Campaign.alterJunling(jl);

				Json::Value updateJson;
				updateJson[msgStr][0u] = 0;
				Json::Value& dataJson = updateJson[msgStr][1u];
				dataJson = Json::arrayValue;
				dataJson.append(sil);
				dataJson.append(keji);
				dataJson.append(jl);
				own.sendToClient(gate_client::player_pay_resp, updateJson);
			}
		}
		levelRank_sys.insertData(own.playerID, level_);
		//////////////////////////////////////////////////////////////////////////


	}


	void playerBase::package(Json::Value& pack)
	{
		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();
		pack[msgStr][1u][playerIDStr] = own.playerID;
		pack[msgStr][1u][playerFaceIDStr] = faceID_;
		pack[msgStr][1u][playerSilverStr] = silver_;
		pack[msgStr][1u][playerGoldTicketStr] = goldTicket_;
		pack[msgStr][1u][playerGoldStr] = gold_;
		pack[msgStr][1u][playerLevelStr] = level_;
		pack[msgStr][1u][playerExpStr] = exp_;
		pack[msgStr][1u][playerNameStr] = name_;
		pack[msgStr][1u][playerPopulationStr] = getCalPopu();
		pack[msgStr][1u][playerBoomStr] = boom_;
		pack[msgStr][1u][playerInvestStr] = getInvest();
		pack[msgStr][1u][playerSharedStr] = getShared();
		pack[msgStr][1u][guildDonateStr] = own.Guild.getCDonate();
		pack[msgStr][1u][guildSDonateStr] = own.Guild.getSDonate();
		pack[msgStr][1u][playerSphereIDStr] = sphereID_;
		pack[msgStr][1u][playerKejiStr] = keji_;//科技点 军工
		pack[msgStr][1u][playerWeiWangStr] = weiwang_;
		pack[msgStr][1u][playerAreaIDStr] = areaID_;
		pack[msgStr][1u][playerAreaPageStr] = areaPage_;
		pack[msgStr][1u][playerWarStoryCurrentProcessStr] = own.Warstory.currentProcess;
		pack[msgStr][1u][playerSetNameCostStr] = getSetNameCost();
		pack[msgStr][1u][playerProcessStr] = playerProcess_;
		pack[msgStr][1u][playerGenderStr] = gender;
		pack[msgStr][1u][arenaRankStr] = own.Arena.getRank();
		pack[msgStr][1u][playerPlanetIDStr] = planetID;
	}

	int playerBase::getSetNameCost()
	{
		return (setNameTimes < 1 ? 300 : ((setNameTimes - 1) * 1000 + 20000));
	}

	bool playerBase::save(){
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = BSON(playerIDStr << own.playerID << playerSilverStr << silver_ << playerFaceIDStr << faceID_ << 
			playerGoldTicketStr << goldTicket_ << playerGoldStr << gold_ << playerLevelStr << level_ << playerExpStr << exp_ <<
			playerNameStr << name_ << playerAreaPageStr << areaPage_ << playerAreaIDStr << areaID_ <<
			playerPopulationStr << population_ << playerBoomStr << boom_ << /*playerLoyaltyStr << loyalty_ <<*/
			playerInvestStr << invest_ << playerSharedStr << shared_ << 
			playerSphereIDStr << sphereID_ << playerKejiStr << keji_ << playerWeiWangStr << weiwang_ <<
			playerSetNameCostStr << setNameTimes <<
			playerProcessStr << playerProcess_ << playerGenderStr << gender << 
			playerCreateRoleTimeStr << createRoleTime_ << playerLastLoginTimeStr << lastLoginTime_);
		return db_mgr.save_mongo(playerBaseDBStr, key, obj);
	}

	bool playerBase::get(){
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(playerBaseDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[playerFaceIDStr]){faceID_ = obj[playerFaceIDStr].Int();}
			checkNotEoo(obj[playerSilverStr]){silver_ = obj[playerSilverStr].Int();}
			checkNotEoo(obj[playerGoldTicketStr]){goldTicket_ = obj[playerGoldTicketStr].Int();}
			checkNotEoo(obj[playerGoldStr]){gold_ = obj[playerGoldStr].Int();}
			checkNotEoo(obj[playerLevelStr]){level_ = obj[playerLevelStr].Int();}
			checkNotEoo(obj[playerExpStr]){exp_ = obj[playerExpStr].Int();}
			checkNotEoo(obj[playerNameStr]){name_ = obj[playerNameStr].String();}
			checkNotEoo(obj[playerAreaIDStr]){areaID_ = obj[playerAreaIDStr].Int();}
			checkNotEoo(obj[playerAreaPageStr]){areaPage_ = obj[playerAreaPageStr].Int();}		
			checkNotEoo(obj[playerPopulationStr]){population_ = obj[playerPopulationStr].Int();}	
			checkNotEoo(obj[playerBoomStr]){boom_ = obj[playerBoomStr].Int();}	
			checkNotEoo(obj[playerSphereIDStr]){sphereID_ = obj[playerSphereIDStr].Int();}
			checkNotEoo(obj[playerInvestStr]){invest_ = obj[playerInvestStr].Int();}
			checkNotEoo(obj[playerSharedStr]){shared_ = obj[playerSharedStr].Int();}
			checkNotEoo(obj[playerKejiStr]){keji_ = obj[playerKejiStr].Int();}
			checkNotEoo(obj[playerWeiWangStr]){weiwang_ = obj[playerWeiWangStr].Int();}	
			checkNotEoo(obj[playerCreateRoleTimeStr]){createRoleTime_ = obj[playerCreateRoleTimeStr].Int();}
			checkNotEoo(obj[playerLastLoginTimeStr]){ lastLoginTime_ = (unsigned)obj[playerLastLoginTimeStr].Int(); }
			checkNotEoo(obj[playerSetNameCostStr]){setNameTimes = obj[playerSetNameCostStr].Int();}
			checkNotEoo(obj[playerProcessStr]){playerProcess_ = (unsigned)obj[playerProcessStr].Int();}
			checkNotEoo(obj[playerGenderStr]){gender = obj[playerGenderStr].Int();}
			planetID = world_sys.getPlayerPlanetID(own.getOwnDataPtr());
			return true;
		}				
		return false;
	}
}