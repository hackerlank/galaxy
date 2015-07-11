#pragma once

#include "ruler_helper.h"
#include "battle_system.h"
#include "season_system.h"

#define ruler_sys (*gg::ruler_system::rulerSys)

namespace gg
{
	enum{                                         // 投注限定额: 
		_betting_cost_one = 0,       // 开服3个月内
		_betting_cost_two,              // 开服6个月内
		_betting_cost_three,           // 开服6个月以上
	};

	enum{
		_time_at_14_00 = 0,
		_time_at_16_00,
		_time_at_18_00,
		_time_at_21_00,
		_time_at_max,
	};

	static string const reportRulerPVPDir = "./report/engross/rulerPVP/";
	const static string ruler_betting_cost_type_field_str = "bnt";
	const static string ruler_next_flush_time_field_str = "nft";
	const static string ruler_client_state_field_str = "st";

	const static string rulerLogDBStr = "log_ruler";

	typedef vector<rulerInfo> rulerInfos;



	class ruler_system
	{
	public:
		enum{
			_betting_log = 1,
			_betting_reward_log,
			_challenge_log,
			_final_battle_log,
			_set_title_log,
			_auto_set_title_log,
		};

		enum 
		{
			_broadcast_at_18_00 = 0,
			_broadcast_at_21_25,
			_broadcast_at_21_30,
			_broadcast_at_21_35
		};

		ruler_system();
		static ruler_system* const rulerSys;
		
		void rulerUpdateReq(msg_json& m, Json::Value& r);
		void rulerTermInfoReq(msg_json& m, Json::Value& r);
		void rulerChallengeReq(msg_json& m, Json::Value& r);
		void rulerChallengeResp(const int battleResult, Battlefield field);
		void rulerTitleInfoReq(msg_json& m, Json::Value& r);
		void rulerSetTitleReq(msg_json& m, Json::Value& r);
		void rulerBettingReq(msg_json& m, Json::Value& r);
		void rulerGetRewardReq(msg_json& m, Json::Value& r);
		void rulerClearCdReq(msg_json& m, Json::Value& r);

		void rulerAutoSetTitleInnerReq(msg_json& m, Json::Value& r);
		void rulerFinalBattleInnerReq(msg_json& m, Json::Value& r);
		void rulerBroadcastInnerReq(msg_json& m, Json::Value& r);

		void rulerAutoSetTitleResp(playerDataPtr d);

		int getPlayerByTitle(int title, int country_id);
		int getWinnerSide(int country_id);

		void rulerFinalBattleResp(const int battleResult, Battlefield field);

		void initData();
		void checkAndUpdate(boost::system_time& tmp);
		bool isFinished(int country_id);
		int getFinalBattlePlayer(int country_id, int side);

		int getClientState(int country_id){ if(country_id>=0&&country_id<=2)return _rulerInfos[country_id].getClientState();return -1;}
		unsigned getRulerTime()
		{
			if(_next_time_node == _time_14_00)
				return _next_update_time;
			return 0;
		}
		unsigned getNextBettingTime() const
		{
			if (_next_time_node == _time_14_00)
			{
				unsigned now = na::time_helper::get_current_time();
				struct tm t = na::time_helper::toTm(now);
				unsigned secs = t.tm_hour * na::time_helper::HOUR + t.tm_min * na::time_helper::MINUTE + t.tm_sec;
				if (secs < 5 * na::time_helper::HOUR || secs >= 21 * na::time_helper::HOUR + 50 * na::time_helper::MINUTE)
					return _next_update_time + na::time_helper::HOUR * 7 + na::time_helper::MINUTE * 30 - 4 * na::time_helper::DAY;
				else
					return _next_update_time + na::time_helper::HOUR * 7 + na::time_helper::MINUTE * 30;
			}

			if (_next_time_node == _time_21_25)
				return _next_update_time + na::time_helper::MINUTE * 5;
			if (_next_time_node == _time_21_30)
				return _next_update_time;
			if (_next_time_node == _time_21_40)
				return _next_update_time - na::time_helper::MINUTE * 10;
			if (_next_time_node == _time_21_45)
				return _next_update_time + na::time_helper::DAY * 4 - na::time_helper::MINUTE * 15;
			if (_next_time_node == _time_21_50)
				return _next_update_time + na::time_helper::DAY * 4 - na::time_helper::MINUTE * 20;
			return 0;
		}

		int getRulerSeason()
		{
			return season_quarter_one;
		}

		void postBroadcastReq(int type, int a_player_id, int b_player_id, int country_id, string report_id, int title = -1);

		void updatePlayerInfo(playerDataPtr d, int type);

		void rulerBroadcastTestReq(msg_json& m, Json::Value& r);

		void getValue(Json::Value& info, int country_id = -1);
		void getOfficerVec(set<int>& officer_vec, int country_id);

		const rulerTitleInfoList& getTitleInfoList(int country_id) const { return _rulerInfos[country_id].getTitleInfoList(); }

		void rulerBroadcast(int type);

	private:
		void doBroadcast();
		void initBroadcast();

		void initTimeNode();
		void initTimeDiffs();
		void loadRulerReportListDB();
		void loadRulerInfoListDB();
		void loadRulerCurrentInfoDB();
		void loadRulerTitleInfoListDB();
		void createReportDir();
		void updateTimeNode();
		int getBettingCostType(unsigned now);

		void package(playerDataPtr& d, Json::Value& pack);
		void updateToPlayer(playerDataPtr& d);
		void updateTitleInfoToPlayer(playerDataPtr& d);

		int getBetNumber(int option);
		void updateAfterBattle(playerDataPtr& d, playerDataPtr& targetP, const int battleResult, Battlefield field);
		string getFileName(int country_id);

		void postToAutoSetTitle(playerDataPtr d);

		unsigned _next_update_time;
		int _next_time_node;
		int _betting_cost_type;

		rulerInfos _rulerInfos;
		unsigned _time_diff[_time_max];
		boost::system_time tick;

		unsigned _next_broadcast_time;
		int _next_broadcast_state;

		bool _bInit;
	};
}