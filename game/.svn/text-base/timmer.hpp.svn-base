#pragma once

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "core_helper.h"
#include "time_helper.h"
#include "season_system.h"
#include <exception>

using namespace std;
using namespace gg;

const static unsigned TimerErrorID = 0xFFFFFFFF;

#define boostFunc boost::bind
#define boostBind(Func, ...) boost::bind(&Func, __VA_ARGS__)

class Timer
{
public:
	typedef boost::function<void()> Handler;
private:
	class HandlerManager
	{
		friend class Timer;
	public:
		HandlerManager()
		{
			HandlerMap.clear();
		}
		void TickEvent(const unsigned ID)
		{
			try
			{
				Handler handler = PopHandler(ID);
				handler();
			}
			catch (string& error)
			{
				LogW << "timer callback failed ... may remove this event..." << error << LogEnd;
			}
			catch (std::exception& e)
			{
				LogW << "timer callback failed ..." << e.what() << LogEnd;
			}
		}
		void RemoveEvent(const unsigned ID)
		{
			boostFunctionMap::iterator it = HandlerMap.find(ID);
			if (it == HandlerMap.end())return;
			HandlerMap.erase(it);
			RcPool.insert(ID);
		}
		unsigned AddEvent(Handler handler)
		{
			unsigned ID = PopID();
			do
			{
				if (ID == TimerErrorID)break;
				HandlerMap[ID] = handler;
			} while (false);
			return ID;
		}
	private:
		Handler PopHandler(const unsigned ID)
		{
			boostFunctionMap::iterator it = HandlerMap.find(ID);
			if (it == HandlerMap.end())
			{
				RecoveryPool::iterator rc = RcPool.find(ID);
				if (rc != RcPool.end())
				{
					RcPool.erase(rc);
					_IDCreator.push_back(ID);
					throw string("event has been remove before call ...");
				}
				throw string("can not find event");
			}
			Handler handler = it->second;
			HandlerMap.erase(it);
			_IDCreator.push_back(ID);
			return handler;
		}
		unsigned PopID()
		{
			if (_IDCreator.empty())return _IDBegin++;
			unsigned ID = _IDCreator.back();
			_IDCreator.pop_back();
			return ID;
		}
	private:
		typedef boost::unordered_map<unsigned, Handler> boostFunctionMap;
		boostFunctionMap HandlerMap;
		typedef boost::unordered_set<unsigned> RecoveryPool;
		RecoveryPool RcPool;
		std::vector<unsigned> _IDCreator;
		unsigned _IDBegin;
	};
private:
	struct tickData
	{
		tickData(const unsigned id, unsigned dl)
		{
			ID = id;
			deadline = dl;// +na::time_helper::get_current_time();
		}
		unsigned ID;
		unsigned deadline;
	};
	typedef std::vector<tickData> CallVec;
	static CallVec _ownVec;
	static bool _isSort;
	static void AddTickImpl(tickData tD)
	{
		_ownVec.push_back(tD);
		_isSort = false;
	}
	static bool Aec(const tickData& t1, const tickData& t2)
	{
		return t1.deadline < t2.deadline;
	}
private:
	static HandlerManager HandlerMgr;
	static bool _InternalAddEvent(Handler handler, const unsigned tickTime)
	{
		const unsigned ID = HandlerMgr.AddEvent(handler);
		if (ID == TimerErrorID)return false;
		tickData tick(ID, tickTime);
		_Timer_Post(boost::bind(&Timer::AddTickImpl, tick));
		return true;
	}
	static int _InternalAddEventID(Handler handler, const unsigned tickTime)
	{
		const unsigned ID = HandlerMgr.AddEvent(handler);
		if (ID == TimerErrorID)return TimerErrorID;
		tickData tick(ID, tickTime);
		_Timer_Post(boost::bind(&Timer::AddTickImpl, tick));
		return ID;
	}
	class CycleTimehelper
	{
		public:
			CycleTimehelper(Handler h, int hour, int min, int sec)
				: handler(h), seasons(-1), hours(hour), mins(min), secs(sec){}
			CycleTimehelper(Handler h, int season, int hour, int min, int sec)
				: handler(h), seasons(season), hours(hour), mins(min), secs(sec){}

			void run()
			{
				handler();
				if (seasons == -1)
					Timer::AddEventDayCycleTime(handler, hours, mins, secs);
				else
					Timer::AddEventSeasonCycleTime(handler, seasons, hours, mins, secs);
			}

		private:
			Handler handler;
			int seasons;
			int hours;
			int mins;
			int secs;
	};

