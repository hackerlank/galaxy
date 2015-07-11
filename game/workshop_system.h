#pragma once
#include "json/json.h"
#include "playerManager.h"
#include <boost/thread/detail/singleton.hpp>
#include "season_system.h"

/*

作坊有品质等级 需要经验升级.

*/


/*
远销宇宙按钮：开通VIP月卡以上才显示此按钮，使用后免费以2.0物价出售商品。
*/
using namespace na::msg;

#define workshop_sys (*gg::workshopManager::workshopMgr)
namespace gg
{
	enum
	{
		type_taiyan = 1,//太阳能、辐射能、离子流、暗物质
		type_fushe,
		type_liuzi,
		type_anwuzhi
	}; 

	const static string workshop_global_db_str = "gl.workshop_global";
	const static string workshop_global_db_key_str = "key";
	const static string workshop_goods_percent_field = "gp";
	const static string workshop_next_goods_percent_field = "ngp";
	const static string workshop_goods_isUp_field = "iu";//物价在上升
	const static string workshop_last_update_percent_timestamp_field = "lupt";

	/******
	只 json 不数据库的字段
	*/ 
	const static string workshop_kucun_field = "kc";//库存数量
	const static string workshop_container_size = "cs";//仓库大小


	namespace workshop_config{
		const static int limit_min  = 50;
		const static int limit_max  = 100;
		const static int random_min = 10;
		const static int random_max = 13;
		const static float host_sale_percent = 0.3f;
		const static float reflush_sale_percent_interval =  30 * 60;
	}
	class workshopManager
	{
	public: 
		workshopManager();
		static workshopManager* const workshopMgr;
		void initData();
		void scheduleUpdate(boost::system_time& sys_time);
		void update(msg_json& m, Json::Value& r);
		void package(Json::Value& msg, playerDataPtr d);
		int getQianzhiYieldCostGold(playerDataPtr d);
		bool addExp(playerDataPtr d, const int exp);//添加经验值
		void yield(msg_json& m, Json::Value& r);// 搓篮子
		void sales(msg_json& m, Json::Value& r);//卖出物品
		void autoSaleReq(msg_json& m, Json::Value& r);//卖出物品
		bool autoSale(playerDataPtr player);//自动卖出(在每次发送角色更新协议时候调用)
	private:
		void get();
		void save();


		boost::system_time tick;
		int sale_percent;
		int nextSalePercent_;
		bool sale_isUP;
		unsigned last_update_percent_timestamp;
		int vipLv;
		int getKuCunCount(playerDataPtr d);
		int levellimit;		//生产等级限制
		int getGoodsBasePrice(int goods_type, int workshop_level);//获取物品基础价格
		double getGoodsPrice(double base_price, int player_level, bool is_auto_sales);//计算单价 @is_auto_sales 是否自动卖出
		double getGoodsPrice(double base_price, int player_level, bool is_auto_sales, double use_sale_percent);
		vector<int> yield_exp_config_vect;
		vector<int> cost_gold_config_vect;

		boost::unordered_map<unsigned, unsigned> yield_probability;//生产概率表    <物品类型goods_type, 概率>  20%    <1:20>
		boost::unordered_map<unsigned, unsigned> yield_baoji_probability;//勾选暴击后的生产概率表
	};
}