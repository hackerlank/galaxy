#include "file_system.h"
#include "embattle_sys.h"
#include "player_embattle.h"
#include "playerManager.h"
#include "helper.h"
#include "record_system.h"
#include "arena_system.h"

using namespace mongo;

namespace gg
{
	const static string logEmbattle = "log_embattle";
	enum{
		log_tag_active_embattle,
		log_tag_current_embattle,
		log_tag_format_embattle,
	};


	playerSingleEmbattle::playerSingleEmbattle() : fomationID(-1)
	{
		battleValue = 0;
		for (unsigned i = 0; i < 9; ++i)
			fomation[i] = -1;
	}

	playerSingleEmbattle::playerSingleEmbattle(const int emID) : fomationID(emID)
	{
		battleValue = 0;
		for (unsigned i = 0; i < 9; ++i)
			fomation[i] = -1;
	}

	playerSingleEmbattle& playerSingleEmbattle::operator =(const playerSingleEmbattle& _source)
	{
		memcpy(this, &_source, sizeof(playerSingleEmbattle));
		return *this;
	}

	const static string playerEmbattleDataStr = "d";
	const static string playerFormatIDStr = "id";
	const static string playerFormationStr = "f";
	const static string playerCurrenFormatIDStr = "cid";
	const static string playerEmbattleValueStr = "bv";

#define NullEmbattleIterator embattleMap.end()

#define EmbattleIterator playerEmbattleMap::iterator

	bool hasSameID(int* fm)
	{
		std::set<int> one;
		for (int i = 0 ; i < 9; ++i)
		{
			int id = fm[i];
			if(id < 1)continue;
			if(!one.insert(id).second)return true;
		}
		return false;
	}

