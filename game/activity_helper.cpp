#include "activity_helper.h"
#include "season_system.h"
#include "guild_system.h"
#include "guild_battle_system.h"
#include "activity_game_param.h"
#include "activity_system.h"

namespace gg
{
	namespace activity
	{
		cnnSeason::cnnSeason(int sprint, int summer, int autumn, int winter, mAcitivityInfo* ptr)
		{
			_season[0] = sprint;
			_season[1] = summer;
			_season[2] = autumn;
			_season[3] = winter;
			Json::Value temp;
			for (unsigned i = 0; i < 4; ++i)
				temp.append(_season[i]);
			ptr->setSeason(temp);
		}

		bool cnnSeason::isTrue(playerDataPtr d)
		{
			return _season[season_sys.getSeason()] != 0;
		}

		cnnTime::cnnTime(const int begin_time, const int end_time)
		{
			_begin_time = (begin_time / 100) * na::time_helper::HOUR + (begin_time % 100) * na::time_helper::MINUTE;
			_end_time = (end_time / 100) * na::time_helper::HOUR + (end_time % 100) * na::time_helper::MINUTE;

			unsigned now = na::time_helper::get_current_time();
			struct tm t = na::time_helper::toTm(now);
			unsigned secs = t.tm_hour * na::time_helper::HOUR + t.tm_min * na::time_helper::MINUTE + t.tm_sec;
			if ((int)secs < _begin_time)
			{
				_state = false;
				_next_update_time = now - secs + _begin_time;
			}
			else if ((int)secs < _end_time)
			{
				_state = true;
				_next_update_time = now - secs + _end_time;
			}
			else
			{
				_state = false;
				_next_update_time = now - secs + _begin_time + na::time_helper::DAY;
			}
		}

		cnnTime::cnnTime(const string& begin_time, const string& end_time)
		{
			_begin_time = 0;
			_end_time = 0;

			int index = -1;
			string temp;

			index = begin_time.find(":");
			temp = begin_time.substr(0, index);
			_begin_time += atoi(temp.c_str()) * na::time_helper::HOUR;
			temp = begin_time.substr(index + 1);
			_begin_time += atoi(temp.c_str()) * na::time_helper::MINUTE;

			index = end_time.find(":");
			temp = end_time.substr(0, index);
			_end_time += atoi(temp.c_str()) * na::time_helper::HOUR;
			temp = end_time.substr(index + 1);
			_end_time += atoi(temp.c_str()) * na::time_helper::MINUTE;

			unsigned now = na::time_helper::get_current_time();
			struct tm t = na::time_helper::toTm(now);
			unsigned secs = t.tm_hour * na::time_helper::HOUR + t.tm_min * na::time_helper::MINUTE + t.tm_sec;
			if ((int)secs < _begin_time)
			{
				_state = false;
				_next_update_time = now - secs + _begin_time;
			}
			else if ((int)secs < _end_time)
			{
				_state = true;
				_next_update_time = now - secs + _end_time;
			}
			else
			{
				_state = false;
				_next_update_time = now - secs + _begin_time + na::time_helper::DAY;
			}
		}

		bool cnnTime::isTrue(playerDataPtr d)
		{
			unsigned now = na::time_helper::get_current_time();
			while (now >= _next_update_time)
			{
				if (_state)
					_next_update_time = _next_update_time + na::time_helper::DAY - (_end_time - _begin_time);
				else
					_next_update_time = _next_update_time + (_end_time - _begin_time);
				_state = !_state;
			}
			return _state;
		}

		bool cnnGuildReward::isTrue(playerDataPtr d)
		{
			int guild_id = d->Guild.getGuildID();
			if (guild_id < 0)
				return false;
			string guild_name = guild_sys.getGuildName(guild_id);
			int occupy_id = -1;
			guild_battle_sys.getGuildOccupyArea(guild_name, occupy_id);
			return occupy_id > 0;
		}

		mAcitivityInfo::rateList mAcitivityInfo::_default_rates = mAcitivityInfo::initDefaultRate();
		Json::Value mAcitivityInfo::_default_season;

		mAcitivityInfo::mAcitivityInfo(){
			_id = activity::_mine_battle;
		}

