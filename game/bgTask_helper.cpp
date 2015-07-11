#include "bgTask_helper.h"

namespace gg
{
	namespace bgTask
	{
		checker::checker()
		{
			initData();
		}

		void checker::initData()
		{
			_handlers[_defeat_npc] = &checker::updateCount;
			_handlers[_get_equipment] = &checker::updateCount;
			_handlers[_vip_level] = &checker::updateVipLevel;
			_handlers[_recharge_gold] = &checker::updateCount;
			_handlers[_consume_gold] = &checker::updateCount;
			_handlers[_consume_times] = &checker::updateCount;
			_handlers[_get_silver] = &checker::updateCount;
			_handlers[_get_keji] = &checker::updateCount;
			_handlers[_get_weiwang] = &checker::updateCount;
			_handlers[_get_guild_contribution] = &checker::updateCount;
			_handlers[_use_guild_contribution] = &checker::updateCount;
			_handlers[_use_silver] = &checker::updateCount;
			_handlers[_use_keji] = &checker::updateCount;
			_handlers[_use_junling] = &checker::updateCount;
			_handlers[_levy] = &checker::updateCount;
			_handlers[_arena_challenge] = &checker::updateCount;
			_handlers[_critical_strengthen] = &checker::updateCount;
			_handlers[_buy_crystal] = &checker::updateCount;
			_handlers[_study] = &checker::updateCount;
			_handlers[_cannon_study] = &checker::updateCount;
			_handlers[_shield_study] = &checker::updateCount;
			_handlers[_xxx] = &checker::updateCount;
			_handlers[_produce] = &checker::updateCount;
			_handlers[_bring_up] = &checker::updateCount;
			_handlers[_use_exp_items] = &checker::updateCount;
			_handlers[_train] = &checker::updateCount;
			_handlers[_transform] = &checker::updateCount;
			_handlers[_mining] = &checker::updateCount;
			_handlers[_purple_mining] = &checker::updateCount;
			_handlers[_red_mining] = &checker::updateCount;
			_handlers[_expert_search] = &checker::updateCount;
			_handlers[_normal_search] = &checker::updateCount;
			_handlers[_rob_mine] = &checker::updateCount;
			_handlers[_rob_purple_mine] = &checker::updateCount;
			_handlers[_rob_red_mine] = &checker::updateCount;
			_handlers[_critical_resolve] = &checker::updateCount;
			_handlers[_lottery_secretary] = &checker::updateCount;
			_handlers[_low_lottery] = &checker::updateCount;
			_handlers[_mid_lottery] = &checker::updateCount;
			_handlers[_high_lottery] = &checker::updateCount;
			_handlers[_upgrade_secretary] = &checker::updateCount;
			_handlers[_upgrade_silver] = &checker::updateCount;
			_handlers[_upgrade_gold] = &checker::updateCount;
			_handlers[_get_secretary_chip] = &checker::updateCount;
			_handlers[_seek_help] = &checker::updateCount;
			_handlers[_help_guild_member] = &checker::updateCount;
			_handlers[_help_critical] = &checker::updateCount;
			_handlers[_donate_xxx] = &checker::updateCount;
			_handlers[_donate_guild_science] = &checker::updateCount;
			_handlers[_get_guild_benefit] = &checker::updateCount;
			_handlers[_guild_trade_task] = &checker::updateCount;
			_handlers[_use_trade_item] = &checker::updateCount;
			_handlers[_trade_event] = &checker::updateCount;
			_handlers[_explore] = &checker::updateCount;
			_handlers[_one_explore] = &checker::updateCount;
			_handlers[_ten_explore] = &checker::updateCount;
			_handlers[_activity_point] = &checker::updateCount;
			_handlers[_world_boss_challenge] = &checker::updateCount;
		}

		int checker::update(playerDataPtr d, int type, int value1, int value2, int& value, int arg)
		{
			if(type <= bgTask::_bgTask_not_set || type >= bgTask::_bgTask_max)
			{
				LogE << __FUNCTION__ << " : error type(" << type << ")" << LogEnd;
				return _task_running;
			}
			return (this->*_handlers[type])(d, value1, value2, value, arg);
		}

		int checker::updateCount(playerDataPtr d, int type, int value1, int value2, int& value, int arg)
		{
			if(arg == -1)
				value = 0;
			else if(arg > 0)
				value += arg;
			value = value >= value1? value1 : value;
			return value >= value1? bgTask::_finish : bgTask::_running;
		}

		int checker::updateVipLevel(playerDataPtr d, int type, int value1, int value2, int& value, int arg)
		{
			int vip_level = d->Vip.getVipLevel();
			value = vip_level > value1? value1 : vip_level;
			return value >= value1? bgTask::_finish : bgTask::_running;
		}

		void condition::load(const Json::Value& info)
		{
			_type = info["t"].asInt();
			_argX = info["x"].asInt();
			_argY = info["y"].asInt();
		}

		void taskInfo::load(const Json::Value& info, bool is_daily)
		{
			_id = info["i"].asInt();
			_point = info["p"].asInt();
			_cnn.load(info);
			_reward.setValue(info["r"]);
			_daily = is_daily;
			if(!is_daily)
				_end_time = info["e"].asUInt();
		}

	}
}
