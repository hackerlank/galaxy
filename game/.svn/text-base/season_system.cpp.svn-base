#include "season_system.h"
#include "time_helper.h"
#include "response_def.h"
#include <cstdlib> 
#include "level_rank.h"

namespace gg
{
	const static string seasonDBName = "gl.season";
	const  static unsigned refreshTime = 18000;
	const static string seasonKey = "key";
	const static string seasonName = "season";
	const static string seasonOpenTimeStr = "open";

	season* const season::seasonSys  = new season();

	void season::initData()
	{		
		db_mgr.ensure_index(seasonDBName, BSON(seasonKey << 1));
		unsigned now = na::time_helper::get_current_time();
		tm tmNow = na::time_helper::toTm(now);
		serverOpenTime  = na::time_helper::get_time_zero(now)  + 5 * 3600;
		if(tmNow.tm_hour < 5)serverOpenTime -= na::time_helper::ONEDAY;
		mongo::BSONObj key = BSON(seasonKey << seasonName);
		mongo::BSONObj obj = db_mgr.FindOne(seasonDBName, key);
		if(obj.isEmpty()){			
			mongo::BSONObj save = BSON(seasonKey << seasonName << seasonOpenTimeStr << serverOpenTime);
			db_mgr.save_mongo(seasonDBName, key, save);
			return;
		}
		if(!obj[seasonOpenTimeStr].eoo())serverOpenTime = (unsigned)obj[seasonOpenTimeStr].Int();
	}

	void season::getServerOpenTime(msg_json& m, Json::Value& r){
		r = serverOpenPackage();
	}

	int season::getServerOpenDay()
	{
		unsigned now = na::time_helper::get_current_time();
		if (now < serverOpenTime)return 0;
		unsigned passTime = now - serverOpenTime;
		int days = passTime / na::time_helper::ONEDAY;
		days = passTime % na::time_helper::ONEDAY > 0 ? days + 1 : days;
		return days;
	}

	double season::serverModule()
	{
		int days = getServerOpenDay();
		if (days <= 1)return 0.0;
		double module = 0.5 + (days - 2) * 0.01;
		module = module > 2.0 ? 2.0 : module;
		return module;
	}

	unsigned season::getNextSeasonTime()
	{
		unsigned now = na::time_helper::get_current_time();
		int now_season = getSeason(now);
		unsigned forward_time = na::time_helper::get_time_zero(now_season) + 5 * 3600;
		int forward_season = getSeason(forward_time);
		if (now_season == forward_season)forward_time += na::time_helper::ONEDAY;
		return forward_time;
	}

	Json::Value season::serverOpenPackage()
	{
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1u] = this->serverOpenTime;
		updateJson[msgStr][2u] = na::time_helper::timeZone();
		updateJson[msgStr][3u] = na::time_helper::get_current_time();
		updateJson[msgStr][4u] = this->getSeason();
		return updateJson;
	}

	int season::getSeason()
	{
		unsigned now = na::time_helper::get_current_time();
		return getSeason(now);
	}

	int season::getSeason(unsigned stamp)
	{
		return (stamp - serverOpenTime) / 86400 % 4;
	}

	unsigned season::getFutureSeason(int quarter, int hours /* = 0 */, int minute /* = 0 */, int sec /* = 0 */)
	{
		hours = hours > 23 ? 23 : hours;
		minute = minute > 59 ? 59 : minute;
		sec = sec > 59 ? 59 : sec;
		hours = hours < 0 ? 0 : hours;
		minute = minute < 0 ? 0 : minute;
		sec = sec < 0 ? 0 : sec;
		if (quarter < season_quarter_one)quarter = season_quarter_one;
		if (quarter > season_quarter_four)quarter = season_quarter_four;
		const unsigned now = na::time_helper::get_current_time();
		const tm now_tm = na::time_helper::toTm(now);
		int value = 0;
		for (int start = getSeason(now); start != quarter; ++start)
		{
			++value;
			start = start > season_quarter_four ? season_quarter_one : start;
		}
		unsigned resTime = now - now_tm.tm_hour * 3600 - now_tm.tm_min * 60 - now_tm.tm_sec + value * na::time_helper::DAY;//设置为指定季节的0点,非标准季节时间
		resTime += (hours * 3600 + minute * 60 + sec);//设置为指定时间
		resTime = hours < 5 ? resTime + na::time_helper::DAY : resTime;//少于5点 , 日期+1
		resTime = now > resTime ? resTime + 4 * na::time_helper::DAY : resTime;//小于当前时间, 日期 + 4
		return resTime;
	}

	unsigned season::getNextSeason(int quarter)
	{
		if(quarter < season_quarter_one)quarter = season_quarter_one;
		if(quarter > season_quarter_four)quarter = season_quarter_four;
		unsigned now = na::time_helper::get_current_time();
		int ss = getSeason(now);
		int value = abs(ss - quarter);
		if(value == 0) value = 4;
		tm tt = na::time_helper::toTm(now);
		unsigned resTime = now - tt.tm_hour * 3600 - tt.tm_min * 60 - tt.tm_sec + 5 * 3600 + value * 24 * 3600;
		return resTime;
	}

	unsigned season::getLastSeason(int quarter)
	{
		if(quarter < season_quarter_one)quarter = season_quarter_one;
		if(quarter > season_quarter_four)quarter = season_quarter_four;
		unsigned now = na::time_helper::get_current_time();
		int ss = getSeason(now);
		int value = abs(ss - quarter);
		if(value == 0) value = 4;
		tm tt = na::time_helper::toTm(now);
		unsigned resTime = now - tt.tm_hour * 3600 - tt.tm_min * 60 - tt.tm_sec + 5 * 3600 - value * 24 * 3600;
		return resTime;
	}

	unsigned season::getNextSeasonTime(unsigned now, int quarter, int hour, int minute /* = 0 */, int sec /* = 0 */)
	{
		unsigned next = na::time_helper::get_next_update_time(now, hour, minute, sec);
		int next_quarter = getSeason(next);
		if (next_quarter == quarter)
			return next;
		else if (next_quarter > quarter)
			return next + (quarter + 4 - next_quarter) * 86400;
		else
			return next + (quarter - next_quarter) * 86400;
	}

	unsigned season::getNextSeasonTime(int quarter, int hour, int minute /* = 0 */, int sec /* = 0 */)
	{
		unsigned now = na::time_helper::get_current_time();
		return getNextSeasonTime(now, quarter, hour, minute, sec);
	}

	unsigned season::getYear(unsigned stamp)
	{
		return (stamp - serverOpenTime) / 86400 / 4 + 145;
	}

	unsigned season::getYearStamp(unsigned now)
	{
		return (now - serverOpenTime) / 86400 / 4 * 86400 * 4 + serverOpenTime;
	}

}