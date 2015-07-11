#include "arena_system.h"
#include "response_def.h"
#include "season_system.h"
#include "file_system.h"
#include "battle_def.h"
#include "battle_system.h"
#include "chat_system.h"
#include "ruler_helper.h"
#include "player_ruler.h"
#include "record_system.h"
#include "task_system.h"
#include "reportShare.h"
#include "timmer.hpp"
#include "activity_system.h"
using namespace mongo;

namespace gg
{
	void hero_info::setFromBSON(const mongo::BSONObj& obj)
	{
		checkNotEoo(obj[arena_player_id_field_str])
			_player_id = obj[arena_player_id_field_str].Int();
		checkNotEoo(obj[arena_country_id_field_str])
			_country_id = obj[arena_country_id_field_str].Int();
		checkNotEoo(obj[arena_level_field_str])
			_lv = obj[arena_level_field_str].Int();
		checkNotEoo(obj[arena_nickname_field_str])
			_nickname = obj[arena_nickname_field_str].String();
		checkNotEoo(obj[arena_face_id_field_str])
			_face_id = obj[arena_face_id_field_str].Int();
		checkNotEoo(obj[arena_power_field_str])
			_power = obj[arena_power_field_str].Int();
	}

	void arena_helper::setFromBSON(const mongo::BSONObj& obj)
	{
		_year = obj[arena_year_field_str].Int();

		vector<BSONElement> eles = obj[arena_famous_person_list_field_str].Array();

		for(unsigned j = 0; j < eles.size(); ++j)
		{
			_mems[j]._player_id = eles[j][arena_player_id_field_str].Int();
			_mems[j]._nickname = eles[j][arena_nickname_field_str].String();
			_mems[j]._country_id = eles[j][arena_country_id_field_str].Int();
		}
	}

	const string arena_system::arenaYearRewardDirStr1 = "./instance/arena/arenaReward_1.json";
	const string arena_system::arenaYearRewardDirStr2 = "./instance/arena/arenaReward_2.json";
	const string arena_system::arenaYearRewardDirStr3 = "./instance/arena/arenaReward_3.json";
	const string arena_system::arenaTenFightRewardDirStr = "./instance/arena/arenaTenReward.json";
	const string arena_system::arenaNpcInfoDirStr = "./instance/arena/npcInfo.json";
	const string arena_system::arenaLuckyRewardDirStr = "./instance/arena/lucky.json";
	

	arena_system* const arena_system::arenaSys = new arena_system;

	arena_system::arena_system()
		: _max_term_id(0), _next_lucky_update_time(0)
	{
		_bInit = false;
		tick = na::time_helper::get_sys_time();
	}

