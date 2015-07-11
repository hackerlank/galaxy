#pragma once

#include <string>
#include <list>
#include <boost/unordered_map.hpp>
#include "playerManager.h"
#include "mongoDB.h"
#include "json/json.h"
#include "msg_base.h"

using namespace std;
using namespace boost;

#define strategy_sys (*gg::strategy_system::strategySys)

namespace gg
{
	enum{
		_strategy_last_pass_num = 5,
	};

	enum{
		_strategy_first_pass_report_id = 0,
		_strategy_best_pass_report_id,
		_strategy_last_pass_report_id_begin,
		_strategy_last_pass_report_id_end = _strategy_last_pass_report_id_begin + _strategy_last_pass_num,
	};

	const static string strategy_info_db_str = "gl.strategy_info";
	const static string reportStrategyPVEDir = "./report/engross/strategyPVE/";

	const static string strategy_local_id_field_str = "id";
	const static string strategy_current_id_field_str = "ci";

	const static string strategy_first_pass_player_field_str = "fp";
	const static string strategy_best_pass_player_field_str = "bp";
	const static string strategy_last_pass_player_field_str = "lp";

	const static string strategy_player_id_field_str = "pi";
	const static string strategy_nickname_field_str = "nn";
	const static string strategy_level_field_str = "lv";
	const static string strategy_country_id_field_str = "ci";
	const static string strategy_report_id_field_str = "ri";
	
	const static string strategy_round_field_str = "rd";
	const static string strategy_lost_army_field_str = "la";
	const static string strategy_time_field_str = "tm";

	class strategyPlayerInfo
	{
	public:
		strategyPlayerInfo() : _player_id(-1), _nickname(""), _country_id(-1), _lv(0), _report_id(""){}

		bool isEmpty(){ return _player_id == -1; }

		void set(playerDataPtr d, const string& report);
		void set(mongo::BSONElement& obj);
		void package(Json::Value& info);
		mongo::BSONObj toBSON();

		int getPlayerId(){ return _player_id; }
		string getNickname(){ return _nickname; }
		int getCountryId(){ return _country_id; }
		int getLv(){ return _lv; }
		string getReportId(){ return _report_id; }

	private:
		int _player_id;
		string _nickname;
		int _country_id;
		int _lv;
		string _report_id;
	};

	class firstStrategyPlayerInfo : public strategyPlayerInfo
	{
	public:
		firstStrategyPlayerInfo() : _time(0){}
		void set(playerDataPtr d, const string& report);
		void set(mongo::BSONElement obj);
		void package(Json::Value& info);
		mongo::BSONObj toBSON();
		unsigned getTime(){ return _time; }

	private:
		unsigned _time;
	};

	class bestStrategyPlayerInfo : public firstStrategyPlayerInfo
	{
	public:
		bestStrategyPlayerInfo() : _round(0), _lost(0){}
		bool isBetter(playerDataPtr d, Battlefield field, Json::Value& report);
		void set(playerDataPtr d, const string& report, Battlefield field, Json::Value& reportJson);
		void set(mongo::BSONElement obj);
		mongo::BSONObj toBSON();

	private:
		int _round;
		int _lost;
	};

	class strategyPlayerInfos
	{
	public:
		strategyPlayerInfos(int local_id);

		bool save();
		void setFromBSON(mongo::BSONObj& obj);
		void package(Json::Value& info);
		bool updateFirstPassPlayer(playerDataPtr d, Battlefield field, Json::Value& report);
		bool updateBestPassPlayer(playerDataPtr d, Battlefield field, Json::Value& report);
		bool updateLastPassPlayer(playerDataPtr d, Battlefield field, Json::Value& report);

	private:
		static bool initIndex();

		int _local_id;
		int _current_id;
		firstStrategyPlayerInfo _first_pass_player;
		bestStrategyPlayerInfo _best_pass_player;
		list<strategyPlayerInfo> _last_pass_players;
		static string _index[_strategy_last_pass_report_id_end];
	};

	typedef unordered_map<int, strategyPlayerInfos> LocalId2PlayerInfos;

	class strategy_system
	{
	public:
		static strategy_system* const strategySys;
		strategy_system(){}
		void strategyUpdateReq(msg_json& m, Json::Value& r);
		void updateInfo(playerDataPtr d, Battlefield field, Json::Value& report, int type);
		void initData();

	private:
		LocalId2PlayerInfos _maps;
	};
}