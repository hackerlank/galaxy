#pragma once

#include <string>
#include "json/json.h"
#include "mongoDB.h"
#include "playerManager.h"
#include "season_system.h"
#include "starwar_base.h"

using namespace std;

namespace gg
{
	enum{
		_ruler_alter_name = 0,
		_ruler_alter_lv,
		_ruler_alter_face_id,
	};

	enum{
		DAY = 24 * 60 * 60,
		HOUR = 60 * 60,
		MINUTE = 60,
		SECOND = 1,
	};

	const static string ruler_left_side_player_field_str = "lp";
	const static string ruler_right_side_player_field_str = "rp";

	enum{
		_time_14_00 = 0,
		_time_21_25,
		_time_21_30,
		_time_21_40,
		_time_21_45,
		_time_21_50,
		_time_max
	};
	
	enum{                                          // 战报类型
		_report_left_battle = 0,        // 左储君战报           
		_report_right_battle,            // 右储君战报
		_report_final_one,                 // 决赛第一场
		_report_final_two,                 // 决赛第二场
		_report_final_three,              // 决赛第三场
		_report_not_set
	};

	enum{                                                          // 客户端状态
		_client_state_candidate_battle = 0,   // 竞争候选人时间
		_client_state_betting_time,                 // 投注时间                
		_client_state_final_battle,                    // 决赛时间
		_client_state_no_battle,                       // 非元首争霸战时间
		_client_state_not_set,                 
	};

	enum{                                                       // 元首争霸战广播类型
		_ruler_broadcast_type_0 = 0,          // [国家] AAA险胜BBB，夺得候选人位
		_ruler_broadcast_type_1,                 // [世界] 元首争霸战“候选人争夺”在5分钟后结束
		_ruler_broadcast_type_2,                 // [世界] 即将进行候选人决战，可前往元首争霸战界面竞猜结果，赢取海量奖励！
		_ruler_broadcast_type_3,                 // [世界] AAA无对手可战，自动加冕为X元首
		_ruler_broadcast_type_4,                 // [世界] X后继无人，AAA自动连任元首
		_ruler_broadcast_type_5,                 // [世界] X一片混乱，至今仍未诞生元首
		_ruler_broadcast_type_6,                 // [世界] AAA旗开得胜，击败了BBB，取得了X元首争霸的第一场胜利。[战报]
		_ruler_broadcast_type_7,                 // [世界] AAA势不可挡，2:0战胜BBB夺得X元首宝座。[战报]
		_ruler_broadcast_type_8,                 // [世界] BBB力挽狂澜，击败了AAA，在X元首争霸赛中扳回一局。[战报]
		_ruler_broadcast_type_9,                 // [世界] AAA势不可挡，2:1战胜BBB夺得X元首宝座。[战报]
		_ruler_broadcast_type_10,               // [国家] AAA通过实力获得了国王的封赏成为YYY，众望所归！
		_ruler_broadcast_type_11,               // [世界] 元首争霸赛候选人争夺战已经开始，欢迎各大总督参加候选！[进入]
	};

	struct rulerBroadcastPlayer
	{
	public:
		rulerBroadcastPlayer() : _player_id(-1){}
		rulerBroadcastPlayer(int player_id, string nickname) : _player_id(player_id), _nickname(nickname){}

		int _player_id;
		string _nickname;
	};

	const static string ruler_broadcast_type_field_str = "rbt";
	

	const static string ruler_player_id_field_str = "pi";
	const static string ruler_nickname_field_str = "nn";


	class rulerPlayerInfo
	{
	public:
		rulerPlayerInfo() : _player_id(-1), _nickname(""){}
		void setPlayerId(int player_id){ _player_id = player_id; }
		void setNickname(const string& nickname){ _nickname = nickname; }
		int getPlayerId() const { return _player_id; }
		string getNickName() const { return _nickname; }
		void reset(){ _player_id = -1; _nickname = ""; }

