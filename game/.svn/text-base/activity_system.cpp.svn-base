#include "activity_system.h"
#include "response_def.h"
#include "activity_game_param.h"

namespace gg
{
	activity_system* const activity_system::activitySys = new activity_system;

	void activity_system::initData()
	{
		loadFile();
		loadReward();
		initActivities();
	}

	void activity_system::loadFile()
	{
		Json::Value val = na::file_system::load_jsonfile_val(activity_daily_activities_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
		{
			activityPtr ptr = creator<activity::mAcitivityInfo>::run();
			ptr->load(val[i]);
			_daily_activities.insert(make_pair(ptr->getId(), ptr));
		}

		val = na::file_system::load_jsonfile_val(activity_time_limited_activities_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
		{
			activityPtr ptr = creator<activity::mAcitivityInfo>::run();
			ptr->load(val[i]);
			_time_limited_activities.insert(make_pair(ptr->getId(), ptr));
		}
	}

	void activity_system::loadReward()
	{
		Json::Value val = na::file_system::load_jsonfile_val(activity_reward_file_dir_str);
		_rewardList.assign(val.size(), activity::mReward());
		for (unsigned i = 0; i < val.size(); ++i)
		{
			int id = val[i]["id"].asInt();
			_rewardList[id - 1].load(val[i]);
		}
	}

	void activity_system::initActivities()
	{
		// 星盟红利
		_daily_activities[activity::_guild_reward]->addCondition(activity::_open, creator<activity::cnnGuildReward>::run());

		// 矿源争夺
		activity::conditionList cnn_list;
		cnn_list.push_back(creator<activity::cnnTime>::run("12:30","14:00"));
		cnn_list.push_back(creator<activity::cnnTime>::run("20:00","23:00"));
		activityPtr ptr = creator<activity::mAcitivityInfo>::run();
		_time_limited_activities.insert(make_pair((int)activity::_mine_battle, ptr));
		_time_limited_activities[activity::_mine_battle]->addCondition(activity::_open, creator<activity::cnnOr>::run(cnn_list));


		//// 日理万机
		//_time_limited_activities[activity::_ri_li_wan_ji]->addCondition(activity::_open, creator<activity::cnnSeason>::run(1, 0, 1, 0, _time_limited_activities[activity::_ri_li_wan_ji]));

		//// 福星高照
		//_time_limited_activities[activity::_fu_xing_gao_zhao]->addCondition(activity::_open, creator<activity::cnnSeason>::run(1, 0, 0, 1, _time_limited_activities[activity::_fu_xing_gao_zhao]));

		//// 财源广进
		//_time_limited_activities[activity::_cai_yuan_guang_jin]->addCondition(activity::_open, creator<activity::cnnSeason>::run(0, 1, 0, 0, _time_limited_activities[activity::_cai_yuan_guang_jin]));

		//// 天道酬勤
		//_time_limited_activities[activity::_tian_dao_chou_qin]->addCondition(activity::_open, creator<activity::cnnSeason>::run(0, 1, 1, 0, _time_limited_activities[activity::_tian_dao_chou_qin]));

		//// 国富民强
		//_time_limited_activities[activity::_guo_fu_min_qiang]->addCondition(activity::_open, creator<activity::cnnSeason>::run(0, 0, 1, 0, _time_limited_activities[activity::_guo_fu_min_qiang]));

		//// 突飞猛进
		//_time_limited_activities[activity::_tu_fei_meng_jin]->addCondition(activity::_open, creator<activity::cnnSeason>::run(0, 0, 0, 1, _time_limited_activities[activity::_tu_fei_meng_jin]));

		//// 要塞战
		//activity::conditionList cnn_list;
		//cnn_list.push_back(creator<activity::cnnTime>::run("12:30", "14:00"));
		//cnn_list.push_back(creator<activity::cnnTime>::run("20:00", "23:00"));
		//_time_limited_activities[activity::_fort_war]->addCondition(activity::_open, creator<activity::cnnOr>::run(cnn_list));

		//// 竞技场
		//_time_limited_activities[activity::_arena]->addCondition(activity::_open, creator<activity::cnnSeason>::run(1, 0, 0, 0, _time_limited_activities[activity::_arena]));
		//_time_limited_activities[activity::_arena]->addCondition(activity::_open, creator<activity::cnnTime>::run("21:50", "22:00"));

		//// 元首候选争斗
		//_time_limited_activities[activity::_ruler_competition]->addCondition(activity::_open, creator<activity::cnnSeason>::run(1, 0, 0, 0, _time_limited_activities[activity::_ruler_competition]));
		//_time_limited_activities[activity::_ruler_competition]->addCondition(activity::_open, creator<activity::cnnTime>::run("14:00", "21:30"));
		//_time_limited_activities[activity::_ruler_competition]->addCondition(activity::_access, creator<activity::cnnCountry>::run());

		//// 星盟领地战
		//_time_limited_activities[activity::_guild_battle]->addCondition(activity::_open, creator<activity::cnnSeason>::run(1, 1, 1, 0, _time_limited_activities[activity::_guild_battle]));
		//_time_limited_activities[activity::_guild_battle]->addCondition(activity::_open, creator<activity::cnnTime>::run("20:00", "20:30"));
		//_time_limited_activities[activity::_guild_battle]->addCondition(activity::_access, creator<activity::cnnGuild>::run());

		//// 星际战场
		//_time_limited_activities[activity::_starwar]->addCondition(activity::_open, creator<activity::cnnTime>::run("20:30", "21:00"));

		//// 元首押注
		//_time_limited_activities[activity::_ruler_betting]->addCondition(activity::_open, creator<activity::cnnSeason>::run(1, 0, 0, 0, _time_limited_activities[activity::_ruler_betting]));
		//_time_limited_activities[activity::_ruler_betting]->addCondition(activity::_open, creator<activity::cnnTime>::run("21:30", "21:40"));
		//_time_limited_activities[activity::_ruler_betting]->addCondition(activity::_access, creator<activity::cnnCountry>::run());

		//// 世界boss
		//_time_limited_activities[activity::_world_boss]->addCondition(activity::_open, creator<activity::cnnTime>::run("21:05", "21:25"));
	}

	void activity_system::resetActivity(activityRecordMap& record_map, int type)
	{
		record_map.clear();
		activityMap& activity_map = type == 0 ? _daily_activities : _time_limited_activities;
		for (activityMap::iterator iter = activity_map.begin(); iter != activity_map.end(); ++iter)
		{
			activity::mRecord record(iter->first, 0);
			record_map.insert(make_pair(record.getId(), record));
		}
	}

	double activity_system::getRate(int id, playerDataPtr d)
	{
		if (id > param_discount_start && id < param_discount_end)
			return act_game_param_mgr.get_game_param_by_id(id);

		activityMap::iterator iter = _time_limited_activities.find(id);
		if (iter == _time_limited_activities.end())
			return 0.0;
		return iter->second->getRate(d);
	}

	int activity_system::getPoints(int index)
	{
		if (index < 1 || index >(int)_rewardList.size())
			return 99999;
		return _rewardList[index - 1].getPoints();
	}

	reward activity_system::getReward(int index, playerDataPtr d)
	{
		if (index < 1 || index >(int)_rewardList.size())
			return reward();
		return _rewardList[index - 1].getReward(d);
	}

	void activity_system::infoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->PlayerActivity.checkAndUpdate();
		d->PlayerActivity.info();
	}

	void activity_system::timeLimitedUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int id = js_msg[0u].asInt();
		d->PlayerActivity.checkAndUpdate();
		d->PlayerActivity.updateTimeLimitedActivity(id);
		Return(r, 0);
	}

