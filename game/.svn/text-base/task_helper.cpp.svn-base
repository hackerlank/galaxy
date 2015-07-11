#include "task_helper.h"
#include "playerManager.h"
#include "study_system.h"
#include "embattle_sys.h"
#include "item_system.h"
#include "guild_system.h"

namespace gg
{
	TaskChecker* const TaskChecker::taskChecker = new TaskChecker;

	TaskChecker::TaskChecker()
	{
		initData();
	}

	void TaskChecker::initData()
	{
		_handlers[_task_role_lv] = &TaskChecker::updateRoleLv;
		_handlers[_task_equipment_lv] = &TaskChecker::updateEquipmentLv;
		_handlers[_task_science_lv] = &TaskChecker::updateScienceLv;
		_handlers[_task_science_num] = &TaskChecker::updateScienceNum;
		_handlers[_task_fire_polit] = &TaskChecker::updateFirePolit;
		_handlers[_task_polit_num] = &TaskChecker::updatePolitNum;
		_handlers[_task_battle_polit_num] = &TaskChecker::updateBattlePolitNum;
		_handlers[_task_advanced_times] = &TaskChecker::updateAdvancedTimes;
		_handlers[_task_equipment_color] = &TaskChecker::updateEquipmentColor;
		_handlers[_task_other_format] = &TaskChecker::updateOtherFormat;
		_handlers[_task_nobility_lv] = &TaskChecker::updateNobilityLv;
		_handlers[_task_equipment_num] = &TaskChecker::updateEquipmentNum;
		_handlers[_task_investment] = &TaskChecker::updateInvestment;
		_handlers[_task_attack_other_country] = &TaskChecker::updateAttackOtherCountry;
		_handlers[_task_change_star_name] = &TaskChecker::updateChangeStarName;
		_handlers[_task_use_exp_item] = &TaskChecker::updateUseExpItem;
		_handlers[_task_polit_lv] = &TaskChecker::updatePolitLv;
		_handlers[_task_train_points] = &TaskChecker::updateTrainPoints;
		_handlers[_task_normal_levy_times] = &TaskChecker::updateNormalLevyTimes;
		_handlers[_task_force_levy_times] = &TaskChecker::updateForceLevyTimes;
		_handlers[_task_free_produce_times] = &TaskChecker::updateFreeProduceTimes;
		_handlers[_task_produce_lv] = &TaskChecker::updateProduceLv;
		_handlers[_task_hero_inheritance_times] = &TaskChecker::updateHeroInheritanceTimes;
		_handlers[_task_guild_donate_times] = &TaskChecker::updateGuildDonateTimes;
		_handlers[_task_cannon_study_times] = &TaskChecker::updateCannonStudyTimes;
		_handlers[_task_shield_study_times] = &TaskChecker::updateShieldStudyTimes;
		_handlers[_task_spirit_study_times] = &TaskChecker::updateSpiritStudyTimes;
		_handlers[_task_arena_win_times] = &TaskChecker::updateArenaWinTimes;
		_handlers[_task_arena_reward_times] = &TaskChecker::updateArenaRewardTimes;
		_handlers[_task_arena_battle_times] = &TaskChecker::updateArenaBattleTimes;
		_handlers[_task_mine_times] = &TaskChecker::updateMineTimes;
		_handlers[_task_rob_other_mine_times] = &TaskChecker::updateRobOtherMineTimes;
		_handlers[_task_shop_buy_times] = &TaskChecker::updateShopBuyTimes;
		_handlers[_task_email_times] = &TaskChecker::updateEmailTimes;
		_handlers[_task_auto_upgrade_mode] = &TaskChecker::updateAutoUpgradeMode;
		_handlers[_task_polit_train_times] = &TaskChecker::updatePolitTrainTimes;
		_handlers[_task_join_guild] = &TaskChecker::updateJoinGuild;
		_handlers[_task_guild_position_primary] = &TaskChecker::updateGuildPositionPrimary;
		_handlers[_task_guild_battle_times] = &TaskChecker::updateGuildBattleTimes;
		_handlers[_task_guild_member_num] = &TaskChecker::updateGuildMemberNum;
		_handlers[_task_ruler_betting_times] = &TaskChecker::updateRulerBettingTimes;
		_handlers[_task_ruler_battle_times] = &TaskChecker::updateRulerBattleTimes;
		_handlers[_task_ruler_position] = &TaskChecker::updateRulerPosition;
	}

	int TaskChecker::updateTaskInfo(playerDataPtr d, int type, int value1, int value2, int& value, int arg)
	{
		if(type < _task_role_lv || type >= _task_max)
		{
			LogE << __FUNCTION__ << " : error type(" << type << ")" << LogEnd;
			return _task_running;
		}
		return (this->*_handlers[type])(d, value1, value2, value, arg);
	}

