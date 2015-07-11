#pragma once

#include <string>
#include <list>
#include "block.h"
#include "reward.h"


using namespace std;

namespace gg
{

	const static string arenaDBStr = "gl.player_arena";
	const static string arenaRankStr = "rk";
	const static string arenaRecordsStr = "rds";
	const static string arenaCdStr = "cd";
	const static string arenaOldRankStr = "ork";
	const static string arenaRemainTimesStr = "rts";
	const static string arenaTotalTimesStr = "tts";
	const static string arenaHaveGotYearRewardStr = "gyr";
	const static string arenaHaveGotTenChallengeRewardStr = "gtcr";
	const static string arenaNextUpdateTimeStr = "nut";
	const static string arenaNextYearTimeStr = "nyt";

	const static string arenaReportIdStr = "ri";
	const static string arenaResultStr = "rst";
	const static string arenaCountryStr = "cty";
	const static string arenaTimeStr = "tm";
	const static string arenaSideStr = "sd";
	const static string arenaNickNameStr = "nn";
	const static string arenaLevelStr = "lv";
	const static string arenaNewRankStr = "nr";
	const static string arenaMaxFileIdStr = "mfi";
	const static string arenaRewardStr = "rwd";
	const static string arenaBuyTimesStr = "bt";
	const static string arenaAllTimesStr = "at";
	const static string arenaNextLuckyUpdateTimeStr = "nlut";
	const static string arenaLuckyRewardStr = "lr";
	const static string arenaLuckyRankStr = "lrk";

	class playerArena : public Block
	{
	public:
		playerArena(playerData& own);
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate();

		void initData(int rank);
		void package(Json::Value& msg);
		void checkAndUpdate(unsigned now = na::time_helper::get_current_time());

		void addEnemyPid(int pid){ _enemy_pid_list.push_back(pid); }

		bool checkEnemyPid(int pid);
		bool checkRemainTime(){ return _remain_times > 0; }
		bool checkInCd(unsigned now){ return now < _next_challenge_time; }

		int getClearCdCost(unsigned now){ return ((_next_challenge_time - now) / 60 + 1) * 2; }
		string getFileName();
		unsigned getBuyCost();
		void updateAfterActiveBattle(Json::Value& info, int rank);
		void updateAfterPassiveBattle(Json::Value& info, int rank);
		void alterCd();
		void updateAfterGetReward(int type);
		void updateAfterBuyTimes();
		void clearCd();
		int getRank(){ return _rank; }
		int getOldRank(){ return _old_rank; }
		int getTotalTimes(){ return _total_times; }
		bool getBTenChallengeReward(){ return _have_got_ten_challenge_reward; }
		bool getBYearChallenge(){ return _have_got_year_reward; }
		void clearEnemyPidList(){ _enemy_pid_list.clear(); }
		unsigned getBuyTimes(){ return _buy_times; }

		bool isLuckyReward();
		void doLuckyReward();
		int getLastLuckyRank() const { return _last_lucky_rank; }

		int getAllTimes(){ return _all_times; }

		void addAllTimes();

		void setNpcPower(int power){ _power = power; }
		int getNpcPower() const { return _power; }

	private:
		int _rank;
		int _old_rank;
		int _total_times;
		int _remain_times;
		int _all_times;
		unsigned _max_file_id;
		unsigned _buy_times;
		unsigned _next_challenge_time;
		unsigned _next_update_time;
		unsigned _next_year_time;
		bool _have_got_year_reward;
		bool _have_got_ten_challenge_reward;
		bool _check_path_exist;


		list<Json::Value> _arena_report_list;
		vector<int> _enemy_pid_list;

		int _power;
		unsigned _next_lucky_update_time;
		bool _lucky_reward;
		int _last_lucky_rank;
	};

}