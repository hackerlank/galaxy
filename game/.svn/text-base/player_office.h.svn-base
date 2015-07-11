#pragma once

#include "block.h"

namespace gg
{
	namespace office
	{
		static const string officeDBStr = "gl.player_office";
		const static string playerOfficialLevelStr = "olv";
		const static string playerNxtRcvSalaryTimeStr = "rst";  
	}

	class playerOffice : public Block
	{
	public:
		playerOffice(playerData& own);
		void autoUpdate();
		bool get();
		bool save();
		void package(Json::Value& pack);
		int getOffcialLevel(){return officialLevel_;}
		int getNextRcvSlrTime(){return nextReceiveSalaryTime_;}
		int setOffcialLevel(int offcialID);
		int setNextRcvSlrTime(unsigned recvTime);
		bool canPromote(int weiwang);
		int getPilotCanEnlistNum();
	private:
		unsigned nextReceiveSalaryTime_;
		int officialLevel_;
		bool isPromote_;
	};
}