	int playerEmbattle::setFormation(int fID, int fm[9])
	{
		if(hasSameID(fm))return -1;
		EmbattleIterator it = getMineEmbattle(fID);
		if(it == NullEmbattleIterator)return -1;

		const embattleData& eD = embattle_sys.getEMbattle(fID);
		if(eD == NullEmbattleData)return -1;
		if(own.Base.getLevel() < eD.levelLimit) return 1;
		for (int i =0; i < 9; ++i)
		{
			int pID = fm[i];
			const playerPilot& cPilot = own.Pilots.getPlayerPilotExtert(pID);
			if(cPilot == playerPilots::NullPilotExtert){
				fm[i] = -1;
				continue;
			}
			if(cPilot.pilotType != pilot_enlist)return 2;
			if(! eD.canUseHole(i, own.getOwnDataPtr()))return 3;
		}
		playerSingleEmbattle& cFm = it->second;
		memcpy(cFm.fomation, fm, 9 * sizeof(int));
		string str;
		for (int i =0; i < 9; ++i)
		{
			int pID = fm[i];
			str += boost::lexical_cast<string, int>(pID) + ",";
		}
		StreamLog::Log(logEmbattle, own, fID, str, log_tag_format_embattle);
		setCFormatID(fID)	;
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	void playerEmbattle::setCFormatID(const int embattleID)
	{
		currentFormatID = embattleID;

		EmbattleIterator it = getMineEmbattle(embattleID);
		if (it == NullEmbattleIterator)return;
		updateSingleBattleValue(it->second);

		playerSingleEmbattle& cFm = it->second;
		string str;
		for (int i = 0; i < 9; ++i)
		{
			int pID = cFm.fomation[i];
			str += boost::lexical_cast<string, int>(pID)+",";
		}
		vector<string> fds;
		fds.push_back(str);
		StreamLog::Log(logEmbattle, own, "SetDefalut", boost::lexical_cast<string, int>(currentFormatID), fds, log_tag_current_embattle);
	}

	int playerEmbattle::setCurrentFormat(const int embattleID)
	{
		EmbattleIterator it = getMineEmbattle(embattleID);
		if(it == NullEmbattleIterator)return -1;
		setCFormatID(embattleID);
		return 0;
	}

	int playerEmbattle::getCurrentFormat()
	{
		return currentFormatID;
	}

	void playerEmbattle::activeEmbattle(const int embattleID)
	{
		const embattleData config = embattle_sys.getEMbattle(embattleID);
		activeEmbattle(config);
	}

	void playerEmbattle::activeEmbattle(const embattleData& embattleC)
	{
		if(activeEMbattleWithOutSave(embattleC))
		{
			helper_mgr.insertSaveAndUpdate(this);
			StreamLog::Log(logEmbattle, own, "active", embattleC.emID, log_tag_active_embattle);
		}
	}

	bool playerEmbattle::activeEMbattleWithOutSave(const embattleData& embattleC)
	{
		if(embattleC == NullEmbattleData)return false;
		const int ID = embattleC.emID;
		EmbattleIterator it = getMineEmbattle(ID);
		if(it != NullEmbattleIterator)return false;
		int level = own.Base.getLevel();
		vector<int> tmp = getCurrentFormatPilot();
		vector<int> ls;
		for (unsigned  i = 0; i < tmp.size(); ++i)
		{
			if(tmp[i] <= 0)continue;
			ls.push_back(tmp[i]);
		}
		playerSingleEmbattle em(ID);
		const vector<embattleHole>& holeConfig = embattleC.holeConfig;
		for (unsigned i = 0, j = 0; i < holeConfig.size() && j < ls.size(); ++i)
		{
			if(level < holeConfig[i].levelLimit && own.Warstory.currentProcess < holeConfig[i].holeOpen)continue;
			em.fomation[holeConfig[i].holeIndex] = ls[j];
			++j;
		}
		updateSingleBattleValue(em);
		embattleMap[ID] = em;
		return true;
	}


	bool playerEmbattle::isNullFormation()
	{
		EmbattleIterator it = getMineEmbattle(currentFormatID);
		if(it == NullEmbattleIterator)
			return true;

		int armyNum = 0;
		for (int i = 0; i < 9; ++i)
		{
			int pilotID = it->second.fomation[i];
			if(own.Pilots.canBattle(pilotID))++armyNum;
		}

		if(armyNum > 0)
			return false;

		return true;
	}

	bool playerEmbattle::isHasFormation(const int formatID)
	{
		return getMineEmbattle(formatID) != NullEmbattleIterator;
	}

	playerEmbattle::playerEmbattle(playerData& own) : Block(own)
	{
		if(embattle_sys.getEMbattle(beginEMID) == NullEmbattleData)return;
		currentFormatID = beginEMID;
		playerSingleEmbattle em(beginEMID);
		em.fomation[4] = startPilotID;
		em.battleValue = own.Pilots.getPlayerPilotExtert(startPilotID).battleValue;
		embattleMap[beginEMID] = em;
	}

	vector<int> playerEmbattle::getCurrentFormatPilot()
	{
		return getFormatPilot(currentFormatID);
	}

	vector<int> playerEmbattle::getFormatPilot(const int embattleID)
	{
		vector<int> vec;
		playerEmbattleMap::const_iterator it = getMineEmbattle(embattleID);
		if(it != NullEmbattleIterator){
			const playerSingleEmbattle& playerEm = it->second;
			for(unsigned i = 0; i < 9; ++i)
			{
//				if(playerEm.fomation[i] <= 0)continue;
				vec.push_back(playerEm.fomation[i]);
			}
		}
		return vec;
	}

	void playerEmbattle::updateSingleBattleValue(playerSingleEmbattle& embattle, const bool tick /* = true */)
	{
		int num = 0;
		embattle.battleValue = 0;
		for (unsigned i = 0; i < 9; ++i)
		{
			const playerPilot& cPilot = own.Pilots.getPlayerPilotExtert(embattle.fomation[i]);
			embattle.battleValue += cPilot.battleValue;
			if (!(cPilot == playerPilots::NullPilotExtert))
			{
				++num;
			}
		}
		embattle.battleValue += (own.Science.embattleLV(embattle.fomationID) * 100 * num);
		embattle.battleValue = embattle.battleValue < 0 ? 0 : embattle.battleValue;
		embattle.battleValue = embattle.battleValue > 4000000000 ? 4000000000 : embattle.battleValue;
		if (tick && getCurrentFormat() == embattle.fomationID)
		{
			arena_sys.updatePower(own.getOwnDataPtr());
		}
	}

	int playerEmbattle::getCurrentBV()
	{
		EmbattleIterator it = getMineEmbattle(getCurrentFormat());
		if (it == NullEmbattleIterator)return 0;
		return it->second.battleValue;
	}

	void playerEmbattle::updateBattleValue(bool client /* = true */)
	{
		for (playerEmbattleMap::iterator it = embattleMap.begin();
			it != embattleMap.end(); ++it)
		{
			playerSingleEmbattle& embattle = it->second;
			updateSingleBattleValue(embattle);
		}
		if (client)
		{
			helper_mgr.insertUpdate(this);
		}
	}

	void playerEmbattle::removeFirePilot(const int pilotID)
	{
		bool update = false;
		for (playerEmbattleMap::iterator it = embattleMap.begin(); 
			it != embattleMap.end();++it)
		{
			playerSingleEmbattle& embattle = it->second;
			bool remove = false;
			for(unsigned i = 0; i < 9; ++i)
			{
				if(embattle.fomation[i] != pilotID)continue;
				embattle.fomation[i] = -1;
				update = true;
				remove = true;
				break;
			}
			if(remove)
			{
				string str;
				for (int i =0; i < 9; ++i)
				{
					int pID = embattle.fomation[i];
					str += boost::lexical_cast<string, int>(pID) + ",";
				}
				StreamLog::Log(logEmbattle, own, embattle.fomationID, str, log_tag_format_embattle);
			}
		}
		if(update)helper_mgr.insertSaveAndUpdate(this);
	}

	void playerEmbattle::autoUpdate()
	{
		update();
	}

	void playerEmbattle::update()
	{
		Json::Value msg;
		package(msg);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::embattle_update_resp, msg);
	}

