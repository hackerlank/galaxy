#pragma once

#include <vector>
#include <list>
#include <deque>
#include "json/json.h"
#include "msg_base.h"
#include "playerManager.h"
#include "reward.h"
#include "season_system.h"
#include "action_format.hpp"
#include "war_story.h"
#include "starwar_base.h"
using namespace std;

#define arena_sys (*gg::arena_system::arenaSys)

namespace gg
{
	enum{
		_arena_alter_name = 0,
		_arena_alter_lv,
		_arena_alter_face_id
	};


	class hero_info
	{
	public:
		hero_info() : _player_id(-1), _country_id(-1), _nickname(""), _lv(0), _face_id(-1), _power(-1){}
		hero_info(int player_id, int country_id, const string& nickname, int lv, int face_id, int power)
			: _player_id(player_id), _country_id(country_id), _nickname(nickname), _lv(lv), _face_id(face_id), _power(power){}

		int get_player_id(){ return _player_id; }
		int get_country_id(){ return _country_id; }
		string get_nickname(){ return _nickname; }
		int get_lv(){ return _lv; }
		int get_face_id(){ return _face_id; }
		int get_power(){ return _power; }

		void setFromBSON(const mongo::BSONObj& obj);

		void reset(int player_id, const string& nickname, int country_id, int lv, int face_id, int power)
		{
			_player_id = player_id;
			_nickname = nickname;
			_country_id = country_id;
			_lv = lv;
			_face_id = face_id;
			_power = power;
		}

		void resetPower(int power)
		{
			_power = power;
		}

	private:
		int _player_id;
		int _country_id;
		int _lv;
		int _face_id;
		string _nickname;
		int _power;
	};

	struct arena_helper
	{
	public:
		struct mem
		{
		public:
			mem() : _player_id(-1), _nickname(""), _country_id(-1){}
			int _player_id;
			string _nickname;
			int _country_id;
		};

		bool operator<(const arena_helper& rhs) const
		{
			return _year < rhs._year;
		}

		void setFromBSON(const mongo::BSONObj& obj);

		Json::Value getValue(int term_id) const
		{
			Json::Value info;
			info.append(term_id);
			for(unsigned i = 0; i < 3; ++i)
			{
				info.append(_mems[i]._player_id);
				info.append(_mems[i]._nickname);
				info.append(_mems[i]._country_id);
			}
			info.append(_year);
			return info;
		}

		unsigned _year;
		mem _mems[3];
	};

	struct lucky_helper
	{
		public:
			lucky_helper(int index, int old_rank, int player_id, const string& name, int rank)
				: _index(index), _old_rank(old_rank), _player_id(player_id), _name(name), _rank(rank){}

			bool operator<(const lucky_helper& rhs) const
			{
				return _index < rhs._index;
			}
			int _index;
			int _old_rank;
			int _player_id;
			string _name;
			int _rank;
	};

	const static string arena_ranking_list_db_str = "gl.arena_ranking_list";
	const static string arena_old_ranking_list_db_str = "gl.arena_old_ranking_list";
	const static string arena_famous_person_list_db_str = "gl.arena_famous_person_list";
	const static string arena_lucky_index_list_db_str = "gl.arena_lucky_index_list";

	const static string arena_rank_field_str = "rank";
	const static string arena_player_id_field_str = "pid";
	const static string arena_nickname_field_str = "nn";
	const static string arena_face_id_field_str = "fi";
	const static string arena_power_field_str = "pow";
	const static string arena_country_id_field_str = "cty";
	const static string arena_level_field_str = "lv";
	const static string arena_year_field_str = "yr";
	const static string arena_famous_person_list_field_str = "fpr";
	const static string arena_famous_person_list_count_field_str = "fprc";
	const static string arena_enemy_list_field_str = "el";
	const static string arena_ranking_list_field_str = "hr";
	const static string arena_reward_type_str = "rt";
	const static string arena_index_field_str = "ind";
	const static string arena_time_field_str = "tm";

	const static string arenaLogDBStr = "log_arena";

	const static string reportArenaPVPDir = "./report/engross/ArenaPVP/";

	class arena_system
	{
	public:
		const static string arenaYearRewardDirStr1;
		const static string arenaYearRewardDirStr2;
		const static string arenaYearRewardDirStr3;
		const static string arenaTenFightRewardDirStr;
		const static string arenaLuckyRewardDirStr;
		const static string arenaNpcInfoDirStr;
		static arena_system* const arenaSys;

		enum{
			_challenge_log = 1,
			_clear_cd_log,
			_year_reward_log,
			_ten_reward_log,
			_buy_times_log,
			_ranking_list_log,
			_lucky_reward_log,
		};

		enum{
			_max_num_of_enemy = 5,

			_challenge_reward_times = 10,

			_max_num_of_ranking_list = 20,
			_max_num_of_famous_person_list = 3,
			_max_num_of_famous_person_list_count = 100,

			_lv_limit = 12,

			_win_silver = 3000,
			_lose_silver = 1500,
		};

		enum 
		{
			_broadcast_at_21_45 = 0,           // 15分钟后 竞技场排名奖励结算！名次越前，奖励越多！
			_broadcast_at_21_50,                  // 10分钟后 竞技场排名奖励结算，请各位总督记得前往争逐排名，获得更丰富的奖励。
			_broadcast_at_21_55                   // 5分钟后 竞技场排名奖励结算，请各位总督迅速前往，争逐排名，排名越前，奖励越丰富。
		};

