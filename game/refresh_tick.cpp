#include "refresh_tick.h"
#include "mongoDB.h"
#include "player_data.h"
#include "time_helper.h"
#include "helper.h"
#include "season_system.h"
#include "record_system.h"
#include "guild_system.h"
#include "email_system.h"

namespace gg
{
	using namespace TICK;
	const static string refreshLog = "log_refresh_time";
	TickTime::TickTime(playerData& own, const unsigned oss) : Block(own), OSS(oss)
	{
		isInit = true;
// 		unsigned now = na::time_helper::get_current_time();
// 		tm tmNow = na::time_helper::toTm(now);
// 		next  = na::time_helper::get_time_zero(now)  + OSS;
// 		if(tmNow.tm_hour < 5)next -= na::time_helper::ONEDAY;
		next = season_sys.getServerOpenTime() + na::time_helper::ONEDAY;
	}

	bool TickTime::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID << strTickKey <<
			OSS);
 		mongo::BSONObj obj = db_mgr.FindOne(TICK::TickMongoDBStr, key);
 		if(!obj.isEmpty())
		{
 			checkNotEoo(obj[strTickValue])
			{
				isInit = false;
				next = (unsigned)obj[strTickValue].Int();
			}
 			return true;
 		}				
		return false;
	}

	bool TickTime::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID << strTickKey <<
			OSS);
		mongo::BSONObj obj = BSON(playerIDStr << own.playerID << strTickKey <<
			OSS << strTickValue << next);
		isInit = false;
		StreamLog::Log(refreshLog, own, OSS, next, 2);
		return db_mgr.save_mongo(TICK::TickMongoDBStr, key, obj);
	}

	bool TickTime::Tick()
	{
		unsigned now = na::time_helper::get_current_time();
		if(now > next)
		{
			tm tmNow = na::time_helper::toTm(now);
			next  = na::time_helper::get_time_zero(now)  + OSS;
			if(tmNow.tm_hour >= 5)next += na::time_helper::ONEDAY;
			helper_mgr.insertSaveAndUpdate(this);
			return true;
		}
		if(isInit)
		{
			helper_mgr.insertSaveAndUpdate(this);
		}
		return false;
	}


	//////////////////////////////////////////////////////////////////////////

	TTManager::TTManager(playerData& own) : Block(own)
	{
		ownMap.clear();
		ownMap.insert(TTMap::value_type(TICK::DayHours_5, TickTime(own, TICK::DayHours_5)));
	}

	bool TTManager::get()
	{
		for (TTMap::iterator it = ownMap.begin(); it != ownMap.end(); ++it)
		{
			TickTime& tt = it->second;
			tt.get();
		}
		return true;
	}

	void TTManager::Tick()
	{
		unsigned now = na::time_helper::get_current_time();
		for (TTMap::iterator it = ownMap.begin(); it != ownMap.end(); ++it)
		{
			unsigned lastTime = it->second.getCNext();
			if(it->first == TICK::DayHours_5 && it->second.Tick())
			{
				StreamLog::Log(refreshLog, own, it->second.OSS, it->second.getCNext(), 0);
				TickDH_5(lastTime, now);
			}
		}
	}

	void TTManager::TickDH_5(unsigned last, unsigned now)
	{
		StreamLog::Log(refreshLog, own, last, now, 1);

		//玩家基本信息
		{
			own.Base.invest_ = 3;
			own.Base.shared_ = 10;
			helper_mgr.insertSaveAndUpdate(&own.Base);
		}

		//购买军令次数
		{
			own.Campaign.buyToday = 0;
			helper_mgr.insertSaveAndUpdate(&own.Campaign);
		}

		//生产重置
		{
			own.Workshop.counter = 10;
			own.Workshop.qianzhi_counter = 0;
			helper_mgr.insertSaveAndUpdate(&own.Workshop);
		}

		//星盟可执行次数
		{
			own.Guild.applyJoin = 0;
			own.Guild.declareWords = 0;
			own.Guild.kickMember = 0;
			own.Guild.TeamEmail = 0;
			own.Guild.sDonate = 0;
			own.Guild.cDonate = 0;
			own.Guild.helpTick = 0;
			helper_mgr.insertSaveAndUpdate(&own.Guild);
		}

		//征收相关
		{
			int day = (now - last) / na::time_helper::ONEDAY + 1;
			own.Levy.levyGold = 0;
			own.Levy.levyToday = 0;
			int season = season_sys.getSeason(now);
			for (int i = 0; i < day; ++i)
			{
				if(season == season_quarter_two)
					own.Levy.levyTimes = own.Levy.levyTimes + 14 > 50 ? 50 : own.Levy.levyTimes + 14;
				else
					own.Levy.levyTimes = own.Levy.levyTimes + 12 > 50 ? 50 : own.Levy.levyTimes + 12;
				if(own.Levy.levyTimes == 50)break;
				--season;
				season = season < 0 ? season_quarter_four : season;
			}
			helper_mgr.insertSaveAndUpdate(&own.Levy);
		}


		//贸易
		{
			own.Trade.TodayRewardTimes = 0;
		}

		//星盟
		{
			if (own.Guild.hasJoinGuild() && now > own.Guild.getJoinTime() && own.Guild.getLeaveContribute())
			{
				int day = (now - own.Guild.getJoinTime()) / na::time_helper::ONEDAY + 1;
				if (day > 2)
				{
					int contribute = own.Guild.PopLeaveContribute();
					if (contribute > 0)
					{
						guild_sys.alterContribute(own.getOwnDataPtr(), contribute);
						Json::Value emailJson;
						emailJson["co"] = contribute;
						emailJson["gna"] = own.Guild.getLastGName();
						email_sys.sendSystemEmailCommon(own.getOwnDataPtr(), email_type_system_ordinary, email_team_system_leave_guild, emailJson);
					}
				}
			}
		}


	}

}