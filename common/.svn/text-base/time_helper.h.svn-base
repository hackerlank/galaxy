#pragma once
#include <time.h>
#include <string>
#include <boost/thread/thread_time.hpp>
#include "Glog.h"
#include <cstdio>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include "config.h"

using namespace std;

namespace na
{
	namespace time_helper
	{
		const static unsigned ONEHOUR = 3600;
		const static unsigned ONEDAY = 86400; 
		const static time_t MAX_TIME_EXPRESS = 4000000000;
		static const std::string time_zone_str = "time_zone";

		enum{
			DAY = 24 * 60 * 60,
			HOUR = 60 * 60,
			MINUTE = 60,
			SECOND = 1,
		};

		unsigned getDeviation();
		void setDeviation(const unsigned val);

		inline int timeZone(void)
		{
			static int time_zone = config_ins.get_config_prame(time_zone_str).asInt();
			return time_zone;
		}

		inline boost::system_time get_sys_time()
		{
			return (boost::get_system_time() + boost::posix_time::seconds(getDeviation()));
		}

		inline string fomat_time_t2str(time_t time,bool print = false)
		{
			boost::posix_time::ptime t = boost::posix_time::from_time_t(time);
			string str;
			str = to_simple_string(t);
			if(print) 
				LogI << str << LogEnd;
			return str;
		}

		//microsecond 
		inline void	sleep(int micro_secs)
		{
			boost::system_time time = boost::get_system_time();
			time += boost::posix_time::microsec(micro_secs);
			boost::thread::sleep(time);
		}

		inline unsigned get_current_time()
		{
			unsigned stamp = (unsigned)std::time(NULL);
			int zone = timeZone();
			time_t res_t = stamp;
			if (zone < 0) res_t = (stamp - (unsigned)abs(zone) * 3600 + getDeviation());
			else res_t = (stamp + (unsigned)zone * 3600 + getDeviation());
			if (res_t >= MAX_TIME_EXPRESS)
			{
				LogE << "can no express this time" << LogEnd;
				sleep(3000000);
				abort();
			}
			return (unsigned)res_t;
		}

		class Deviation
		{
			friend unsigned getDeviation();
			friend void setDeviation(const unsigned val);
			static unsigned _Dev;
		};

		inline unsigned get_time_zero(unsigned time)
		{
			boost::posix_time::ptime pt = boost::posix_time::from_time_t(time);
			tm t = boost::posix_time::to_tm(pt);
			time -= (t.tm_hour * 3600 + t.tm_min * 60 + t.tm_sec);
			return time;
		}

		inline time_t get_next_time(unsigned time, unsigned day = 0, unsigned h = 0, 
												unsigned m = 0, unsigned s = 0)
		{
			return (time + day * 24 * 3600 + h * 3600 + m * 60 + s);
		}

		inline tm toTm(time_t t)
		{			
			boost::posix_time::ptime pt = boost::posix_time::from_time_t(t); 
			return boost::posix_time::to_tm(pt);
		}

		inline unsigned fromTm(struct tm* pTm)
		{
			time_t stamp = mktime(pTm);
			int zone = timeZone();
			if(zone < 0) return (unsigned)(stamp - (unsigned)abs(zone) * 3600);
			return (unsigned)(stamp + (unsigned)zone * 3600);
		}

		inline void	showlocaltime()
		{
			time_t time = std::time(NULL) + timeZone() * 3600;
			tm t = toTm(time); 
			printf("%02d-%02d-%02d-%02d-%02d-%02d\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
		}

		inline bool	get_duration_time(time_t time_start, time_t time_end, int& hour, int& min, int& sec)
		{
			boost::posix_time::ptime ts = boost::posix_time::from_time_t(time_start);
			boost::posix_time::ptime te = boost::posix_time::from_time_t(time_end);
			boost::posix_time::time_duration td = te - ts;
			hour = td.hours();
			min = td.minutes() + hour*60;
			sec = td.seconds() + min*60;
			return true;
		}

		inline unsigned get_next_update_time(unsigned now, int hour = 5, int min = 0, int sec = 0)
		{
			tm tm_time = toTm((time_t)now);
			unsigned secs = (unsigned)(tm_time.tm_hour * HOUR + tm_time.tm_min * MINUTE + tm_time.tm_sec);
			unsigned next = now - secs + hour * HOUR + min * MINUTE + sec;
			if(next <= now)
				next += DAY;
			return next;
		}
	}
}


class time_logger
{
	boost::system_time start_time;
	boost::system_time end_time;
	std::string str;
public:
	time_logger(const char* s)
	{
		start_time = boost::get_system_time();
		str = s;
	}
	~time_logger()
	{
		end_time = boost::get_system_time();
		float delta = (float)((end_time - start_time).total_nanoseconds()/1000000);
		if(delta > 50)
		{
			LogD << str << "\t" << delta << " ms" << LogEnd;
		}
	}
};


// example 
//#pragma once
//#include <time_helper.h>
//#include <iostream>
//void main()
//{
//	time_t t = na::time_helper::get_current_time();
//	time_t t1 = na::time_helper::calc_time(t,24,20,10);
//	na::time_helper::print_time_string(t,true);
//	na::time_helper::print_time_string(t1,true);
//	int hour,min,sec;
//	na::time_helper::get_duration_time(t,t1,hour,min,sec);
//	std::cout << "hours:\t" <<hour << "\tmins:\t" << min << "\tsecs:\t" << sec << std::endl;
//}


// output:
//		2011-Dec-13 05:08:43
//		2011-Dec-14 05:28:53
//		hours:  24      mins:   1460    secs:   87610
//		Çë°´ÈÎÒâ¼ü¼ÌÐø. . .
