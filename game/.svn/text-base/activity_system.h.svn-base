#pragma once

#include "activity_helper.h"
#include "player_activity.h"
#include "recorder.hpp"

#define activity_sys (*gg::activity_system::activitySys)

namespace gg
{
	const static string activity_daily_activities_file_dir_str = "./instance/activity/newActivityDaily.json";
	const static string activity_time_limited_activities_file_dir_str = "./instance/activity/newActivityLimit.json";
	const static string activity_reward_file_dir_str = "./instance/activity/newActivenessRewards.json";

	const static string activityLogDBStr = "log_event";
	const static string activityRewardLogDBStr = "log_active_reward";

#define activityLog(player, tag, ...)\
	LogTemplate(activityLogDBStr, player, tag, __VA_ARGS__)       // __VA_ARGS__ = preV, nowV, f1, f2, ... 

#define activityRewardLog(player, tag, ...)\
	LogTemplate(activityRewardLogDBStr, player, tag, __VA_ARGS__)       // __VA_ARGS__ = preV, nowV, f1, f2, ... 

	class activity_system
	{
		public:
			static activity_system* const activitySys;

			enum{
				_log_active = 0
			};

			void initData();
			void infoReq(msg_json& m, Json::Value& r);
			void timeLimitedUpdateReq(msg_json& m, Json::Value& r);
			void getRewardReq(msg_json& m, Json::Value& r);

			void updateActivity(playerDataPtr d, int type, int id);

			activityPtr getDailyActivityPtr(int id);
			activityPtr getTimeLimitedActivityPtr(int id);
			void resetActivity(activityRecordMap& record_map, int type);

			int getPoints(int index);
			reward getReward(int index, playerDataPtr d);

			double getRate(int id, playerDataPtr d);
			void noticeClientUpdateBonusPar(playerDataPtr d = playerDataPtr());

		private:
			void loadFile();
			void loadReward();
			void initActivities();

			typedef boost::unordered_map<int, activityPtr> activityMap;
			activityMap _daily_activities;
			activityMap _time_limited_activities;
			typedef vector<activity::mReward> rewardList;
			rewardList _rewardList;
	};
}