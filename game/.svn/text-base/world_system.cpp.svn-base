#include "world_system.h"
#include "file_system.h"
#include "mongoDB.h"
#include "Glog.h"
#include "response_def.h"
#include "block.h"
#include "helper.h"
#include "battle_system.h"
#include "time_helper.h"
#include "task_system.h"
#include "chat_system.h"
#include "activity_system.h"
using namespace mongo;

namespace gg
{
	const static unsigned flagNameSizeLimit = 10;
	const static unsigned leaveMessageSizeLimit = 100;
	const areaData areaData::NULLAreaData = areaData();
	const playerAreaData playerAreaData::NullPlayerAreaData = playerAreaData();
	const areaConfig areaConfig::NullConfig = areaConfig();
	//
	const static string worldMapTotalMapPageStr = "tpa";

	const static string worldDBNameStr = "gl.world_areas";
	const static string worldAreaIDStr = "id";
	const static string worldAreaPageStr = "pa";
	const static string worldAreaNextUpdateTimeStr = "nut";
	const static string worldAreaDataStr = "d";
	const static string worldPlayerIDStr = "pi";
	const static string worldPlayerNameStr = "na";
	const static string worldPlayerPopuStr = "pop";
	const static string worldPlayerFaceStr = "fid";
	const static string worldPlayerLevelStr = "plv";
	const static string worldPlanetIDStr = "pli";
	const static string worldFLagNameStr = "fn";
	const static string worldLeaveMessageStr = "lm";
	const static string worldHostileValueStr = "h";
	const static string worldProtectCDStr = "pcd";
	const static string worldFailedTodayStr = "ft";
	const static string worldBattleTodayStr = "bt";
	const static string worldBeHeatTodayStr = "bht";

	struct PlanetConfig
	{
		PlanetConfig()
		{
			id = -1;
			silver = -1;
			gold = -1;
		}
		bool isVaild()
		{
			return (id != -1 && silver != -1 && gold != -1);
		}
		int id;
		int silver;
		int gold;
	};

	static std::map<int ,PlanetConfig> PlanetMap;

	PlanetConfig getPlanetConfig(const int ID)
	{
		std::map<int ,PlanetConfig>::iterator it = PlanetMap.find(ID);
		if(it == PlanetMap.end())return PlanetConfig();
		return it->second;
	}