		void setFromBSONObj(const mongo::BSONObj& obj)
		{
			checkNotEoo(obj[ruler_player_id_field_str])
				_player_id = obj[ruler_player_id_field_str].Int();
			checkNotEoo(obj[ruler_nickname_field_str])
				_nickname = obj[ruler_nickname_field_str].String();
		}
		void setFromBSONElement(const mongo::BSONElement& obj)
		{
			checkNotEoo(obj[ruler_player_id_field_str])
				_player_id = obj[ruler_player_id_field_str].Int();
			checkNotEoo(obj[ruler_nickname_field_str])
				_nickname = obj[ruler_nickname_field_str].String();
		}
		void package(Json::Value& info)
		{
			info[ruler_player_id_field_str] = _player_id;
			info[ruler_nickname_field_str] = _nickname;
		}

	private:
		int _player_id;
		string _nickname;
	};

	const static string ruler_title_info_list_db_str = "gl.ruler_title_info_list";
	const static string ruler_face_id_field_str = "fi";

	class rulerTitleInfo : public rulerPlayerInfo
	{
	public:
		rulerTitleInfo(int title) : _title(title), _face_id(-1){}
		void package(Json::Value& info) const
		{
			info = Json::arrayValue;
			info.append(_title);
			info.append(getPlayerId());
			info.append(getNickName());
			info.append(_face_id);
		}
		void setFromBSONObj(const mongo::BSONObj& obj)
		{
			rulerPlayerInfo::setFromBSONObj(obj);
			checkNotEoo(obj[ruler_face_id_field_str])
				_face_id = obj[ruler_face_id_field_str].Int();
		}
		void setFromBSONElement(const mongo::BSONElement& obj)
		{
			rulerPlayerInfo::setFromBSONElement(obj);
			checkNotEoo(obj[ruler_face_id_field_str])
				_face_id = obj[ruler_face_id_field_str].Int();
		}
		mongo::BSONObj toBSON() const
		{
			return BSON(ruler_title_field_str << _title
				<< ruler_player_id_field_str << getPlayerId()
				<< ruler_nickname_field_str << getNickName()
				<< ruler_face_id_field_str << _face_id);
		}

		bool save(int country_id)
		{
			mongo::BSONObj key = BSON(ruler_title_field_str << (_title + country_id * 10000));

			mongo::BSONObj obj = BSON(ruler_title_field_str << (_title + country_id * 10000)
				<< ruler_player_id_field_str << getPlayerId()
				<< ruler_nickname_field_str << getNickName()
				<< ruler_face_id_field_str << _face_id);

			return db_mgr.save_mongo(ruler_title_info_list_db_str, key, obj);
		}
		int getTitle() const { return _title; }
		bool checkTitle()
		{
			return getPlayerId() != -1;
		}
		void setTitle(int player_id, const string& nickname, int country_id, int face_id)
		{
			setPlayerId(player_id);
			setNickname(nickname);
			_face_id = face_id;
			save(country_id);
		}
		void resetAndSave(int country_id)
		{
			rulerPlayerInfo::reset();
			_face_id = -1;
			save(country_id);
		}
		void setFaceId(int face_id){ _face_id = face_id; }

	private:
		int _title;
		int _face_id;
	};

	const static string ruler_level_field_str = "lv";


	class rulerKingInfo : public rulerPlayerInfo
	{
	public:
		rulerKingInfo() : _lv(0), _face_id(-1){}

		void setLv(int lv){ _lv = lv; }
		void setFaceId(int face_id){ _face_id = face_id; }
		int getLv() const { return _lv; }
		int getFaceId() const { return _face_id; }
		void reset()
		{
			rulerPlayerInfo::reset();
			_lv = 0;
			_face_id = -1;
		}

		void setFromBSONObj(const mongo::BSONObj& obj)
		{
			rulerPlayerInfo::setFromBSONObj(obj);
			checkNotEoo(obj[ruler_level_field_str])
				_lv = obj[ruler_level_field_str].Int();
			checkNotEoo(obj[ruler_face_id_field_str])
				_face_id = obj[ruler_face_id_field_str].Int();
		}

		void setFromBSONElement(const mongo::BSONElement& obj)
		{
			rulerPlayerInfo::setFromBSONElement(obj);
			checkNotEoo(obj[ruler_level_field_str])
				_lv = obj[ruler_level_field_str].Int();
			checkNotEoo(obj[ruler_face_id_field_str])
				_face_id = obj[ruler_face_id_field_str].Int();
		}

		void package(Json::Value& info)
		{
			rulerPlayerInfo::package(info);
			info[ruler_level_field_str] = _lv;
			info[ruler_face_id_field_str] = _face_id;
		}

