#include "world_boss_system.h"
#include "response_def.h"
#include "playerManager.h"
#include "chat_system.h"
#include "season_system.h"
#include "battle_def.h"
#include "one_to_one.h"
#include "email_system.h"
#include "guild_system.h"
#include "arena_system.h"
#include "record_system.h"

namespace gg
{
using namespace world_boss_def;
#define PERSONAL_RANK_SIZE 20
#define BOSS_HOUR 21
#define BOSS_BEGIN_MINUTE 5 //修改这个时间可能会对joinPlayerList_的更新以及loopUpdate有影响，因此要同时修改
#define BOSS_END_MINUTE 25
#define TOTAL_REPORT_NUM 20
#define MY_REPORT_NUM 10
#define MAX_NORMAL_INSPIRE_LAYER 30
#define MAX_GUILD_INSPIRE_LAYER 20
#define SEASON2INDEX(SEASON) (SEASON&1)
#define SAVE_TOTAL_EVERY_TIME
#define STORAGE_BASE 20
#define STORAGE_EXTRA 10
#define STORAGE_SIZE 14

enum 
{
	KEY_PERSONAL_RANK = 1,
	KEY_TOTAL_REPORT,
	KEY_COUNTRY_RANK,
	KEY_BOSS_DATA,
	KEY_STORAGE_CAPACITY
};

	world_boss_system* const world_boss_system::worldBossSys = new world_boss_system();

	static fighterDataPtr fdata = fighterDataPtr();

#define AsyncGetRankListData(m) \
	bool isfind = false; \
	for (unsigned i = 0; i < personalDamageRank_.size(); i++) \
		{ \
		personalDamageItem &pItem = personalDamageRank_[i]; \
		playerDataPtr player = player_mgr.getPlayerMain(pItem.playerID_); \
		if (player == playerDataPtr()) \
			isfind = true; \
		} \
	for (unsigned i = 0; i < storageCapacityList_.size(); i++) \
	{ \
		storyCapacityItem &sItem = storageCapacityList_[i]; \
		playerDataPtr player = player_mgr.getPlayerMain(sItem.playerID_); \
		if (player == playerDataPtr()) \
			isfind = true; \
	} \
	playerDataPtr d = player_mgr.getPlayerMain(m._player_id, m._net_id); \
	if (playerDataPtr() == d) \
	{ \
		isfind = true; \
	} \
	\
	if (isfind) \
	{ \
		if (m._post_times < 1) \
		{ \
			player_mgr.postMessage(m); \
			return; \
		} \
		else \
		{ \
			Return(r, -1); \
		} \
	} \
	
	changeItem::changeItem()
		:startTime_(0), endTime_(0), changeNum_(0)
	{
	}

	changeItem::changeItem(int startTime, int endTime, int changeNum)
		:startTime_(startTime), endTime_(endTime), changeNum_(changeNum)
	{
	}

	bossDynamicData::bossDynamicData()
	{
		bossID_ = -1;
		remainHP_ = -1;
		beginTime_ = 0;
		killTime_ = -1;
		damageState_ = damage_zhengchang;
		killTailName_ = "";
	}

	void bossDynamicData::reset()
	{
		bossID_ = -1;
		remainHP_ = -1;
		beginTime_ = 0;
		killTime_ = -1;
		damageState_ = damage_zhengchang;
		killTailName_ = "";
	}

	void bossDynamicData::calBuffState(int &buff, double &rewardRate, int &buffTime)
	{
		int passTime = na::time_helper::get_current_time() - beginTime_;
		buff = buff_null;
		rewardRate = 0.0;
		buffTime = 0;
		if (passTime < 0)
		{
		}
		if (passTime >= 0 && passTime <= 300)
		{
			buff = buff_xianfazhiren;
			rewardRate = 1;
			buffTime = beginTime_ + 300;
		}
		else if (passTime <= 600)
		{
			buff = buff_wenzhawenda;
			rewardRate = 0.6;
			buffTime = beginTime_ + 600;
		}
		else if (passTime <= 900)
		{
			buff = buff_yuweiyouzai;
			rewardRate = 0.4;
			buffTime = beginTime_ + 900;
		}
		else
		{
			buff = buff_null;
			rewardRate = 0.2;
			buffTime = 0;
		}
	}

	worldBossConfig::worldBossConfig()
	{//数值混乱。。
		kejiInspireSucessProb_ = 0.8;
		goldInspireSucessProb_ = 1;
		kejiInspireCost_ = 100;
	}

	void worldBossConfig::init()
	{
		changeRule_.ChangeLose_ = -5;
		changeRule_.ChangeMax_ = 51;

		changeItem ci1(0, 3, 5);
		changeItem ci2(3, 5, 3);
		changeItem ci3(5, 10, 1);
		changeItem ci4(10, 15, -1);
		changeItem ci5(15, 20, -3);
		changeItem ci6(20, 22, -5);
		changeRule_.changeWin_.push_back(ci1);
		changeRule_.changeWin_.push_back(ci2);
		changeRule_.changeWin_.push_back(ci3);
		changeRule_.changeWin_.push_back(ci4);
		changeRule_.changeWin_.push_back(ci5);
		changeRule_.changeWin_.push_back(ci6);

		Json::Value bossConfigJson = na::file_system::load_jsonfile_val(strWorldBossConfigFile);
		attackCD_ = bossConfigJson["attackCd"].asInt();
		killRewardSilver_ = bossConfigJson["killRewardSilver"].asInt();

		clearAttackCDCostList_.clear();
		for (unsigned i = 0; i < bossConfigJson["clearAttackCdCost"].size(); i++)
			clearAttackCDCostList_.push_back(bossConfigJson["clearAttackCdCost"][i].asInt());

		if (bossConfigJson["worldRankRewardSilver"].size() != bossConfigJson["worldRankRewardCrystal"].size())
		{
			LogE << "worldRankReward size of silver and crystal not the same!" << LogEnd;
			throw exception();
		}
		unsigned personalRewardSize = bossConfigJson["worldRankRewardSilver"].size();
		personalRankRewardList_.clear();
		for (unsigned i = 0; i < personalRewardSize; i++)
		{
			bossRewardItem br;
			br.rewardSilver_ = bossConfigJson["worldRankRewardSilver"][i].asInt();
			br.rewardCrystal_ = bossConfigJson["worldRankRewardCrystal"][i].asInt();
			personalRankRewardList_.push_back(br);
		}

		countryRankRewardSilverList_.clear();
		unsigned countryRankRewardSize = bossConfigJson["countryRankRewardSilver"].size();
		if (countryRankRewardSize != 3)
		{
			LogE << "countryRankRewardSilver size error!" << LogEnd;
			throw exception();
		}
		for (unsigned i = 0; i < countryRankRewardSize; i++)
			countryRankRewardSilverList_.push_back(bossConfigJson["countryRankRewardSilver"][i].asInt());

	}

	personalDamageItem::personalDamageItem()
	{
		playerID_ = 0;
		playerName_ = "";
		damage_ = 0;
	}

	countryDamageItem::countryDamageItem()
	{
		countryID_ = 0;
		damage_ = 0;
	}

	world_boss_system::world_boss_system()
		:addRobot_(false)
	{
	}

	world_boss_system::~world_boss_system()
	{
	}

	void world_boss_system::initData()
	{
		bossData_.resize(2);
		bossConfig_.init();
		curSeason_ = season_sys.getSeason();
		isBossInBattle_ = false;
		totalReportLink_ = 1;
		activityState_ = state_waiting;
		duration_ = 0;
		memset(&annouce_, 0, sizeof(annouce_));

		countryDamageRank_.clear();
		for (unsigned i = 0; i < 3; i++)
		{
			countryDamageItem cItem;
			cItem.countryID_ = i;
			countryDamageRank_.push_back(cItem);
		}

		db_mgr.ensure_index(strDbWorldBoss, BSON(strKey << 1));
		db_mgr.ensure_index(strDbPlayerWorldBoss, BSON(playerIDStr << 1));

		initNpc();
		get();
	}

	void world_boss_system::initNpc()
	{
		vector<na::file_system::json_value_vec> jv;
		jv.resize(2);
		na::file_system::load_jsonfiles_from_dir(strWorldBoss1thDir, jv[0]);
		na::file_system::load_jsonfiles_from_dir(strWorldBoss2thDir, jv[1]);

		for (unsigned idxDir = 0; idxDir < jv.size(); idxDir++)
		{//和矿战配置文件基本一样，多了个maxHP字段
			na::file_system::json_value_vec jfile = jv[idxDir];
			map< int, mapBossDataPtr> bossNpc;
			for (unsigned idxFile = 0; idxFile < jfile.size(); idxFile++)
			{
				Json::Value& mapDataJson = jfile[idxFile];
				mapBossDataPtr mData = create();
				mData->mapName = mapDataJson[WAR::warStoryNameStr].asString();
				mData->localID = mapDataJson[WAR::warStoryLocalIDStr].asInt();
				int mapID = mData->localID / mapIDOffset;
				mData->faceID = mapDataJson[WAR::warStoryLeaderGeneralRawIdStr].asInt();
				mData->mapLevel = mapDataJson[WAR::warStoryLevelStr].asInt();
				mData->mapValue = mapDataJson[WAR::warStoryValueStr].asInt();
				mData->frontLocalID = mapDataJson[WAR::warStoryFrontIDStr].asInt();
				mData->playerExp = mapDataJson[WAR::warStoryPlayerExpStr].asInt();
				mData->playerExp = mData->playerExp < 0 ? 0 : mData->playerExp;
				mData->pilotExp = mapDataJson[WAR::warStoryPilotExpStr].asInt();
				mData->pilotExp = mData->pilotExp < 0 ? 0 : mData->pilotExp;
				Json::Value& army_data = mapDataJson[WAR::warStoryArmyDataStr];
				mData->npcArmy = war_story.formatNpcArmy(army_data);
				mData->actionVec = war_story.formationAction(mapDataJson, 0);

				//boss
				mData->maxHP_ = mapDataJson[strMaxHP].asInt();

				bossNpc[mData->localID] = mData;
			}
			bossConfig_.npcList_.push_back(bossNpc);
		}
	}