	playerEmbattle::EmbattleIterator playerEmbattle::getMineEmbattle(const int fID)
	{
		return embattleMap.find(fID);
	}

	void playerEmbattle::package(Json::Value& pack)
	{
		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();
		{
			Json::Value& embattle_json = pack[msgStr][1u][playerEmbattleDataStr];
			embattle_json = Json::arrayValue;
			unsigned i = 0;
			for (playerEmbattleMap::const_iterator it = embattleMap.begin(); it != embattleMap.end(); ++it,++i)
			{
				const playerSingleEmbattle& em = it->second;
				embattle_json[i][playerFormatIDStr] = em.fomationID;
				embattle_json[i][playerEmbattleValueStr] = em.battleValue;
				Json::Value& formation =	embattle_json[i][playerFormationStr];
				formation = Json::arrayValue;
				for (int n = 0; n < 9; ++n)formation.append(em.fomation[n]);
			}			
		}
		pack[msgStr][1u][playerCurrenFormatIDStr] = currentFormatID;
	}

	bool playerEmbattle::get()
	{
		bool ret = false;
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(embattleSaveDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[playerCurrenFormatIDStr]){
				currentFormatID = obj[playerCurrenFormatIDStr].Int();
			}
			embattleMap.clear();
			checkNotEoo(obj[playerEmbattleDataStr]){
				vector<BSONElement> els = obj[playerEmbattleDataStr].Array();
				for (unsigned i = 0; i < els.size(); ++i)
				{
					int fID = els[i][playerFormatIDStr].Int();
					playerSingleEmbattle em(fID);
					vector<BSONElement> sels = els[i][playerFormationStr].Array();
					for (unsigned n = 0; n < sels.size() && n < 9; ++n)
						em.fomation[n] = sels[n].Int();
					updateSingleBattleValue(em, false);
					embattleMap[em.fomationID] = em;
				}
			}
			ret = true;
		}
		embattle_sys.checkNullEMbattle(own.getOwnDataPtr());
		return ret;
	}

	bool playerEmbattle::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID;
		{
			mongo::BSONArrayBuilder array_embattle;
			for (playerEmbattleMap::iterator it = embattleMap.begin(); it != embattleMap.end(); ++it)
			{
				mongo::BSONObjBuilder b;
				b << playerFormatIDStr <<  it->second.fomationID;
				mongo::BSONArrayBuilder ba;
				for (int n = 0; n < 9; ++n)ba << it->second.fomation[n];
				b << playerFormationStr << ba.arr(); 
				array_embattle.append(b.obj());
			}
			obj << playerEmbattleDataStr << array_embattle.arr();
		}
		obj << playerCurrenFormatIDStr << currentFormatID;
		return db_mgr.save_mongo(embattleSaveDBStr, key, obj.obj());
	}	
}
