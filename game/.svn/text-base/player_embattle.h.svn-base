#pragma once

#include "block.h"
#include <boost/unordered_map.hpp>


namespace gg
{
	const static string embattleSaveDBStr = "gl.player_embattle";

	struct playerSingleEmbattle
	{
		playerSingleEmbattle();
		playerSingleEmbattle(const int emID);
		playerSingleEmbattle& operator=(const playerSingleEmbattle& _source);
		const int fomationID;//阵法id
		int fomation[9];//布阵信息
		int battleValue;//战力
	};
	
	const static playerSingleEmbattle NULLPlayerSingleEmbattle;
	struct embattleData;
	class playerEmbattle : public Block
	{
	public:
		playerEmbattle(playerData& own);
		virtual void autoUpdate();
		virtual void update();
		virtual bool save();
		virtual bool get();
		bool activeEMbattleWithOutSave(const embattleData& embattleC);
		void activeEmbattle(const embattleData& embattleC);
		void activeEmbattle(const int embattleID);
		int setCurrentFormat(const int embattleID);
		int getCurrentFormat();
		int setFormation(int fID, int fm[9]);
		bool isNullFormation();
		bool isHasFormation(const int formatID);
		vector<int> getCurrentFormatPilot();
		vector<int> getFormatPilot(const int embattleID);
		void removeFirePilot(const int pilotID);
		int getCurrentBV();
		void updateBattleValue(bool client = true);
	private:
		void updateSingleBattleValue(playerSingleEmbattle& embattle, const bool tick = true);
		void setCFormatID(const int embattleID);
		typedef boost::unordered_map<int, playerSingleEmbattle> playerEmbattleMap;
		playerEmbattleMap::iterator getMineEmbattle(const int fID);
		int currentFormatID;
		void package(Json::Value& pack);
		playerEmbattleMap embattleMap;
	};
}