	private:
		int _lv;
		int _face_id;
	};

	const static string ruler_score_field_str = "sc";
	const static string ruler_report_id_field_str = "ri";

	class rulerBattlePlayer : public rulerKingInfo
	{
	public:
		rulerBattlePlayer() : _score(0), _report_id("-1"){}

		int getScore() const { return _score; }
		string getReportId() const { return _report_id; }
		void setScore(int score){ _score = score; }
		void setReportId(const string& report_id){ _report_id = report_id; }

		void setFromBSONObj(const mongo::BSONObj& obj)
		{
			rulerKingInfo::setFromBSONObj(obj);
			checkNotEoo(obj[ruler_score_field_str])
				_score = obj[ruler_score_field_str].Int();
			checkNotEoo(obj[ruler_report_id_field_str])
				_report_id = obj[ruler_report_id_field_str].String();
		}

		void setFromBSONElement(const mongo::BSONElement& obj)
		{
			rulerKingInfo::setFromBSONElement(obj);
			checkNotEoo(obj[ruler_score_field_str])
				_score = obj[ruler_score_field_str].Int();
			checkNotEoo(obj[ruler_report_id_field_str])
				_report_id = obj[ruler_report_id_field_str].String();
		}

		void package(Json::Value& info)
		{
			rulerKingInfo::package(info);
			info[ruler_score_field_str] = _score;
			info[ruler_report_id_field_str] = _report_id;
		}

		void toBSONObj(mongo::BSONObj& obj)
		{
			obj = BSON(ruler_player_id_field_str << getPlayerId()
				<< ruler_nickname_field_str << getNickName()
				<< ruler_level_field_str << getLv()
				<< ruler_face_id_field_str << getFaceId()
				<< ruler_report_id_field_str << _report_id
				<< ruler_score_field_str << _score);
		}

		void reset()
		{
			rulerKingInfo::reset();
			_score = 0;
			_report_id = "-1";
		}

	private:
		int _score;
		string _report_id;
	};

	const static string ruler_report_list_db_str = "gl.ruler_report_list";
	const static string ruler_count_id_field_str = "ci";
	const static string ruler_report_type_field_str = "rt";

	class rulerReport
	{
	public:
		rulerReport() : _report_id(""), _report_type(_report_not_set){}

	    bool operator<(const rulerReport& report)
		{
			return boost::lexical_cast<int , string>(_report_id) > boost::lexical_cast<int , string>(report.getReportId());
		}

		void package(Json::Value& info)
		{
			info = Json::arrayValue;
			info.append(_rulerPlayers[_left_side].getPlayerId());
			info.append(_rulerPlayers[_left_side].getNickName());
			info.append(_rulerPlayers[_right_side].getPlayerId());
			info.append(_rulerPlayers[_right_side].getNickName());
			info.append(_report_id);
			info.append(_report_type);
		}
		void setFromBSONObj(mongo::BSONObj& obj)
		{
			checkNotEoo(obj[ruler_count_id_field_str])
				_count_id = obj[ruler_count_id_field_str].Int();
			checkNotEoo(obj[ruler_report_id_field_str])
				_report_id = obj[ruler_report_id_field_str].String();
			checkNotEoo(obj[ruler_report_type_field_str])
				_report_type = obj[ruler_report_type_field_str].Int();
			checkNotEoo(obj[ruler_left_side_player_field_str])
				_rulerPlayers[_left_side].setFromBSONElement(obj[ruler_left_side_player_field_str]);
			checkNotEoo(obj[ruler_right_side_player_field_str])
				_rulerPlayers[_right_side].setFromBSONElement(obj[ruler_right_side_player_field_str]);
		}
		void setFromBSONElement(mongo::BSONElement& obj)
		{
			checkNotEoo(obj[ruler_count_id_field_str])
				_count_id = obj[ruler_count_id_field_str].Int();
			checkNotEoo(obj[ruler_report_id_field_str])
				_report_id = obj[ruler_report_id_field_str].String();
			checkNotEoo(obj[ruler_report_type_field_str])
				_report_type = obj[ruler_report_type_field_str].Int();
			checkNotEoo(obj[ruler_left_side_player_field_str])
				_rulerPlayers[_left_side].setFromBSONElement(obj[ruler_left_side_player_field_str]);
			checkNotEoo(obj[ruler_right_side_player_field_str])
				_rulerPlayers[_right_side].setFromBSONElement(obj[ruler_right_side_player_field_str]);
		}

