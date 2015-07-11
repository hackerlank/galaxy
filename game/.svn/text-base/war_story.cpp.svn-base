#include "war_story.h"
#include "battle_system.h"
#include "mongoDB.h"
#include "file_system.h"
#include "pilotManager.h"
#include "response_def.h"
#include <boost/lexical_cast.hpp>
#include "player_war_story.h"
#include "playerManager.h"
#include "gate_game_protocol.h"
#include "gate_game_protocol.h"
#include "chat_system.h"
#include "translate.h"
#include "task_system.h"
#include "action_instance.hpp"
#include "activity_system.h"
#include "activity_game_param.h"
#include  "record_system.h"
#include "email_system.h"
#include "gm_tools.h"
#include "system_response.h"

#define WAR_STORY_TEAM_OPEN_LEVEL 9
#define SWEEP_ORDER_ID 13009

namespace gg
{
	using namespace WAR;
	warStory* const warStory::warSys = new warStory();

	const static string logWarRewardBox = "log_reward_box";
	const static string logWarTeamSweepLog = "log_war_story_sweep";
	const static string BattleNpcMongoDB = "gl.multi_battle_npc";
	void warStory::pushBattleNpc(const int localID, battleSidePtr npc)
	{
		if (npc == NULL || !isMatchNpcList(localID))return;
		if (!npc->isPlayerSide())return;//不是玩家数据
		battleSidePtr newPtr = npc->copyState();
		if (newPtr == NULL)return;
		newPtr->resetState();//重置战斗数据
		OneVec& oM = npcMap[localID];
		//remove
		for (unsigned i = 0; i < oM.size(); ++i)
		{
			if (npc->getSideID() == oM[i]->getSideID())
			{
				oM.erase(oM.begin() + i);
				break;
			}
		}
		//insert and save
		{
			oM.insert(oM.begin(), newPtr);
			mongo::BSONObj key = BSON("nid" << localID << "pi" << npc->getSideID());
			db_mgr.save_mongo(BattleNpcMongoDB, key, newPtr->stateToBson(localID));
		}

		if (oM.size() > 20)
		{
			mongo::BSONObj key = BSON("nid" << localID << "pi" << oM.back()->getSideID());
			db_mgr.remove_collection(BattleNpcMongoDB, key);
			oM.pop_back();
		}
	}

	void warStory::chaper1RewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);		
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		warStoryMap& cMap = chaperRewardMap[0];
		warStoryMap::iterator it = cMap.find(localID);
		if (it == cMap.end())Return(r, -1);
		mapDataPtr mPtr =  it->second;
		int mapID = localID / mapIDOffset;
		playerWarStoryPtr ptr = d->Warstory.getMap(mapID);
		if(ptr == NULL)Return(r, -1);
		if(!ptr->isProcessLocal(localID))Return(r, 1);
		if(ptr->Chaper1RW())Return(r, 2);
		int star = ptr->getStarNum();
		if(star < mPtr->frontLocalID)Return(r, 3);
		actionResult resAction = actionFormat::actionDo(d, mPtr->actionVec);
		r[msgStr][1u][errorActionJson::breakIDStr] = resAction.breakID;
		r[msgStr][1u][errorActionJson::breakCodeStr] = resAction.resultCode;
		if(resAction.OK())ptr->SetChaper1RW(true);
		StreamLog::Log(logWarRewardBox, d, localID, star, 0);
		Return(r, 0);
	}

	void warStory::chaper2RewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);		
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		warStoryMap& cMap = chaperRewardMap[1];
		warStoryMap::iterator it = cMap.find(localID);
		if (it == cMap.end())Return(r, -1);
		mapDataPtr mPtr =  it->second;
		int mapID = localID / mapIDOffset;
		playerWarStoryPtr ptr = d->Warstory.getMap(mapID);
		if(ptr == NULL)Return(r, -1);
		if(!ptr->isProcessLocal(localID))Return(r, 1);
		if(ptr->Chaper2RW())Return(r, 2);
		int star = ptr->getStarNum();
		if (star < mPtr->frontLocalID)Return(r, 3);
		actionResult resAction = actionFormat::actionDo(d, mPtr->actionVec);
		r[msgStr][1u][errorActionJson::breakIDStr] = resAction.breakID;
		r[msgStr][1u][errorActionJson::breakCodeStr] = resAction.resultCode;
		if(resAction.OK())ptr->SetChaper2RW(true);
		StreamLog::Log(logWarRewardBox, d, localID, star, 1);
		Return(r, 0);
	}

	void warStory::chaper3RewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		warStoryMap& cMap = chaperRewardMap[2];
		warStoryMap::iterator it = cMap.find(localID);
		if (it == cMap.end())Return(r, -1);
		mapDataPtr mPtr = it->second;
		int mapID = localID / mapIDOffset;
		playerWarStoryPtr ptr = d->Warstory.getMap(mapID);
		if (ptr == NULL)Return(r, -1);
		if (!ptr->isProcessLocal(localID))Return(r, 1);
		if (ptr->Chaper3RW())Return(r, 2);
		int star = ptr->getStarNum();
		if (star < mPtr->frontLocalID)Return(r, 3);
		actionResult resAction = actionFormat::actionDo(d, mPtr->actionVec);
		r[msgStr][1u][errorActionJson::breakIDStr] = resAction.breakID;
		r[msgStr][1u][errorActionJson::breakCodeStr] = resAction.resultCode;
		if (resAction.OK())ptr->SetChaper3RW(true);
		StreamLog::Log(logWarRewardBox, d, localID, star, 1);
		Return(r, 0);
	}

	void warStory::warStoryUpdate(msg_json& m, Json::Value& r){
		AsyncGetPlayerData(m);		
		ReadJsonArray;
		int mapID = js_msg[0u].asInt();
		int res = d->Warstory.updateClientMap(mapID);
		if(res != 0){
			r[msgStr][0u] = 0;
			r[msgStr][1u][updateFromStr] = m._type;
			r[msgStr][1u][playerWarStoryMapIDStr] = mapID;
			r[msgStr][1u][playerWarStoryDataStr] = Json::arrayValue;
			//r[msgStr][1u][playerMapRewardStr] = false;
			r[msgStr][1u][playerChaperRewardStr].append(false);
			r[msgStr][1u][playerChaperRewardStr].append(false);
			r[msgStr][1u][playerChaperRewardStr].append(false);
		}
	}

	void warStory::battleWithNpc(msg_json& m, Json::Value& r){
		AsyncGetPlayerData(m);
		if(d->isOperatBattle())Return(r, 3); //等待战报反馈
  		if(d->Campaign.getJunling() < 1)Return(r, 4); //军令不足	(补给)
  		if(d->Campaign.getCD().getCD().Lock)Return(r, 5);//军令cd中
		if(d->Embattle.isNullFormation())	Return(r, 1);//空阵
		ReadJsonArray;
		int localID = js_msg[0u].asInt();

		warStoryMap::const_iterator it = map_.find(localID);
		if(it == map_.end())Return(r, -1);

		mapDataPtr mD = it->second;
		int frontID = mD->frontLocalID;
		if(frontID >= 0 && ! d->Warstory.isProcessMap(frontID))
			Return(r, 2);//还没有打过这张地图

		battleSidePtr npcSide = battleSide::Create(mD);
		if(npcSide == battleSidePtr())Return(r, -1);

		battleSidePtr playerSide = battleSide::Create(d);
		if(playerSide == battleSidePtr())Return(r, -1);

		Battlefield field;
		field.atkSide = playerSide;
		field.defSide = npcSide;
		field.ParamJson[BATTLE::strBackGround] = mD->backGround;

		battle_sys.PostBattle(field, battle_player_vs_npc_type);
		d->Warstory.SetLastWSInfo(localID, mD->mapName);
		if(d->Warstory.isProcessMap(CDCreateMap))d->Campaign.getCD().addCD(60);
		activity_sys.updateActivity(d, 0, activity::_war_story);
		task_sys.updateBranchTaskInfo(d, _task_other_format);
	}

	bool warStory::isProcessMap(playerDataPtr player, const int mapID)
	{
		if(mapID < 0)return true;
		completeMap::iterator it = cmap_.find(mapID);
		if(it == cmap_.end())return false;
		return player->Warstory.isProcessMap(it->second);
	}

	vector<int> warStory::getAllMapID()
	{
		return mapIDVec_;
	}

	vector<int> warStory::getAllTeamMapID()
	{
		return mapIDTeamVec_;
	}

	vector<int> warStory::getAllMapLocalID()
	{
		vector<int> allLocalId;
		for(warStoryMap::iterator iter = map_.begin(); iter != map_.end(); ++iter)
			allLocalId.push_back(iter->first);
		return allLocalId;
	}

	mapDataPtr warStory::getMapData(const int localID)
	{
		warStoryMap::iterator it = map_.find(localID);
		if(it == map_.end())return mapDataPtr();
		return it->second;
	}

	teamMapDataPtr warStory::getTeamMapData(const int localID)
	{
		teamWarStoryMap::iterator it = tmap_.find(localID);
		if(it == tmap_.end())return teamMapDataPtr();
		return it->second;
	}

#define checkNotNullFomatInt(val ,json)\
	if(! json.isNull()){\
	val = json.asInt();\
	}