	Json::Value areaData::package()const
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		msg[msgStr][1u][worldAreaIDStr] = areaID;
		msg[msgStr][1u][worldAreaPageStr] = areaPage;
		msg[msgStr][1u][worldAreaDataStr] = Json::arrayValue;
		for (unsigned i = 0; i < areaVec.size(); ++i){
			Json::Value sw = Json::objectValue;
			if(areaVec[i] != playerAreaData::NullPlayerAreaData){			
				sw[worldPlayerIDStr] = areaVec[i].playerID;
				sw[worldPlayerLevelStr] = areaVec[i].playerLv;
				sw[worldPlayerNameStr] = areaVec[i].playerName;
				sw[worldPlayerPopuStr] = areaVec[i].population;
				sw[worldPlayerFaceStr] = areaVec[i].faceID;
				sw[worldPlanetIDStr] = areaVec[i].planetID;
				sw[worldFLagNameStr] = areaVec[i].flagName;
				sw[worldLeaveMessageStr] = areaVec[i].leaveMessage;
				sw[worldHostileValueStr][0u] = areaVec[i].hostileValue[0];
				sw[worldHostileValueStr][1u] = areaVec[i].hostileValue[1];
				sw[worldHostileValueStr][2u] = areaVec[i].hostileValue[2];
				sw[worldProtectCDStr] =  areaVec[i].protectCD;
				sw[worldFailedTodayStr] =  areaVec[i].failedToday;
				sw[worldBattleTodayStr] =  areaVec[i].battleToday;
				sw[worldBeHeatTodayStr] = areaVec[i].BeHeatToday;
			}
			msg[msgStr][1u][worldAreaDataStr].append(sw);
		}
		return msg;
	}

	world* const world::worldSys = new world();

	world::world()
	{
	}

	void world::updateMapPage(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int areaID = js_msg[0u].asInt();
		
		r[msgStr][0u] = 0;
		r[msgStr][1u][updateFromStr] = m._type;
		areaTotalPage::iterator it = areaPages.find(areaID);
		if(it == areaPages.end())
			r[msgStr][1u][worldMapTotalMapPageStr] = 0;
		else
			r[msgStr][1u][worldMapTotalMapPageStr] = it->second;
	}

	void world::updateClient(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int areaID = js_msg[0u].asInt();
		int areaPage = js_msg[1u].asInt();
		const areaConfig& config = getConfig(areaID);
		if(config == areaConfig::NullConfig)Return(r, -1);
		if(d->Base.getLevel() < config.minLevelLimit)Return(r, -1);
		const areaData& aD = getAreaData(areaKey(areaID, areaPage));
		if(aD ==areaData::NULLAreaData)Return(r, -1);
		r = aD.package();
	}

	void world::changePlanet(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if(d->Base.getLevel() < 20)Return(r, -1);
		ReadJsonArray;
		int planetID = js_msg[0u].asInt();
		PlanetConfig c = getPlanetConfig(planetID);
		if(!c.isVaild())Return(r, -1);
		PADPair pa = getPlayerAreaData(d);
		areaData* currentAreaData = pa.first;
		playerAreaData* currentPlayerAreaData = pa.second;
		if(currentAreaData == NULL || currentPlayerAreaData == NULL)Return(r, -1);
		if(c.gold >  0 && d->Base.getAllGold() < c.gold)Return(r, 1);//金币不足
		if(c.silver > 0 && d->Base.getSilver() < c.silver)Return(r, 2);//银币不足
		if(c.gold > 0)
		{
			d->Base.alterBothGold(-c.gold);
		}
		if(c.silver > 0)
		{
			d->Base.alterSilver(-c.silver);
		}
		currentPlayerAreaData->planetID = planetID;
		saveSingleArea(*currentAreaData);
		d->Base.planetID = planetID;
		d->Base.update();
		Json::Value uj = currentAreaData->package();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::world_update_part_resp, uj);
		Return(r, 0);
	}

	void world::motifyFlagName(msg_json& m, Json::Value& r)
	{		
		ReadJsonArray;
		string words = js_msg[0u].asString();
		if(words.length() > flagNameSizeLimit)Return(r, -1);
		AsyncGetPlayerData(m);
		PADPair pa = getPlayerAreaData(d);
		areaData* currentAreaData = pa.first;
		playerAreaData* currentPlayerAreaData = pa.second;
		if(currentAreaData == NULL || currentPlayerAreaData == NULL)Return(r, -1);
		currentPlayerAreaData->flagName = words;
		saveSingleArea(*currentAreaData);
		Json::Value uj = currentAreaData->package();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::world_update_part_resp, uj);
		Return(r, 0);
	}

	void world::motifyLeaveMessage(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		string words = js_msg[0u].asString();
		if(words.length() > leaveMessageSizeLimit)Return(r, -1);
		AsyncGetPlayerData(m);
		PADPair pa = getPlayerAreaData(d);
		areaData* currentAreaData = pa.first;
		playerAreaData* currentPlayerAreaData = pa.second;
		if(currentAreaData == NULL || currentPlayerAreaData == NULL)Return(r, -1);
		currentPlayerAreaData->leaveMessage = words;
		saveSingleArea(*currentAreaData);
		Json::Value uj = currentAreaData->package();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::world_update_part_resp, uj);
		Return(r, 0);
	}

	void world::battleAreaPlayer(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int targetPlayer = js_msg[0u].asInt();
		playerDataPtr targetP = player_mgr.getPlayerMain(targetPlayer);
		playerDataPtr d = player_mgr.getPlayerMain(m._player_id, m._net_id);
		if(playerDataPtr() == d || playerDataPtr() == targetP){
			if(m._post_times > 0)Return(r, -1);
			player_mgr.postMessage(m);
			return;
		}
		if(targetP == d)Return(r, -1);
		if(d->Campaign.getJunling() < 1)Return(r, 4);//军令不足
		if(d->Campaign.getCD().getCD().Lock)Return(r, 6);//CD中
		PADPair playerArea = getPlayerAreaData(d);
		if(playerArea.first == NULL || playerArea.second == NULL)Return(r, -1);
		if(playerArea.second->battleToday > 9)Return(r, 5);//今天战斗已经超过10次呢
		if(targetP == playerDataPtr())Return(r, -1);

		{
			const areaConfig& mineConfig = getConfig(d->Base.getPosition().first);
			if(mineConfig == areaConfig::NullConfig)Return(r, -1);
			const areaConfig& targetConfig = getConfig(targetP->Base.getPosition().first);
			if(targetConfig == areaConfig::NullConfig)Return(r, -1);
			if(mineConfig.minLevelLimit != targetConfig.minLevelLimit)Return(r, 1);//不允许越界PVP
		}

		PADPair targetArea = getPlayerAreaData(targetP);
		unsigned now = na::time_helper::get_current_time();
		if(targetArea.second == NULL || targetArea.second->protectCD > now)Return(r, 2);//保护CD
		if(d->isOperatBattle() || targetP->isOperatBattle())Return(r, 3);//等待战报反馈
		
		if(d->Base.getSphereID() == targetP->Base.getSphereID() || targetP->Base.getSphereID() < 0 || 
			d->Base.getSphereID() < 0) Return(r, -1);

		Battlefield field;
		field.atkSide = battleSide::Create(d);
		field.defSide = battleSide::Create(targetP);
		if(field.atkSide == battleSidePtr() || field.defSide == battleSidePtr())Return(r, -1);
		activity_sys.updateActivity(d, 0, activity::_attack_player);
		task_sys.updateBranchTaskInfo(d, _task_attack_other_country);
		battle_sys.PostBattle(field, battle_player_vs_player_type);
		d->Campaign.alterJunling(-1);
		d->Campaign.getCD().addCD(60);
	}