	void world_boss_system::worldBossLoopUpdate(boost::system_time& tmp)
	{
		if ((tmp - tick_).total_seconds() < duration_)return;

		tick_ = tmp;
		duration_ = 20;

		unsigned cur_time = na::time_helper::get_current_time();
		boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
		tm t = boost::posix_time::to_tm(p);

		if (!updateClientMgr_.isJoinPlayerListClear_ && t.tm_hour == BOSS_HOUR && t.tm_min == BOSS_BEGIN_MINUTE - 2)
		{
			updateClientMgr_.isJoinPlayerListClear_ = true;
			updateClientMgr_.joinPlayerList_.clear();
		}

		if (t.tm_hour == BOSS_HOUR && t.tm_min == BOSS_BEGIN_MINUTE - 1 && t.tm_sec > 30
			|| t.tm_hour == BOSS_HOUR && t.tm_min == BOSS_END_MINUTE - 1 && t.tm_sec > 30)
		{
			duration_ = 1;
		}

		if (activityState_ != state_ing && activityState_ != state_kill_not_end 
			&& t.tm_hour == BOSS_HOUR && t.tm_min >= BOSS_BEGIN_MINUTE && t.tm_min < BOSS_END_MINUTE)
		{
			activityState_ = state_ing;
			bossBegin();
		}

		if (activityState_ != state_waiting && activityState_ != state_not_start
			&& (t.tm_hour == BOSS_HOUR && t.tm_min >= BOSS_END_MINUTE || t.tm_hour > BOSS_HOUR))
		{
			activityState_ = state_waiting;
			int curSeason = season_sys.getSeason();
			bossDynamicData & bossData = getBossData(curSeason);
			if (bossData.remainHP_ > 0)
			{
				bossEnd();
			}
		}

		annouce_.refreshData();
		if (!annouce_.ba_20_35_ && t.tm_hour == 20 && t.tm_min == 35)
		{
			annouce_.ba_20_35_ = true;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_will_come_20_35);
		}
		if (!annouce_.ba_21_00_ && t.tm_hour == 21 && t.tm_min == 0)
		{
			annouce_.ba_21_00_ = true;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_will_come_21_00);

			timePeriod tp;
			tp.startFromZeroTime_ = 21 * 3600 + 5;
			tp.endFromZeroTime_ = tp.startFromZeroTime_ + 60*3;
			vector<timePeriod> vecPeriod;
			vecPeriod.push_back(tp);
			chat_sys.addSystemRollBroadCast(ROLLBROADCAST::roll_world_boss_21_00_05, 120, vecPeriod);
		}
		if (!annouce_.ba_21_04_55_ && t.tm_hour == 21 && t.tm_min == 4 && t.tm_sec >= 55)
		{
			annouce_.ba_21_04_55_ = true;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_will_come_21_04_55);

			timePeriod tp;
			tp.startFromZeroTime_ = 21 * 3600 + 4 * 60 + 57;
			tp.endFromZeroTime_ = tp.startFromZeroTime_ + 600;
			vector<timePeriod> vecPeriod;
			vecPeriod.push_back(tp);
			chat_sys.addSystemRollBroadCast(ROLLBROADCAST::roll_world_boss_21_04_55, 120, vecPeriod);
		}

		if (addRobot_)
		{
			string s;
			na::msg::msg_json mj(gate_client::world_boss_inner_auto_attack_req, s);
			player_mgr.postMessage(mj);
		}
	}

	void world_boss_system::bossBegin()
	{
		unsigned curTime = na::time_helper::get_current_time();
		unsigned standardTime = na::time_helper::get_time_zero(curTime);
		int bossBeginTime = standardTime + BOSS_HOUR * 3600 + BOSS_BEGIN_MINUTE * 60;
		int curSeason = season_sys.getSeason();
		bossDynamicData & bossData = getBossData(curSeason);
		if (bossData.bossID_ != -1 && bossData.beginTime_ == bossBeginTime)
		{
			curSeason_ = curSeason;
			mapBossDataPtr bossPtr = getBossNpcConfig(curSeason, bossData.bossID_);
			mapDataPtr mapdata = (mapDataPtr)bossPtr;
			bossSide_ = battleSide::Create(mapdata);
			divideBlood(bossData, true);
			for (set<playerDataPtr>::iterator it = updateClientMgr_.joinPlayerList_.begin(); it != updateClientMgr_.joinPlayerList_.end(); it++)
			{
				(*it)->WorldBoss.refreshData();
			}
			updateInfoToClients(true);
			return;
		}

		int maxBossID = getMaxBossID(curSeason);
 		int bossID = bossData.bossID_;
		bossID = calNextBossID(curSeason);

		bossData.reset();
		bossData.bossID_ = bossID;
		bossData.beginTime_ = bossBeginTime;
		bossData.damageState_ = damage_zhengchang;
		mapBossDataPtr bossPtr = getBossNpcConfig(curSeason, bossID);
		bossData.remainHP_ = bossPtr->maxHP_;

		curSeason_ = curSeason;
		personalDamageRank_.clear();
		totalReportList_.clear();

		countryDamageRank_.clear();
		for (unsigned i = 0; i < 3; i++)
		{
			countryDamageItem cItem;
			cItem.countryID_ = i;
			countryDamageRank_.push_back(cItem);
		}

		mapDataPtr mapdata = (mapDataPtr)bossPtr;
		bossSide_ = battleSide::Create(mapdata);
		divideBlood(bossData, true);

		save();
		for (set<playerDataPtr>::iterator it = updateClientMgr_.joinPlayerList_.begin(); it != updateClientMgr_.joinPlayerList_.end(); it++)
		{
			(*it)->WorldBoss.refreshData();
		}
		updateInfoToClients(true);
	}

	int world_boss_system::calNextBossID(int sea)
	{
		int maxBossID = getMaxBossID(sea);
		bossDynamicData & bossData = getBossData(sea);
		int bossID = bossData.bossID_;
		unsigned curTime = na::time_helper::get_current_time();
		unsigned standardTime = na::time_helper::get_time_zero(curTime);
		if (bossData.bossID_ == -1)
		{//未参战
			int serverOpenTime = season_sys.getServerOpenTime();
			bossID = (curTime - serverOpenTime) / (86400 * 4) * 2 + 10;
			if (bossID > bossConfig_.changeRule_.ChangeMax_)
				bossID = bossConfig_.changeRule_.ChangeMax_;
		}
		else
		{
			if (bossData.remainHP_ == 0)
			{//胜利
				int passTime = bossData.killTime_ - bossData.beginTime_;
				if (passTime < 0 || passTime >= 20*60)
					passTime = 21 * 60;
				bool isChange = false;
				for (unsigned i = 0; i < bossConfig_.changeRule_.changeWin_.size(); i++)
				{
					changeItem &ci = bossConfig_.changeRule_.changeWin_[i];
					if (ci.startTime_ * 60 < passTime && passTime <= ci.endTime_ * 60)
					{
						bossID = bossData.bossID_ + ci.changeNum_;
						break;
					}
				}
			}
			else
			{//失败
				bossID = bossData.bossID_ + bossConfig_.changeRule_.ChangeLose_;
			}
		}

		if (bossID > maxBossID)
			bossID = maxBossID;
		if (bossID < 1)
			bossID = 1;

		return bossID;
	}

	void world_boss_system::bossEnd()
	{
		string s = "";
		na::msg::msg_json m(gate_client::world_boss_inner_deal_end_req, s);
		player_mgr.postMessage(m);
	}

	void world_boss_system::worldBossUpdateClientReq(msg_json& m, Json::Value& r)
	{
		AsyncGetRankListData(m);

		if (d->Base.getLevel() < WORLD_BOSS_OPEN_LEVEL || d->Base.getSphereID() == -1)
			Return(r, -1);

		updateClientMgr_.joinPlayerList_.insert(d);
		if (m._type == gate_client::world_boss_update_client_req)
		{
			updateInfoSingleClient(d);
		}
		else
		{
			updateInfoSingleClient(d, gate_client::world_boss_manual_update_client_resp);
		}
	}

	void world_boss_system::worldBossAttackReq(msg_json& m, Json::Value& r)
	{
		AsyncGetRankListData(m);

		if (d->Base.getLevel() < WORLD_BOSS_OPEN_LEVEL || d->Base.getSphereID() == -1)
			Return(r, -1);
		d->WorldBoss.refreshData();
		if (activityState_ != state_ing)
		{
			if (activityState_ == state_not_start)
				Return(r, 1);
			if (activityState_ == state_kill_not_end)
				Return(r, 2);
			if (activityState_ == state_waiting)
				Return(r, 3);
		}

		int curTime = na::time_helper::get_current_time();
		if (curTime < d->WorldBoss.battleCD_)
			Return(r, 4);

		if (d->Embattle.isNullFormation())
			Return(r, 5);

		bossDynamicData & bossData = getBossData(curSeason_);
		if (bossData.remainHP_ <= 0)
			Return(r, 2);

		if (d->isOperatBattle())
			Return(r, 7);

		{
//			boost::mutex::scoped_lock lock(queueMutex_);
			if (isBossInBattle_)
			{
//				LogE << __FUNCTION__ << ",push " << m._player_id << LogEnd;
				bossMsgQueue_.push(m);
				return;
			}
		}

		Battlefield field;
		bSideReCall f;
		f = BindSideReCall(world_boss_system::worldBossSys, world_boss_system::addBattleBuff);
		divideBlood(bossData, false);
		field.atkSide = battleSide::Create(d, f);
		field.defSide = bossSide_;
		if (field.atkSide == battleSidePtr() || field.defSide == battleSidePtr())
			Return(r, -1);
		
		if (d->WorldBoss.latestBossID_ != bossData.bossID_)
			d->WorldBoss.latestBossID_ = bossData.bossID_;
		if (d->WorldBoss.latestBossBeginTime_ != bossData.beginTime_)
			d->WorldBoss.latestBossBeginTime_ = bossData.beginTime_;
		d->WorldBoss.curReportLink_++;
		if (d->WorldBoss.curReportLink_ > MY_REPORT_NUM)
		{
			d->WorldBoss.curReportLink_ = 1;
		}
		field.atkSide->setFileName(boost::lexical_cast<string, int>(d->WorldBoss.curReportLink_));
		isBossInBattle_ = true;
		d->WorldBoss.save();

		battle_sys.PostBattle(field, battle_world_boss_type);
	}

	void world_boss_system::worldBossAttackCallback(const int battleResult, Battlefield field, Json::Value &report)
	{
		State::setState(gate_client::world_boss_attack_req);
		NumberCounter::Step();
		int curHP = 0;
		armyVec bossMems = bossSide_->getMember(MO::mem_null);
		for (unsigned i = 0; i < bossMems.size(); i++)
			curHP += bossMems[i]->getHP();

		playerDataPtr player = player_mgr.getPlayerMain(field.atkSide->getPlayerID(), field.atkSide->netID);
		bossDynamicData & bossData = getBossData(curSeason_);
		mapBossDataPtr bossPtr = getBossNpcConfig(curSeason_, bossData.bossID_);
		int damage = bossData.remainHP_ - curHP;
		bossData.remainHP_ = curHP;
		damage = damage > 0 ? damage : 0;
		int st = damageRate2state(bossPtr->maxHP_, damage);
		if (st > bossData.damageState_)
		{
			bossData.damageState_ = st;
			Json::Value chatContent;
			chatContent[playerSphereIDStr] = player->Base.getSphereID();
			chatContent[playerNameStr] = field.atkSide->getName();
			chatContent[strDamageState] = st;
			chatContent["rt"] = int(state2DamageRate(st)*100);
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_damage_state,chatContent);
		}

		bossRewardItem roundReward = calRoundDamageReward(bossData, bossPtr, player, damage);
		player->WorldBoss.damageReward_.rewardSilver_ += roundReward.rewardSilver_;
		player->WorldBoss.damageReward_.rewardCrystal_ += roundReward.rewardCrystal_;

		Json::Value reportRet;
		reportRet.append(curSeason_);
		reportRet.append(bossData.bossID_);
		reportRet.append(damage);
		Json::Value rwsl, rwcr, rw = Json::arrayValue;
		if (roundReward.rewardSilver_ > 0)
		{
			rwsl[email::actionID] = action_add_silver;
			rwsl[email::value] = roundReward.rewardSilver_;
			rw.append(rwsl);
		}
		if (roundReward.rewardCrystal_ > 0)
		{
			rwcr[email::actionID] = action_add_item;
			rwcr[email::itemID] = 13535;
			rwcr[email::addNum] = roundReward.rewardCrystal_;
			rw.append(rwcr);
		}
		reportRet.append(rw);
		report[BATTLE::strBattleReward] = reportRet;

		reportItem rItem;
		rItem.atkName_ = field.atkSide->getName();
		rItem.damage_ = damage;
		rItem.reportLink_ = getReportLinkWithoutIncrease();
		totalReportList_.insert(totalReportList_.begin(), rItem);
		if (totalReportList_.size() > TOTAL_REPORT_NUM)
		{
			totalReportList_.pop_back();
		}

		myReportItem mItem;
		mItem.damage_ = damage;
		mItem.reportLink_ = player->WorldBoss.curReportLink_;
		player->WorldBoss.myReportList_.insert(player->WorldBoss.myReportList_.begin(), mItem);
		if (player->WorldBoss.myReportList_.size() > MY_REPORT_NUM)
		{
			player->WorldBoss.myReportList_.pop_back();
		}
		player->WorldBoss.totalDamage_ += damage;
		player->WorldBoss.battleCD_ = na::time_helper::get_current_time() + bossConfig_.attackCD_;

		personalDamageItem pItem;
		pItem.damage_ = player->WorldBoss.totalDamage_;
		pItem.playerID_ = player->playerID;
		pItem.playerName_ = player->Base.getName();
		insertPersonalRankList(personalDamageRank_, pItem);

		int playerCountry = player->Base.getSphereID();
		countryDamageItem cItem = getItemByCountry(playerCountry);
		cItem.damage_ = cItem.damage_ + damage;
		insertCountryRankList(countryDamageRank_, cItem);

		if (!annouce_.ba_drop_to_70_ && double(bossData.remainHP_) / bossPtr->maxHP_ <= 0.7)
		{
			annouce_.ba_drop_to_70_ = true;
			Json::Value chatContent;
			chatContent["rt"] = 70;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_drop_percent, chatContent);
		}
		if (!annouce_.ba_drop_to_50_ && double(bossData.remainHP_) / bossPtr->maxHP_ <= 0.5)
		{
			annouce_.ba_drop_to_50_ = true;
			Json::Value chatContent;
			chatContent["rt"] = 50;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_drop_percent, chatContent);
		}
		if (!annouce_.ba_drop_to_30_ && double(bossData.remainHP_) / bossPtr->maxHP_ <= 0.3)
		{
			annouce_.ba_drop_to_30_ = true;
			Json::Value chatContent;
			chatContent["rt"] = 30;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_drop_percent, chatContent);
		}
		if (!annouce_.ba_drop_to_20_ && double(bossData.remainHP_) / bossPtr->maxHP_ <= 0.2)
		{
			annouce_.ba_drop_to_20_ = true;
			Json::Value chatContent;
			chatContent["rt"] = 20;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_drop_percent, chatContent);
		}
		if (!annouce_.ba_drop_to_10_ && double(bossData.remainHP_) / bossPtr->maxHP_ <= 0.1)
		{
			annouce_.ba_drop_to_10_ = true;
			Json::Value chatContent;
			chatContent["rt"] = 10;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_drop_percent, chatContent);
		}

		player->WorldBoss.attackTimes_++;
		if (player->WorldBoss.storyCapacityTime_ != 0)
		{
			player->WorldBoss.storyCapacityTime_ = 0;
			updateStorageToClient(player);
		}
		if (curHP <= 0)
		{
			unsigned curTime = na::time_helper::get_current_time();
			bossData.damageState_ = damage_yijitui;
			activityState_ = state_kill_not_end;
			bossData.killTailName_ = player->Base.getName();
			bossData.killTime_ = curTime;
			dealEndThings();
		}
		else
		{
			static int percent20 = int(bossPtr->maxHP_ * 0.2);
			if (bossData.remainHP_ < percent20)
			{
				Json::Value msg;
				msg[msgStr][0u] = 0;
				msg[msgStr][1u] = bossData.remainHP_;
				updateMsgToClients(false, msg, gate_client::world_boss_remain_blood_resp);
			}
			//hp<=0时已经有发
			updateInfoSingleClient(player);
		}

		player->WorldBoss.save();