#define checkNotNullFomatDouble(val ,json)\
	if(! json.isNull()){\
	val = json.asDouble();\
	}

	gg::actionVector warStory::formationAction( Json::Value& dataJson, int actionOpt )
	{
		Json::Value actionJson = Json::arrayValue;
		if (!dataJson[warStoryPlayerExpStr].isNull())
		{
			Json::Value temp;
			temp[ACTION::strActionID] = action_add_role_exp;
			temp[ACTION::strValue] = dataJson[warStoryPlayerExpStr].asInt();
			actionJson.append(temp);
		}
		if (!dataJson[warStoryPilotExpStr].isNull())
		{
			if(0 == actionOpt)
			{
				Json::Value temp;
				temp[ACTION::strActionID] = action_deal_war;
				temp[ACTION::strValue] = dataJson[warStoryPilotExpStr].asInt();
				actionJson.append(temp);
			}
			Json::Value temp;
			temp[ACTION::strActionID] = action_add_mutil_pilot_exp;
			temp[ACTION::strValue] = dataJson[warStoryPilotExpStr].asInt();
			actionJson.append(temp);
		}
		if (!dataJson[warStoryRewardJunGongStr].isNull())
		{
			Json::Value temp;
			if (actionOpt == 0)
			{
				temp[ACTION::strActionID] = action_add_keji;
			}
			else if (actionOpt == 1)
			{
				temp[ACTION::strActionID] = action_add_keji_yaosai;
			}
			temp[ACTION::strValue] = dataJson[warStoryRewardJunGongStr].asInt();
			actionJson.append(temp);
		}

		if (!dataJson[warStoryMapTypeStr].isNull() && dataJson[warStoryMapTypeStr].asInt() == 2)
		{
			Json::Value temp;
			temp[ACTION::strActionID] = action_active_pilot;
			temp[ACTION::strPilotActiveList].append(dataJson[warStoryLeaderGeneralRawIdStr].asInt());
			actionJson.append(temp);
		}
		if (!dataJson[warStoryMapTypeStr].isNull() && dataJson[warStoryMapTypeStr].asInt() == 3)
		{
			Json::Value temp;
			temp[ACTION::strActionID] = action_add_pilot;
			temp[ACTION::strPilotActiveList].append(dataJson[warStoryLeaderGeneralRawIdStr].asInt());
			actionJson.append(temp);
		}
		if (!dataJson[warStoryDropItemIdStr].isNull())
		{
			Json::Value temp;
			temp[ACTION::strActionID] = action_rate_add_item;
			temp[ACTION::addNumStr] = 1;
			Json::Value& dropItems = dataJson[warStoryDropItemIdStr];
			Json::Value& dropRates = dataJson[warStoryDropRateStr];
			for (unsigned i = 0; i < dropItems.size(); ++i)
			{
				temp[ACTION::addItemIDStr] = dropItems[i].asInt();
				temp[ACTION::strAddItemRate] = dropRates[i].asDouble();
				actionJson.append(temp);
			}
		}
// 		if (!dataJson[warStoryDropItemIdStr].isNull())
// 		{
// 			Json::Value temp;
// 			temp[ACTION::strActionID] = action_rate_add_item;
// 			temp[ACTION::addItemIDStr] = dataJson[warStoryDropItemIdStr];
// 			temp[ACTION::strAddItemRate] = dataJson[warStoryDropRateStr];
// 			temp[ACTION::addNumStr] = 1;
// 			actionJson.append(temp);
// 		}
		return actionFormat::formatActionVec(actionJson);
	}

	npcDataList warStory::formatNpcArmy(Json::Value& dataJson)
	{
		npcDataList npcVec;
		for (unsigned n = 0; n < dataJson.size(); ++n)
		{
			Json::Value& currenNpc = dataJson[n];
			npcData npc;
			npc.rawID = currenNpc[warStoryArmyRawIDStr].asInt();
			if(npc.rawID < 0)
				continue;
			npc.armyIndex = currenNpc[warStoryArmyCoorStr].asInt();
			if(npc.armyIndex < 0 || npc.armyIndex > 8)
				continue;
			npc.armsLevel = currenNpc[warStoryArmyArmsLevelStr].asInt();
			npc.Hp = currenNpc[warStoryArmyHPStr].asInt();

			checkNotNullFomatInt(npc.initialAttribute[0], currenNpc[pilotEWaiTongStr]);
			checkNotNullFomatInt(npc.initialAttribute[1], currenNpc[pilotEWaiYongStr]);
			checkNotNullFomatInt(npc.initialAttribute[2], currenNpc[pilotEWaiZhiStr]);
			const pilotConfig& pConfig = pilot_sys.getPilotConfig(npc.rawID);
			if (!pConfig.isVaild())
			{
				LogW << npc.rawID << " can not found ... by war story" << LogEnd;
				continue;
			}
			npc.initialAttribute[0] -= pConfig.initialAttribute[0];
			npc.initialAttribute[1] -= pConfig.initialAttribute[1];
			npc.initialAttribute[2] -= pConfig.initialAttribute[2];

			checkNotNullFomatDouble(npc.initialCharacter[0], currenNpc[pilotPuGongStr]);
			checkNotNullFomatDouble(npc.initialCharacter[1], currenNpc[pilotPuFangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[2], currenNpc[pilotZhanGongStr]);
			checkNotNullFomatDouble(npc.initialCharacter[3], currenNpc[pilotZhanFangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[4], currenNpc[pilotMoGongStr]);
			checkNotNullFomatDouble(npc.initialCharacter[5], currenNpc[pilotMoFangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[6], currenNpc[pilotShanBiStr]);
			checkNotNullFomatDouble(npc.initialCharacter[7], currenNpc[pilotGeDangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[8], currenNpc[pilotBaoJiStr]);
			checkNotNullFomatDouble(npc.initialCharacter[9], currenNpc[pilotFanJiStr]);
			checkNotNullFomatDouble(npc.initialCharacter[10], currenNpc[pilotWuLiZengShangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[11], currenNpc[pilotWuLiJianShangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[12], currenNpc[pilotZhanFaZengShangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[13], currenNpc[pilotZhanFaJianShangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[14], currenNpc[pilotMoFaZengShangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[15], currenNpc[pilotMoFaJianShangStr]);
			checkNotNullFomatDouble(npc.initialCharacter[16], currenNpc[pilotZhiLiaoXiShuStr]);
			checkNotNullFomatDouble(npc.initialCharacter[17], currenNpc[pilotBeiZhiLiaoXiShuStr]);
			npcVec.push_back(npc);
		}
		return npcVec;
	}

	bool descsort(battleSidePtr bs1, battleSidePtr bs2)
	{
		return bs1->createTime > bs2->createTime;
	}

	void warStory::initData()
	{
		db_mgr.ensure_index(BattleNpcMongoDB, BSON("nid" << 1 << "pi" << 1));
		db_mgr.ensure_index(playerCampaignDBStr, BSON(playerIDStr << 1));
		db_mgr.ensure_index(playerWarStoryLastInfoDBStr, BSON(playerIDStr << 1));

		CDCreateMap = -1;
		map_.clear();
		tmap_.clear();
		cmap_.clear();
		mapIDVec_.clear();
		state_ = WAR::state_null;
		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(warStoryDataDirStr, jv);
		
		LogS << "start init npc data ..." << LogEnd;
		int currentMapID = -1;
		for (unsigned i = 0; i < jv.size(); ++i)
		{
			Json::Value& map_data = jv[i];
			cmap_[map_data[warStoryMapIDStr].asInt()] = map_data[warStoryEndMapStr].asInt();
			for (unsigned mapIndex = 0; mapIndex < map_data[warStroyMapDataStr].size(); ++mapIndex)
			{
				Json::Value mapDataJson = map_data[warStroyMapDataStr][mapIndex];
				mapDataPtr mData = create();
				if(mData == NULL)continue;
				string keyName = mapDataJson[warStoryNameStr].asString();
				//mData->mapName = TransLate::getTranslate(keyName);
				mData->mapName = keyName;
				mData->localID = mapDataJson[warStoryLocalIDStr].asInt();
				int mapID = mData->localID / mapIDOffset;
				if(mapID != currentMapID){
					currentMapID = mapID;
					mapIDVec_.push_back(currentMapID);
					db_mgr.ensure_index(playerWarStoryDBStr + boost::lexical_cast<string,int>(currentMapID), BSON(playerIDStr << 1));
				}
				mData->firstBlood = (mapDataJson[warStoryMapTypeStr].asInt() == 5);
				mData->backGround = mapDataJson[warStoryBackGroundIDStr].asInt();
				mData->faceID = mapDataJson[warStoryLeaderGeneralRawIdStr].asInt();
				mData->mapLevel = mapDataJson[warStoryLevelStr].asInt();
				mData->mapValue = mapDataJson[warStoryValueStr].asInt();
				mData->frontLocalID = mapDataJson[warStoryFrontIDStr].asInt();
				mData->playerExp = mapDataJson[warStoryPlayerExpStr].asInt();
				mData->playerExp = mData->playerExp < 0 ? 0 : mData->playerExp;
				mData->pilotExp = mapDataJson[warStoryPilotExpStr].asInt();
				mData->pilotExp = mData->pilotExp < 0 ? 0 : mData->pilotExp;
				mData->mapType = mapDataJson[warStoryMapTypeStr].asInt();
				Json::Value& army_data = mapDataJson[warStoryArmyDataStr];
				mData->npcArmy = formatNpcArmy(army_data);
				mData->actionVec = formationAction(mapDataJson, 0);
				map_[mData->localID] = mData;
			}			
		}

		LogS << "start init npc team data ..." << LogEnd;
		unsigned index = 0;
		NpcIDList.clear();
		do 
		{
			string headPath =  teamWarMapDirStr + boost::lexical_cast<string ,int>(index) + ".json";
			Json::Value headJson = na::file_system::load_jsonfile_val(headPath);
			string dataPath = teamNpcDataDirStr + boost::lexical_cast<string ,int>(index) + ".json";
			Json::Value dataJson = na::file_system::load_jsonfile_val(dataPath);
			if(headJson.isNull() || dataJson.isNull())break;
			teamMapDataPtr mData = createTeam();
			if(mData == NULL)continue;
			mData->teamWinMax = headJson[teamNpcMaxWinNumStr].asInt();
			mData->teamMax = headJson[teamNpcMaxTeamMemberStr].asInt();
			mData->teamAmryNum = headJson[teamNpcArmyNumStr].asInt();
			mData->battleSize = headJson[teamNpcStepStr].asUInt();
			mData->battleSize =  mData->battleSize < 2 ?  2 : mData->battleSize; 
			mData->localID = headJson[warStoryLocalIDStr].asInt();
			mData->faceID = headJson[warStoryLeaderGeneralRawIdStr].asInt();
			mData->mapLevel = headJson[warStoryLevelStr].asInt();
			mData->mapValue = headJson[warStoryValueStr].asInt();
			mData->frontMapID = headJson[warStoryFrontIDStr].asInt();
			mData->playerExp = headJson[warStoryPlayerExpStr].asInt();
			mData->playerExp = mData->playerExp < 0 ? 0 : mData->playerExp;
			if (headJson.isMember(warStoryPilotExpStr))
			{
				mData->pilotExp = headJson[warStoryPilotExpStr].asInt();
			}

			{//要塞战助战
				npcHelpConfigPtr helpPtr = createNpcHelpPtr();
				helpPtr->localID_ = headJson["localID"].asInt();
				helpPtr->cost_ = headJson["helpCost"].asInt();
				helpPtr->warStoryLimit_ = headJson["warStoryLimit"].asInt();
				helpPtr->fortLimit_ = headJson["fortLimit"].asInt();
				helpPtr->minPassNumber_ = headJson["minPassNumber"].asInt();
				npcHelpConfigMap_[helpPtr->localID_] = helpPtr;
				NpcIDList.insert(helpPtr->warStoryLimit_);
				NpcIDList.insert(helpPtr->fortLimit_);
			}

			mData->pilotExp = mData->pilotExp < 0 ? 0 : mData->pilotExp;
			mapIDTeamVec_.push_back(mData->localID);
			for (unsigned i = 0; i < dataJson.size(); ++i)
			{
				mapDataPtr mDP = create();
				if(mDP == NULL)continue;
				mDP->faceID = dataJson[i][warStoryLeaderGeneralRawIdStr].asInt();
				string mNa = dataJson[i][warStoryNameStr].asString();
				//mDP->mapName = TransLate::getTranslate(mNa);
				mDP->mapName = mNa;
				mDP->localID = dataJson[i][warStoryLocalIDStr].asInt();
				mDP->mapLevel = dataJson[i][warStoryLevelStr].asInt();
				mDP->mapValue = dataJson[i][warStoryValueStr].asInt();
				Json::Value& troop = dataJson[i]["trd"];
				mDP->npcArmy = formatNpcArmy(troop);
				mData->npcArmy.push_back(mDP);
			}
			mData->actionVec = formationAction(headJson, 1);
			tmap_[mData->localID] = mData;

			++index;
		} while (true);

		LogS << "start init npc team robot data ..." << LogEnd;
		Json::Value robotJson = na::file_system::load_jsonfile_val(teamRobotFileStr);
		robotItem_ = formatNpcArmy(robotJson["trd"]);
		strPoint_ = robotJson["point"].asString();

// 		LogS << "start init npc team robot name data ..." << LogEnd;
// 		Json::Value nameJson;
// 		nameJson = na::file_system::load_jsonfile_val(teamRobotBoysNameStr);
// 		for (unsigned i = 0; i < nameJson.size(); i++)
// 			boysNameList_.push_back(nameJson[i].asString());
// 
// 		nameJson = na::file_system::load_jsonfile_val(teamRobotGirlsNameStr);
// 		for (unsigned i = 0; i < nameJson.size(); i++)
// 			girlsNameList_.push_back(nameJson[i].asString());
// 
// 		nameJson = na::file_system::load_jsonfile_val(teamRobotFamilyNameStr);
// 		for (unsigned i = 0; i < nameJson.size(); i++)
// 			familyNameList_.push_back(nameJson[i].asString());


		LogS << "start initial chaper reward config json..." << LogEnd;
		const static string WarStroryReward = "./instance/warStroryReward/";
		const static string WarStoryRewardActionStr = "action";
		for (unsigned n = 1; n < 4; ++n)
		{
			string path = WarStroryReward + "chaper" + boost::lexical_cast<string, unsigned>(n)+".json";
			Json::Value rewardJson = na::file_system::load_jsonfile_val(path);
			for (unsigned i = 0; i < rewardJson.size(); ++i)
			{
				Json::Value& rw = rewardJson[i];
				mapDataPtr mDPtr = create();
				mDPtr->localID = rw[warStoryLocalIDStr].asInt();
				const static string starLimitStr = "starLimit";
				mDPtr->frontLocalID = rw[starLimitStr].asInt();
				mDPtr->actionVec = actionFormat::formatActionVec(rw[WarStoryRewardActionStr]);
				chaperRewardMap[n - 1][mDPtr->localID] = mDPtr;
			}
		}

		{
			string path = WarStroryReward + "create_cd.json";
			Json::Value mapJson = na::file_system::load_jsonfile_val(path);
			CDCreateMap = mapJson[0u].asInt();
		}

// 		{
// 			const static string npcListJsonStr = "./instance/warStroryReward/npc_list.json";
// 			NpcIDList.clear();
// 			Json::Value listJson = na::file_system::load_jsonfile_val(npcListJsonStr);
// 			for (unsigned i = 0; i < listJson.size(); ++i)
// 			{
// 				NpcIDList.insert(listJson[i].asInt());
// 			}
// 		}

		objCollection objC = db_mgr.Query(BattleNpcMongoDB);
		for (unsigned i = 0; i < objC.size(); ++i)
		{
			battleSidePtr newSide = battleSide::Create();
			if (newSide == NULL)
			{
				LogE << "can not malloc memory ..." << LogEnd;
				abort();
			}
			mongo::BSONObj& obj = objC[i];
			newSide->bsonToState(obj);
			npcMap[obj["nid"].Int()].push_back(newSide);
		}

		for (NpcMaps::iterator it = npcMap.begin(); it != npcMap.end();)
		{
			NpcMaps::iterator cIt = it;
			++it;
			const int cID = cIt->first;
			if (!isMatchNpcList(cID))
			{
				npcMap.erase(cIt);
				mongo::BSONObj rmkey = BSON("nid" << cID);
				db_mgr.remove_collection(BattleNpcMongoDB, rmkey);
				continue;
			}
			OneVec& vec = cIt->second;
			std::sort(vec.begin(), vec.end(), descsort);
		}

// 		LogS << "start initial npc help json..." << LogEnd;
// 		{
// 			const static string strNpcHelpDir = "./instance/corps/npcHelp/";
// 			na::file_system::json_value_vec npcHelpList;
// 			na::file_system::load_jsonfiles_from_dir(strNpcHelpDir, npcHelpList);
// 			for (unsigned i = 0; i < npcHelpList.size(); i++)
// 			{
// 				Json::Value singleConfig = npcHelpList[i];
// 				npcHelpConfigPtr helpPtr = createNpcHelpPtr();
// 				helpPtr->localID_ = singleConfig["localID"].asInt();
// 				helpPtr->cost_ = singleConfig["cost"].asInt();
// 				helpPtr->helpCondition_ = singleConfig["helpCondition"].asInt();
// 				helpPtr->minPassNumber_ = singleConfig["minPassNumber"].asInt();
// 				npcHelpConfigMap_[helpPtr->localID_] = helpPtr;
// 			}
// 		}
 	}

	bool warStory::isMatchNpcList(const int localID)
	{
		return NpcIDList.find(localID) != NpcIDList.end();
	}

	void warStory::teamUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int localID = js_msg[0u].asInt();
		int startIndex = js_msg[1].asInt();
		int teamOffset = js_msg[2].asInt();
		teamMapDataPtr ptr = getTeamMapData(localID);
		if (ptr == teamMapDataPtr())
		{
			Return(r, -1);
		}

		refreshList_[localID].insert(d);
		teamListPtr teamListData = getTeamList(localID);

		int maxIndex = -1;
		int ret = 0;
		Json::Value pack = Json::arrayValue;
		if (teamListData != teamListPtr())
		{
			ret = teamListData->package(startIndex, teamOffset, pack, maxIndex);
		}
		r[msgStr][1u] = pack;
		r[msgStr][2u] = maxIndex;
		Return(r, ret);
	}

	void warStory::teamManualUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int localID = js_msg[0u].asInt();
		int startIndex = js_msg[1].asInt();
		int teamOffset = js_msg[2].asInt();
		teamMapDataPtr ptr = getTeamMapData(localID);
		if (ptr == teamMapDataPtr())
		{
			Return(r, -1);
		}

		refreshList_[localID].insert(d);
		teamListPtr teamListData = getTeamList(localID);

		int maxIndex = -1;
		int ret = 0;
		Json::Value pack = Json::arrayValue;
		if (teamListData != teamListPtr())
		{
			ret = teamListData->package(startIndex, teamOffset, pack, maxIndex);
		}
		r[msgStr][1u] = pack;
		r[msgStr][2u] = maxIndex;
		Return(r, ret);
	}

	void warStory::teamMemberUpdateReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;

		int localID = js_msg[0u].asInt();
		int leaderName = js_msg[1].asInt();

		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{//没有该地图
			Return(r, 1);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(leaderName);
		if (teamItemData == teamItemPtr())
		{//没有该队伍
			Return(r, 2);
		}

		Json::Value pack;
		teamItemData->package(pack);
		r[msgStr][1u] = pack;
		Return(r, 0);
	}

	void warStory::teamCreateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int localID = js_msg[0u].asInt();
		int joinCond = js_msg[1].asInt();
		int joinLevel = js_msg[2].asInt();
		bool isNewbie = js_msg[3].asBool();

		vector<string> vecNameList;
		if (isNewbie)
		{
			if (js_msg.size() < 5)
			{
				Return(r, -1);
			}
			else
			{
				Json::Value nameList = js_msg[4];
				if (!nameList.isArray() || nameList.size() == 0 || nameList.size() > 4)
					Return(r, -1);
				for (unsigned i = 0; i < nameList.size(); i++)
				{
					if (!nameList[i].isString())
						Return(r, -1);
					vecNameList.push_back(nameList[i].asString());
				}
			}
		}

		int leaderPlayer = d->playerID;
		teamMapDataPtr ptr = getTeamMapData(localID);
		if (ptr == teamMapDataPtr())
			Return(r, -1);

		if (d->Base.getLevel() < WAR_STORY_TEAM_OPEN_LEVEL)
			Return(r, -1);

		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			teamListData = createTeamList(localID);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(leaderPlayer);
		if (teamItemData != teamItemPtr())
		{//已经有该队伍了
			Return(r, 1);
		}
		
		if (isMemberIn(d->playerID))
		{//已经在队伍里面了不能再创建队伍
			Return(r, 2);
		}

		int frontID = ptr->frontMapID;
//		if(frontID >= 0 && (d->Warstory.currentProcess / mapIDOffset) <= frontID)
		if(frontID >= 0 && !isProcessMap(d, frontID))
			Return(r, 3);//还没有打过这张地图

		if (d->Warstory.hasFirstBlood(ptr->localID) && d->Campaign.getJunling() < 1)
			Return(r, 4);//军令不足

		if (d->Campaign.getCD().getCD().Lock)Return(r, 5);//军令cd中

		teamItemData = teamListData->createTeamItem();
		memberUnion memUn;
		memUn.player_ = d;
		teamItemData->members_.push_back(memUn);
		teamItemData->joinCondition_ = joinCond;
		teamItemData->levelLimit_ = joinLevel;
		teamItemData->aliveTime_ = int(na::time_helper::get_current_time());
		teamItemData->isNewbie_ = isNewbie;
		teamItemData->createTime_ = na::time_helper::get_current_time();
		teamItemData->lastNpcHelpAddTime_ = teamItemData->createTime_;
		teamItemData->robotNameList_ = vecNameList;

		teamListData->teams.push_back(teamItemData);
		teamListMap_[localID] = teamListData;
		
		memberInfo_[d->playerID].localID_ = localID;
		memberInfo_[d->playerID].leaderPlayer_ = leaderPlayer;

		r[msgStr][0u] = 0;
		d->sendToClient(gate_client::war_story_team_create_resp, r);

		updateTeamMem2Client(teamItemData);		
		r = Json::Value::null;

		if (localID == 0 && isNewbie)
			duration_ = 1;

		Json::Value annouceJson;
		annouceJson[msgStr][0u] = 0;
		annouceJson[msgStr][1] = localID;
		annouceJson[msgStr][2] = d->Base.getName();
		string s = annouceJson.toStyledString();
		msg_json mj(gate_client::war_story_team_annouce_create_team_resp, s);
		war_story.sendToTeamList(localID, mj);
	}

	void warStory::teamQuitInterfaceReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		refreshList_[localID].erase(d);
		Return(r, 0);
	}

	void warStory::teamBonusUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		r[msgStr][1] = act_game_param_mgr.get_game_param_by_id(activity::_fort_war);
		Return(r, 0);
	}

	void warStory::teamInviteReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (!isMemberIn(d->playerID))
		{
			Return(r, -1);
		}

		if (d->Base.getLevel() < WAR_STORY_TEAM_OPEN_LEVEL)
			Return(r, -1);

		unsigned now = na::time_helper::get_current_time();
		if(now < d->ChatArray[chat_team + 1] || now - d->ChatArray[chat_team + 1] < 15)
			Return(r, 3);
		d->ChatArray[chat_team + 1] = now;

		int localID = memberInfo_[d->playerID].localID_;
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(d->playerID);
		if (teamItemData == teamItemPtr())
		{//不是队长不能邀请
			Return(r, 1);
		}

// 		playerWarStoryPtr ptr = d->Warstory.getMap(localID);
// 		if (ptr == playerWarStoryPtr())
// 		{
// 			Return(r, -1);
// 		}
		if (d->Warstory.hasFirstBlood(localID))
		{//不是首杀不能邀请
			Return(r, 2);
		}

		teamMapDataPtr mapPtr = getTeamMapData(localID);
		int frontID = mapPtr->frontMapID;
		if(frontID >= 0 && !isProcessMap(d, frontID))
			Return(r, 4);//还没有打过这张地图
		
		Json::Value chatContent;
		chatContent[playerWarStoryMapIDStr] = localID;
		chatContent[WAR::warStoryTeamMemberNum] = (unsigned)teamItemData->members_.size();
		chatContent["ld"] = d->playerID;
		chatContent["ln"] = d->Base.getName();
		chatContent["lmlv"] = teamItemData->levelLimit_;
		chatContent["lmc"] = teamItemData->joinCondition_;
		chat_sys.sendToTeamBroadCastCommon(BROADCAST::npc_team_invite, chatContent);

		Return(r, 0);
	}

	void warStory::teamChangeMemberPosReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		if (d->Base.getLevel() < WAR_STORY_TEAM_OPEN_LEVEL)
			Return(r, -1);

		if (!isMemberIn(d->playerID))
		{
			Return(r, -1);
		}

		int name1 = js_msg[0].asInt();
		int name2 = js_msg[1].asInt();

		int localID = memberInfo_[d->playerID].localID_;
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(d->playerID);
		if (teamItemData == teamItemPtr())
		{//不是队长不能交换玩家位置
			Return(r, 1);
		}

		if (name1 == name2)
			Return(r, -1);

		if (name1 == d->playerID || name2 == d->playerID)
		{//不能改变自己的位置
			Return(r, 2);
		}

		bool isFindName1 = false, isFindName2 = false;
		int pos1 = -1, pos2 = -1;
		for (unsigned idx = 0; idx < teamItemData->members_.size(); idx++)
		{
			if (teamItemData->members_[idx].isPlayer())
			{
				if (teamItemData->members_[idx].player_->playerID == name1)
				{
					pos1 = idx;
					isFindName1 = true;
				}
				if (teamItemData->members_[idx].player_->playerID == name2)
				{
					pos2 = idx;
					isFindName2 = true;
				}
			}
			else
			{
				int  lid = 0;
				if (teamItemData->members_[idx].isNpc())
					lid = teamItemData->members_[idx].npc_->localID;
				else
					lid = teamItemData->members_[idx].npcFromPlayer_->getSideID();

				if (lid == name1)
				{
					pos1 = idx;
					isFindName1 = true;
				}
				if (lid == name2)
				{
					pos2 = idx;
					isFindName2 = true;
				}
			}

		}

		if (!isFindName1 || !isFindName2)
		{//没有这两个玩家
			Return(r, 3);
		}

		teamMapDataPtr ptr = getTeamMapData(localID);
		int frontID = ptr->frontMapID;
//		if(frontID >= 0 && (d->Warstory.currentProcess / mapIDOffset) <= frontID)
		if(frontID >= 0 && !isProcessMap(d, frontID))
			Return(r, 4);//还没有打过这张地图

		memberUnion temp;
		temp = teamItemData->members_[pos1];
		teamItemData->members_[pos1] = teamItemData->members_[pos2];
		teamItemData->members_[pos2] = temp;

		r[msgStr][0u] = 0;
		d->sendToClient(gate_client::war_story_team_change_member_pos_resp, r);

		updateTeamMem2Client(teamItemData);

		r = Json::Value::null;
	}

	void warStory::teamRemoveMemberReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		if (!isMemberIn(d->playerID))
		{
			Return(r, -1);
		}

		int removePlayer = js_msg[0].asInt();
		int localID = memberInfo_[d->playerID].localID_;
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(d->playerID);
		if (teamItemData == teamItemPtr())
		{//不是队长不能移除玩家
			Return(r, 1);
		}

		if (removePlayer == d->playerID)
		{//不能移除自己
			Return(r, 2);
		}

		bool isRemove = false;
		memberUnion removePlayerData;
		for (vector<memberUnion>::iterator it_mem = teamItemData->members_.begin(); it_mem != teamItemData->members_.end();)
		{
			if (it_mem->isPlayer())
			{
				if (it_mem->player_->playerID == removePlayer)
				{
					removePlayerData = (*it_mem);
					memberInfo_.erase(it_mem->player_->playerID);
					it_mem = teamItemData->members_.erase(it_mem);
					isRemove = true;
					break;
				}
				else
				{
					it_mem++;
				}
			}
			else
			{
				int lid = 0;
				if (it_mem->isNpc())
					lid = it_mem->npc_->localID;
				else
					lid = it_mem->npcFromPlayer_->getSideID();
				if (lid == removePlayer)
				{
					removePlayerData = (*it_mem);
					memberInfo_.erase(lid);
					removeRobot(lid);
					it_mem = teamItemData->members_.erase(it_mem);
					isRemove = true;
					break;
				}
				else
				{
					it_mem++;
				}
			}
		}

		if (!isRemove)
		{//没有该玩家
			Return(r, 3);
		}

		r[msgStr][0u] = 0;
		d->sendToClient(gate_client::war_story_team_remove_member_resp, r);

		if (removePlayerData.isPlayer())
		{
			removePlayerData.player_->sendToClient(gate_client::war_story_team_remove_member_resp, r);
		}

		updateTeamMem2Client(teamItemData);

		r = Json::Value::null;
	}

	void warStory::teamDismissReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (!isMemberIn(d->playerID))
		{//没有队伍，不能解散
			Return(r, 5);
		}

		if (d->Base.getLevel() < WAR_STORY_TEAM_OPEN_LEVEL)
			Return(r, -1);

		int localID = memberInfo_[d->playerID].localID_;

		teamMapDataPtr ptr = getTeamMapData(localID);
		if (ptr == teamMapDataPtr())
		{
			Return(r, -1);
		}

		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(d->playerID);
		if (teamItemData == teamItemPtr())
		{//不是队长不能解散
			Return(r, 1);
		}

		r[msgStr][0u] = 0;

		Json::Value updateJson;
		updateJson[msgStr][0u] = localID;
		string strUpdate = updateJson.toStyledString();
		na::msg::msg_json mj_team_update(m._player_id, m._net_id, gate_client::war_story_team_update_req, strUpdate);

		vector<memberUnion> tempPlayer;
		for (vector<memberUnion>::iterator it_mem = teamItemData->members_.begin(); it_mem != teamItemData->members_.end();)
		{
			tempPlayer.push_back(*it_mem);
			if (it_mem->isPlayer())
			{
				memberInfo_.erase(it_mem->player_->playerID);
			}
			else
			{
				int lid = 0;
				if (it_mem->isNpc())
					lid = it_mem->npc_->localID;
				else
					lid = it_mem->npcFromPlayer_->getSideID();

				memberInfo_.erase(lid);
				removeRobot(lid);
			}
			it_mem = teamItemData->members_.erase(it_mem);
		}
		
		vector<teamItemPtr>::iterator it_team = find(teamListData->teams.begin(), teamListData->teams.end(), teamItemData);
		teamListData->teams.erase(it_team);

		for (vector<memberUnion>::iterator it_mem = tempPlayer.begin(); it_mem != tempPlayer.end(); it_mem++)
		{
			if (it_mem->isPlayer())
			{
				it_mem->player_->sendToClient(gate_client::war_story_team_dismiss_resp, r);
				mj_team_update._player_id = it_mem->player_->playerID;
				mj_team_update._net_id = it_mem->player_->netID;
				player_mgr.postMessage(mj_team_update);
			}

		}

		r = Json::Value::null;
	}

	void warStory::teamLeaderAliveReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (!isMemberIn(d->playerID))
		{
			Return(r, 4);
		}
		int localID = memberInfo_[d->playerID].localID_;
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(d->playerID);
		if (teamItemData == teamItemPtr())
		{
			Return(r, 1);
		}

		teamItemData->aliveTime_ = int(na::time_helper::get_current_time());

		Return(r, 0);
	}

	void warStory::teamNpcHelpUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		int playerID = m._player_id;
		if (!isMemberIn(playerID))
		{
			Return(r, 1);
		}
		int localID = memberInfo_[playerID].localID_;
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(playerID);
		if (teamItemData == teamItemPtr())
		{//不是队长
			Return(r, 2);
		}
		teamMapDataPtr ptr = getTeamMapData(localID);
		int frontID = ptr->frontMapID;
		
		bool isfind = false;
		npcHelpConfigPtr helpCfg = getNpcHelpConfig(localID);
		if (helpCfg == npcHelpConfigPtr())
			Return(r, -1);
		vector<battleSidePtr> allWar = npcMap[helpCfg->warStoryLimit_];
		
		if (frontID >= 0 && !isProcessMap(d, frontID))
			Return(r, 3);//还没有打过这张地图

		if (d->Base.getLevel() < WAR_STORY_TEAM_OPEN_LEVEL)
			Return(r, -1);

		vector<battleSidePtr> allPort = npcMap[helpCfg->fortLimit_];
		vector<battleSidePtr> validSide;
		for (unsigned i = 0; i < allPort.size(); i++)
		{
			battleSidePtr singlePort = allPort[i];
			for (unsigned j = 0; j < allWar.size(); j++)
			{
				battleSidePtr singleWar = allWar[j];
				if (singlePort->getSideID() == singleWar->getSideID())
				{
					validSide.push_back(singleWar);
					break;
				}
			}
		}

		Json::Value retJson = Json::arrayValue;
		for (unsigned i = 0; i < validSide.size(); i++)
		{
			battleSidePtr singleSide = validSide[i];
			Json::Value singleJson;
			singleJson[playerNameStr] = singleSide->getName();
			singleJson[playerLevelStr] = singleSide->getPlayerLevel();
			singleJson[playerFaceIDStr] = singleSide->getFaceID();
			singleJson[playerSphereIDStr] = singleSide->sphereID;
			retJson.append(singleJson);
		}

		r[msgStr][0u] = 0;
		r[msgStr][1] = retJson;
	}

	void warStory::teamNpcHelpReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int playerID = m._player_id;
		string prefixName = js_msg[0u].asString();

		if (!isMemberIn(playerID))
		{
			Return(r, 1);
		}
		int localID = memberInfo_[playerID].localID_;
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(playerID);
		if (teamItemData == teamItemPtr())
		{//不是队长
			Return(r, 2);
		}
		teamMapDataPtr ptr = getTeamMapData(localID);
		int frontID = ptr->frontMapID;

		npcHelpConfigPtr helpCfg = getNpcHelpConfig(localID);
		if (helpCfg == npcHelpConfigPtr())
			Return(r, -1);

		if (frontID >= 0 && !isProcessMap(d, frontID))
			Return(r, 3);//还没有打过这张地图

		if (d->Base.getLevel() < WAR_STORY_TEAM_OPEN_LEVEL)
			Return(r, -1);

		if (d->Base.getKeJi() < helpCfg->cost_)
			Return(r, 4);

		if (teamItemData->hasSeekHelp_)
			Return(r, 5);

		vector<battleSidePtr> all = npcMap[helpCfg->fortLimit_];
		if (int(all.size()) < helpCfg->minPassNumber_)
			Return(r, 6);
		
		teamItemData->hasSeekHelp_ = true;
		teamItemData->prefixName_ = prefixName;
		duration_ = 0;

		Return(r, 0);
	}

	void warStory::teamAddNpcFromPlayerReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int playerID = m._player_id;

		if (!isMemberIn(playerID))
		{
			Return(r, 1);
		}
		int localID = memberInfo_[playerID].localID_;
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(playerID);
		if (teamItemData == teamItemPtr())
		{//不是队长
			Return(r, 2);
		}

		npcHelpConfigPtr helpCfg = getNpcHelpConfig(localID);

		vector<battleSidePtr> all = npcMap[helpCfg->warStoryLimit_];

		teamMapDataPtr ptr = getTeamMapData(localID);
		int frontID = ptr->frontMapID;

		if (frontID >= 0 && !isProcessMap(d, frontID))
			Return(r, 3);//还没有打过这张地图

		if (!teamItemData->hasSeekHelp_)
			Return(r, 4);

		if (teamItemData->members_.size() >= 8)
			Return(r, 5);

		int npcFromPlayerNum = 0;
		for (vector<memberUnion>::iterator it = teamItemData->members_.begin(); it != teamItemData->members_.end(); it++)
		{
			if (it->isNpcFromPlayer())
			{
				npcFromPlayerNum++;
			}
		}
		if (npcFromPlayerNum >= int(all.size()))
			Return(r, 6);

		vector<battleSidePtr> validAll;
		vector<battleSidePtr> fortAll = npcMap[helpCfg->fortLimit_];
		for (vector<battleSidePtr>::iterator itFort = fortAll.begin(); itFort != fortAll.end(); itFort++)
		{
			for (vector<battleSidePtr>::iterator itAll = all.begin(); itAll != all.end(); itAll++)
			{
				if ((*itFort)->getSideID() == (*itAll)->getSideID())
				{
					validAll.push_back(*itAll);
					break;
				}
			}
		}

		vector<battleSidePtr> validSide;
		for (vector<battleSidePtr>::iterator itSide = validAll.begin(); itSide != validAll.end(); itSide++)
		{
			bool isInMem = false;
			for (vector<memberUnion>::iterator itMem = teamItemData->members_.begin(); itMem != teamItemData->members_.end(); itMem++)
			{
				if (itMem->isNpcFromPlayer() && (-itMem->npcFromPlayer_->getSideID()) == (*itSide)->getSideID())
				{
					isInMem = true;
					break;
				}
			}
			if (!isInMem)
			{
				validSide.push_back(*itSide);
			}
		}

		if (validSide.size() == 0)
			Return(r, 6);

 		int cur_time = (int)na::time_helper::get_current_time();
