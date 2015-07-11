#pragma once
#include "activity_base.h"
#include "msg_base.h"
#include <map>
#include <boost/shared_ptr.hpp>

#define act_game_param_mgr (*gg::act_game_param::actGameParamMgr)

using namespace na::msg;
using namespace std;

namespace gg
{
	
	enum actionParamIDEnum
	{
		param_discount_start = 100,
		param_vip_buy_junling,//VIP购买补给价格加成值>>vip购买补给打折
		param_force_levy_cost,//强征扣除信用点价格加成值>>强征打折
		param_transform_cost_silver,//改造时星币价格加成值>>洗练星币打折
		param_transform_cost_gold,//改造时信用点价格加成值>>洗练信用点打折
		param_force_produce_cost,//强制生产价格加成值>>强制生产打折
		param_active_cost,//快速激活价格加成值>>召唤商人打折
		param_resolve_equip_cost,//分解装备消耗打折
		param_crystal_products_cost,//商城晶核打折
		param_purchase_products_cost,//商城超值礼包打折
		param_interplane_explore_discount,//星际探险打折
		param_space_ten_explore_discount,//太空十次寻宝打折

		param_discount_end
	};

	enum actionOtherParamEnum
	{
		param_other_start = 120,
		param_other_space_explore_open,

		param_other_end
	};

	namespace game_param_def
	{
		const static string str_act_online_reward_times = "ort";
		const static string str_act_arena_rate = "ar";
		const static string str_act_active_rate = "atr";
		const static string str_param_list = "pls";
//		const static string str_act_db_game_param = "gl.activity_game_param";
	}

	class act_game_param
		:public activity_base
	{
	public:
		static act_game_param* const actGameParamMgr;
		act_game_param();
		~act_game_param();
		virtual void package(Json::Value& update_json);
		virtual int modify(Json::Value& modify_json);
		virtual void set_to_default();

		void gmActivityGameParamUpdateReq(msg_json& m, Json::Value& r);
		void gmActivityGameParamModifyReq(msg_json& m, Json::Value& r);

		int get_online_reward_times();
		int get_arena_rate(){ return arena_rate; }
		int get_activity_rate(){return activity_rate;}
		double get_game_param_by_id(int actionID);
		void initData();
		bool is_param_valid(int actionID);
	private:
		void refreshData();
		bool isActivityParamID(int actionID);
		virtual bool get();
		virtual bool save();
		int online_reward_times;
		int arena_rate;
		int activity_rate;
		map<int,double> action_show_par;
		map<int,double> action_show_par_backstage_default;
		map<int,double> action_show_par_default;
		map<int,double> action_show_par_current;
	};

}



