#include "player_market.h"
#include "playerManager.h"
#include "helper.h"

namespace gg
{
	void playerMarket::checkAndUpdate(unsigned now)
	{
		if (_key_id == -1)
			return;

		if (now >= _next_update_time)
		{
			_next_update_time = na::time_helper::get_next_update_time(now, 5);

			for (Id2Num::iterator iter = _id2num.begin(); iter != _id2num.end(); ++iter)
				iter->second._num = 0;

			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	bool playerMarket::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(marketDBStr, key);
		if (!obj.isEmpty()){
			checkNotEoo(obj[market_next_update_time_field_str])
				_next_update_time = obj[market_next_update_time_field_str].Int();
			checkNotEoo(obj[market_key_id_field_str])
				_key_id = obj[market_key_id_field_str].Int();
			checkNotEoo(obj[market_id_to_num_field_str])
			{
				vector<mongo::BSONElement> ele = obj[market_id_to_num_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
				{
					int id = ele[i]["id"].Int();
					int num = ele[i]["n"].Int();
					unsigned cd = ele[i]["cd"].Int();
					_id2num.insert(make_pair(id, numCd(num, cd)));
				}
			}
		}
		return true;
	}

	bool playerMarket::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID << market_next_update_time_field_str << _next_update_time << market_key_id_field_str << _key_id;
		mongo::BSONArrayBuilder array_temp;
		for (Id2Num::iterator iter = _id2num.begin(); iter != _id2num.end(); ++iter)
		{
			mongo::BSONObj b = BSON("id" << iter->first << "n" << iter->second._num << "cd" << iter->second._cd);
			array_temp.append(b);
		}
		obj << market_id_to_num_field_str << array_temp.arr();
		return db_mgr.save_mongo(marketDBStr, key, obj.obj());
	}

	int playerMarket::getNum(int key_id, int id)
	{
		if (_key_id != key_id)
		{ 
			_key_id = key_id;
			_id2num.clear();
			helper_mgr.insertSaveAndUpdate(this);
		}

		checkAndUpdate(na::time_helper::get_current_time());
		Id2Num::iterator iter = _id2num.find(id);
		if (iter != _id2num.end())
			return iter->second._num;
		else
			return 0;
	}

	unsigned playerMarket::getCd(int key_id, int id)
	{
		if (_key_id != key_id)
		{
			_key_id = key_id;
			_id2num.clear();
			helper_mgr.insertSaveAndUpdate(this);
		}

		checkAndUpdate(na::time_helper::get_current_time());
		Id2Num::iterator iter = _id2num.find(id);
		if (iter != _id2num.end())
			return iter->second._cd;
		else
			return 0;
	}

	void playerMarket::alterNum(int id, int num)
	{
		Id2Num::iterator iter = _id2num.find(id);
		if (iter != _id2num.end())
		{
			iter->second._num += num;
			iter->second._cd = na::time_helper::get_current_time() + 30;
		}
		else
			_id2num.insert(make_pair(id, numCd(num, na::time_helper::get_current_time() + 30)));
		helper_mgr.insertSaveAndUpdate(this);
	}
}