// 		if (cur_time - teamItemData->lastNpcHelpAddTime_ < 5)
// 			Return(r, 7);

		int sideIndex = commom_sys.randomBetween(0, validSide.size() - 1);
		battleSidePtr copyPtr = validSide[sideIndex]->copyState();
		if (teamItemData->addNewNpcFromPlayer(copyPtr))
		{
			teamItemData->lastNpcHelpAddTime_ = cur_time;
			updateTeamMem2Client(teamItemData);
		}
	
		Return(r, 0);
	}

	void warStory::teamSweepReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int localID = js_msg[0u].asInt();
		int sweepTimes = js_msg[1u].asInt();
		if (sweepTimes < 1 || sweepTimes > 50)
			Return(r, 5);
		
		int beforeJunling = d->Campaign.getJunling();
		if (beforeJunling < sweepTimes)
			Return(r, 1);

		int beforeOrderNum = d->Item.getItemStack(SWEEP_ORDER_ID);
		if (beforeOrderNum < sweepTimes)
			Return(r, 2);

		teamMapDataPtr ptr = getTeamMapData(localID);
		if (ptr == teamMapDataPtr())
			Return(r, -1);

		if (!d->Warstory.hasFirstBlood(localID))
			Return(r, 3);

		Json::Value actionTemp = Json::arrayValue;
		for (actionVector::iterator itAct = ptr->actionVec.begin(); itAct != ptr->actionVec.end(); itAct++)
		{
			actionTemp.append((*itAct)->JsonData());
		}

		int expMedicID = 14035;
		int sweepOrderID = 13009;
		int titaniumAlloyID = 13008;

		Json::Value Param;
		double kejiAddRateWinNum = 0.0, kejiAddRateBase = 0.0;
		double addItemExtraRate = activity_sys.getRate(activity::_fu_xing_gao_zhao, d);
		double d1, d2;
		kejiAddRateBase = d->Science.getKeJiBuffer();
		d2 = activity_sys.getRate(activity::_jing_bing_qiang_zhen, d);
		d1 = activity_sys.getRate(activity::_fort_war, d);
		kejiAddRateBase = kejiAddRateBase + d1 + d2;
		kejiAddRateWinNum = (0.2 + 0.05 * (3 - 2));//奖励按照三连胜来计算
		Param[ACTION::strYaoSaiParamBase] = kejiAddRateBase;
		Param[ACTION::strYaoSaiParamWinNum] = kejiAddRateWinNum;

		Json::Value actionJsonList = actionTemp;
		for (unsigned idxAct = 0; idxAct < actionJsonList.size(); idxAct++)
		{
			if (actionJsonList[idxAct][ACTION::strActionID].asInt() == action_rate_add_item)
			{
				int itemID = actionJsonList[idxAct][ACTION::addItemIDStr].asInt();
				actionJsonList[idxAct][ACTION::strAddItemRate] = actionJsonList[idxAct][ACTION::strAddItemRate].asDouble() * (1 + addItemExtraRate);
				if (itemID != expMedicID && itemID != titaniumAlloyID && itemID != sweepOrderID)//这里对装备单独处理
				{
					actionJsonList[idxAct][ACTION::strAddItemRate] = actionJsonList[idxAct][ACTION::strAddItemRate].asDouble() / 6;
				}
			}
		}

		int ret = 0;
		Json::Value totalReward = Json::arrayValue;
		for (int i = 0; i < sweepTimes; i++)
		{
			vector<actionResult> vecRetCode;
			vecRetCode = actionFormat::actionDoJump(d, actionJsonList, Param);
			Json::Value failSimpleJson;
			bool isSend = email_sys.sendDoJumpFailEmail(d, actionJsonList, vecRetCode, email_team_system_bag_full_sweep, failSimpleJson);
			if (isSend)
				ret = 4;
			Json::Value sweepReward = actionFormat::getLastDoJson();
			email_sys.dealDoJumpFailLastDo(actionJsonList, vecRetCode, sweepReward);
			totalReward.append(sweepReward);
		}

		d->Campaign.alterJunling(-sweepTimes);
		d->Item.removeItem(SWEEP_ORDER_ID, sweepTimes);

		int afterJunling = d->Campaign.getJunling();
		int afterOrderNum = d->Item.getItemStack(SWEEP_ORDER_ID);
		//时间、玩家ID、对应要塞ID、扫荡次数、获得奖励、扫荡前扫荡令数量、扫荡前燃料数量、
		//扫荡后扫荡令数量、扫荡后燃料数量
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(na::time_helper::get_current_time()));
		fields.push_back(boost::lexical_cast<string, int>(localID));
		fields.push_back(boost::lexical_cast<string, int>(sweepTimes));
		fields.push_back(boost::lexical_cast<string, int>(beforeJunling));
		fields.push_back(boost::lexical_cast<string, int>(afterJunling));
		fields.push_back(totalReward.toStyledString());
		StreamLog::Log(logWarTeamSweepLog, d, boost::lexical_cast<string, int>(beforeOrderNum), boost::lexical_cast<string, int>(afterOrderNum), fields);

		r[msgStr][1] = totalReward;
		Return(r, ret);
	}

	void warStory::teamAttackReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (!isMemberIn(d->playerID))
		{
			Return(r, 4);
		}
		int localID = memberInfo_[d->playerID].localID_;
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		if (d->Base.getLevel() < WAR_STORY_TEAM_OPEN_LEVEL)
			Return(r, -1);

		teamItemPtr teamItemData = teamListData->getTeamItem(d->playerID);
		if (teamItemData == teamItemPtr())
		{//不是队长不能发动公鸡
			Return(r, 1);
		}

		teamMapDataPtr ptr = getTeamMapData(localID);
		if (int(teamItemData->members_.size() * ptr->teamWinMax) <= ptr->teamAmryNum)
		{//没达到最低开战人数
//			Return(r, 2);
		}

		int frontID = ptr->frontMapID;
