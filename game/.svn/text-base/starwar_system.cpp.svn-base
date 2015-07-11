#include "starwar_system.h"
#include "response_def.h"
#include "battle_def.h"
#include "one_to_one.h"
#include "ruler_system.h"
#include "chat_system.h"
#include "record_system.h"

namespace gg
{
	void starwarTime::load(const Json::Value& info)
	{
		unsigned temp = info[0u].asUInt();
		_begin_tm.hour = temp / 100;
		_begin_tm.min = temp % 100;
		_begin_secs = _begin_tm.hour * na::time_helper::HOUR + _begin_tm.min * na::time_helper::MINUTE;
		temp = info[1u].asUInt();
		_end_tm.hour = temp / 100;
		_end_tm.min = temp % 100;
		_end_secs = _end_tm.hour * na::time_helper::HOUR + _end_tm.min * na::time_helper::MINUTE;
	}

	void starwarParam::loadFile()
	{
		loadOutput();
		loadPrime();
		loadRaid();
		loadSupply();
		loadTransfer();
		loadTruce();
		loadItem();
		loadExploit();
		loadUnityReward();
		loadRankingReward();
		loadRewardRate();
		loadToLoadNpc();
	}

	void starwarParam::loadOutput()
	{
		/*Json::Value val = na::file_system::load_jsonfile_val(starwar_country_reward_file_dir_str);
		unsigned size = val.size();
		for (unsigned i = 0; i < size; ++i)
		{
		if (i == size - 1)
		_max_output = val[i]["max"].asUInt();
		else
		_output.push_back(val[i]["gold"].asUInt());
		}*/
	}

