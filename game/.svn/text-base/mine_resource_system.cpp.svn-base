#include "mine_resource_system.h"
#include "response_def.h"
#include "playerManager.h"
#include "battle_def.h"
#include "battle_system.h"
#include "guild_system.h"
#include "email_system.h"
#include "record_system.h"
#include "battle_system.h"
#include "system_response.h"
#include "gm_tools.h"
#include "translate.h"
#include "task_system.h"
#include "chat_system.h"
#include "activity_system.h"
#include "activity_game_param.h"
using namespace mongo;

// #define MINE_CD 2
// #define MAX_MINE_TIMES 1000

#define MINE_CD 20
#define ROB_CD 5
#define MAX_MINE_TIMES 3
#define MINE_RES_OPEN_LEVEL 18

namespace gg
{
	mine_resource_system* const mine_resource_system::mineResSys = new mine_resource_system();

	mine_resource_system::mine_resource_system()
	{
		duration_ = 1;
	}

	mine_resource_system::~mine_resource_system()
	{

	}

	void mine_resource_system::initData()
	{
		mongo::BSONObj key = BSON(mine_res::strLayer << 1);
		db_mgr.ensure_index(mine_res::strDBMineCommon, key);
		key = BSON(mine_res::strLayer << 1 << mine_res::strMineId << 1);
		db_mgr.ensure_index(mine_res::strDBMineID2Pid, key);
		key = BSON(playerIDStr << 1);
		db_mgr.ensure_index(mine_res::strDBMinePlayerInfo, key);

		initPlayerMineInfo();
		initMineList();
		initMID2PID();
		initReportPath();
		initConfig();
		initNpc();
	}

	bool mine_resource_system::insertMineID(int layer, int mineid)
	{
		if (layer < LEVEL_20_30_LAYER || layer > LEVEL_101_120_LAYER)
			return false;

		vector<int>::iterator it = find(layerMap_[layer].mineIDList_.begin(), layerMap_[layer].mineIDList_.end(), mineid);
		if (it == layerMap_[layer].mineIDList_.end())
		{
			layerMap_[layer].mineIDList_.push_back(mineid);
		}
			
		return saveMineList(layer);
	}

	bool mine_resource_system::removeMineID(int layer, int mineid)
	{
		if (layer < LEVEL_20_30_LAYER || layer > LEVEL_101_120_LAYER)
			return false;

		vector<int>::iterator it = find(layerMap_[layer].mineIDList_.begin(), layerMap_[layer].mineIDList_.end(), mineid);
		if (it != layerMap_[layer].mineIDList_.end())
			layerMap_[layer].mineIDList_.erase(it);
		return saveMineList(layer);
	}

	bool mine_resource_system::saveMineList(int layer)
	{
		if (layer < LEVEL_20_30_LAYER || layer > LEVEL_101_120_LAYER)
			return false;

		layerItem lItem = layerMap_[layer];
		mongo::BSONObj key = BSON(mine_res::strLayer << layer);
		mongo::BSONObjBuilder obj;
		obj << mine_res::strLayer << layer;
// 		obj << mine_res::strStartMineId << lItem.startMineId;
// 		obj << mine_res::strEndMineId << lItem.endMineId;

		mongo::BSONArrayBuilder larr;
		for (unsigned i = 0; i < lItem.mineIDList_.size(); i++)
		{
			larr << lItem.mineIDList_[i];
		}
		obj << mine_res::strMineIDList << larr.arr();

		return db_mgr.save_mongo(mine_res::strDBMineCommon, key, obj.obj());
	}

	bool mine_resource_system::insertMineIDpID(int layer, int mineId, int player_id)
	{
		mineAreaKey mKey(layer, mineId);
		mineIDpIDmap_[mKey] = player_id;

		mongo::BSONObj key = BSON(mine_res::strLayer << layer << mine_res::strMineId << mineId);
		mongo::BSONObjBuilder obj;
		obj << mine_res::strLayer << layer << mine_res::strMineId << mineId;
		obj << playerIDStr << player_id;

		return db_mgr.save_mongo(mine_res::strDBMineID2Pid, key, obj.obj());
	}

	void mine_resource_system::removeMineIDpID( int layer, int mineId )
	{
		mineAreaKey mKey(layer, mineId);
		mineIDpIDmap_.erase(mKey);
		mongo::BSONObj key = BSON(mine_res::strLayer << layer << mine_res::strMineId << mineId);
		db_mgr.remove_collection(mine_res::strDBMineID2Pid, key);
	}

	bool mineCompare(mineItem m1, mineItem m2)
	{
		return m1.mineID_ < m2.mineID_;
	}

	void mine_resource_system::initMineList()
	{
		objCollection objs = db_mgr.Query(mine_res::strDBMineCommon);
		for (unsigned i = LEVEL_20_30_LAYER; i <= LEVEL_101_120_LAYER; i++)
		{
			layerItem tempItem;
			layerMap_[i] = tempItem;
		}
		if (objs.size() == 0)
		{
			return;
		}
		for (unsigned i = 0; i < objs.size(); i++)
		{
			layerItem tempItem;
			mongo::BSONObj obj = objs[i];
			checkNotEoo(obj[mine_res::strMineIDList])
			{
				vector<BSONElement> sets = obj[mine_res::strMineIDList].Array();
				tempItem.mineIDList_.clear();
				for (unsigned j = 0; j < sets.size(); j++)
				{
					tempItem.mineIDList_.push_back(sets[j].Int());
				}
			}
			int layer = obj[mine_res::strLayer].Int();
// 			tempItem.startMineId = obj[mine_res::strStartMineId].Int();
// 			tempItem.endMineId = obj[mine_res::strEndMineId].Int();
			layerMap_[layer] = tempItem;
		}
	}

	void mine_resource_system::initPlayerMineInfo()
	{
	}

	void mine_resource_system::initMID2PID()
	{
		objCollection objs = 	db_mgr.Query(mine_res::strDBMineID2Pid);

		mineIDpIDmap_.clear();
		for (unsigned i = 0; i < objs.size(); i++)
		{
			mongo::BSONObj obj = objs[i];
			mineAreaKey mkey;
			mkey.mineId = obj[mine_res::strMineId].Int();
			mkey.mineLayer = obj[mine_res::strLayer].Int();
			int pid = obj[playerIDStr].Int();
			mineIDpIDmap_[mkey] = pid;
		}
	}

	void mine_resource_system::initReportPath()
	{
		boost::filesystem::path path_dir(mine_res::strReportMineResDir);
		if(!boost::filesystem::exists(path_dir))
			boost::filesystem::create_directory(path_dir);

	}

	void mine_resource_system::initConfig()
	{
		Json::Value cfg = na::file_system::load_jsonfile_val(mine_res::strMineResConfigPath);
		mineConfig_.baseSilver = cfg[mine_res::strConfigBaseGold].asInt();
		mineConfig_.baseWeiwang = cfg[mine_res::strConfigBaseWeiwang].asInt();

		mineConfig_.mineScan.resize(cfg[mine_res::strConfigResCoeff].size());

		mineConfig_.mineScan[0].resCoeff = cfg[mine_res::strConfigResCoeff][0].asDouble();
		for (unsigned i = 1; i < cfg[mine_res::strConfigResCoeff].size(); i++)
		{
			mineConfig_.mineScan[i].resCoeff = cfg[mine_res::strConfigResCoeff][i].asDouble();
		}

		mineConfig_.mineScan[0].generalScanRate = int((cfg[mine_res::strConfigGeneralScanRate][0].asDouble()+PRECISION)*RANGE);
		for (unsigned i = 1; i < cfg[mine_res::strConfigGeneralScanRate].size(); i++)
		{
			mineConfig_.mineScan[i].generalScanRate = int((cfg[mine_res::strConfigGeneralScanRate][i].asDouble()+PRECISION)*RANGE);
			mineConfig_.mineScan[i].generalScanRate = mineConfig_.mineScan[i].generalScanRate + mineConfig_.mineScan[i-1].generalScanRate;
		}

		mineConfig_.mineScan[0].advanceScanRate = int((cfg[mine_res::strConfigAdvanceScanRate][0].asDouble()+PRECISION)*RANGE);
		for (unsigned i = 1; i < cfg[mine_res::strConfigAdvanceScanRate].size(); i++)
		{
			mineConfig_.mineScan[i].advanceScanRate = int((cfg[mine_res::strConfigAdvanceScanRate][i].asDouble()+PRECISION)*RANGE);
			mineConfig_.mineScan[i].advanceScanRate = mineConfig_.mineScan[i].advanceScanRate + mineConfig_.mineScan[i-1].advanceScanRate;
		}
	}

	void mine_resource_system::mineResLoopUpdate(boost::system_time& tmp)
	{
		if ((tmp - tick_).total_seconds() < duration_)return;

		tick_ = tmp;
		duration_ = 20;

		unsigned cur_time = na::time_helper::get_current_time();
		boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
		tm t = boost::posix_time::to_tm(p);

		br_.refresh();
		if (!br_.br_12_30_ && t.tm_hour == 12 && ((t.tm_min == 29 && t.tm_sec > 50) || (t.tm_min == 30)))
			duration_ = 2;
		if (!br_.br_20_00_ && t.tm_hour == 20 && t.tm_min == 0)
			duration_ = 2;

		if (duration_ != 2)
			return;

		if (!br_.br_12_30_ && t.tm_hour == 12 && t.tm_min == 30 && t.tm_sec >= 20)
		{
			br_.br_12_30_ = true;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::mine_res_bonus_12_30, Json::Value::null);
		}