//		if(frontID >= 0 && (d->Warstory.currentProcess / mapIDOffset) <= frontID)
		if(frontID >= 0 && !isProcessMap(d, frontID))
			Return(r, 3);//还没有打过这张地图

		if(d->isOperatBattle())Return(r, 4); //等待战报反馈

		npcHelpConfigPtr helpCfg = getNpcHelpConfig(localID);
		if (helpCfg == npcHelpConfigPtr())
			Return(r, -1);

		if (teamItemData->hasSeekHelp_)
			d->Base.alterKeJi(-helpCfg->cost_);

		BattlefieldMuilt mu;
		for (vector<memberUnion>::iterator it_mem = teamItemData->members_.begin(); it_mem != teamItemData->members_.end(); it_mem++)
		{
			//+++++++把要塞战加入征战事件中
			if (it_mem->isPlayer())
			{
				activity_sys.updateActivity(it_mem->player_, 0, activity::_war_story);
				mu.atkSide.push_back(battleSide::Create(it_mem->player_, ptr));
			}
			else if (it_mem->isNpc())
			{
				mu.atkSide.push_back(battleSide::Create(ptr, it_mem->npc_));
			}
			else
			{
				mu.atkSide.push_back(it_mem->npcFromPlayer_);
			}
		}

		mu.ParamJson[BATTLE::strTeamStep] = ptr->battleSize;
		mu.ParamJson[BATTLE::strWarMapID] = ptr->localID;
		vector<mapDataPtr>& vec = ptr->npcArmy;
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			mapDataPtr npcs = vec[i];
			mu.defSide.push_back(battleSide::Create(ptr, npcs));
		}

		Json::Value memType = Json::arrayValue;
		for (vector<memberUnion>::iterator it_mem = teamItemData->members_.begin(); it_mem != teamItemData->members_.end(); it_mem++)
		{
			if (it_mem->isPlayer())
				memType.append(0);
			else if (it_mem->isNpc())
				memType.append(1);
			else if (it_mem->isNpcFromPlayer())
				memType.append(2);
			else;
		}
		mu.ParamJson[BATTLE::strWarStoryTeamMemberType] = memType;

		for (vector<memberUnion>::iterator it_mem = teamItemData->members_.begin(); it_mem != teamItemData->members_.end(); it_mem++)
		{
			if (it_mem->isPlayer())
			{
				memberInfo_.erase(it_mem->player_->playerID);
			}
			else
			{
				int lid = 0;
				if (it_mem->isNpc())
					lid = it_mem->npc_->localID;
				else
					lid = it_mem->npcFromPlayer_->getSideID();

				memberInfo_.erase(lid);
				removeRobot(lid);
			}
		}

		teamItemData->members_.clear();

		vector<teamItemPtr>::iterator it_team = find(teamListData->teams.begin(), teamListData->teams.end(), teamItemData);
		teamListData->teams.erase(it_team);
		battle_sys.PostTeamBattle(mu, team_battle_npc_type);
	}

	void warStory::teamJoinReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int localID = js_msg[0u].asInt();
		int leaderPlayer = js_msg[1].asInt();

		if (isMemberIn(d->playerID))
		{
			Return(r, 1);
		}

		if (d->Base.getLevel() < WAR_STORY_TEAM_OPEN_LEVEL)
			Return(r, -1);

		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{//没有该地图
			Return(r, 2);
		}

		teamItemPtr teamItemData = teamListData->getTeamItem(leaderPlayer);
		if (teamItemData == teamItemPtr())
		{//没有该队伍
			Return(r, 3);
		}

		playerDataPtr leaderPtr = player_mgr.getPlayerMain(leaderPlayer);
		if (leaderPtr == playerDataPtr())
			Return(r, -1);

		teamMapDataPtr ptr = getTeamMapData(localID);
		if (teamItemData->members_.size() == ptr->teamMax)
		{//人满为患
			Return(r, 4);
		}

		if (teamItemData->joinCondition_ == condition_sphere)
		{
			if (d->Base.getSphereID() != teamItemData->members_[0].player_->Base.getSphereID())
			{//不满足加入队伍的条件:国家，军团
				Return(r, 5);
			}
		}

		if (teamItemData->joinCondition_ == condition_legion)
		{
			if (d->Guild.getGuildID() != teamItemData->members_[0].player_->Guild.getGuildID())
			{
				Return(r, 5);
			}
		}

		int joinLevel = teamItemData->levelLimit_;
		if (joinLevel > d->Base.getLevel())
		{
			Return(r, 6);
		}

		int frontID = ptr->frontMapID;
