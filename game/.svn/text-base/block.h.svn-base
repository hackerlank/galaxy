#pragma once
#include "mongoDB.h"
#include "gate_game_protocol.h"
#include "json/json.h"
#include "msg_base.h"
#include <queue>
#include "time_helper.h"

using namespace std;
using namespace na::msg;

namespace gg
{
#define BlockObj2P(BlockObj) (Block*)(&BlockObj)
#define BlockP2P(BlockPoint) (Block*)(BlockPoint)

#define  OutParameter

#define checkNotEoo(bs)\
	if(! bs.eoo())

namespace GGCOMMON	
{
		static int costCDClear(unsigned time, unsigned div = 600){
			int cost = 0;
			unsigned now= na::time_helper::get_current_time();
			if(now >= time)return cost;
			cost = (time - now) / div;
			cost = (time - now) % div > 0 ? cost + 1 : cost;
			return cost;
		}
}

	class State
	{
	public:
		inline static void setState(const short type)
		{
			state = type;
		}
		inline static int getState()
		{
			return state;
		}
	private:
		static short state;
	};

	class NumberCounter
	{
	public:
		inline static int getCounter()
		{
			return counter;
		}
		inline static void Step()
		{
			counter  = ++counter % 1000000;
		}
	private:
		static int counter;
	};

	const static string playerIDStr = "pi";
	const static string updateFromStr = "uf";
	const static string msgStr = "msg";

	class playerData;
	typedef boost::shared_ptr<playerData> playerDataPtr;

	class Block{
	public:		
		Block(playerData& self) : own(self){}
		~Block(){ }
//		bool begin_save();
		virtual void autoUpdate() {};
		virtual bool save() { return false; };
		virtual bool get() { return false; };
//		virtual void addPackage(const short protocol, Json::Value& msg);
		virtual void doEnd(){}
	protected:		
		playerData& own;
//		virtual void sendAllMsgJson();
	private:
//		queue<msg_json::ptr> msgQueue;
	};
}

