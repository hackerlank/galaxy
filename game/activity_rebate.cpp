#include "activity_rebate.h"
#include "mongoDB.h"
#include "response_def.h"
#include "playerManager.h"
#include "helper.h"
#include "gm_tools.h"

namespace gg
{
	act_rebate* const act_rebate::rebateSys = new act_rebate();

	act_rebate::act_rebate()
	{
		set_to_default();
	}

	act_rebate::~act_rebate()
	{
	}

	void act_rebate::initData()
	{
		get();
	}

	void act_rebate::package(Json::Value& update_json)
	{
		activity_base::package(update_json);
		int receiveDueTime = receiveDueTime_;
		if (gm_tools_mgr.isGmProtocal(State::getState()))
		{
			gm_tools_mgr.actTimePretreat(receiveDueTime, -1);
		}
		update_json[rebate_def::strReceiveDueTime] = receiveDueTime;
		update_json[rebate_def::strTotalReceiveTimes] = totalReceiveTimes_;
		update_json[rebate_def::strPerDayReceiveTimes] = perDayReceiveTimes_;

		Json::Value rewardArr = Json::arrayValue;
		for (unsigned i = 0; i < rebate_list_.size(); i++)
		{
			Json::Value itemArr;
			itemArr.append(rebate_list_[i].condition_);
			itemArr.append(rebate_list_[i].reward_);
			rewardArr.append(itemArr);
		}
		update_json[rebate_def::strRewardList] = rewardArr;
	}

	int act_rebate::modify(Json::Value& modify_json)
	{
// 		int ret = 0;
// 		ret = activity_base::modify(modify_json);
// 		if (ret != 0)
// 		{
// 			return ret;
// 		}
// 
// 		save();
 		return 0;
	}

	void act_rebate::set_to_default()
	{
		activity_base::set_to_default();
		receiveDueTime_ = 0;
		totalReceiveTimes_ = 0;
		perDayReceiveTimes_ = 0;
	}

	bool act_rebate::get()
	{
		mongo::BSONObj key = BSON(act_base_def::str_act_key << KEY_ACT_REBATE);
		mongo::BSONObj obj = db_mgr.FindOne(act_base_def::str_act_db_total, key);
		if(!obj.isEmpty())
		{
			superGet(obj);
			checkNotEoo(obj[rebate_def::strReceiveDueTime])
				receiveDueTime_ = obj[rebate_def::strReceiveDueTime].Int();
			checkNotEoo(obj[rebate_def::strTotalReceiveTimes])
				totalReceiveTimes_ = obj[rebate_def::strTotalReceiveTimes].Int();
			checkNotEoo(obj[rebate_def::strPerDayReceiveTimes])
				perDayReceiveTimes_ = obj[rebate_def::strPerDayReceiveTimes].Int();
			checkNotEoo(obj[rebate_def::strRewardList])
			{
				rebate_list_.clear();
				for (unsigned i = 0; i < obj[rebate_def::strRewardList].Array().size(); i++)
				{
					rebateData temp;
					temp.condition_ = obj[rebate_def::strRewardList].Array()[i].Array()[0u].Int();
					temp.reward_ = obj[rebate_def::strRewardList].Array()[i].Array()[1].Int();
					rebate_list_.push_back(temp);
				}
			}
			return true;
		}
		return false;
	}

	bool act_rebate::save()
	{
		mongo::BSONObj key = BSON(act_base_def::str_act_key << KEY_ACT_REBATE);
		mongo::BSONObjBuilder obj;

		superSave(obj);
		obj << act_base_def::str_act_key << KEY_ACT_REBATE;
		obj << rebate_def::strReceiveDueTime << receiveDueTime_;
		obj << rebate_def::strTotalReceiveTimes << totalReceiveTimes_;
		obj << rebate_def::strPerDayReceiveTimes << perDayReceiveTimes_;

		mongo::BSONArrayBuilder rewardArr;
		for (unsigned i = 0; i < rebate_list_.size(); i++)
		{
			mongo::BSONArrayBuilder itemArr;
			itemArr << rebate_list_[i].condition_;
			itemArr << rebate_list_[i].reward_;
			rewardArr << itemArr.arr();
		}
		obj << rebate_def::strRewardList << rewardArr.arr();

		return db_mgr.save_mongo(act_base_def::str_act_db_total, key, obj.obj());
	}

// 	void act_rebate::gmActivityRebateUpdateReq(msg_json& m, Json::Value& r)
// 	{
// 
// 	}

