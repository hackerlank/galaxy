#pragma once

#include "block.h"
#include <boost/unordered_map.hpp>

namespace gg
{
	const static string shopDBStr = "gl.player_shop";
	const static string shop_goods_info_list_field_str = "gl";
	const static string shop_goods_id_field_str = "gi";
	const static string shop_goods_day_num_field_str = "dn";
	const static string shop_goods_total_num_field_str = "tn";
	const static string shop_next_update_time_field_str = "nt";

	struct DealNums
	{
	public:
		DealNums() : _day_num(0), _total_num(0){}
		DealNums(int d, int t) : _day_num(d), _total_num(t){}

		int _day_num;
		int _total_num;
	};

	typedef boost::unordered_map<int, DealNums> Id2DealNums;

	class playerShop : public Block
	{
	public:
		playerShop(playerData& own) : Block(own), _next_update_time(0){}
		virtual void autoUpdate(){}
		virtual bool get();
		virtual bool save();
		void packageBuyTimes(Json::Value& pack, int id);
		void updateAfterDeal(int id);
		void checkAndUpdate(unsigned now);
		void getNum(int id, int& day_num, int& total_num);

	private:
		unsigned _next_update_time;
		Id2DealNums _id2nums;
	};
}


