#include "player_levy.h"
#include "time_helper.h"
#include "mongoDB.h"
#include "playerManager.h"
#include "season_system.h"
#include "helper.h"
#include "record_system.h"
#include "guild_system.h"
#include "task_system.h"
#include "activity_system.h"
#include "activity_game_param.h"
#include "params.hpp"
namespace gg
{
	const static string playerTodayLevyStr = "tl";
	const static string playerLevyGoldStr = "lg";
	const static string playerLeaveLevyTimesStr = "llt";
	const static string playerLevyTodayTimeStr = "ltt";
	const static string playerLevyCDStr = "cd";
	const static string playerScienceLevelStr = "psl";
	const static string playerCanRewardStr = "cr";
	const static string playerLevyCDLockStr = "cl";
	const static string playerLevyNormalTimesStr = "nt";
	const static string playerLevyForceTimesStr = "ft";

	const static string logPlayerLevy = "log_player_levy";
	enum{
		levy_use_times,
		levy_nouse_times,
		levy_today_times,
	};
	const static unsigned levySetLockTime = 1800;
	playerLevy::playerLevy(playerData& own) : Block(own), levyCD(-1, levySetLockTime), normalLevyTimes(0), forceLevyTimes(0)
	{
		levyToday = 0;
		levyGold = 0;
		levyTimes = 50;
	}

	int levySilver(playerData& own, const bool nb = true)
	{
		int popu = own.Base.getCalPopu() / 10000;
		int plv = own.Base.getLevel();
		int sclv = guild_sys.getScienceData(own.Guild.getGuildID(), playerGuild::levy_way).LV;
		double ratio = activity_sys.getRate(activity::_guo_fu_min_qiang, own.getOwnDataPtr());
		int rwSilver = int((plv * 12 + 8 * sclv) * (1 + 0.02 * plv) + popu * 10.2 + 3000);
		rwSilver = nb ? int(rwSilver * (1 + ratio)) : rwSilver;
		rwSilver = rwSilver < 0 ? 0 : rwSilver;
		return rwSilver;
	}

	int playerLevy::clearCD(const bool costGold /* = true */)
	{
		if(costGold){
			CDData cCD = levyCD.getCD();
			int costGold = GGCOMMON::costCDClear(cCD.CD);
			if(own.Base.getAllGold() < costGold)return 1;
			own.Base.alterBothGold(-costGold);
		}
		levyCD.clearCD();
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}
	
	int playerLevy::levy(const bool useGold /* = false */)
	{
		own.TickMgr.Tick();
		int VIPLV = own.Vip.getVipLevel();
		if(VIPLV  < 3 && !useGold && levyCD.getCD().Lock)return 1;
		if(!useGold && levyTimes <= 0)return 2;
		if(useGold){
			int costGold = 2 * (levyGold + 1);
// 			double rate = own.Buff.getValueDouble(force_levy_cost_gold_rate_buff);
// 			rate = rate < 0.0 ? 0.0 : rate;
// 			rate = rate > 1.0 ? 1.0 : rate;
// 			costGold = (int)( costGold * (1.0 - rate) );
			costGold = costGold < 0 ?  0 : costGold;
			costGold = (int)ceil(costGold*(1+activity_sys.getRate(param_force_levy_cost, own.getOwnDataPtr())));
			if(own.Base.getAllGold() < costGold)return 3;
			own.Base.alterBothGold(-costGold);
			++levyGold;
			addLevyTimes(true);
			StreamLog::Log(logPlayerLevy, own, levyGold - 1, levyGold, levy_nouse_times);
		}else{
			--levyTimes;
			unsigned cd = 240;// 150 + 30 * levyToday;
			++levyToday;
			if(VIPLV  < 3)levyCD.addCD(cd);
			addLevyTimes(false);
			StreamLog::Log(logPlayerLevy, own, levyToday - 1, levyToday, levy_today_times);
			StreamLog::Log(logPlayerLevy, own, levyTimes + 1, levyTimes, levy_use_times);
		}
		own.Base.alterSilver(levySilver(own));
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	void playerLevy::autoUpdate()
	{
		update();
	}

	void playerLevy::update()
	{
		own.TickMgr.Tick();
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		msg[msgStr][1u][playerTodayLevyStr] = levyToday;
		msg[msgStr][1u][playerLevyGoldStr] = levyGold;
		msg[msgStr][1u][playerLeaveLevyTimesStr] = levyTimes;
		msg[msgStr][1u][playerLevyCDStr] = levyCD.getCD().CD;
		msg[msgStr][1u][playerLevyCDLockStr] = levyCD.getCD().Lock;
		int sclv = guild_sys.getScienceData(own.Guild.getGuildID(), playerGuild::levy_way).LV;
		msg[msgStr][1u][playerCanRewardStr] = levySilver(own, false);
		msg[msgStr][1u][playerScienceLevelStr] = sclv;
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::player_levy_update_resp, msg);
	}

	bool playerLevy::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(playerLevyDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[playerLevyGoldStr]){	levyGold = obj[playerLevyGoldStr].Int();}
			checkNotEoo(obj[playerTodayLevyStr]){	levyToday = obj[playerTodayLevyStr].Int();}
			checkNotEoo(obj[playerLeaveLevyTimesStr]){	levyTimes = obj[playerLeaveLevyTimesStr].Int();}
			CDData& CD = levyCD.getCD();
			checkNotEoo(obj[playerLevyCDStr]){CD.CD = (unsigned)obj[playerLevyCDStr].Int();}
			checkNotEoo(obj[playerLevyCDLockStr]){CD.Lock = obj[playerLevyCDLockStr].Bool();}
			checkNotEoo(obj[playerLevyNormalTimesStr]){normalLevyTimes = obj[playerLevyNormalTimesStr].Int();}
			checkNotEoo(obj[playerLevyForceTimesStr]){forceLevyTimes = obj[playerLevyForceTimesStr].Int();}
			return true;
		}				
		return false;
	}

	bool playerLevy::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = BSON(playerIDStr << own.playerID << playerLevyGoldStr << levyGold
			<< playerLeaveLevyTimesStr << levyTimes
			<< playerLevyCDStr<< levyCD.getCD().CD << playerLevyCDLockStr << levyCD.getCD().Lock
			<< playerTodayLevyStr << levyToday << playerLevyNormalTimesStr << normalLevyTimes
			<< playerLevyForceTimesStr << forceLevyTimes);
		return db_mgr.save_mongo(playerLevyDBStr, key, obj);
	}

	void playerLevy::addLevyTimes(bool bForce)
	{
		if(bForce)
		{
			++forceLevyTimes;
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_force_levy_times);
		}
		else
		{
			++normalLevyTimes;
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_normal_levy_times);
		}

		helper_mgr.insertSaveAndUpdate(this);
	}

}