		void toBSONObj(mongo::BSONObj& obj)
		{
			mongo::BSONObjBuilder a;
			a << ruler_count_id_field_str << _count_id << ruler_report_id_field_str << _report_id
				<< ruler_report_type_field_str << _report_type;
			{
				mongo::BSONObjBuilder b;
				b << ruler_player_id_field_str << _rulerPlayers[_left_side].getPlayerId()
					<< ruler_nickname_field_str << _rulerPlayers[_left_side].getNickName();
				a << ruler_left_side_player_field_str << b.obj();
			}
			{
				mongo::BSONObjBuilder b;
				b << ruler_player_id_field_str << _rulerPlayers[_right_side].getPlayerId()
					<< ruler_nickname_field_str << _rulerPlayers[_right_side].getNickName();
				a << ruler_right_side_player_field_str << b.obj();
			}
			obj = a.obj();
		}
		bool save()
		{
			mongo::BSONObj key = BSON(ruler_count_id_field_str << _count_id);
			mongo::BSONObj obj;
			toBSONObj(obj);
			return db_mgr.save_mongo(ruler_report_list_db_str, key, obj);
		}
		bool drop()
		{
			_report_type = _report_not_set;
			mongo::BSONObj key = BSON(ruler_count_id_field_str << _count_id);
			mongo::BSONObj obj = BSON(ruler_count_id_field_str << _count_id
				<< ruler_report_type_field_str << _report_type);
			return db_mgr.save_mongo(ruler_report_list_db_str, key, obj);
		}
		int getWinnerPlayerId(){ return _rulerPlayers[_left_side].getPlayerId(); }
		int getBattlePlayerId(int side){ return _rulerPlayers[side].getPlayerId(); }
		void set(playerDataPtr& d, playerDataPtr& targetP, const string& report_id, int report_type, int count_id)
		{
			_rulerPlayers[_left_side].setPlayerId(d->playerID);
			_rulerPlayers[_left_side].setNickname(d->Base.getName());
			_rulerPlayers[_right_side].setPlayerId(targetP->playerID);
			_rulerPlayers[_right_side].setNickname(targetP->Base.getName());
			_report_id = report_id;
			_report_type = report_type;
			_count_id = count_id;
		}
		string getReportId() const { return _report_id; }
		int getCountId() const { return _count_id; }

	private:
		rulerPlayerInfo _rulerPlayers[_max_side];
		int _count_id;
		int _report_type;
		string _report_id;
	};

	const static string ruler_term_info_list_db_str = "gl.ruler_term_info_list";
	const static string ruler_begin_year_field_str = "by";
	const static string ruler_end_year_field_str = "ey";

	class rulerTermInfo
	{
	public:
		rulerTermInfo() : _end_year_stamp(-1){}

		void setFromBSONObj(const mongo::BSONObj& obj)
		{
			checkNotEoo(obj[ruler_count_id_field_str])
				_term_id = obj[ruler_count_id_field_str].Int() % 10000;
			checkNotEoo(obj[ruler_begin_year_field_str])
				_begin_year_stamp = obj[ruler_begin_year_field_str].Int();
			checkNotEoo(obj[ruler_end_year_field_str])
				_end_year_stamp = obj[ruler_end_year_field_str].Int();
			_kingInfo.setFromBSONObj(obj);
		}
		bool save(int country_id)
		{
			int count_id = _term_id  + country_id * 10000;

			mongo::BSONObj key = BSON(ruler_count_id_field_str << count_id);

			mongo::BSONObj obj = BSON(ruler_count_id_field_str << count_id << ruler_begin_year_field_str << _begin_year_stamp
				<< ruler_end_year_field_str << _end_year_stamp << ruler_player_id_field_str << _kingInfo.getPlayerId()
				<< ruler_nickname_field_str << _kingInfo.getNickName() << ruler_level_field_str << _kingInfo.getLv()
				<< ruler_face_id_field_str << _kingInfo.getFaceId());

			return db_mgr.save_mongo(ruler_term_info_list_db_str, key, obj);
		}
		void package(Json::Value& info)
		{
			info = Json::arrayValue;
			info.append(_term_id);
			info.append(_begin_year_stamp);
			info.append(_end_year_stamp);
			info.append(_kingInfo.getPlayerId());
			info.append(_kingInfo.getNickName());
			info.append(_kingInfo.getLv());
			info.append(_kingInfo.getFaceId());
		}

