#include "player_shop.h"
#include "playerManager.h"
#include "helper.h"

using namespace mongo;

namespace gg
{
	void playerShop::checkAndUpdate(unsigned now)
	{
		if(now >= _next_update_time)
		{
			_next_update_time = na::time_helper::get_next_update_time(now, 22);

			for(Id2DealNums::iterator iter = _id2nums.begin(); iter != _id2nums.end(); ++iter)
				iter->second._day_num = 0;

			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	bool playerShop::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(shopDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[shop_next_update_time_field_str])
				_next_update_time = obj[shop_next_update_time_field_str].Int();
			checkNotEoo(obj[shop_goods_info_list_field_str])
			{
				vector<mongo::BSONElement> ele = obj[shop_goods_info_list_field_str].Array();
				for(unsigned i = 0; i < ele.size(); ++i)
				{
					if(ele[i][shop_goods_id_field_str].eoo()) continue;
					if(ele[i][shop_goods_day_num_field_str].eoo()) continue;
					if(ele[i][shop_goods_total_num_field_str].eoo()) continue;

					int id = ele[i][shop_goods_id_field_str].Int();
					int day_num = ele[i][shop_goods_day_num_field_str].Int();
					int total_num = ele[i][shop_goods_total_num_field_str].Int();
					_id2nums.insert(make_pair(id, DealNums(day_num, total_num)));
				}
			}
		}
		return true;
	}

	bool playerShop::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		
		mongo::BSONArrayBuilder array_shop;
		for(Id2DealNums::iterator iter = _id2nums.begin(); iter != _id2nums.end(); ++iter)
		{
			mongo::BSONObj b = BSON(shop_goods_id_field_str << iter->first
				<< shop_goods_total_num_field_str << iter->second._total_num
				<< shop_goods_day_num_field_str << iter->second._day_num);
			array_shop.append(b);
		}
		mongo::BSONObj obj = BSON(playerIDStr << own.playerID << shop_goods_info_list_field_str << array_shop.arr()
			<< shop_next_update_time_field_str << _next_update_time);
		return db_mgr.save_mongo(shopDBStr, key, obj);
	}

	void playerShop::packageBuyTimes(Json::Value& pack, int id)
	{
		pack[shop_goods_id_field_str] = id;
		Id2DealNums::iterator iter = _id2nums.find(id);
		if(iter != _id2nums.end())
		{
			pack[shop_goods_total_num_field_str] = iter->second._total_num;
			pack[shop_goods_day_num_field_str] = iter->second._day_num;
		}
		else
		{
			pack[shop_goods_total_num_field_str] = 0;
			pack[shop_goods_day_num_field_str] = 0;
		}
	}

	void playerShop::updateAfterDeal(int id)
	{
		Id2DealNums::iterator iter = _id2nums.find(id);
		if(iter != _id2nums.end())
		{
			++iter->second._day_num;
			++iter->second._total_num;
		}
		else
			_id2nums.insert(make_pair(id, DealNums(1, 1)));

		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerShop::getNum(int id, int& day_num, int& total_num)
	{
		Id2DealNums::iterator iter = _id2nums.find(id);
		if(iter != _id2nums.end())
		{
			day_num = iter->second._day_num;
			total_num = iter->second._total_num;
		}
		else
		{
			day_num = 0;
			total_num = 0;
		}
	}

}