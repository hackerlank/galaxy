#pragma once

#include "json/json.h"
#include "block.h"
#include "reward.h"

#define task_checker (*gg::TaskChecker::taskChecker)

namespace gg
{
	enum{
		_task_not_set = 0,
		_task_pass_npc,                           // 击败某个npc

		_task_role_lv,                               // 主角等级
		_task_equipment_lv,                  // 装备等级
		_task_science_lv,                         // 科技等级
		_task_science_num,                    // 科技数量
		_task_fire_polit,                           // 解雇飞机
		_task_polit_num,                         // 飞机数量
		_task_battle_polit_num,             // 上阵飞机数
		_task_advanced_times,              // 使用进阶次数
		_task_equipment_color,            // 装备品质
		_task_other_format,                    // 使用其他阵型战斗
		_task_nobility_lv,                         // 爵位等级
		_task_equipment_num,              // 着装数量
		_task_investment,                       // 进行投资
		_task_attack_other_country,     // 攻击其他玩家
		_task_change_star_name,          // 星球改名
		_task_use_exp_item,                   // 使用经验丹
		_task_polit_lv,                               // 飞机等级
		_task_train_points,                       // 培养属性
		_task_normal_levy_times,           // 普通征收次数
		_task_force_levy_times,              // 强制征收次数
		_task_free_produce_times,        // 免费生产次数
		_task_produce_lv,                        // 生产等级
		_task_hero_inheritance_times,  // 英雄传承次数
		_task_guild_donate_times,        // 星盟捐献次数
		_task_cannon_study_times,       // 主炮研究次数
		_task_shield_study_times,          // 护盾研究次数
		_task_spirit_study_times,           // 灵能研究次数
		_task_arena_win_times,              // 竞技场胜利次数
		_task_arena_reward_times,        // 竞技场领取奖励次数
		_task_arena_battle_times,          // 竞技场战斗次数
		_task_mine_times,                        // 采矿次数
		_task_rob_other_mine_times,    // 抢夺别人矿次数
		_task_shop_buy_times,                // 商城购买商品次数
		_task_email_times,                        // 发送邮件次数
		_task_auto_upgrade_mode,       // 自动升级模式
		_task_polit_train_times,               // 飞机培养次数
		_task_join_guild,                           // 加入星盟
		_task_guild_position_primary,   // 星盟职位到初级会员
		_task_guild_battle_times,           // 参加星盟战
		_task_guild_member_num,        // 星盟成员数
		_task_ruler_betting_times,         // 国王争霸投注
		_task_ruler_battle_times,            // 进行国王候选人战
		_task_ruler_position,                   // 获得国家官职

		_task_max,
	};

	class TaskChecker
	{
	public:
		static TaskChecker* const taskChecker;
		TaskChecker();

		int updateTaskInfo(playerDataPtr d, int type, int value1, int value2, int& value, int arg = -1);

	private:
		void initData();

		typedef int (TaskChecker::*Handler)(playerDataPtr, int, int, int&, int);
		Handler _handlers[_task_max];

		int updateRoleLv(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateEquipmentLv(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateScienceLv(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateScienceNum(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateFirePolit(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updatePolitNum(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateBattlePolitNum(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateAdvancedTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateEquipmentColor(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateOtherFormat(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateNobilityLv(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateEquipmentNum(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateInvestment(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateAttackOtherCountry(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateChangeStarName(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateUseExpItem(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updatePolitLv(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateTrainPoints(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateNormalLevyTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateForceLevyTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateFreeProduceTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateProduceLv(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateHeroInheritanceTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateGuildDonateTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateCannonStudyTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateShieldStudyTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateSpiritStudyTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateArenaWinTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateArenaRewardTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateArenaBattleTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateMineTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateRobOtherMineTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateShopBuyTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateEmailTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateAutoUpgradeMode(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updatePolitTrainTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateJoinGuild(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateGuildPositionPrimary(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateGuildBattleTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateGuildMemberNum(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateRulerBettingTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateRulerBattleTimes(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
		int updateRulerPosition(playerDataPtr d, int value1, int value2, int& value, int arg = -1);
	};

	class taskCondition
	{
	public:
		taskCondition() : _type(_task_not_set){}

		void setFromValue(Json::Value& info)
		{
			_type = info["type"].asInt();
			if(_type < _task_role_lv || _type >= _task_max)
			{
				LogE << "Error in " << __FUNCTION__ << " : type(" << _type << ")" << LogEnd;
				throw(-1);
			}
			_value1 = info["condition1"].asInt();
			_value2 = info["condition2"].asInt();
		}

		int getType() const { return _type; }

		int updateTaskInfo(playerDataPtr d, int& value, int arg = -1)
		{
			return task_checker.updateTaskInfo(d, _type, _value1, _value2, value, arg);
		}

	private:
		int _type;
		int _value1;
		int _value2;
	};

	class mainTaskInfo
	{
	public:
		mainTaskInfo() : _task_id(-1), _previous_id(-1), _next_id(-1){}

		bool isCommit(playerDataPtr d);

		void getReward(playerDataPtr d){ _reward.getRewardWithMail(d); }
		const reward& getReward(){ return _reward; }

		int getTaskId(){ return _task_id; }
		int getNextId(){ return _next_id; }
		int getPreId(){ return _previous_id; }
		void setNextId(int id){ _next_id = id; }
		void setFromValue(Json::Value& info)
		{
			_task_id = info["id"].asInt();
			_previous_id = info["precondition"].asInt();
			_value = info["description"].asInt();
			_reward.setValue(info["reward"]);
		}

	private:
		int _task_id;
		int _previous_id;
		int _next_id;
		int _value;
		reward _reward;
	};

	class branchTaskInfo
	{
	public:
		branchTaskInfo(int task_id = -1) : _task_id(task_id), _previous_id(-1), _priority(0){}

		void setFromValue(Json::Value& info)
		{
			_task_id = info["id"].asInt();
			_previous_id = info["precondition"].asInt();
			_priority = info["priority"].asInt();
			_condition.setFromValue(info);
			_reward.setValue(info["reward"]);
		}

		int updateTaskInfo(playerDataPtr d, int& value, int arg = -1){ return _condition.updateTaskInfo(d, value, arg); }

		int getTaskId() const{ return _task_id; }
		int getPriority() const{ return _priority; }
		int getType() const { return _condition.getType(); }
		int getPreId() const { return _previous_id; }
		void getReward(playerDataPtr d){ _reward.getRewardWithMail(d); }
		const reward& getReward(){ return _reward; }
		const vector<int>& getNextTaskId() const { return _next_id; }
		void addNextId(int id){ _next_id.push_back(id); }

	private:
		int _task_id;
		int _previous_id;
		vector<int> _next_id;
		int _priority;
		taskCondition _condition;
		reward _reward;
	};

	typedef boost::shared_ptr<branchTaskInfo> branchTaskPtr;
}