		arena_system();

		void arenaMainUpdateReq(msg_json& m, Json::Value& r);
		void arenaHeroRankNewUpdateReq(msg_json& m, Json::Value& r);
		void arenaHeroRankOldUpdateReq(msg_json& m, Json::Value& r);
		void arenaFamousPersonRankUpdateReq(msg_json& m, Json::Value& r);
		void arenaChallegeReq(msg_json& m, Json::Value& r);
		void arenaGetRewardReq(msg_json& m, Json::Value& r);
		void arenaGetYearRewardReq(msg_json& m, Json::Value& r);
		void arenaClearCdReq(msg_json& m, Json::Value& r);
		void arenaChallegeResp(const int battleResult, Battlefield field);
		void arenaBuyChallegeTimes(msg_json& m, Json::Value& r);
		void getAutoTitledPlayers(int country_id, Json::Value& players);
		void arenaNewLuckyRankListReq(msg_json& m, Json::Value& r);
		void arenaOldLuckyRankListReq(msg_json& m, Json::Value& r);
		void arenaGetLuckyRewardReq(msg_json& m, Json::Value& r);

		void arenaFakeChallegeReq(msg_json& m, Json::Value& r);
		void arenaFakeChallegeResp(const int battleResult, Battlefield field);

		void arenaSummaryBroadcastInnerReq(msg_json& m, Json::Value& r);

		void initData();

		unsigned getArenaEndTime(){return _next_year_time;}
		int getArenaSeason(){return season_quarter_one;}

		void updatePlayerInfo(playerDataPtr d, int type);
		void checkAndUpdate(boost::system_time& tmp);
		void doCheckAndUpdate(unsigned now = na::time_helper::get_current_time());

		void updatePower(playerDataPtr d);

		vector<reward>& getYearRewards();
		bool isOldLuckyIndex(int rank)
		{
			if (rank > (int)_old_lucky_index.size() || rank < 1)
				return false;
			return _old_lucky_index[rank - 1] == 1;
		}

		bool isNewLuckyIndex(int rank)
		{
			if (rank >(int)_lucky_index.size() || rank < 1)
				return false;
			return _lucky_index[rank - 1] == 1;
		}

		void loadNpcInfo();
		int getLuckyReward(int rank);

		void luckyRankingBroadcast();
		void summaryComming(int type);
		vector<int> getArenaRankList(int startRank, int endRank);
		int inLuckyRank(playerDataPtr d);
		unsigned nextYearTime() const { return _next_year_time; }
		unsigned nextLuckyTime() const { return _next_lucky_update_time; }

	private:
		void updateFakeToPlayer(playerDataPtr d);
		void updateToPlayer(playerDataPtr d, int pre_rank = -1);
		void loadFile();
		void package(playerDataPtr& d, Json::Value& info);
		void getEnemyList(playerDataPtr& d, Json::Value& msg);
		void sendReportToAll(Battlefield field, playerDataPtr d, playerDataPtr targetP);

		void updateRankingListAfterBattle(int result, playerDataPtr d, playerDataPtr targetP, bool flag);
		int getNewPlayerRank(playerDataPtr d);
		void updateArenaRankListDB(playerDataPtr d, int rank);
		void updateLuckyListDB();

		void packageEnemyInfo(Json::Value& info, int player_id, const string& name, int lv , int rank, int face_id, int country_id, int power);
		void packageHeroInfo(Json::Value& info, int player_id, const string& name, int country_id, int lv, int face_id);
		void packageReportInfo(Json::Value& info, const string& file, bool side, int country_id, string& name, int result, int old_rank, int rank, int time, int reward, int lv);

		void updateArenaOldRankingList();
		void updateFamousPersonList(int year);

		void loadRankingListDB();
		void loadOldRankingListDB();
		void loadFamousPersonListDB();
		void loadLuckyIndexDB();

		void getNewLuckyIndex(vector<int>& lucky_index);

		void saveFamousPersonList(Json::Value& info);
		void alterFamousPersonList(playerDataPtr d);
		void alterOldRankingList(playerDataPtr d);

		void sendRankingListLog();
		void setYearStamp(unsigned t);
		int getRewardType(){ return _reward_type; }


	private:
		string _npcName[5];

		set<arena_helper> _helper_set;

		typedef vector<hero_info> HeroInfos;
		HeroInfos _hero_infos;
		deque<Json::Value> _famous_person_infos_list;
		Json::Value _last_term_hero_infos;
		int _max_term_id;

		unsigned _next_year_time;
		unsigned _year_stamp;

		vector<reward> _rewards[3];
		vector<reward> _ten_fight_rewards;
		boost::system_time tick;
		bool _bInit;
		int _reward_type;

		mapDataPtr _npcPtr;
		vector<int> _lucky_index;
		vector<int> _old_lucky_index;
		vector<Json::Value> _lucky_ranking_list;
		vector<Json::Value> _old_ranking_list;
		typedef map<int, int> PlayerId2Rank;
		PlayerId2Rank _playerId2Rank;
		unsigned _next_lucky_update_time;

		int _lucky_reward[3];
		//int _rate;
	};

}
