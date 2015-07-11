#pragma once

#include <boost/shared_ptr.hpp>
#include "playerManager.h"
#include <boost/unordered_map.hpp>
#include "game_helper.hpp"
#include "params.hpp"
#include <string>
using namespace  std;

namespace gg
{
//	const static int buffStart = 2000;
	namespace ACTION
	{
		const static string strActionID = "actionID";
	}

	enum
	{
		action_add_silver = 1,//银币
		action_add_gold_ticket = 2,//金票
		action_add_gold = 3,//金币
		action_add_keji = 4,//科技点
		action_add_role_exp = 5,//主角经验
		action_add_pilot_exp = 6,//武将经验
		action_add_item = 7,//物品
		action_add_work_times = 8,//生产次数
		action_rate_add_item = 9,//概率掉落
		action_add_weiwang = 10,//威望
		action_active_pilot = 11,//激活武将
		action_add_pilot = 12,//武将
		action_add_keji_yaosai = 13,//要塞战加科技
		action_add_mutil_pilot_exp = 14,//多个武将同时加经验
		action_deal_war = 15,//战后处理 生成经验丹碎片
		action_add_junling = 16,//军令
		action_set_vip = 17,//设置VIP等级
		action_add_silver_vip_resource = 18,//资源礼包资源 银币
		action_add_gold_vip_resource = 19,//资源礼包资源 金票/金币
		action_add_keji_vip_resource = 20,//资源礼包资源 科技点
		action_add_weiwang_vip_resource = 21,//资源礼包资源 威望
		action_add_secretary = 22, //加秘书
		action_add_paper = 23, // 加红包币

// 		action_buff_begin =  buffStart,
// 		//////////////////////////////
// 		no_param_buff = buffStart + 1,
// 		player_protect_buff, 
// 		equip_upgrade_nofailed_buff,
// 		no_param_buff_end,
// 		////////////////////////////////////
// 		int_param_buff = buffStart + 1000,
// //		player_work_times_buff,
// 		int_param_buff_end,
// 		//////////////////////////////////////
// 		double_param_buff = buffStart + 2000,
// 		force_levy_cost_gold_rate_buff,
// 		war_story_jungong_reward_rate_buff,
// 		mine_fight_silver_reward_rate_buff,
// 		force_work_cost_gold_rate_buff,
// 		buy_item_rate_buff,
// 		all_attack_change_rate_buff,
// 		all_defend_change_rate_buff,
// 		all_hp_change_rate_buff,
// 		upgrade_attribute_cost_rate,
// 		double_param_buff_end,
// 		////////////////////
//		action_buff_end,

		//////////////
		action_end,
	};


	class actionBase
	{
	public:		
		static void destory(actionBase* aB)
		{
			aB->~actionBase();
			GGDelete(aB);
		}
		actionBase(Json::Value& dataJson)
		{
			if(!dataJson.isObject())return;
			Params::ObjectValue tmp(dataJson);
			value = tmp;
			actionID = value[ACTION::strActionID].asInt();
		}
		virtual ~actionBase(){}
		bool isValid(){return !value.isEmpty();}
		int getID(){return actionID;}
		virtual int Check(playerDataPtr d, Json::Value& Param) = 0;
		virtual Json::Value Do(playerDataPtr d, Json::Value& Param) = 0;
		virtual void clearStaticData(){}
		inline Json::Value JsonData()
		{
			if(value.getType() == Params::Array)
				return Params::ArrayValue::toJson(value);
			if(value.getType() == Params::Object)
				return Params::ObjectValue::toJson(value);
			return Json::Value::null;
		}
	protected:
		Params::ObjectValue value;
	private:
		int actionID;
	};
	typedef boost::shared_ptr<actionBase> actionPtr;
}