#ifdef SAVE_TOTAL_EVERY_TIME
		save();
#endif
//		LogE << __FUNCTION__ << ",pid:" << player->playerID << LogEnd;
		_Battle_Post(boost::bind(&battle_system::WorldBossRespon, battle_system::battleSys, battleResult, field, player, report));
	}

	void world_boss_system::worldBossEnding(const int battleResult, Battlefield field, playerDataPtr atkData)
	{
		State::setState(gate_client::world_boss_attack_req);
		NumberCounter::Step();

		string playerReportFile = field.atkSide->getFileName();
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = battleResult;
		msg[msgStr][2u] = playerReportFile;
		player_mgr.sendToPlayer(atkData->playerID, atkData->netID, gate_client::world_boss_attack_resp, msg);

		bossDynamicData & bossData = getBossData(curSeason_);
		if (bossData.remainHP_ == 0)
		{
			Json::Value upmsg;
			upmsg[msgStr][0u] = 0;
			updateMsgToClients(false, upmsg, gate_client::world_boss_notify_ending_resp);
			updateClientMgr_.isJoinPlayerListClear_ = false;
			updateClientMgr_.joinPlayerList_.clear();
		}

//		LogE << __FUNCTION__ << ",pid:" << atkData->playerID << LogEnd;
//		cout << __FUNCTION__ << ",msg:" << msg.toStyledString() << endl;
//		boost::mutex::scoped_lock lock(queueMutex_);
	}

	void world_boss_system::worldBossRewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (activityState_ == state_ing)
			Return(r, 1);

		if (d->WorldBoss.latestBossID_ == -1)
			Return(r, 2);

		if (d->Base.getLevel() < WORLD_BOSS_OPEN_LEVEL || d->Base.getSphereID() == -1)
			Return(r, -1);