		int getPlayerId() const { return _kingInfo.getPlayerId(); }
		string getName() const { return _kingInfo.getNickName(); }
		void setKingInfo(const rulerBattlePlayer& player)
		{
			_kingInfo.setPlayerId(player.getPlayerId());
			_kingInfo.setNickname(player.getNickName());
			_kingInfo.setLv(player.getLv());
			_kingInfo.setFaceId(player.getFaceId());
		}
		void setName(const string& name){ _kingInfo.setNickname(name); }
		void setFaceId(int face_id){ _kingInfo.setFaceId(face_id); }
		void setLv(int lv){ _kingInfo.setLv(lv); }
		void setEndYear(int year){ _end_year_stamp = year; }
		void setTermId(int term_id){ _term_id = term_id; }
		void setBeginYear(int year){ _begin_year_stamp = year; }
	private:
		int _term_id;
		int _begin_year_stamp;
		int _end_year_stamp;
		rulerKingInfo _kingInfo;
	};

	const static string ruler_current_info_db_str = "gl.ruler_current_info";
	const static string ruler_finished_field_str = "fi";
	const static string ruler_year_field_str = "yr";
	const static string ruler_country_id_field_str = "ci";
	const static string ruler_last_time_node_field_str = "ltn";

	const static string ruler_report_list_field_str = "rl";
	const static string ruler_new_report_field_str = "new";

	class rulerCurrentInfo
	{
	public:
		rulerCurrentInfo(int country_id) : _country_id(country_id), _year_stamp(-1), _finished(false), _last_time_node(_time_21_50){}

		void package(Json::Value& info)
		{
			_battlePlayers[_left_side].package(info[ruler_left_side_player_field_str]);
			_battlePlayers[_right_side].package(info[ruler_right_side_player_field_str]);
		}
		bool save()
		{
			mongo::BSONObj key = BSON(ruler_country_id_field_str << _country_id);

			mongo::BSONObjBuilder obj;
			obj << ruler_country_id_field_str << _country_id << ruler_finished_field_str << _finished
				<< ruler_year_field_str << _year_stamp << ruler_last_time_node_field_str << _last_time_node;
			{
				mongo::BSONObj b;
				_battlePlayers[_left_side].toBSONObj(b);
				obj << ruler_left_side_player_field_str << b;
			}
			{
				mongo::BSONObj b;
				_battlePlayers[_right_side].toBSONObj(b);
				obj << ruler_right_side_player_field_str << b;
			}

			return db_mgr.save_mongo(ruler_current_info_db_str, key, obj.obj());
		}
		
		void setFromBSONObj(const mongo::BSONObj& obj)
		{
			checkNotEoo(obj[ruler_finished_field_str])
				_finished = obj[ruler_finished_field_str].Bool();
			checkNotEoo(obj[ruler_last_time_node_field_str])
				_last_time_node = obj[ruler_last_time_node_field_str].Int();
			checkNotEoo(obj[ruler_year_field_str])
				_year_stamp = obj[ruler_year_field_str].Int();
			checkNotEoo(obj[ruler_left_side_player_field_str])
				_battlePlayers[_left_side].setFromBSONElement(obj[ruler_left_side_player_field_str]);
			checkNotEoo(obj[ruler_right_side_player_field_str])
				_battlePlayers[_right_side].setFromBSONElement(obj[ruler_right_side_player_field_str]);
		}

		bool getFinshed(){ return _finished; }

		void reset(int year)
		{
			_finished = false;
			_year_stamp = year;
			_battlePlayers[_left_side].reset();
			_battlePlayers[_right_side].reset();
		}

		void setTimeNode(int node){ _last_time_node = node; }
		void setFinished(bool flag){ _finished = flag; }

		int getBattlePlayerNum()
		{
			int num = 0;
			if(_battlePlayers[_left_side].getPlayerId() != -1)
				++num;
			if(_battlePlayers[_right_side].getPlayerId() != -1)
				++num;
			return num;
		}