	void act_rebate::gmActivityRebateModifyReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;

		int start_time = js_msg[1][act_base_def::str_act_start_time].asInt();
		int end_time = js_msg[1][act_base_def::str_act_end_time].asInt();
		gm_tools_mgr.actTimePretreat(start_time);
		gm_tools_mgr.actTimePretreat(end_time);
		int ret = activity_base::check(start_time, end_time);

		if (ret == 1 && start_time == act_time_.start_time && end_time == act_time_.end_time)
		{
			ret = 0;
		}

		if (ret != 0)
		{
			Return(r, ret);
		}

		int receiveEndTime = js_msg[1][rebate_def::strReceiveDueTime].asInt();
		gm_tools_mgr.actTimePretreat(receiveEndTime);
		int totalReceiveTimes = js_msg[1][rebate_def::strTotalReceiveTimes].asInt();
		int perDayReceiveTimes = js_msg[1][rebate_def::strPerDayReceiveTimes].asInt();

		if (act_time_.start_time != 0 && receiveEndTime <= act_time_.end_time)
		{
			Return(r, 3);
		}

		rebate_list_.clear();
		for (unsigned i = 0; i < js_msg[1][rebate_def::strRewardList].size(); i++)
		{
			Json::Value tempJson = js_msg[1][rebate_def::strRewardList][i];
			rebateData rdata;
			rdata.condition_ = tempJson[0u].asInt();
			rdata.reward_ = tempJson[1].asInt();
			rebate_list_.push_back(rdata);
		}

		receiveDueTime_ = receiveEndTime;
		totalReceiveTimes_ = totalReceiveTimes;
		perDayReceiveTimes_ = perDayReceiveTimes;

		act_time_.start_time = start_time;
		act_time_.end_time = end_time;

		save();
		Return(r, 0);
	}

	void act_rebate::rebateUpdateReq(msg_json& m, Json::Value& r)
	{
		Json::Value updateJson;
		updateJson[msgStr][1u][updateFromStr] = State::getState();
		package(updateJson[msgStr][1u]);
		r = updateJson;
		Return(r, 0);
	}

	void act_rebate::rebatePlayerInfoUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Rebate.autoUpdate();
	}

	void act_rebate::rebatePickupGiftReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (!isActivityEnd())
		{
			Return (r, -1);
		}

		int cur_time = na::time_helper::get_current_time();
		if (cur_time > getReceiveDueTime())
		{
			Return (r, 4);
		}

		int ret = d->Rebate.pickupGift();
		Return(r, ret);
	}

	void act_rebate::rebateRecharge(playerDataPtr pdata, int amount)
	{
		if (!is_valid())
		{
			return;
		}

		pdata->Rebate.rebateRecharge(act_time_, amount);
// 		pdata->Rebate.refreshData(act_time_);
// 		pdata->Rebate.setRechargeAmount(pdata->Rebate.getRechargeAmount() + amount);
// 		helper_mgr.insertSaveSet(&pdata->Rebate);
	}

	int act_rebate::getRewardByRechargeAmount(int mount)
	{
		for (int i = rebate_list_.size() - 1; i >= 0; i--)
		{
			if (mount > rebate_list_[i].condition_)
			{
				return rebate_list_[i].reward_;
			}
		}
		return 0;
	}

	int act_rebate::getTotalReceiveTimes()
	{
		return totalReceiveTimes_;
	}

	bool act_rebate::isActivityEnd()
	{
		int cur_time = int(na::time_helper::get_current_time());
		if (cur_time >= act_time_.start_time && cur_time <= receiveDueTime_)
		{
			return true;
		}
		return false;
	}
}




