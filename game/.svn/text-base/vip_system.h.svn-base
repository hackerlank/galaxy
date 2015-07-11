#pragma once
#include "json/json.h"
#include "msg_base.h"
#include "action_format.hpp"

#define vip_sys (*gg::vip_system::vipSys)

using namespace na::msg;

namespace gg
{
	enum
	{
		not_vip,
		first_charge_not_pick_up,
		has_picked_up
	};

	namespace vip
	{
		const static string strVipDataPath = "./instance/vip/vip_config.json";
		const static string strMonthCardDataPath = "./instance/vip/month_card.json";

		const static string strFirstChargeReward = "first_charge_reward";

		const static string MysqlLogChargeGold = "log_charge";

		const static string strVipPrice = "vip_price";
		const static string strVipReward = "vip_reward";

		const static string strChargeType = "tye";
		const static string strChargeGold = "gld";
		const static string strChargeExtraGold = "egld";
		
	}

	enum
	{
		vip_log_recharge = 1,
		vip_log_gm_modify,
		vip_log_get_vip_gift,
		vip_log_month_card_reward,
	};

	struct monthCardConfig
	{
	public:
		monthCardConfig() : _days(0), _reward(0){}
		int _days;
		int _reward;
		bool _accumulate;
	};

	struct vipConfig 
	{
		vector<int> vip_prices;
		vector<Json::Value> vip_gift_action;
		actionVector first_charge_action;
	};

	class vip_system
	{
	public:
		static vip_system* const vipSys;
		vip_system();
		~vip_system();
		void initData();
		void vipUpdateReq(msg_json& m, Json::Value& r);
		void pickUpVipGift(msg_json& m, Json::Value& r);
		void pickUpFirstGift(msg_json& m, Json::Value& r);
		void chargeGold(msg_json& m, Json::Value& r);
		vipConfig& getVipConfig();
		Json::Value getRewardJson(int vlevel);

		//void buyMonthCard(msg_json& m, Json::Value& r);
		void monthCardReward(msg_json& m, Json::Value& r);
		const monthCardConfig& getMonthCardConfig() const { return _monthCardConfig; }
		int getVipLevelByChargeGold(int chargeGold);
		bool isVipByChargeGold(int chargeGold);
	private:
		void vipChargeResp(playerDataPtr d, int type, int gold = 0, int extra_gold = 0);


		monthCardConfig _monthCardConfig;
		vipConfig _vipConfig;
		Json::Value _vipConfigJson;
	};
}