//		d->Warstory.isProcessMap(frontID);
//		if(frontID >= 0 && (d->Warstory.currentProcess / mapIDOffset) <= frontID)
		if(frontID >= 0 && !isProcessMap(d, frontID))
			Return(r, 7);//还没有打过这张地图

		if ((d->Warstory.hasFirstBlood(ptr->localID) && leaderPtr->Warstory.hasFirstBlood(ptr->localID)) && d->Campaign.getJunling() < 1)
			Return(r, 8);//军令不足

		if (d->Campaign.getCD().getCD().Lock)Return(r, 9);//军令cd中

		memberUnion memUn;
		memUn.player_ = d;
		vector<memberUnion>::iterator it = teamItemData->members_.begin();
		for (; it != teamItemData->members_.end(); it++)
		{
			if (it->isNpcFromPlayer())
			{
				break;
			}
		}

		teamItemData->members_.insert(it, memUn);
		memberInfo_[d->playerID].localID_ = localID;
		memberInfo_[d->playerID].leaderPlayer_ = leaderPlayer;
		
		r[msgStr][0u] = 0;

		playerDataPtr leader = player_mgr.getPlayerMain(leaderPlayer);
		r[msgStr][1u] = leader->Warstory.hasFirstBlood(localID);
		d->sendToClient(gate_client::war_story_team_join_resp, r);
