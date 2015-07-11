#pragma once
#include "json/json.h"
#include "msg_base.h"
#include "playerManager.h"
#include "reward.h"
#include "player_sign.h"


#define sign_sys (*gg::sign_system::signSys)

namespace gg
{
	class sign_reward
	{
	public:
		sign_reward(bool is_first, int day = -1) : _day(day), _have_rate(false), _is_first(is_first){}
		bool setValue(Json::Value& var);
		reward getSignReward(playerDataPtr d);
		int getDay() const { return _day; }

	private:
		reward _reward;
		int _day;
		bool _have_rate;
		bool _is_first;
	};

	const static string signLogDBStr = "log_sign";

	class sign_system
	{
	public:
		enum{
			_first_time = 0,
			_cycle_time = 1,
			
			_max_time
		};

		enum{
			_log_sign_day = 0,
			_log_sign_total
		};

		const static string signDayRewardDir;
		const static string signTotalRewardDir;

		static sign_system* const signSys;

		sign_system(){}
		void initData();

		void signUpdateReq(msg_json& m, Json::Value& r);
		void signRewardReq(msg_json& m, Json::Value& r);

	private:
		void loadFile();
		
		vector<sign_reward> _rewards_day[_max_time];
		vector<sign_reward> _rewards_total[_max_time];
	};
}