		if (!br_.br_20_00_ && t.tm_hour == 20 && t.tm_min == 0 && t.tm_sec >= 30)
		{
			br_.br_20_00_ = true;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::mine_res_bonus_20_00, Json::Value::null);
		}
	}

	void mine_resource_system::initNpc()
	{
		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(mine_res::strMineResNpcDir, jv);

		for (unsigned i = 0; i < jv.size(); ++i)
		{
			Json::Value& map_data = jv[i];
			for (unsigned mapIndex = 0; mapIndex < map_data.size(); ++mapIndex)
			{
				Json::Value mapDataJson = map_data[mapIndex];
//				Json::Value mapDataJson = map_data;
				mapDataPtr mData = create();
				//mData->mapName = TransLate::getTranslate(mapDataJson[WAR::warStoryNameStr].asString());
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
				npcMap_[mData->localID] = mData;
			}			
		}
	}

	bool mine_resource_system::savePlayerMineInfo( int playerId, mineItem& mItem )
	{
		mongo::BSONObj key = BSON(playerIDStr << playerId);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << mItem.player_id_;
		obj << playerLevelStr << mItem.playerLevel_;
		obj << mine_res::strMineId << mItem.mineID_;
		obj << mine_res::strQuality << mItem.quality_;
		obj << mine_res::strBeRobTimes << mItem.beRobTimes_;
		obj << mine_res::strRemainMineTimes << mItem.remainMineTimes_;
		obj << mine_res::strStartMineTime_ << mItem.startMineTime_;
		obj << mine_res::strActExtraRate << mItem.actExtraRate_;
		obj << mine_res::strScienceMineExtraRate << mItem.ScienceMineExtraRate_;
		obj << mine_res::strRemainRobTimes << mItem.remainRobTimes_;
		obj << mine_res::strHasSeekMineCDHelp << mItem.hasSeekMineCDHelp_;
		obj << mine_res::strHasSeekMineHelp << mItem.hasSeekMineHelp_;
		obj << mine_res::strBeHelpMineTimes << mItem.beHelpMineTimes_;
		obj << mine_res::strMineCD << mItem.mineCD_;
		obj << mine_res::strRobCD << mItem.robCD_;
		obj << mine_res::strMaxReportId << mItem.maxReportId_;
		obj << mine_res::strNextUpdateTime << mItem.nextUpdateTime_;
		obj << mine_res::strGeneralScanTimes << mItem.generalScanTimes_;
		obj << mine_res::strAdvanceScanTimes << mItem.advanceScanTimes_;
		obj << mine_res::strTotalBeRobSilver << mItem.totalBeRobSilver_;
		obj << mine_res::strTotalBeRobWeiwang << mItem.totalBeRobWeiwang_;

		mongo::BSONArrayBuilder rlarr;
		for (vector<mineReport>::iterator it = mItem.reportList_.begin(); it != mItem.reportList_.end(); it++)
		{
			mongo::BSONObjBuilder objReport;
			objReport << mine_res::strReportAtkName << it->atkName;
			objReport << mine_res::strReportDefName << it->defName;
			objReport << mine_res::strReportAtkID << it->atkID;
			objReport << mine_res::strReportDefID << it->defID;
			objReport << mine_res::strReportAtkLink << it->atkReportLink;
			objReport << mine_res::strReportDefLink << it->defReportLink;
			objReport << mine_res::strIsPlayer << it->isPlayer;
			objReport << mine_res::strReportResult << it->result;
			objReport << mine_res::strReportTime << it->battleTime;
			rlarr << objReport.obj();
		}
		obj << mine_res::strReportList << rlarr.arr();

		playerMineInfoMap_[playerId] = mItem;
		return db_mgr.save_mongo(mine_res::strDBMinePlayerInfo, key, obj.obj());
	}

	void mine_resource_system::mineResUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		vector <mineItem> mineList;
		vector <int> npcList;
		int pageId = 1;
		int maxPageId = 1;
		int layer = level2layer(d->Base.getLevel());
		if (layer == -1 || d->Base.getSphereID() == -1)
		{
			Return (r, 1);
		}
		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}
		AsyncRefreshMine(m, layer);

		mineItem myItem;
		if (getPlayerMineInfo(d->playerID, myItem))
		{
			int mineId = myItem.mineID_;
			pageId = getCurrentPage(layer, mineId);
		}
		else
		{
			createMineItem(d, myItem);
		}
		getMineItemsByPage(layer, pageId, mineList, npcList);

		bool isFindPlayer = true;
		vector <int> playerList;
		for (vector<mineItem>::iterator it = mineList.begin(); it != mineList.end(); it++)
		{
			playerDataPtr player = player_mgr.getPlayerMain(it->player_id_);
			if (player == playerDataPtr())
			{
				isFindPlayer = false;
				playerList.push_back(it->player_id_);
			}
		}
		if (!isFindPlayer)
		{
			if (m._post_times < 4)
			{//次数多1次，以免挖矿刷新过于频繁导致少部分玩家数据查找不到
				player_mgr.postMessage(m);
				return;
			}
			else
			{
				Return(r, -1);
			}
		}

		maxPageId = getMaxPage(layer);
		Json::Value res = Json::arrayValue;
		for (vector<mineItem>::iterator it = mineList.begin(); it != mineList.end(); it++)
		{
			Json::Value pack;
			playerDataPtr pdata = player_mgr.getPlayerMain(it->player_id_);
			if (pdata == playerDataPtr())
				Return(r, -1);
			it->packageLittleBaseInfo(pdata, pack);
			res.append(pack);
		}
		r[msgStr][1][mine_res::strActBonus] = act_game_param_mgr.get_game_param_by_id(activity::_mine_battle);
		r[msgStr][1][mine_res::strMineList] = res;
		r[msgStr][1][mine_res::strPageId] = pageId;
		r[msgStr][1][mine_res::strMaxPageId] = maxPageId;
		r[msgStr][1][mine_res::strNpcList] = Json::arrayValue;
		for (unsigned i = 0; i < npcList.size(); i++)
		{
			r[msgStr][1][mine_res::strNpcList].append(npcList[i]);
		}
		double mineRate = 0.0f;
		if (myItem.isMining())
		{
			mineRate = myItem.ScienceMineExtraRate_;
		}
		else
		{
			mineRate = getMineExtraRate(d);
		}
		
		myItem.packageLargeBaseInfo(d, r[msgStr][1][mine_res::strMineMyInfo], true, false, mineRate);

		Return(r, 0);
	}

	void mine_resource_system::mineResTurnPageReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int pageId = js_msg[0u].asInt();
		vector <mineItem> mineList;
		vector <int> npcList;
		int maxPageId;
		mineItem myItem;
		int layer = level2layer(d->Base.getLevel());
		if (layer == -1)
		{
			Return (r, 1);
		}
		if (pageId < 1)
		{
			Return(r, -1);
		}
		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}

		AsyncRefreshMine(m, layer);
		if (getPlayerMineInfo(d->playerID, myItem))
		{
		}
		else
		{
		}
		maxPageId = getMaxPage(layer);
		if (pageId > maxPageId)
		{
			Return(r, 1);
		}
		getMineItemsByPage(layer, pageId, mineList, npcList);

		bool isFindPlayer = true;
		vector <int> playerList;
		for (vector<mineItem>::iterator it = mineList.begin(); it != mineList.end(); it++)
		{
			playerDataPtr player = player_mgr.getPlayerMain(it->player_id_);
			if (player == playerDataPtr())
			{
				isFindPlayer = false;
				playerList.push_back(it->player_id_);
			}
		}
		if (!isFindPlayer)
		{
			if (m._post_times < 4)
			{//次数多1次，以免挖矿刷新过于频繁导致少部分玩家数据查找不到
				player_mgr.postMessage(m);
				return;
			}
			else
			{
				Return(r, -1);
			}
		}

		Json::Value res = Json::arrayValue;

		for (vector<mineItem>::iterator it = mineList.begin(); it != mineList.end(); it++)
		{
			Json::Value pack;
			playerDataPtr pdata = player_mgr.getPlayerMain(it->player_id_);
			if (pdata == playerDataPtr())
				Return(r, -1);
			it->packageLittleBaseInfo(pdata, pack);
			res.append(pack);
		}

		r[msgStr][1][mine_res::strMineList] = res;
		r[msgStr][1][mine_res::strPageId] = pageId;
		r[msgStr][1][mine_res::strMaxPageId] = maxPageId;
		r[msgStr][1][mine_res::strNpcList] = Json::arrayValue;

		for (unsigned i = 0; i < npcList.size(); i++)
		{
			r[msgStr][1][mine_res::strNpcList].append(npcList[i]);
		}

		double mineRate = 0.0f;
		if (myItem.isMining())
		{
			mineRate = myItem.ScienceMineExtraRate_;
		}
		else
		{
			mineRate = getMineExtraRate(d);
		}

		myItem.packageLargeBaseInfo(d, r[msgStr][1][mine_res::strMineMyInfo], true, false, mineRate);
		Return(r, 0);
	}

	void mine_resource_system::mineResFightRecordReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}
		int layer = level2layer(d->Base.getLevel());
		AsyncRefreshMine(m, layer);

		int player_id = m._player_id;
		Json::Value temp = Json::arrayValue;
		mineItem myItem;
		if (getPlayerMineInfo(d->playerID, myItem))
		{
			myItem.packageReport(temp);
		}
		r[msgStr][1] = temp;
		Return(r, 0);
	}

	void mine_resource_system::mineResClearFightCdReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		int player_id = m._player_id;
		int level = d->Base.getLevel();
		int layer = level2layer(level);
		if (layer == -1)
		{
			Return (r, 1);
		}
		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}

		AsyncRefreshMine(m, layer);

		mineItem mItem;
		if (!getPlayerMineInfo(d->playerID, mItem))
		{
			Return(r, -1);
		}

		int mineId = mItem.mineID_;
		int curTime = na::time_helper::get_current_time();
		if (mItem.robCD_ < curTime)
		{
			Return(r, 2);
		}

		int costMinute = int(ceil(double(mItem.robCD_ - curTime) / 60));
		int costGold = 1 * costMinute;
		if (d->Base.getAllGold() < costGold)
		{
			Return(r, 3);
		}
		d->Base.alterBothGold(-costGold);
		mItem.robCD_ = 0;

		savePlayerMineInfo(player_id, mItem);

		Json::Value pack;
		Json::Value updateJson;
		double mineRate = 0.0f;
		if (mItem.isMining())
		{
			mineRate = mItem.ScienceMineExtraRate_;
		}
		else
		{
			mineRate = getMineExtraRate(d);
		}
		mItem.packageLargeBaseInfo(d, pack, true, false, mineRate);
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1] = pack;
		//cout << __LINE__ << ",updateJson:" << updateJson.toStyledString();
		player_mgr.sendToPlayer(player_id, gate_client::mine_res_update_my_info_resp, updateJson);

		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(costMinute));
		fields.push_back(boost::lexical_cast<string, int>(costGold));
		StreamLog::Log(mine_res::mysqlLogMineRes, d, boost::lexical_cast<string, int>(curTime - na::time_helper::timeZone() * 3600), 
			boost::lexical_cast<string, int>(mItem.mineID_), fields, mine_res_log_clear_fight_cd);

		Return(r, 0);
	}
	 
	void mine_resource_system::mineResSingleUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int level = d->Base.getLevel();
		int layer = level2layer(level);
		int mineId = js_msg[0u].asInt();
		int update_opt = js_msg[1u].asInt();
		if (update_opt != 0 && update_opt != 1)
		{
			Return(r, -1);
		}

		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}
		AsyncRefreshMine(m, layer);
		mineItem mItem;
		double robCoeff = getRobExtraRate(d);
		if (update_opt == 1)
		{//npc
			if (npcMap_.find(mineId) == npcMap_.end())
			{
				Return(r, -1);
			}
			
			Json::Value pack;
			createMineNpcItem(npcMap_[mineId], mItem, d);
			mItem.packageLargeNpcBaseInfo(d, npcMap_[mineId], pack, true, true, robCoeff);
			r[msgStr][1] = pack;

			Return(r, 0);
		}

		int player_id = getPlayerIdByLMMap(layer, mineId);
		if (player_id <= 0)
			Return(r, -1);

		playerDataPtr player = player_mgr.getPlayerMain(player_id);
		if (player == playerDataPtr())
		{
			if (m._post_times > 0)
			{
				Return(r, -1);
			}
			player_mgr.postMessage(m);
			return;
		}

		if (!getPlayerMineInfo(player->playerID, mItem))
		{
			Return(r, -1);
		}
		if (!mItem.isMining())
		{
			Return(r, -1);
		}

		if (layer < LEVEL_20_30_LAYER || layer > LEVEL_101_120_LAYER)
		{
			Return(r, -1);
		}

		Json::Value pack;
		if (player_id == d->playerID)
		{
			double mineRate = 0.0f;
			if (mItem.isMining())
			{
				mineRate = mItem.ScienceMineExtraRate_;
			}
			else
			{
				mineRate = getMineExtraRate(d);
			}
			mItem.packageLargeBaseInfo(player, pack, true, false, mineRate);
		}
		else
		{
			mItem.packageLargeBaseInfo(player, pack, false, true, 0.0f, robCoeff);
		}
		r[msgStr][1] = pack;

		Return(r, 0);
	}

	int mine_resource_system::getPlayerIdByLMMap(int layer, int mineID)
	{
		mineAreaKey mKey(layer, mineID);
		map <mineAreaKey, int>::iterator it = mineIDpIDmap_.find(mKey);
		if (it == mineIDpIDmap_.end())
			return -1;

		return it->second;
	}

	void mine_resource_system::mineResFightReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (d->Base.getLevel() < MINE_RES_OPEN_LEVEL)
			Return(r, -1);

		ReadJsonArray;
		if(d->isOperatBattle())Return(r, 5);
		int atk_player_id = m._player_id;
		int targetMineId = js_msg[0u].asInt();
		int fight_opt = js_msg[1].asInt();
		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}

		int level = d->Base.getLevel();
		int layer = level2layer(level);
		if (layer == -1)
		{
			Return(r, 1);
		}

		AsyncRefreshMine(m, layer);
		mineItem atkItem;
		if (!getPlayerMineInfo(d->playerID, atkItem))
		{
			Return(r, -1);
		}
		if (atkItem.remainRobTimes_ <= 0)
		{
			Return(r, 2);
		}

		int cur_time = na::time_helper::get_current_time();
		int rcd = atkItem.robCD_;
		if (cur_time < rcd)
		{
			Return(r, 3);
		}

		int def_player_id;
		battleSidePtr npcSide;
		Battlefield field;

		playerDataPtr atkData = player_mgr.getPlayerMain(m._player_id);
		if (fight_opt == 1)
		{//npc
			def_player_id = 0;
			map<int, mapDataPtr>::iterator it_npc;
			it_npc = npcMap_.find(targetMineId);
			if(it_npc == npcMap_.end())Return(r, -1);

			if (npcid2layer(it_npc->second->localID) != layer)
			{
				Return(r, -1);
			}

			if (atkData == playerDataPtr())
			{
				if (m._post_times > 1)Return(r, -1);
				player_mgr.postMessage(m);
				return;
			}

			field.defSide = battleSide::Create(it_npc->second);
		}
		else
		{
			def_player_id = getPlayerIdByLMMap(layer, targetMineId);
			playerDataPtr defData = player_mgr.getPlayerMain(def_player_id);
			if (atkData == playerDataPtr() || defData == playerDataPtr())
			{
				if (m._post_times > 1)Return(r, -1);
				player_mgr.postMessage(m);
				return;
			}
			if (def_player_id < 0)
				Return(r, -1);

			mineItem defItem;
			if (!getPlayerMineInfo(defData->playerID, defItem))
			{
				Return(r, -1);
			}
			if (atk_player_id == def_player_id)
			{
				Return(r, -1);
			}
			if (!defItem.isMining())
			{
				Return(r, -1);
			}
			if (defItem.beRobTimes_ >= 2)
			{
				Return(r, 4);
			}
			if(defData->isOperatBattle())Return(r, 5);

			if (atkData->Guild.getGuildID() != -1 && atkData->Guild.getGuildID() == defData->Guild.getGuildID())
			{
				Return(r, 6);
			}

			field.defSide = battleSide::Create(defData);
			field.defSide->setFileName(defItem.getReportName(defData));
			savePlayerMineInfo(defData->playerID, defItem);
		}

		field.atkSide = battleSide::Create(atkData);
		field.atkSide->setFileName(atkItem.getReportName(atkData));
		savePlayerMineInfo(atkData->playerID, atkItem);
		if(field.atkSide == battleSidePtr() || field.defSide == battleSidePtr())Return(r, -1);
		activity_sys.updateActivity(d, 0, activity::_occupy_mine);
		task_sys.updateBranchTaskInfo(d, _task_rob_other_mine_times);
		battle_sys.PostBattle(field, battle_mine_res_type);
	}

	void mine_resource_system::mineResFightCallback( const int battleResult, Battlefield field, Json::Value &report )
	{//战报，奖励,log,发送给客户端
		State::setState(gate_client::mine_res_fight_req);
		NumberCounter::Step();
		int def_player_id = field.defSide->getSideID();
		bool isPlayer = false;
		if (def_player_id > (1 << 20))
		{
			isPlayer = true;
		}
		playerDataPtr atkData = player_mgr.getPlayerMain(field.atkSide->getSideID());
		playerDataPtr defData;

		if (isPlayer)
		{
			defData = player_mgr.getPlayerMain(field.defSide->getSideID());
		}
		if(playerDataPtr() == atkData || isPlayer && playerDataPtr() == defData)return;

		mineReport mr;
		mr.result = battleResult;
		mr.atkName = field.atkSide->getName();
		mr.defName = field.defSide->getName();
		mr.atkID = field.atkSide->getPlayerID();
		mr.defID = field.defSide->getPlayerID();
		mr.atkReportLink = field.atkSide->getFileName();
		mr.defReportLink = field.defSide->getFileName();
		mr.isPlayer = field.defSide->isPlayerSide();

		int cur_time = na::time_helper::get_current_time();
		mr.battleTime = cur_time;

		mineItem atkItem;
		if (!getPlayerMineInfo(atkData->playerID, atkItem))
		{
			return;
		}
		atkItem.insertReport(mr);

		if (cur_time > atkItem.robCD_)
		{
			atkItem.robCD_ = cur_time + ROB_CD * 60;
		}
		else
		{
			atkItem.robCD_ += ROB_CD * 60;
		}
		atkItem.remainRobTimes_--;

		double robRate = getRobExtraRate(atkData);

		mineItem defItem;
		int loseSilver = 0;
		int loseWeiwang = 0;
		if (isPlayer)
		{
			if (!getPlayerMineInfo(defData->playerID, defItem))
			{
				return;
			}
			defItem.insertReport(mr);
			int silver1 = defItem.getMineRewardSilver(atkData, defItem.playerLevel_, true, false, true, true, 0.0f, robRate);
			int silver2 = defItem.getMineRewardSilver(atkData, defItem.playerLevel_, false, true, false, false);
			int weiwang1 = defItem.getMineRewardWeiwang(atkData, defItem.playerLevel_, true, false, true, true, 0.0f, robRate);
			int weiwang2 = defItem.getMineRewardWeiwang(atkData, defItem.playerLevel_, false, true, false, false);

			if (battleResult == 1)
			{
				defItem.beRobTimes_++;
				loseSilver = int(silver1 * 0.15) + int(silver2 * 0.15);
				loseWeiwang = int(weiwang1 * 0.15) + int(weiwang2 * 0.15);
				defItem.totalBeRobSilver_ += loseSilver;
				defItem.totalBeRobWeiwang_ += loseWeiwang;
			}

			savePlayerMineInfo(field.defSide->getSideID(), defItem);
		}
		else
		{
			map<int, mapDataPtr>::iterator it_npc = npcMap_.find(def_player_id);
			if(it_npc == npcMap_.end())return;
			mineItem mItem;
			createMineNpcItem(it_npc->second, mItem, atkData);

			int silver1 = mItem.getMineRewardSilver(atkData, mItem.playerLevel_, true, false, true, true, 0.0f, robRate);
			int silver2 = mItem.getMineRewardSilver(atkData, mItem.playerLevel_, false, true, false, false);
			int weiwang1 = mItem.getMineRewardWeiwang(atkData, mItem.playerLevel_, true, false, true, true, 0.0f, robRate);
			int weiwang2 = mItem.getMineRewardWeiwang(atkData, mItem.playerLevel_, false, true, false, false);

			loseSilver = int(silver1 * 0.15) + int(silver2 * 0.15);
			loseWeiwang = int(weiwang1 * 0.15) + int(weiwang2 * 0.15);
		}

		if (battleResult == 1)
		{
			Json::Value actionJson, actionItem;
			actionItem[ACTION::strActionID] = action_add_silver;
			actionItem[ACTION::strValue] = loseSilver;
			actionJson.append(actionItem);

			actionItem = Json::nullValue;
			actionItem[ACTION::strActionID] = action_add_weiwang;
			actionItem[ACTION::strValue] = loseWeiwang;
			actionJson.append(actionItem);
			
			Json::Value Param;
			armyVec av = field.atkSide->getMember(MO::mem_null);
			Json::Value& PilotList = Param[ACTION::strPilotList];
			for (unsigned i = 0; i < av.size(); i++)	PilotList.append(av[i]->getID());

			actionFormat::actionDoJump(atkData, actionJson, Param);

			Json::Value lastDoJson = actionFormat::getLastDoJson();;

			{
				Json::Value doJson;
				Json::Value PilotInfo = Json::arrayValue;
				doJson.append(action_add_pilot_exp);
				doJson.append(0);
				for (unsigned i = 0; i < PilotList.size(); i++)
				{//所有在阵上的武将都加经验
					if(!PilotList[i].isInt())continue;
					int pilotID = PilotList[i].asInt();
					const playerPilot& cPilot = atkData->Pilots.getPlayerPilotExtert(pilotID);
					if(cPilot == playerPilots::NullPilotExtert)continue;
					Json::Value pilotJson;
					pilotJson.append(cPilot.pilotID);
					pilotJson.append(cPilot.pilotLevel);
					pilotJson.append(cPilot.pilotExp);
//					d->Pilots.addExp(pilotID, value[ACTION::strValue].asInt());
					pilotJson.append(cPilot.pilotLevel);
					pilotJson.append(cPilot.pilotExp);
					PilotInfo.append(pilotJson);
				}
				if(PilotInfo.empty())doJson = Json::nullValue;
				else doJson.append(PilotInfo);
				lastDoJson.append(doJson);

				doJson = Json::nullValue;
				doJson.append((action_add_role_exp));
				doJson.append(0);
				doJson.append(atkData->Base.getLevel());
				doJson.append(atkData->Base.getExp());
				doJson.append(atkData->Base.getLevel());
				doJson.append(atkData->Base.getExp());
				lastDoJson.append(doJson);
			}

			report[BATTLE::strBattleReward] = lastDoJson;
		}
		else
		{
			report[BATTLE::strBattleReward] = Json::arrayValue;
		}

		Json::Value emailJson, attachJson;
		emailJson[mine_res::strLoseSilver] = loseSilver;
		emailJson[mine_res::strLoseWeiwang] = loseWeiwang;
		emailJson[mine_res::strReportAtkName] = atkData->Base.getName();
		emailJson[mine_res::strReportDefName] = field.defSide->getName();
		emailJson[mine_res::strIsPlayer] = field.defSide->isPlayerSide();
		emailJson[mine_res::strReportResult] = battleResult;

		savePlayerMineInfo(field.atkSide->getSideID(), atkItem);

		emailJson["cn"] = atkData->Base.getName();
//		email_sys.sendSystemEmailCommon(atkData, email_type_system_ordinary, email_team_system_mine_res_atk, emailJson, attachJson);
		if (isPlayer)
		{
			emailJson["cn"] = defData->Base.getName();
//			email_sys.sendSystemEmailCommon(defData, email_type_system_ordinary, email_team_system_mine_res_atk, emailJson, attachJson);
		}

		vector<string> fields;

		fields.push_back(boost::lexical_cast<string, int>(def_player_id));
		fields.push_back(boost::lexical_cast<string, int>(battleResult));
		fields.push_back(boost::lexical_cast<string, int>(loseSilver));
		fields.push_back(boost::lexical_cast<string, int>(loseWeiwang));
		if (isPlayer)
			fields.push_back(boost::lexical_cast<string, int>(0));
		else
			fields.push_back(boost::lexical_cast<string, int>(1));
		StreamLog::Log(mine_res::mysqlLogMineRes, atkData, boost::lexical_cast<string, int>(cur_time - na::time_helper::timeZone() * 3600), 
			boost::lexical_cast<string, int>(defItem.mineID_), fields, mine_res_log_rob);


		_Battle_Post(boost::bind(&battle_system::MineResRespon, battle_system::battleSys, battleResult, field, atkData, report));
	}

	void mine_resource_system::mineResFightEnding(const int battleResult, Battlefield field, playerDataPtr atkData)
	{
		State::setState(gate_client::mine_res_fight_req);
		NumberCounter::Step();
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = battleResult;
		msg[msgStr][2u] = field.atkSide->getFileName();
		player_mgr.sendToPlayer(atkData->playerID, atkData->netID, gate_client::mine_res_fight_resp, msg);

		Json::Value pack;
		Json::Value updateJson;

		mineItem atkItem;
		if (!getPlayerMineInfo(atkData->playerID, atkItem))
		{
			return;
		}

		double mineRate = 0.0f;
		if (atkItem.isMining())
		{
			mineRate = atkItem.ScienceMineExtraRate_;
		}
		else
		{
			mineRate = getMineExtraRate(atkData);
		}
		atkItem.packageLargeBaseInfo(atkData, pack, true, false, mineRate);
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1] = pack;
		//cout << __LINE__ << ",updateJson:" << updateJson.toStyledString();
		player_mgr.sendToPlayer(atkData->playerID, atkData->netID, gate_client::mine_res_update_my_info_resp, updateJson);
		helper_mgr.runSaveAndUpdate();
	}

	bool mine_resource_system::levelUp(playerDataPtr pdata)
	{
		int plevel = pdata->Base.getLevel();
		if (plevel != 30 && plevel != 40 && plevel != 60
			&& plevel != 80	&& plevel != 100 && plevel != 120)
		{
			return false;
		}

		mineItem mItem;
		if (!getPlayerMineInfo(pdata->playerID, mItem))
			return false;
		if (mItem.mineCD_ <= 0)
			return false;

		return finishSingleMine(pdata);
	}

	void mine_resource_system::mineResFreeScanReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		int player_id = m._player_id;

		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}
		int layer = level2layer(d->Base.getLevel());
		AsyncRefreshMine(m, layer);

		mineItem mItem;
		if (!getPlayerMineInfo(d->playerID, mItem))
		{
			createMineItem(d, mItem);
		}

		int mine_quality = mItem.scanMine(scan_null);

		r[msgStr][1] = mine_quality;

		mItem.playerLevel_ = d->Base.getLevel();
		Json::Value pack;
		Json::Value updateJson;
		savePlayerMineInfo(mItem.player_id_, mItem);

		double mineRate = 0.0f;
		if (mItem.isMining())
		{
			mineRate = mItem.ScienceMineExtraRate_;
		}
		else
		{
			mineRate = getMineExtraRate(d);
		}

		mItem.packageLargeBaseInfo(d, pack, true, false, mineRate);
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1] = pack;
		//cout << __LINE__ << ",updateJson:" << updateJson.toStyledString();
		player_mgr.sendToPlayer(player_id, gate_client::mine_res_update_my_info_resp, updateJson);

		Return(r, 0);
	}

	void mine_resource_system::mineResGeneralScanReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}

		int layer = level2layer(d->Base.getLevel());
		AsyncRefreshMine(m, layer);

		mineItem mItem;
		int player_id = m._player_id;
		if (!getPlayerMineInfo(d->playerID, mItem))
		{
			Return(r, -1);
		}

		if (mItem.isMining())
		{
			Return(r, 1);
		}

		int silverCost = mItem.getGeneralScanCost();
		if (d->Base.getSilver() < silverCost)
		{//不够银币
			Return(r, 1);
		}

		if (mItem.generalScanTimes_ >= 2)
			Return(r, 2);

		int mine_quality = mItem.scanMine(scan_general);
		mItem.playerLevel_ = d->Base.getLevel();
		d->Base.alterSilver(-silverCost);
		mItem.generalScanTimes_++;
		r[msgStr][1] = mine_quality;

		Json::Value pack;
		Json::Value updateJson;
		savePlayerMineInfo(mItem.player_id_, mItem);

		double mineRate = 0.0f;
		if (mItem.isMining())
		{
			mineRate = mItem.ScienceMineExtraRate_;
		}
		else
		{
			mineRate = getMineExtraRate(d);
		}

		mItem.packageLargeBaseInfo(d, pack, true, false, mineRate);
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1] = pack;
		//cout << __LINE__ << ",updateJson:" << updateJson.toStyledString();
		player_mgr.sendToPlayer(player_id, gate_client::mine_res_update_my_info_resp, updateJson);
		
		unsigned cur_time = na::time_helper::get_current_time();
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(silverCost));
		StreamLog::Log(mine_res::mysqlLogMineRes, d, boost::lexical_cast<string, int>(cur_time - na::time_helper::timeZone() * 3600), 
			boost::lexical_cast<string, int>(mItem.quality_), fields, mine_res_log_general_scan);
			
		Return(r, 0);
	}

	void mine_resource_system::mineResAdvanceScanReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}

		int layer = level2layer(d->Base.getLevel());
		AsyncRefreshMine(m, layer);

		mineItem mItem;
		int player_id = m._player_id;
		if (!getPlayerMineInfo(d->playerID, mItem))
		{
			Return(r, -1);
		}

		if (mItem.isMining())
		{
			Return(r, 1);
		}

		int goldCost = mItem.getAdvanceScanCost();
		if (d->Base.getAllGold() < goldCost)
		{//不够金币
			Return(r, 1);
		}

		int mine_quality = mItem.scanMine(scan_advance);
		mItem.playerLevel_ = d->Base.getLevel();
		d->Base.alterBothGold(-goldCost);
		mItem.advanceScanTimes_++;
		r[msgStr][1] = mine_quality;

		Json::Value pack;
		Json::Value updateJson;
		savePlayerMineInfo(mItem.player_id_, mItem);

		double mineRate = 0.0f;
		if (mItem.isMining())
		{
			mineRate = mItem.ScienceMineExtraRate_;
		}
		else
		{
			mineRate = getMineExtraRate(d);
		}

		mItem.packageLargeBaseInfo(d, pack, true, false, mineRate);
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1] = pack;
		//cout << __LINE__ << ",updateJson:" << updateJson.toStyledString();
		player_mgr.sendToPlayer(player_id, gate_client::mine_res_update_my_info_resp, updateJson);

		unsigned cur_time = na::time_helper::get_current_time();
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(goldCost));
		StreamLog::Log(mine_res::mysqlLogMineRes, d, boost::lexical_cast<string, int>(cur_time - na::time_helper::timeZone() * 3600), 
			boost::lexical_cast<string, int>(mItem.quality_), fields, mine_res_log_advance_scan);
		
		Return(r, 0);
	}

	void mine_resource_system::mineResMineReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		int layer = level2layer(d->Base.getLevel());
		if (layer == -1)
		{
			Return(r, 1)
		}

		if (d->Base.getLevel() < MINE_RES_OPEN_LEVEL)
			Return(r, -1);

		if (d->Base.getSphereID() == -1)
		{
			Return (r, 10);
		}
		AsyncRefreshMine(m, layer);

		mineItem myItem;
		int player_id = m._player_id;
		if (!getPlayerMineInfo(d->playerID, myItem))
		{
			Return(r, 3);
		}


		int cur_time = na::time_helper::get_current_time();
		if (cur_time < myItem.mineCD_)
		{//还在采矿中，不能采
			Return(r, 2);
		}

		if (myItem.remainMineTimes_ <= 0)
		{//剩余采矿次数不足
			Return(r, 3);
		}

		if (cur_time < myItem.mineCD_)
		{//采矿CD中
			Return(r, 4);
		}

		{
			myItem.beginToMine(layer, d);
			insertMineID(layer, myItem.mineID_);
			insertMineIDpID(layer, myItem.mineID_, player_id);
			savePlayerMineInfo(player_id, myItem);

//			StreamLog::Log(mine_res::mysqlLogMineRes, d, cur_time - na::time_helper::timeZone() * 3600, myItem.mineID_, mine_res_log_mine);
			
			Json::Value pack;
			myItem.packageLargeBaseInfo(d, pack, true, false, myItem.ScienceMineExtraRate_);
			Json::Value updateJson;
			updateJson[msgStr][0u] = 0;
			updateJson[msgStr][1] = pack;
			//cout << __LINE__ << ",updateJson:" << updateJson.toStyledString();
			player_mgr.sendToPlayer(player_id, gate_client::mine_res_update_my_info_resp, updateJson);

			string sreq = "";
			msg_json mj(gate_client::mine_res_update_req, sreq);
			mj._player_id = m._player_id;
			mj._net_id = m._net_id;
			player_mgr.postMessage(mj);
			activity_sys.updateActivity(d, 0, activity::_mining);
			task_sys.updateBranchTaskInfo(d, _task_mine_times);
			Return(r, 0);
		}
	}


	bool mine_resource_system::refreshMineListBegin(int layer)
	{
		if (refreshList_[layer].size() == 0)
		{
			for (unsigned refreshIdx = 0; refreshIdx < layerMap_[layer].mineIDList_.size(); refreshIdx++)
			{
				int refresh_player_id = getPlayerIdByLMMap(layer, layerMap_[layer].mineIDList_[refreshIdx]);
				if (refresh_player_id < 0)
					continue;

				mineItem mItem;
				if (!getPlayerMineInfo(refresh_player_id, mItem))
					continue;
				if (mItem.isGainTime())
				{
					refreshList_[layer].push_back(refresh_player_id);
				}
				else
				{				
					break;
				}
			}
		}

		bool isfind = false;
		for (unsigned refreshIdx = 0; refreshIdx < refreshList_[layer].size(); refreshIdx++)
		{
			playerDataPtr pdata = player_mgr.getPlayerMain(refreshList_[layer][refreshIdx]);
			if (pdata == playerDataPtr()) isfind = true;
		}
		return isfind;
	}

	bool mine_resource_system::refreshMineListEnd(int layer)
	{
		for (unsigned refreshIdx = 0; refreshIdx < refreshList_[layer].size(); refreshIdx++)
		{
			int player_id = refreshList_[layer][refreshIdx];
			playerDataPtr pdata = player_mgr.getPlayerMain(player_id);
			finishSingleMine(pdata);
		}
		refreshList_[layer].clear();
		return true;
	}

	bool mine_resource_system::finishSingleMine(playerDataPtr pdata)
	{
		mineItem mItem;
		if (!getPlayerMineInfo(pdata->playerID, mItem))
			return false;
		int oldId = mItem.mineID_;
		int layer = level2layer(pdata->Base.getLevel());
		mItem.gain(pdata);
		guild_sys.removeHelp(pdata, Guild::GuildHelp::mine_battle);
		mItem.scanMine(scan_null);
		removeMineID(layer, oldId);
		saveMineList(layer);
		savePlayerMineInfo(pdata->playerID, mItem);
		removeMineIDpID(layer, oldId);
		return true;
	}

	bool mine_resource_system::getPlayerMineInfo(playerDataPtr pdata, mineItem& mItem)
	{
		bool ret = getPlayerMineInfo(pdata->playerID, mItem);
		if (!ret)
			return ret;

		if (mItem.isGainTime())
		{
			finishSingleMine(pdata);
			getPlayerMineInfo(pdata->playerID, mItem);
			ret = false;
		}

		return ret;
	}

	bool mine_resource_system::getPlayerMineInfo(int player_id, mineItem& mItem)
	{
		if (playerMineInfoMap_.find(player_id) == playerMineInfoMap_.end())
		{
			mongo::BSONObj key = BSON(playerIDStr << player_id);
			mongo::BSONObj obj = db_mgr.FindOne(mine_res::strDBMinePlayerInfo, key);

			if (obj.isEmpty())
				return false;

			checkNotEoo(obj[playerIDStr])
				mItem.player_id_ = obj[playerIDStr].Int();
			checkNotEoo(obj[playerLevelStr])
				mItem.playerLevel_ = obj[playerLevelStr].Int();
			checkNotEoo(obj[mine_res::strMineId])
				mItem.mineID_ = obj[mine_res::strMineId].Int();
			checkNotEoo(obj[mine_res::strQuality])
				mItem.quality_ = obj[mine_res::strQuality].Int();
			checkNotEoo(obj[mine_res::strBeRobTimes])
				mItem.beRobTimes_ = obj[mine_res::strBeRobTimes].Int();
			checkNotEoo(obj[mine_res::strRemainMineTimes])
				mItem.remainMineTimes_ = obj[mine_res::strRemainMineTimes].Int();
			checkNotEoo(obj[mine_res::strStartMineTime_])
				mItem.startMineTime_ = obj[mine_res::strStartMineTime_].Int();
			checkNotEoo(obj[mine_res::strActExtraRate])
				mItem.actExtraRate_ = obj[mine_res::strActExtraRate].Double();
			checkNotEoo(obj[mine_res::strScienceMineExtraRate])
				mItem.ScienceMineExtraRate_ = obj[mine_res::strScienceMineExtraRate].Double();
			checkNotEoo(obj[mine_res::strRemainRobTimes])
				mItem.remainRobTimes_ = obj[mine_res::strRemainRobTimes].Int();
			checkNotEoo(obj[mine_res::strMineCD])
				mItem.mineCD_ = obj[mine_res::strMineCD].Int();
			checkNotEoo(obj[mine_res::strHasSeekMineCDHelp])
				mItem.hasSeekMineCDHelp_ = obj[mine_res::strHasSeekMineCDHelp].Bool();
			checkNotEoo(obj[mine_res::strHasSeekMineHelp])
				mItem.hasSeekMineHelp_ = obj[mine_res::strHasSeekMineHelp].Bool();
			checkNotEoo(obj[mine_res::strBeHelpMineTimes])
				mItem.beHelpMineTimes_ = obj[mine_res::strBeHelpMineTimes].Int();
			checkNotEoo(obj[mine_res::strRobCD])
				mItem.robCD_ = obj[mine_res::strRobCD].Int();
			checkNotEoo(obj[mine_res::strMaxReportId])
				mItem.maxReportId_ = obj[mine_res::strMaxReportId].Int();
			checkNotEoo(obj[mine_res::strNextUpdateTime])
				mItem.nextUpdateTime_ = obj[mine_res::strNextUpdateTime].Int();
			checkNotEoo(obj[mine_res::strGeneralScanTimes])
				mItem.generalScanTimes_ = obj[mine_res::strGeneralScanTimes].Int();
			checkNotEoo(obj[mine_res::strAdvanceScanTimes])
				mItem.advanceScanTimes_ = obj[mine_res::strAdvanceScanTimes].Int();
			checkNotEoo(obj[mine_res::strTotalBeRobSilver])
				mItem.totalBeRobSilver_ = obj[mine_res::strTotalBeRobSilver].Int();
			checkNotEoo(obj[mine_res::strTotalBeRobWeiwang])
				mItem.totalBeRobWeiwang_ = obj[mine_res::strTotalBeRobWeiwang].Int();

			if (mItem.mineCD_ > 0)
				mItem.ising_ = true;

			checkNotEoo(obj[mine_res::strReportList])
			{
				mItem.reportList_.clear();
				vector<BSONElement> sets = obj[mine_res::strReportList].Array();
				for (unsigned j = 0; j < sets.size(); j++)
				{
					BSONElement objReport = sets[j];
					mineReport mr;
					checkNotEoo(objReport[mine_res::strReportAtkName])
						mr.atkName = objReport[mine_res::strReportAtkName].String();
					checkNotEoo(objReport[mine_res::strReportDefName])
						mr.defName = objReport[mine_res::strReportDefName].String();
					checkNotEoo(objReport[mine_res::strReportAtkID])
						mr.atkID = objReport[mine_res::strReportAtkID].Int();
					checkNotEoo(objReport[mine_res::strReportDefID])
						mr.defID = objReport[mine_res::strReportDefID].Int();
					checkNotEoo(objReport[mine_res::strReportAtkLink])
						mr.atkReportLink = objReport[mine_res::strReportAtkLink].String();
					checkNotEoo(objReport[mine_res::strReportDefLink])
						mr.defReportLink = objReport[mine_res::strReportDefLink].String();
					checkNotEoo(objReport[mine_res::strReportResult])
						mr.result = objReport[mine_res::strReportResult].Int();
					checkNotEoo(objReport[mine_res::strReportTime])
						mr.battleTime = objReport[mine_res::strReportTime].Int();
					checkNotEoo(objReport[mine_res::strIsPlayer])
					{
						mr.isPlayer = sets[j][mine_res::strIsPlayer].Bool();
					}
					else
					{
						mr.isPlayer = true;
					}
					mItem.reportList_.push_back(mr);
				}
			}

			playerMineInfoMap_[player_id] = mItem;
		}
		if (playerMineInfoMap_[player_id].dailyRefresh())
		{
			savePlayerMineInfo(player_id, playerMineInfoMap_[player_id]);
		}
		mItem = playerMineInfoMap_[player_id];
		return true;
	}

	int mine_resource_system::getCurrentPage( int layer, int mineId)
	{
		if (layerMap_[layer].mineIDList_.size() == 0)
		{
			return 1;
		}
		if (mineId <= 0 || layer <= 0)
		{
			return 1;
		}

		int pageId;
		int curIndex = 0;
		vector<int>::iterator it = find(layerMap_[layer].mineIDList_.begin(), layerMap_[layer].mineIDList_.end(), mineId);
		if (it != layerMap_[layer].mineIDList_.end())
			curIndex = (it - layerMap_[layer].mineIDList_.begin());
		
		double fpage;
		fpage = double(curIndex + 1) / PAGE_SIZE;
		pageId = (int)ceil(fpage);

		return pageId;
	}

	int mine_resource_system::getMaxPage(int layer)
	{
		if (layerMap_[layer].mineIDList_.size() == 0)
			return 1;

		int maxPageId = 1;
		double fmaxpage;
		int firstMineId = 0;
		int lastMineId = layerMap_[layer].mineIDList_.size() - 1;
		fmaxpage = double(NPC_NUMBER + lastMineId - firstMineId + 1) / PAGE_SIZE;
		maxPageId = (int)ceil(fmaxpage);
		return maxPageId;
	}

