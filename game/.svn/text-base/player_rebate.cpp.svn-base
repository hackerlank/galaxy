#include "player_rebate.h"
#include "helper.h"
#include "record_system.h"

namespace gg
{
	playerRebate::playerRebate(playerData& own) : Block(own)
	{
		startTime_ = 0;
		receiveTimes_ = 0;
		rechargeAmount_ = 0;
		todayReceiveTimes_ = 0;
		perdayReceiveUpdateTime_ = 0;
		mongo::BSONObj key = BSON(playerIDStr << 1);
		db_mgr.ensure_index(rebate_def::strActDbPlayerRebate, key);
	}

	bool playerRebate::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(rebate_def::strActDbPlayerRebate, key);

		if (!obj.isEmpty())
		{
			startTime_ = obj[act_base_def::str_act_start_time].Int();
			receiveTimes_ = obj[rebate_def::strReceiveTimes].Int();
			rechargeAmount_ = obj[rebate_def::strRechargeAmount].Int();
			return true;
		}

		return false;
	}

	bool playerRebate::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;

		obj << playerIDStr << own.playerID;
		obj << act_base_def::str_act_start_time << startTime_;
		obj << rebate_def::strReceiveTimes << receiveTimes_;
		obj << rebate_def::strRechargeAmount << rechargeAmount_;

		db_mgr.save_mongo(rebate_def::strActDbPlayerRebate, key, obj.obj());
		return true;
	}

	void playerRebate::package(Json::Value& pack)
	{
		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();
		pack[msgStr][1u][rebate_def::strReceiveTimes] = receiveTimes_;
		pack[msgStr][1u][rebate_def::strRemainTimes] = rebate_sys.getTotalReceiveTimes() - receiveTimes_;
		pack[msgStr][1u][rebate_def::strRechargeAmount] = rechargeAmount_;
	}

	void playerRebate::autoUpdate()
	{
		act_time_info act_time = rebate_sys.getActTime();
		refreshData(act_time);
		Json::Value msg;
		package(msg);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::rebate_player_info_update_resp, msg);
	}

	bool playerRebate::refreshData( act_time_info act_time )
	{
		if (act_time.start_time != startTime_)
		{
			startTime_ = act_time.start_time;
			receiveTimes_ = 0;
			rechargeAmount_ = 0;
			perdayReceiveUpdateTime_ = 0;
			todayReceiveTimes_ = 0;
			helper_mgr.insertSaveAndUpdate(this);
		}

		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > perdayReceiveUpdateTime_)
		{
			boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
			tm t = boost::posix_time::to_tm(p);
			unsigned standard_time = cur_time - t.tm_hour * 3600 - t.tm_min * 60 - t.tm_sec;

			if (t.tm_hour < 5)
			{
				perdayReceiveUpdateTime_ = (unsigned)na::time_helper::get_next_time(standard_time, 0, 5);
			}
			else
			{
				perdayReceiveUpdateTime_ = (unsigned)na::time_helper::get_next_time(standard_time, 1, 5);
			}
			helper_mgr.insertSaveAndUpdate(this);
		}

		return true;
	}

	void playerRebate::rebateRecharge( act_time_info act_time, int amount )
	{
		refreshData(act_time);
		rechargeAmount_ += amount;
		helper_mgr.insertSaveAndUpdate(this);
		helper_mgr.runSaveAndUpdate();

		vector<string> fds;
		fds.push_back(boost::lexical_cast<string, int>(rechargeAmount_));
		StreamLog::Log(rebate_def::strMysqlChargeRebateLog, own.shared_from_this(), 
			(boost::lexical_cast<string,int>(na::time_helper::get_current_time())), 
			boost::lexical_cast<string, int>(amount), fds, rebate_def::log_charge);
	}

	int playerRebate::pickupGift()
	{
		act_time_info act_time = rebate_sys.getActTime();
		refreshData(act_time);

		int recvTimes = rebate_sys.getTotalReceiveTimes();
		if (receiveTimes_ >= recvTimes)
		{
			return 1;
		}

		int perDayRcvTimes = rebate_sys.getPerDayReceiveTimes();
		if (todayReceiveTimes_ >= perDayRcvTimes)
		{
			return 2;
		}

		int gift = rebate_sys.getRewardByRechargeAmount(rechargeAmount_);
		if (gift == 0)
		{
			return 3;
		}

		own.Base.alterGoldTicket(gift);
		receiveTimes_++;
		todayReceiveTimes_++;
		helper_mgr.insertSaveAndUpdate(this);

		StreamLog::Log(rebate_def::strMysqlChargeRebateLog, own.shared_from_this(), 
			(boost::lexical_cast<string,int>(na::time_helper::get_current_time())), 
			boost::lexical_cast<string,int>(gift), rebate_def::log_get_gift);

		return 0;
	}
}