	void arena_system::loadRankingListDB()
	{
		db_mgr.ensure_index(arena_ranking_list_db_str, BSON(arena_rank_field_str << 1));

		objCollection objs = db_mgr.Query(arena_ranking_list_db_str);

		_hero_infos.reserve(int(objs.size() * 1.5));
		_hero_infos.assign(objs.size() + 1, hero_info());

		for(unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];
			int rank = obj[arena_rank_field_str].Int();
			if (rank > 0)
				_hero_infos[rank].setFromBSON(obj);
			else
				LogW << "arena_system::loadRankingListDB error: rank <= 0" << LogEnd;
		}
	}

	void arena_system::loadOldRankingListDB()
	{
		objCollection objs =	db_mgr.Query(arena_old_ranking_list_db_str);

		_last_term_hero_infos = Json::arrayValue;

		vector<hero_info> temp_list;
		temp_list.assign(objs.size(), hero_info());

		// sort
		for(unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];

			int rank = obj[arena_rank_field_str].Int();
			if(rank <= (int)temp_list.size())
				temp_list[rank - 1].setFromBSON(obj);
		}

		for(vector<hero_info>::iterator iter = temp_list.begin(); iter != temp_list.end(); ++iter)
		{
			Json::Value info;
			packageHeroInfo(info, iter->get_player_id(), iter->get_nickname(), iter->get_country_id(), iter->get_lv(), iter->get_face_id());
			_last_term_hero_infos.append(info);
		}
	}

	void arena_system::getNewLuckyIndex(vector<int>& lucky_index)
	{
		lucky_index.clear();
		lucky_index.assign(300, 0);

		for (unsigned i = 0; i < 20; ++i)
		{
			int rank = commom_sys.random() % 5 + i * 5;
			lucky_index[rank] = 1;
		}
		for (unsigned i = 0; i < 20; ++i)
		{
			int rank = commom_sys.random() % 10 + i * 10 + 100;
			lucky_index[rank] = 1;
		}
	}

	void arena_system::loadLuckyIndexDB()
	{
		objCollection objs = db_mgr.Query(arena_lucky_index_list_db_str);

		if (!objs.empty())
		{
			_lucky_index.assign(300, 0);
			_old_lucky_index.assign(300, 0);
		}

		
		set<lucky_helper> my_helper;
		for (unsigned i = 0; i < objs.size(); ++i)
		{
			int index = objs[i][arena_index_field_str].Int();
			if (index == -1)
			{
				_next_lucky_update_time = objs[i][arena_time_field_str].Int();
				continue;
			}

			int old_rank = objs[i][arenaOldRankStr].Int();
			int player_id = objs[i][arena_player_id_field_str].Int();
			string name = objs[i][arena_nickname_field_str].String();
			int now_rank = objs[i][arena_rank_field_str].Int();

			my_helper.insert(lucky_helper(index, old_rank, player_id, name, now_rank));

			_playerId2Rank.insert(make_pair(player_id, old_rank));

			if (old_rank <= 300 && old_rank > 0)
				_old_lucky_index[old_rank - 1] = 1;
			if (now_rank <= 300 && now_rank > 0)
				_lucky_index[now_rank - 1] = 1;
		}

		for (set<lucky_helper>::iterator iter = my_helper.begin(); iter != my_helper.end(); ++iter)
		{
			if (iter->_player_id != -1)
			{
				Json::Value temp;
				temp.append(iter->_old_rank);
				temp.append(iter->_player_id);
				temp.append(iter->_name);
				_old_ranking_list.push_back(temp);
			}
			_lucky_ranking_list.push_back(iter->_rank);
		}
	}

	void arena_system::arenaGetLuckyRewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		d->Arena.checkAndUpdate();
		if (d->Arena.isLuckyReward())
			Return(r, 1);

		d->Arena.doLuckyReward();

		updateToPlayer(d);
		r[msgStr][1u] = Json::arrayValue;
		Json::Value temp;
		temp["aid"] = action_add_gold;
		temp["an"] = getLuckyReward(d->Arena.getLastLuckyRank());
		r[msgStr][1u].append(temp);
		Return(r, 0);
	}
	
	void arena_system::arenaNewLuckyRankListReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int begin = js_msg[0u].asInt();
		int count = js_msg[1u].asInt();

		if (begin < 1 || count < 1)
			Return(r, 1);

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u]["l"] = Json::arrayValue;
		msg[msgStr][1u]["n"] = (int)_lucky_ranking_list.size();
		for (int i = 0; i < count; ++i)
		{
			if (begin + i - 1 >= (int)_lucky_ranking_list.size() || begin + i - 1 < 0)
				break;

			msg[msgStr][1u]["l"].append(_lucky_ranking_list[begin + i - 1].asInt());
		}
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::arena_new_lucky_ranking_list_resp, msg);
	}

	void arena_system::arenaOldLuckyRankListReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int begin = js_msg[0u].asInt();
		int count = js_msg[1u].asInt();

		if (begin < 1 || count < 1)
			Return(r, 1);

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u]["l"] = Json::arrayValue;
		msg[msgStr][1u]["n"] = (int)_old_ranking_list.size();
		for (int i = 0; i < count; ++i)
		{
			if (begin + i - 1 >= (int)_old_ranking_list.size() || begin + i - 1 < 0)
				break;
			msg[msgStr][1u]["l"].append(_old_ranking_list[begin + i - 1]);
		}
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::arena_old_lucky_ranking_list_resp, msg);
	}

	void arena_system::loadFamousPersonListDB()
	{
		objCollection objs = 	db_mgr.Query(arena_famous_person_list_db_str);

		for(unsigned i = 0; i < objs.size(); ++i)
		{
			checkNotEoo(objs[i][arena_famous_person_list_field_str])
			{
				mongo::BSONObj& obj = objs[i];
				arena_helper h;
				h.setFromBSON(obj);
				_helper_set.insert(h);
			}
		}

		for(set<arena_helper>::const_iterator iter = _helper_set.begin(); iter != _helper_set.end(); ++iter)
			_famous_person_infos_list.push_back(iter->getValue(++_max_term_id));

		while(_famous_person_infos_list.size() > _max_num_of_famous_person_list_count)
			_famous_person_infos_list.pop_front();
	}

	void arena_system::loadFile()
	{
    	Json::Value yearReward = na::file_system::load_jsonfile_val(arenaYearRewardDirStr1);
		for(unsigned i = 0; i < yearReward.size(); ++i)
		{
			reward one_reward;
			one_reward.setValue(yearReward[i]);
			_rewards[0].push_back(one_reward);
		}
		Json::Value yearReward2 = na::file_system::load_jsonfile_val(arenaYearRewardDirStr2);
		for(unsigned i = 0; i < yearReward2.size(); ++i)
		{
			reward one_reward;
			one_reward.setValue(yearReward2[i]);
			_rewards[1].push_back(one_reward);
		}
		Json::Value yearReward3 = na::file_system::load_jsonfile_val(arenaYearRewardDirStr3);
		for(unsigned i = 0; i < yearReward3.size(); ++i)
		{
			reward one_reward;
			one_reward.setValue(yearReward3[i]);
			_rewards[2].push_back(one_reward);
		}

		Json::Value tenReward = na::file_system::load_jsonfile_val(arenaTenFightRewardDirStr);

		for(unsigned i = 0; i < tenReward.size(); ++i)
		{
			reward one_reward;
			one_reward.setValue(tenReward[i]);
			_ten_fight_rewards.push_back(one_reward);
		}

		Json::Value luckyReward = na::file_system::load_jsonfile_val(arenaLuckyRewardDirStr);
		
		for (unsigned i = 0; i < 3; ++i)
			_lucky_reward[i] = luckyReward[i].asInt();
	}

	void arena_system::initData()
	{
		db_mgr.ensure_index(arenaDBStr, BSON(playerIDStr << 1));

		loadRankingListDB();
		loadOldRankingListDB();
		loadFamousPersonListDB();
		loadLuckyIndexDB();
		
		loadFile();
		loadNpcInfo();

		boost::filesystem::path path_dir(reportArenaPVPDir);
		if(!boost::filesystem::exists(path_dir))
			boost::filesystem::create_directory(path_dir);

		Timer::AddEventSeasonCycleTime(boostBind(arena_system::luckyRankingBroadcast, this), season_quarter_two, 21, 55, 30);
		Timer::AddEventSeasonCycleTime(boostBind(arena_system::luckyRankingBroadcast, this), season_quarter_three, 21, 55, 30);
		Timer::AddEventSeasonCycleTime(boostBind(arena_system::luckyRankingBroadcast, this), season_quarter_four, 21, 55, 30);
		Timer::AddEventSeasonCycleTime(boostBind(arena_system::summaryComming, this, (int)_broadcast_at_21_45), season_quarter_one, 21, 45, 5);
		Timer::AddEventSeasonCycleTime(boostBind(arena_system::summaryComming, this, (int)_broadcast_at_21_50), season_quarter_one, 21, 50, 5);
		Timer::AddEventSeasonCycleTime(boostBind(arena_system::summaryComming, this, (int)_broadcast_at_21_55), season_quarter_one, 21, 55, 5);
	}

	void arena_system::getEnemyList(playerDataPtr& d, Json::Value& msg)
	{
		int rank = d->Arena.getRank();
		int diff = 1;
		if(rank > 99)
			diff += rank / 100;

		int enemy_num = _max_num_of_enemy > (_hero_infos.size() - 1)? 
			(_hero_infos.size() - 1) : _max_num_of_enemy;

		if(rank <= enemy_num)
			rank = enemy_num + 1;

		msg = Json::arrayValue;
		vector<int> ranks;
		for(int i = 0; i < enemy_num; ++i)
		{
			rank -= diff;
			ranks.push_back(rank);
			d->Arena.addEnemyPid(_hero_infos[rank].get_player_id());
		}
		for(vector<int>::reverse_iterator iter = ranks.rbegin(); iter != ranks.rend(); ++iter)
		{
			int rank = *iter;
			Json::Value info;
			packageEnemyInfo(info, _hero_infos[rank].get_player_id(), _hero_infos[rank].get_nickname(),
				_hero_infos[rank].get_lv(), rank, _hero_infos[rank].get_face_id(), _hero_infos[rank].get_country_id(), _hero_infos[rank].get_power());
			msg.append(info);
		}
	}

	int arena_system::getLuckyReward(int rank)
	{
		if (rank <= 25)
			return _lucky_reward[0];
		if (rank <= 100)
			return _lucky_reward[1];
		return _lucky_reward[2];
	}

	void arena_system::updateLuckyListDB()
	{
		_old_lucky_index = _lucky_index;
		getNewLuckyIndex(_lucky_index);

		int new_index = 0;
		int db_index = 1;
		
		unsigned max = 300;
		if (_old_lucky_index.empty())
			max = 40;

		_lucky_ranking_list.clear();
		_old_ranking_list.clear();
		_playerId2Rank.clear();

		for (unsigned i = 1; i <= max; ++i)
		{
			int old_rank = -1;
			int player_id = -1;
			string name = "";

			if (!_old_lucky_index.empty())
			{
				if (_old_lucky_index[i - 1] == 0)
					continue;
				else
				{
					if (i < (int)_hero_infos.size())
					{
						player_id = _hero_infos[i].get_player_id();
						name = _hero_infos[i].get_nickname();
					}
					old_rank = i;
				}
			}

			while (_lucky_index[new_index++] == 0);

			mongo::BSONObj key = BSON(arena_index_field_str << db_index);
			mongo::BSONObj obj = BSON(arena_index_field_str << db_index
				<< arenaOldRankStr << old_rank << arena_player_id_field_str << player_id
				<< arena_nickname_field_str << name << arena_rank_field_str << new_index);

			_playerId2Rank.insert(make_pair(player_id, old_rank));

			db_mgr.save_mongo(arena_lucky_index_list_db_str, key, obj);

			if (player_id != -1)
			{
				Json::Value temp;
				temp.append(i);
				temp.append(player_id);
				temp.append(name);
				_old_ranking_list.push_back(temp);
			}
			
			_lucky_ranking_list.push_back(new_index);

			++db_index;
		}

		mongo::BSONObj key = BSON(arena_index_field_str << -1);
		mongo::BSONObj obj = BSON(arena_index_field_str << -1
			<< arena_time_field_str << _next_lucky_update_time);
		db_mgr.save_mongo(arena_lucky_index_list_db_str, key, obj);
	}

	void arena_system::updateArenaRankListDB(playerDataPtr d, int rank)
	{
		mongo::BSONObj key = BSON(arena_rank_field_str << rank);

		mongo::BSONObj obj = BSON(arena_rank_field_str << rank
			<< arena_player_id_field_str << d->playerID << arenaCountryStr << d->Base.getSphereID() 
			<< arenaNickNameStr << d->Base.getName() << arenaLevelStr << d->Base.getLevel() 
			<< arena_face_id_field_str << d->Base.getFaceID() << arena_power_field_str << d->Embattle.getCurrentBV());

		db_mgr.save_mongo(arena_ranking_list_db_str, key, obj);
	}

	int arena_system::getNewPlayerRank(playerDataPtr d)
	{
	    int rank = _hero_infos.size();

		_hero_infos.push_back(hero_info(d->playerID, d->Base.getSphereID(), 
			d->Base.getName(), d->Base.getLevel(), d->Base.getFaceID(), d->Embattle.getCurrentBV()));

		updateArenaRankListDB(d, rank);

		return rank;
	}

	void arena_system::packageEnemyInfo(Json::Value& info, int player_id, const string& name, int lv , int rank, int face_id, int country_id, int power)
	{
		info.append(player_id);
		info.append(name);
		info.append(lv);
		info.append(rank);
		info.append(face_id);
		info.append(country_id);
		info.append(power);
		if (rank <= (int)_lucky_index.size() && _lucky_index[rank - 1] == 1)
			info.append(true);
	}

	void arena_system::packageHeroInfo(Json::Value& info, int player_id, const string& name, int country_id, int lv, int face_id)
	{
		info.append(player_id);
		info.append(name);
		info.append(country_id);
		info.append(lv);
		info.append(face_id);
	}

	void arena_system::packageReportInfo(Json::Value& info, const string& file, bool side, int country_id, string& name, int result, int old_rank, int rank, int time, int reward, int lv)
	{
		info = Json::arrayValue;
		info.append(file);
		info.append(side);
		info.append(country_id);
		info.append(name);
		info.append(result);
		info.append(old_rank);
		info.append(rank);
		info.append(time);
		info.append(reward);
		info.append(lv);
	}

	void arena_system::setYearStamp(unsigned t)
	{
		_year_stamp = t;

		if((_year_stamp - season_sys.getServerOpenTime()) / DAY >= 90)
			_reward_type = 2;
		else if((_year_stamp - season_sys.getServerOpenTime()) / DAY >= 40)
			_reward_type = 1;
		else
			_reward_type = 0;
	}

	void arena_system::doCheckAndUpdate(unsigned now)
	{
		if (_bInit)
		{
			static bool flag = true;

			if (now >= _next_year_time)
			{
				flag = true;
				setYearStamp(season_sys.getYearStamp(now));
				_next_year_time = season_sys.getNextSeasonTime(season_quarter_one, 22);

				updateFamousPersonList(_year_stamp);
				updateArenaOldRankingList();

				string str = "";
				na::msg::msg_json auto_m(gate_client::arena_summary_broadcast_inner_req, str);
				player_mgr.postMessage(auto_m);
			}
			if ((now >= (_next_year_time - 10 * MINUTE)) && flag)
			{
				flag = false;
				//chat_sys.sendToAllArenaSummary(playerDataPtr(), 0, -1, -1, -1);	
			}
			if (now >= _next_lucky_update_time)
			{
				_next_lucky_update_time = na::time_helper::get_next_update_time(now, 22);
				updateLuckyListDB();
				playerManager::playerDataVec vec = player_mgr.onlinePlayer();
				ForEach(playerManager::playerDataVec, iter, vec)
				{
					if (*iter != playerDataPtr())
					{
						(*iter)->Arena.checkAndUpdate(now);
						updateToPlayer(*iter);
					}
				}
			}
		}
		else
		{
			_bInit = true;
			setYearStamp(season_sys.getYearStamp(now));
			_next_year_time = season_sys.getNextSeasonTime(season_quarter_one, 22);
			if (now - season_sys.getServerOpenTime() >= 17 * HOUR)
			{
				if (_next_year_time - now <= 17 * HOUR)
					setYearStamp(_year_stamp - (86400 * 4));
				arena_helper h;
				h._year = _year_stamp;
				if (_helper_set.find(h) == _helper_set.end())
				{
					updateFamousPersonList(_year_stamp);
					updateArenaOldRankingList();
				}
			}
		}
	}

	void arena_system::checkAndUpdate(boost::system_time& tmp)
	{
		if((tmp - tick).total_seconds() < 1)return;
		tick = tmp;
		doCheckAndUpdate();
	}

	void arena_system::loadNpcInfo()
	{
		for (unsigned i = 0; i < 5; ++i)
			_npcName[i] = war_story.getNewRandomName();

		Json::Value info = na::file_system::load_jsonfile_val(arenaNpcInfoDirStr);

		_npcPtr = warStory::create();

		_npcPtr->mapName = _npcName[0];
		_npcPtr->localID = -1;
		_npcPtr->backGround = -1;
		_npcPtr->faceID = 208;
		_npcPtr->mapLevel = _lv_limit;
		_npcPtr->mapValue = -1;
		_npcPtr->frontLocalID = -1;
		_npcPtr->playerExp = 0;
		_npcPtr->pilotExp = 0;
		_npcPtr->mapType = -1;
		Json::Value& army_data = info["trd"];
		_npcPtr->npcArmy = war_story.formatNpcArmy(army_data);
		_npcPtr->actionVec = war_story.formationAction(info, 0);
	}

	void arena_system::updateFakeToPlayer(playerDataPtr d)
	{
		const static int face_ids[] = { 208, 200, 207, 196, 197 };

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][arenaRankStr] = d->Arena.getRank() + 5;
		msg[msgStr][1u][arenaCdStr] = 0;
		msg[msgStr][1u][arenaOldRankStr] = -1;
		msg[msgStr][1u][arenaRemainTimesStr] = 15;
		msg[msgStr][1u][arenaTotalTimesStr] = 0;
		msg[msgStr][1u][arenaBuyTimesStr] = 0;
		msg[msgStr][1u][arenaHaveGotTenChallengeRewardStr] = false;
		msg[msgStr][1u][arenaHaveGotYearRewardStr] = true;
		msg[msgStr][1u][arena_year_field_str] = _year_stamp;
		msg[msgStr][1u][arena_reward_type_str] = getRewardType();
		msg[msgStr][1u][arenaLuckyRewardStr] = 0;
		msg[msgStr][1u][arenaLuckyRankStr] = arena_sys.isNewLuckyIndex(d->Arena.getRank() + 5);

		msg[msgStr][1u][arenaRecordsStr] = Json::arrayValue;
		msg[msgStr][1u][arena_enemy_list_field_str] = Json::arrayValue;
		Json::Value& ref = msg[msgStr][1u][arena_enemy_list_field_str];
		for (unsigned i = 0; i < 5; ++i)
		{
			Json::Value info;
			int power = commom_sys.randomBetween(35000, 40000);
			if (i == 0)
				d->Arena.setNpcPower(power);
			packageEnemyInfo(info, i + 1, _npcName[i], _lv_limit, d->Arena.getRank() + i, face_ids[i], -1, power);
			ref.append(info);
		}

		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::arena_main_update_resp, msg);
	}

	void arena_system::updateToPlayer(playerDataPtr d, int pre_rank)
	{
		if (d->Arena.getRank() == -1)
			return;

		d->Arena.clearEnemyPidList();

		Json::Value msg;
		getEnemyList(d, msg[msgStr][1u][arena_enemy_list_field_str]);
		d->Arena.checkAndUpdate();
		d->Arena.package(msg);
		msg[msgStr][1u][arena_year_field_str] = _year_stamp;
		msg[msgStr][1u][arena_reward_type_str] = getRewardType();
		//msg[msgStr][1u][arena_pre_rank_field_str] = pre_rank;
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::arena_main_update_resp, msg);
	}

	void arena_system::arenaMainUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		bool state = js_msg[0u].asBool();

		if(d->Base.getLevel() < _lv_limit)
			Return(r, 1);

		if(d->Arena.getRank() == -1)
			d->Arena.initData(getNewPlayerRank(d));

		if (state && d->Arena.getAllTimes() <= 0)
			updateFakeToPlayer(d);
		else
			updateToPlayer(d);
	}
	
	void arena_system::arenaHeroRankNewUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		Json::Value msg;
		msg[msgStr][1u] = Json::arrayValue;
		Json::Value& ref_hero_rank = msg[msgStr][1u];
		for(unsigned i = 1; i < _hero_infos.size(); ++i)
		{
			if(i > _max_num_of_ranking_list)
				break;
			Json::Value info;
			packageHeroInfo(info, _hero_infos[i].get_player_id(), _hero_infos[i].get_nickname(),_hero_infos[i].get_country_id(), _hero_infos[i].get_lv(), _hero_infos[i].get_face_id());
			ref_hero_rank.append(info);
		}
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::arena_hero_rank_new_update_resp, msg); 
	}

	void arena_system::arenaHeroRankOldUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		Json::Value msg;
		msg[msgStr][1u] = _last_term_hero_infos;
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::arena_hero_rank_old_update_resp, msg); 
	}

	void arena_system::arenaFamousPersonRankUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		UnAcceptRetrun(js_msg[1u].isInt(), -1);

		int begin = js_msg[0u].asInt();
		int count = js_msg[1u].asInt();

		Json::Value msg;
		msg[msgStr][1u][arena_famous_person_list_count_field_str] = unsigned(_famous_person_infos_list.size());
		msg[msgStr][1u][arena_famous_person_list_field_str] = Json::arrayValue;
		Json::Value& ref_famous_person = msg[msgStr][1u][arena_famous_person_list_field_str];
		for(unsigned i = 0; i < (unsigned)count; ++i)
		{
			if(begin + i > _famous_person_infos_list.size())
				break;
			ref_famous_person.append(_famous_person_infos_list[_famous_person_infos_list.size() - begin - i]);
		}

		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::arena_famous_person_rank_update_resp, msg); 
	}

	void arena_system::arenaFakeChallegeReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		if (d->Base.getLevel() < _lv_limit || d->Arena.getAllTimes() > 0)
			Return(r, -1);

		unsigned now = na::time_helper::get_current_time();
		d->Arena.checkAndUpdate(now);
		doCheckAndUpdate(now);

		string name = js_msg[2u].asString();
		_npcPtr->mapName = name;
		_npcPtr->mapValue = d->Arena.getNpcPower();

		Battlefield field;
		field.atkSide = battleSide::Create(d);
		field.defSide = battleSide::Create(_npcPtr);
		if (field.atkSide == battleSidePtr() || field.defSide == battleSidePtr())
			Return(r, -2);

		field.atkSide->setFileName(d->Arena.getFileName());
		d->Arena.alterCd();
		field.ParamJson[BATTLE::strArenaParam].append(d->Arena.getRank() + 5);
		field.ParamJson[BATTLE::strArenaParam].append(d->Arena.getRank());
		field.ParamJson[BATTLE::strArenaParam].append(true);
		field.ParamJson[BATTLE::strArenaParam].append(name);
		battle_sys.PostBattle(field, battle_arena_type);
		activity_sys.updateActivity(d, 0, activity::_arena_challenge);
	}

	void arena_system::arenaChallegeReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		int targetPlayer = js_msg[0u].asInt();

		if (targetPlayer > 0 && targetPlayer <= 5)
		{
			arenaFakeChallegeReq(m, r);
			return;
		}

		playerDataPtr targetP = player_mgr.getPlayerMain(targetPlayer);
		playerDataPtr d = player_mgr.getPlayerMain(m._player_id, m._net_id);

		if(playerDataPtr() == d || playerDataPtr() == targetP){
			if(m._post_times > 0)
				Return(r, -1);
			player_mgr.postMessage(m);
			return;
		}

		int rank = js_msg[1u].asInt();
		if (_hero_infos[rank].get_player_id() != targetPlayer)
		{
			updateToPlayer(d);
			Return(r, 5);
		}

		unsigned now = na::time_helper::get_current_time();
		d->Arena.checkAndUpdate(now);
		targetP->Arena.checkAndUpdate(now);
		doCheckAndUpdate(now);

		if(targetP == d)
			Return(r, 1);

		if(!d->Arena.checkEnemyPid(targetPlayer))
			Return(r, 1);

		if(!d->Arena.checkRemainTime())
			Return(r, 2);

		if(d->Arena.checkInCd(now))
			Return(r, 3);

		if(d->isOperatBattle() || targetP->isOperatBattle())
			Return(r, 4);

		Battlefield field;
		field.atkSide = battleSide::Create(d);
		field.defSide = battleSide::Create(targetP);
		if(field.atkSide == battleSidePtr() || field.defSide == battleSidePtr())Return(r, -1);
		field.atkSide->setFileName(d->Arena.getFileName());
		field.defSide->setFileName(targetP->Arena.getFileName());
		d->Arena.alterCd();
		field.ParamJson[BATTLE::strArenaParam].append(d->Arena.getRank());
		field.ParamJson[BATTLE::strArenaParam].append(targetP->Arena.getRank());
		field.ParamJson[BATTLE::strArenaParam].append(false);
		battle_sys.PostBattle(field, battle_arena_type);
		activity_sys.updateActivity(d, 0, activity::_arena_challenge);
	}

	void arena_system::arenaFakeChallegeResp(const int battleResult, Battlefield field)
	{
		playerDataPtr d = player_mgr.getPlayerMain(field.atkSide->getSideID());
		if (d == playerDataPtr())
			return;

		unsigned now = na::time_helper::get_current_time();
		d->Arena.checkAndUpdate(now);
		doCheckAndUpdate(now);

		Json::Value info;
		string file_name = field.atkSide->getFileName();
		string name = field.ParamJson[BATTLE::strArenaParam][3u].asString();
		packageReportInfo(info, file_name, true, -1, name, battleResult, d->Arena.getRank() + 5, d->Arena.getRank(), now, _win_silver, _lv_limit);
		d->Arena.updateAfterActiveBattle(info, d->Arena.getRank());
		task_sys.updateBranchTaskInfo(d, _task_arena_win_times);

		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(d->Base.getLevel()));
		fields.push_back(boost::lexical_cast<string, int>(_lv_limit));
		fields.push_back(boost::lexical_cast<string, int>(battleResult));
		fields.push_back(boost::lexical_cast<string, int>(_win_silver));
		StreamLog::Log(arenaLogDBStr, d, boost::lexical_cast<string, int>(d->Arena.getRank() + 5), boost::lexical_cast<string, int>(d->Arena.getRank()), fields, _challenge_log);

		d->Base.alterSilver(_win_silver);

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = battleResult;
		msg[msgStr][2u] = field.atkSide->getFileName();
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::arena_challege_resp, msg);

		updateToPlayer(d);
		helper_mgr.runSaveAndUpdate();
	}

	void arena_system::arenaChallegeResp(const int br, Battlefield field)
	{
		int battle_result = br;
		if (battle_result != 1)
			battle_result = 2;

		State::setState(gate_client::arena_challege_req);
		NumberCounter::Step();

		bool isfake = field.ParamJson[BATTLE::strArenaParam][2u].asBool();
		if (isfake)
		{
			arenaFakeChallegeResp(battle_result, field);
			return;
		}

		playerDataPtr d = player_mgr.getPlayerMain(field.atkSide->getSideID());
		playerDataPtr targetP = player_mgr.getPlayerMain(field.defSide->getSideID());
		if(playerDataPtr() == d || playerDataPtr() == targetP)return;

		unsigned now = na::time_helper::get_current_time();
		d->Arena.checkAndUpdate(now);
		targetP->Arena.checkAndUpdate(now);
		doCheckAndUpdate(now);

		bool flag = false;

		int old_rank_d = d->Arena.getRank();
		int old_rank_targetP = targetP->Arena.getRank();
		int new_rank_d = d->Arena.getRank();
		int new_rank_targetP = targetP->Arena.getRank();
		int reward = battle_result == 1 ? _win_silver : _lose_silver;
		if (battle_result == 1 && d->Arena.getRank() > targetP->Arena.getRank())
		{
			flag = true;
			new_rank_d = targetP->Arena.getRank();
			new_rank_targetP = d->Arena.getRank();
		}
		if (battle_result == 1)
			task_sys.updateBranchTaskInfo(d, _task_arena_win_times);

		Json::Value info;
		string file_name = field.atkSide->getFileName();
		string name = targetP->Base.getName();
		packageReportInfo(info, file_name, true, targetP->Base.getSphereID(), name, battle_result, d->Arena.getRank(), new_rank_d, now, reward, targetP->Base.getLevel());
		d->Arena.updateAfterActiveBattle(info, new_rank_d);

		int result = battle_result;
		if (battle_result == 2)
			result = 1;
		else if (battle_result == 1)
			result = 2;

		info = Json::nullValue;
		file_name = field.defSide->getFileName();
		name = d->Base.getName();
		packageReportInfo(info, file_name, false, d->Base.getSphereID(), name, result, targetP->Arena.getRank(), new_rank_targetP, now, 0, d->Base.getLevel());
		targetP->Arena.updateAfterPassiveBattle(info, new_rank_targetP);
		
		updateRankingListAfterBattle(battle_result, d, targetP, flag);

		if(flag)
			sendReportToAll(field, d, targetP);

		
	
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(d->Base.getLevel()));
		fields.push_back(boost::lexical_cast<string, int>(targetP->Base.getLevel()));
		fields.push_back(boost::lexical_cast<string, int>(battle_result));
		fields.push_back(boost::lexical_cast<string, int>(reward));
		StreamLog::Log(arenaLogDBStr, d, boost::lexical_cast<string, int>(old_rank_d), boost::lexical_cast<string, int>(new_rank_d), fields, _challenge_log);
		fields.clear();
		fields.push_back(boost::lexical_cast<string, int>(targetP->Base.getLevel()));
		fields.push_back(boost::lexical_cast<string, int>(d->Base.getLevel()));
		fields.push_back(boost::lexical_cast<string, int>(result));
		fields.push_back(boost::lexical_cast<string, int>(0));
		StreamLog::Log(arenaLogDBStr, targetP, boost::lexical_cast<string, int>(old_rank_targetP), boost::lexical_cast<string, int>(new_rank_targetP), fields, _challenge_log);

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = battle_result;
		msg[msgStr][2u] = field.atkSide->getFileName();
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::arena_challege_resp, msg);

		updateToPlayer(d, old_rank_d);
		helper_mgr.runSaveAndUpdate();
	}

	void arena_system::arenaClearCdReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);	

		unsigned now = na::time_helper::get_current_time();

		if(!d->Arena.checkInCd(now))
			Return(r, 1);

		if(!d->Arena.checkRemainTime())
			Return(r, 2);

		int cost = d->Arena.getClearCdCost(now);

		if(d->Base.getAllGold() < cost)
			Return(r, 3);

		d->Base.alterBothGold(0 - cost);

		d->Arena.clearCd();

		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(d->Base.getLevel()));
		fields.push_back(boost::lexical_cast<string, int>(cost));
		StreamLog::Log(arenaLogDBStr, d, "", "", fields, _clear_cd_log);

		Return(r, 0);
	}

	void arena_system::arenaGetRewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if(d->Arena.getBTenChallengeReward())
			Return(r, 1);
		if(d->Arena.getTotalTimes() < _challenge_reward_times)
			Return(r, 2);

		int index = d->Base.getLevel() > (int)_ten_fight_rewards.size()? 
			(int)_ten_fight_rewards.size() - 1 : d->Base.getLevel() - 1;

		const vector<rewardItem>& rewards = _ten_fight_rewards[index].getItems();

		int silver = 0;
		int weiwang = 0;
		for(unsigned i = 0; i < rewards.size(); ++i)
		{
			int type = rewards[i].getType();
			int num = rewards[i].getNum();
			if(type == action_add_silver)
				silver += num;
			else if(type == action_add_weiwang)
				weiwang += num;
		}

		d->Base.alterSilver(silver);
		d->Base.alterWeiWang(weiwang);

		d->Arena.updateAfterGetReward(0);

		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(d->Arena.getRank()));
		fields.push_back(boost::lexical_cast<string, int>(d->Arena.getTotalTimes()));
		fields.push_back(boost::lexical_cast<string, int>(silver));
		fields.push_back(boost::lexical_cast<string, int>(weiwang));
		StreamLog::Log(arenaLogDBStr, d, "", "", fields, _ten_reward_log);

		r[msgStr][1u] = _ten_fight_rewards[index].getValue();

		Return(r, 0);
	}

	void arena_system::arenaGetYearRewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if(d->Arena.getBYearChallenge() || d->Arena.getOldRank() == -1)
			Return(r, 1);

		int old_rank = d->Arena.getOldRank();
		vector<reward>& yearrewards = getYearRewards();
		if(old_rank > (int)yearrewards.size())
			old_rank = yearrewards.size();
		yearrewards[old_rank - 1].getRewardWithMail(d);

		d->Arena.updateAfterGetReward(1);

		const vector<rewardItem>& rewards = yearrewards[old_rank - 1].getItems();

		

		for(unsigned i = 0; i < rewards.size(); ++i)
		{
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(d->Base.getLevel()));
			fields.push_back(boost::lexical_cast<string, int>(d->Arena.getOldRank()));
			int type = rewards[i].getType();
			fields.push_back(boost::lexical_cast<string, int>(type));
			if(type == action_add_item)
			{
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getId()));
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getNum()));
			}
			else if(type == action_active_pilot)
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getId()));
			else
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getNum()));

			StreamLog::Log(arenaLogDBStr, d, "", "", fields, _year_reward_log);
		}

		r[msgStr][1u] = yearrewards[old_rank - 1].getValue();
		Return(r, 0);
	}

	void arena_system::arenaBuyChallegeTimes(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		int cost = d->Arena.getBuyCost();
		if(cost > d->Base.getAllGold())
			Return(r, 1);

		d->Arena.updateAfterBuyTimes();

		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(d->Base.getLevel()));
		fields.push_back(boost::lexical_cast<string, int>(d->Arena.getRank()));
		fields.push_back(boost::lexical_cast<string, unsigned>(d->Arena.getBuyTimes()));
		fields.push_back(boost::lexical_cast<string, int>(cost));
		fields.push_back(boost::lexical_cast<string, int>(d->Base.getAllGold()));
		StreamLog::Log(arenaLogDBStr, d, "", "", fields, _buy_times_log);

		d->Base.alterBothGold(0 - cost);

		Return(r, 0);
	}

	void arena_system::updateRankingListAfterBattle(int result, playerDataPtr d, playerDataPtr targetP, bool flag)
	{
		if(flag)
		{
			_hero_infos[targetP->Arena.getRank()].reset(targetP->playerID, targetP->Base.getName(), targetP->Base.getSphereID(), targetP->Base.getLevel(), targetP->Base.getFaceID(), targetP->Embattle.getCurrentBV());
			_hero_infos[d->Arena.getRank()].reset(d->playerID, d->Base.getName(), d->Base.getSphereID(), d->Base.getLevel(), d->Base.getFaceID(), d->Embattle.getCurrentBV());

			updateArenaRankListDB(d, d->Arena.getRank());
			updateArenaRankListDB(targetP, targetP->Arena.getRank());
		}
		else if (_hero_infos[targetP->Arena.getRank()].get_power() != targetP->Embattle.getCurrentBV())
		{
			_hero_infos[targetP->Arena.getRank()].reset(targetP->playerID, targetP->Base.getName(), targetP->Base.getSphereID(), targetP->Base.getLevel(), targetP->Base.getFaceID(), targetP->Embattle.getCurrentBV());
			updateArenaRankListDB(targetP, targetP->Arena.getRank());
		}

		int get_silver = result == 1 ? _win_silver : _lose_silver;
		d->Base.alterSilver(get_silver);
	}

	void arena_system::updateFamousPersonList(int year)
	{
		if(_hero_infos.size() <= 1)
			return;

		Json::Value msg = Json::arrayValue;

		mongo::BSONObj key = BSON(arena_year_field_str << year);
		mongo::BSONObjBuilder obj;
		obj << arena_year_field_str << year;

		msg.append(++_max_term_id);

		{
			mongo::BSONArrayBuilder array_arena;
			unsigned i = 1;
			for(; i <= _max_num_of_famous_person_list; ++i)
			{
				if(i >= _hero_infos.size())
					break;

				mongo::BSONObjBuilder b;
				b << arena_player_id_field_str << _hero_infos[i].get_player_id()
					<< arena_nickname_field_str << _hero_infos[i].get_nickname()
					<< arena_country_id_field_str << _hero_infos[i].get_country_id();
				array_arena.append(b.obj());

				msg.append(_hero_infos[i].get_player_id());
				msg.append(_hero_infos[i].get_nickname());
				msg.append(_hero_infos[i].get_country_id());
			}
			for(; i <= _max_num_of_famous_person_list; ++i)
			{
				msg.append(-1);
				msg.append("");
				msg.append(-1);
			}
			msg.append(year);

			obj << arena_famous_person_list_field_str << array_arena.arr();
		}

		db_mgr.save_mongo(arena_famous_person_list_db_str, key, obj.obj());

		_famous_person_infos_list.push_back(msg);
		while(_famous_person_infos_list.size() > _max_num_of_famous_person_list_count)
			_famous_person_infos_list.pop_front();
	}

	void arena_system::sendRankingListLog()
	{
		for(unsigned i = 1; i < _hero_infos.size(); ++i)
		{
			if(i > 300)
				break;

			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(_max_term_id));
			fields.push_back(boost::lexical_cast<string, int>(i));
			fields.push_back(boost::lexical_cast<string, int>(_hero_infos[i].get_country_id()));
			StreamLog::Log(arenaLogDBStr, _hero_infos[i].get_player_id(), _hero_infos[i].get_nickname(), _hero_infos[i].get_lv(), "", "", fields, _ranking_list_log);
		}
	}

	void arena_system::updateArenaOldRankingList()
	{
		_last_term_hero_infos = Json::arrayValue;
		for (unsigned i = 1; i < _hero_infos.size(); ++i)
		{
			if(i > _max_num_of_ranking_list)
				break;

			mongo::BSONObj key = BSON(arena_rank_field_str << i);
			mongo::BSONObj obj = BSON(arena_rank_field_str << i
				<< arena_player_id_field_str << _hero_infos[i].get_player_id()
				<< arena_nickname_field_str << _hero_infos[i].get_nickname()
				<< arena_country_id_field_str << _hero_infos[i].get_country_id()
				<< arena_level_field_str << _hero_infos[i].get_lv()
				<< arena_face_id_field_str << _hero_infos[i].get_face_id());
			 db_mgr.save_mongo(arena_old_ranking_list_db_str, key, obj);

			 Json::Value info;
			 packageHeroInfo(info, _hero_infos[i].get_player_id(), _hero_infos[i].get_nickname(),
				 _hero_infos[i].get_country_id(), _hero_infos[i].get_lv(), _hero_infos[i].get_face_id());
			 _last_term_hero_infos.append(info);		
		}

		 sendRankingListLog();
	}	
	
	void arena_system::sendReportToAll(Battlefield field, playerDataPtr d, playerDataPtr targetP)
	{
		if((d->Arena.getRank() <= 5)
			|| (d->Arena.getRank() <= 100 && d->Base.getLevel() >= 60
			&& commom_sys.random()%100 < activity_sys.getRate(activity::_arena, d)))
		{
			Json::Value msg;
			
			string filePath = reportArenaPVPDir + field.atkSide->getFileName();
			string link = shareSys.shareLink(filePath);
			if(link == "")
			{
				LogW << "arena report shared failed ..." << LogEnd;
				return;
			}
			msg[msgStr][0u] = 0;
			msg[msgStr][1u][senderChannelStr] = chat_all;
			msg[msgStr][1u][chatBroadcastID] = BROADCAST::arean_report;
			const static string ParamListStr = "pl";
			msg[msgStr][1u][ParamListStr].append(d->Base.getName());
			msg[msgStr][1u][ParamListStr].append(targetP->Base.getName());
			msg[msgStr][1u][ParamListStr].append(d->Arena.getRank());
			msg[msgStr][1u][ParamListStr].append(link);

			player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);
		}
	}

	void arena_system::getAutoTitledPlayers(int country_id, Json::Value& players)
	{
		unsigned count = _title_king;
		
		players = Json::arrayValue;
		for(unsigned i = 1; i < _hero_infos.size(); ++i)
		{
			if(i > 200 || count >= _title_max)
				break;
			if(_hero_infos[i].get_country_id() == country_id)
			{
				players.append(_hero_infos[i].get_player_id());
				++count;
			}
		}
	}

	void arena_system::alterFamousPersonList(playerDataPtr d)
	{
		for(unsigned i = 0; i < _famous_person_infos_list.size(); ++i)
		{
			Json::Value& ref =  _famous_person_infos_list[i];
			if(ref[1u].asInt() == d->playerID)
			{
				ref[2u] = d->Base.getName();
				saveFamousPersonList(ref);
				continue;
			}
			if(ref[4u].asInt() == d->playerID)
			{
				ref[5u] = d->Base.getName();
				saveFamousPersonList(ref);
				continue;
			}
			if(ref[7u].asInt() == d->playerID)
			{
				ref[8u] = d->Base.getName();
				saveFamousPersonList(ref);
				continue;
			}
		}
	}

	void arena_system::saveFamousPersonList(Json::Value& info)
	{
		mongo::BSONObj key = BSON(arena_year_field_str << info[10u].asInt());
		mongo::BSONArrayBuilder array_arena;
		for(unsigned i = 1; i < 10; ++i)
		{
			mongo::BSONObjBuilder b;
			b << arena_player_id_field_str << info[i].asInt();
			++i;
			b << arena_nickname_field_str << info[i].asString();
			++i;
			b << arena_country_id_field_str << info[i].asInt();
			array_arena.append(b.obj());
		}
		mongo::BSONObjBuilder obj;
		obj << arena_year_field_str << info[10u].asInt() << arena_famous_person_list_field_str << array_arena.arr();

		db_mgr.save_mongo(arena_famous_person_list_db_str, key, obj.obj());
	}

	int arena_system::inLuckyRank(playerDataPtr d)
	{
		PlayerId2Rank::iterator iter = _playerId2Rank.find(d->playerID);
		if (iter == _playerId2Rank.end())
			return -1;
		return iter->second;
	}

	void arena_system::alterOldRankingList(playerDataPtr d)
	{
		if(d->Arena.getRank() <= _max_num_of_ranking_list)
		{
			for(unsigned i = 0; i < _last_term_hero_infos.size(); ++i)
			{
				if(_last_term_hero_infos[i][0u].asInt() == d->playerID)
				{
					Json::Value& ref = _last_term_hero_infos[i];

					ref[1u] = d->Base.getName();

					mongo::BSONObj key = BSON(arena_rank_field_str << i + 1);
					mongo::BSONObj obj = BSON(arena_rank_field_str << i + 1
						<< arena_player_id_field_str << ref[0u].asInt()
						<< arena_nickname_field_str << ref[1u].asString()
						<< arena_country_id_field_str << ref[2u].asInt()
						<< arena_level_field_str << ref[3u].asInt());
					db_mgr.save_mongo(arena_old_ranking_list_db_str, key, obj);

					break;
				}
			}
		}
	}

	void arena_system::updatePower(playerDataPtr d)
	{
		if (d->Arena.getRank() != -1 && d->Embattle.getCurrentBV() != _hero_infos[d->Arena.getRank()].get_power())
		{
			_hero_infos[d->Arena.getRank()].reset(d->playerID, d->Base.getName(), d->Base.getSphereID(), d->Base.getLevel(), d->Base.getFaceID(), d->Embattle.getCurrentBV());
			updateArenaRankListDB(d, d->Arena.getRank());
		}
	}

	void arena_system::updatePlayerInfo(playerDataPtr d, int type)
	{
		if(d->Arena.getRank() != -1)
		{
			_hero_infos[d->Arena.getRank()].reset(d->playerID, d->Base.getName(), d->Base.getSphereID(), d->Base.getLevel(), d->Base.getFaceID(), d->Embattle.getCurrentBV());	
			updateArenaRankListDB(d, d->Arena.getRank());  // 更新当前排名

			if(type == _arena_alter_name) // 修改名字
			{
				alterOldRankingList(d);
				alterFamousPersonList(d);
			}
		}
	}

	void arena_system::arenaSummaryBroadcastInnerReq(msg_json& m, Json::Value& r)
	{
		vector<playerDataPtr> vec;
		int num = _last_term_hero_infos.size() < 5? _last_term_hero_infos.size() : 5;
		for(int i = 0; i < num; ++i)
		{
			playerDataPtr d = player_mgr.getPlayerMain(_last_term_hero_infos[i][0u].asInt());
			if(d != playerDataPtr())
				vec.push_back(d);
		}
		if(vec.size() != num)
		{
			if(m._post_times < 2)
				player_mgr.postMessage(m);
			return;
		}

		vector<reward>& rewards = getYearRewards();

		for(unsigned i = 0; i < vec.size(); ++i)
		{
			chat_sys.sendToAllArenaSummary(vec[i], i + 1, rewards[i].getItemNum(action_add_silver), 
				rewards[i].getItemNum(action_add_weiwang), rewards[i].getItemNum(action_add_gold_ticket));
		}

		chat_sys.sendToAllArenaSummary(playerDataPtr(), -1, -1, -1, -1);
	}

	vector<reward>& arena_system::getYearRewards()
	{
		return _rewards[_reward_type];
	}

	void arena_system::luckyRankingBroadcast()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::lucky_ranking;
		msg[msgStr][1u][senderChannelStr] = -1;
		player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);

		chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_lucky_ranking);
	}

	void arena_system::summaryComming(int type)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::arena_summary_comming;
		msg[msgStr][1u][senderChannelStr] = -1;
		msg[msgStr][1u]["pl"].append(type);
		player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);

		int times = 3;
		if (type == _broadcast_at_21_55)
			times = 5;
		int r_type = -1;
		switch (type)
		{
			case _broadcast_at_21_45:
				r_type = ROLLBROADCAST::roll_arena_summary_comming_21_45;
				break;
			case _broadcast_at_21_50:
				r_type = ROLLBROADCAST::roll_arena_summary_comming_21_50;
				break;
			case _broadcast_at_21_55:
				r_type = ROLLBROADCAST::roll_arena_summary_comming_21_55;
				break;
			default:
				return;
		}
		chat_sys.sendToAllRollBroadCastCommon(r_type, times);
	}

	vector<int> arena_system::getArenaRankList(int startRank, int endRank)
	{
		if (startRank > endRank || startRank < 1)
			return vector<int>();

		vector<int> ret;
		for (int i = startRank; i <= endRank; i++)
		{
			if (i >= int(_hero_infos.size()))
				break;
			ret.push_back(_hero_infos[i].get_player_id());
		}
		return ret;
	}
}