// 	void mine_resource_system::getMineItemsByPage( int layer, int pageId, vector<mineItem>& mineList, vector<int>& npcList )
// 	{
// 		int startMineId = 0;
// 		int endMineId = layerMap_[layer].mineIDList_.size() - 1;
// 
// 		int startIndex = (pageId -1) * PAGE_SIZE + startMineId;
// 		int endIndex = (pageId -1) * PAGE_SIZE + startMineId + PAGE_SIZE - 1;
// 
// // 		if (endIndex > MAX_MINE_ID)
// // 		{
// // 			endIndex = startIndex -MAX_MINE_ID;
// // 		}
// 		int minEndMineId = (endIndex > endMineId ? endMineId : endIndex); 
// 		for (int i = minEndMineId; i >= startIndex; i--)
// 		{
// 			int mineid = i;
// 			int pid = getPlayerIdByLMMap(layer, mineid);
// 			if (pid < 0)
// 				continue;
// 
// 			mineItem mItem;
// 			if (!getPlayerMineInfo(pid, mItem))
// 				continue;
// 
// 			mineList.push_back(mItem);
// 		}
// 
// 		if (startIndex <= minEndMineId)
// 		{
// 			for (int i = minEndMineId - startIndex + 1; i < PAGE_SIZE; i++)
// 			{
// 				int npcID = i - (minEndMineId - startIndex);
// 				if (npcID > NPC_NUMBER)
// 					break;
// 				npcList.push_back(npcID);
// 			}
// 		}
// 		else
// 		{
// 			for (int i = startIndex - minEndMineId; i <= NPC_NUMBER; i++)
// 			{
// 				int npcID = i;
// 				npcList.push_back(npcID);
// 			}
// 		}
// 	}

	void mine_resource_system::getMineItemsByPage(int layer, int pageId, vector<mineItem>& mineList, vector<int>& npcList)
	{
		if (pageId < 1)
			return;

		int startIndex = (pageId - 1) * PAGE_SIZE;
		int endIndex = (pageId - 1) * PAGE_SIZE + PAGE_SIZE - 1;

		int totalSize = layerMap_[layer].mineIDList_.size();
		int minEndMineId = (endIndex > totalSize - 1 ? totalSize - 1 : endIndex);
		for (int i = minEndMineId; i >= startIndex; i--)
		{
			int mineid = layerMap_[layer].mineIDList_[i];
			int pid = getPlayerIdByLMMap(layer, mineid);
			if (pid < 0)
				continue;

			mineItem mItem;
			if (!getPlayerMineInfo(pid, mItem))
				continue;

			mineList.push_back(mItem);
		}

		if (startIndex <= minEndMineId)
		{
			for (int i = minEndMineId - startIndex + 1; i < PAGE_SIZE; i++)
			{
				int npcID = i - (minEndMineId - startIndex);
				if (npcID > NPC_NUMBER)
					break;
				npcList.push_back(npcID);
			}
		}
		else
		{
			for (int i = startIndex - minEndMineId; i <= NPC_NUMBER; i++)
			{
				int npcID = i;
				npcList.push_back(npcID);
			}
		}
	}

	int mine_resource_system::mineId2vecIndx( int layer, int mineId )
	{
// 		if (layerMap_[layer].size() > 0)
// 		{
// 			int firstMineId = layerMap_[layer][0].mineID;
// 			int lastMineId = lastMineIdMap_[layer];
// 			if (firstMineId <= mineId)
// 			{
// 				return mineId - firstMineId;
// 			}
// 			else if (mineId >= lastMineId)
// 			{
// 				return -1;
// 			}
// 			else
// 			{
// 				return mineId + MAX_MINE_ID - firstMineId;
// 			}
// 		}
		return -1;
	}

	int mine_resource_system::level2layer(int level)
	{
		if (level >= MINE_RES_OPEN_LEVEL && level <= 30)
		{
			return LEVEL_20_30_LAYER;
		}
		if (level >= 31 && level <= 40)
		{
			return LEVEL_31_40_LAYER;
		}
		else if (level >= 41 && level <= 60)
		{
			return LEVEL_41_60_LAYER;
		}
		else if (level >= 61 && level <= 80)
		{
			return LEVEL_61_80_LAYER;
		}
		else if (level >= 81 && level <= 100)
		{
			return LEVEL_81_100_LAYER;
		}
		else if (level >= 101 && level <= 120)
		{
			return LEVEL_101_120_LAYER;
		}
		else
		{
			return -1;
		}
	}

	int mine_resource_system::npcid2layer(int npcid)
	{
		return(npcid - START_NPC_ID) / 5;
	}

	void mine_resource_system::createMineItem(playerDataPtr pdata, mineItem& mItem)
	{
		if (pdata == playerDataPtr())
		{
			return;
		}

		mItem.player_id_ = pdata->playerID;
		mItem.playerLevel_ = pdata->Base.getLevel();
		playerMineInfoMap_[pdata->playerID] = mItem;
	}

	void mine_resource_system::createMineNpcItem(mapDataPtr mData, mineItem& mItem, playerDataPtr pdata)
	{
		mItem.quality_ = 1;
		mItem.mineID_ = mData->localID;
		mItem.playerLevel_ = mData->mapLevel;
		mItem.actExtraRate_ = activity_sys.getRate(activity::_mine_battle, pdata);
	}

	int mine_resource_system::getNewMineId(int layer)
	{
		int i = 1;
		int count = 0;
		do 
		{
			count++;
			vector<int>::iterator it = find(layerMap_[layer].mineIDList_.begin(), layerMap_[layer].mineIDList_.end(), i);
			if (it != layerMap_[layer].mineIDList_.end())
			{
				i++;
				continue;
			}
			else
			{
				return i;
			}
		} while (count < 2000);
		return -1;
	}

	double mine_resource_system::getRobExtraRate( playerDataPtr pdata )
	{
		int gid = pdata->Guild.getGuildID();
		Guild::guildPtr gPtr = guild_sys.getGuild(gid);
		double robRate = 0.0;
		if (gPtr != Guild::guildPtr())
		{
			robRate = double(gPtr->getScienceData(playerGuild::privateer_license).ADD) / 10000.0;
		}
		return robRate;
	}

	double mine_resource_system::getMineExtraRate(playerDataPtr pdata)
	{
		int gid = pdata->Guild.getGuildID();
		Guild::guildPtr gPtr = guild_sys.getGuild(gid);
		double mineRate = 0.0;
		if (gPtr != Guild::guildPtr())
		{
			mineRate = double(gPtr->getScienceData(playerGuild::mine_refine).ADD) / 10000.0;
		}
		return mineRate;
	}

	bool mine_resource_system::clearPlayerMineCD(playerDataPtr player, unsigned s, bool& NoData)
	{
		NoData = false;
		mineItem mItem;
		int cur_time = na::time_helper::get_current_time();
		if (getPlayerMineInfo(player, mItem))
		{
			if (cur_time >= mItem.mineCD_)
				return true;
			mItem.mineCD_ -= s;
			mItem.getHasSeekMineCDHelp();
			savePlayerMineInfo(player->playerID, mItem);
			if (cur_time >= mItem.mineCD_)
			{
				return true;
			}
			else
				return false;
		}
		else
		{
			NoData = true;
			return true;
		}
	}

	bool mine_resource_system::clearPlayerMineCD(playerDataPtr player, unsigned s)
	{
		bool NoData = false;
		return clearPlayerMineCD(player, s, NoData);
	}

	bool mine_resource_system::getRewardTime(unsigned &start_time, unsigned &end_time)
	{
		unsigned cur_time = na::time_helper::get_current_time();

		boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
		tm t = boost::posix_time::to_tm(p);

		unsigned stand_time1 = cur_time - 3600 * t.tm_hour - 60 * t.tm_min - t.tm_sec;
		unsigned stand_time2 = stand_time1 + 5 * 3600;

		unsigned start_time1 = stand_time1 + 20 * 3600;
		unsigned end_time1 = start_time1 + 60 * 60;

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

	mineItem::mineItem()
	{
		player_id_ = -1;
		playerLevel_ = -1;
		mineID_ = -1;
		maxReportId_ = 0;
		beRobTimes_ = 0;
		totalBeRobSilver_ = 0;
		totalBeRobWeiwang_ = 0;
		remainMineTimes_ = MAX_MINE_TIMES;
		startMineTime_ = 0;
		actExtraRate_ = 0;
		ScienceMineExtraRate_ = 0;
		remainRobTimes_ = 5;
		mineCD_ = 0;
		hasSeekMineCDHelp_ = false;
		hasSeekMineHelp_ = false;
		beHelpMineTimes_ = 0;
		robCD_ = 0;
		generalScanTimes_ = 0;
		advanceScanTimes_ = 0;
		quality_ = -1;
		playerLevel_ = -1;
		ising_ = false;
	}

	void mineItem::reset()
	{
		mineID_ = -1;
		quality_ = -1;
		playerLevel_ = -1;
		startMineTime_ = 0;
		ScienceMineExtraRate_ = 0;
		actExtraRate_ = 0.0f;
		beRobTimes_ = 0;
		totalBeRobSilver_ = 0;
		totalBeRobWeiwang_ = 0;
		hasSeekMineCDHelp_ = false;
		hasSeekMineHelp_ = false;
		beHelpMineTimes_ = 0;
		mineCD_ = 0;
		generalScanTimes_ = 0;
		advanceScanTimes_ = 0;
		ising_ = false;
	}

	bool mineItem::getHasSeekMineCDHelp()
	{
		int cur_time = na::time_helper::get_current_time();
		if (cur_time >= mineCD_)
		{
			hasSeekMineCDHelp_ = false;
			if (mineCD_ > 0)
				mine_res_sys.savePlayerMineInfo(this->player_id_, *this);
		}
		return hasSeekMineCDHelp_;
	}

	void mineItem::setHasSeekMineCDHelp(bool bVal)
	{
		hasSeekMineCDHelp_ = bVal;
		mine_res_sys.savePlayerMineInfo(this->player_id_, *this);
	}

	bool mineItem::getHasSeekMineHelp()
	{
		int cur_time = na::time_helper::get_current_time();
		if (hasSeekMineHelp_ && cur_time >= mineCD_)
		{
			hasSeekMineHelp_ = false;
			mine_res_sys.savePlayerMineInfo(this->player_id_, *this);
		}
		return hasSeekMineHelp_;
	}

	void mineItem::setHasSeekMineHelp(bool bVal)
	{
		hasSeekMineHelp_ = bVal;
		mine_res_sys.savePlayerMineInfo(this->player_id_, *this);
	}
	
	int mineItem::getBeHelpMineTimes()
	{
		int cur_time = na::time_helper::get_current_time();
		if (beHelpMineTimes_ != 0 && cur_time >= mineCD_)
		{
			beHelpMineTimes_ = 0;
			mine_res_sys.savePlayerMineInfo(this->player_id_, *this);
		}
		return beHelpMineTimes_;
	}

	void mineItem::alterBeHelpMineTimes(int num)
	{		
		getBeHelpMineTimes();//修改前是否需要刷新次数？
		beHelpMineTimes_ += num;
		if (beHelpMineTimes_ < 0)
			beHelpMineTimes_ = 0;

		mine_res_sys.savePlayerMineInfo(this->player_id_, *this);
	}
	
	bool mineItem::dailyRefresh()
	{
		int cur_time = na::time_helper::get_current_time();
		if (cur_time >= nextUpdateTime_)
		{
			beRobTimes_ = 0;
			totalBeRobSilver_ = 0;
			totalBeRobWeiwang_ = 0;
			remainMineTimes_ = MAX_MINE_TIMES;
			remainRobTimes_ = 5;

			boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
			tm t = boost::posix_time::to_tm(p);
			int standard_time = cur_time - 3600*t.tm_hour - 60*t.tm_min - t.tm_sec;
			if (t.tm_hour >= 5)
			{
				nextUpdateTime_ = int(na::time_helper::get_next_time(standard_time, 1, 5));
			}
			else
			{
				nextUpdateTime_ = int(na::time_helper::get_next_time(standard_time, 0, 5));
			}
			return true;
		}
		return false;
	}

	void mineItem::packageLargeBaseInfo(playerDataPtr pdata, Json::Value &pack, bool isScienceMineInflu, bool isScienceRobInflu, double mineCoeff /*= 0.0f*/, double robCoeff /*= 0.0f*/)
	{
		pack[playerLevelStr] = pdata->Base.getLevel();
		pack[playerNameStr] = pdata->Base.getName();
		pack[playerFaceIDStr] = pdata->Base.getFaceID();
		pack[playerSphereIDStr] = pdata->Base.getSphereID();
		pack[playerNameStr] = pdata->Base.getName();
		pack["bv"] = pdata->Embattle.getCurrentBV();

		Json::Value guild_info = guild_sys.getPersonal(pdata);
		pack["gna"] = guild_info[guildNameStr].asString();;
		pack[mine_res::strMineId] = mineID_;
		pack[mine_res::strQuality] = quality_;
		pack[mine_res::strBeRobTimes] = beRobTimes_;
		pack[mine_res::strRemainMineTimes] = remainMineTimes_;
		pack[mine_res::strRemainRobTimes] = remainRobTimes_;
		pack[mine_res::strGeneralScanTimes] = generalScanTimes_;
		pack[mine_res::strMineCD] = mineCD_;
//		pack[mine_res::strHasSeekMineCDHelp] = getHasSeekMineCDHelp();
		pack[mine_res::strHasSeekMineHelp] = getHasSeekMineHelp();
		pack[mine_res::strBeHelpMineTimes] = getBeHelpMineTimes();
		pack[mine_res::strRobCD] = robCD_;
		pack[mine_res::strGeneralScanCost] = getGeneralScanCost();
		pack[mine_res::strAdvanceScanCost] = getAdvanceScanCost();
		pack[mine_res::strMineRewardSilver] = getMineRewardSilver(pdata, this->playerLevel_, true, false, isScienceMineInflu, isScienceRobInflu, mineCoeff, robCoeff);
		pack[mine_res::strMineRewardExtraSilver] = getMineRewardSilver(pdata, this->playerLevel_, false, true, false, false);
		pack[mine_res::strMineRewardWeiwang] = getMineRewardWeiwang(pdata, this->playerLevel_, true, false, isScienceMineInflu, isScienceRobInflu, mineCoeff, robCoeff);
		pack[mine_res::strMineRewardExtraWeiwang] = getMineRewardWeiwang(pdata, this->playerLevel_, false, true, false, false);
	}

	void mineItem::packageLargeNpcBaseInfo(playerDataPtr pdata, mapDataPtr& mData, Json::Value &pack, bool isScienceMineInflu, bool isScienceRobInflu, double robCoeff/* = 0.0f*/)
	{
		pack[playerLevelStr] = mData->mapLevel;
		pack[WAR::warStoryLocalIDStr] = mData->localID;
		pack[playerFaceIDStr] = mData->faceID;
		pack[playerSphereIDStr] = 3;
//		pack["bv"] = mData->mapValue;
		pack["bv"] = -1;

		Json::Value guild_info = guild_sys.getPersonal(pdata);
		pack["gna"] = "";
		pack[mine_res::strMineId] = mineID_;
		pack[mine_res::strQuality] = quality_;
		pack[mine_res::strBeRobTimes] = beRobTimes_;
		pack[mine_res::strRemainMineTimes] = remainMineTimes_;
		pack[mine_res::strRemainRobTimes] = 99999;
		pack[mine_res::strMineCD] = mineCD_;
//		pack[mine_res::strHasSeekMineCDHelp] = getHasSeekMineCDHelp();
		pack[mine_res::strHasSeekMineHelp] = getHasSeekMineHelp();
		pack[mine_res::strBeHelpMineTimes] = getBeHelpMineTimes();
		pack[mine_res::strRobCD] = robCD_;
		pack[mine_res::strGeneralScanCost] = getGeneralScanCost();
		pack[mine_res::strAdvanceScanCost] = getAdvanceScanCost();

		pack[mine_res::strMineRewardSilver] = getMineRewardSilver(pdata, this->playerLevel_, true, false, isScienceMineInflu, isScienceRobInflu, 0.0f, robCoeff);
		pack[mine_res::strMineRewardExtraSilver] = getMineRewardSilver(pdata, this->playerLevel_, false, true, false, false);
		pack[mine_res::strMineRewardWeiwang] = getMineRewardWeiwang(pdata, this->playerLevel_, true, false, isScienceMineInflu, isScienceRobInflu, 0.0f, robCoeff);
		pack[mine_res::strMineRewardExtraWeiwang] = getMineRewardWeiwang(pdata, this->playerLevel_, false, true, false, false);
	}

	void mineItem::packageLittleBaseInfo(playerDataPtr pdata, Json::Value &pack)
	{
		pack[playerLevelStr] = playerLevel_;
		pack[playerNameStr] = pdata->Base.getName();
		pack[playerSphereIDStr] = pdata->Base.getSphereID();
		pack["bv"] = pdata->Embattle.getCurrentBV();
		pack[mine_res::strMineId] = mineID_;
		pack[mine_res::strQuality] = quality_;
		pack[mine_res::strBeRobTimes] = beRobTimes_;
		pack[mine_res::strRemainMineTimes] = remainMineTimes_;
		pack[mine_res::strRemainRobTimes] = remainRobTimes_;
		pack[mine_res::strMineCD] = mineCD_;
//		pack[mine_res::strHasSeekMineCDHelp] = getHasSeekMineCDHelp();
		pack[mine_res::strHasSeekMineHelp] = getHasSeekMineHelp();
		pack[mine_res::strBeHelpMineTimes] = getBeHelpMineTimes();
		pack[mine_res::strRobCD] = robCD_;
	}

	void mineItem::packageReport(Json::Value &pack)
	{
		pack = Json::arrayValue;
		for (vector<mineReport>::iterator it = reportList_.begin(); it != reportList_.end(); it++)
		{
			Json::Value temp;
			it->package(temp);
			pack.append(temp);
		}
	}

	int mineItem::scanMine(int scan_option)
	{
		mineConfig mcfg = mine_res_sys.getConfig();
		if (scan_option == scan_null)
		{
			if (quality_ == -1)
			{
				quality_ = scanMine(scan_general);
				return quality_;
			}
			else
			{
				return quality_;
			}
		}
		else if (scan_option == scan_general)
		{
			int scan_rate = commom_sys.randomBetween(0, RANGE-1);
			int scan_idx;
			for (unsigned idx = 1; idx <= MINE_TYPE; idx++)
			{
				if (scan_rate < mcfg.mineScan[idx-1].generalScanRate)
				{
					scan_idx = idx;
					quality_ = scan_idx;
					return scan_idx;
				}
			}
			return -1;
		}
		else if (scan_option == scan_advance)
		{
			int scan_rate = commom_sys.randomBetween(0, RANGE-1);
			int scan_idx;
			for (unsigned idx = 1; idx <= MINE_TYPE; idx++)
			{
				if (scan_rate < mcfg.mineScan[idx-1].advanceScanRate)
				{
					scan_idx = idx;
					quality_ = scan_idx;
					return scan_idx;
				}
			}
			return -1;
		}
		else
		{
			return -1;
		}
	}

	string mineItem::getReportName(playerDataPtr pdata)
	{
		string dirname = mine_res::strReportMineResDir;
		dirname += boost::lexical_cast<string , int>(pdata->playerID);
		boost::filesystem::path path_dir(dirname);
		if(!boost::filesystem::exists(dirname))
			boost::filesystem::create_directory(dirname);

		string sfile = boost::lexical_cast<string, int>(maxReportId_);
		maxReportId_++;
		if (maxReportId_ > MAX_REPORT_ID)
		{
			maxReportId_ = 0;
		}
		string dir = "";
		dir = dir + boost::lexical_cast<string, int>(pdata->playerID) + "/";
		return dir + sfile;
	}

	bool mineItem::isGainTime()
	{
		if (mineCD_ <= 0)
		{
			return false;
		}
		int cur_time = na::time_helper::get_current_time();
		if (cur_time >= mineCD_)
		{
			return true;
		}
		return false;
	}

	bool mineItem::isMining()
	{
		int cur_time = na::time_helper::get_current_time();
		if (cur_time < mineCD_)
		{
			return true;
		}
		return false;

	}

	void mineItem::gain( playerDataPtr pdata, int cur_time )
	{
		int totalSilver = getMineRewardSilver(pdata, playerLevel_, true, false, true, false, ScienceMineExtraRate_)
			+ getMineRewardSilver(pdata, playerLevel_, false, true, false, false);
		cur_time = 0;//全部结算
		if (cur_time != 0)
		{
			if (cur_time < mineCD_)
			{
				totalSilver = int(double(cur_time - mineCD_ + MINE_CD * 60) / (MINE_CD * 60) * totalSilver);
			}
		}

//		int loseSilver = int(getMineRewardSilver(playerLevel_, true, false, false, false) * 0.15 * beRobTimes_);
		int loseSilver = this->totalBeRobSilver_;
		int gainSilver = int(totalSilver - loseSilver);
		int beHelpMineTime = this->beHelpMineTimes_;//用没刷新过的值
		if (gainSilver < 0)
			gainSilver = 0;
		int helpSilver = int(getMineRewardSilver(pdata, playerLevel_, true, false, false, false) * 0.01 * beHelpMineTime);

		int totalWeiwang = getMineRewardWeiwang(pdata, playerLevel_, true, false, true, false, ScienceMineExtraRate_)
			+ getMineRewardWeiwang(pdata, playerLevel_, false, true, false, false);
		if (cur_time != 0)
		{
			if (cur_time < mineCD_)
			{
				totalWeiwang = int(double(cur_time - mineCD_ + MINE_CD * 60) / (MINE_CD * 60) * totalWeiwang);
			}
		}

//		int loseWeiwang = int(getMineRewardWeiwang(playerLevel_, true, false, false, false) * 0.15 * beRobTimes_);
		int loseWeiwang = this->totalBeRobWeiwang_;
		int gainWeiwang = int(totalWeiwang - loseWeiwang);
		if (gainWeiwang < 0)
			gainWeiwang = 0;
		int helpWeiwang = int(getMineRewardWeiwang(pdata, playerLevel_, true, false, false, false) * 0.01 * beHelpMineTime);

		pdata->Base.alterSilver(gainSilver+helpSilver);
		pdata->Base.alterWeiWang(gainWeiwang+helpWeiwang);

		Json::Value gainJson, attachJson;
		gainJson[mine_res::strTotalSilver] = totalSilver;
		gainJson[mine_res::strTotalWeiwang] = totalWeiwang;
		gainJson[mine_res::strLoseSilver] = loseSilver;
		gainJson[mine_res::strLoseWeiwang] = loseWeiwang;
		gainJson[mine_res::strGainSilver] = gainSilver+helpSilver;
		gainJson[mine_res::strGainWeiwang] = gainWeiwang+helpWeiwang;
		gainJson[mine_res::strHelpSilver] = helpSilver;
		gainJson[mine_res::strHelpWeiwang] = helpWeiwang;
		gainJson[mine_res::strBeHelpMineTimes] = beHelpMineTime;
		gainJson[mine_res::strBeRobTimes] = beRobTimes_;
		email_sys.sendSystemEmailCommon(pdata, email_type_system_ordinary, email_team_system_mine_res_gain, gainJson, attachJson);

		cur_time = na::time_helper::get_current_time();
		
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(this->beRobTimes_));
		fields.push_back(boost::lexical_cast<string, int>(gainSilver));
		fields.push_back(boost::lexical_cast<string, int>(gainWeiwang));
		int startMineTime = this->startMineTime_;
		gm_tools_mgr.actTimePretreat(startMineTime);
		fields.push_back(boost::lexical_cast<string, int>(startMineTime));
		StreamLog::Log(mine_res::mysqlLogMineRes, pdata, boost::lexical_cast<string, int>(cur_time - na::time_helper::timeZone() * 3600), 
			boost::lexical_cast<string, int>(this->mineID_), fields, mine_res_log_mine_result);

		reset();
	}

	int mineItem::beginToMine(int layer, playerDataPtr pdata)
	{
		int cur_time = na::time_helper::get_current_time();
		mineCD_ = cur_time + MINE_CD * 60;
		remainMineTimes_--;
		playerLevel_ = pdata->Base.getLevel();
		startMineTime_ = cur_time;
		int newId = mine_res_sys.getNewMineId(layer);
		mineID_ = newId;
//		actExtraRate_ = 0;
		actExtraRate_ = activity_sys.getRate(activity::_mine_battle, pdata);
		ising_ = true;
		ScienceMineExtraRate_ = mine_res_sys.getMineExtraRate(pdata);
		return 0;
	}

	void mineReport::package(Json::Value& pack)
	{
		pack[mine_res::strReportAtkName] = atkName;
		pack[mine_res::strReportDefName] = defName;
		pack[mine_res::strReportAtkID] = atkID;
		pack[mine_res::strReportDefID] = defID;
		pack[mine_res::strReportAtkLink] = atkReportLink;
		pack[mine_res::strReportDefLink] = defReportLink;
		pack[mine_res::strReportResult] = result;
		pack[mine_res::strIsPlayer] = isPlayer;
		pack[mine_res::strReportTime] = battleTime;
	}

	mineReport::mineReport()
	{
		atkID = 0;
		defID = 0;
	}

	void mineItem::insertReport(mineReport& mReport)
	{
		reportList_.insert(reportList_.begin(), mReport);
		if (reportList_.size() > MAX_REPORT_ID)
		{
			reportList_.pop_back();
		}
	}

	int mineItem::getMineRewardSilver(playerDataPtr pdata, int level, bool isAddBase, bool isActInflu, bool isScienceMineInflu, bool isScienceRobInflu, double mineCoeff /*= 0.0f*/, double robCoeff /*= 0.0f*/)
	{
		if (quality_ < 1 || quality_ > MINE_TYPE)
		{
			return 0;
		}
		mineConfig mcfg = mine_res_sys.getConfig();
//		int rewardSilver = int((level + 50) * mcfg.baseSilver * mcfg.mineScan[quality_-1].resCoeff);
		//挖矿星币获得=int(（主角等级*星币基础书）^2.2/40*矿源系数）
//		int rewardSilver = int(pow(level*mcfg.baseSilver, 2.2) / 40 * mcfg.mineScan[quality_-1].resCoeff);

		//挖矿获得星币=主角等级*星币基础书*矿源系数
		double actExtraRate = 0;
		if (isActInflu)
		{
			if (isMining())
			{
				actExtraRate = actExtraRate_;
			}
			else
			{
				actExtraRate = activity_sys.getRate(activity::_mine_battle, pdata);
			}
			if (ising_)
			{
				actExtraRate = actExtraRate_;
			}
		}
		double mineExtraRate = isScienceMineInflu ? mineCoeff : 0;
		double robExtraRate = isScienceRobInflu ? robCoeff : 0;
		int addBase = isAddBase ? 1 : 0;
// 		int rewardSilver = int(level * mcfg.baseSilver * mcfg.mineScan[quality_-1].resCoeff
// 			* (addBase + actExtraRate + mineExtraRate + robExtraRate));
		int rewardSilver = int((level * mcfg.mineScan[quality_ - 1].resCoeff + mcfg.baseSilver)
			* (addBase + actExtraRate + mineExtraRate + robExtraRate));
		return rewardSilver;
	}

	int mineItem::getMineRewardWeiwang(playerDataPtr pdata, int level, bool isAddBase, bool isActInflu, bool isScienceMineInflu, bool isScienceRobInflu, double mineCoeff /*= 0.0f*/, double robCoeff /*= 0.0f*/)
	{
		if (quality_ < 0)
		{
			return 0;
		}
		mineConfig mcfg = mine_res_sys.getConfig();
//		int rewardWeiwang = int((level + 20) * mcfg.baseWeiwang * mcfg.mineScan[quality_-1].resCoeff);
		//挖矿声望获得=int((主角等级+声望基础数）^2.2/30*矿源系数))
//		int rewardWeiwang = int(pow(level+mcfg.baseWeiwang, 2.2) / 30 * mcfg.baseWeiwang);
		//挖矿获得威望=主角等级*威望基础书*矿源系数
		double actExtraRate = 0;
		if (isActInflu)
		{
			if (isMining())
			{
				actExtraRate = actExtraRate_;
			}
			else
			{
				actExtraRate = activity_sys.getRate(activity::_mine_battle, pdata);
			}
			if (ising_)
			{
				actExtraRate = actExtraRate_;
			}
		}

		double mineExtraRate = isScienceMineInflu ? mineCoeff : 0;
		double robExtraRate = isScienceRobInflu ? robCoeff : 0;
		int addBase = isAddBase ? 1 : 0;
// 		int rewardWeiwang = int(level * mcfg.baseWeiwang * mcfg.mineScan[quality_-1].resCoeff
// 			* (addBase + actExtraRate + mineExtraRate + robExtraRate));
		int rewardWeiwang = int((level * mcfg.mineScan[quality_ - 1].resCoeff / 30+ mcfg.baseWeiwang)
			* (addBase + actExtraRate + mineExtraRate + robExtraRate));
		return rewardWeiwang;
	}

	int mineItem::getGeneralScanCost()
	{
		int costSilver = 1000*(generalScanTimes_+1);
		return costSilver;
	}

	int mineItem::getAdvanceScanCost()
	{
//		int costGold = 2*(advanceScanTimes_+1);
		int costGold = 10;
		return costGold;
	}

	layerItem::layerItem()
	{
	}

	mineAnnouceBroadCast::mineAnnouceBroadCast()
	{
		br_12_30_ = false;
		br_20_00_ = false;

		nextUpdateTime_ = 0;
	}

	void mineAnnouceBroadCast::reset()
	{
		br_12_30_ = false;
		br_20_00_ = false;
	}

	void mineAnnouceBroadCast::refresh()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > nextUpdateTime_)
		{
			reset();
			nextUpdateTime_ = na::time_helper::get_next_update_time(cur_time);
		}
	}

}



