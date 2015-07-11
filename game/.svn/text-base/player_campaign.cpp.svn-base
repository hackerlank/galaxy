#include "player_campaign.h"
#include "helper.h"
#include "time_helper.h"
#include "season_system.h"
#include "record_system.h"
#include "junling_system.h"
#include "guild_system.h"

namespace gg
{
	const static string playerJunlingStr = "jl";
	const static string playerCampaignLastUpdateTimeStr = "lut";
	const static string playerCampaignCDStr = "cd";
	const static string playerCampaignCDLockStr = "cl";
	const static string playerTodayBuyStr = "tb";
	const static string playerLastEnemyStr = "le";
	const static string playerCDHelpTickStr = "cht";

	const static unsigned CampaignLockTime = 600;
	playerCampaign::playerCampaign(playerData& own) : Block(own), CD(-1, CampaignLockTime)
	{
		helpTick = false;
		junling = 30;
		buyToday = 0;
		lastUpdateTime = na::time_helper::get_current_time();
	}
	enum{
		alter_junling,
		alter_buy_today,
		alter_refresh,
	};
	const static string playerJunlingLog = "log_player_junling";
	void playerCampaign::alterJunling(const int num)
	{
		refresh();
		int tmp = junling;
		junling += num;
		if(junling > 99999)junling = 99999;
		if(junling < 0) junling = 0;
		StreamLog::Log(playerJunlingLog, own, tmp, junling, alter_junling);
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerCampaign::setHelpTick(const bool ret)
	{
		helpTick = ret;
		helper_mgr.insertSaveAndUpdate(this);
	}

	playerCD& playerCampaign::getCD()
	{
		CDData& CDD = CD.getCD();
		if (!CDD.Lock)
		{
			helpTick = false;
			guild_sys.removeHelp(own.getOwnDataPtr(), Guild::GuildHelp::war_story_cd);
			helper_mgr.insertSaveAndUpdate(this);
		}
		return CD;
	}

	void playerCampaign::alterBuyToday(const int num)
	{
		int tmp = buyToday;
		buyToday += num;
		if(buyToday > 99999)buyToday = 99999;
		if(buyToday < 0) buyToday = 0;
		StreamLog::Log(playerJunlingLog, own, tmp, buyToday, alter_buy_today);
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerCampaign::getJunling()
	{
		refresh();
		return junling;
	}

	int playerCampaign::getBuyToday()
	{
		refresh();
		return buyToday;
	}

	void playerCampaign::refresh()
	{
		own.TickMgr.Tick();
		unsigned now = na::time_helper::get_current_time();
		int num = 0; 

		if(now > lastUpdateTime){
			int temp = (now - lastUpdateTime) / 1800;
			lastUpdateTime = now - (now - lastUpdateTime) % 1800;
			num += temp;
		}
		if(junling >= 50 || num < 1)return;
		int tmp = junling;
		junling += num;
		junling = junling > 50 ? 50 : junling;
		StreamLog::Log(playerJunlingLog, own, tmp, junling, alter_refresh);
	}

	bool playerCampaign::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = BSON(playerIDStr << own.playerID << playerJunlingStr << junling
			<< playerCampaignLastUpdateTimeStr << lastUpdateTime << playerCampaignCDStr 
			<< CD.getCD().CD << playerCampaignCDLockStr << 	CD.getCD().Lock <<
			playerTodayBuyStr << buyToday << playerCDHelpTickStr << helpTick);
		return db_mgr.save_mongo(playerCampaignDBStr, key, obj);
	}

	bool playerCampaign::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(playerCampaignDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[playerJunlingStr]){	junling = obj[playerJunlingStr].Int();}
			checkNotEoo(obj[playerCampaignLastUpdateTimeStr]){lastUpdateTime = (unsigned)obj[playerCampaignLastUpdateTimeStr].Int();}
			checkNotEoo(obj[playerCampaignCDStr]){CD.getCD().CD = (unsigned)obj[playerCampaignCDStr].Int();}
			checkNotEoo(obj[playerCampaignCDLockStr]){CD.getCD().Lock = obj[playerCampaignCDLockStr].Bool();}
			checkNotEoo(obj[playerTodayBuyStr]){buyToday = obj[playerTodayBuyStr].Int();}
			checkNotEoo(obj[playerCDHelpTickStr]){ helpTick = obj[playerCDHelpTickStr].Bool(); }
			return true;
		}				
		return false;
	}

	void playerCampaign::autoUpdate()
	{
		update();
	}

	void playerCampaign::update()
	{
		refresh();
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		msg[msgStr][1u][playerJunlingStr] = junling;
		CDData CDD = getCD().getCD();
		msg[msgStr][1u][playerCampaignCDStr] = CDD.CD;
		msg[msgStr][1u][playerCampaignCDLockStr] = CDD.Lock;
		msg[msgStr][1u][playerCampaignLastUpdateTimeStr] = lastUpdateTime;
		msg[msgStr][1u][playerTodayBuyStr] = jl_sys.getCost(own.getOwnDataPtr());
		msg[msgStr][1u][playerCDHelpTickStr] = helpTick;
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::junling_update_resp, msg);
	}
}