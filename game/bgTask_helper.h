#pragma once

namespace gg
{

	namespace bgTask
	{
		enum
		{
			_running = 0,
			_finished,
			_rewarded,
			_timeout
		};

		enum
		{
			_bgTask_not_set = 0,
			_defeat_npc,
			_get_equipment,
			_vip_level,
			_recharge_gold,
			_consume_gold,
			_consume_times,
			_get_silver,
			_get_keji,
			_get_weiwang,
			_get_guild_contribution,
			_use_guild_contribution,
			_use_silver,
			_use_keji,
			_use_junling,
			_levy,
			_arena_challenge,
			_critical_strengthen,
			_buy_crystal,
			_study,
			_cannon_study,
			_shield_study,
			_xxx,
			_produce,
			_bring_up,
			_use_exp_items,
			_train,
			_transform,
			_mining,
			_purple_mining,
			_red_mining,
			_expert_search,
			_normal_search,
			_rob_mine,
			_rob_purple_mine,
			_rob_red_mine,
			_critical_resolve,
			_lottery_secretary,
			_low_lottery,
			_mid_lottery,
			_high_lottery,
			_upgrade_secretary,
			_upgrade_silver,
			_upgrade_gold,
			_get_secretary_chip,
			_seek_help,
			_help_guild_member,
			_help_critical,
			_donate_xxx,
			_donate_guild_science,
			_get_guild_benefit,
			_guild_trade_task,
			_use_trade_items,
			_trade_event,
			_explore,
			_one_explore,
			_ten_explore,
			_activity_point,
			_world_boss_challenge,
			_world_boss_damage,
			_starwar_get_silver,
			_starwar_occupy_star,
			_starwar_win_times,
			_starwar_battle_times,
			_starwar_get_exploit,
			_starwar_use_transfer,
			_xxx,
			_xxx,
			_paper,
			_get_paper,
			_attack_others,

			_bgTask_max
		};

		class checker
		{
			public:
				static checker* const bg_checker;
				checker();
				int update(playerDataPtr d, int type, int value1, int value2, int& value, int arg);


			private:
				void initData();

				typedef int (Checker::*Handler)(playerDataPtr, int, int, int&, int);
				Handler _handler[_bgTask_max];

				int updateCount(playerDataPtr d, int type, int value1, int value2, int& value, int arg);
				int updateVipLevel(playerDataPtr d, int type, int value1, int value2, int& value, int arg);

		};

		class condition
		{
			public:
				void load(const Json::Value& info);

				int update(playerDataPtr d, int& value, int arg)
				{
					return bg_checker.update(d, _type, _argX, _argY, value, arg);
				}

			private:
				int _type;
				int _argX;
				int _argY;
		};

		class taskInfo
		{
			public:
				void load(const Json::Value& info, bool is_daily);

				int update(playerDataPtr d, int& value, int arg)  // <arg>  -1:init  0:recheck  >0:param
				{
					return _cnn.update(d, value, arg);
				}

				int getId() const { return _id; }
				bool isDaily() const { return _daily; }
				bool isTimeOut() const
				{
					return _daily? false : (na::time_helper::get_current_time() >= _end_time);
				}

			private:
				int _id;
				int _point;
				condition _cnn;
				reward _reward;

				bool _daily;
				unsigned _end_time;
		};
		
		typedef boost::shared_ptr<taskInfo> infoPtr;
	}
}