	int TaskChecker::updateRoleLv(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Base.getLevel() > value1 ? value1 : d->Base.getLevel();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateEquipmentLv(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		int max_lv = d->Item.getMaxLvEquip(value2);
		value = max_lv > value1? value1 : max_lv;
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateScienceLv(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		int max_lv = d->Science.getMaxScienceLv();
		value = max_lv > value1? value1 : max_lv;
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateScienceNum(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		int num = d->Science.science_maps.size();
		value = num > value1? value1 : num;
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateFirePolit(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updatePolitNum(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Pilots.getCurrentPilotNum() > value1? value1 : d->Pilots.getCurrentPilotNum();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateBattlePolitNum(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else
		{
			vector<int> vec = d->Embattle.getCurrentFormatPilot();
			int num = 0;
			for(vector<int>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
				if(*iter != -1)
					++num;
			
			value = num >= value2? _task_finished : _task_running;
		}

		return value;
	}

	int TaskChecker::updateAdvancedTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateEquipmentColor(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = d->Item.getEquipNum(value2);
		else if(arg == value2)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateOtherFormat(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else
		{
			if(d->Embattle.getCurrentFormat() != beginEMID)
				++value;
		}

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateNobilityLv(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Office.getOffcialLevel() > value1? value1 : d->Office.getOffcialLevel();
		return value >= value1? _task_finished : _task_running; 
	}

	int TaskChecker::updateEquipmentNum(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		int num = d->Pilots.getMaxPolitEquipmentNum();
		value = num > value1? value1 : num;
		return value >= value1? _task_finished : _task_running; 
	}

	int TaskChecker::updateInvestment(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateAttackOtherCountry(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateChangeStarName(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		LogW << "not done: " << __FUNCTION__ << LogEnd;

		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateUseExpItem(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updatePolitLv(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		int lv = d->Pilots.getMaxPolitLv();
		value = lv > value1? value1 : lv;
		return value >= value1? _task_finished : _task_running; 
	}

	int TaskChecker::updateTrainPoints(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Pilots.getMaxTotalAttribute() > value1? value1 : d->Pilots.getMaxTotalAttribute();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateNormalLevyTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Levy.getNormalLevyTimes() > value1? value1 : d->Levy.getNormalLevyTimes();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateForceLevyTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Levy.getForceLevyTimes() > value1? value1 : d->Levy.getForceLevyTimes();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateFreeProduceTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Workshop.getTotalFreeCounter() > value1? value1 : d->Workshop.getTotalFreeCounter();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateProduceLv(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Workshop.level > value1? value1 : d->Workshop.level;
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateHeroInheritanceTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Pilots.getInheritTimes() > value1? value1 : d->Pilots.getInheritTimes();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateGuildDonateTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateCannonStudyTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Study.getTotalDelegateTimes(ARTIL) > value1? value1 : d->Study.getTotalDelegateTimes(ARTIL);
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateShieldStudyTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Study.getTotalDelegateTimes(SHIELD) > value1? value1 : d->Study.getTotalDelegateTimes(SHIELD);
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateSpiritStudyTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Study.getTotalPsionicStudyTimes() > value1? value1 : d->Study.getTotalPsionicStudyTimes();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateArenaWinTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateArenaRewardTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateArenaBattleTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Arena.getAllTimes() > value1? value1 : d->Arena.getAllTimes();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateMineTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateRobOtherMineTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateShopBuyTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateEmailTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateAutoUpgradeMode(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg == value2)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updatePolitTrainTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateJoinGuild(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = d->Guild.getGuildID() == -1? 0 : 1;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateGuildPositionPrimary(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
		{
			int official = guild_sys.getGuildOfficial(d);
			value = (official <= value2 && official >= 0)? _task_finished : _task_running;
		}
		else
			value = (arg <= value2 && arg >= 0)? _task_finished : _task_running;
		return value;
	}

	int TaskChecker::updateGuildBattleTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateGuildMemberNum(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		LogW << "not done: " << __FUNCTION__ << LogEnd;
		return true;
	}

	int TaskChecker::updateRulerBettingTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = 0;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateRulerBattleTimes(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		value = d->Ruler.getBattleTimes() > value1? value1 : d->Ruler.getBattleTimes();
		return value >= value1? _task_finished : _task_running;
	}

	int TaskChecker::updateRulerPosition(playerDataPtr d, int value1, int value2, int& value, int arg /* = -1 */)
	{
		if(arg == -1)
			value = d->Ruler.getTitle() == _no_title? 0 : 1;
		else if(arg != 0)
			++value;

		return value >= value1? _task_finished : _task_running;
	}

	bool mainTaskInfo::isCommit(playerDataPtr d)
	{
		return d->Warstory.currentProcess >= _value; 
	}
}