		rulerBattlePlayer& getBattlePlayer(int side){ return _battlePlayers[side]; }
		int getYear() const { return _year_stamp; }
		int getCountryId() const { return _country_id; }
		int getTimeNode() const { return _last_time_node; }

		void alterBattlePlayerAndSave(playerDataPtr& d, int side)
		{
			_battlePlayers[side].setNickname(d->Base.getName());
			_battlePlayers[side].setLv(d->Base.getLevel());
			_battlePlayers[side].setFaceId(d->Base.getFaceID());

			save();
		}

		void setBattlePlayerAndSave(playerDataPtr& d, int side, const string& report_id)
		{
			_battlePlayers[side].setPlayerId(d->playerID);
			_battlePlayers[side].setNickname(d->Base.getName());
			_battlePlayers[side].setLv(d->Base.getLevel());
			_battlePlayers[side].setFaceId(d->Base.getFaceID());
			_battlePlayers[side].setReportId(report_id);

			save();
		}

	private:
		bool _finished;                                      // 决出国王或没有候选人时置为true
		int _last_time_node;                            // 上一个时间结点
		int _country_id;
		int _year_stamp;                                                 

		rulerBattlePlayer _battlePlayers[_max_side];           // 左右候选人信息
	};

	typedef vector<rulerTitleInfo> rulerTitleInfoList;
	typedef list<rulerReport> rulerReportList;
	typedef vector<rulerTermInfo> rulerTermInfoList;

	const static string ruler_term_info_list_field_str = "tl";
	const static string ruler_term_info_list_count_field_str = "tc";
	const static string ruler_title_info_list_field_str = "tl";




	class rulerInfo
	{
	public:
		typedef boost::function<void(rulerInfo*)> timeWorker;

		rulerInfo(int country_id);

		void doTime(int time_node);

		void initStateAndFlushTime(int next_time_node, unsigned next_update_time)
		{
			if(next_time_node == _time_14_00)
			{
				_client_state = _client_state_no_battle;
				_next_flush_time = next_update_time;
			}
			else if(next_time_node == _time_21_25 || next_time_node == _time_21_30)
			{
				_client_state = _client_state_candidate_battle;
				if(next_time_node == _time_21_25)
					_next_flush_time = next_update_time + 5 * MINUTE;
				else
					_next_flush_time = next_update_time;
			}
			else if(next_time_node == _time_21_40)
			{
				if(isFinished())
				{
					_client_state = _client_state_no_battle;
					_next_flush_time = next_update_time + 20 * MINUTE + 4 * DAY - 8 * HOUR;
				}
				else
				{
					_client_state = _client_state_betting_time;
					_next_flush_time = next_update_time;
				}
			}
			else if(next_time_node == _time_21_45)
			{
				if(isFinished())
				{
					_client_state = _client_state_no_battle;
					_next_flush_time = next_update_time + 15 * MINUTE + 4 * DAY - 8 * HOUR;
				}
				else
				{
					_client_state = _client_state_final_battle;
					_next_flush_time = next_update_time;
				}
			}
			else
			{
				if(isFinished())
				{
					_client_state = _client_state_no_battle;
					_next_flush_time = next_update_time + 10 * MINUTE + 4 * DAY - 8 * HOUR;
				}
				else
				{
					_client_state = _client_state_final_battle;
					_next_flush_time = next_update_time;
				}
			}
		}

		void packageTermInfoList(Json::Value& info, int begin, int count)
		{
			if(begin < 1 || count < 1)
				return;

			unsigned num = begin + count - 1;
			num = num > _termInfoList.size()? _termInfoList.size() : num;

			info[ruler_term_info_list_field_str] = Json::arrayValue;
			Json::Value& ref = info[ruler_term_info_list_field_str];
			for(unsigned i = begin - 1; i < num; ++i)
			{
				Json::Value term_info;
				_termInfoList[i].package(term_info);
				ref.append(term_info);
			}

			info[ruler_term_info_list_count_field_str] = (unsigned)_termInfoList.size();
		}
		void packageCurrentInfo(Json::Value& info)
		{
			_currentInfo.package(info);
		}
		void packageReportList(Json::Value& info)
		{
			info[ruler_report_list_field_str] = Json::arrayValue;
			Json::Value& ref = info[ruler_report_list_field_str];

			for(rulerReportList::iterator iter = _reportList.begin(); iter != _reportList.end(); ++iter)
			{
				Json::Value report;
				iter->package(report);
				ref.append(report);
			}
		}
		void packageTitleInfoList(Json::Value& info)
		{
			info[ruler_title_info_list_field_str] = Json::arrayValue;
			Json::Value& ref = info[ruler_title_info_list_field_str];

			for(unsigned i = _title_king; i < _title_max; ++i)
			{
				Json::Value title_info;
				_titleInfoList[i].package(title_info);
				ref.append(title_info);
			}
		}