//		LogE << __FUNCTION__ << ",pid:" << d->playerID << LogEnd;
		d->WorldBoss.refreshData();
		Json::Value rewardJson;
		Json::Value &personalReward = rewardJson[strPersonalReward];
		int personalRank = getPersonalRank(d);
		bossRewardItem pr = calPersonalRankReward(d, personalRank);
		personalReward.append(personalRank);
		personalReward.append(d->WorldBoss.totalDamage_);
		personalReward.append(pr.toJson());
		personalReward.append(d->WorldBoss.damageReward_.toJson());

		Json::Value &countryReward = rewardJson[strCountryReward];
		int spID = d->Base.getSphereID();
		int countryRank = getCountryRank(spID);
		countryDamageItem cItem = getItemByCountry(spID);
		countryReward.append(countryRank); 
		countryReward.append(cItem.damage_);
		countryReward.append(calCountryRankReward(countryRank).toJson());

		Json::Value &killReward = rewardJson[strKillReward];
		bossDynamicData &bossData = getBossData(getLatestSeason());
		bool isKill = (bossData.remainHP_ == 0);
		killReward.append(isKill);
		killReward.append(calKillReward(isKill).toJson());
		killReward.append(calTailReward(d->Base.getName()).toJson());

		r[msgStr][0u] = 0;
		r[msgStr][1u] = rewardJson;
	}

	void world_boss_system::worldBossKejiInspireReq(msg_json& m, Json::Value& r)
	{
		AsyncGetRankListData(m);

		if (d->Base.getLevel() < WORLD_BOSS_OPEN_LEVEL || d->Base.getSphereID() == -1 || activityState_ != state_ing)
			Return(r, -1);
		if (activityState_ != state_ing)
			Return(r, 1);
		int kejiInspireCost = calKejiInspireCost(d);
		if (d->Base.getKeJi() < kejiInspireCost)
			Return(r, 2);
		if (d->WorldBoss.normalInspireLayer_ >= MAX_NORMAL_INSPIRE_LAYER)
			Return(r, 3);
		if (!commom_sys.randomOk(bossConfig_.kejiInspireSucessProb_))
			Return(r, 4);

		d->WorldBoss.refreshData();

		d->WorldBoss.normalInspireLayer_++;
		d->Base.alterKeJi(-kejiInspireCost);

		r[msgStr][0u] = 0;
		string s = r.toStyledString();
		na::msg::msg_json mj(d->playerID, d->netID, gate_client::world_boss_keji_inspire_resp, s);
		player_mgr.sendToPlayer(mj);
		updateInfoSingleClient(d);

		d->WorldBoss.save();
		r = Json::Value::null;
	}

	void world_boss_system::worldBossGoldInspireReq(msg_json& m, Json::Value& r)
	{
		AsyncGetRankListData(m);

		if (d->Base.getLevel() < WORLD_BOSS_OPEN_LEVEL || d->Base.getSphereID() == -1 || activityState_ != state_ing)
			Return(r, -1);
		if (activityState_ != state_ing)
			Return(r, 1);
		int goldInspireCost = calGoldInspireCost();
		if (d->Base.getAllGold() < goldInspireCost)
			Return(r, 2);
		if (d->WorldBoss.normalInspireLayer_ >= MAX_NORMAL_INSPIRE_LAYER)
			Return(r, 3);
		if (!commom_sys.randomOk(bossConfig_.goldInspireSucessProb_))
			Return(r, 4);

		d->WorldBoss.refreshData();
		d->WorldBoss.normalInspireLayer_++;
		d->Base.alterBothGold(-goldInspireCost);

		r[msgStr][0u] = 0;
		string s = r.toStyledString();
		na::msg::msg_json mj(d->playerID, d->netID, gate_client::world_boss_gold_inspire_resp, s);
		player_mgr.sendToPlayer(mj);
		updateInfoSingleClient(d);

		d->WorldBoss.save();
		r = Json::Value::null;
	}

	void world_boss_system::worldBossClearAttackCdReq(msg_json& m, Json::Value& r)
	{
		AsyncGetRankListData(m);

		if (d->Base.getLevel() < WORLD_BOSS_OPEN_LEVEL || d->Base.getSphereID() == -1 || activityState_ != state_ing)
			Return(r, -1);

		d->WorldBoss.refreshData();
		//钻石数量-10钻石，消耗数量随清除cd次数而变化
		int cost = calClearAttackCdCost(d->WorldBoss.clearAttackCDTimes_);
		if (d->Base.getAllGold() < cost)
			Return(r, 1);
		int curTime = na::time_helper::get_current_time();
		if (curTime >= d->WorldBoss.battleCD_)
			Return(r, 2);

		d->Base.alterBothGold(-cost);
		d->WorldBoss.battleCD_ = 0;
		d->WorldBoss.clearAttackCDTimes_++;

		r[msgStr][0u] = 0;
		string s = r.toStyledString();
		na::msg::msg_json mj(d->playerID, d->netID, gate_client::world_boss_clear_attack_cd_resp, s);
		player_mgr.sendToPlayer(mj);
		updateInfoSingleClient(d);

		d->WorldBoss.save();
		r = Json::Value::null;
	}

	void world_boss_system::worldBossCloseUiReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		set<playerDataPtr>::iterator it = find(updateClientMgr_.joinPlayerList_.begin(), updateClientMgr_.joinPlayerList_.end(), d);
		if (it != updateClientMgr_.joinPlayerList_.end())
			updateClientMgr_.joinPlayerList_.erase(it);

		Return(r, 0);
	}

	void world_boss_system::worldBossInnerDealEndReq(msg_json& m, Json::Value& r)
	{
		bool isfind = false;
		for (unsigned i = 0; i < personalDamageRank_.size(); i++)
		{
			personalDamageItem &pItem = personalDamageRank_[i];
			playerDataPtr player = player_mgr.getPlayerMain(pItem.playerID_);
			if (player == playerDataPtr())
				isfind = true;
		}

		if (isfind)
		{
			if (m._post_times < 2)
			{
				player_mgr.postMessage(m);
				return;
			}
			else
			{
				Return(r, -1);
			}
		}
		
		dealEndThings();
	}

	void world_boss_system::updateStorageToClient(playerDataPtr pdata)
	{
		Json::Value r;
		int storageAdd = calStorageAttack(pdata);
		r[msgStr][0u] = 0;
		r[msgStr][1] = storageAdd;
		r[msgStr][2] = STORAGE_EXTRA;

		string s = r.toStyledString();
		na::msg::msg_json mj(pdata->playerID, pdata->netID, gate_client::world_boss_storage_capacity_update_resp, s);
		player_mgr.sendToPlayer(mj);
	}

	void world_boss_system::worldBossStorageCapacityUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (d->Base.getLevel() < WORLD_BOSS_OPEN_LEVEL || d->Base.getSphereID() == -1)
			Return(r, -1);
		d->WorldBoss.refreshData();
		updateStorageToClient(d);
	}

	void world_boss_system::worldBossStorageCapacityReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (d->Base.getLevel() < WORLD_BOSS_OPEN_LEVEL || d->Base.getSphereID() == -1)
			Return(r, -1);

		unsigned curTime = na::time_helper::get_current_time();
		boost::posix_time::ptime p = boost::posix_time::from_time_t(curTime);
		tm t = boost::posix_time::to_tm(p);
		if (!(t.tm_hour >= 12 && t.tm_hour <= BOSS_HOUR-1 || t.tm_hour == BOSS_HOUR && t.tm_min < BOSS_BEGIN_MINUTE))
			Return(r, 1);

		d->WorldBoss.refreshData();
		if (d->WorldBoss.storyCapacityTime_ != 0)
			Return(r, 2);

		d->WorldBoss.storyCapacityTime_ = curTime;
