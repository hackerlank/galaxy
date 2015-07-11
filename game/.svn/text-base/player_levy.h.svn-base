#pragma once

#include "block.h"
#include "playerCD.h"
#include "params.hpp"

namespace gg
{
	const static string playerLevyDBStr = "gl.player_levy";
	
	class TTManager;
	class playerLevy : public Block
	{
		friend class TTManager;
	public:
		playerLevy(playerData& own);
		virtual void update();
		virtual void autoUpdate();
		virtual bool get();
		virtual bool save();
		int levy(const bool useGold = false);
		int clearCD(const bool costGold = true);
		playerCD getLevyCD(){return levyCD;}
		int getNormalLevyTimes(){ return normalLevyTimes; }
		int getForceLevyTimes(){ return forceLevyTimes; }

		void addLevyTimes(bool bForce);
	private:	
		int levyGold;
		int levyToday;
		int levyTimes;
		int normalLevyTimes;
		int forceLevyTimes;
		playerCD levyCD;
	};

}