#include "player_war_story.h"
#include "war_story.h"
#include "helper.h"
#include "battle_def.h"
#include "record_system.h"
#include "guild_system.h"
using namespace mongo;

namespace gg
{
	playerWarStory::playerWarStory(playerData& own, const int mID) : 
		Block(own),  mapID(mID)
	{
		father = NULL;
		playerLocal_.clear();
		chaper1Reward = false;
		chaper2Reward = false;
		chaper3Reward = false;
	}

	void playerWarStory::autoUpdate()
	{
		update();
	}

	void playerWarStory::SetChaper3RW(const bool val)
	{
		{chaper3Reward = val; helper_mgr.insertSaveAndUpdate(this); }
	}

	void playerWarStory::SetChaper2RW(const bool val)
	{
		{chaper2Reward = val;helper_mgr.insertSaveAndUpdate(this);}
	}

	void playerWarStory::SetChaper1RW(const bool val)
	{
		{chaper1Reward = val;helper_mgr.insertSaveAndUpdate(this);}
	}

	int playerWarStory::getStarNum()
	{
		int star = 0;
		for (playerLocalMap::const_iterator it = playerLocal_.begin(); it != playerLocal_.end(); ++it)
		{
			star += (it->second.pastStar);
		}
		return star;
	}

	bool playerWarStory::isProcessLocal(const int localID)
	{
		playerWarStory::playerLocalMap::iterator localMap = playerLocal_.find(localID);
		if(localMap == playerLocal_.end())
			return false;
		return true;
	}
	const static string mysqlLogPlayerProcess = "log_player_process";
	const static string mysqlLogPlayerWarStar = "log_player_war_star";
	void playerWarStory::processMapData(const int localID, Battlefield field)
	{
		if(localID / mapIDOffset != mapID)return;
		playerLocalMap::iterator it = playerLocal_.find(localID);
		mapDataPtr mD = war_story.getMapData(localID);
		if(mD == NULL)return;
		int star = field.atkSide->getStar();
		StreamLog::Log(mysqlLogPlayerWarStar, own, localID, star);
		if(it == playerLocal_.end() || it->second.pastStar < star){
			playerLocal_[localID] = playerMapData(localID, star);
			int tempProcess = father->currentProcess;
			father->currentProcess = localID > tempProcess ? localID : tempProcess;
			if(tempProcess != father->currentProcess){
				guild_sys.asyncPlayerData(own.getOwnDataPtr());
				StreamLog::Log(mysqlLogPlayerProcess, own, tempProcess, father->currentProcess);
			}
			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	void playerWarStory::processMapData(const int localID, const int star)
	{
		if (star < 1 || star > 5)return;
		if (localID / mapIDOffset != mapID)return;
		playerLocalMap::iterator it = playerLocal_.find(localID);
		mapDataPtr mD = war_story.getMapData(localID);
		if (mD == NULL)return;
		StreamLog::Log(mysqlLogPlayerWarStar, own, localID, star);
		if (it == playerLocal_.end() || it->second.pastStar < star){
			playerLocal_[localID] = playerMapData(localID, star);
			int tempProcess = father->currentProcess;
			father->currentProcess = localID > tempProcess ? localID : tempProcess;
			if (tempProcess != father->currentProcess){
				guild_sys.asyncPlayerData(own.getOwnDataPtr());
				StreamLog::Log(mysqlLogPlayerProcess, own, tempProcess, father->currentProcess);
			}
			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	void playerWarStory::update()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		msg[msgStr][1u][playerWarStoryMapIDStr] = mapID;
		msg[msgStr][1u][playerWarStoryDataStr] = Json::arrayValue;
		for (playerLocalMap::iterator it = playerLocal_.begin(); it != playerLocal_.end(); ++it)
		{
			Json::Value m;
			m[playerWarStoryIDStr] = it->second.localID;
			m[playerWarStoryPastStarStr] = it->second.pastStar;
			msg[msgStr][1u][playerWarStoryDataStr].append(m);
		}
		Json::Value& rewardJson = msg[msgStr][1u][playerChaperRewardStr];
		rewardJson = Json::arrayValue;
		rewardJson.append(chaper1Reward);
		rewardJson.append(chaper2Reward);
		rewardJson.append(chaper3Reward);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::war_story_update_resp, msg);
	}

	bool playerWarStory::get()
	{
		string dbName = playerWarStoryDBStr + boost::lexical_cast<string,int>(mapID);
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(dbName, key);
		if(! obj.isEmpty()){
			playerLocal_.clear();
			checkNotEoo(obj[playerMapRewardStr]){chaper1Reward = obj[playerMapRewardStr].Bool();}
			checkNotEoo(obj[playerChaperRewardStr]){chaper3Reward = obj[playerChaperRewardStr].Bool();}
			checkNotEoo(obj[playerChaperReward2Str]){ chaper2Reward = obj[playerChaperReward2Str].Bool(); }
			checkNotEoo(obj[playerWarStoryDataStr]){
				vector<BSONElement> els = obj[playerWarStoryDataStr].Array();
				for (unsigned i = 0; i < els.size(); ++i)
				{
					playerMapData md;
					md.localID = els[i][playerWarStoryIDStr].Int();
					md.pastStar = els[i][playerWarStoryPastStarStr].Int();
					father->currentProcess = md.localID > father->currentProcess 
						? md.localID : father->currentProcess;
					playerLocal_[md.localID] = md;
				}
			}else{
				return false;
			}
			return true;
		}
		return false;
	}

	bool playerWarStory::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID << playerMapRewardStr << chaper1Reward
			 << playerChaperRewardStr << chaper3Reward << playerChaperReward2Str << chaper2Reward;
		mongo::BSONArrayBuilder array_warStory;
		for (playerLocalMap::iterator it = playerLocal_.begin(); it != playerLocal_.end(); ++it)
		{
			int currentMapID = it->first / mapIDOffset;
			if(currentMapID != mapID)
				continue;
			mongo::BSONObjBuilder b;
			b << playerWarStoryIDStr <<  it->second.localID << playerWarStoryPastStarStr << it->second.pastStar;
			array_warStory.append(b.obj());
		}
		obj << playerWarStoryDataStr << array_warStory.arr();
		return db_mgr.save_mongo(playerWarStoryDBStr + boost::lexical_cast<string,int>(mapID), key, obj.obj());
	}

	playerAllWarStory::playerAllWarStory(playerData& own) : Block(own)
	{
		currentProcess = -1;
	}

	bool playerAllWarStory::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(playerWarStoryLastInfoDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[playerLastWarStroyTargetIDStr]){lastTargetID = obj[playerLastWarStroyTargetIDStr].Int();}
			checkNotEoo(obj[playerLastWarStroyTargetNameStr]){lastTargetName = obj[playerLastWarStroyTargetNameStr].String();}
		}
		load();
		return true;
	}

	bool playerAllWarStory::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = BSON(playerIDStr << own.playerID << 
			playerLastWarStroyTargetIDStr << lastTargetID <<
			playerLastWarStroyTargetNameStr << lastTargetName); 
		return db_mgr.save_mongo(playerWarStoryLastInfoDBStr, key, obj);
	}

	bool playerAllWarStory::isProcessMap(const int localID)
	{
		if(localID < 0)return true;
		int mapID = localID / mapIDOffset;
		playerWMMap::iterator it = playerMap_.find(mapID);
		if(it == playerMap_.end())	return false;
		return it->second->isProcessLocal(localID);
	}

	void playerAllWarStory::load()
	{
		playerMap_.clear();
		vector<int> mapCollection =  war_story.getAllMapID();
		for (unsigned i = 0; i < mapCollection.size(); ++i)
		{
			playerWarStoryPtr ptr = Create(own, mapCollection[i]);
			if(ptr == playerWarStoryPtr())continue;
			ptr->setHandler(this);
			if(ptr->get()) playerMap_[ptr->mapID] = ptr;
		}

		getTeam(own);
	}

	int playerAllWarStory::updateClientMap(const int mapID)
	{
		int res = -1;
		do 
		{
			playerWMMap::iterator it = playerMap_.find(mapID);
			if(it == playerMap_.end())break;
			it->second->update();
			res = 0;
		} while (false);
		return res;
	}

	playerWarStoryPtr playerAllWarStory::getMap(const int mapID)
	{
		playerWMMap::iterator it = playerMap_.find(mapID);
		if(it == playerMap_.end())return playerWarStoryPtr();
		return it->second;
	}

	void playerAllWarStory::SetLastWSInfo(const int tID, const string tName)
	{
		lastTargetID = tID;
		lastTargetName = tName;
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerAllWarStory::processWarStory(const int localID, const int star)
	{
		if (war_story.getMapData(localID) == NULL)return;
		int mapID = localID / mapIDOffset;
		playerWarStoryPtr ptr = getMap(mapID);
		if (ptr == playerWarStoryPtr()){
			ptr = Create(own, mapID);
			if (ptr == playerWarStoryPtr())return;
			ptr->setHandler(this);
			playerMap_[mapID] = ptr;
		}
		ptr->processMapData(localID, star);
	}

	void playerAllWarStory::processWarStory(playerDataPtr own, Battlefield field)
	{
		int localID = field.defSide->getSideID();
		int mapID = localID / mapIDOffset;
		playerWarStoryPtr ptr = getMap(mapID);
		if(ptr == playerWarStoryPtr()){
			ptr = Create(*own.get(), mapID);
			if(ptr == playerWarStoryPtr())return;
			ptr->setHandler(this);
			playerMap_[mapID] = ptr;
		}
		ptr->processMapData(localID, field);
	}

	bool playerAllWarStory::saveTeam(playerData& own)
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;

		obj << playerIDStr << own.playerID;

		mongo::BSONArrayBuilder mapArr;
		for (playerWMTMap::iterator it = playerTMap_.begin(); it != playerTMap_.end(); it++)
		{
			mongo::BSONObjBuilder mapItem;
			mapItem << playerWarStoryIDStr << it->second.mapID_;
			mapItem << playerWarStoryNpcTeamFirstBloodStr << it->second.hasNpcTeamFirstBlood_;
			mapArr << mapItem.obj();
		}

		obj << playerWarStoryDataStr << mapArr.arr();

		return db_mgr.save_mongo(playerWarStoryTeamDBStr, key, obj.obj());
	}

	bool playerAllWarStory::getTeam(playerData& own)
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(playerWarStoryTeamDBStr, key);

		playerTMap_.clear();

		vector<int> mapTCollection =  war_story.getAllTeamMapID();
		for (unsigned i = 0; i < mapTCollection.size(); ++i)
		{
			playerTeamMapData tmd = playerTeamMapData(mapTCollection[i], false);
			playerTMap_.insert(make_pair(tmd.mapID_, tmd));
		}

		checkNotEoo(obj[playerWarStoryDataStr])
		{
			for (unsigned i = 0; i < obj[playerWarStoryDataStr].Array().size(); i++)
			{
				mongo::BSONElement temp = obj[playerWarStoryDataStr].Array()[i];
				playerTMap_[temp[playerWarStoryIDStr].Int()].hasNpcTeamFirstBlood_ = temp[playerWarStoryNpcTeamFirstBloodStr].Bool();
			}
			return true;
		}
		return false;
	}

	void playerAllWarStory::autoUpdateTeam(int mapID, playerData& own)
	{
		Json::Value packJson;
		packageTeam(mapID, own, packJson);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::war_story_team_map_update_resp, packJson);
//		cout << "packJson:" << packJson.toStyledString() << endl;
	}

	void playerAllWarStory::packageTeam(int mapID, playerData& own, Json::Value &packJson)
	{
		packJson[msgStr][0u] = 0;
		packJson[msgStr][1][playerWarStoryIDStr] = mapID;
		packJson[msgStr][1][playerWarStoryNpcTeamFirstBloodStr] = playerTMap_[mapID].hasNpcTeamFirstBlood_;
	}

	bool playerAllWarStory::hasFirstBlood( int mapID )
	{
		return playerTMap_[mapID].hasNpcTeamFirstBlood_;
	}

	void playerAllWarStory::setFirstBlood(int mapID)
	{
		playerTMap_[mapID].hasNpcTeamFirstBlood_ = true;
		saveTeam(own);
		autoUpdateTeam(mapID, own);
	}


	playerTeamMapData::playerTeamMapData()
	{
		hasNpcTeamFirstBlood_ = false;
	}

	playerTeamMapData::playerTeamMapData(int mapID, bool firstBlood)
	{
		mapID_ = mapID;
		hasNpcTeamFirstBlood_ = firstBlood;
	}

}