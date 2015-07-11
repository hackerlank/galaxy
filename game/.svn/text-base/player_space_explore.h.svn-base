#pragma once
#include "block.h"
#include "space_explore_system.h"

namespace gg
{
	class playerSpaceExplore : public Block
	{
	public:
		playerSpaceExplore(playerData& own);
		void autoUpdate();
		bool get();
		bool save();
		void package(Json::Value& pack);
		int exploreOne(struct rewardResult& rr, Json::Value &failSimpleJson);
		int exploreTen(vector<rewardResult>& rlist, Json::Value &failSimpleJson);
	private:
		void exploreRandom(rewardResult& rr);
		void refreshData();
		void maintainDailyData();
		int todayOneHuntNotFreeUsedTimes_;
		int totalTenNotFreeHuntsUsedTimes_;
		int nextFreeOneHuntTime_;
		int nextFreeTenHuntsTime_;
		int nextDailyFreshTime_;
	};
}