		void mAcitivityInfo::load(const Json::Value& info)
		{
			_id = info["id"].asInt();
			_limit_times = info["maxCount"].asInt();
			_points = info["eachPoint"].asInt();
			
			const Json::Value& cnnV = info["condition"];
			if (cnnV[0u].asInt() > 0)
				addCondition(_access, creator<cnnLevel>::run(cnnV[0u].asInt()));
			if (cnnV[1u].asInt() > 0)
				addCondition(_access, creator<cnnCountry>::run());
			if (cnnV[2u].asInt() > 0)
				addCondition(_access, creator<cnnGuild>::run());

			if (info["season"] != Json::nullValue)
			{
				const Json::Value& seasonV = info["season"];
				int season[4] = { 0, 0, 0, 0 };
				for (unsigned i = 0; i < seasonV.size(); ++i)
					season[seasonV[i].asInt()] = 1;
				addCondition(_open, creator<cnnSeason>::run(season[0], season[1], season[2], season[3], this));
			}

			if (info["acTime"] != Json::nullValue)
			{
				const Json::Value& timeV = info["acTime"];
				if (timeV.size() == 1)
					addCondition(_open, creator<cnnTime>::run(timeV[0u][0u].asInt(), timeV[0u][1u].asInt()));
				if (timeV.size() > 1)
				{
					conditionList cnn_list;
					for (unsigned i = 0; i < timeV.size(); ++i)
						cnn_list.push_back(creator<cnnTime>::run(timeV[i][0u].asInt(), timeV[i][1u].asInt()));
					addCondition(_open, creator<cnnOr>::run(cnn_list));
				}
			}
		}

		Json::Value mAcitivityInfo::initDefaultSeason()
		{
			Json::Value sv;
			for (unsigned i = 0; i < 4; ++i)
				sv.append(1);
			return sv;
		}

		mAcitivityInfo::rateList mAcitivityInfo::initDefaultRate()
		{
			mAcitivityInfo::rateList rate_list(_time_limited_activity_end, 0.0);
			rate_list[_ri_li_wan_ji] = 1.0;
			rate_list[_jing_bing_qiang_zhen] = 0.2;
			rate_list[_fu_xing_gao_zhao] = 0.4;
			rate_list[_cai_yuan_guang_jin] = 0.5;
			rate_list[_tian_dao_chou_qin] = 1.0;
			rate_list[_wei_zhen_huan_yu] = 0.3;
			rate_list[_guo_fu_min_qiang] = 0.5;
			rate_list[_unused_1] = 0.0;
			rate_list[_tu_fei_meng_jin] = 0.3;
			rate_list[_fort_war] = 0.2;
			rate_list[_arena] = 30.0;
			rate_list[_ruler_competition] = 0.0;
			rate_list[_guild_battle] = 0.0;
			rate_list[_mine_battle] = 0.5;
			rate_list[_starwar] = 0.0;
			rate_list[_ruler_betting] = 0.0;
			rate_list[_world_boss] = 0.0;
			return rate_list;
		}

		double mAcitivityInfo::getRate(playerDataPtr d)
		{
			if (act_game_param_mgr.is_param_valid(_id))
				return act_game_param_mgr.get_game_param_by_id(_id);
			if (!opened(d))
				return 0.0;
			return _default_rates[_id];
		}

		Json::Value mAcitivityInfo::getSeason()
		{
			if (_season == Json::nullValue)
			{
				_season = Json::arrayValue;
				for (unsigned i = 0; i < 4; ++i)
					_season[i] = 1;
			}
			if (act_game_param_mgr.is_param_valid(_id))
			{
				static Json::Value default_season = initDefaultSeason();
				return default_season;
			}
			return _season;
		}
		void mAcitivityInfo::setSeason(const Json::Value& season)
		{
			if (_season == Json::nullValue)
			{
				_season = Json::arrayValue;
				for (unsigned i = 0; i < 4; ++i)
					_season[i] = 0;
			}
			for (unsigned i = 0; i < season.size(); ++i)
			{
				if (_season[i].asInt() == 0 && season[i].asInt() == 1)
					_season[i] = 1;
			}
		}

		void mReward::load(const Json::Value& info)
		{
			_reward.setValue(info["items"]);
			_rate = false;
			_points = info["num"].asInt();
			/*ForEachC(vector<rewardItem>, iter, _reward.getItems())
			{
			if (iter->getType() == action_add_silver || iter->getType() == action_add_weiwang)
			_rate = true;
			}*/
		}

		reward mReward::getReward(playerDataPtr d)
		{
			int reward_rate = (int)activity_sys.getRate(activity::_ri_li_wan_ji, d) + 1;
			reward new_reward;
			ForEachC(vector<rewardItem>, iter, _reward.getItems())
			{
				rewardItem item;
				item.setValue(iter->getType(), iter->getId(), iter->getNum() * reward_rate);
				if (item.getType() == action_add_silver)
				{
					int value = (int)((d->Base.getLevel() / 10 + 1) * 4.5 * item.getNum());
					item.setValue(item.getType(), item.getId(), value);
				}
				else if (item.getType() == action_add_weiwang)
					item.setValue(item.getType(), item.getId(), d->Base.getLevel() * item.getNum());
				else if (item.getType() == action_add_keji)
					item.setValue(item.getType(), item.getId(), d->Base.getLevel() * item.getNum());
				new_reward.addRewardItem(item);
			}
			return new_reward;
		}
	}
}