	void activity_system::getRewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int index = js_msg[0u].asInt();
		d->PlayerActivity.checkAndUpdate();
		int result = d->PlayerActivity.getReward(index);
		Return(r, result);
	}

	void activity_system::updateActivity(playerDataPtr d, int type, int id)
	{
		d->PlayerActivity.checkAndUpdate();

		if (type == 0)
			d->PlayerActivity.updateDailyActivity(id);
		else
			d->PlayerActivity.updateTimeLimitedActivity(id);
	}

	activityPtr activity_system::getDailyActivityPtr(int id)
	{
		activityMap::iterator iter = _daily_activities.find(id);
		if (iter == _daily_activities.end())
			return activityPtr();
		return iter->second;
	}

	activityPtr activity_system::getTimeLimitedActivityPtr(int id)
	{
		activityMap::iterator iter = _time_limited_activities.find(id);
		if (iter == _time_limited_activities.end())
			return activityPtr();
		return iter->second;
	}

	void activity_system::noticeClientUpdateBonusPar(playerDataPtr d /* = playerDataPtr() */)
	{
		if (d == playerDataPtr())
		{
			playerManager::playerDataVec vec = player_mgr.onlinePlayer();
			ForEach(playerManager::playerDataVec, iter, vec)
			{
				if (*iter != playerDataPtr())
					noticeClientUpdateBonusPar(*iter);
			}
		}
		else
		{
			Json::Value msg;
			msg[msgStr][0u] = 0;
			msg[msgStr][1u] = Json::arrayValue;
			Json::Value& ref_msg = msg[msgStr][1u];
			ForEach(activityMap, iter, _time_limited_activities)
			{
				Json::Value temp;
				temp["bID"] = iter->first;
				temp["bs"] = iter->second->getSeason();
				temp["bv"] = iter->second->getRate(d);
				ref_msg.append(temp);
			}
			Json::Value sv;
			for (unsigned i = 0; i < 4; ++i)
				sv.append(1);
			for (unsigned i = param_discount_start + 1; i < param_discount_end; ++i)
			{
				Json::Value temp;
				temp["bID"] = i;
				temp["bv"] = act_game_param_mgr.get_game_param_by_id(i);
				temp["bs"] = sv;
				ref_msg.append(temp);
			}
			player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::notice_client_update_bonus_par_resp, msg);
		}
	}
}