//		cout << __LINE__ << ", assign storyCapacityTime_:" << d->WorldBoss.storyCapacityTime_ << endl;
		d->WorldBoss.save();

		storyCapacityItem scItem;
		scItem.playerID_ = d->playerID;
		scItem.playerName_ = d->Base.getName();
		bool isfind = false;
		for (unsigned i = 0; i < storageCapacityList_.size(); i++)
		{
			if (storageCapacityList_[i].playerID_ == scItem.playerID_)
			{
				isfind = true;
				break;
			}
		}
		if (!isfind)
		{
			storageCapacityList_.insert(storageCapacityList_.begin(), scItem);
			if (storageCapacityList_.size() > STORAGE_SIZE)
				storageCapacityList_.pop_back();
			save();
		}

		r[msgStr][0u] = 0;
		r[msgStr][1u] = calStorageAttack(d);
		string s = r.toStyledString();
		na::msg::msg_json mj(d->playerID, d->netID, gate_client::world_boss_storage_capacity_resp, s);
		player_mgr.sendToPlayer(mj);
		r = Json::Value::null;

		updateStorageToClient(d);
	}

	void world_boss_system::worldBossInnerAutoAttackReq(msg_json& m, Json::Value& r)
	{
		int attackNum = 10;
		if (!addRobot_)
			return;
//		cout << __FUNCTION__ ",addRobot" << endl;
		vector<int>arenaRankList = arena_sys.getArenaRankList(1, attackNum);
		if (arenaRankList.size() < 2)
			return;

		bool isfind = false;
		for (unsigned i = 0; i < arenaRankList.size(); i++)
		{
			playerDataPtr player = player_mgr.getPlayerMain(arenaRankList[i]);
			if (player == playerDataPtr())
				isfind = true;
		}

		if (isfind)
		{
			if (m._post_times < 1)
			{
				player_mgr.postMessage(m);
				return;
			}
			else
			{
				Return(r, -1);
			}
		}

		Json::Value msg;
		msg[msgStr][0u] = 0;
		string s = msg.toStyledString();
		na::msg::msg_json mj(gate_client::world_boss_attack_req, s);
		unsigned curTime = na::time_helper::get_current_time();
		for (unsigned i = 0; i < arenaRankList.size(); i++)
		{
			playerDataPtr player = player_mgr.getPlayerMain(arenaRankList[i]);
			if (int(curTime) < player->WorldBoss.battleCD_)
				continue;
			mj._player_id = arenaRankList[i];
			mj._post_times = -1;
			player_mgr.postMessage(mj);
		}
	}

	bool world_boss_system::save()
	{
		{
			mongo::BSONObjBuilder objPersonal;
			mongo::BSONObj key = BSON(strKey << 1);
			objPersonal << strKey << 1;
			mongo::BSONArrayBuilder arrPersonal;
			for (unsigned i = 0; i < personalDamageRank_.size(); i++)
			{
				personalDamageItem pItem = personalDamageRank_[i];
				mongo::BSONObjBuilder singlePerson;
				singlePerson << playerIDStr << pItem.playerID_;
				singlePerson << playerNameStr << pItem.playerName_;
				singlePerson << strDamage << pItem.damage_;
				arrPersonal << singlePerson.obj();
			}
			objPersonal << strPersonalRankList << arrPersonal.arr();
			db_mgr.save_mongo(strDbWorldBoss, key, objPersonal.obj());
		}

		{
			mongo::BSONObjBuilder objReport;
			mongo::BSONObj key = BSON(strKey << 2);
			objReport << strKey << 2;
			mongo::BSONArrayBuilder arrReport;
			for (unsigned i = 0; i < totalReportList_.size(); i++)
			{
				reportItem rItem = totalReportList_[i];
				mongo::BSONObjBuilder singleReport;
				singleReport << strAtkName << rItem.atkName_;
				singleReport << strDamage << rItem.damage_;
				singleReport << strReport << rItem.reportLink_;
				arrReport << singleReport.obj();
			}
			objReport << strTotalReportList << arrReport.arr();
			db_mgr.save_mongo(strDbWorldBoss, key, objReport.obj());
		}

		{
			mongo::BSONObjBuilder objCountry;
			mongo::BSONObj key = BSON(strKey << 3);
			objCountry << strKey << 3;

			mongo::BSONArrayBuilder arrCountry;
			for (unsigned i = 0; i < countryDamageRank_.size(); i++)
			{
				countryDamageItem cItem = countryDamageRank_[i];
				mongo::BSONObjBuilder singleCountry;
				singleCountry << playerSphereIDStr << cItem.countryID_;
				singleCountry << strDamage << cItem.damage_;
				arrCountry << singleCountry.obj();
			}
			objCountry << strCountryRankList << arrCountry.arr();
			db_mgr.save_mongo(strDbWorldBoss, key, objCountry.obj());
		}

		{
			mongo::BSONObjBuilder objBoss;
			mongo::BSONObj key = BSON(strKey << 4);
			objBoss << strKey << 4;

			mongo::BSONArrayBuilder arrBoss;
			for (unsigned i = 0; i < bossData_.size(); i++)
			{
				bossDynamicData bossData = bossData_[i];
				mongo::BSONObjBuilder singleBoss;
				singleBoss << strBossID << bossData.bossID_;
				singleBoss << strBeginTime << bossData.beginTime_;
				singleBoss << strKillTime << bossData.killTime_;
				singleBoss << strRemainHP << bossData.remainHP_;
				singleBoss << strKillTailName << bossData.killTailName_;
				singleBoss << strDamageState << bossData.damageState_;
				arrBoss << singleBoss.obj();
			}
			objBoss << strBossList << arrBoss.arr();
			db_mgr.save_mongo(strDbWorldBoss, key, objBoss.obj());
		}

		{
			mongo::BSONObjBuilder objCapacity;
			mongo::BSONObj key = BSON(strKey << 5);
			objCapacity << strKey << 5;

			mongo::BSONArrayBuilder arrCapacity;
			for (unsigned i = 0; i < storageCapacityList_.size(); i++)
			{
				storyCapacityItem scItem = storageCapacityList_[i];
				mongo::BSONObjBuilder singleStorage;
				singleStorage << playerIDStr << scItem.playerID_;
				singleStorage << playerNameStr << scItem.playerName_;
				arrCapacity << singleStorage.obj();
			}
			objCapacity << strStorageCapacityList << arrCapacity.arr();
			db_mgr.save_mongo(strDbWorldBoss, key, objCapacity.obj());
		}

		return true;
	}

	bool world_boss_system::get()
	{
		objCollection objs = db_mgr.Query(strDbWorldBoss);
		if (objs.size() == 0)
			return false;
		mongo::BSONObj objPersonal, objReport, objCountry, objBoss, objStorage;
		for (unsigned i = 0; i < objs.size(); i++)
		{
			mongo::BSONObj obj = objs[i];
			if (!obj.isEmpty())
			{
				checkNotEoo(obj[strKey])
				{
					int ikey = obj[strKey].Int();
					if (ikey == KEY_PERSONAL_RANK)
						objPersonal = obj;
					else if (ikey == KEY_TOTAL_REPORT)
						objReport = obj;
					else if (ikey == KEY_COUNTRY_RANK)
						objCountry = obj;
					else if (ikey == KEY_BOSS_DATA)
						objBoss = obj;
					else if (ikey == KEY_STORAGE_CAPACITY)
						objStorage = obj;
					else
					{
					}
				}
			}
		}

		if (!objPersonal.isEmpty())
		{
			checkNotEoo(objPersonal[strPersonalRankList])
			{
				personalDamageRank_.clear();
				vector<mongo::BSONElement> rankList = objPersonal[strPersonalRankList].Array();
				for (unsigned i = 0; i < rankList.size(); i++)
				{
					mongo::BSONObj rItem = rankList[i].Obj();
					personalDamageItem pItem;
					checkNotEoo(rItem[playerIDStr])
						pItem.playerID_ = rItem[playerIDStr].Int();
					checkNotEoo(rItem[playerNameStr])
						pItem.playerName_ = rItem[playerNameStr].String();
					checkNotEoo(rItem[strDamage])
						pItem.damage_ = rItem[strDamage].Int();
					personalDamageRank_.push_back(pItem);
				}
			}
		}
		if (!objReport.isEmpty())
		{
			checkNotEoo(objReport[strTotalReportList])
			{
				totalReportList_.clear();
				vector<mongo::BSONElement> reportList = objReport[strTotalReportList].Array();
				for (unsigned i = 0; i < reportList.size(); i++)
				{
					mongo::BSONObj rItem = reportList[i].Obj();
					reportItem pItem;
					checkNotEoo(rItem[strAtkName])
						pItem.atkName_ = rItem[strAtkName].String();
					checkNotEoo(rItem[strDamage])
						pItem.damage_ = rItem[strDamage].Int();
					checkNotEoo(rItem[strReport])
						pItem.reportLink_ = rItem[strReport].Int();
					totalReportList_.push_back(pItem);
				}
			}
		}
		if (!objCountry.isEmpty())
		{
			checkNotEoo(objCountry[strCountryRankList])
			{
				countryDamageRank_.clear();
				vector<mongo::BSONElement> countryList = objCountry[strCountryRankList].Array();
				for (unsigned i = 0; i < countryList.size(); i++)
				{
					mongo::BSONObj rItem = countryList[i].Obj();
					countryDamageItem cItem;
					checkNotEoo(rItem[playerSphereIDStr])
						cItem.countryID_ = rItem[playerSphereIDStr].Int();
					checkNotEoo(rItem[strDamage])
						cItem.damage_ = rItem[strDamage].Int();
					countryDamageRank_.push_back(cItem);
				}
			}
		}
		if (!objBoss.isEmpty())
		{
			checkNotEoo(objBoss[strBossList])
			{
				bossData_.clear();
				vector<mongo::BSONElement> bossList = objBoss[strBossList].Array();
				for (unsigned i = 0; i < bossList.size(); i++)
				{
					mongo::BSONObj bItem = bossList[i].Obj();
					bossDynamicData bossData;
					checkNotEoo(bItem[strBossID])
						bossData.bossID_ = bItem[strBossID].Int();
					checkNotEoo(bItem[strBeginTime])
						bossData.beginTime_ = bItem[strBeginTime].Int();
					checkNotEoo(bItem[strKillTime])
						bossData.killTime_ = bItem[strKillTime].Int();
					checkNotEoo(bItem[strRemainHP])
						bossData.remainHP_ = bItem[strRemainHP].Int();
					checkNotEoo(bItem[strKillTailName])
						bossData.killTailName_ = bItem[strKillTailName].String();
					checkNotEoo(bItem[strDamageState])
						bossData.damageState_ = bItem[strDamageState].Int();
					bossData_.push_back(bossData);
				}
			}

			if (bossData_[0].bossID_ == 0 && bossData_[1].bossID_ == 0)
				activityState_ = state_not_start;
		}
		if (!objStorage.isEmpty())
		{
			checkNotEoo(objStorage[strStorageCapacityList])
			{
				storageCapacityList_.clear();
				vector<mongo::BSONElement> storageList = objStorage[strStorageCapacityList].Array();
				for (unsigned i = 0; i < storageList.size(); i++)
				{
					mongo::BSONObj sItem = storageList[i].Obj();
					storyCapacityItem scItem;
					checkNotEoo(sItem[playerIDStr])
						scItem.playerID_ = sItem[playerIDStr].Int();
					checkNotEoo(sItem[playerNameStr])
						scItem.playerName_ = sItem[playerNameStr].String();
					storageCapacityList_.push_back(scItem);
				}
			}
		}

		return false;
	}

	void world_boss_system::updateInfoSingleClient(playerDataPtr pdata, int updateType, bool updateNextBoss/* = false*/)
	{
		int latestSeason = getLatestSeason();
		bossDynamicData &bossData = getBossData(latestSeason);
		Json::Value updateJson;
		pdata->WorldBoss.refreshData();
		//个人伤害榜
		Json::Value &personalRank = updateJson[world_boss_def::strPersonalRankList];
		personalRank = Json::arrayValue;
		for (unsigned i = 0; i < personalDamageRank_.size(); i++)
		{
			personalDamageItem &pdItem = personalDamageRank_[i];
			if (i >= PERSONAL_RANK_SIZE)
				break;
			Json::Value singlePlayer;
			singlePlayer.append(pdItem.playerID_);
			singlePlayer.append(pdItem.playerName_);
			singlePlayer.append(pdItem.damage_);
			playerDataPtr player = player_mgr.getPlayerMain(pdItem.playerID_);
			if (player != playerDataPtr())
			{
				vector<int> pilotList = player->Embattle.getCurrentFormatPilot();
				if (pilotList.size() > 0)
				{
					vector<int> validList;
					for (unsigned j = 0; j < pilotList.size(); j++)
					{
						if (pilotList[j] > 0)
						{
							validList.push_back(pilotList[j]);
						}
					}
					if (validList.size() > 0)
					{
						int idx = commom_sys.randomBetween(0, validList.size() - 1);
						singlePlayer.append(validList[idx]);
					}
					else
					{
						singlePlayer.append(0);
					}
				}
				else
				{
					singlePlayer.append(0);
				}
			}
			else
			{
				singlePlayer.append(0);
			}
			personalRank.append(singlePlayer);
		}

		Json::Value &storageList = updateJson[world_boss_def::strStorageCapacityList];
		storageList = Json::arrayValue;

		int personalRankSize = personalDamageRank_.size();
		for (unsigned j = 0; j < storageCapacityList_.size(); j++)
		{
			if (storageList.size() + personalRankSize >= STORAGE_SIZE-1)
				break;

			bool isfind = false;
			storyCapacityItem scItem = storageCapacityList_[j];
			if (scItem.playerID_ == pdata->playerID)
				continue;

			for (unsigned k = 0; k < personalDamageRank_.size(); k++)
			{
				personalDamageItem pdItem = personalDamageRank_[k];
				if (scItem.playerID_ == pdItem.playerID_)
				{
					isfind = true;
					break;
				}
			}
			if (isfind)
			{
				continue;
			}

			Json::Value singleItem;
			singleItem.append(scItem.playerID_);
			singleItem.append(scItem.playerName_);
			playerDataPtr player = player_mgr.getPlayerMain(scItem.playerID_);
			if (player != playerDataPtr())
			{
				vector<int> pilotList = player->Embattle.getCurrentFormatPilot();
				if (pilotList.size() > 0)
				{
					vector<int> validList;
					for (unsigned j = 0; j < pilotList.size(); j++)
					{
						if (pilotList[j] > 0)
						{
							validList.push_back(pilotList[j]);
						}
					}
					if (validList.size() > 0)
					{
						int idx = commom_sys.randomBetween(0, validList.size() - 1);
						singleItem.append(validList[idx]);
					}
					else
					{
						singleItem.append(0);
					}
				}
				else
				{
					singleItem.append(0);
				}
			}
			else
			{
				singleItem.append(0);
			}
			storageList.append(singleItem);
		}

		//国家伤害榜
		Json::Value &contryRank = updateJson[world_boss_def::strCountryRankList];
		contryRank = Json::arrayValue;
		for (unsigned i = 0; i < countryDamageRank_.size(); i++)
		{
			countryDamageItem &cdItem = countryDamageRank_[i];
			Json::Value singleCountry;
			singleCountry.append(cdItem.countryID_);
			singleCountry.append(cdItem.damage_);
			contryRank.append(singleCountry);
		}

		//总战况
		Json::Value &totalReportList = updateJson[world_boss_def::strTotalReportList];
		totalReportList = Json::arrayValue;
		for (unsigned i = 0; i < totalReportList_.size(); i++)
		{
			reportItem &rItem = totalReportList_[i];
			Json::Value singleReport;
			singleReport.append(rItem.atkName_);
			singleReport.append(rItem.damage_);
			singleReport.append(rItem.reportLink_);
			totalReportList.append(singleReport);
		}

		//我的战况
		Json::Value &myReportList = updateJson[world_boss_def::strMyReportList];
		myReportList = Json::arrayValue;
		for (unsigned i = 0; i < pdata->WorldBoss.myReportList_.size(); i++)
		{
			myReportItem &rItem = pdata->WorldBoss.myReportList_[i];
			Json::Value singleReport;
			singleReport.append(rItem.damage_);
			singleReport.append(rItem.reportLink_);
			myReportList.append(singleReport);
		}

		//鼓舞层数
		updateJson[world_boss_def::strNormalInspire] = pdata->WorldBoss.normalInspireLayer_;
		updateJson[world_boss_def::strGuildInspire] = pdata->WorldBoss.guildInspireLayer_;

		//战斗cd
		updateJson[world_boss_def::strBattleCD] = pdata->WorldBoss.battleCD_;

		//boss剩余血量，收益状态，伤害状态，玩家排名		
		int updateBossID = bossData.bossID_;
		int updateSeason = latestSeason;
		int updateRemainHP = bossData.remainHP_;
		int updateDamageState = bossData.damageState_;
		if (activityState_ == state_not_start || activityState_ == state_waiting)
		{
			updateSeason = getNextSeason();
			updateBossID = calNextBossID(updateSeason);
			mapBossDataPtr nextBossPtr = getBossNpcConfig(updateSeason, updateBossID);
			if (nextBossPtr != mapBossDataPtr())
			{
				updateRemainHP = nextBossPtr->maxHP_;
			}
			updateDamageState = damage_zhengchang;
		}
		updateJson[world_boss_def::strBossID] = updateBossID;
		updateJson[world_boss_def::strRemainHP] = updateRemainHP;
		string strLatestTotalHP = "lthp";
		mapBossDataPtr bossPtr = getBossNpcConfig(latestSeason, bossData.bossID_);
		if (bossPtr != mapBossDataPtr())
		{
			updateJson[strLatestTotalHP] = bossPtr->maxHP_;
		}
		else
		{
			updateJson[strLatestTotalHP] = 0;
		}
		updateJson[world_boss_def::strSeason] = latestSeason;
		updateJson[world_boss_def::strNextSeason] = updateSeason;
		updateJson[world_boss_def::strClearAttackCDCost] = calClearAttackCdCost(pdata->WorldBoss.clearAttackCDTimes_);
		updateJson[world_boss_def::strKejiInspireCost] = calKejiInspireCost(pdata);
		updateJson[world_boss_def::strGoldInspireCost] = calGoldInspireCost();
		updateJson[world_boss_def::strDamageState] = updateDamageState;
		updateJson[world_boss_def::strHasSeekBossHelp] = pdata->WorldBoss.hasSeekBossHelp_;
		updateJson[world_boss_def::strHasStorageCapacity] = (pdata->WorldBoss.storyCapacityTime_ != 0);
		int buffSt = buff_null;
		double rewardRate = 0.0;
		int buffTime = 0;
		bossData.calBuffState(buffSt, rewardRate, buffTime);
		updateJson[world_boss_def::strBuffState].append(buffSt);
		updateJson[world_boss_def::strBuffState].append(int(rewardRate+0.000001)*500);
		updateJson[world_boss_def::strBuffState].append(buffTime);
		updateJson[world_boss_def::strActivityState] = activityState_;
		updateJson[world_boss_def::strMyRank] = getPersonalRank(pdata);
		updateJson[world_boss_def::strMyDamage] = pdata->WorldBoss.totalDamage_;

		Json::Value r;
		r[msgStr][0u] = 0;
		r[msgStr][1] = updateJson;
		string s = r.toStyledString();
		na::msg::msg_json mj(pdata->playerID, pdata->netID, updateType, s);
//		cout << __FUNCTION__ << ",mj:" << s << endl;
		player_mgr.sendToPlayer(mj);
	}

	bossDynamicData& world_boss_system::getBossData(int curSeason)
	{
		return bossData_[SEASON2INDEX(curSeason)];
	}

	const bossDynamicData& world_boss_system::getLatestBoss()
	{
		int ss = getLatestSeason();
		return bossData_[SEASON2INDEX(ss)];
	}

	mapBossDataPtr world_boss_system::getBossNpcConfig(int curSeason, int bossID)
	{
		map<int, mapBossDataPtr> &bossList = bossConfig_.npcList_[SEASON2INDEX(curSeason)];
		map<int, mapBossDataPtr>::iterator it = bossList.find(bossID);
		if (it != bossList.end())
			return it->second;

		return mapBossDataPtr();
	}

	int world_boss_system::getLatestSeason()
	{
		int curSeason = season_sys.getSeason();
		unsigned curTime = na::time_helper::get_current_time();
		boost::posix_time::ptime p = boost::posix_time::from_time_t(curTime);
		tm t = boost::posix_time::to_tm(p);

		if ((t.tm_hour == BOSS_HOUR && t.tm_min >= BOSS_BEGIN_MINUTE) || t.tm_hour > BOSS_HOUR || t.tm_hour < 5)
			return curSeason;
		else
			return (curSeason + 3) % 4;
	}

	int world_boss_system::getNextSeason()
	{
		int curSeason = season_sys.getSeason();
		unsigned curTime = na::time_helper::get_current_time();
		boost::posix_time::ptime p = boost::posix_time::from_time_t(curTime);
		tm t = boost::posix_time::to_tm(p);

		if ((t.tm_hour == BOSS_HOUR && t.tm_min < BOSS_END_MINUTE)
			|| activityState_ == state_ing
			|| activityState_ == state_kill_not_end
			|| (t.tm_hour < BOSS_HOUR && t.tm_hour >= 5))
			return curSeason;
		else
			return (curSeason + 1) % 4;
	}

	int world_boss_system::getMaxBossID(int curSeason)
	{
		int max_id = 0;

		map<int, mapBossDataPtr> &bossList = bossConfig_.npcList_[SEASON2INDEX(curSeason)];
		for (map<int, mapBossDataPtr>::iterator it = bossList.begin(); it != bossList.end(); it++)
		{
			if (max_id < it->second->localID)
			{
				max_id = it->second->localID;
			}
		}

		return max_id;
	}

	void world_boss_system::divideBlood(bossDynamicData& bossData, bool isInit)
	{
		int curSoldierNumber = bossData.remainHP_;
		armyVec bossMems = bossSide_->getMember(MO::mem_null);
		size_t troopSize = bossMems.size();

		if (troopSize == 1)
		{
			if (isInit)
			{
				bossMems[0]->setAddChar(idx_hp, curSoldierNumber);
			}
			bossMems[0]->setHP(curSoldierNumber);
		}
		else
		{
			int averageSoldierNum = curSoldierNumber / troopSize;
			int firstTroopNum = curSoldierNumber - averageSoldierNum * (troopSize - 1);

			for (size_t tidx = 1; tidx < troopSize; tidx++)
			{
				if (isInit)
				{
					bossMems[tidx]->setAddChar(idx_hp, averageSoldierNum);
				}
				bossMems[tidx]->setHP(averageSoldierNum);
			}

			if (isInit)
			{
				bossMems[0]->setAddChar(idx_hp, firstTroopNum);
			}
			bossMems[0]->setHP(firstTroopNum);
		}

		{
			int curHP = 0;
			armyVec bossMems = bossSide_->getMember(MO::mem_null);
			for (unsigned i = 0; i < bossMems.size(); i++)
				curHP += bossMems[i]->getHP();
			cout << __FUNCTION__ << ",curHP:" << curHP << endl;
		}

	}

	void world_boss_system::addBattleBuff(playerDataPtr pdata, battleSide& side)
	{
		bossDynamicData & bossData = getBossData(curSeason_);
		armyVec vec = side.getMember(MO::mem_null);
		int totalInspireLayer = pdata->WorldBoss.guildInspireLayer_ + pdata->WorldBoss.normalInspireLayer_;
		double stateDamage = state2DamageRate(bossData.damageState_);
		int storageAdd = calStorageAttack(pdata);
		for (armyVec::iterator it = vec.begin(); it != vec.end(); it++)
		{
			(*it)->addCharacter[idx_phyAddHurt] += (totalInspireLayer * 0.01 + stateDamage + storageAdd * 0.01);
			(*it)->addCharacter[idx_warAddHurt] += (totalInspireLayer * 0.01 + stateDamage + storageAdd * 0.01);
			(*it)->addCharacter[idx_magicAddHurt] += (totalInspireLayer * 0.01 + stateDamage + storageAdd * 0.01);
		}
	}

	double world_boss_system::state2DamageRate(int st)
	{
		double stateDamage = 0.0;
		switch (st)
		{
		case damage_buan:
			stateDamage = 0.01;
			break;
		case damage_dongyao:
			stateDamage = 0.02;
			break;
		case damage_weidi:
			stateDamage = 0.03;
			break;
		case damage_sangdan:
			stateDamage = 0.04;
			break;
		case damage_hunluan:
			stateDamage = 0.06;
			break;
		case damage_qijia:
			stateDamage = 0.08;
			break;
		case damage_kuisan:
			stateDamage = 0.1;
			break;
		default:
			break;
		}
		return stateDamage;
	}

	int world_boss_system::damageRate2state(int totalHP, int damage)
	{
		int st = damage_zhengchang;
		double rate = damage*1.0 / totalHP;
		
		if (rate >= 0.02)
			st = damage_kuisan;
		else if (rate >= 0.01)
			st = damage_qijia;
		else if (rate >= 0.0075)
			st = damage_hunluan;
		else if (rate >= 0.005)
			st = damage_sangdan;
		else if (rate >= 0.004)
			st = damage_weidi;
		else if (rate >= 0.003)
			st = damage_dongyao;
		else if (rate >= 0.002)
			st = damage_buan;
		else
		{
		}

		return st;
	}

	bossRewardItem world_boss_system::calRoundDamageReward(bossDynamicData& bossData, mapBossDataPtr bossPtr, playerDataPtr pdata, int damage)
	{
		//buff图标计算
		int buff = 0;
		double rewardRate = 0.0;
		int buffTime = 0;
		bossData.calBuffState(buff, rewardRate, buffTime);

// 		if  X <= 50000
// 			X = int[[(玩家等级 ^ 1 / 2)*(打掉BOSS的血量 / BOSS的总血量) * 300000 + (BOSS等级 * 200) + 3000] * 时间段加成]
// 
// 		else
// 		X = 50000 + int[(X - 50000)*0.1]

		int X;
		X = int((pow(pdata->Base.getLevel(), 0.5)*((double)damage / bossPtr->maxHP_) * 300000 + (bossPtr->mapLevel * 200) + 3000) * rewardRate);

		if (X > 50000)
			X = 50000 + int((X - 50000)*0.1);

		bossRewardItem bItem;
		bItem.rewardSilver_ = X;
		return bItem;
	}

	int world_boss_system::getReportLinkAndIncrease()
	{
		int tmp = totalReportLink_;
		totalReportLink_++;
		if (totalReportLink_ > TOTAL_REPORT_NUM)
			totalReportLink_ = 1;

		return tmp;
	}

	int world_boss_system::getReportLinkWithoutIncrease()
	{
		return totalReportLink_;
	}

	int binarySearch(vector<personalDamageItem> parr, int key, int &low, int &high)
	{
		int aSize = parr.size();
		if (aSize == 0)
			return -1;
		low = 0;
		high = aSize - 1;
		int mid = 0;

		while (low <= high)
		{
			mid = (low + high) / 2;

			if (parr[mid].damage_ < key)
				high = mid - 1;
			else if (parr[mid].damage_ > key)
				low = mid + 1;
			else
				return mid;
		}
		return -1;
	}

	int world_boss_system::getPersonalRank(playerDataPtr player)
	{
		int low, high;
		int ret = binarySearch(personalDamageRank_, player->WorldBoss.totalDamage_, low, high);
		if (ret == -1)
			return ret;
		for (int i = low; i <= high; i++)
		{
			if (personalDamageRank_[i].playerID_ == player->playerID)
				return i;
		}
		return -1;
	}

	int world_boss_system::getCountryRank(int country)
	{
		if (country == -1)
			return -1;
		for (unsigned i = 0; i < countryDamageRank_.size(); i++)
		{
			if (countryDamageRank_[i].countryID_ == country)
				return i;
		}
		return -1;
	}

	countryDamageItem world_boss_system::getItemByCountry(int country)
	{
		for (unsigned i = 0; i < countryDamageRank_.size(); i++)
		{
			if (countryDamageRank_[i].countryID_ == country)
				return countryDamageRank_[i];
		}

		return countryDamageItem();
	}

	bool world_boss_system::insertPersonalRankList(vector<personalDamageItem> &plist, personalDamageItem &pinfo)
	{
		int playerID = pinfo.playerID_;

		for (unsigned i = 0; i < plist.size(); ++i)
		{
			if (playerID == plist[i].playerID_)
			{
				plist.erase(plist.begin() + i);
				break;
			}
		}

		if (plist.empty())
		{
			plist.push_back(pinfo);
			return true;
		}

		if (pinfo.damage_ <= plist.back().damage_)
		{
			plist.push_back(pinfo);
			return true;
		}

		for (unsigned i = 0; i < plist.size(); ++i)
		{
			if (pinfo.damage_ > plist[i].damage_)
			{
				plist.insert(plist.begin() + i, pinfo);
				return true;
			}
		}

		return false;
	}

	bool world_boss_system::insertCountryRankList(vector<countryDamageItem> &plist, countryDamageItem &pinfo)
	{
		int  countryID = pinfo.countryID_;

		for (unsigned i = 0; i < plist.size(); ++i)
		{
			if (countryID == plist[i].countryID_)
			{
				plist.erase(plist.begin() + i);
				break;
			}
		}

		if (plist.empty())
		{
			plist.push_back(pinfo);
			return true;
		}

		if (pinfo.damage_ <= plist.back().damage_)
		{
			plist.push_back(pinfo);
			return true;
		}

		for (unsigned i = 0; i < plist.size(); ++i)
		{
			if (pinfo.damage_ > plist[i].damage_)
			{
				plist.insert(plist.begin() + i, pinfo);
				return true;
			}
		}

		return false;
	}

	bossRewardItem world_boss_system::calPersonalRankReward(playerDataPtr player, int rank)
	{//0开始
		if (rank < 0 || rank >= int(bossConfig_.personalRankRewardList_.size()))
			return bossRewardItem();

		return bossConfig_.personalRankRewardList_[rank];
	}

	bossRewardItem world_boss_system::calCountryRankReward(int rank)
	{//0开始
		if (rank < 0 || rank >= int(bossConfig_.countryRankRewardSilverList_.size()))
			return bossRewardItem();

		bossRewardItem br;
		br.rewardSilver_ = bossConfig_.countryRankRewardSilverList_[rank];
		return br;
	}

	bossRewardItem world_boss_system::calKillReward(bool isKill)
	{
		if (!isKill)
			return bossRewardItem();

		bossRewardItem br;
		br.rewardSilver_ = bossConfig_.killRewardSilver_;
		return br;
	}

	bossRewardItem world_boss_system::calTailReward(string name)
	{
		int latestSeason = getLatestSeason();
		bossDynamicData bossData = getBossData(latestSeason);
		if (bossData.killTailName_ != name)
			return bossRewardItem();

		if (bossData.bossID_ <= 0)
			return bossRewardItem();

		mapBossDataPtr bossPtr = getBossNpcConfig(latestSeason, bossData.bossID_);
		bossRewardItem br;
//		br.rewardSilver_ = (personalDamageRank_.size() + bossPtr->mapLevel) * 1000;
		br.rewardSilver_ = 300000 + personalDamageRank_.size() * 3000;

		return br;
	}

	int world_boss_system::calClearAttackCdCost(int times)
	{
		if (times < 0)
			return 999999;
		if (times >= int(bossConfig_.clearAttackCDCostList_.size())-1)
			return bossConfig_.clearAttackCDCostList_[bossConfig_.clearAttackCDCostList_.size() - 1];
		return bossConfig_.clearAttackCDCostList_[times];
	}

	void world_boss_system::dealEndThings()
	{
		short pushType = State::getState();
		State::setState(gate_client::world_boss_inner_deal_end_req);
		bossDynamicData &bossData = getBossData(curSeason_);
		mapBossDataPtr bossPtr = getBossNpcConfig(curSeason_, bossData.bossID_);
		bool isKill = (bossData.remainHP_ == 0);

		if (isKill)
		{
			unsigned curTime = na::time_helper::get_current_time();
			int passTime = curTime - bossData.beginTime_;
			int passMinute = (int)ceil((double)passTime / 60);
			if (passMinute <= 0)
			{
			}
			else if (passMinute <= 10)
			{
				chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_end_a1);
			}
			else if (passMinute < 20 + 1)
			{
				chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_end_a2);
			}
			else
			{
			}

