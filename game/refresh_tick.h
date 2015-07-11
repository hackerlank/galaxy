#pragma once

#include "block.h"
#include <map>
using namespace std;

namespace gg
{
	namespace TICK
	{
		const static string TickMongoDBStr = "gl.player_tick";

		const static unsigned DayHours_5 = 18000;

		const static string strTickKey = "key";
		const static string strTickValue = "val";
	}

	class TickTime : public Block
	{
	public:
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate(){}

		const unsigned OSS;
		TickTime(playerData& own, const unsigned oss);
		~TickTime(){}
		bool Tick();
		inline unsigned getCNext(){return next;}
	private:
		unsigned next;
		bool isInit;
	};

	class TTManager : public Block
	{
	public:
		virtual bool get();
		virtual bool save(){return true;}
		virtual void autoUpdate(){}

		TTManager(playerData& own);
		~TTManager(){};
		void Tick();
	private:
		void TickDH_5(unsigned last, unsigned now);
		typedef map<unsigned, TickTime> TTMap;
		TTMap ownMap;
	};
}