	typedef boost::shared_ptr<CycleTimehelper> CycleTimePtr;

public:
	static bool AddEventSeconds(Handler handler, const unsigned seconds)
	{
		unsigned tickTime = seconds + na::time_helper::get_current_time();
		return _InternalAddEvent(handler, tickTime);
	}
	static int AddEventSecondsID(Handler handler, const unsigned seconds)
	{
		unsigned tickTime = seconds + na::time_helper::get_current_time();
		return _InternalAddEventID(handler, tickTime);
	}
	static bool AddEventTickTime(Handler handler, const unsigned tickTime)
	{
		return _InternalAddEvent(handler, tickTime);
	}
	static int AddEventTickTimeID(Handler handler, const unsigned tickTime)
	{
		return _InternalAddEventID(handler, tickTime);
	}
	//hours 0~23
	//minute 0~59
	//sec 0~59
	// 季节刷新是每日5点, 所以如果时间指定是0 ~ 5点之间, 实际日期会 + 1日
	//只会指定未来的时间,不会对已经过去的时间做处理
	//广播比较适用, 具体的逻辑事件比较适合自己管理时间点并且使用AddEventTickTime
	static bool AddEventFutureSeason(Handler handler, const int season, const int hours = 0, const int minute = 0, const int secs = 0)
	{
		const unsigned tickTime = season_sys.getFutureSeason(season, hours, minute, secs);
		return _InternalAddEvent(handler, tickTime);
	}
	static int AddEventFutureSeasonID(Handler handler, const int season, const int hours = 0, const int minute = 0, const int secs = 0)
	{
		const unsigned tickTime = season_sys.getFutureSeason(season, hours, minute, secs);
		return _InternalAddEventID(handler, tickTime);
	}
	//无规则
	static bool AddEventFutureTime(Handler handler, const int hours = 0, const int minute = 0, const int secs = 0)
	{
		unsigned now = na::time_helper::get_current_time();
		unsigned tickTime = na::time_helper::get_time_zero(now) + hours * 3600 + minute * 60 + secs;
		tickTime = now > tickTime ? tickTime + na::time_helper::DAY : tickTime;
		return _InternalAddEvent(handler, tickTime);
	}
	static int AddEventFutureTimeID(Handler handler, const int hours = 0, const int minute = 0, const int secs = 0)
	{
		unsigned now = na::time_helper::get_current_time();
		unsigned tickTime = na::time_helper::get_time_zero(now) + hours * 3600 + minute * 60 + secs;
		tickTime = now > tickTime ? tickTime + na::time_helper::DAY : tickTime;
		return _InternalAddEventID(handler, tickTime);
	}
	// 每天某个时间点触发
	static bool AddEventDayCycleTime(Handler handler, const int hours = 0, const int minute = 0, const int secs = 0)
	{
		unsigned now = na::time_helper::get_current_time();
		unsigned tickTime = na::time_helper::get_next_update_time(now, hours, minute, secs);
		CycleTimePtr ptr(creator<CycleTimehelper>::run(handler, hours, minute, secs));
		return _InternalAddEvent(boostBind(CycleTimehelper::run, ptr), tickTime);
	}
	// 每年某个季节时间点触发
	static bool AddEventSeasonCycleTime(Handler handler, const int season, const int hours = 0, const int minute = 0, const int secs = 0)
	{
		unsigned now = na::time_helper::get_current_time();
		unsigned tickTime = season_sys.getNextSeasonTime(now, season, hours, minute, secs);
		CycleTimePtr ptr(creator<CycleTimehelper>::run(handler, season, hours, minute, secs));
		return _InternalAddEvent(boostBind(CycleTimehelper::run, ptr), tickTime);
	}
	static void RemoveEvent(const unsigned ID)
	{
		HandlerMgr.RemoveEvent(ID);
	}
	static void EventUpdate()
	{
		static boost::system_time tick = boost::get_system_time();
		boost::system_time now_sys = boost::get_system_time();
		do 
		{
			if ((now_sys - tick).total_milliseconds() < 1000)break;
			tick = now_sys;
			if (!_isSort)
			{
				std::sort(_ownVec.begin(), _ownVec.end(), Timer::Aec);
				_isSort = true;
			}

			unsigned now = na::time_helper::get_current_time();
			unsigned remove = 0;
			for (unsigned i = 0; i < _ownVec.size(); ++i)
			{
				tickData& tD = _ownVec[i];
				if (now > tD.deadline)
				{
					_Logic_Post(boost::bind(&HandlerManager::TickEvent, &HandlerMgr, tD.ID));
					++remove;
					continue;
				}
				break;
			}
			_ownVec.erase(_ownVec.begin(), _ownVec.begin() + remove);
		} while (false);

		na::time_helper::sleep(100);
		_Timer_Post(boost::bind(Timer::EventUpdate));
	}
};