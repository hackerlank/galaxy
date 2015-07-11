#pragma once 

#include "block.h"
#include <bitset>
using namespace std;

namespace gg
{ 
	const static string player_sign_db_str = "gl.player_sign";

	const static string player_sign_total_days_field_str = "td";
	const static string player_sign_is_first_time_field_str = "ift";
	const static string player_sign_next_update_time_field_str = "nt";
	const static string player_sign_records_field_str = "rd";
	const static string player_sign_records_per_day_field_str = "rpd";
	const static string player_sign_records_total_field_str = "rt";

	class playerSign : public Block{
	public:
		enum
		{
			_cycle_days = 28,
			_total_rewards = 4
		};

		typedef bitset<_cycle_days + _total_rewards> record_set;

		playerSign(playerData& own);
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate(){}
		void update();

		void package(Json::Value& msg);

		bool checkRecord(int type, int index);
		bool isFirst(){ return _is_first; }
		int getTotalDays(){ return _total_days; }
		void checkAndUpdate();
		void alterData(int type, int index);
		void playerLogin();
		void playerLogout();
		//bool getState();

	private:
		record_set _records;

		int _total_days;
		bool _is_first;
		unsigned _next_update_time;
	};
}