// 			{//b公告
// 				Json::Value chatContent;
// 				chatContent[strMaxHP] = bossPtr->maxHP_;
// 				for (unsigned i = 0; i < countryDamageRank_.size(); i++)
// 				{
// 					countryDamageItem &cItem = countryDamageRank_[i];
// 					Json::Value singleCountry;
// 					singleCountry.append(cItem.countryID_);
// 					singleCountry.append(cItem.damage_);
// 					singleCountry.append(calCountryRankReward(i).toJson());
// 					chatContent[strCountryRankList].append(singleCountry);
// 				}
// 				for (unsigned i = 0; i < personalDamageRank_.size(); i++)
// 				{
// 					if (i >= 3) //只发3条
// 						break;
// 					personalDamageItem & pItem = personalDamageRank_[i];
// 					Json::Value singlePerson;
// 					singlePerson.append(pItem.playerName_);
// 					singlePerson.append(pItem.damage_);
// 					playerDataPtr player = player_mgr.getPlayerMain(pItem.playerID_);
// 					singlePerson.append(calPersonalRankReward(player, i).toJson());
// 					chatContent[strPersonalRankList].append(singlePerson);
// 				}
// 				Json::Value tailJson;
// 				tailJson.append(bossData.killTailName_);
// 				tailJson.append(calTailReward(bossData.killTailName_).toJson());
// 				chatContent[strKillTailName].append(tailJson);
// 
// 				chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_end_b, chatContent);
// 
// 			}

		}
		else
		{
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_end_a3);
		}

		//b公告
		{
			//前4条
			Json::Value chatContent;
			string strArticalID = "aid";
			string strContent = "ct";
			chatContent[strArticalID] = 1;
			chatContent["is"] = isKill;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_end_b, chatContent);
			int startArtID = 2;
			for (unsigned i = 0; i < countryDamageRank_.size(); i++)
			{
				countryDamageItem &cItem = countryDamageRank_[i];
				chatContent = Json::Value::null;
				chatContent[strArticalID] = i + startArtID;
				Json::Value singleCountry;
				singleCountry.append(bossPtr->maxHP_);
				singleCountry.append(cItem.countryID_);
				singleCountry.append(cItem.damage_);
				singleCountry.append(calCountryRankReward(i).toJson());
				chatContent[strContent].append(singleCountry);
				chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_end_b, chatContent);
			}

			//中4条
			chatContent = Json::Value::null;
			chatContent[strArticalID] = 5;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_end_b, chatContent);
			startArtID = 6;
			for (unsigned i = 0; i < personalDamageRank_.size(); i++)
			{
				if (i >= 3) //只发3条
					break;
				chatContent = Json::Value::null;
				chatContent[strArticalID] = i + startArtID;
				personalDamageItem & pItem = personalDamageRank_[i];
				Json::Value singlePerson;
				singlePerson.append(bossPtr->maxHP_);
				singlePerson.append(pItem.playerName_);
				singlePerson.append(pItem.damage_);
				playerDataPtr player = player_mgr.getPlayerMain(pItem.playerID_);
				singlePerson.append(calPersonalRankReward(player, i).toJson());
				chatContent[strContent].append(singlePerson);
				chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_end_b, chatContent);
			}

			//后1条
			if (isKill)
			{
				chatContent = Json::Value::null;
				chatContent[strArticalID] = 9;
				Json::Value tailJson;
				tailJson.append(bossData.killTailName_);
				tailJson.append(calTailReward(bossData.killTailName_).toJson());
				chatContent[strContent].append(tailJson);
				chat_sys.sendToAllBroadCastCommon(BROADCAST::world_boss_end_b, chatContent);
			}
		}

		map<int, int> fightNum;
		for (unsigned i = 0; i < 3; i++)
			fightNum[i] = 0;
		for (unsigned i = 0; i < personalDamageRank_.size(); i++)
		{
			personalDamageItem & pItem = personalDamageRank_[i];
			playerDataPtr player = player_mgr.getPlayerMain(pItem.playerID_);
			Json::Value emailContent;
			emailContent[strRank] = i + 1;
			emailContent[strDamage] = pItem.damage_;
			bossRewardItem exceptItem = calTotalRewardExceptDamageByPlayer(player, isKill);
			bossRewardItem totalItem;
			totalItem.rewardSilver_ = exceptItem.rewardSilver_ + player->WorldBoss.damageReward_.rewardSilver_;
			totalItem.rewardCrystal_ = exceptItem.rewardCrystal_ + player->WorldBoss.damageReward_.rewardCrystal_;
			//邮件
			emailContent[strReward].append(totalItem.toJson());
			email_sys.sendSystemEmailCommon(player, email_type_system_ordinary, email_team_system_world_boss, emailContent);
			//发奖励
			if (totalItem.rewardSilver_ > 0)
				player->Base.alterSilver(totalItem.rewardSilver_);
			if (totalItem.rewardCrystal_ > 0)
			{
				int crystalID = 13535;
				itemVector vecPtr = player->Item.addItem(crystalID, totalItem.rewardCrystal_);
				if (vecPtr.empty())
				{
					Json::Value attachJson;
					Json::Value attachItem;
					attachItem = Json::Value::null;
					attachItem[email::actionID] = action_add_item;
					attachItem[email::addNum] = totalItem.rewardCrystal_;
					attachItem[email::itemID] = crystalID;
					attachJson.append(attachItem);
					email_sys.sendSystemEmailCommon(player, email_type_system_attachment, email_team_system_bag_full_world_boss, Json::Value::null, attachJson);
				}
			}
			player->Base.alterWeiWang(totalItem.rewardCrystal_);
			fightNum[player->Base.getSphereID()]++;
			guild_sys.removeHelp(player, Guild::GuildHelp::world_boss);
		}

		StreamLog::Log(strWorldBossLog, 0, "", fightNum[0], fightNum[1], fightNum[2], -1); //暂时不要word_boss_fighter_number

		save();
		
		if (!isKill)
		{
			Json::Value msg;
			msg[msgStr][0u] = 0;
			updateMsgToClients(false, msg, gate_client::world_boss_notify_ending_resp);
			updateClientMgr_.isJoinPlayerListClear_ = false;
			updateClientMgr_.joinPlayerList_.clear();
		}
		updateInfoToClients(true);
		memset(&annouce_, 0, sizeof(annouce_));
		State::setState(pushType);
	}

	gg::bossRewardItem world_boss_system::calTotalRewardExceptDamageByPlayer(playerDataPtr player, bool isKill)
	{
		int countryRank = getCountryRank(player->Base.getSphereID());
		bossRewardItem countryRW = calCountryRankReward(countryRank);
		int personalRank = getPersonalRank(player);
		bossRewardItem personalRW = calPersonalRankReward(player, personalRank);
		bossRewardItem killRW = calKillReward(isKill);
		bossRewardItem TailRW = calTailReward(player->Base.getName());
		int totalRewardSilver = countryRW.rewardSilver_ + personalRW.rewardSilver_ + killRW.rewardSilver_ + TailRW.rewardSilver_;
		int totalRewardWeiwang = countryRW.rewardCrystal_ + personalRW.rewardCrystal_ + killRW.rewardCrystal_ + TailRW.rewardCrystal_;

		bossRewardItem ret;
		ret.rewardSilver_ = totalRewardSilver;
		ret.rewardCrystal_ = totalRewardWeiwang;
		return ret;
	}

	bool world_boss_system::isBossActValid()
	{
		if (activityState_ != state_ing || getBossData(curSeason_).remainHP_ <= 0)
			return false;

		return true;
	}

	void world_boss_system::setBossInBattleState(bool st)
	{
		isBossInBattle_ = st;
	}

	void world_boss_system::worldBossOver()
	{
		world_boss_sys.setBossInBattleState(false);
//		LogE << __FUNCTION__ << LogEnd;
		if (bossMsgQueue_.size() > 0)
		{
			na::msg::msg_json mj = bossMsgQueue_.front();
			bossMsgQueue_.pop();
//			LogE << __FUNCTION__ << ",pop " << mj._player_id << LogEnd;
			player_mgr.postMessage(mj);
		}
	}

	bool world_boss_system::getBossInBattleState()
	{
		return isBossInBattle_;
	}

	void world_boss_system::updateInfoToClients(bool updateAll/* = false*/)
	{
		if (!updateAll)
		{
			for (set<playerDataPtr>::iterator it = updateClientMgr_.joinPlayerList_.begin(); it != updateClientMgr_.joinPlayerList_.end(); it++)
			{
				updateInfoSingleClient(*it);
			}
		}
		else
		{
			vector<playerDataPtr> vec = player_mgr.onlinePlayer();
			for (vector<playerDataPtr>::iterator it = vec.begin(); it != vec.end(); it++)
			{
				updateInfoSingleClient(*it);
			}
		}
	}

	void world_boss_system::updateMsgToClients(bool updateAll, Json::Value& msgJson, const short protocol)
	{
		vector<playerDataPtr> vec;
		bossDynamicData bossData = getBossData(curSeason_);
		if (!updateAll)
		{
			for (set<playerDataPtr>::iterator it = updateClientMgr_.joinPlayerList_.begin(); it != updateClientMgr_.joinPlayerList_.end(); it++)
			{
				vec.push_back(*it);
			}
		}
		else
		{
			vector<playerDataPtr> vec = player_mgr.onlinePlayer();
			for (vector<playerDataPtr>::iterator it = vec.begin(); it != vec.end(); it++)
			{
			}
		}

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = bossData.remainHP_;

		MsgSignOnline(vec, msg, protocol);
	}

	int world_boss_system::calStorageAttack(playerDataPtr pdata)
	{
		int storageAdd = 0;
		if (pdata->WorldBoss.attackTimes_ == 0 && pdata->WorldBoss.storyCapacityTime_ != 0)
		{
			unsigned curTime = na::time_helper::get_current_time();
			boost::posix_time::ptime p = boost::posix_time::from_time_t(curTime);
			tm t = boost::posix_time::to_tm(p);
			unsigned bossTime = curTime - 3600 * t.tm_hour - 60 * t.tm_min - t.tm_sec + BOSS_HOUR * 3600 + BOSS_BEGIN_MINUTE * 60;
			unsigned minTime = bossTime;
//			if (minTime > bossTime)
//				minTime = bossTime;
			int passTime = (int)minTime - (int)pdata->WorldBoss.storyCapacityTime_;
			if (passTime == 0)
				passTime = 1;
			if (passTime < 0)
				LogE << __FUNCTION__ << ",passTime error!!!" << LogEnd;
			else
				storageAdd = STORAGE_BASE + ((int)ceil((double)passTime / 3600)) * STORAGE_EXTRA;
		}
		return storageAdd;
	}

	int world_boss_system::calGoldInspireCost()
	{
		return 10;
	}

	int world_boss_system::calKejiInspireCost(playerDataPtr player)
	{
		return player->Base.getLevel() * 20 + 500;
	}

	bool world_boss_system::getActivityTime(unsigned &start_time, unsigned &end_time)
	{
		unsigned cur_time = na::time_helper::get_current_time();

		boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
		tm t = boost::posix_time::to_tm(p);

		unsigned stand_time1 = cur_time - 3600 * t.tm_hour - 60 * t.tm_min - t.tm_sec;
		unsigned stand_time2 = stand_time1 + 5 * 3600;

		unsigned start_time1 = stand_time1 + BOSS_HOUR * 3600 + BOSS_BEGIN_MINUTE * 60;
		unsigned end_time1 = stand_time1 + BOSS_HOUR * 3600 + BOSS_END_MINUTE * 60;

		bool ret = false;
		start_time = 0;
		end_time = 0;
		if (cur_time < stand_time2)
		{
			start_time = start_time1 - 86400;
			end_time = end_time1 - 86400;
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
		else
		{
			start_time = start_time1;
			end_time = end_time1;
		}

		return ret;
	}

	updateClientManager::updateClientManager()
	{
		isJoinPlayerListClear_ = false;
	}

	void updateClientManager::addPlayer(playerDataPtr player)
	{
		joinPlayerList_.insert(player);
	}

	void updateClientManager::removePlayer(playerDataPtr player)
	{
		joinPlayerList_.erase(player);
	}


	mapBossData::mapBossData()
	{
		maxHP_ = 0;
	}

	mapBossData::~mapBossData()
	{

	}

	void bossAnnouce::refreshData()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > nextUpdateTime_)
		{
			reset();
			nextUpdateTime_ = na::time_helper::get_next_update_time(cur_time);
		}
	}

	bossAnnouce::bossAnnouce()
	{
		nextUpdateTime_ = 0;
		ba_20_35_ = false;
		ba_21_00_ = false;
		ba_21_04_55_ = false;
		ba_drop_to_70_ = false;
		ba_drop_to_50_ = false;
		ba_drop_to_30_ = false;
		ba_drop_to_20_ = false;
		ba_drop_to_10_ = false;
	}

	void bossAnnouce::reset()
	{
		ba_20_35_ = false;
		ba_21_00_ = false;
		ba_21_04_55_ = false;
		ba_drop_to_70_ = false;
		ba_drop_to_50_ = false;
		ba_drop_to_30_ = false;
		ba_drop_to_20_ = false;
		ba_drop_to_10_ = false;
	}


	storyCapacityItem::storyCapacityItem()
	{
		playerID_ = -1;
		playerName_ = "";;
	}

}




