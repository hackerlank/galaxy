#pragma once

#include "json/json.h"
#include "msg_base.h"
#include "playerManager.h"

#define ally_sys (*gg::ally_system::allySys)

namespace gg
{
	const static string ally_reward_file_dir_str = "./instance/ally/reward.json";

	class ally_system
	{
	public:
		static ally_system* const allySys;

		void initData();
		void loadFile();

		void updateReq(msg_json& m, Json::Value& r);
		void searchReq(msg_json& m, Json::Value& r);
		void sendReq(msg_json& m, Json::Value& r);
		void cancelReq(msg_json& m, Json::Value& r);
		void agreeReq(msg_json& m, Json::Value& r);
		void refuseReq(msg_json& m, Json::Value& r);
		void dissolveReq(msg_json& m, Json::Value& r);
		void delReq(msg_json& m, Json::Value& r);

		void rewardReq(msg_json& m, Json::Value& r);

		void opUpdateInnerReq(msg_json& m, Json::Value& r);
		void postToUpdateInfo(playerDataPtr d);
		void infoUpdateInnerReq(msg_json& m, Json::Value& r);
		void activeOpRewardInnerReq(msg_json& m, Json::Value& r);

		unsigned getGold() const { return _gold; }
		unsigned getExtraGold() const { return _extra_gold; }

	private:
		unsigned _gold;
		unsigned _extra_gold;
	};
}