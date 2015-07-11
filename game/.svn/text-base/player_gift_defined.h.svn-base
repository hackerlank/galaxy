#pragma once
#include <string>
#include "activity_gift_defined.h"

using namespace std;

namespace gg
{
	namespace gift_defined_def
	{
		const static string strActDbPlayerGiftDefined = "gl.player_gift_define";
		const static string strPlayerTotalReceiveTimes = "pttrt";
		const static string strPlayerTodayReceiveTimes = "ptrt";
		const static string strNextDailyFreshTime = "ndft";
	}

	struct singlePlayerPack 
	{
		singlePlayerPack();
		bool operator==(singlePlayerPack& singlePack);
		bool maintainDailyData();
		static singlePlayerPack sNull_;
		int id_;
		int startTime_;
		int playerTodayReceiveTimes_;
		int playerTotalReceiveTimes_;
		int nextDailyFreshTime_;
	};

	class playerGiftDefined : public Block
	{
		friend class act_gift_defined;
	public:
		playerGiftDefined(playerData& own);
		void autoUpdate();
		bool get();
		bool save();
		void package(Json::Value& pack);
		bool refreshData(const vector<singleDefinedPack> vecTotalPack);
		singlePlayerPack& getAndUpdateSinglePack(const singleDefinedPack &definedPack);
	private:
		singlePlayerPack& getSinglePack(const singleDefinedPack &definedPack);
		bool removeInvalidPack(const vector<singleDefinedPack> vecTotalPack);
		void addPack(singlePlayerPack& singlePack);
		vector <singlePlayerPack> vecPlayerPack_;
	};
}