//		cout << __FUNCTION__ << "r:" << r.toStyledString() << endl;

		updateTeamMem2Client(teamItemData);

// 		Json::Value updateJson;
// 		updateJson[msgStr][0u] = localID;
// 		str = updateJson.toStyledString();
// 		na::msg::msg_json mj_update(m._player_id, m._net_id, gate_client::war_story_team_update_req, str);
// 		player_mgr.postMessage(mj_update);

		r = Json::Value::null;
	}

	void warStory::teamQuitReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (!isMemberIn(d->playerID))
		{//不在队伍里不能退出
			Return(r, 1);
		}

		int localID = memberInfo_[d->playerID].localID_;
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			Return(r, -1);
		}

		teamMapDataPtr ptr = getTeamMapData(localID);
		if (ptr == teamMapDataPtr())
		{
			Return(r, -1);
		}

		if (memberInfo_[d->playerID].leaderPlayer_ == d->playerID)
		{//队长不能发退出协议，只能解散
			Return(r, 2);
		}
		teamItemPtr teamItemData = teamListData->getTeamItem(memberInfo_[d->playerID].leaderPlayer_);
		if (teamItemData == teamItemPtr())
		{
			Return(r, -1);
		}

		for (vector<memberUnion>::iterator it_mem = teamItemData->members_.begin(); it_mem != teamItemData->members_.end();)
		{
			if (!it_mem->isPlayer())
			{
				it_mem++;
				continue;
			}

			if (it_mem->player_ == d)
			{
				memberInfo_.erase(it_mem->player_->playerID);
				it_mem = teamItemData->members_.erase(it_mem);
				break;
			}
			else
			{
				it_mem++;
			}
		}

		r[msgStr][0u] = 0;
		d->sendToClient(gate_client::war_story_team_quit_resp, r);

		updateTeamMem2Client(teamItemData);

		r = Json::Value::null;
	}

	void warStory::teamMapUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int mapID = js_msg[0u].asInt();
		d->Warstory.autoUpdateTeam(mapID, *d);
	}

	void warStory::sendToTeamList(int localID, msg_json& m)
	{
		teamListPtr teamListData = getTeamList(localID);
		if (teamListData == teamListPtr())
		{
			return;
		}

		for (set<playerDataPtr>::iterator itPlayer = refreshList_[localID].begin(); itPlayer != refreshList_[localID].end(); itPlayer++)
		{
			m._player_id = (*itPlayer)->playerID;
			m._net_id = (*itPlayer)->netID;
			player_mgr.sendToPlayer(m);
		}
// 		for (vector<teamItemPtr>::iterator itList = teamListData->teams.begin(); itList != teamListData->teams.end(); itList++)
// 		{
// 			for (vector<playerDataPtr>::iterator itMem = (*itList)->members.begin(); itMem != (*itList)->members.end(); itMem++)
// 			{
// 				m._player_id = (*itMem)->playerID;
// 				m._net_id = (*itMem)->netID;
// 				player_mgr.sendToPlayer(m);
// 			}
// 		}
	}

	void warStory::playerLogout(int player_id)
	{
		string s = "";
		na::msg::msg_json mj(gate_client::war_story_team_quit_req, s);
		mj._player_id = player_id;
		Json::Value jresp;
		teamQuitReq(mj, jresp);

		jresp = Json::Value::null;
		mj._type = gate_client::war_story_team_dismiss_req;
		teamDismissReq(mj, jresp);
	}

	teamItem::teamItem()
	{
		joinCondition_ = condition_null;
		levelLimit_ = 0;
		aliveTime_ = 0;
		createTime_ = 0;
		is5secondAdd_ = false;
		hasSeekHelp_ = false;
		lastAddTime_ = 0;
		lastNpcHelpAddTime_ = 0;
		isNewbie_ = false;
	}

	void teamItem::package(Json::Value &pack)
	{
		pack = Json::arrayValue;
		for (vector<memberUnion>::iterator it_mem = members_.begin(); it_mem != members_.end(); it_mem++)
		{
			Json::Value temp;
			string strIsNpcFromPlayer = "isn";
			temp[strIsNpcFromPlayer] = false;
			if (it_mem->isPlayer())
			{
				temp[playerIDStr] = it_mem->player_->playerID;
				temp[playerNameStr] = it_mem->player_->Base.getName();
				temp[playerLevelStr] = it_mem->player_->Base.getLevel();
				temp[playerSphereIDStr] = it_mem->player_->Base.getSphereID();
				temp[playerFaceIDStr] = it_mem->player_->Base.getFaceID();
				pack.append(temp);
			}
			else
			{
				int mid = 0;
				string mname;
				int mlevel = 0;
				int mspereID = -1;
				int mfaceID = 0;
				if (it_mem->isNpc())
				{
					mid = it_mem->npc_->localID;
					mname = it_mem->npc_->mapName;
					mlevel = it_mem->npc_->mapLevel;
					mfaceID = it_mem->npc_->faceID;
					mspereID = it_mem->npc_->sphereID;
				}
				else
				{
					temp[strIsNpcFromPlayer] = true;
					mid = it_mem->npcFromPlayer_->getSideID();
					mname = it_mem->npcFromPlayer_->getName();
					mlevel = it_mem->npcFromPlayer_->getPlayerLevel();
					mfaceID = it_mem->npcFromPlayer_->getFaceID();
					mspereID = it_mem->npcFromPlayer_->sphereID;
				}

				temp[playerIDStr] = mid;
				temp[playerNameStr] = mname;
				temp[playerLevelStr] = mlevel;
				temp[playerSphereIDStr] = mspereID;
				temp[playerFaceIDStr] = mfaceID;
				pack.append(temp);
			}
		}
	}

	bool teamItem::addNewRobot()
	{
		if (members_.size() >= 4)
			return false;

		int leaderLevel = members_[0].player_->Base.getLevel();

		int robotLevel = 0;
		int topLevel = 100;
		if (this->levelLimit_ > 0)
		{
			robotLevel = commom_sys.randomBetween(this->levelLimit_, this->levelLimit_ + 10);
//			robotLevel = commom_sys.randomBetween(leaderLevel - 10, leaderLevel + 10);
		}
		else
		{
			robotLevel = commom_sys.randomBetween(leaderLevel + 6, leaderLevel + 15);
		}

		if (robotLevel > topLevel)
			robotLevel = topLevel;

		if (robotLevel < 1)
			robotLevel = 1;

		if (robotLevel < 15)
			robotLevel = 15;

		int robotShereID = commom_sys.randomBetween(0, 2);
		if (this->joinCondition_ == condition_sphere
			|| this->joinCondition_ == condition_legion)
		{
			robotShereID = members_[0].player_->Base.getSphereID();
		}

		int robotFaceID = commom_sys.randomBetween(21, 90);
		memberUnion memUn;
		mapDataPtr mapItem = warStory::create();
		vector<string> usedNameList;
		for (unsigned i = 1; i < members_.size(); i++)
		{
			if (members_[i].isNpc())
				usedNameList.push_back(members_[i].npc_->mapName);
			else if (members_[i].isNpcFromPlayer())
				usedNameList.push_back(members_[i].npcFromPlayer_->getName());
			else {}
		}
		mapItem->mapName = war_story.getNewRandomName(this->robotNameList_,usedNameList);
		mapItem->localID = war_story.getNewRobotID();
		mapItem->npcArmy = war_story.getRobotInst();
		mapItem->faceID = robotFaceID;
		mapItem->mapLevel = robotLevel;
		mapItem->sphereID = robotShereID;
		memUn.npc_ = mapItem;
		members_.push_back(memUn);
		return true;
	}

	bool teamItem::addNewNpcFromPlayer(battleSidePtr addSide)
	{
		mapDataPtr mapItem = warStory::create();
		addSide->setName(this->prefixName_ + addSide->getName());
		addSide->setSideID(-addSide->getSideID());

		memberUnion memUn;
		memUn.npcFromPlayer_ = addSide;
		members_.push_back(memUn);
		return true;
	}

	teamItemPtr teamList::getTeamItem(int leader)
	{
		for (vector<teamItemPtr>::iterator it = teams.begin(); it != teams.end(); it++)
		{
			if ((*it)->members_[0].player_->playerID == leader)
			{
				return *it;
			}
		}

		return teamItemPtr();
	}

	int teamList::package( int startIndex, int teamOffset, Json::Value &pack, int &maxIndex )
	{
		pack = Json::arrayValue;
		maxIndex = teams.size() - 1;
		if (startIndex < 0)
		{
			return 3;
		}

		int pageSize = 9;
		unsigned endIndex = startIndex + teamOffset - 1;

		if (startIndex != 0 && int(teams.size()) <= startIndex)
		{
			return 0;
		}
		
		for (unsigned i = startIndex; i < teams.size(); i++)
//		for (teamItemMap::iterator it_team = teams.begin(); it_team != teams.end(); it_team++)
		{
			if (i > endIndex)
			{
				break;
			}
			Json::Value temp;
			temp[playerIDStr] = teams[i]->members_[0].player_->playerID;
			temp[playerNameStr] = teams[i]->members_[0].player_->Base.getName();
			temp[warStoryTeamJoinCondition] = teams[i]->joinCondition_;
			temp[warStoryTeamLevelLimit] = teams[i]->levelLimit_;
			temp[warStoryTeamMemberNum] = (unsigned)teams[i]->members_.size();
			temp[playerWarStoryNpcTeamFirstBloodStr] = teams[i]->members_[0].player_->Warstory.hasFirstBlood(this->localID);
			pack.append(temp);
		}
		return 0;
	}

	void teamList::leaderAliveCheck()
	{
		int cur_time = na::time_helper::get_current_time();
		vector <int> leaderList;
		for (vector<teamItemPtr>::iterator it = teams.begin(); it != teams.end(); it++)
		{
			if (cur_time - (*it)->aliveTime_ > 60)
			{
				if ((*it)->members_.size() > 0)
				{
					leaderList.push_back((*it)->members_[0].player_->playerID);
				}
			}
		}

		for (vector<int>::iterator it = leaderList.begin(); it != leaderList.end(); it++)
		{
			war_story.playerLogout(*it);
		}
	}

	void teamList::addRobot()
	{
		int cur_time = na::time_helper::get_current_time();
		for (vector<teamItemPtr>::iterator it = teams.begin(); it != teams.end(); it++)
		{
			if (!(*it)->isNewbie_)
				continue;
			int dif = cur_time - (*it)->createTime_;
			if (dif < 5)
			{
				continue;
			}
			else if (!(*it)->is5secondAdd_ && dif >= 5 && dif < 10)
			{
				if ((*it)->addNewRobot())
				{
					war_story.updateTeamMem2Client(*it);
					(*it)->lastAddTime_ = cur_time;
					(*it)->is5secondAdd_ = true;
				}
			}
			else if (dif >= 10)
			{
				if ((*it)->members_.size() < 4 && cur_time - (*it)->lastAddTime_ >= 1)
				{
					if ((*it)->addNewRobot())
					{
						war_story.updateTeamMem2Client(*it);
						(*it)->lastAddTime_ = cur_time;
					}
				}
			}
			else
			{
			}
		}
	}