	void starwarParam::loadTruce()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_country_reward_xiuzhan_file_dir_str);
		_war_time.load(val[0u]);
		_truce_rate = val[1u][0u].asDouble();
	}

	void starwarParam::loadPrime()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_country_huangjintime_file_dir_str);
		_prime_time.load(val[0u]);
		_move_rate = val[1u][0u].asInt();
		_supply_rate = val[2u][0u].asInt();
	}

	void starwarParam::loadSupply()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_country_player_hp_file_dir_str);
		_max_supply = val[0u][0u].asUInt();
		for (unsigned i = 0; i < val[1u].size(); ++i)
			_supply_cost[_in_path].push_back(val[1u][i].asUInt());
		for (unsigned i = 0; i < val[2u].size(); ++i)
			_supply_cost[_in_star].push_back(val[2u][i].asUInt());
	}

	void starwarParam::loadTransfer()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_country_yueqian_file_dir_str);
		_transfer_cd_cost_per_min = val[0u][0u].asUInt();
		for (unsigned i = 0; i < val[1u].size(); ++i)
			_transfer_cost.push_back(val[1u][i].asUInt());

		val = na::file_system::load_jsonfile_val(starwar_country_yueqian_cost_file_dir_str);
		_transfer_cd = val[0u][0u].asUInt();
		_transfer_times_limit_per_day = val[1u][0u].asUInt();
		_max_transfer_times = val[2u][0u].asUInt();
	}

	void starwarParam::loadRaid()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_country_touxi_file_dir_str);
		_raid_cost = val[0u][0u].asUInt();
		_raid_enable_lv_limit = val[1u][0u].asUInt();
		_raid_times_limit_per_day = val[2u][0u].asUInt();
	}

	void starwarParam::loadItem()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_country_item_file_dir_str);
		_lv_rate = val[0u];
		_type_rate = val[1u];

		val = na::file_system::load_jsonfile_val(starwar_country_item_cost_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
			_item_cost.push_back(val[i].asUInt());

		val = na::file_system::load_jsonfile_val(starwar_country_item_effect_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
		{
			effects e;
			for (unsigned j = 0; j < val[i].size(); ++j)
			{
				if (i < 2)
					e.push_back(effect(val[i][j].asDouble()));
				else
					e.push_back(effect(val[i][j].asUInt()));
			}
			_item_effect.push_back(e);
		}

		val = na::file_system::load_jsonfile_val(starwar_country_item_time_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
			_item_cd.push_back(val[i].asInt());
	}

	void starwarParam::loadExploit()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_country_gongxun_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
		{
			int first = val[i][0u].asInt();
			int second = val[i][1u].asInt();
			_exploits.push_back(make_pair(first, second));
		}
	}

	void starwarParam::loadUnityReward()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_country_reward_tongyi_file_dir_str);
		_unity_reward = val[0u].asInt();
	}

	void starwarParam::loadRankingReward()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_countreward2_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
			_ranking_reward.push_back(val[i].asInt());
	}

	void starwarParam::loadRewardRate()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_countrewardyear_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
			_reward_rate.push_back(val[i].asDouble());
	}

	void starwarParam::loadToLoadNpc()
	{
		/*Json::Value val = na::file_system::load_jsonfile_val(starwar_country_to_load_npc_dir_str);
		if (val.size() > 0)
		_load_npc = val[0u].asBool();*/
	}

	starwar_system* const starwar_system::starwarSys = new starwar_system;

	starwar_system::starwar_system()
		: _busy(false), _prime_state(false), _war_state(false), _trigger(&_busy), _unity_state(false), _max_num(0), _plane_count(0)
	{
		for (unsigned i = 0; i < 3; ++i)
			_rankList[i].init(i);

		_npcMap.assign(4, npcMap());
	}

	void starwar_system::createDir()
	{
		boost::filesystem::path path_dir(reportStarwarDir);
		if (!boost::filesystem::exists(path_dir))
			boost::filesystem::create_directory(path_dir);
	}

	void starwar_system::initTriggerItems(unsigned now)
	{
		_trigger.push(getWarStateTriggerItem(true));
		_trigger.push(getPrimeStateTriggerItem(true));
		_trigger.push(getResourceTriggerItem());
		_trigger.push(getRankingTriggerItem(true));
		_trigger.push(getUnityTriggerItem(true));
		_trigger.push(getBroadcastItem(starwar::_prime_time_coming, now));
		_trigger.push(getBroadcastItem(starwar::_prime_time_begin, now));
		_trigger.push(getPrimeTimeBroadcast(now, true));
		//_trigger.push(getTestBugItem(now));
	}

	triggerItem starwar_system::getRankingTriggerItem(bool init)
	{
		if (init)
		{
			if (_systemInfo.getSystemTime(_ranking_index) == 0)
			{
				_systemInfo.alterSystemTime(_ranking_index, season_sys.getNextSeasonTime(
					season_quarter_one, _param.getWarTime().beginTm().hour, _param.getWarTime().beginTm().min) - 4 * na::time_helper::DAY);
			}
			else
			{
				unsigned last = season_sys.getNextSeasonTime(
					season_quarter_one, _param.getWarTime().beginTm().hour, _param.getWarTime().beginTm().min) - 4 * na::time_helper::DAY;
				if (_systemInfo.getSystemTime(_ranking_index) != last)
					_systemInfo.alterSystemTime(_ranking_index, last - 4 * na::time_helper::DAY);
			}
		}
		return triggerItem(_ranking, -1, _systemInfo.getSystemTime(_ranking_index) + 4 * na::time_helper::DAY);
	}

	triggerItem starwar_system::getBroadcastItem(int type, unsigned now)
	{
		int min = _param.getPrimeTime().beginTm().min;
		int hour = _param.getPrimeTime().beginTm().hour;
		if (type == starwar::_prime_time_coming)
		{
			if (min < 5)
			{
				if (--hour < 0)
					hour = 23;
				min += 60;
			}
			min -= 5;
		}
		unsigned next_time = na::time_helper::get_next_update_time(now, hour, min);
		return triggerItem(type, -1, next_time);
	}

	triggerItem starwar_system::getUnityTriggerItem(bool init)
	{
		if (init)
		{
			if (_systemInfo.getSystemTime(_unity_check_index) == 0)
			{
				_systemInfo.alterSystemTime(_unity_check_index, season_sys.getNextSeasonTime(
					season_quarter_four, _param.getWarTime().endTm().hour, _param.getWarTime().endTm().min) - 4 * na::time_helper::DAY);
			}
			else
			{
				unsigned last = season_sys.getNextSeasonTime(
					season_quarter_four, _param.getWarTime().endTm().hour, _param.getWarTime().endTm().min) - 4 * na::time_helper::DAY;
				if (_systemInfo.getSystemTime(_unity_check_index) != last)
					_systemInfo.alterSystemTime(_unity_check_index, last - 4 * na::time_helper::DAY);
				if (last == _systemInfo.getSystemTime(_unity_time_index))
					_unity_state = true;
			}
		}
		return triggerItem(_unity, -1, _systemInfo.getSystemTime(_unity_check_index) + 4 * na::time_helper::DAY);
	}

	triggerItem starwar_system::getResourceTriggerItem()
	{
		if (_systemInfo.getSystemTime(_resource_index) == 0)
		{
			unsigned now = na::time_helper::get_current_time();
			struct tm t = na::time_helper::toTm(now);
			unsigned secs = t.tm_hour * na::time_helper::HOUR + t.tm_min * na::time_helper::MINUTE + t.tm_sec;
			secs = secs / _resource_period * _resource_period + (now - secs);
			_systemInfo.alterSystemTime(_resource_index, secs);
		}

		_next_gold_time = _systemInfo.getSystemTime(_resource_index) + _resource_period;
		return triggerItem(_resource, -1, _systemInfo.getSystemTime(_resource_index) + _resource_period);
	}

	triggerItem starwar_system::getWarStateTriggerItem(bool init)
	{
		if (init)
		{
			if (_systemInfo.getSystemTime(_last_war_time_index) == 0)
			{
				unsigned now = na::time_helper::get_current_time();
				unsigned last_begin_secs = na::time_helper::get_next_update_time(now,
					starwar_sys.getParam().getWarTime().beginTm().hour, starwar_sys.getParam().getWarTime().beginTm().min) - na::time_helper::DAY;
				unsigned  last_end_secs = na::time_helper::get_next_update_time(now,
					starwar_sys.getParam().getWarTime().endTm().hour, starwar_sys.getParam().getWarTime().endTm().min) - na::time_helper::DAY;
				if (last_begin_secs < last_end_secs)
				{
					_next_war_time = last_end_secs;
					return triggerItem(_truce_time, -1, last_end_secs);
				}
				else
				{
					_next_war_time = last_begin_secs;
					return triggerItem(_war_time, -1, last_begin_secs);
				}
			}
			else
			{
				unsigned last = _systemInfo.getSystemTime(_last_war_time_index);
				int state = _systemInfo.getState(_last_war_time_index);
				int code = state == 1 ? _war_time : _truce_time;
				_next_war_time = last;
				return triggerItem(code, -1, last);
			}
		}
		else
		{
			unsigned now = _systemInfo.getSystemTime(_last_war_time_index);
			unsigned next_begin_secs = na::time_helper::get_next_update_time(now,
				starwar_sys.getParam().getWarTime().beginTm().hour, starwar_sys.getParam().getWarTime().beginTm().min);
			unsigned  next_end_secs = na::time_helper::get_next_update_time(now,
				starwar_sys.getParam().getWarTime().endTm().hour, starwar_sys.getParam().getWarTime().endTm().min);

			if (next_begin_secs < next_end_secs)
			{
				_next_war_time = next_begin_secs;
				return triggerItem(_war_time, -1, next_begin_secs);
			}
			else
			{
				_next_war_time = next_end_secs;
				return triggerItem(_truce_time, -1, next_end_secs);
			}
		}
	}

	triggerItem starwar_system::getPrimeStateTriggerItem(bool init)
	{
		if (init)
		{
			if (_systemInfo.getSystemTime(_last_prime_time_index) == 0)
			{
				unsigned now = na::time_helper::get_current_time();
				unsigned last_begin_secs = na::time_helper::get_next_update_time(now,
					starwar_sys.getParam().getPrimeTime().beginTm().hour, starwar_sys.getParam().getPrimeTime().beginTm().min) - na::time_helper::DAY;
				unsigned  last_end_secs = na::time_helper::get_next_update_time(now,
					starwar_sys.getParam().getPrimeTime().endTm().hour, starwar_sys.getParam().getPrimeTime().endTm().min) - na::time_helper::DAY;

				if (last_begin_secs < last_end_secs)
				{
					_next_prime_time = last_end_secs;
					return triggerItem(_ordinary_time, -1, last_end_secs);
				}
				else
				{
					_next_prime_time = last_begin_secs;
					return triggerItem(_prime_time, -1, last_begin_secs);
				}
			}
			else
			{
				unsigned last = _systemInfo.getSystemTime(_last_prime_time_index);
				int state = _systemInfo.getState(_last_prime_time_index);
				int code = state == 1 ? _prime_time : _ordinary_time;
				_next_prime_time = last;
				return triggerItem(code, -1, last);
			}
		}
		else
		{
			unsigned now = _systemInfo.getSystemTime(_last_prime_time_index);
			unsigned next_begin_secs = na::time_helper::get_next_update_time(now,
				starwar_sys.getParam().getPrimeTime().beginTm().hour, starwar_sys.getParam().getPrimeTime().beginTm().min);
			unsigned  next_end_secs = na::time_helper::get_next_update_time(now,
				starwar_sys.getParam().getPrimeTime().endTm().hour, starwar_sys.getParam().getPrimeTime().endTm().min);

			if (next_begin_secs < next_end_secs)
			{
				_next_prime_time = next_begin_secs;
				return triggerItem(_prime_time, -1, next_begin_secs);
			}
			else
			{
				_next_prime_time = next_end_secs;
				return triggerItem(_ordinary_time, -1, next_end_secs);
			}
		}
	}

	triggerItem starwar_system::getTestBugItem(unsigned now)
	{
		struct tm t = na::time_helper::toTm(now);
		unsigned secs = t.tm_hour * 3600 + t.tm_min * 60 + t.tm_sec;
		unsigned next = now - secs + 21 * 3600 + 1;
		return triggerItem(starwar::_test_bug, 187695123, next);
	}

	triggerItem starwar_system::getPrimeTimeBroadcast(unsigned now, bool init /* = false */)
	{
		const int cycle = 5 * na::time_helper::MINUTE;

		if (init)
		{
			struct tm t = na::time_helper::toTm(now);
			unsigned secs = t.tm_hour * na::time_helper::HOUR + t.tm_min * na::time_helper::MINUTE + t.tm_sec;
			unsigned next = 0;
			if (secs > _param.getPrimeTime().beginSecs() && secs < _param.getPrimeTime().endSecs())
				next = now / cycle * cycle;
			else
				next = na::time_helper::get_next_update_time(now, _param.getPrimeTime().beginTm().hour, _param.getPrimeTime().beginTm().min) + cycle;
			return triggerItem(_prime_time_running, -1, next);
		}
		else
		{
			unsigned next = now + 5 * na::time_helper::MINUTE;
			struct tm t = na::time_helper::toTm(next);
			unsigned secs = t.tm_hour * na::time_helper::HOUR + t.tm_min * na::time_helper::MINUTE + t.tm_sec;
			if (secs == _param.getPrimeTime().endSecs())
				next = na::time_helper::get_next_update_time(now, _param.getPrimeTime().beginTm().hour, _param.getPrimeTime().beginTm().min) + cycle;
			return triggerItem(_prime_time_running, -1, next);
		}
	}

	int starwar_system::tickWarState(const triggerItem& item)
	{
		if (item.getCode() == _war_time)
		{
			if (_unity_state)
				_war_state = false;
			else
				_war_state = true;
		}
		else
		{
			_war_state = false;
		}

		_systemInfo.alterSystemTime(_last_war_time_index, item.getTime(), _war_state? 1 : 0);
		_trigger.push(getWarStateTriggerItem());
		sendMainInfo(playerDataPtr());
		return _success;
	}

	int starwar_system::tickPrimeState(const triggerItem& item)
	{
		if (item.getCode() == _prime_time)
		{
			_prime_state = true;
			_max_num = _playerList.size();
			LogI << "starwar online: " << _max_num << LogEnd;
			if (_unity_state)
			{
				_war_state = true;
				_unity_state = false;
			}
		}
		else
		{
			_prime_state = false;
			_trigger.push(triggerItem(_prime_time_summary, -1, item.getTime() + 5));
			LogI << "starwar online: " << _max_num << LogEnd;
		}

		tickSupplyRate(item.getTime());
		_pathList.resetRate(item.getTime());
		//mainInfoBroadcast();

		_systemInfo.alterSystemTime(_last_prime_time_index, item.getTime(), _prime_state? 1 : 0);
		_trigger.push(getPrimeStateTriggerItem());
		sendMainInfo(playerDataPtr());
		return _success;
	}

	int starwar_system::tickRanking(unsigned now)
	{
		for (unsigned i = 0; i < 3; ++i)
		{
			_rankList[i].broadcast();
			_rankList[i].tick();
		}

		unsigned last = season_sys.getNextSeasonTime(
			season_quarter_one, _param.getWarTime().beginTm().hour, _param.getWarTime().beginTm().min) - 4 * na::time_helper::DAY;

		_systemInfo.alterSystemTime(_ranking_index, last);
		_trigger.push(getRankingTriggerItem());
		return _success;
	}

	int starwar_system::tickBroadcast(const triggerItem& item)
	{
		int code = item.getCode();
		int b_type = -1;
		int r_type = -1;
		if (code == starwar::_prime_time_coming)
		{
			b_type = BROADCAST::starwar_prime_time_coming;
			r_type = ROLLBROADCAST::roll_starwar_prime_time_coming;
		}
		else
		{
			b_type = BROADCAST::starwar_prime_time_begin;
			r_type = ROLLBROADCAST::roll_starwar_prime_time_begin;
		}
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatBroadcastID] = b_type;
		msg[msgStr][1u][senderChannelStr] = chat_all;
		chat_sys.chatOption(chat_all, gate_client::chat_broadcast_resp, msg, playerDataPtr());
		chat_sys.sendToAllRollBroadCastCommon(r_type);

		unsigned now = na::time_helper::get_current_time();
		_trigger.push(getBroadcastItem(code, now));
		return starwar::_success;
	}

	int starwar_system::tickLoadPlayer(const triggerItem& item)
	{
		int player_id = item.getId();
		loadHelperMap::iterator iter = _loadHelperMap.find(player_id);
		if (iter != _loadHelperMap.end())
		{
			const starwar::position& pos = iter->second._position;
			const starwar::playerInfo& info = iter->second._playerInfo;
			pathPtr ptr = _pathList.getPath(pos.getId());
			ptr->load(pos.getAction().getType(), pos.getTime(), info, pos.getFromStarId());
			_loadHelperMap.erase(iter);
		}
		return starwar::_success;
	}

	int starwar_system::tickPrimeTimeBroadcast(const triggerItem& item)
	{
		chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_starwar_prime_time_running, 1);
		_trigger.push(getPrimeTimeBroadcast(item.getTime()));
		LogI << "starwar online: " << _max_num << LogEnd;
		_max_num = _playerList.size();
		return starwar::_success;
	}

	int starwar_system::tickPrimeTimeSummaryBroadcast(const triggerItem& item)
	{
		for (unsigned i = 0; i < 3; ++i)
			_rankList[i].primeTimeSummary();
		return starwar::_success;
	}

	int starwar_system::tickClearSignInfo(const triggerItem& item)
	{
		int country_id = _systemInfo.clearSignInfo(item.getId());
		if (country_id != -1)
			updateSignInfo(country_id);
		return starwar::_success;
	}

	int starwar_system::tickTestBug(const triggerItem& item)
	{
		int player_id = item.getId();
		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		Json::Value msg;
		msg.append(12);
		msg.append(0);
		starwar::message m(starwar::_move, msg, item.getTime() - 1, d);
		move(m);
		return starwar::_success;
	}

	int starwar_system::tickUnity(const triggerItem& item)
	{
		int country_id = _occupyInfo.getUnityCountry();

		if (country_id != -1)
		{
			_unity_state = true;
			reset(item.getTime());
			for (unsigned i = 0; i < 3; ++i)
			{
				if (i == country_id)
					_systemInfo.alterUnityReward(i, _param.getUnityReward());
				else
					_systemInfo.alterUnityReward(i, 0);
			}

			noticeUnity(country_id);
			_systemInfo.alterSystemTime(_unity_time_index, item.getTime());
			const rulerTitleInfoList& title_list = ruler_sys.getTitleInfoList(country_id);
			_systemInfo.pushGreatEvent(starwar::greatEvent(season_sys.getYear(item.getTime()), country_id, title_list));
			_occupyInfo.clear();
			sendMainInfo(playerDataPtr());

			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(country_id));
			fields.push_back(boost::lexical_cast<string, int>(item.getTime()));
			fields.push_back(boost::lexical_cast<string, int>(title_list[_title_king].getPlayerId()));
			fields.push_back(title_list[_title_king].getNickName());
			StreamLog::Log(starwarLogDBStr, -1, "", -1, "", "", fields, playerStarwar::_log_unity);

			for (rulerTitleInfoList::const_iterator iter = title_list.begin(); iter != title_list.end(); ++iter)
			{
				if (iter->getPlayerId() == -1)
					continue;
				fields.clear();
				fields.push_back(boost::lexical_cast<string, int>(country_id));
				fields.push_back(boost::lexical_cast<string, int>(item.getTime()));
				fields.push_back(boost::lexical_cast<string, int>(iter->getPlayerId()));
				fields.push_back(iter->getNickName());
				fields.push_back(boost::lexical_cast<string, int>(iter->getTitle()));
				StreamLog::Log(starwarLogDBStr, -1, "", -1, "", "", fields, playerStarwar::_log_great_event);
			}
		}

		unsigned last = season_sys.getNextSeasonTime(
			season_quarter_four, _param.getWarTime().endTm().hour, _param.getWarTime().endTm().min) - 4 * na::time_helper::DAY;
		_systemInfo.alterSystemTime(_unity_check_index, last);
		_trigger.push(getUnityTriggerItem());
		return _success;
	}

	int starwar_system::tickResource(unsigned now)
	{
		for (starList::iterator iter = _starList.begin(); iter != _starList.end(); ++iter)
			(*iter)->tick();
		
		for (set<basePlayer>::iterator iter = _playerList.begin(); iter != _playerList.end(); ++iter)
		{
			playerDataPtr d = player_mgr.getOnlinePlayer(iter->getPlayerId());
			if (d != playerDataPtr())
				d->Starwar.update();
		}

		_systemInfo.alterSystemTime(_resource_index, now);
		_trigger.push(getResourceTriggerItem());
		return _success;
	}

	void starwar_system::initData()
	{
		unsigned now = na::time_helper::get_current_time();

		loadNpc();
		loadFile();
		loadDB();
		createDir();

		_starList.update(true);
		_pathList.update(true);
		_trigger.init(boost::bind(&starwar_system::handleEvent, this, _1));

		initTriggerItems(now);
	}

	void starwar_system::tickMainInfo()
	{
		_starList.update();
		_pathList.update();

		if (_playerList.empty())
			return;
		
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][starwar_path_info_field_str] = _pathList.getPathChangeInfo();
		msg[msgStr][1u][starwar_star_info_field_str] = _starList.getOccupyChangeInfo();
		if (msg[msgStr][1u][starwar_path_info_field_str].size() == 0
			&& msg[msgStr][1u][starwar_star_info_field_str].size() == 0)
			return;

		string str = msg.toStyledString();
		na::msg::msg_json mj(gate_client::starwar_main_update_resp, str);
		for (set<basePlayer>::iterator iter = _playerList.begin(); iter != _playerList.end(); ++iter)
		{
			playerDataPtr d = player_mgr.getOnlinePlayer(iter->getPlayerId());
			if (d != playerDataPtr() && d->netID >= 0)
				player_mgr.sendToPlayer(d->playerID, d->netID, mj);
		}	
	}

	void starwar_system::tickSupplyRate(unsigned now)
	{
		_starList.tickSupplyRate(getSupplyRate(), now);
	}

	void starwar_system::tick(unsigned now)
	{
		static unsigned next_update_time = na::time_helper::get_current_time();
		if (now >= next_update_time)
		{
			next_update_time = now + _mainInfo_period;
			tickMainInfo();
		}
	}

	void starwar_system::loadFile()
	{
		_param.loadFile();
		loadTask();

		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(starwar_config_file_dir_str, jv);
		for (unsigned i = 0; i < jv.size(); ++i)
		{
			const Json::Value& starInfo = jv[i];
			int star_id = starInfo["id"].asInt();
			starPtr ptr(new star(star_id));
			ptr->init(starInfo);
			_starList.push(ptr);
		}

		mGraph graph(jv.size());
		for (unsigned i = 0; i < jv.size(); ++i)
		{
			const Json::Value& starInfo = jv[i];
			int star_id = starInfo["id"].asInt();
			const Json::Value& link_stars = starInfo["relationship"];
			const Json::Value& path_types = starInfo["relationship_type"];
			const Json::Value& cost_times = starInfo["distance"];

			for (unsigned i = 0; i < link_stars.size(); ++i)
			{
				int link_star = link_stars[i].asInt();
				int left_star = star_id > link_star ? link_star : star_id;
				int right_star = star_id > link_star ? star_id : link_star;
				int path_id = left_star + 100 * right_star;
				pathPtr ptr = _pathList.getPath(path_id);
				if (ptr == pathPtr())
				{
					pathPtr new_ptr(new path(path_id, left_star, right_star));
					_pathList.push(new_ptr);
					_starList.getStar(left_star)->addPath(new_ptr);
					_starList.getStar(right_star)->addPath(new_ptr);
					ptr = new_ptr;
				}
				ptr->setCostTime(star_id, link_star, cost_times[i].asInt());
				ptr->setPathType(star_id, link_star, path_types[i].asInt());
				if (ptr->isAccess(star_id, link_star))
					graph.load(star_id, link_star, cost_times[i].asInt());
			}
		}

		vector<mPathList> path_lists;
		for (unsigned i = 0; i < jv.size(); ++i)
		{
			mPathList path_list;
			shortestPath_DIJ(graph, path_list, i);
			path_lists.push_back(path_list);
		}

		_shortestPath.load(path_lists);
	}

	void starwar_system::loadTask()
	{
		Json::Value val = na::file_system::load_jsonfile_val(starwar_task_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
		{
			starwar::taskPtr t(new starwar::taskInfo);
			t->load(val[i]);
			_taskList.insert(make_pair(t->getId(), t));
			_taskProbs.append(t->getRate());
		}
	}

	void starwar_system::loadNpc()
	{
		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(starwar_npc_file_dir_str, jv);

		for (unsigned i = 0; i < jv.size(); ++i)
		{
			npcMap npc_map;
			Json::Value& info = jv[i];
			int id = info["id"].asInt();
			Json::Value& map_data = info["MapData"];
			int size = map_data.size();
			for (unsigned j = 0; j < map_data.size(); ++j)
			{
				int local_id = map_data[j]["localID"].asInt();
				int exploit = map_data[j]["gongxun"].asInt();
				string name = map_data[j]["name"].asString();
				int face_id = map_data[j]["bgId"].asInt();
				_exploit_npc.push_back(exploit);
				Json::Value& army_data = map_data[j]["armyData"];
				mapDataPtr ptr = warStory::create();
				ptr->npcArmy = war_story.formatNpcArmy(army_data);
				ptr->frontLocalID = exploit;
				ptr->localID = local_id;
				ptr->mapName = name;
				ptr->faceID = face_id;
				_npcMap[id - 1].insert(make_pair(local_id, ptr));
			}
		}
	}

	void starwar_system::loadStarDB()
	{
		db_mgr.ensure_index(starwar_star_info_db_str, BSON(starwar_star_id_field_str << 1));

		objCollection objs;
		objs = db_mgr.Query(starwar_star_info_db_str);

		for (unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];
			int star_id = obj[starwar_star_id_field_str].Int();
			starPtr ptr = _starList.getStar(star_id);
			if (ptr != starPtr())
				ptr->load(obj);
		}
	}

	void starwar_system::loadSystemInfoDB()
	{
		_systemInfo.load();
	}

	void starwar_system::loadRankingDB()
	{
		objCollection objs;
		objs = db_mgr.Query(starwar_old_ranking_db_str);

		for (unsigned i = 0; i < objs.size(); ++i)
		{
			int rank = objs[i][starwar_rank_field_str].Int();
			_rankList[rank / 1000].loadLastTerm(objs[i][starwar_player_id_field_str].Int()
				,  rank % 1000, objs[i][starwar_exploit_field_str].Int());
		}
	}

	void starwar_system::loadPlayerInfo(playerMap& player_map)
	{
		objCollection objs;
		objs = db_mgr.Query(ruler_title_info_list_db_str);
		typedef map<int, int> titleMap;
		titleMap title_map;
		for(unsigned i = 0; i < objs.size(); ++i)
			title_map.insert(make_pair(objs[i][ruler_player_id_field_str].Int(), objs[i][ruler_title_field_str].Int()));

		objs.clear();
		objs = db_mgr.Query(playerBaseDBStr);
		for(unsigned i = 0; i < objs.size(); ++i)
		{
			int country_id = objs[i][playerSphereIDStr].Int();
			if(country_id < 0 || country_id > 2)
				continue;
			
			int player_id = objs[i][playerIDStr].Int();
			titleMap::iterator iter = title_map.find(player_id);
			if(iter == title_map.end())
				player_map.insert(make_pair(player_id, 
					starwar::playerInfo(player_id, objs[i][playerNameStr].String(), country_id
					, objs[i][playerFaceIDStr].Int(), objs[i][playerLevelStr].Int(), 0)));
			else
				player_map.insert(make_pair(player_id, 
					starwar::playerInfo(player_id, objs[i][playerNameStr].String(), country_id
					, objs[i][playerFaceIDStr].Int(), objs[i][playerLevelStr].Int(), 0, (iter->second) % 1000)));
		}
	}

	void starwar_system::loadStarwarInfo(playerMap& player_map)
	{
		unsigned now = na::time_helper::get_current_time();

		objCollection objs;
		objs = db_mgr.Query(starwar_db_str);
		for(unsigned i = 0; i < objs.size(); ++i)
		{
			int player_id = objs[i][playerIDStr].Int();
			playerMap::iterator iter = player_map.find(player_id);
			if(iter == player_map.end())
				continue;

			starwar::position pos;
			pos.load(objs[i][starwar_position_field_str]);
			int last_unity_time = objs[i][starwar_last_unity_time_field_str].Int();
			int sup = objs[i][starwar_supply_field_str].Int();
			int base_sup = objs[i][starwar_base_supply_field_str].Int();
			if (last_unity_time != _systemInfo.getSystemTime(_unity_time_index))
			{
				if (pos.getType() != starwar::_in_star || pos.getId() != star::mainstar[iter->second.getCountryId()])
					pos.setPosition(starwar::_in_star, star::mainstar[iter->second.getCountryId()], _systemInfo.getSystemTime(_unity_time_index));
				sup = 100;
				base_sup = 0;
			}

			if(pos.getType() == starwar::_in_star)
			{
				starPtr ptr = _starList.getStar(pos.getId());
				ptr->load(iter->second.getPlayerId(), iter->second.getName(), iter->second.getFaceId(), pos.getTime(), sup, base_sup);
			}
			else if(pos.getType() == starwar::_in_path)
			{
				iter->second.alterSupply(sup);
				_loadHelperMap.insert(make_pair(iter->second.getPlayerId(), loadHelper(pos, iter->second)));
				_trigger.push(triggerItem(starwar::_load_player, iter->second.getPlayerId(), pos.getTime()));
			}

			unsigned next_term_time = objs[i][starwar_next_term_time_field_str].Int();
			if(now >= next_term_time)
				continue;
			
			int exploit = objs[i][starwar_exploit_field_str].Int();
			if(exploit <= 0)
				continue;

			unsigned first_time = objs[i][starwar_first_time_field_str].eoo() ? 0 : objs[i][starwar_first_time_field_str].Int();
			_rankList[iter->second.getCountryId()].load(player_id, iter->second.getName(), iter->second.getLv(), exploit, first_time);
		}
	}

	void starwar_system::loadDB()
	{
		loadSystemInfoDB();
		loadStarDB();
		playerMap player_map;
		loadPlayerInfo(player_map);
		loadStarwarInfo(player_map);
		loadRankingDB();
	}

	Json::Value starwar_system::getStarBuff(playerDataPtr d)
	{
		Json::Value info = Json::nullValue;
		starwar::position position = d->Starwar.getPosition();
		int star_id = position.getId();
		if (position.getType() == _in_path)
			star_id = position.getFromStarId();

		starPtr ptr = _starList.getStar(star_id);
		if (ptr != starPtr())
			ptr->getBuffValue(info, d->Base.getSphereID());
		return info;
	}

	Json::Value starwar_system::getOutputBuff(playerDataPtr d)
	{
		Json::Value info = Json::arrayValue;
		info.append(_index_star_gold);
		info.append(_starList.getOutputBuff(d->Base.getSphereID()));
		return info;
	}

	void starwar_system::checkAndUpdate(boost::system_time& tmp)
	{
		if (!_busy)
		{
			unsigned now = na::time_helper::get_current_time();
			tick(now);
			run();
			if (!_busy)
				_trigger.check(now);
		}
	}

	int starwar_system::handlePathEvent(const triggerItem& e)
	{
		pathPtr path_ptr = _pathList.getPath(e.getId());
		if (path_ptr == pathPtr())
			return _server_error;

		int result = path_ptr->handleEventReq();
		if (result == _success)
			save();

		return result;
	}

	void starwar_system::handleEvent(const triggerItem& e)
	{
		_busy = true;
		int result = _server_error;
		switch (e.getCode())
		{
			case starwar::_meet:
			case starwar::_arrive:
			case starwar::_attack:
				result = handlePathEvent(e);
				break;
			case starwar::_war_time:
			case starwar::_truce_time:
				result = tickWarState(e);
				break;
			case starwar::_prime_time:
			case starwar::_ordinary_time:
				result = tickPrimeState(e);
				break;
			case starwar::_resource:
				result = tickResource(e.getTime());
				break;
			case starwar::_ranking:
				result = tickRanking(e.getTime());
				break;
			case starwar::_unity:
				result = tickUnity(e);
				break;
			case starwar::_prime_time_coming:
			case starwar::_prime_time_begin:
				result = tickBroadcast(e);
				break;
			case starwar::_load_player:
				result = tickLoadPlayer(e);
				break;
			case starwar::_prime_time_running:
				result = tickPrimeTimeBroadcast(e);
				break;
			case starwar::_prime_time_summary:
				result = tickPrimeTimeSummaryBroadcast(e);
				break;
			case starwar::_clear_sign_info:
				result = tickClearSignInfo(e);
				break;
			case starwar::_test_bug:
				result = tickTestBug(e);
				break;
			default:
				break;
		}
		if (result == starwar::_async_throw)
		{
			_async = true;
			return;
		}
		_busy = false;
	}

	void starwar_system::save()
	{
		for (set<saveHelper*>::iterator iter = _save_helper.begin(); iter != _save_helper.end(); ++iter)
			(*iter)->save();
		_save_helper.clear();
	}

	void starwar_system::callBack(int type, int id, unsigned time, bool enter_star)
	{
		_async = false;
		_busy = false;
		save();
		for (set<int>::iterator iter = _cachePlayer.begin(); iter != _cachePlayer.end(); ++iter)
		{
			playerDataPtr d = player_mgr.getPlayerMain(*iter);
			if (d != playerDataPtr())
			{
				starwar::reportPtr describe = repFactory::create(type, time, id);
				starwar::reportPtr ptr = repFactory::create(describe, d->Starwar.getCacheReport());
				bool update = !(enter_star && d->Starwar.toAddCache());
				d->Starwar.addReport(ptr, update);
			}
		}
		_cachePlayer.clear();

		for (set<int>::iterator iter = _ownerPlayer.begin(); iter != _ownerPlayer.end(); ++iter)
		{
			Json::Value msg;
			msg[msgStr].append(*iter);
			msg[msgStr].append(id);
			msg[msgStr].append(time);
			msg[msgStr].append(_targetName);
			string str = msg.toStyledString();
			na::msg::msg_json m(gate_client::starwar_notice_npc_battle_inner_req, str);
			player_mgr.postMessage(m);
		}
		_ownerPlayer.clear();

		//run();
	}

	void starwar_system::moveReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		if (!_war_state)
			Return(r, starwar::_not_war_time);

		ReadJsonArray;
		int type = js_msg[1u].asInt();

		if (type < starwar::_raid || type > starwar::_auto_transfer)
			Return(r, starwar::_client_error);

		_mQueue.push_back(message(type, js_msg, na::time_helper::get_current_time(), d));
		run();
	}

	void starwar_system::sendBack(unsigned now, int player_id, const string& name, int face_id, int country_id, bool update)
	{
		starPtr ptr = _starList.getStar(star::mainstar[country_id]);
		if (ptr != starPtr())
		{
			ptr->load(player_id, name, face_id, now, 0, _starList.getStar(star::mainstar[country_id])->getSupply(now, country_id));
			if(update)
				postToUpdatePosition(player_id, starwar::_in_star, star::mainstar[country_id], now);
		}
	}

	void starwar_system::postToUpdatePosition(int player_id, int type, int id, unsigned now, bool next)
	{
		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(player_id);
		msg[msgStr].append(type);
		msg[msgStr].append(id);
		msg[msgStr].append(now);
		msg[msgStr].append(next);
		string str = msg.toStyledString();
		na::msg::msg_json m(gate_client::starwar_update_info_inner_req, str);
		player_mgr.postMessage(m);
	}

	void starwar_system::reset(unsigned now)
	{
		_starList.reset(now);
		_pathList.reset(now);
		tickSupplyRate(now);
		save();
	}

	void starwar_system::run()
	{
		if (_busy)
			return;

		_busy = true;
		while (!_mQueue.empty())
		{
			message m = _mQueue.front();
			_mQueue.pop_front();
			if (handleMessage(m) == _async_throw)
			{
				_async = true;
				return;
			}
		}
		_busy = false;
	}

	int starwar_system::autoMove(const message& m)
	{
		playerDataPtr d = m.getPlayerPtr();
		if (d == playerDataPtr())
			return starwar::_over_time;

		const Json::Value& args = m.getArgs();
		unsigned now = m.getTime();
		int to_star_id = args[0u].asInt();

		const starwar::position& position = d->Starwar.getPosition();
		if (position.getType() == -1)
			return starwar::_client_error;

		if (position.getType() == _in_path)
			return starwar::_not_in_star;

		if (position.getId() == star::mainstar[d->Base.getSphereID()]
			&& d->Starwar.getSupply(now) < (int)_param.getMaxSupply())
			return starwar::_in_cd;

		/*if (!position.empty())
			return starwar::_client_error;*/

		if (d->Starwar.inBattleCd(now))
			return starwar::_in_cd;

		int star_id = position.getId();
		starPtr star_ptr = _starList.getStar(star_id);
		if (star_ptr == starPtr())
			return starwar::_server_error;

		const starwar::pathInfo& path_info = _shortestPath.getPathInfo(star_id, to_star_id);
		if (path_info.isNull())
			return starwar::_not_access;

		d->Starwar.getPosition().setPath(path_info.getPassStarList());
		d->Starwar.getPosition().popPath();

		int first_star_id = d->Starwar.getPosition().frontPath();
		Json::Value new_args;
		new_args.append(first_star_id);
		starwar::message new_m(starwar::_move, new_args, now, d);

		return move(new_m);
	}

	int starwar_system::move(const message& m)
	{
		playerDataPtr d = m.getPlayerPtr();
		if (d == playerDataPtr())
			return starwar::_over_time;

		const Json::Value& args = m.getArgs();
		unsigned now = m.getTime();
		int to_star_id = args[0u].asInt();
		
		const starwar::position& position = d->Starwar.getPosition();
		if (position.getType() == -1)
			return starwar::_client_error;

		if (position.getType() == _in_path)
			return starwar::_not_in_star;

		if (position.getId() == star::mainstar[d->Base.getSphereID()] 
			&& d->Starwar.getSupply(now) < (int)_param.getMaxSupply())
			return starwar::_in_cd;

		int star_id = position.getId();
		starPtr star_ptr = _starList.getStar(star_id);
		if (star_ptr == starPtr())
			return starwar::_server_error;

		pathPtr path_ptr = star_ptr->getPath(to_star_id);
		if (path_ptr == pathPtr() || !path_ptr->isAccess(star_id, to_star_id))
		{
			d->Starwar.getPosition().clearPath();
			return starwar::_not_access;
		}

		if (!star_ptr->checkGuildList(d))
			return starwar::_not_in_star;

		return star_ptr->doLeave(_move, now, d, to_star_id);
	}

	int starwar_system::transfer(const message& m)
	{
		playerDataPtr d = m.getPlayerPtr();
		if (d == playerDataPtr())
			return starwar::_over_time;

		const Json::Value& args = m.getArgs();
		unsigned now = m.getTime();
		int to_star_id = args[0u].asInt();

		const starwar::position& position = d->Starwar.getPosition();
		if (position.getType() == -1)
			return _client_error;

		if (position.getId() == star::mainstar[d->Base.getSphereID()]
			&& d->Starwar.getSupply(now) < (int)_param.getMaxSupply())
			return starwar::_in_cd;
		
		if (position.getType() == starwar::_in_star)
		{
			int star_id = position.getId();
			starPtr star_ptr = _starList.getStar(star_id);
			if (star_ptr == starPtr())
				return starwar::_server_error;

			pathPtr path_ptr = star_ptr->getPath(to_star_id);
			if (path_ptr == pathPtr() || !path_ptr->isAccess(star_id, to_star_id))
				return starwar::_not_access;

			if (!star_ptr->checkGuildList(d))
				return starwar::_not_in_star;
			
			d->Starwar.calculateSupply(now);
			return star_ptr->doLeave(_transfer, now, d, to_star_id);
		}
		else
		{
			int path_id = position.getId();
			int from_star_id = position.getFromStarId();
			pathPtr path_ptr = _pathList.getPath(path_id);
			if (path_ptr == pathPtr() || !path_ptr->isAccess(from_star_id, to_star_id))
				return starwar::_not_access;

			return path_ptr->doTransfer(now, d, to_star_id, true);
		}
	}

	int starwar_system::raid(const message& m)
	{
		playerDataPtr d = m.getPlayerPtr();
		if (d == playerDataPtr())
			return starwar::_over_time;

		const Json::Value& args = m.getArgs();
		unsigned now = m.getTime();
		int to_star_id = args[0u].asInt();

		const starwar::position& position = d->Starwar.getPosition();
		if (position.getType() == -1)
			return starwar::_client_error;

		if (position.getType() == _in_path)
			return starwar::_not_in_star;

		if (position.getId() == star::mainstar[d->Base.getSphereID()]
			&& d->Starwar.getSupply(now) < (int)_param.getMaxSupply())
			return starwar::_in_cd;

		int star_id = position.getId();
		starPtr star_ptr = _starList.getStar(star_id);
		if (star_ptr == starPtr())
			return starwar::_server_error;

		pathPtr path_ptr = star_ptr->getPath(to_star_id);
		if (path_ptr == pathPtr() || !path_ptr->isRaid(star_id, to_star_id))
			return starwar::_not_access;

		if (!star_ptr->checkGuildList(d))
			return starwar::_not_in_star;

		if (_starList.getStar(to_star_id)->getCountryId() != d->Base.getSphereID())
		{
			int result = d->Starwar.isRaidLimit(now);
			if (result != starwar::_success)
				return result;
			d->Starwar.doRaid(now);
		}

		d->Starwar.getPosition().setPath(vector<int>(1, to_star_id));
		return star_ptr->doLeave(_raid, now, d, to_star_id);
	}

	int starwar_system::kingGather(const message& m)
	{
		playerDataPtr d = m.getPlayerPtr();
		if (d == playerDataPtr())
			return starwar::_over_time;

		const Json::Value& args = m.getArgs();
		unsigned now = m.getTime();
		int to_star_id = args[0u].asInt();

		if (!_prime_state)
			return starwar::_not_prime_time;

		if (d->Ruler.getTitle() != _title_king)
			return starwar::_not_king;
		
		const starwar::position& position = d->Starwar.getPosition();
		if (position.getType() != _in_star)
			return starwar::_client_error;

		if (position.getType() == _in_star && position.getId() != to_star_id)
			return starwar::_not_in_star;

		starPtr ptr = _starList.getStar(to_star_id);
		if (ptr == starPtr())
			return starwar::_client_error;

		int result = d->Starwar.gather(now);
		if (result != starwar::_success)
			return result;

		_systemInfo.resetGatherInfo(d->Base.getSphereID(), now, to_star_id);
		noticeKingGather(d->Base.getSphereID());
		gatherBroadcast(d, to_star_id);
		return starwar::_success;
	}

	int starwar_system::officerGather(const message& m)
	{
		playerDataPtr d = m.getPlayerPtr();
		if (d == playerDataPtr())
			return starwar::_over_time;

		const Json::Value& args = m.getArgs();
		unsigned now = m.getTime();
		bool state = args[0u].asBool();

		if (!_systemInfo.isGather(d))
			return starwar::_not_gather;

		_systemInfo.kickPlayer(d);

		if (!state)
			return starwar::_success;

		int to_star_id = _systemInfo.getGatherInfo(d->Base.getSphereID()).getToStarId();
		const starwar::position& position = d->Starwar.getPosition();
		if (position.getType() == _in_path)
		{
			pathPtr ptr = _pathList.getPath(position.getId());
			if (ptr == pathPtr())
				return starwar::_server_error;
			ptr->kickPlayer(d->playerID);
		}
		else
		{
			starPtr ptr = _starList.getStar(position.getId());
			if (ptr == starPtr())
				return starwar::_server_error;
			ptr->kickPlayer(d->playerID, d->Starwar.getPosition().getTime());
		}

		starPtr ptr = _starList.getStar(to_star_id);
		if (ptr == starPtr())
			return starwar::_server_error;

		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(_raid + 1));
		fields.push_back(boost::lexical_cast<string, int>(d->Starwar.getPosition().getFromStarId()));
		fields.push_back(boost::lexical_cast<string, int>(to_star_id));
		StreamLog::Log(starwarLogDBStr, d, "", "", fields, playerStarwar::_log_move);

		d->Starwar.getPosition().clearPath();
		starwar::playerInfo player_info(d, now);
		return ptr->doEnter(starwar::_officer_gather, now, player_info, -1);
	}

	int starwar_system::useItem(const message& m)
	{
		playerDataPtr d = m.getPlayerPtr();
		if (d == playerDataPtr())
			return starwar::_over_time;

		const Json::Value& args = m.getArgs();
		if (args.size() == 4)
		{
			if (args[3u].asInt() == starwar::_player)
			{
				playerDataPtr targetP = player_mgr.getPlayerMain(args[2u].asInt());
				if (targetP == playerDataPtr())
					return starwar::_over_time;
				return d->Starwar.useItem(args[0u].asInt(), args[1u].asInt(), args[2u].asInt(), targetP);
			}
			else
				return d->Starwar.useItem(args[0u].asInt(), args[1u].asInt(), args[2u].asInt());
		}

		return d->Starwar.useItem(args[0u].asInt());
	}

	int starwar_system::handleMessage(const message& m)
	{
		int result = _server_error;
		NumberCounter::Step();
		switch (m.getType())
		{
			case starwar::_move:
				State::setState(gate_client::starwar_move_req);
				result = move(m);
				break;
			case starwar::_transfer:
				State::setState(gate_client::starwar_move_req);
				result = transfer(m);
				break;
			case starwar::_raid:
				State::setState(gate_client::starwar_move_req);
				result = raid(m);
				break;
			case starwar::_auto_move:
				State::setState(gate_client::starwar_move_req);
				result = autoMove(m);
				break;
			case starwar::_auto_transfer:
				State::setState(gate_client::starwar_move_req);
				result = autoTransfer(m);
				break;
			case starwar::_king_gather:
				State::setState(gate_client::starwar_king_gather_req);
				result = kingGather(m);
				break;
			case starwar::_officer_gather:
				State::setState(gate_client::starwar_officer_gather_req);
				result = officerGather(m);
				break;
			case starwar::_use_item:
				State::setState(gate_client::starwar_use_item_req);
				result = useItem(m);
				break;
			case starwar::_settle_npc:
				State::setState(gate_client::starwar_settle_npc_req);
				result = settleNpc(m);
				break;
			default:
				break;
		}

		response(m.getPlayerPtr()->playerID, m.getType(), result);

		if (result == _success)
			save();

		helper_mgr.runSaveAndUpdate();
		return result;
	}

	void starwar_system::reportListReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		Json::Value msg;
		msg[msgStr][0u] = 0;
		d->Starwar.packageReport(msg[msgStr][1u]);
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_report_list_resp, msg);
	}

	void starwar_system::playerInfoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		d->Starwar.update();
	}

	void starwar_system::playerRankReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		Json::Value msg;
		msg[msgStr][0u] = 0;
		_rankList[d->Base.getSphereID()].getValue(msg[msgStr][1u], d);
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_player_rank_resp, msg);
	}

	void starwar_system::countryRankReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		Json::Value msg;
		msg[msgStr][0u] = 0;
		_occupyInfo.getValue(msg[msgStr][1u][starwar_star_num_field_str]);
		ruler_sys.getValue(msg[msgStr][1u][starwar_king_info_field_str]);
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_country_rank_resp, msg);
	}

	void starwar_system::kickReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		kickPlayer(d);
		Return(r, _success);
	}

	void starwar_system::rewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		ReadJsonArray;
		int star_id = js_msg[0u].asInt();
		int get_num = 0;

		if ((get_num = d->Starwar.getResource(star_id)) <= 0)
			Return(r, starwar::_no_reward);

		r[msgStr][1u] = get_num;
		Return(r, 0);
	}

	void starwar_system::starInfoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		ReadJsonArray;
		int star_id = js_msg[0u].asInt();
		int begin = js_msg[1u].asInt();
		int count = js_msg[2u].asInt();

		if (begin <= 0 || count <= 0)
			Return(r, _client_error);

		updateStarInfo(d, star_id, begin, count);
	}

	void starwar_system::useItemReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		playerDataPtr d = player_mgr.getPlayerMain(m._player_id);

		vector<playerDataPtr> player_vec;
		player_vec.push_back(d);
		if (js_msg.size() == 4 && js_msg[3u].asInt() == starwar::_player)
			player_vec.push_back(player_mgr.getPlayerMain(js_msg[2u].asInt()));

		for (vector<playerDataPtr>::iterator iter = player_vec.begin(); iter != player_vec.end(); ++iter)
		{
			if (*iter == playerDataPtr())
			{
				if (m._post_times < 1)
					player_mgr.postMessage(m);
				return;
			}
		}
		CheckCountryId();		

		if (!_war_state)
			Return(r, starwar::_not_war_time);

		_mQueue.push_back(message(starwar::_use_item, js_msg, na::time_helper::get_current_time(), d));
		run();
	}

	void starwar_system::openItemReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		int result = d->Starwar.openItem();
		Return(r, result);
	}

	void starwar_system::flushItemReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		int result = d->Starwar.flushItem();
		Return(r, result);
	}

	void starwar_system::buyItemReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		ReadJsonArray;
		int position = js_msg[0u].asInt();
		int result = d->Starwar.buyItem(position);
		Return(r, result);
	}

	void starwar_system::kingGatherReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();
		if (!_war_state)
			Return(r, _not_war_time);

		ReadJsonArray;
		unsigned now = na::time_helper::get_current_time();
		message msg(_king_gather, js_msg, now, d);
		_mQueue.push_back(msg);
		run();
	}

	void starwar_system::officerGatherReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		ReadJsonArray;
		unsigned now = na::time_helper::get_current_time();
		message msg(_officer_gather, js_msg, now, d);
		_mQueue.push_back(msg);
		run();
	}

	void starwar_system::greatEventReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = _systemInfo.getGreatEventValue();
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_great_event_resp, msg);
	}

	void starwar_system::updateSignInfo(int country_id)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][starwar_type_field_str] = 2;
		_systemInfo.getSignInfo(country_id, msg[msgStr][1u][starwar_sign_info_list_field_str]);

		for (set<basePlayer>::iterator iter = _playerList.begin(); iter != _playerList.end(); ++iter)
		{
			if (iter->getCountryId() != country_id)
				continue;
			playerDataPtr d = player_mgr.getPlayerMain(iter->getPlayerId());
			if (d != playerDataPtr())
				player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_notice_unity_reward_resp, msg);
		}
	}

	void starwar_system::updateSignInfo(playerDataPtr d)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][starwar_type_field_str] = 2;
		_systemInfo.getSignInfo(d->Base.getSphereID(), msg[msgStr][1u][starwar_sign_info_list_field_str]);
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_notice_unity_reward_resp, msg);
	}

	double starwar_system::getRateBySup(int sup, bool type)
	{
		if (sup <= 30)
			return -0.1;
		if (sup <= 40)
			return type ? -0.1 : - 0.05;
		if (sup <= 60)
			return -0.05;
		if (sup <= 80)
			return type ? -0.05 : 0.0;
		return 0.0;
	}

	void starwar_system::adjustRate(double& rate, bool type)
	{
		if (type)
		{
			if (rate < -1.0)
				rate = -1.0;
		}
		else
		{
			if (rate > 1.0)
				rate = 1.0;
		}
	}

	inline void starwar_system::updateBattleTask(bool result, playerDataPtr d, playerDataPtr targetP)
	{
		if (result)
		{
			d->Starwar.updateTask(starwar::_win_times);
			d->Starwar.updateTask(starwar::_win_streak, 1);
			if (targetP != playerDataPtr() && targetP->Ruler.getTitle() == _title_king)
			{
				d->Starwar.updateTask(starwar::_kill_king);
				killKingBroadcast(d, targetP);
			}
		}
		else
		{
			d->Starwar.updateTask(starwar::_lose_times);
			d->Starwar.updateTask(starwar::_win_streak, 2);
		}
	}

	inline int starwar_system::getBattleSilver(bool result)
	{
		return result ? starwar::_win_silver : starwar::_lose_silver;
	}

	inline int starwar_system::getBattleCost(bool result, unsigned now, int position, battleSidePtr ptr, playerDataPtr d, playerDataPtr targetP)
	{
		if (result)
		{
			int cost = _param.getSupplyCost(ptr->getStar() - 1, position);
			if (d->Base.getLevel() - targetP->Base.getLevel() > 25 || targetP->Base.getLevel() - d->Base.getLevel() > 25)
				cost /= 2;
			return cost;
		}
		else
			return d->Starwar.getSupply(now);
	}

	void starwar_system::updateStarInfo(playerDataPtr d, int star_id, int begin, int count)
	{
		starPtr star_ptr = _starList.getStar(star_id);
		if (star_ptr == starPtr())
			return;

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = star_ptr->getInfo(begin, count);
		msg[msgStr][1u]["nu"] = d->Starwar.getSettleNpcNum();
		if (d->Starwar.getPosition().getType() == starwar::_in_star)
		{
			int from_id = d->Starwar.getPosition().getId();
			if (from_id != star_id)
			{
				const starwar::pathInfo& path_info = _shortestPath.getPathInfo(from_id, star_id);
				if (!path_info.isNull())
				{
					msg[msgStr][1u]["tm"] = path_info.getTime() / getMoveRate();
					msg[msgStr][1u]["tt"] = getTransferTimes(path_info.getPassStarNum());
					msg[msgStr][1u]["p"] = Json::arrayValue;
					for (vector<int>::const_reverse_iterator iter = path_info.getPassStarList().rbegin(); iter != path_info.getPassStarList().rend(); ++iter)
						msg[msgStr][1u]["p"].append(*iter);
				}
			}
		}
		else
		{
			int from_id = d->Starwar.getPosition().getFromStarId();
			if (from_id != star_id)
			{
				const starwar::pathInfo& path_info = _shortestPath.getPathInfo(from_id, star_id);
				if (!path_info.isNull())
				{
					msg[msgStr][1u]["tt"] = getTransferTimes(path_info.getPassStarNum());
					msg[msgStr][1u]["p"] = Json::arrayValue;
					for (vector<int>::const_reverse_iterator iter = path_info.getPassStarList().rbegin(); iter != path_info.getPassStarList().rend(); ++iter)
						msg[msgStr][1u]["p"].append(*iter);
				}
			}
		}
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_star_info_resp, msg);
	}

	int starwar_system::getCostTime(int from_id, int to_id)
	{
		starPtr star_ptr = _starList.getStar(from_id);
		pathPtr path_ptr = star_ptr->getPath(to_id);
		return path_ptr->getCostTime(from_id, to_id);
	}

	void starwar_system::signStarReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		ReadJsonArray;
		int star_id = js_msg[0u].asInt();
		bool state = js_msg[1u].asBool();

		if (d->Ruler.getTitle() != _title_king)
			Return(r, starwar::_not_king);

		starPtr ptr = _starList.getStar(star_id);
		if (ptr == starPtr())
			Return(r, starwar::_client_error);

		if (!_systemInfo.setSignInfo(d->Base.getSphereID(), star_id, state))
			Return(r, starwar::_client_error);

		updateSignInfo(d->Base.getSphereID());

		Return(r, starwar::_success);
	}

	void starwar_system::getShortestPathReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		ReadJsonArray;
		int from_id = js_msg[0u].asInt();
		int to_id = js_msg[1u].asInt();
		Json::Value msg;
		msg[msgStr][0u] = 0;
		const starwar::pathInfo& path_info = _shortestPath.getPathInfo(from_id, to_id);
		if (path_info.isNull())
			msg[msgStr][0u] = 1;
		else
		{
			for (mPath::const_iterator iter = path_info.getPassStarList().begin(); iter != path_info.getPassStarList().end(); ++iter)
				msg[msgStr][1u].append(*iter);
		}

		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_get_shortest_path_resp, msg);
	}

	void starwar_system::settleNpcReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		if (!_war_state)
			Return(r, starwar::_not_war_time);

		ReadJsonArray;
		_mQueue.push_back(message(starwar::_settle_npc, js_msg, na::time_helper::get_current_time(), d));
		run();
	}

	void starwar_system::response(int player_id, int type, int result)
	{
		if (result == _async_throw)
			result = _success;

		Json::Value msg;
		msg[msgStr][0u] = result;
		int protocol = -1;
		switch (type)
		{
			case _raid:
			case _auto_move:
			case _auto_transfer:
				protocol = gate_client::starwar_move_resp;
				break;
			case _king_gather:
				protocol = gate_client::starwar_king_gather_resp;
				break;
			case _officer_gather:
				protocol = gate_client::starwar_officer_gather_resp;
				break;
			case _use_item:
				protocol = gate_client::starwar_use_item_resp;
				break;
			case _settle_npc:
				protocol = gate_client::starwar_settle_npc_resp;
				break;
			default:
				break;
		}
		player_mgr.sendToPlayer(player_id, protocol, msg);
	}

	int starwar_system::settleNpc(const message& m)
	{
		playerDataPtr d = m.getPlayerPtr();
		if (d == playerDataPtr())
			return starwar::_over_time;

		const Json::Value& args = m.getArgs();
		int star_id = args[0u].asInt();
		if (d->Starwar.getPosition().getType() != starwar::_in_star || d->Starwar.getPosition().getId() != star_id)
			return starwar::_not_in_star;

		int num = args[1u].asInt();

		int result = d->Starwar.isSettleNpcLimit(num);
		if (result != starwar::_success)
			return result;

		starPtr ptr = _starList.getStar(star_id);
		if (ptr == starPtr())
			return starwar::_server_error;

		result = ptr->settlePlayerNpc(d, m.getTime(), num);

		if (result == starwar::_success)
		{
			d->Starwar.settleNpc(num);
			updateStarInfo(d, star_id);
		}

		return result;
	}

	void starwar_system::enterPlayer(playerDataPtr d)
	{
		if (!d->Starwar.onMain())
		{
			d->Starwar.onMain(true);
			_playerList.insert(basePlayer(d->playerID, d->Base.getSphereID()));
			if (_playerList.size() > _max_num)
				_max_num = _playerList.size();

			updateSignInfo(d);

			sendMainInfo(d);

			if (_systemInfo.isGather(d))
				noticeKingGather(d->Base.getSphereID(), d->playerID);
		}
	}

	void starwar_system::sendMainInfo(playerDataPtr d)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][starwar_path_info_field_str] = _pathList.isExtra() > 0? Json::arrayValue : _pathList.getPathInfo();
		msg[msgStr][1u][starwar_star_info_field_str] = _starList.getOccupyInfo();
		msg[msgStr][1u][starwar_move_rate_field_str] = starwar_sys.getMoveRate();
		msg[msgStr][1u][starwar_supply_rate_field_str] = starwar_sys.getSupplyRate();
		msg[msgStr][1u][starwar_war_state_field_str] = _war_state;
		msg[msgStr][1u][starwar_next_update_time_field_str] = _unity_state? _next_prime_time : _next_war_time;
		msg[msgStr][1u][starwar_unity_state_field_str] = _unity_state;

		if (d != playerDataPtr())
		{
			player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_main_info_resp, msg);

			for (int i = 0; i < _pathList.isExtra(); ++i)
			{
				Json::Value extraInfo;
				extraInfo[msgStr][0u] = 0;
				extraInfo[msgStr][1u][starwar_path_info_field_str] = _pathList.getExtraInfo()[i];
				extraInfo[msgStr][1u][starwar_star_info_field_str] = Json::arrayValue;
				player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_main_update_resp, extraInfo);
			}
		}
		else
		{
			string str = msg.toStyledString();
			na::msg::msg_json mj(gate_client::starwar_main_info_resp, str);
			for (set<basePlayer>::iterator iter = _playerList.begin(); iter != _playerList.end(); ++iter)
			{
				playerDataPtr d = player_mgr.getOnlinePlayer(iter->getPlayerId());
				if (d != playerDataPtr())
					player_mgr.sendToPlayer(d->playerID, d->netID, mj);
			}

			for (int i = 0; i < _pathList.isExtra(); ++i)
			{
				Json::Value extraInfo;
				extraInfo[msgStr][0u] = 0;
				extraInfo[msgStr][1u][starwar_path_info_field_str] = _pathList.getExtraInfo()[i];
				extraInfo[msgStr][1u][starwar_star_info_field_str] = Json::arrayValue;
				string str = extraInfo.toStyledString();
				na::msg::msg_json mj(gate_client::starwar_main_update_resp, str);
				for (set<basePlayer>::iterator iter = _playerList.begin(); iter != _playerList.end(); ++iter)
				{
					playerDataPtr d = player_mgr.getOnlinePlayer(iter->getPlayerId());
					if (d != playerDataPtr() && d->netID >= 0)
						player_mgr.sendToPlayer(d->playerID, d->netID, mj);
				}
			}
		}	
	}

	void starwar_system::kickPlayer(playerDataPtr d, bool offline)
	{
		if (d->Starwar.onMain() || offline)
		{
			d->Starwar.onMain(false, offline);
			_playerList.erase(basePlayer::makeKey(d->playerID));
		}
	}

	void starwar_system::noticeNpcBattleInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int player_id = js_msg[0u].asInt();
		int star_id = js_msg[1u].asInt();
		unsigned time = js_msg[2u].asInt();
		string name = js_msg[3u].asString();

		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		if (d == playerDataPtr())
		{
			if (m._post_times < 2)
				player_mgr.postMessage(m);
			return;
		}

		d->Starwar.addReport(repFactory::create(time, star_id, name), false);
	}

	void starwar_system::updateInfoInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int player_id = js_msg[0u].asInt();
		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		if (d == playerDataPtr())
		{
			if (m._post_times < 2)
				player_mgr.postMessage(m);
			return;
		}

		d->Starwar.updateData();
		int type = js_msg[1u].asInt();
		int id = js_msg[2u].asInt();
		unsigned time = js_msg[3u].asUInt();
		bool next = js_msg[4u].asBool();
		d->Starwar.setPosition(type, id, time, next);
	}

	void starwar_system::pushMessage(const message& m)
	{
		_mQueue.push_back(m);
	}

	int starwar_system::getTransferTimes(int star_num)
	{
		if (star_num < 3)
			return 1;
		if (star_num < 6)
			return 2;
		return 3;
	}

	int starwar_system::autoTransfer(const message& m)
	{
		playerDataPtr d = m.getPlayerPtr();
		if (d == playerDataPtr())
			return starwar::_over_time;

		const Json::Value& args = m.getArgs();
		unsigned now = m.getTime();
		int to_star_id = args[0u].asInt();

		const starwar::position& position = d->Starwar.getPosition();
		if (position.getType() == -1)
			return _client_error;

		if (position.getId() == star::mainstar[d->Base.getSphereID()]
			&& d->Starwar.getSupply(now) < (int)_param.getMaxSupply())
			return starwar::_in_cd;

		if (d->Starwar.inBattleCd(now))
			return starwar::_in_cd;

		if (position.getType() == starwar::_in_star)
		{
			int star_id = position.getId();
			starPtr star_ptr = _starList.getStar(star_id);
			if (star_ptr == starPtr())
				return starwar::_server_error;

			if (!star_ptr->checkGuildList(d))
				return starwar::_not_in_star;

			const starwar::pathInfo& path_info = _shortestPath.getPathInfo(star_id, to_star_id);

			if (path_info.isNull())
				return starwar::_not_access;

			int num = getTransferTimes(path_info.getPassStarNum());

			int result = d->Starwar.isTransferLimit(now, num);
			if (result != starwar::_success)
				return result;

			d->Starwar.useTransferTimes(num);

			d->Starwar.getPosition().setPath(path_info.getPassStarList());
			d->Starwar.getPosition().popPath();

			int first_star_id = d->Starwar.getPosition().frontPath();
			Json::Value new_args;
			new_args.append(first_star_id);
			starwar::message new_m(starwar::_transfer, new_args, now, d);

			return transfer(new_m);
		}
		else
		{
			if (!position.empty())
			{
				if (position.backPath() != to_star_id)
					return starwar::_not_access;
			}
			else
			{
				int from_star_id = position.getFromStarId();
				pathPtr path_ptr = _pathList.getPath(position.getId());
				if (path_ptr == pathPtr())
					return starwar::_server_error;

				if (path_ptr->getLinkedStarId(from_star_id) != to_star_id)
					return starwar::_not_access;
			}

			const starwar::pathInfo& path_info = _shortestPath.getPathInfo(position.getFromStarId(), to_star_id);

			if (path_info.isNull())
				return starwar::_not_access;

			int num = getTransferTimes(path_info.getPassStarNum());

			int result = d->Starwar.isTransferLimit(now, num);
			if (result != starwar::_success)
				return result;

			d->Starwar.useTransferTimes(num);

			d->Starwar.getPosition().setPath(path_info.getPassStarList());
			d->Starwar.getPosition().popPath();

			int first_star_id = d->Starwar.getPosition().frontPath();
			Json::Value new_args;
			new_args.append(first_star_id);
			starwar::message new_m(starwar::_transfer, new_args, now, d);

			return transfer(new_m);
		}
	}

	void starwar_system::attackWithPlayerInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int position = js_msg[0u].asInt();
		int player_id = js_msg[1u].asInt();
		double atk_rate = js_msg[2u].asDouble();
		double def_rate = js_msg[3u].asDouble();
		int target_id = js_msg[4u].asInt();
		double target_atk_rate = js_msg[5u].asDouble();
		double target_def_rate = js_msg[6u].asDouble();
		int id = js_msg[7u].asInt();
		unsigned now = js_msg[8u].asUInt();

		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		playerDataPtr targetP = player_mgr.getPlayerMain(target_id);
		if (d == playerDataPtr() || targetP == playerDataPtr())
		{
			if (m._post_times < 4)
				player_mgr.postMessage(m);
			return;
		}

		if (d->isOperatBattleWithoutTimeLimit() || targetP->isOperatBattleWithoutTimeLimit())
		{
			player_mgr.postMessage(m);
			return;
		}

		Battlefield field;
		_buff[battleParam::_index_atk].setValue(atk_rate + d->Starwar.getAtkBuff(now) + getRateBySup(d->Starwar.getSupply(now), true));
		_buff[battleParam::_index_def].setValue(def_rate + d->Starwar.getDefBuff(now) + getRateBySup(d->Starwar.getSupply(now), false));
		field.atkSide = battleSide::Create(d, boost::bind(&starwar_system::addBattleBuff, this, _1, _2));
		_buff[battleParam::_index_atk].setValue(target_atk_rate + targetP->Starwar.getAtkBuff(now) + getRateBySup(targetP->Starwar.getSupply(now), true));
		_buff[battleParam::_index_def].setValue(target_def_rate + targetP->Starwar.getDefBuff(now) + getRateBySup(targetP->Starwar.getSupply(now), false));
		field.defSide = battleSide::Create(targetP, boost::bind(&starwar_system::addBattleBuff, this, _1, _2));
		if (field.atkSide == battleSidePtr() || field.defSide == battleSidePtr())
			return;

		field.atkSide->setFileName(d->Starwar.getFileName());
		field.defSide->setFileName(targetP->Starwar.getFileName());

		field.ParamJson[BATTLE::strStarwarParam].append(position);
		field.ParamJson[BATTLE::strStarwarParam].append(id);
		field.ParamJson[BATTLE::strStarwarParam].append(now);
		field.ParamJson[BATTLE::strStarwarParam].append(d->Base.getLevel());
		field.ParamJson[BATTLE::strStarwarParam].append(targetP->Base.getLevel());
		if (position == _in_path)
			field.ParamJson[BATTLE::strStarwarParam].append(js_msg[9u].asInt());
		battle_sys.PostBattle(field, battle_starwar_with_player_type);
	}

	void starwar_system::attackWithNpcInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int position = js_msg[0u].asInt();
		int player_id = js_msg[1u].asInt();
		double atk_rate = js_msg[2u].asDouble();
		double def_rate = js_msg[3u].asDouble();
		int npc_id = js_msg[4u].asInt();
		int id = js_msg[7u].asInt();
		unsigned now = js_msg[8u].asUInt();
		int type = js_msg[9u].asInt();
		int supply = js_msg[10u].asInt();
		int title = js_msg[11u].asInt();
		int owner_id = js_msg[12u].asInt();

		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		if (d == playerDataPtr())
		{
			if (m._post_times < 4)
				player_mgr.postMessage(m);
			return;
		}

		if (d->isOperatBattleWithoutTimeLimit())
		{
			player_mgr.postMessage(m);
			return;
		}

		int index = type - 1;
		if (index >= (int)_npcMap.size())
			return;

		npcMap::const_iterator it = _npcMap[index].find(npc_id);
		if (it == _npcMap[index].end())
			return;


		mapDataPtr mD = it->second;
		Battlefield field;
		_buff[battleParam::_index_atk].setValue(atk_rate + d->Starwar.getAtkBuff(now) + getRateBySup(d->Starwar.getSupply(now), true));
		_buff[battleParam::_index_def].setValue(def_rate + d->Starwar.getDefBuff(now) + getRateBySup(d->Starwar.getSupply(now), false));
		field.atkSide = battleSide::Create(d, boost::bind(&starwar_system::addBattleBuff, this, _1, _2));
		field.defSide = battleSide::Create(mD);
		if (field.atkSide == battleSidePtr() || field.defSide == battleSidePtr())
			return;

		field.atkSide->setFileName(d->Starwar.getFileName());
		field.ParamJson[BATTLE::strStarwarParam].append(position);
		field.ParamJson[BATTLE::strStarwarParam].append(id);
		field.ParamJson[BATTLE::strStarwarParam].append(now);
		field.ParamJson[BATTLE::strStarwarParam].append(type);
		field.ParamJson[BATTLE::strStarwarParam].append(supply);
		field.ParamJson[BATTLE::strStarwarParam].append(mD->frontLocalID);
		field.ParamJson[BATTLE::strStarwarParam].append(d->Base.getLevel());
		field.ParamJson[BATTLE::strStarwarParam].append(title);
		field.ParamJson[BATTLE::strStarwarParam].append(owner_id);
		battle_sys.PostBattle(field, battle_starwar_with_npc_type);
	}

	void starwar_system::taskUpdateInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int player_id = js_msg[0u].asInt();
		int star_id = js_msg[1u].asInt();

		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		if (d == playerDataPtr())
		{
			if (m._post_times < 2)
				player_mgr.postMessage(m);
			return;
		}

		d->Starwar.updateData();
		d->Starwar.updateTask(starwar::_occupy_specified, star_id + 1);
		d->Starwar.updateTask(starwar::_occupy_times, star_id + 1);
	}

	void starwar_system::attackWithNpcInnerResp(const int battleResult, Battlefield field)
	{
		State::setState(gate_client::starwar_attack_with_npc_inner_req);
		NumberCounter::Step();

		int player_id = field.atkSide->getPlayerID();
		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		if (d == playerDataPtr())
			return;

		d->Starwar.updateData();
		_cachePlayer.insert(player_id);

		int position = field.ParamJson[BATTLE::strStarwarParam][0u].asInt();
		int id = field.ParamJson[BATTLE::strStarwarParam][1u].asInt();
		unsigned now = field.ParamJson[BATTLE::strStarwarParam][2u].asUInt();
		int type = field.ParamJson[BATTLE::strStarwarParam][3u].asInt();
		int supply = field.ParamJson[BATTLE::strStarwarParam][4u].asInt();
		int title = field.ParamJson[BATTLE::strStarwarParam][7u].asInt();
		int owner_id = field.ParamJson[BATTLE::strStarwarParam][8u].asInt();

		if (type == 4)
		{
			_ownerPlayer.insert(owner_id);
			_targetName = d->Base.getName();
		}
		
		int index = type - 1;
		if (index >= (int)_npcMap.size())
			return;

		npcMap::const_iterator it = _npcMap[index].find(field.defSide->getSideID());
		if (it == _npcMap[index].end())
			return;

		// exploit
		bool result = battleResult == 1;
		int exploit = result ? it->second->frontLocalID : 0;
		_rankList[d->Base.getSphereID()].update(d, d->Starwar.alterExploit(exploit));

		d->Starwar.addBattleTimes();

		// task
		updateBattleTask(result, d);

		// silver
		int silver = getBattleSilver(result);
		d->Base.alterSilver(silver);

		// sup
		int cost = result ? _param.getSupplyCost(field.atkSide->getStar() - 1, position) : d->Starwar.getSupply(now);
		int target_cost = !result ? _param.getSupplyCost(field.defSide->getStar() - 1, position) : supply;
		d->Starwar.alterSupply(0 - cost, now);

		// rep
		starwar::reportPtr rep = repFactory::create(battleResult, field.atkSide->getFileName(), it->second->mapName, exploit, cost, target_cost, silver, true, title);
		d->Starwar.addCacheReport(rep);

		supply -= target_cost;
		if (supply < 0)
			supply = 0;

		switch (position)
		{
			case _in_path:
				break;
			case _in_star:
			{
				starPtr ptr = _starList.getStar(id);
				if (ptr != starPtr())
					ptr->attackResp(d->Starwar.getSupply(now), supply);
				break;
			}
			default:
				break;
		}
		helper_mgr.runSaveAndUpdate();
	}

	void starwar_system::attackWithPlayerInnerResp(const int battleResult, Battlefield field)
	{
		State::setState(gate_client::starwar_attack_with_player_inner_req);
		NumberCounter::Step();

		int player_id = field.atkSide->getPlayerID();
		int target_id = field.defSide->getPlayerID();
		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		playerDataPtr targetP = player_mgr.getPlayerMain(target_id);
		if (d == playerDataPtr() || targetP == playerDataPtr())
			return;

		d->Starwar.updateData();
		targetP->Starwar.updateData();
		_cachePlayer.insert(player_id);

		int position = field.ParamJson[BATTLE::strStarwarParam][0u].asInt();
		int id = field.ParamJson[BATTLE::strStarwarParam][1u].asInt();
		unsigned now = field.ParamJson[BATTLE::strStarwarParam][2u].asUInt();

		// exploit
		bool result = battleResult == 1;
		int exploit = _param.getExploit(targetP->Base.getLevel(), result);
		_rankList[d->Base.getSphereID()].update(d, d->Starwar.alterExploit(exploit));
		int target_exploit = _param.getExploit(d->Base.getLevel(), !result);
		_rankList[targetP->Base.getSphereID()].update(targetP, targetP->Starwar.alterExploit(target_exploit));

		d->Starwar.addBattleTimes();
		targetP->Starwar.addBattleTimes();

		// task
		updateBattleTask(result, d, targetP);
		updateBattleTask(!result, targetP, d);

		// silver
		int silver = getBattleSilver(result);
		int target_silver = getBattleSilver(!result);
		d->Base.alterSilver(silver);
		targetP->Base.alterSilver(target_silver);

		// sup
		int cost = getBattleCost(result, now, position, field.atkSide, d, targetP);
		int target_cost = getBattleCost(!result, now, position, field.defSide, targetP, d);
		d->Starwar.alterSupply(0 - cost, now);
		targetP->Starwar.alterSupply(0 - target_cost, now);

		// rep
		starwar::reportPtr describe = repFactory::create(starwar::_report_0, now, d->Base.getSphereID());
		starwar::reportPtr target_rep = repFactory::create(result ? 2 : 1, field.defSide->getFileName(), d->Base.getName(), target_exploit, target_cost, cost, target_silver, false);
		starwar::reportPtr target_ptr = repFactory::create(describe, reportList(1, target_rep));
		targetP->Starwar.addReport(target_ptr);

		starwar::reportPtr ptr = repFactory::create(battleResult, field.atkSide->getFileName(), targetP->Base.getName(), exploit, cost, target_cost, silver, false);
		d->Starwar.addCacheReport(ptr);

		switch (position)
		{
			case _in_path:
			{
				int type = field.ParamJson[BATTLE::strStarwarParam][5u].asInt();
				pathPtr ptr = _pathList.getPath(id);
				if (ptr != pathPtr())
					ptr->attackResp(type, d->Starwar.getSupply(now), targetP->Starwar.getSupply(now));
				break;
			}
			case _in_star:
			{
				starPtr ptr = _starList.getStar(id);
				if (ptr != starPtr())
					ptr->attackResp(d->Starwar.getSupply(now), targetP->Starwar.getSupply(now));
				break;
			}
		}
		helper_mgr.runSaveAndUpdate();
	}

	void starwar_system::noticeOccupy(int player_id, int star_id)
	{
		playerDataPtr d = player_mgr.getOnlinePlayer(player_id);
		if (d == playerDataPtr() || !d->Starwar.onMain())
			return;

		if (d->Starwar.toAddCache())
			return;

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = star_id;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::starwar_notice_occupy_resp, msg);
	}

	void starwar_system::getTaskList(vector<taskRecord>& task_list, playerDataPtr d)
	{
		unsigned i = task_list.size();
		for (; i < _task_num; ++i)
		{
			do
			{
				int task_id = commom_sys.randomList(_taskProbs) + 1;
				bool flag = false;
				for (vector<taskRecord>::iterator iter = task_list.begin(); iter != task_list.end(); ++iter)
				{
					if ((task_id - 1) / 3 == (iter->getId() - 1) / 3)
					{
						flag = true;
						break;
					}
				}
				if (!flag)
				{
					starwarTaskList::iterator iter = _taskList.find(task_id);
					if (iter != _taskList.end())
					{
						taskRecord record;
						record.setValue(iter->second, d);
						task_list.push_back(record);
						break;
					}
				}
			} 
			while (true);
		}
	}

	bool starwar_system::checkNpcConfig(int type, int npc_id)
	{
		int index = type - 1;
		return _npcMap[index].find(npc_id) != _npcMap[index].end();
	}

	void starwar_system::postToUpdateTask(int player_id, int type, int temp /* = -1 */)
	{
		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(player_id);
		msg[msgStr].append(type);
		msg[msgStr].append(temp);
		string str = msg.toStyledString();
		na::msg::msg_json m(gate_client::starwar_task_update_inner_req, str);
		player_mgr.postMessage(m);
	}

	void starwar_system::taskInfoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		d->Starwar.updateTask();
	}

	taskPtr starwar_system::getTaskPtr(int task_id)
	{
		starwarTaskList::iterator iter = _taskList.find(task_id);
		if (iter != _taskList.end())
			return iter->second;
		return taskPtr();
	}

	void starwar_system::taskCommitReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		ReadJsonArray;
		int task_id = js_msg[0u].asInt();

		starwarTaskList::iterator iter = _taskList.find(task_id);
		if (iter == _taskList.end())
			Return(r, _task_error);

		int result = d->Starwar.taskCommit(task_id);
		if (result != starwar::_task_finished)
			Return(r, _task_error);

		d->Starwar.updateTask();
		Return(r, _success);
	}

	void starwar_system::buyTransferTimeReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();
		
		ReadJsonArray;
		int num = js_msg[0u].asInt();

		int result = d->Starwar.buyTransferTimes(num);
		Return(r, result);
	}

	void starwar_system::rankingRewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		int result = d->Starwar.getRankingReward();
		Return(r, result);
	}

	void starwar_system::clearTransferCdReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CheckCountryId();

		int result = d->Starwar.clearTransferCd();
		Return(r, result);
	}

	void starwar_system::addBattleBuff(playerDataPtr pdata, battleSide& side)
	{
		armyVec vec = side.getMember(MO::mem_null);
		for (armyVec::iterator it = vec.begin(); it != vec.end(); it++)
		{
			//LogI << (*it)->addCharacter[idx_phyAddHurt] << LogEnd;
			(*it)->addCharacter[idx_phyAddHurt] += _buff[battleParam::_index_atk].getRate();
			//LogI << (*it)->addCharacter[idx_phyAddHurt] << LogEnd;
			adjustRate((*it)->addCharacter[idx_phyAddHurt], true);
			//LogI << (*it)->addCharacter[idx_phyAddHurt] << LogEnd;
			(*it)->addCharacter[idx_warAddHurt] += _buff[battleParam::_index_atk].getRate();
			adjustRate((*it)->addCharacter[idx_warAddHurt], true);
			(*it)->addCharacter[idx_magicAddHurt] += _buff[battleParam::_index_atk].getRate();
			adjustRate((*it)->addCharacter[idx_magicAddHurt], true);
			(*it)->addCharacter[idx_cureModule] += _buff[battleParam::_index_atk].getRate();
			adjustRate((*it)->addCharacter[idx_cureModule], true);
			
			(*it)->addCharacter[idx_phyCutHurt] += _buff[battleParam::_index_def].getRate();
			adjustRate((*it)->addCharacter[idx_phyCutHurt], false);
			(*it)->addCharacter[idx_warCutHurt] += _buff[battleParam::_index_def].getRate();
			adjustRate((*it)->addCharacter[idx_warCutHurt], false);
			(*it)->addCharacter[idx_magicCutHurt] += _buff[battleParam::_index_def].getRate();
			adjustRate((*it)->addCharacter[idx_magicCutHurt], false);
		}
		_buff[battleParam::_index_atk].clear();
		_buff[battleParam::_index_def].clear();
	}

	void starwar_system::getNpcGuildList(guildList& guild_list, int type)
	{
		if (_param.toLoadNpc())
		{
			int index = type - 1;
			if (index >= (int)_npcMap.size() || index < 0)
				return;
			npcMap& npc_map = _npcMap[index];
			for (npcMap::iterator iter = npc_map.begin(); iter != npc_map.end(); ++iter)
				guild_list.push(iter->first, (int)_param.getMaxSupply());
		}
	}

	rankRecord starwar_system::getOldRank(int player_id, int country_id)
	{
		return _rankList[country_id].getOldRank(player_id);
	}

	void starwar_system::noticeUnity(int country_id)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][starwar_type_field_str] = 0;
		msg[msgStr][1u][starwar_country_id_field_str] = country_id;
		ruler_sys.getValue(msg[msgStr][1u][starwar_king_info_field_str], country_id);
		playerManager::playerDataVec vec = player_mgr.onlineSpherePlayer();
		MsgSignOnline(vec, msg, gate_client::starwar_notice_unity_reward_resp);