		bool isNew(playerDataPtr d)
		{
			bool flag = d->Ruler.getLastReportTime() < _last_report_time;
			if(flag)
				d->Ruler.setLastReportTime(_last_report_time);
			return flag;
		}

		void setCurrentInfo(mongo::BSONObj& obj){ _currentInfo.setFromBSONObj(obj); }
		void setBattlePlayerAndSave(playerDataPtr& d, int side, const string& report_id){ _currentInfo.setBattlePlayerAndSave(d, side, report_id); }

		int getReportId()
		{
			++_max_report_id;
			if(_max_report_id >= 10000)
				_max_report_id = 1;
			return _max_report_id + 10000 * _currentInfo.getCountryId();
		}
		int getMaxReportId(){ return _max_report_id; }
		void setMaxReportId(int report_id){ _max_report_id = report_id; }
		int getReportCountId();
		void addReport(const rulerReport& report);
		void sortReportList();
		void addReportAndSave(playerDataPtr& d, playerDataPtr& targetP, const string& report_id, int report_type);

		void addTermInfo(const rulerTermInfo& term_info){ _termInfoList.push_back(term_info); }

		int getPlayerId(int side){ return _currentInfo.getBattlePlayer(side).getPlayerId(); }
		string getName(int side){ return _currentInfo.getBattlePlayer(side).getNickName(); }
		int getFinalBattleSide();
		int getPlayerByTitle(int title){ return _titleInfoList[title].getPlayerId();}

		bool isFinished(){ return _currentInfo.getFinshed(); }
		int getYear(){ return _currentInfo.getYear(); }
		int getTimeNode(){ return _currentInfo.getTimeNode(); }

		void resetTimeNode(){ _currentInfo.setTimeNode(_time_21_50); }
		void checkCurrentInfo();

		void repairCurrentInfo(int next_node, unsigned next_update_time)
		{
			if(_bRepaired)
				return;

			checkCurrentInfo();

			_bRepaired = true;
			_end_node = next_node;
			_end_time = next_update_time;

			int next = (_currentInfo.getTimeNode() + 1) % _time_max;

			if(next != _end_node)
				doTime(next);
			else
			{
				_bInit = true;
				initStateAndFlushTime(_end_node, _end_time);
			}
		}

		bool checkTitleAndSet(playerDataPtr& d, int title)
		{
			if(_titleInfoList[title].checkTitle())
				return false;
			
			_titleInfoList[title].setTitle(d->playerID, d->Base.getName(), _currentInfo.getCountryId(), d->Base.getFaceID());
			return true;
		}
		void setKingTitleLog(int player_id, const string& nickname);

		void setKingTitle(int player_id, const string& nickname, int face_id)
		{
			_titleInfoList[_title_king].setTitle(player_id, nickname, _currentInfo.getCountryId(), face_id);
			setKingTitleLog(player_id, nickname);
		}
		void setTitle(int title, mongo::BSONObj& obj)
		{
			_titleInfoList[title].setFromBSONObj(obj);
		}
		void clearTitleInfoAndSave();
		int getClientState(){ return _client_state; }
		unsigned getNextFlushTime(){ return _next_flush_time; }

		bool checkAllTitles()
		{
			int title = _title_king + 1;
			for(; title < _title_max; ++title)
			{
				if(_titleInfoList[title].checkTitle())
					return false;
			}
			return true;
		}

		int getWinnerSide()
		{
			return _currentInfo.getBattlePlayer(_left_side).getScore() == 2? _left_side : _right_side;
		}
		int getLoserSide()
		{
			return _currentInfo.getBattlePlayer(_right_side).getScore() == 2? _left_side : _right_side;
		}
		
		void postFinalBattleReq();
		void postFinalBattleResp(playerDataPtr d, playerDataPtr targetP, string report);

