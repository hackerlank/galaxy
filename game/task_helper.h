#pragma once

#include "json/json.h"
#include "block.h"
#include "reward.h"

#define task_checker (*gg::TaskChecker::taskChecker)

namespace gg
{
	enum{
		_task_not_set = 0,
		_task_pass_npc,                           // ����ĳ��npc

		_task_role_lv,                               // ���ǵȼ�
		_task_equipment_lv,                  // װ���ȼ�
		_task_science_lv,                         // �Ƽ��ȼ�
		_task_science_num,                    // �Ƽ�����
		_task_fire_polit,                           // ��ͷɻ�
		_task_polit_num,                         // �ɻ�����
		_task_battle_polit_num,             // ����ɻ���
		_task_advanced_times,              // ʹ�ý��״���
		_task_equipment_color,            // װ��Ʒ��
		_task_other_format,                    // ʹ����������ս��
		_task_nobility_lv,                         // ��λ�ȼ�
		_task_equipment_num,              // ��װ����
		_task_investment,                       // ����Ͷ��
		_task_attack_other_country,     // �����������
		_task_change_star_name,          // �������
		_task_use_exp_item,                   // ʹ�þ��鵤
		_task_polit_lv,                               // �ɻ��ȼ�
		_task_train_points,                       // ��������
		_task_normal_levy_times,           // ��ͨ���մ���
		_task_force_levy_times,              // ǿ�����մ���
		_task_free_produce_times,        // �����������
		_task_produce_lv,                        // �����ȼ�
		_task_hero_inheritance_times,  // Ӣ�۴��д���
		_task_guild_donate_times,        // ���˾��״���
		_task_cannon_study_times,       // �����о�����
		_task_shield_study_times,          // �����о�����
		_task_spirit_study_times,           // �����о�����
		_task_arena_win_times,              // ������ʤ������
		_task_arena_reward_times,        // ��������ȡ��������
		_task_arena_battle_times,          // ������ս������
		_task_mine_times,                        // �ɿ����
		_task_rob_other_mine_times,    // ������˿����
		_task_shop_buy_times,                // �̳ǹ�����Ʒ����
		_task_email_times,                        // �����ʼ�����
		_task_auto_upgrade_mode,       // �Զ�����ģʽ
		_task_polit_train_times,               // �ɻ���������
		_task_join_guild,                           // ��������
		_task_guild_position_primary,   // ����ְλ��������Ա
		_task_guild_battle_times,           // �μ�����ս
		_task_guild_member_num,        // ���˳�Ա��
		_task_ruler_betting_times,         // ��������Ͷע
		_task_ruler_battle_times,            // ���й�����ѡ��ս
		_task_ruler_position,                   // ��ù��ҹ�ְ

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