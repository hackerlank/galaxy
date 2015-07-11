#include "player_arena.h"
#include "time_helper.h"
#include "helper.h"
#include "season_system.h"
#include "arena_system.h"
#include "battle_system.h"
#include "guild_system.h"
#include "task_system.h"
#include "recorder.hpp"

using namespace mongo;

namespace gg
{
	playerArena::playerArena(playerData& own) 
		: Block(own), _rank(-1), _old_rank(-1), _have_got_ten_challenge_reward(false), _have_got_year_reward(true), _remain_times(0)
		, _total_times(0), _check_path_exist(false), _buy_times(0), _max_file_id(0), _next_challenge_time(0), _all_times(0), _power(0), _next_lucky_update_time(0)
		, _lucky_reward(false), _last_lucky_rank(-1)
	{

	}

	void playerArena::checkAndUpdate(unsigned now)
	{
		if (_rank == -1)
			return;

		bool flag = false;

		if(now >= _next_update_time)
		{
			_next_update_time = na::time_helper::get_next_update_time(now);
			if(_remain_times < 15)
				_remain_times = 15;
			_have_got_ten_challenge_reward = false;
			_buy_times = 0;
			_total_times = 0;
			flag = true;
		}

		if (arena_sys.nextLuckyTime() > _next_lucky_update_time)
		{
			_next_lucky_update_time = arena_sys.nextLuckyTime();
			_lucky_reward = false;
			//_last_lucky_rank = _rank;
			flag = true;
		}

		if(arena_sys.nextYearTime() > _next_year_time)
		{
			_next_year_time = arena_sys.nextYearTime();
			_old_rank = _rank;
			_have_got_year_reward = false;
			flag = true;
		}

		if(!_check_path_exist)
		{
			_check_path_exist = true;
			string pathname = reportArenaPVPDir;
			pathname += boost::lexical_cast<string , int>(own.playerID);
			boost::filesystem::path path_dir(pathname);
			if(!boost::filesystem::exists(path_dir))
				boost::filesystem::create_directory(path_dir);
		}

		if(flag)
			helper_mgr.insertSaveAndUpdate(this);
	}

	void playerArena::autoUpdate()
	{

	}

	void playerArena::package(Json::Value& msg)
	{
		msg[msgStr][1u][arenaRankStr] = _rank;
		msg[msgStr][1u][arenaCdStr] = _next_challenge_time;
		msg[msgStr][1u][arenaOldRankStr] = _old_rank;
		msg[msgStr][1u][arenaRemainTimesStr] = _remain_times;
		msg[msgStr][1u][arenaTotalTimesStr] = _total_times;
		msg[msgStr][1u][arenaBuyTimesStr] = _buy_times;
		msg[msgStr][1u][arenaHaveGotTenChallengeRewardStr] = _have_got_ten_challenge_reward;
		msg[msgStr][1u][arenaHaveGotYearRewardStr] = _have_got_year_reward;
		msg[msgStr][1u][arenaLuckyRewardStr] = arena_sys.inLuckyRank(own.getOwnDataPtr()) != -1 ? (_lucky_reward? 2 : 1) : 0;
		msg[msgStr][1u][arenaLuckyRankStr] = arena_sys.isNewLuckyIndex(_rank);

		msg[msgStr][1u][arenaRecordsStr] = Json::arrayValue;
		Json::Value& ref_records = msg[msgStr][1u][arenaRecordsStr];
		for(list<Json::Value>::reverse_iterator iter = _arena_report_list.rbegin(); iter != _arena_report_list.rend(); ++iter)
		{
			ref_records.append(*iter);
		}
	}

