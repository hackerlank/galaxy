#pragma once

#include "mongoDB.h" 
#include "playerManager.h"
#include "json/json.h"

#define season_sys (*gg::season::seasonSys)

namespace gg
{
	enum
	{
		season_quarter_one,
		season_quarter_two,
		season_quarter_three,
		season_quarter_four,
	};

	class season
	{
	public:
		static season* const seasonSys;
		void initData();
		int getSeason();
		int getSeason(unsigned stamp);
		unsigned getNextSeason(int quarter);
		unsigned getLastSeason(int quarter);
		unsigned getFutureSeason(int quarter, int hours = 0, int minute = 0, int sec = 0);
		void getServerOpenTime(msg_json& m, Json::Value& r);
		int getServerOpenDay();
		double serverModule();
		unsigned getServerOpenTime(){ return serverOpenTime; }
		unsigned getNextSeasonTime();
		unsigned getNextSeasonTime(unsigned now, int quarter, int hour, int minute = 0, int sec = 0);
		unsigned getNextSeasonTime(int quarter, int hour, int minute = 0, int sec = 0);
		unsigned getYear(unsigned stamp);
		unsigned getYearStamp(unsigned now);
		Json::Value serverOpenPackage();
	private:
		unsigned serverOpenTime;
	};
}