// 	void teamList::addNpcFromPlayer(int passMapID)
// 	{
// 		int cur_time = na::time_helper::get_current_time();
// 		for (vector<teamItemPtr>::iterator itTeam = teams.begin(); itTeam != teams.end(); itTeam++)
// 		{
// 			if (!(*itTeam)->hasSeekHelp_)
// 				continue;
// 			int dif = cur_time - (*itTeam)->lastNpcHelpAddTime_;
// 			if (dif >= 5)
// 			{
// 				if ((*itTeam)->addNewNpcFromPlayer(passMapID))
// 				{
// 					(*itTeam)->lastNpcHelpAddTime_ = cur_time;
// 					war_story.updateTeamMem2Client(*itTeam);
// 				}
// 			}
// 		}
// 	}

	teamListPtr warStory::getTeamList(int localID)
	{
		teamListMap::iterator it = teamListMap_.find(localID);
		if(it == teamListMap_.end())return teamListPtr();
		return it->second;
	}

	npcHelpConfigPtr warStory::getNpcHelpConfig(int localID)
	{
		npcHelpconfigMap::iterator it = npcHelpConfigMap_.find(localID);
		if (it == npcHelpConfigMap_.end())return npcHelpConfigPtr();
		return it->second;

	}

	bool warStory::isMemberIn(int player_id)
	{
		memberInfoMap::iterator it = memberInfo_.find(player_id);
		if (it == memberInfo_.end())return false;

		return true;
	}

	void warStory::updateTeamMem2Client(teamItemPtr teamItemData)
	{
		Json::Value pack, packUpdate;
		teamItemData->package(pack);
		packUpdate[msgStr][0u] = 0;
		packUpdate[msgStr][1] = pack;
		string s = packUpdate.toStyledString();
		na::msg::msg_json mj(gate_client::war_story_team_member_update_resp, s);
		for (vector<memberUnion>::iterator it_mem = teamItemData->members_.begin(); it_mem != teamItemData->members_.end(); it_mem++)
		{
			if (it_mem->isPlayer())
			{
				mj._player_id = it_mem->player_->playerID;
				mj._net_id = it_mem->player_->netID;
				player_mgr.sendToPlayer(mj);
			}
		}
	}

	bool warStory::getRewardTime(unsigned &start_time, unsigned &end_time)
	{
		unsigned cur_time = na::time_helper::get_current_time();

		boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
		tm t = boost::posix_time::to_tm(p);

		unsigned stand_time1 = cur_time - 3600*t.tm_hour - 60*t.tm_min - t.tm_sec;
		unsigned stand_time2 = stand_time1 + 5 * 3600;
		
		unsigned start_time1 = stand_time1 + 12 * 3600 + 30 * 60;
		unsigned end_time1 = start_time1 + 90 * 60;
		unsigned start_time2 = stand_time1 + 20 * 3600;
		unsigned end_time2 = start_time2 + 180 * 60;

		bool ret = false;
		start_time = 0;
		end_time = 0;
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

	void warStory::warStoryTeamLoopUpdate( boost::system_time& tmp )
	{
		if((tmp - tick).total_seconds() < duration_)return;

		tick = tmp;
		duration_ = 20;

		unsigned cur_time = na::time_helper::get_current_time();
		boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
		tm t = boost::posix_time::to_tm(p);

		if (t.tm_sec > 30 
			&& (t.tm_hour == 12 && t.tm_min == 29
			|| t.tm_hour == 13 && t.tm_min == 59
			|| t.tm_hour == 19 && t.tm_min == 59
			|| t.tm_hour == 20 && t.tm_min == 59))
		{
			duration_ = 1;
		}

		bool noon_fight = false, night_fight = false;
		if (t.tm_hour == 12 && t.tm_min >= 30 || t.tm_hour == 13)
			noon_fight = true;
		if (t.tm_hour >= 20 && t.tm_hour < 23)
			night_fight = true;
		if (state_ != WAR::state_bonus && noon_fight)
		{
			activity_sys.noticeClientUpdateBonusPar();
			state_ = WAR::state_bonus;
			Json::Value chatContent;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::npc_team_bonus_time_12_30, chatContent);
		}

		br_.refresh();
		//12:25、12：55、13.25 要塞战集结令！12:30-14:00攻打要塞可获奖励加成！"
		if (!br_.br_12_25_ && t.tm_hour == 12 && t.tm_min == 25)
		{
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_war_story_team_begin_12_30);
			br_.br_12_25_ = true;
		}
		if (!br_.br_12_55_ && t.tm_hour == 12 && t.tm_min == 55)
		{
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_war_story_team_begin_12_30);
			br_.br_12_55_ = true;
		}
		if (!br_.br_13_25_ && t.tm_hour == 13 && t.tm_min == 25)
		{
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_war_story_team_begin_12_30);
			br_.br_13_25_ = true;
		}

		//19:55 、20:55、21:55 要塞战集结令！20:00-23:00攻打要塞可获奖励加成！"
		if (!br_.br_19_55_ && t.tm_hour == 19 && t.tm_min == 55)
		{
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_war_story_team_begin_20_00);
			br_.br_19_55_ = true;
		}
		if (!br_.br_20_55_ && t.tm_hour == 20 && t.tm_min == 55)
		{
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_war_story_team_begin_20_00);
			br_.br_20_55_ = true;
		}
		if (!br_.br_21_55_ && t.tm_hour == 21 && t.tm_min == 55)
		{//去掉，挡住了别的广播
//			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_war_story_team_begin_20_00);
			br_.br_21_55_ = true;
		}

		if (state_ != WAR::state_bonus && night_fight)
		{
			activity_sys.noticeClientUpdateBonusPar();
			state_ = WAR::state_bonus;
			Json::Value chatContent;
			chat_sys.sendToAllBroadCastCommon(BROADCAST::npc_team_bonus_time_20_00, chatContent);
		}

		if (!(noon_fight || night_fight))
		{
			state_ = WAR::state_null;
		}

		for (vector<int>::iterator it = mapIDTeamVec_.begin(); it != mapIDTeamVec_.end(); it++)
		{
			teamListPtr teamListData = getTeamList(*it);
			if (teamListData != teamListPtr())
			{
				teamListData->leaderAliveCheck();
			}
		}

		teamListPtr teamListData = getTeamList(0);
		if (teamListData != teamListPtr())
		{
			duration_ = 1;
			teamListData->addRobot();
		}