//		playerManager::playerDataVec vec = player_mgr.onlinePlayer();
// 		for (unsigned i = 0; i < vec.size(); ++i)
// 		{
// 			if (vec[i]->Base.getSphereID() < 0 || vec[i]->Base.getSphereID() > 2)
// 				continue;
// 			player_mgr.sendToPlayer(vec[i]->playerID, gate_client::starwar_notice_unity_reward_resp, msg);
// 		}
	}

	void starwar_system::gatherBroadcast(playerDataPtr d, int star_id)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::starwar_king_gather;
		msg[msgStr][1u][senderChannelStr] = chat_kingdom;
		msg[msgStr][1u]["pl"].append(d->Base.getName());
		msg[msgStr][1u]["pl"].append(star_id);
		chat_sys.chatOption(chat_kingdom, gate_client::chat_broadcast_resp, msg, d);
	}

	void starwar_system::mainInfoBroadcast()
	{
		for (set<basePlayer>::iterator iter = _playerList.begin(); iter != _playerList.end(); ++iter)
		{
			playerDataPtr d = player_mgr.getOnlinePlayer(iter->getPlayerId());
			if (d == playerDataPtr())
				continue;
			sendMainInfo(d);
		}
	}

	void starwar_system::killKingBroadcast(playerDataPtr d, playerDataPtr targetP)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::starwar_kill_king;
		msg[msgStr][1u][senderChannelStr] = chat_all;
		msg[msgStr][1u]["pl"].append(d->Base.getName());
		msg[msgStr][1u]["pl"].append(targetP->Base.getSphereID());
		msg[msgStr][1u]["pl"].append(targetP->Base.getName());
		chat_sys.chatOption(chat_all, gate_client::chat_broadcast_resp, msg, playerDataPtr());
	}

	void starwar_system::noticeKingGather(int country_id, int player_id)
	{
		const gatherInfo& gather_info = _systemInfo.getGatherInfo(country_id);
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = gather_info.getToStarId();
		if (player_id == -1)
		{
			const gatherInfo::gatherList& gather_list = gather_info.getGatherList();
			for (gatherInfo::gatherList::const_iterator iter = gather_list.begin(); iter != gather_list.end(); ++iter)
			{
				if (_playerList.find(basePlayer::makeKey(*iter)) == _playerList.end())
					continue;
				player_mgr.sendToPlayer(*iter, gate_client::starwar_notice_king_gather_resp, msg);
			}
		}
		else
		{
			player_mgr.sendToPlayer(player_id, gate_client::starwar_notice_king_gather_resp, msg);
		}
	}

	unsigned starwar_system::getUnityReward(int country_id)
	{
		return _systemInfo.getUnityReward(country_id);
	}

	void starwar_system::testBroadcastReq(msg_json& m, Json::Value& r)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::starwar_kill_king;
		msg[msgStr][1u][senderChannelStr] = chat_all;
		msg[msgStr][1u]["pl"].append("aaa");
		msg[msgStr][1u]["pl"].append(0);
		msg[msgStr][1u]["pl"].append("bbb");
		chat_sys.chatOption(chat_all, gate_client::chat_broadcast_resp, msg, playerDataPtr());

		for (unsigned i = 0; i < 3; ++i)
			_rankList[i].broadcast();
	}
}