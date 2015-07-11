#pragma once
//#include "block.h"
//#include "playerCD.h"
#include "activity_rebate.h"
// int todayReceiveTimes_;
// int perdayReceiveUpdateTime_
namespace gg
{
	namespace rebate_def
	{
		const static string strActDbPlayerRebate = "gl.player_rebate";
		const static string strReceiveTimes = "rt";
		const static string strRemainTimes = "rmt";
		const static string strRechargeAmount = "ra";
		const static string strMysqlChargeRebateLog = "log_charge_rebate";

		enum
		{
			log_charge,
			log_get_gift
		};
	}

	class playerRebate : public Block
	{
	public:
		playerRebate(playerData& own);
		void autoUpdate();
		bool get();
		bool save();
		void package(Json::Value& pack);
		void rebateRecharge(act_time_info act_time, int amount);
		int pickupGift();
		bool refreshData(act_time_info act_time);

// 		int getRechargeAmount(){return rechargeAmount_;};
// 		int setRechargeAmount(int amount){rechargeAmount_ = amount;};
// 		int getReceiveTimes(){return receiveTimes_;};
// 		int setReceiveTimes(int times){receiveTimes_ = times;};
// 		int getTodayReceiveTimes(){return todayReceiveTimes_;};
// 		int setTodayReceiveTimes(int times){todayReceiveTimes_ = times;};
	private:
		int startTime_;
		int receiveTimes_;
		int rechargeAmount_;
		int todayReceiveTimes_;
		unsigned perdayReceiveUpdateTime_;
	};
}