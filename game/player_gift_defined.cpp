#include "player_gift_defined.h"
#include "activity_gift_defined.h"
#include "helper.h"

namespace gg
{
	singlePlayerPack singlePlayerPack::sNull_ = singlePlayerPack();

	playerGiftDefined::playerGiftDefined( playerData& own ) : Block(own)
	{
	}

	void playerGiftDefined::autoUpdate()
	{
	}

	bool playerGiftDefined::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(gift_defined_def::strActDbPlayerGiftDefined, key);

		if (!obj.isEmpty())
		{
			checkNotEoo(obj[gift_defined_def::strGiftDefinedList])
			{
				vecPlayerPack_.clear();
				singlePlayerPack spp;
				for (unsigned packIdx = 0; packIdx < obj[gift_defined_def::strGiftDefinedList].Array().size(); packIdx++)
				{
					mongo::BSONElement singleObj = obj[gift_defined_def::strGiftDefinedList].Array()[packIdx];
					checkNotEoo(singleObj[gift_defined_def::strPackID])
						spp.id_ = singleObj[gift_defined_def::strPackID].Int();
					checkNotEoo(singleObj[act_base_def::str_act_start_time])
						spp.startTime_ = singleObj[act_base_def::str_act_start_time].Int();
					checkNotEoo(singleObj[gift_defined_def::strPlayerTodayReceiveTimes])
						spp.playerTodayReceiveTimes_ = singleObj[gift_defined_def::strPlayerTodayReceiveTimes].Int();
					checkNotEoo(singleObj[gift_defined_def::strPlayerTotalReceiveTimes])
						spp.playerTotalReceiveTimes_ = singleObj[gift_defined_def::strPlayerTotalReceiveTimes].Int();
					checkNotEoo(singleObj[gift_defined_def::strNextDailyFreshTime])
						spp.nextDailyFreshTime_ = singleObj[gift_defined_def::strNextDailyFreshTime].Int();

					vecPlayerPack_.push_back(spp);
				}
			}

			return true;
		}

		return false;
	}

	bool playerGiftDefined::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		mongo::BSONArrayBuilder packArr;

		obj << playerIDStr << own.playerID;

		for (unsigned idx = 0; idx < vecPlayerPack_.size(); idx++)
		{
			singlePlayerPack spp = vecPlayerPack_[idx];
			mongo::BSONObjBuilder singleObj;
			singleObj << gift_defined_def::strPackID << spp.id_;
			singleObj << act_base_def::str_act_start_time << spp.startTime_;
			singleObj << gift_defined_def::strPlayerTodayReceiveTimes << spp.playerTodayReceiveTimes_;
			singleObj << gift_defined_def::strPlayerTotalReceiveTimes << spp.playerTotalReceiveTimes_;
			singleObj << gift_defined_def::strNextDailyFreshTime << spp.nextDailyFreshTime_;
			packArr << singleObj.obj();
		}
		obj << gift_defined_def::strGiftDefinedList << packArr.arr();

		db_mgr.save_mongo(gift_defined_def::strActDbPlayerGiftDefined, key, obj.obj());

		return true;
	}

	void playerGiftDefined::package( Json::Value& pack )
	{
	}

	bool playerGiftDefined::removeInvalidPack(const vector<singleDefinedPack> vecTotalPack)
	{
		bool ischange = false;
		for (vector <singlePlayerPack>::iterator it = vecPlayerPack_.begin(); it != vecPlayerPack_.end();)
		{
			bool isFind = false;
			for (unsigned idx = 0; idx < vecTotalPack.size(); idx++)
			{
				singleDefinedPack singleDef = vecTotalPack[idx];
				if (singleDef.id_ == it->id_ && singleDef.act_time_.start_time == it->startTime_)
				{
					isFind = true;
					break;
				}
			}
			if (!isFind)
			{
				it = vecPlayerPack_.erase(it);
				ischange = true;
			}
			else
			{
				it++;
			}
		}
		return ischange;
	}

	bool playerGiftDefined::refreshData(const vector<singleDefinedPack> vecTotalPack)
	{
		bool ischange = false;
		for (unsigned i = 0; i < vecTotalPack.size(); i++)
		{
			singleDefinedPack singleDef = vecTotalPack[i];
			singlePlayerPack &spp = getAndUpdateSinglePack(singleDef);
			if (singleDef.act_time_.start_time != spp.startTime_)
			{
				spp.startTime_ = singleDef.act_time_.start_time;
				spp.playerTodayReceiveTimes_ = 0;
				spp.playerTotalReceiveTimes_ = 0;
			}
			ischange = (spp.maintainDailyData() ? true :ischange);
		}
		ischange = removeInvalidPack(vecTotalPack) ? true : ischange;
		if (ischange)
			helper_mgr.insertSaveAndUpdate(this);

		return false;
	}

	bool singlePlayerPack::maintainDailyData()
	{
		bool isChange = false;
		int cur_time = int(na::time_helper::get_current_time());
		if (cur_time > nextDailyFreshTime_)
		{
			playerTodayReceiveTimes_ = 0;

			boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
			tm t = boost::posix_time::to_tm(p);
			unsigned standard_time = cur_time - 3600*t.tm_hour - 60*t.tm_min - t.tm_sec;
			if (t.tm_hour >= 5)
			{
				nextDailyFreshTime_ = unsigned(na::time_helper::get_next_time(standard_time, 1, 5));
			}
			else
			{
				nextDailyFreshTime_ = unsigned(na::time_helper::get_next_time(standard_time, 0, 5));
			}
			isChange = true;
		}
		return isChange;
	}

	singlePlayerPack& playerGiftDefined::getAndUpdateSinglePack(const singleDefinedPack &definedPack)
	{
		singlePlayerPack &ret = getSinglePack(definedPack);
		if (ret == singlePlayerPack::sNull_)
		{
			singlePlayerPack newPack;
			newPack.id_ = definedPack.id_;
			newPack.startTime_ = definedPack.act_time_.start_time;
			newPack.maintainDailyData();
			addPack(newPack);
			singlePlayerPack &retNew = getSinglePack(definedPack);
			return retNew;
		}
		else
		{
			return ret;
		}
	}

	singlePlayerPack& playerGiftDefined::getSinglePack(const singleDefinedPack &definedPack)
	{
		for (unsigned i = 0; i < vecPlayerPack_.size(); i++)
		{
			singlePlayerPack& singlePack = vecPlayerPack_[i];
			if (definedPack.id_ == singlePack.id_ && definedPack.act_time_.start_time == singlePack.startTime_)
				return singlePack;
		}
		return singlePlayerPack::sNull_;
	}

	void playerGiftDefined::addPack(singlePlayerPack& singlePack)
	{
		vecPlayerPack_.push_back(singlePack);
	}


	singlePlayerPack::singlePlayerPack()
	{
		id_ = -2;
		startTime_ = 0;
		playerTodayReceiveTimes_ = 0;
		playerTotalReceiveTimes_ = 0;
		nextDailyFreshTime_ = 0;
	}

	bool singlePlayerPack::operator==(singlePlayerPack& singlePack)
	{
		return singlePack.id_ == id_ && singlePack.startTime_ == startTime_;
	}
}