	bool playerArena::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(arenaDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[arenaRankStr]){_rank = obj[arenaRankStr].Int();}
			checkNotEoo(obj[arenaCdStr]){_next_challenge_time = obj[arenaCdStr].Int();}
			checkNotEoo(obj[arenaOldRankStr]){_old_rank = obj[arenaOldRankStr].Int();}
			checkNotEoo(obj[arenaRemainTimesStr]){_remain_times = obj[arenaRemainTimesStr].Int();}
			checkNotEoo(obj[arenaTotalTimesStr]){_total_times = obj[arenaTotalTimesStr].Int();}
			checkNotEoo(obj[arenaHaveGotYearRewardStr]){_have_got_year_reward = obj[arenaHaveGotYearRewardStr].Bool();}
			checkNotEoo(obj[arenaHaveGotTenChallengeRewardStr]){_have_got_ten_challenge_reward = obj[arenaHaveGotTenChallengeRewardStr].Bool();}
			checkNotEoo(obj[arenaNextUpdateTimeStr]){_next_update_time = obj[arenaNextUpdateTimeStr].Int();}
			checkNotEoo(obj[arenaNextYearTimeStr]){_next_year_time = obj[arenaNextYearTimeStr].Int();}
			checkNotEoo(obj[arenaMaxFileIdStr]){_max_file_id = obj[arenaMaxFileIdStr].Int();}
			checkNotEoo(obj[arenaBuyTimesStr]){_buy_times = obj[arenaBuyTimesStr].Int();}
			checkNotEoo(obj[arenaAllTimesStr]){_all_times = obj[arenaAllTimesStr].Int();}
			checkNotEoo(obj[arenaNextLuckyUpdateTimeStr]){ _next_lucky_update_time = obj[arenaNextLuckyUpdateTimeStr].Int(); }
			checkNotEoo(obj[arenaLuckyRewardStr]){ _lucky_reward = obj[arenaLuckyRewardStr].Bool(); }
			checkNotEoo(obj[arenaLuckyRankStr]){ _last_lucky_rank = obj[arenaLuckyRankStr].Int(); }
			checkNotEoo(obj[arenaRecordsStr])
			{
				_arena_report_list.clear();
				vector<BSONElement> ele = obj[arenaRecordsStr].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
				{
					Json::Value info;
					info = Json::arrayValue;
					info.append(ele[i][arenaReportIdStr].String());
					info.append(ele[i][arenaSideStr].Bool());
					info.append(ele[i][arenaCountryStr].Int());
					info.append(ele[i][arenaNickNameStr].String());
					info.append(ele[i][arenaResultStr].Int());
					info.append(ele[i][arenaOldRankStr].Int());
					info.append(ele[i][arenaNewRankStr].Int());
					info.append(ele[i][arenaTimeStr].Int());
					info.append(ele[i][arenaRewardStr].Int());
					if (ele[i][arena_level_field_str].eoo())
						info.append(-1);
					else
						info.append(ele[i][arena_level_field_str].Int());
					_arena_report_list.push_back(info);
				}
			}
		}				
		return true;
	}

	bool playerArena::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID << arenaRankStr << _rank
			<< arenaCdStr << _next_challenge_time << arenaOldRankStr << _old_rank
			<< arenaRemainTimesStr << _remain_times << arenaHaveGotYearRewardStr << _have_got_year_reward
			<< arenaHaveGotTenChallengeRewardStr << _have_got_ten_challenge_reward << arenaNextUpdateTimeStr << _next_update_time
			<< arenaNextYearTimeStr << _next_year_time << arenaTotalTimesStr << _total_times << arenaMaxFileIdStr << _max_file_id
			<< arenaBuyTimesStr << _buy_times << arenaAllTimesStr << _all_times << arenaNextLuckyUpdateTimeStr << _next_lucky_update_time
			<< arenaLuckyRewardStr << _lucky_reward << arenaLuckyRankStr << _last_lucky_rank;
		{
			mongo::BSONArrayBuilder array_arena;
			for (list<Json::Value>::iterator it = _arena_report_list.begin(); it != _arena_report_list.end(); ++it)
			{
				Json::Value& ref_report = *it;
				mongo::BSONObjBuilder b;
				b << arenaReportIdStr << ref_report[0u].asString() << arenaSideStr << ref_report[1u].asBool()
					<< arenaCountryStr << ref_report[2u].asInt() << arenaNickNameStr << ref_report[3u].asString()
					<< arenaResultStr << ref_report[4u].asInt() << arenaOldRankStr << ref_report[5u].asInt()
					<< arenaNewRankStr << ref_report[6u].asInt() << arenaTimeStr << ref_report[7u].asInt()
					<< arenaRewardStr << ref_report[8u].asInt() << arena_level_field_str << ref_report[9u].asInt();
				array_arena.append(b.obj());
			}
			obj << arenaRecordsStr << array_arena.arr();
		}
		return db_mgr.save_mongo(arenaDBStr, key, obj.obj());
	}


	void playerArena::initData(int rank)
	{
		string pathname = reportArenaPVPDir;
		pathname += boost::lexical_cast<string , int>(own.playerID);
		boost::filesystem::path path_dir(pathname);
		if(!boost::filesystem::exists(path_dir))
			boost::filesystem::create_directory(path_dir);

		_rank = rank;
		guild_sys.asyncPlayerData(own.getOwnDataPtr());
		_remain_times = 15;

		unsigned now = na::time_helper::get_current_time();
		_next_update_time = na::time_helper::get_next_update_time(now);
		_next_lucky_update_time = na::time_helper::get_next_update_time(now, 22);
		_next_year_time = season_sys.getNextSeasonTime(season_quarter_one, 22);
		helper_mgr.insertSaveAndUpdate(this);
	}

	bool playerArena::checkEnemyPid(int pid)
	{
		for(vector<int>::iterator iter = _enemy_pid_list.begin(); iter != _enemy_pid_list.end(); ++iter)
		{
			if(*iter == pid)
				return true;
		}
		return false;
	}

	void playerArena::alterCd()
	{
		_next_challenge_time = na::time_helper::get_current_time() + 3 * 60;
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerArena::updateAfterActiveBattle(Json::Value& info, int rank)
	{
		++_total_times;
		--_remain_times;
		addAllTimes();
		//_next_challenge_time = na::time_helper::get_current_time() + 5 * 60;
		if(_rank != rank)
		{
			_rank = rank;
			guild_sys.asyncPlayerData(own.getOwnDataPtr());
		}

		_arena_report_list.push_back(info);

		while(_arena_report_list.size() > 10)
			_arena_report_list.pop_front();

		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerArena::updateAfterPassiveBattle(Json::Value& info, int rank)
	{
		if(_rank != rank)
		{
			_rank = rank;
			guild_sys.asyncPlayerData(own.getOwnDataPtr());
		}

		_arena_report_list.push_back(info);

		while(_arena_report_list.size() > 10)
			_arena_report_list.pop_front();

		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerArena::clearCd()
	{
		_next_challenge_time = 0;
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerArena::updateAfterGetReward(int type)
	{
		if(type == 0)
			_have_got_ten_challenge_reward = true;
		else
			_have_got_year_reward = true;

		helper_mgr.insertSaveAndUpdate(this);

		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_arena_reward_times);
	}

	string playerArena::getFileName()
	{
		if(++_max_file_id > 10)
			_max_file_id = 1;

		helper_mgr.insertSaveAndUpdate(this);

		string str = boost::lexical_cast<string , int>(own.playerID);
		str += "/";
		str += boost::lexical_cast<string , unsigned>(_max_file_id);
		return str;
	}

	unsigned playerArena::getBuyCost()
	{
		unsigned cost = 5 + _buy_times;
		return cost > 100? 100 : cost;
	}

	void playerArena::updateAfterBuyTimes()
	{
		_buy_times += 1;
		_remain_times += 1;

		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerArena::addAllTimes()
	{
		++_all_times;
		helper_mgr.insertSaveAndUpdate(this);

		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_arena_battle_times);
	}

	bool playerArena::isLuckyReward()
	{
		int rank = arena_sys.inLuckyRank(own.getOwnDataPtr());
		if (rank == -1)
			return true;

		return _lucky_reward;
	}

	void playerArena::doLuckyReward()
	{
		_lucky_reward = true;

		int gold_before = own.Base.getAllGold();
		int rank = arena_sys.inLuckyRank(own.getOwnDataPtr());
		int lucky_gold = arena_sys.getLuckyReward(rank);
		own.Base.alterBothGold(lucky_gold);
		int gold_after = own.Base.getAllGold();

		LogTemplate(arenaLogDBStr, own.getOwnDataPtr(), arena_system::_lucky_reward_log, gold_before, gold_after, _last_lucky_rank, lucky_gold);

		helper_mgr.insertSaveAndUpdate(this);
	}
}
