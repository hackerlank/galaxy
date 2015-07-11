#pragma once

#include <string>
using namespace std;

namespace gg
{
	const static string skillIDStr = "skillID";
	const static string skillCanCounterStr = "canCounter";
	const static string skillDoStr = "skillDo";
	//////////////////////////////////////////////////////////////////////////
	const static string skillTargetStr = "target"; //-- 可以用作技能指向
	const static string skillSignTargetStr = "signTarget";//随机目标有效//可选 -- 可以用做技能指向
	const static string skiilMainTargetStr = "mainTarget";
	const static string skillJudgeTypeStr = "judgeType";
	const static string skillActionStr = "action";
	
	//effect
	const static string skillBreakLevelTypeStr = "breakLevel";//effect 也有判定
	const static string skillEfTargetIncludeStr = "include";//目标包括
	const static string skillEfRunTypeStr = "runID";//执行ID

	//技能成功率
	const static string skillEfRunRateStr = "runRate";//技能释放成功率
	//技能指向
	const static string buffIDStr = "buffID";//buffid
	const static string buffValStr = "buffVal";//buff数值
	const static string lastRoundStr = "lastRound";//buff持续效果

	//战法
	const static string skillGetEnergyStr = "getEnergy";//能否获得士气
	const static string skillCanBlockStr = "canBlock";//能否被格挡
	const static string skillUseEnergyStr = "useEnergy";//是否使用士气结算
	const static string skillEfWarModuleStr = "warModule";//技能系数
	const static string skillDecayStr = "damageDecay";//伤害衰减

	//效果
	const static string energyStr = "energy";//士气相关数值

	//持续加/减血/能量,的效果
	const static string selfModuleStr = "selfModule";//加血公式的系数, 背水一战增伤的系数等等系数
	const static string rateTargetStr = "rateTarget";//暂时无用
	const static string fixedValueStr = "fixedValue";//吸血技能的加成固定值

	enum //skillAttackType//攻击类型
	{
		atk_to_def_type,
		war_to_anti_war_type,
		magic_to_magic_def_type,
		magic_to_add_hp_type,//这里在分开
		set_buff_type,
		set_backup_energy_type,
		alter_energy_formule_type,
		alter_energy_type,
		set_energy_type,
		damage_conver_by_hurt_type,//吸血
		absorb_energy_type,//吸魔
		doing_rate_type,//随机一些事件
		reduce_current_rate_hp_type,//按比例减少HP
	};

	enum //buffType
	{
		lock_buff,
		defend_phy_buff,
		defend_war_buff,
		defend_magic_buff,
		reduce_phy_buff,
		reduce_war_buff,
		reduce_magic_buff,
		continuous_add_hp_buff,
		continuous_reduce_hp_buff,
		continuous_reduce_energy_buff,
		reconver_backup_energy,

		end_buff,
	};

	enum //skillTargetInclude
	{
		target_inclue_all,//全部有效
		target_include_only_main,//仅主目标有效
		target_include_only_adds,//仅附目标有效
	};

	enum //skillFollow
	{
		never_break = 0,
		break_by_run_failed = (0x0001 << 0),
		break_by_dodge = (0x0001 << 1),
		break_by_buff = (0x0001 << 2),
		break_by_block = (0x0001 << 3),
		break_by_invalid = (0x0001 << 4),
		reset_continue = (0x0001 << 30),
		reset_break = (0x0001 << 31),
	};

	//敌方目标 = 目标值
	//己方目标 = 目标值 + 1000
	const static int TargetSelfOffset = 1000;

	enum //skillTargetsType//目标类型
	{
		target_single,
		target_line,
		target_row,
		target_t,
		target_all,
		target_energy_all,
		target_energy_all_not_full,
		target_energy_all_except_self,
		target_energy_all_not_full_except_self,

		target_opp_dead_line = 100,

		target_opp_single,
		target_opp_line,
//		target_all_opp,
// 		target_energy_all_opp,
// 		target_energy_all_not_full_opp,

		target_other_dead_line = 200,

		target_random,
		target_self,
		target_random_energy,
		target_random_not_full_energy,

	};


}