// 		for (vector<int>::iterator itMap = mapIDTeamVec_.begin(); itMap != mapIDTeamVec_.end(); itMap++)
// 		{
// 			teamListPtr teamListData = getTeamList(*itMap);
// 			if (teamListData == teamListPtr())
// 				continue;
// 			npcHelpConfigPtr nPtr = getNpcHelpConfig(teamListData->localID);
// 			teamListData->addNpcFromPlayer(nPtr->helpCondition_);
// 		}
	}

	int warStory::calTeamKejiReward( playerDataPtr player, playerDataPtr leader )
	{
		return 0;
// 		int kejiAddRateBase = 0.0;
// 		kejiAddRateBase = double(player->Science.getKeJiBuffer());
// 		kejiAddRateBase = kejiAddRateBase 
// 			+ action_show_sys.get_action_show_par(npc_fort_fight,player) 
// 			+ action_show_sys.get_action_show_par(jingbingqiangzhen,player);
// 
// 		if (!leader->Warstory.hasFirstBlood(mapID) || !player->Warstory.hasFirstBlood(mapID))
// 		{
// 			kejiAddRateBase = kejiAddRateBase + 0.2;
// 		}
	}

	gg::playerDataPtr warStory::getTeamLeader( teamItemPtr teamItemData )
	{
		if (teamItemData->members_.size() > 0)
		{
			return teamItemData->members_[0].player_;
		}
		return playerDataPtr();
	}

	mongo::string warStory::getNewRandomName(vector<string> allNames, vector<string> exceptNames)
	{
		for (vector<string>::iterator it = allNames.begin(); it != allNames.end(); it++)
		{
			if (find(exceptNames.begin(), exceptNames.end(), *it) == exceptNames.end())
			{
				return *it;
			}
		}
		return "";

// 		unsigned boyNameSize = boysNameList_.size();
// 		unsigned girlNameSize = girlsNameList_.size();
// 		unsigned familyNameSize = familyNameList_.size();
// 		int randomBoyIdx, randomFamilyIdx;
// 		string randomBoyName, randomFamilyName, entityName;
// 		int maxCount = 1000;
// 		int count = 0;
// 		do 
// 		{
// 			randomBoyIdx = commom_sys.randomBetween(1, boyNameSize);
// 			randomBoyName = boysNameList_[randomBoyIdx - 1];
// 			randomFamilyIdx = commom_sys.randomBetween(1, familyNameSize);
// 			randomFamilyName = familyNameList_[randomFamilyIdx - 1];
// 			string entityName = randomBoyName + strPoint_ + randomFamilyName;
// 
// 			if (entityName.length() < 1 || entityName.length() > 42)
// 				continue;;
// 			int strLength = 0;
// 			for (unsigned i = 0; i < entityName.length(); i++)
// 			{
// 				char v = entityName[i];
// 				unsigned leg = 1;
// 				if ((v & 0xe0) == 0xc0){ leg = 2; }
// 				else if ((v & 0xf0) == 0xe0){ leg = 3; }
// 				else if ((v & 0xf8) == 0xf0)	{ leg = 4; }
// 				else if ((v & 0xfc) == 0xf8)	{ leg = 5; }
// 				else if ((v & 0xfe) == 0xfc)	{ leg = 6; }
// 				if (leg > 1)i += leg - 1;
// 				++strLength;
// 			}
// 			if (strLength > 7)
// 				continue;
// 
// 			if (!player_mgr.hasRegisterName(entityName)
// 				&& find(exceptNames.begin(), exceptNames.end(), entityName) == exceptNames.end())
// 			{
// 				return entityName;
// 			}
// 		} while (count < maxCount);
// 		if (count >= maxCount)
// 			LogE << __FUNCTION__ << " count too much!" << LogEnd;
// 
// 		return "";
	}

	int warStory::getNewRobotID()
	{
		int i = 1;
		do 
		{
			vector<int>::iterator it = find(robotIDUsed_.begin(), robotIDUsed_.end(), i);
			if (it == robotIDUsed_.end())
			{
				robotIDUsed_.push_back(i);
				return i;
			}
			i++;
		} while (true);
		return 0;
	}

	void warStory::removeRobot(int lid)
	{
		vector<int>::iterator itID = find(robotIDUsed_.begin(), robotIDUsed_.end(), lid);
		if (itID != robotIDUsed_.end())
			robotIDUsed_.erase(itID);
	}

	warStoryRollBroadCast::warStoryRollBroadCast()
	{
		br_12_25_ = false;
		br_12_55_ = false;
		br_13_25_ = false;
		br_19_55_ = false;
		br_20_55_ = false;
		br_21_55_ = false;
		nextUpdateTime_ = 0;
	}

	void warStoryRollBroadCast::reset()
	{
		br_12_25_ = false;
		br_12_55_ = false;
		br_13_25_ = false;
		br_19_55_ = false;
		br_20_55_ = false;
		br_21_55_ = false;
	}

	void warStoryRollBroadCast::refresh()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > nextUpdateTime_)
		{
			reset();
			nextUpdateTime_ = na::time_helper::get_next_update_time(cur_time);
		}
	}
}