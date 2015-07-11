#pragma once

#include "block.h"

namespace gg
{
	const static string rulerDBStr = "gl.player_ruler";

	const static string ruler_next_update_time_field_str = "nut";
	const static string ruler_title_field_str = "ttl";
	const static string ruler_cd_field_str = "cd";
	const static string ruler_bet_option_field_str = "bo";
	const static string ruler_bet_number_field_str = "bn";
	const static string ruler_reward_type_field_str = "rt";
	const static string ruler_have_got_reward_field_str = "ir";
	const static string ruler_battle_times_field_str = "bat";

	enum
	{
		_no_title = 0,
		_title_king,                           // 元首
		_title_councilor,                  // 议会长
		_title_consul,                       // 执政官
		_title_affairs_minister,       // 事务大臣
		_title_justice_minister,      // 司法大臣
		_title_defense_minister,   // 国防大臣
		_title_building_envoy,      // 营造特使
		_title_traffic_envoy,           // 交通特使
		_title_security_envoy,       // 安全特使
		_title_religion_envoy,       // 宗教特使
		_title_economy_envoy,    // 经济特使
		_title_culture_envoy,        // 文化特使

		_title_max,
	};

	enum
	{
		_no_bet = 0,
		_bet_left_side_one,
		_bet_left_side_two,
		_bet_right_side_one,
		_bet_right_side_two,
	};

	enum
	{
		_no_reward = 0,
		_bet_lose_reward,
		_bet_win_reward,
		_winner_reward,
		_loser_reward,
	};

	class playerRuler : public Block
	{
	public:
		playerRuler(playerData& own) : Block(own), _next_update_time(0), _next_challenge_time(0), _title(_no_title), _reward_type(gg::_no_reward)
			, _bet_option(_no_bet), _bet_number(0), _have_got_reward(false), _battle_times(0), _last_report_time(na::time_helper::get_current_time())
		{
			
		}
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate(){}
		void package(Json::Value& pack);
		void checkAndUpdate(unsigned now);
		void setTitle(int title);
		bool checkCd(unsigned now){ return now >= _next_challenge_time; }
		void updateAfterBattle(unsigned now);
		int getTitle();
		int getBetOption(){ return _bet_option; }
		void setBetOption(int option);

		int setBetNumber(){ return _bet_number; }
		void setBetNumber(int number);

		int getRewardType();

		int getReward(int type);

		void setRewardType(int type);
		int clearCd();

		int getBattleTimes(){ return _battle_times; }
		void addBattleTimes();

		unsigned getLastReportTime(){ return _last_report_time; }
		void setLastReportTime(unsigned t){ _last_report_time = t; }

	private:
		int _title;
		unsigned _next_challenge_time;
		unsigned _next_update_time;
		unsigned _bet_number;
		unsigned _battle_times;
		int _bet_option;
		int _reward_type;
		bool _have_got_reward;
		unsigned _last_report_time;
	};
}