		int getFinalBattlePlayer(int side)
		{
			if(_currentInfo.getBattlePlayer(_left_side).getPlayerId() == -1 && _currentInfo.getBattlePlayer(_right_side).getPlayerId() == -1)
				return -1;

			if(!isFinished())
				return -1;

			

			if (side == 0)
			{
				if (_currentInfo.getBattlePlayer(_left_side).getPlayerId() == -1 && _currentInfo.getBattlePlayer(_right_side).getPlayerId() != -1)
					return _currentInfo.getBattlePlayer(_right_side).getPlayerId();

				if (_currentInfo.getBattlePlayer(_left_side).getPlayerId() != -1 && _currentInfo.getBattlePlayer(_right_side).getPlayerId() == -1)
					return _currentInfo.getBattlePlayer(_left_side).getPlayerId();

				return _currentInfo.getBattlePlayer(getWinnerSide()).getPlayerId();
			}
			else
			{
				if (_currentInfo.getBattlePlayer(_left_side).getPlayerId() == -1 || _currentInfo.getBattlePlayer(_right_side).getPlayerId() == -1)
					return -1;

				return _currentInfo.getBattlePlayer(getLoserSide()).getPlayerId();
			}

		}

		void saveCurrentInfo();
		bool isRepaired(){ return _bRepaired; }
		bool isInited(){ return _bInit; }

		void alterPlayerInfo(playerDataPtr d, int type)
		{
			if(type != _ruler_alter_lv)
			{
				for(unsigned i = 0; i < _termInfoList.size(); ++i)
				{
					if(_termInfoList[i].getPlayerId() == d->playerID)
					{
						_termInfoList[i].setName(d->Base.getName());
						_termInfoList[i].setFaceId(d->Base.getFaceID());
						_termInfoList[i].save(_currentInfo.getCountryId());
					}
				}
				int title = d->Ruler.getTitle();
				if(title != _no_title)
				{
					_titleInfoList[title].setNickname(d->Base.getName());
					_titleInfoList[title].setFaceId(d->Base.getFaceID());
					_titleInfoList[title].save(_currentInfo.getCountryId());
				}
			}
			
			if (d->Ruler.getTitle() == _title_king && type == _ruler_alter_lv)
			{
				_termInfoList.back().setLv(d->Base.getLevel());
				_termInfoList.back().save(_currentInfo.getCountryId());
			}

			if(_currentInfo.getBattlePlayer(_left_side).getPlayerId() == d->playerID)
				_currentInfo.alterBattlePlayerAndSave(d, _left_side);
			if(_currentInfo.getBattlePlayer(_right_side).getPlayerId() == d->playerID)
				_currentInfo.alterBattlePlayerAndSave(d, _right_side);
		}

		void setClientState(int state);
		int getLastYear()
		{
			int year = season_sys.getYear(_currentInfo.getYear());
			if(isFinished())
				return year;
			else
				return year - 1;
		}

		void getKingValue(Json::Value& info);
		void getOfficerVec(set<int>& officer_vec);
		const rulerTitleInfoList& getTitleInfoList() const { return _titleInfoList; }

	private:
		bool initTimeWorkers();
		void doAt1400();
		void doAt2125();
		void doAt2130();
		void doAt2140();
		void doAt2145();
		void doAt2150();

		void battleRespAt2140(rulerReport& report);
		void battleRespAt2145(rulerReport& report);
		void battleRespAt2150(rulerReport& report);

		void clearReportListAndSave();
		bool isReelection(int player_id);
		void addTermInfoAndSave();
		void addReportAndSave(const rulerReport& report);

		bool _bInit;
		bool _bRepaired;
		int _end_node;
		int _end_time;

		int _max_count_id;
		int _max_report_id;
		int _client_state;                                                          // 当前客户端状态(见_client_state枚举)
		unsigned _next_flush_time;                                      
		rulerTermInfoList _termInfoList;                              // 历届国王信息
		rulerTitleInfoList _titleInfoList;                                 // 官职列表信息
		rulerReportList _reportList;                                      // 战报列表
		rulerCurrentInfo _currentInfo;                                  // 当前左右储君的信息

		unsigned _last_report_time;

		static timeWorker _timeWorkers[_time_max];
	};
}