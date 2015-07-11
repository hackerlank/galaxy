#include "player_ruler.h"
#include "helper.h"
#include "ruler_system.h"
#include "task_system.h"

namespace gg
{
	bool playerRuler::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);

		mongo::BSONObj obj = BSON(playerIDStr << own.playerID << ruler_cd_field_str << _next_challenge_time
			<< ruler_next_update_time_field_str << _next_update_time << ruler_bet_number_field_str << _bet_number
			<< ruler_bet_option_field_str << _bet_option << ruler_reward_type_field_str << _reward_type
			<< ruler_title_field_str << _title << ruler_have_got_reward_field_str << _have_got_reward
			<< ruler_battle_times_field_str << _battle_times);
	
		return db_mgr.save_mongo(rulerDBStr, key, obj);
	}

	bool playerRuler::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(rulerDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[ruler_cd_field_str]){_next_challenge_time = obj[ruler_cd_field_str].Int();}
			checkNotEoo(obj[ruler_next_update_time_field_str]){_next_update_time = obj[ruler_next_update_time_field_str].Int();}
			checkNotEoo(obj[ruler_bet_number_field_str]){_bet_number = obj[ruler_bet_number_field_str].Int();}
			checkNotEoo(obj[ruler_bet_option_field_str]){_bet_option = obj[ruler_bet_option_field_str].Int();}
			checkNotEoo(obj[ruler_reward_type_field_str]){_reward_type = obj[ruler_reward_type_field_str].Int();}
			checkNotEoo(obj[ruler_title_field_str]){_title = obj[ruler_title_field_str].Int();}
			checkNotEoo(obj[ruler_have_got_reward_field_str]){_have_got_reward = obj[ruler_have_got_reward_field_str].Bool();}
			checkNotEoo(obj[ruler_battle_times_field_str]){_battle_times = obj[ruler_battle_times_field_str].Int();}
		}
			
		return true;
	}

	void playerRuler::package(Json::Value& pack)
	{
		pack[ruler_cd_field_str] = _next_challenge_time;
		pack[ruler_bet_number_field_str] = _bet_number;
		pack[ruler_bet_option_field_str] = _bet_option;
		pack[ruler_have_got_reward_field_str] = _have_got_reward;
		pack[ruler_reward_type_field_str] = getRewardType();
	}

	void playerRuler::checkAndUpdate(unsigned now)
	{
		if(now >= _next_update_time)
		{
			_next_update_time = season_sys.getNextSeasonTime(season_quarter_one, 14);

			_bet_number = 0;
			_bet_option = _no_bet;
			_reward_type = gg::_no_reward;
			_have_got_reward = false;

			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	int playerRuler::getTitle()
	{
		if(own.Base.getSphereID() < 0 || own.Base.getSphereID() > 2)
			return _no_title;

		if (_title == _no_title)
		{
			if (own.playerID == ruler_sys.getPlayerByTitle(_title_king, own.Base.getSphereID()))
				setTitle(_title_king);
		}
		else if (own.playerID != ruler_sys.getPlayerByTitle(_title, own.Base.getSphereID()))
			setTitle(_no_title);
		return _title;
	}

	int playerRuler::getRewardType()
	{
		if(own.Base.getSphereID() < 0 || own.Base.getSphereID() > 2)
			return gg::_no_reward;

		if(!ruler_sys.isFinished(own.Base.getSphereID()))
			return gg::_no_reward;

		if (_reward_type != gg::_no_reward)
			return _reward_type;

		if(ruler_sys.getFinalBattlePlayer(own.Base.getSphereID(), 0) == own.playerID)
		{
			_reward_type = _winner_reward;
			helper_mgr.insertSaveAndUpdate(this);
		}
		else if(ruler_sys.getFinalBattlePlayer(own.Base.getSphereID(), 1) == own.playerID)
		{
			_reward_type = _loser_reward;
			helper_mgr.insertSaveAndUpdate(this);
		}
		else if (_bet_option != _no_bet && _reward_type == gg::_no_reward)
		{
			if(_bet_option == _bet_left_side_one || _bet_option == _bet_left_side_two)
			{
				if(ruler_sys.getWinnerSide(own.Base.getSphereID()) == _left_side)
					_reward_type = _bet_win_reward;
				else
					_reward_type = _bet_lose_reward;
			}
			else
			{
				if(ruler_sys.getWinnerSide(own.Base.getSphereID()) == _right_side)
					_reward_type = _bet_win_reward;
				else
					_reward_type = _bet_lose_reward;
			}
		}
		return _reward_type;
	}

	void playerRuler::setTitle(int title)
	{
		_title = title;
		helper_mgr.insertSaveAndUpdate(this);

		if (_title != _no_title)
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_ruler_position);
	}

	void playerRuler::setRewardType(int type)
	{
		_reward_type = type;
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerRuler::clearCd()
	{
		unsigned now = na::time_helper::get_current_time();
		if(now >= _next_challenge_time)
			return -1;
		int cost = (_next_challenge_time - now) / 60 + 1;
		if(cost > own.Base.getAllGold())
			return 1;
		own.Base.alterBothGold(0 - cost);
		_next_challenge_time = 0;
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	void playerRuler::updateAfterBattle(unsigned now)
	{
		_next_challenge_time = now + 5 * 60;
		helper_mgr.insertSaveAndUpdate(this);

		addBattleTimes();
	}

	void playerRuler::setBetOption(int option)
	{
		_bet_option = option;
		helper_mgr.insertSaveAndUpdate(this);

		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_ruler_betting_times);
	}

	void playerRuler::setBetNumber(int number)
	{
		_bet_number = number;
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerRuler::getReward(int type)
	{
		if(_have_got_reward)
			return 0;

		int getsilver = 0;

		switch(_reward_type)
		{
		case _bet_win_reward:
			{
				if(_bet_option == _bet_left_side_one || _bet_option == _bet_right_side_one)
					getsilver = _bet_number * 2;
				else if(_bet_option == _bet_left_side_two || _bet_option == _bet_right_side_two)
					getsilver = _bet_number * 4;
			}
			break;
		case _winner_reward:
			{
				if(type == _betting_cost_one)
					getsilver = 600000;
				else if(type == _betting_cost_two)
					getsilver = 700000;
				else
					getsilver = 800000;
			}
			break;
		case _loser_reward:
			{
				if(type == _betting_cost_one)
					getsilver = 200000;
				else if(type == _betting_cost_two)
					getsilver = 400000;
				else
					getsilver = 600000;
			}
			break;
		case gg::_no_reward:
		case _bet_lose_reward:
		default:
			return 0;
		}

		if(getsilver != 0)
			own.Base.alterSilver(getsilver);

		_have_got_reward = true;

		helper_mgr.insertSaveAndUpdate(this);

		return getsilver;
	}

	void playerRuler::addBattleTimes()
	{
		++_battle_times;
		helper_mgr.insertSaveAndUpdate(this);

		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_ruler_battle_times);
	}
}
