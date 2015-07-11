#include "player_activity.h"
#include "activity_system.h"
#include "helper.h"

namespace gg
{
	playerActivity::playerActivity(playerData& own) : Block(own), _next_update_time(0), _points(0)
	{
		for (unsigned i = 0; i < 5; ++i)
			_rwRecord[i] = false;
		_daily_states.reset();
		_time_limited_states.reset();
	}

	bool playerActivity::get()
	{
		getBegin(activity_db_str);

		checkNotEoo(obj[strOf_next_update_time()])
			_next_update_time = obj[strOf_next_update_time()].Int();
			
		checkNotEoo(obj[strOf_points()])
			_points = obj[strOf_points()].Int();

		checkNotEoo(obj[strOf_rwRecord()])
		{
			vector<mongo::BSONElement> eles = obj[strOf_rwRecord()].Array();
			for (unsigned i = 0; i < eles.size(); ++i)
				_rwRecord[i] = eles[i].Bool();
		}
		checkNotEoo(obj[strOf_daily_records()])
		{
			vector<mongo::BSONElement> eles = obj[strOf_daily_records()].Array();
			for (unsigned i = 0; i < eles.size(); ++i)
			{
				activity::mRecord record(eles[i]);
				_daily_records.insert(make_pair(record.getId(), record));
			}
		}
		checkNotEoo(obj[strOf_time_limited_records()])
		{
			vector<mongo::BSONElement> eles = obj[strOf_time_limited_records()].Array();
			for (unsigned i = 0; i < eles.size(); ++i)
			{
				activity::mRecord record(eles[i]);
				_time_limited_records.insert(make_pair(record.getId(), record));
			}
		}
		getEnd();
	}

	bool playerActivity::save()
	{
		saveBegin();
		obj << strOf_next_update_time() << _next_update_time
			<< strOf_points() << _points;
		{
			mongo::BSONArrayBuilder array_builder;
			for (unsigned i = 0; i < 5; ++i)
				array_builder.append(_rwRecord[i]);
			obj << strOf_rwRecord() << array_builder.arr();
		}
		{
			mongo::BSONArrayBuilder array_builder;
			for (activityRecordMap::iterator iter = _daily_records.begin(); iter != _daily_records.end(); ++iter)
			{
				mongo::BSONObjBuilder b;
				iter->second.toBson(b);
				array_builder.append(b.obj());
			}
			obj << strOf_daily_records() << array_builder.arr();
		}
		{
			mongo::BSONArrayBuilder array_builder;
			for (activityRecordMap::iterator iter = _time_limited_records.begin(); iter != _time_limited_records.end(); ++iter)
			{
				mongo::BSONObjBuilder b;
				iter->second.toBson(b);
				array_builder.append(b.obj());
			}
			obj << strOf_time_limited_records() << array_builder.arr();
		}
		saveEnd(activity_db_str);
	}

	void playerActivity::checkAndUpdate(unsigned now)
	{
		unsigned next_update_time = na::time_helper::get_next_update_time(_next_update_time);
		if (now >= next_update_time)
		{
			_next_update_time = now;
			_points = 0;
			activity_sys.resetActivity(_daily_records, 0);
			_daily_states.reset();
			activity_sys.resetActivity(_time_limited_records, 1);
			_time_limited_states.reset();
			for (unsigned i = 0; i < 5; ++i)
				_rwRecord[i] = false;

			helper_mgr.insertSave(this);
		}
	}

