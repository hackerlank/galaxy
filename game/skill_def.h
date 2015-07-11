#pragma once

#include <string>
using namespace std;

namespace gg
{
	const static string skillIDStr = "skillID";
	const static string skillCanCounterStr = "canCounter";
	const static string skillDoStr = "skillDo";
	//////////////////////////////////////////////////////////////////////////
	const static string skillTargetStr = "target"; //-- ������������ָ��
	const static string skillSignTargetStr = "signTarget";//���Ŀ����Ч//��ѡ -- ������������ָ��
	const static string skiilMainTargetStr = "mainTarget";
	const static string skillJudgeTypeStr = "judgeType";
	const static string skillActionStr = "action";
	
	//effect
	const static string skillBreakLevelTypeStr = "breakLevel";//effect Ҳ���ж�
	const static string skillEfTargetIncludeStr = "include";//Ŀ�����
	const static string skillEfRunTypeStr = "runID";//ִ��ID

	//���ܳɹ���
	const static string skillEfRunRateStr = "runRate";//�����ͷųɹ���
	//����ָ��
	const static string buffIDStr = "buffID";//buffid
	const static string buffValStr = "buffVal";//buff��ֵ
	const static string lastRoundStr = "lastRound";//buff����Ч��

	//ս��
	const static string skillGetEnergyStr = "getEnergy";//�ܷ���ʿ��
	const static string skillCanBlockStr = "canBlock";//�ܷ񱻸�
	const static string skillUseEnergyStr = "useEnergy";//�Ƿ�ʹ��ʿ������
	const static string skillEfWarModuleStr = "warModule";//����ϵ��
	const static string skillDecayStr = "damageDecay";//�˺�˥��

	//Ч��
	const static string energyStr = "energy";//ʿ�������ֵ

	//������/��Ѫ/����,��Ч��
	const static string selfModuleStr = "selfModule";//��Ѫ��ʽ��ϵ��, ��ˮһս���˵�ϵ���ȵ�ϵ��
	const static string rateTargetStr = "rateTarget";//��ʱ����
	const static string fixedValueStr = "fixedValue";//��Ѫ���ܵļӳɹ̶�ֵ

	enum //skillAttackType//��������
	{
		atk_to_def_type,
		war_to_anti_war_type,
		magic_to_magic_def_type,
		magic_to_add_hp_type,//�����ڷֿ�
		set_buff_type,
		set_backup_energy_type,
		alter_energy_formule_type,
		alter_energy_type,
		set_energy_type,
		damage_conver_by_hurt_type,//��Ѫ
		absorb_energy_type,//��ħ
		doing_rate_type,//���һЩ�¼�
		reduce_current_rate_hp_type,//����������HP
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
		target_inclue_all,//ȫ����Ч
		target_include_only_main,//����Ŀ����Ч
		target_include_only_adds,//����Ŀ����Ч
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

	//�з�Ŀ�� = Ŀ��ֵ
	//����Ŀ�� = Ŀ��ֵ + 1000
	const static int TargetSelfOffset = 1000;

	enum //skillTargetsType//Ŀ������
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