// 	void world::investedMyself(msg_json& m, Json::Value& r)
// 	{
// 		ReadJsonArray;
// 		int option = js_msg[0u].asInt();
// 		AsyncGetPlayerData(m);
// 		PADPair pa = getPlayerAreaData(d);
// 		if(pa.first == NULL || pa.second == NULL)Return(r, -1);
// 		d->Base.alterPopulation(1);
// 		Return(r, 0);
// 	}

	world::PADPair world::getPlayerAreaData(playerDataPtr player)
	{
		areaData* aDPtr = NULL;
		playerAreaData* pADPtr = NULL;
		PlayerPosition pos = player->Base.getPosition();
		areaData& refAD = getAreaData(areaKey(pos.first, pos.second));
		if(refAD == areaData::NULLAreaData) return PADPair(aDPtr, pADPtr);
		aDPtr = &refAD;
		areaData::areaVector& aV = aDPtr->areaVec;
		for (unsigned i = 0; i < aV.size(); ++i)
		{
			if(aV[i].playerID == player->playerID)
				return PADPair(aDPtr, &aV[i]);
		}
		return PADPair(aDPtr, pADPtr);
	}


	void world::insertNewAddress(playerDataPtr player, areaConfig& config, playerAreaData& pA)
	{
		bool insert = false;
		int areaID = config.areaID;
		int index = config.findIndex;
		do 
		{
			areaData& aD = getAreaData(areaKey(areaID, index));
			if(areaData::NULLAreaData == aD)break;
			if(aD.freePos > 0){
				areaData::areaVector& aV = aD.areaVec;
				for (unsigned i = 0; i < aV.size(); ++i)
					if(aV[i] == playerAreaData::NullPlayerAreaData){
						aV[i] = pA;
						insert = true;
						-- aD.freePos;
						saveSingleArea(aD);
						break;
				}
				if(insert)break;
			}
		} while (++index);
		if(!insert){
			areaData aD;
			aD.areaID = config.areaID;
			aD.areaPage = index;
			aD.areaVec[0] = pA;
			areaDatas[areaKey(aD.areaID, aD.areaPage)] = aD;
			saveSingleArea(aD);
			++(areaPages[aD.areaID]);
		}
		if(index < config.findIndex && index >= 0)config.findIndex = index;
		player->Base.setPosition(areaID, index);
	}

	void world::removeOldAddress(playerDataPtr player, areaConfig& config, playerAreaData& pA)
	{
		PlayerPosition pos = player->Base.getPosition();
		int aID = pos.first;
		int aPage = pos.second;
		if(aID < 0 || aPage < 0) return;
		PADPair pa = getPlayerAreaData(player);
		areaData* currentAreaData = pa.first;
		playerAreaData* currentPlayerAreaData = pa.second;
		if(currentAreaData == NULL || currentPlayerAreaData == NULL)return;
		player->Base.setPosition(-1, -1);
		pA.planetID = currentPlayerAreaData->planetID;
		pA.leaveMessage = currentPlayerAreaData->leaveMessage;
		pA.flagName = currentPlayerAreaData->flagName;
		memcpy(pA.hostileValue, currentPlayerAreaData->hostileValue, sizeof(pA.hostileValue));
		pA.battleToday = currentPlayerAreaData->battleToday;
		pA.failedToday = currentPlayerAreaData->failedToday;
		pA.BeHeatToday = currentPlayerAreaData->BeHeatToday;
		*currentPlayerAreaData = playerAreaData();
		++currentAreaData->freePos;
		if(aPage < config.findIndex && aPage >= 0) config.findIndex = aPage;
		saveSingleArea(*currentAreaData);
	}

	//0 continue 1 stop
	int world::movingAreaSystem(playerDataPtr player, const int areaID)
	{
		areaConfig& config = getConfigPrivate(areaID);
		if(config == areaConfig::NullConfig)return 0;
		int playerLv = player->Base.getLevel();
		if(playerLv < config.minLevelLimit) return 0;
		if(player->Base.getSphereID() != config.subjectCamp) return 0;
		if(areaID == player->Base.getPosition().first)return 1;//不允许同一个aID多次转移
		playerAreaData pA;
		pA.playerName = player->Base.getName();
		pA.faceID = player->Base.getFaceID();
		pA.playerID = player->playerID;
		pA.playerLv = player->Base.getLevel();
		pA.population = player->Base.getCalPopu();
		areaConfig& oldConfig = getConfigPrivate(player->Base.getPosition().first);
		if(!(areaConfig::NullConfig == oldConfig))
		{
			removeOldAddress(player, oldConfig, pA);
		}
		insertNewAddress(player, config, pA);
		return 1;
	}

	void world::checkMoving(playerDataPtr player)
	{
		for (unsigned i = 0; i < Vector.size(); ++i)
		{
			if(movingAreaSystem(player, Vector[i]) == 1)break;
		}
	}

	void alterHostileValue(int& value, const int num)
	{
		value += num;
		value =  value < -50 ? -50 : value;
		value = value > 100 ? 100 : value;
	}

	int createHostileValue(const int value)
	{
		if(value < 55)return 1;
		return 3;
	}

	unsigned createProtectCDBase(const int value)
	{
		unsigned base = 0;
		if(value > 90)base = 0;
		else if(value > 35)base = 360;
		else if(value > 10)base = 420;
		else if(value > 5)base = 480;
		else if(value  > -5)base = 600;
		else if(value > -10)base = 720;
		else if(value > -25)base = 900;
		else base = 1200;
		return base;
	}

	void world::wPVPDeal(playerDataPtr player, playerDataPtr target, const int battleResult)
	{
		if(player == NULL || target == NULL)return;
		PADPair mine = getPlayerAreaData(player);
		if(mine.first == NULL || mine.second == NULL)return;
		PADPair enemy = getPlayerAreaData(target);
		if(enemy.first == NULL || enemy.second == NULL)return;
		++mine.second->battleToday;
		 ++enemy.second->BeHeatToday;
		do 
		{
			if(battleResult != 1)break;
			++enemy.second->failedToday;
			target->Base.alterPopulation(-PvPPopulation[target->Base.getLevel()].asInt());
			int pki = player->Base.getSphereID();
			int tki = target->Base.getSphereID();
			if(pki < 0 || tki < 0 || pki > 2 || tki > 2)
			{
				LogE << "wPVP error... no invaild kingdomID !!! player : " << pki << " target : " << tki << LogEnd; 
				break;
			}
			if(enemy.second->hostileValue[pki] > 10)
			{
				int channelID = chat_area;
				if(enemy.second->hostileValue[pki] >= 100)channelID = chat_all;
				else if(enemy.second->hostileValue[pki] > 90)channelID = chat_kingdom;

				Json::Value bCast;
				bCast[msgStr][0u] = 0;
				bCast[msgStr][1u][senderChannelStr] = channelID;
				bCast[msgStr][1u][chatBroadcastID] = BROADCAST::world_pvp_broadcast;
				const static string ParamListStr = "pl";
				bCast[msgStr][1u][ParamListStr].append(player->Base.getName());
				bCast[msgStr][1u][ParamListStr].append(tki);
				bCast[msgStr][1u][ParamListStr].append(target->Base.getName());
				if (channelID == chat_area)player_mgr.sendToArea(gate_client::chat_broadcast_resp, bCast, player->Base.getPosition().first);
				else if (channelID == chat_kingdom)player_mgr.sendToSphere(gate_client::chat_broadcast_resp, bCast, player->Base.getSphereID());
				else player_mgr.sendToAll(gate_client::chat_broadcast_resp, bCast);
			}
			int hval = createHostileValue(enemy.second->hostileValue[pki]);
			unsigned bcd = createProtectCDBase(enemy.second->hostileValue[pki]);
			alterHostileValue(mine.second->hostileValue[tki], hval);
			alterHostileValue(enemy.second->hostileValue[pki], -hval);
			unsigned ccd = unsigned(bcd * (1 + enemy.second->failedToday * 0.17));
			ccd = ccd > 7200 ? 7200 : ccd;
			enemy.second->protectCD = na::time_helper::get_current_time() + ccd;
		} while (false);
		saveSingleArea(*mine.first);
		saveSingleArea(*enemy.first);
	}

	void world::asyncPlayerData(playerDataPtr player)
	{
		if(playerDataPtr() == player)return;
		PADPair ppair = getPlayerAreaData(player);
		if(ppair.first == NULL || ppair.second == NULL)return;
		ppair.second->playerName = player->Base.getName();
		ppair.second->playerLv = player->Base.getLevel();
		ppair.second->faceID = player->Base.getFaceID();
		ppair.second->population = player->Base.getCalPopu();
		saveSingleArea(*ppair.first);
	}

	int world::getPlayerPlanetID(playerDataPtr player)
	{
		PADPair p = getPlayerAreaData(player);
		if(p.second != NULL)return p.second->planetID;
		return -1;
	}

	const areaConfig& world::getConfig(const int areaID)
	{
		areaConfigMap::iterator it = areaConfigs.find(areaID);
		if(it == areaConfigs.end())return areaConfig::NullConfig;
		return it->second;
	}

	areaConfig& world::getConfigPrivate(const int areaID)
	{
		static areaConfig NullConfig = areaConfig();
		areaConfigMap::iterator it = areaConfigs.find(areaID);
		if(it == areaConfigs.end())return (NullConfig = areaConfig());
		return it->second;
	}

	void createNextUpdateTime(unsigned& time)
	{
		unsigned now = na::time_helper::get_current_time();
		tm t = na::time_helper::toTm(now);
		time = na::time_helper::get_time_zero(now) + 5 * 3600;
		if(t.tm_hour >= 5)time += na::time_helper::ONEDAY;
	}

	areaData& world::getAreaData(const areaKey& key)
	{
		static areaData NullData = areaData();
		areaDataMap::iterator it = areaDatas.find(key);
		if(it == areaDatas.end())return (NullData = areaData());
		if(na::time_helper::get_current_time() > it->second.nextUpdateTime)
		{
			areaData::areaVector& aV = it->second.areaVec;
			for (unsigned i = 0; i < aV.size(); ++i)
			{
				aV[i].clearOldData();
			}
			createNextUpdateTime(it->second.nextUpdateTime);
		}
		return it->second;
	}

	bool world::saveSingleArea(areaData& aD)
	{
		BSONObj key = BSON(worldAreaIDStr << aD.areaID << worldAreaPageStr << aD.areaPage);
		BSONObjBuilder objBuild;
		objBuild << worldAreaIDStr << aD.areaID << worldAreaPageStr << aD.areaPage  << worldAreaNextUpdateTimeStr << aD.nextUpdateTime ;
		BSONArrayBuilder arrBuild;
		for(unsigned i = 0; i < aD.areaVec.size(); ++i)
		{
			const playerAreaData& currentData = aD.areaVec[i];
			BSONArrayBuilder arr;
			arr << currentData.hostileValue[0] << currentData.hostileValue[1] << currentData.hostileValue[2]; 
			BSONObjBuilder obj;
			obj << worldPlayerIDStr << currentData.playerID << worldPlanetIDStr << currentData.planetID << worldPlayerLevelStr << currentData.playerLv <<
				worldFLagNameStr << currentData.flagName << worldLeaveMessageStr << currentData.leaveMessage <<
				worldHostileValueStr << arr.arr() << worldProtectCDStr << currentData.protectCD << worldPlayerNameStr <<
				currentData.playerName << worldFailedTodayStr << currentData.failedToday << worldBattleTodayStr << currentData.battleToday <<
				worldPlayerFaceStr << currentData.faceID << worldPlayerPopuStr << currentData.population << worldBeHeatTodayStr << currentData.BeHeatToday;
			arrBuild << obj.obj();
		}
		objBuild << worldAreaDataStr <<arrBuild.arr();

		return db_mgr.save_mongo(worldDBNameStr, key, objBuild.obj());
	}	

	void world::loadAllAreaData()
	{
		objCollection objC = db_mgr.Query(worldDBNameStr);
		LogS << "load " << color_yellow(objC.size()) << " area info from DB" << LogEnd;
		for (unsigned i = 0; i < objC.size(); ++ i){
			BSONObj& obj = objC[i];
			areaData aD;
			checkNotEoo(obj[worldAreaIDStr]){aD.areaID =  obj[worldAreaIDStr].Int();}
			areaConfig& cConfig = getConfigPrivate(aD.areaID);
			if(cConfig == areaConfig::NullConfig)continue;
			checkNotEoo(obj[worldAreaPageStr]){aD.areaPage =  obj[worldAreaPageStr].Int();}
			checkNotEoo(obj[worldAreaNextUpdateTimeStr]){aD.nextUpdateTime =  (unsigned)obj[worldAreaNextUpdateTimeStr].Int();}
			checkNotEoo(obj[worldAreaDataStr]){
				vector<BSONElement> els = obj[worldAreaDataStr].Array(); 
				for (unsigned n = 0; n < els.size() && n < aD.areaVec.size(); ++n)
				{
					BSONElement& sobj = els[n];
					playerAreaData pa;
					checkNotEoo(sobj[worldPlayerIDStr]){pa.playerID =  sobj[worldPlayerIDStr].Int();}
					if(pa.playerID == -1)continue;
					checkNotEoo(sobj[worldPlanetIDStr]){pa.planetID =  sobj[worldPlanetIDStr].Int();}
					checkNotEoo(sobj[worldPlayerNameStr]){pa.playerName =  sobj[worldPlayerNameStr].String();}
					checkNotEoo(sobj[worldPlayerFaceStr]){pa.faceID =  sobj[worldPlayerFaceStr].Int();}
					checkNotEoo(sobj[worldPlayerPopuStr]){pa.population =  sobj[worldPlayerPopuStr].Int();}
					checkNotEoo(sobj[worldPlayerLevelStr]){pa.playerLv =  sobj[worldPlayerLevelStr].Int();}
					checkNotEoo(sobj[worldProtectCDStr])
					{
						int cd = sobj[worldPlanetIDStr].Int();
						if (cd < 0)pa.protectCD = 0;
						else pa.protectCD = (unsigned)cd;
					}
					checkNotEoo(sobj[worldFLagNameStr]){pa.flagName =  sobj[worldFLagNameStr].String();}
					checkNotEoo(sobj[worldLeaveMessageStr]){pa.leaveMessage =  sobj[worldLeaveMessageStr].String();}
					checkNotEoo(sobj[worldFailedTodayStr]){pa.failedToday =  sobj[worldFailedTodayStr].Int();}
					checkNotEoo(sobj[worldBattleTodayStr]){pa.battleToday =  sobj[worldBattleTodayStr].Int();}
					checkNotEoo(sobj[worldBeHeatTodayStr]){pa.BeHeatToday =  sobj[worldBeHeatTodayStr].Int();}
					checkNotEoo(sobj[worldHostileValueStr]){
						vector<BSONElement> arrs = sobj[worldHostileValueStr].Array(); 
						for (unsigned index = 0; index < arrs.size() && index < 3; ++ index)
							pa.hostileValue[index] = arrs[index].Int();
					}
					if(pa != playerAreaData::NullPlayerAreaData)--aD.freePos;
					aD.areaVec[n] = pa;
				}
			}
			++(areaPages[aD.areaID]);
			if(aD.areaPage >= 0 && aD.areaPage < cConfig.findIndex && aD.freePos > 0) 
				cConfig.findIndex = aD.areaPage;
			areaDatas[areaKey(aD.areaID, aD.areaPage)] = aD;
		}
	}	
	
	const static string worldSystemDataDirStr = "./instance/world/";;  
	const static string configAreaIDStr = "areaID";
	const static string configMinLevelLimitStr = "minLevelLimit";
	const static string configSubjectCampStr = "subjectCamp";

	typedef boost::unordered_set<size_t> TMPSET;



	void world::initData()
	{
		db_mgr.ensure_index(worldDBNameStr, BSON(worldAreaIDStr << 1 << worldAreaPageStr << 1));

		{
			Json::Value val = na::file_system::load_jsonfile_val("./instance/world_extern/pvpPopulation.json");
			Params::ArrayValue arr(val);
			PvPPopulation = arr;
		}

		Json::Value PlanetJson = na::file_system::load_jsonfile_val("./instance/world_extern/starInfoConfig.json");
		for (unsigned i  = 0; i < PlanetJson.size(); ++i)
		{
			PlanetConfig c;
			c.id = PlanetJson[i]["starID"].asInt();
			c.silver = PlanetJson[i]["silver"].asInt();
			c.gold = PlanetJson[i]["gold"].asInt();
			PlanetMap[c.id] = c;
		}

		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(worldSystemDataDirStr, jv);
		TMPSET tmps;
		vector<size_t> vec;
		for (unsigned i = 0; i < jv.size(); ++i)
		{
			Json::Value& worldConfig = jv[i];

			areaConfig ac;
			ac.areaID = worldConfig[configAreaIDStr].asInt();
			ac.minLevelLimit = worldConfig[configMinLevelLimitStr].asInt();
			ac.subjectCamp = worldConfig[configSubjectCampStr].asInt();

			areaPages[ac.areaID] = 0;
			areaConfigs[ac.areaID] = ac;

			tmps.insert(ac.minLevelLimit);
		}
		for (TMPSET::iterator it = tmps.begin(); it != tmps.end(); ++it )
		{
			vec.push_back(*it);
		}
		std::sort(vec.begin(), vec.end(), greater<size_t>());
		unsigned index = 0;
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			int lv = vec[i];
			for (areaConfigMap::iterator it = areaConfigs.begin(); it != areaConfigs.end();++it)
			{
				if(it->second.minLevelLimit == lv)Vector.push_back(it->second.areaID);
			}
		}
		loadAllAreaData();
	}

}