	void playerActivity::updateDailyActivity(int id)
	{
		if (_daily_states.test(id))
			return;
		
		activityPtr ptr = activity_sys.getDailyActivityPtr(id);
		if (ptr == activityPtr())
			return;

		activityRecordMap::iterator iter_record = _daily_records.find(id);
		if (iter_record == _daily_records.end())
		{
			_daily_records.insert(make_pair(id, activity::mRecord(id, 0)));
			iter_record = _daily_records.find(id);
		}

		if (iter_record->second.getNum() >= ptr->getLimitTimes())
		{
			_daily_states.set(id);
			return;
		}

		if (!ptr->opened(own.getOwnDataPtr()) || !ptr->isAccess(own.getOwnDataPtr()))
			return;

		iter_record->second.addNum();
		if (iter_record->second.getNum() >= ptr->getLimitTimes())
			_daily_states.set(id);
		unsigned temp = _points;
		_points += ptr->getPoints();
		activityLog(own.getOwnDataPtr(), (int)activity_system::_log_active, temp, _points, 0, id, ptr->getPoints());
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerActivity::updateTimeLimitedActivity(int id)
	{
		if (_time_limited_states.test(id))
			return;


		activityPtr ptr = activity_sys.getTimeLimitedActivityPtr(id);
		if (ptr == activityPtr())
			return;

		activityRecordMap::iterator iter_record = _time_limited_records.find(id);
		if (iter_record == _time_limited_records.end())
		{
			_time_limited_records.insert(make_pair(id, activity::mRecord(id, 0)));
			iter_record = _time_limited_records.find(id);
		}

		if (iter_record->second.getNum() >= ptr->getLimitTimes())
		{
			_time_limited_states.set(id);
			return;
		}

		if (!ptr->opened(own.getOwnDataPtr()) || !ptr->isAccess(own.getOwnDataPtr()))
			return;

		iter_record->second.addNum();
		if (iter_record->second.getNum() >= ptr->getLimitTimes())
			_time_limited_states.set(id);
		unsigned temp = _points;
		_points += ptr->getPoints();
		activityLog(own.getOwnDataPtr(), (int)activity_system::_log_active, temp, _points, 1, id, ptr->getPoints());
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerActivity::package(Json::Value& info)
	{
		info[strOf_daily_records()] = Json::arrayValue;
		Json::Value& ref_daily = info[strOf_daily_records()];
		for (activityRecordMap::iterator iter = _daily_records.begin(); iter != _daily_records.end(); ++iter)
		{
			Json::Value temp;
			iter->second.package(temp);
			ref_daily.append(temp);
		}

		info[strOf_time_limited_records()] = Json::arrayValue;
		Json::Value& ref_time_limited = info[strOf_time_limited_records()];
		for (activityRecordMap::iterator iter = _time_limited_records.begin(); iter != _time_limited_records.end(); ++iter)
		{
			Json::Value temp;
			iter->second.package(temp);
			ref_time_limited.append(temp);
		}

		info[strOf_rwRecord()] = Json::arrayValue;
		for (unsigned i = 0; i < 5; ++i)
			info[strOf_rwRecord()].append(_rwRecord[i] ? 1 : 0);

		info[strOf_points()] = _points;
	}

	void playerActivity::info()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		package(msg[msgStr][1u]);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::activity_info_resp, msg);
	}

	int playerActivity::getReward(int index)
	{
		if (index < 1 || index > 5)
			return activity::_client_error;

		if (_rwRecord[index - 1])
			return activity::_received;

		int need_points = activity_sys.getPoints(index);
		if ((int)_points < need_points)
			return activity::_points_not_enough;

		_rwRecord[index - 1] = true;
		reward rw = activity_sys.getReward(index, own.getOwnDataPtr());

		vector<int> preV;
		const vector<rewardItem>& reward_items = rw.getItems();
		ForEachC(vector<rewardItem>, iter, reward_items)
			preV.push_back(rewardItem::getItemNum(own.getOwnDataPtr(), iter->getType(), iter->getId()));

		rw.getRewardWithMail(own.getOwnDataPtr());

		int i = 0;
		ForEachC(vector<rewardItem>, iter, reward_items)
		{
			int nowV = rewardItem::getItemNum(own.getOwnDataPtr(), iter->getType(), iter->getId());
			activityRewardLog(own.getOwnDataPtr(), -1, preV[i], nowV, iter->getType(), iter->getId(), index, need_points);
			++i;
		}

		// get reward
		// Log
		helper_mgr.insertSaveAndUpdate(this);
		return activity::_success;
	}
}
