#include "embattle_sys.h"
#include "playerManager.h"
#include "file_system.h"
#include "response_def.h"
#include "helper.h"
#include "war_story.h"


namespace gg
{

	bool embattleData::canUseHole(const int hole, playerDataPtr player)const
	{
		for (unsigned i = 0; i < holeConfig.size(); ++i)
			if(hole == holeConfig[i].holeIndex && player->Base.getLevel() >= holeConfig[i].levelLimit 
				&& player->Warstory.currentProcess >= holeConfig[i].holeOpen){
				return true;
			}
		return false;
	}

	embattle_system* const embattle_system::embattleSys = new embattle_system();

	void embattle_system::embattleUpdate(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Embattle.update();
	}


	void embattle_system::setCurrentFormat(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int formatID = js_msg[0u].asInt();
		const embattleData& em = getEMbattle(formatID);
		if(em == NullEmbattleData)Return(r, -1);
		if(d->Base.getLevel() < em.levelLimit) Return(r, 1);
		d->Embattle.setCurrentFormat(formatID);
		helper_mgr.insertSaveAndUpdate(&d->Embattle);
		Return(r, 0);
	}

	void embattle_system::embattleInFormat(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
 		int formatID = js_msg[0u].asInt();
		int fm[9];
		if(!js_msg[1u].isArray())Return(r, -1);
		for (int i = 0; i < 9; ++i)
		{
			if(!js_msg[1u][(unsigned)i].isInt())Return(r, -1);
			fm[i] = js_msg[1u][(unsigned)i].asInt();
		}
		int res = d->Embattle.setFormation(formatID, fm);
		Return(r, res);
	}

	const embattleData& embattle_system::getEMbattle(const int rawID)
	{
		embattleMap::iterator it = map_.find(rawID);
		if(it == map_.end())
			return NullEmbattleData;

		return it->second;
	}

	void embattle_system::activeEMbattle(playerDataPtr player)
	{
		int level = player->Base.getLevel();
		for (embattleMap::const_iterator it = map_.begin(); it != map_.end();++it)
		{
			if(level < it->second.levelLimit)continue;
			player->Embattle.activeEmbattle(it->second);
		}
	}

	void embattle_system::checkNullEMbattle(playerDataPtr player)
	{
		int level = player->Base.getLevel();
		for (embattleMap::const_iterator it = map_.begin(); it != map_.end();++it)
		{
			if(level < it->second.levelLimit)continue;
			player->Embattle.activeEMbattleWithOutSave(it->second);
		}
	}

	const static string embattleDataDirStr = "./instance/embattle/";
	const static string embattleRawIDStr = "rawID";
	const static string embattleLevelLimitStr = "levelLimit";
	const static string embattleHolesConfigStr = "holesConfig";
	const static string embattleHoleIndexStr = "holeIndex";
	const static string embattleHoleOpenStr = "holeOpen";
	const static string embattleAddAttriStr = "addAttribute";
	const static string embattleAddIndexStr = "addIndex";
	const static string embattleAddBasicValueStr = "basicValue";

	void embattle_system::initData()
	{
		db_mgr.ensure_index(embattleSaveDBStr, BSON(playerIDStr << 1));
		map_.clear();
		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(embattleDataDirStr, jv);

		for (unsigned i = 0; i < jv.size(); ++i)
		{
			Json::Value& json = jv[i];
			embattleData ebD;
			ebD.emID = json[embattleRawIDStr].asInt();
			ebD.levelLimit = json[embattleLevelLimitStr].asInt();

			for (unsigned n = 0; n < json[embattleHolesConfigStr].size(); ++n)
			{
				Json::Value& config = json[embattleHolesConfigStr][n];
				embattleHole hole;
				hole.holeIndex = config[embattleHoleIndexStr].asInt();
				hole.holeOpen = config[embattleHoleOpenStr].asInt();
				hole.levelLimit = config[embattleLevelLimitStr].asInt();
				ebD.holeConfig.push_back(hole);
			}

			for (unsigned n = 0; n < json[embattleAddAttriStr].size(); ++n)
			{
				Json::Value& config = json[embattleAddAttriStr][n];
				embattleAddAttri added;
				added.idxWhat = config[embattleAddIndexStr].asInt();
				added.addBase = config[embattleAddBasicValueStr].asDouble();
				ebD.embattleAdd.push_back(added);
			}

			map_[ebD.emID] = ebD;
		}
	}

}