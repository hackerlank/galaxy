#pragma once
#include <string>
#include "activity_point.h"

using namespace std;

namespace gg
{
	namespace point_def
	{
		const static string strActDbPlayerPoint = "gl.player_point";
		const static string strMyPoint = "mp";
		const static string strReceiveTimesList = "rtl";
		const static string strMysqlChargePointLog = "log_charge_point";

		enum
		{
			log_charge,
			log_get_gift,
			log_point
		};
	}

	class playerPoint : public Block
	{
	public:
		playerPoint(playerData& own);
		void autoUpdate();
		bool get();
		bool save();
		void package(Json::Value& pack);
		int exchangeGift(int exType, int exIndex, Json::Value &failArr);
		bool refreshData();
		void pointRecharge(int amount );
		void alterMyPoint(int num);
		int getMyPoint();
	private:
		int startTime_;
		int myPoints_;
		vector <int> vecReceiveTimes_;
	};
}
