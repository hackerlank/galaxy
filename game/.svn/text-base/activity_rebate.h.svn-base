#pragma once
#include "activity_base.h"
#include "msg_base.h"
#include <map>
#include <boost/shared_ptr.hpp>
#include "block.h"

#define rebate_sys (*gg::act_rebate::rebateSys)

using namespace na::msg;
using namespace std;

namespace gg
{
	namespace rebate_def
	{
//		const static string str_act_db_rebate = "gl.activity_rebate";
		const static string strReceiveDueTime = "rdt";
		const static string strTotalReceiveTimes = "trt";
		const static string strPerDayReceiveTimes = "pdt";
		const static string strRewardList = "rl";
	}

	struct rebateData
	{
		int condition_;
		int reward_;
	};

	class act_rebate
		:public activity_base
	{
	public:
		static act_rebate* const rebateSys;
		act_rebate();
		~act_rebate();
		virtual void package(Json::Value& update_json);
		virtual int modify(Json::Value& modify_json);
		virtual void set_to_default();

		void initData();
		void gmActivityRebateModifyReq(msg_json& m, Json::Value& r);
		void rebateUpdateReq(msg_json& m, Json::Value& r);
		void rebatePickupGiftReq(msg_json& m, Json::Value& r);
		void rebatePlayerInfoUpdateReq(msg_json& m, Json::Value& r);
		void rebateRecharge(playerDataPtr pdata, int amount);
		int getTotalReceiveTimes();
		int getPerDayReceiveTimes(){return perDayReceiveTimes_;};
		int getRewardByRechargeAmount(int mount);
		int getReceiveDueTime(){return receiveDueTime_;};
	private:
		bool isActivityEnd();
		virtual bool get();
		virtual bool save();
		int receiveDueTime_;
		int totalReceiveTimes_;
		int perDayReceiveTimes_;
		vector<rebateData> rebate_list_;
	};

}



