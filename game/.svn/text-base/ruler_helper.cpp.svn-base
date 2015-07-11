#include "ruler_helper.h"
#include "time_helper.h"
#include "season_system.h"
#include "ruler_system.h"
#include "arena_system.h"
#include "chat_system.h"
#include "record_system.h"
namespace gg
{
	rulerInfo::timeWorker rulerInfo::_timeWorkers[_time_max];

	rulerInfo::rulerInfo(int country_id)
		: _currentInfo(country_id), _max_report_id(0), _max_count_id(0), _client_state(_client_state_not_set), _next_flush_time(0)
		, _bInit(false), _bRepaired(false), _last_report_time(0)
	{
		for(unsigned i = 0; i < _title_max; ++i)
			_titleInfoList.push_back(rulerTitleInfo(i));
	}

	bool rulerInfo::initTimeWorkers()
	{
		_timeWorkers[_time_14_00] = &rulerInfo::doAt1400;
		_timeWorkers[_time_21_25] = &rulerInfo::doAt2125;
		_timeWorkers[_time_21_30] = &rulerInfo::doAt2130;
		_timeWorkers[_time_21_40] = &rulerInfo::doAt2140;
		_timeWorkers[_time_21_45] = &rulerInfo::doAt2145;
		_timeWorkers[_time_21_50] = &rulerInfo::doAt2150;
		return true;
	}

	void rulerInfo::doTime(int time_node)
	{
		static bool bInit = initTimeWorkers();
		_timeWorkers[time_node](this);
	}

	void rulerInfo::doAt1400()
	{
		if(_bInit && _currentInfo.getCountryId() == 0)
		{
			ruler_sys.postBroadcastReq(_ruler_broadcast_type_11, -1, -1, -1, "");
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_ruler_begin);
		}

		_currentInfo.setTimeNode(_time_14_00);
		_next_flush_time += (7 * HOUR + 30 * MINUTE);
		unsigned now = na::time_helper::get_current_time();
		int year = season_sys.getYearStamp(now);
		_currentInfo.reset(year);
		clearReportListAndSave();
		setClientState(_client_state_candidate_battle);
		saveCurrentInfo();

		//if (_currentInfo.getCountryId() == 0)
		//{
		//	unsigned temp = _next_flush_time;
		//	int season[4];
		//	memset(season, 0x0, sizeof(season));
		//	season[0] = 1;
		//	ActionLimitMap limitMap;
		//	limitMap[isJoinCountry] = 1;
		//	action_show_sys.addSystemActivityToActivityDisplayerList(ruler_betting, temp, temp + 10 * na::time_helper::MINUTE, season, limitMap);
		//}
	}

	void rulerInfo::doAt2125()
	{
		if(_bInit && _currentInfo.getCountryId() == 0)
			ruler_sys.postBroadcastReq(_ruler_broadcast_type_1, -1, -1, -1, "");
		
		_currentInfo.setTimeNode(_time_21_25);
		setClientState(_client_state_candidate_battle);
		saveCurrentInfo();
	}

	void rulerInfo::doAt2130()
	{
		/*if(_bInit && _currentInfo.getCountryId() == 0)
			ruler_sys.postBroadcastReq(_ruler_broadcast_type_2, -1, -1, -1, "");*/

		_currentInfo.setTimeNode(_time_21_30);

		//if (_currentInfo.getCountryId() == 0)
		//{
		//	unsigned temp = _next_flush_time;
		//	int season[4];
		//	memset(season, 0x0, sizeof(season));
		//	season[0] = 1;
		//	ActionLimitMap limitMap;
		//	limitMap[isJoinCountry] = 1;
		//	action_show_sys.addSystemActivityToActivityDisplayerList(ruler_betting, temp, temp + 10 * na::time_helper::MINUTE, season, limitMap);
		//}

		int num = _currentInfo.getBattlePlayerNum();
		if(num == 0)
		{
			_currentInfo.setFinished(true);
			_next_flush_time += (30 * MINUTE + 4 * DAY - 8 * HOUR);
			setClientState(_client_state_no_battle);

			if(_bInit)
			{
				if(_termInfoList.empty())
					ruler_sys.postBroadcastReq(_ruler_broadcast_type_5, -1, -1, _currentInfo.getCountryId(), "");
				else
					ruler_sys.postBroadcastReq(_ruler_broadcast_type_4, _termInfoList.back().getPlayerId(), -1, _currentInfo.getCountryId(), "");
			}
		}

		if(num == 1)
		{
			_next_flush_time += (30 * MINUTE + 4 * DAY - 8 * HOUR);
			_currentInfo.setFinished(true);
			addTermInfoAndSave();
			clearTitleInfoAndSave();
			int side = _currentInfo.getBattlePlayer(_left_side).getPlayerId() != -1? _left_side : _right_side;
			setKingTitle(_currentInfo.getBattlePlayer(side).getPlayerId(), _currentInfo.getBattlePlayer(side).getNickName(), _currentInfo.getBattlePlayer(side).getFaceId());
			setClientState(_client_state_no_battle);
			
			if(_bInit)
				ruler_sys.postBroadcastReq(_ruler_broadcast_type_3, _termInfoList.back().getPlayerId(), -1, _currentInfo.getCountryId(), "");
		}

		if(num == 2)
		{
			_next_flush_time += (10 * MINUTE);
			setClientState(_client_state_betting_time);

		}

		saveCurrentInfo();
	}

	void rulerInfo::doAt2140()
	{
		if (!isFinished())
		{
			postFinalBattleReq();
		}
		else
		{
			_currentInfo.setTimeNode(_time_21_40);
			saveCurrentInfo();
		}
	}

	void rulerInfo::doAt2145()
	{
		if(!isFinished())
			postFinalBattleReq();
		else
		{
			_currentInfo.setTimeNode(_time_21_45);
			saveCurrentInfo();
		}
	}

	void rulerInfo::doAt2150()
	{
		if(!isFinished())
			postFinalBattleReq();
		else
		{
			_currentInfo.setTimeNode(_time_21_50);
			saveCurrentInfo();
		}
	}

	void rulerInfo::clearReportListAndSave()
	{
		for(rulerReportList::iterator iter = _reportList.begin(); iter != _reportList.end(); ++iter)
			iter->drop();

		_max_count_id = 0;
		_max_report_id = 0;
		_reportList.clear();
	}

	bool rulerInfo::isReelection(int player_id)
	{
		if(_termInfoList.empty())
			return false;
		return _termInfoList.back().getPlayerId() == player_id;
	}

	void rulerInfo::addTermInfoAndSave()
	{
		int num = _currentInfo.getBattlePlayerNum();
		if(num == 0)
			return;

		int side = 0;
		if(num == 1)
			side = _currentInfo.getBattlePlayer(_left_side).getPlayerId() != -1? _left_side : _right_side;
		if(num == 2)
			side = _currentInfo.getBattlePlayer(_left_side).getScore() == 2? _left_side : _right_side;

		if(!isReelection(_currentInfo.getBattlePlayer(side).getPlayerId())) // ²»Á¬ÈÎ
		{
			if(!_termInfoList.empty())
			{
				_termInfoList.back().setEndYear(_currentInfo.getYear());
				_termInfoList.back().save(_currentInfo.getCountryId());
			}
			_termInfoList.push_back(rulerTermInfo());
			_termInfoList.back().setBeginYear(_currentInfo.getYear());
			_termInfoList.back().setTermId(_termInfoList.size());
		}
		_termInfoList.back().setKingInfo(_currentInfo.getBattlePlayer(side));
		_termInfoList.back().save(_currentInfo.getCountryId());
	}

	void rulerInfo::addReportAndSave(playerDataPtr& d, playerDataPtr& targetP, const string& report_id, int report_type)
	{
		rulerReport report;
		report.set(d, targetP, report_id, report_type, getReportCountId());
		_reportList.push_front(report);
		while(_reportList.size() > 20)
			_reportList.pop_back();

		_last_report_time = na::time_helper::get_current_time();

		_reportList.front().save();
	}

	void rulerInfo::addReport(const rulerReport& report)
	{
		_reportList.push_front(report);
		while(_reportList.size() > 20)
			_reportList.pop_back();
	}

	void rulerInfo::addReportAndSave(const rulerReport& report)
	{
		_reportList.push_front(report);
		while(_reportList.size() > 20)
			_reportList.pop_back();

		_reportList.front().save();
	}

	void rulerInfo::sortReportList()
	{
		if(!_reportList.empty())
		{
			_reportList.sort();
			_max_count_id = _reportList.front().getCountId() % 10000;
		}
	}

	int rulerInfo::getReportCountId()
	{
		++_max_count_id;
		if(_max_count_id > 20)
			_max_count_id = 1;
		return _max_count_id + 10000 * _currentInfo.getCountryId();
	}

	int rulerInfo::getFinalBattleSide()
	{
		switch(_currentInfo.getTimeNode())
		{
			case _time_21_30:
				return _left_side;
			case _time_21_40:
				return _right_side;
			default:
				return rand() % _max_side;
		}
	}

	void rulerInfo::checkCurrentInfo()
	{
		unsigned now = na::time_helper::get_current_time();
		unsigned year = season_sys.getYearStamp(now);
		if(year != _currentInfo.getYear() && !_currentInfo.getFinshed())
		{
			_currentInfo.reset(_currentInfo.getYear());
			_currentInfo.setFinished(true);
			_currentInfo.setTimeNode(_time_21_50);
			clearReportListAndSave();
			_currentInfo.save();
		}
	}

	void rulerInfo::clearTitleInfoAndSave()
	{
		for(unsigned i = _title_king + 1; i < _title_max; ++i)
			_titleInfoList[i].resetAndSave(_currentInfo.getCountryId());
	}

	void rulerInfo::postFinalBattleReq()
	{
		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(_currentInfo.getCountryId());
		string str = msg.toStyledString();
		na::msg::msg_json m(gate_client::ruler_final_battle_inner_req, str);
		player_mgr.postMessage(m);
	}

	void rulerInfo::battleRespAt2140(rulerReport& report)
	{
		_currentInfo.setTimeNode(_time_21_40);
		_next_flush_time += (5 * MINUTE);
		//_client_state = _client_state_final_battle;
		addReportAndSave(report);
		int side = report.getWinnerPlayerId() == _currentInfo.getBattlePlayer(_left_side).getPlayerId()? _left_side : _right_side;
		_currentInfo.getBattlePlayer(side).setScore(1);
		setClientState(_client_state_final_battle);
		if(_bInit)
		{
			int other_side = (side == _left_side)? _right_side : _left_side;
			ruler_sys.postBroadcastReq(_ruler_broadcast_type_6, _currentInfo.getBattlePlayer(side).getPlayerId(), _currentInfo.getBattlePlayer(other_side).getPlayerId()
				, _currentInfo.getCountryId(), _reportList.front().getReportId());
		}
	}

	void rulerInfo::battleRespAt2145(rulerReport& report)
	{
		_currentInfo.setTimeNode(_time_21_45);
		_next_flush_time += (5 * MINUTE);
		addReportAndSave(report);
		int side = report.getWinnerPlayerId() == _currentInfo.getBattlePlayer(_left_side).getPlayerId()? _left_side : _right_side;
		_currentInfo.getBattlePlayer(side).setScore(_currentInfo.getBattlePlayer(side).getScore() + 1);
		setClientState(_client_state_final_battle);

		if(_currentInfo.getBattlePlayer(side).getScore() == 2)
		{
			_next_flush_time += (10 * MINUTE + 4 * DAY - 8 * HOUR);
			//_client_state = _client_state_no_battle;
			addTermInfoAndSave();
			_currentInfo.setFinished(true);
			//_currentInfo.getFinalReports().clear();
			clearTitleInfoAndSave();
			setKingTitle(_currentInfo.getBattlePlayer(side).getPlayerId(), _currentInfo.getBattlePlayer(side).getNickName(), _currentInfo.getBattlePlayer(side).getFaceId());
			int other_side = (side == _left_side)? _right_side : _left_side;

			setClientState(_client_state_no_battle);

			vector<string> fields;
			fields.push_back(_currentInfo.getBattlePlayer(_left_side).getNickName());
			fields.push_back(_currentInfo.getBattlePlayer(_right_side).getNickName());
			fields.push_back(boost::lexical_cast<string, int>(_currentInfo.getCountryId()));
			fields.push_back(boost::lexical_cast<string, int>(side == _left_side? 1 : 2));
			StreamLog::Log(rulerLogDBStr, -1, "", 0, "", "", fields, ruler_system::_final_battle_log);

			if(_bInit)
				ruler_sys.postBroadcastReq(_ruler_broadcast_type_7, _currentInfo.getBattlePlayer(side).getPlayerId()
				, _currentInfo.getBattlePlayer(other_side).getPlayerId(), _currentInfo.getCountryId()
				, _reportList.front().getReportId());	
		}
		else if(_bInit)
		{
			int other_side = (side == _left_side)? _right_side : _left_side;
			ruler_sys.postBroadcastReq(_ruler_broadcast_type_8, _currentInfo.getBattlePlayer(side).getPlayerId()
				, _currentInfo.getBattlePlayer(other_side).getPlayerId(), _currentInfo.getCountryId()
				, _reportList.front().getReportId());
		}
	}

	void rulerInfo::battleRespAt2150(rulerReport& report)
	{
		_currentInfo.setTimeNode(_time_21_50);
		_next_flush_time += (10 * MINUTE + 4 * DAY - 8 * HOUR);
		addReportAndSave(report);

		int side = report.getWinnerPlayerId() == _currentInfo.getBattlePlayer(_left_side).getPlayerId()? _left_side : _right_side;
		_currentInfo.getBattlePlayer(side).setScore(_currentInfo.getBattlePlayer(side).getScore() + 1);

		addTermInfoAndSave();
		_currentInfo.setFinished(true);
		clearTitleInfoAndSave();
		setKingTitle(_currentInfo.getBattlePlayer(side).getPlayerId(), _currentInfo.getBattlePlayer(side).getNickName(), _currentInfo.getBattlePlayer(side).getFaceId());
		int other_side = side == _left_side? _right_side : _left_side;

		setClientState(_client_state_no_battle);

		vector<string> fields;
		fields.push_back(_currentInfo.getBattlePlayer(_left_side).getNickName());
		fields.push_back(_currentInfo.getBattlePlayer(_right_side).getNickName());
		fields.push_back(boost::lexical_cast<string, int>(_currentInfo.getCountryId()));
		fields.push_back(boost::lexical_cast<string, int>(side == _left_side? 1 : 2));
		StreamLog::Log(rulerLogDBStr, -1, "", 0, "", "", fields, ruler_system::_final_battle_log);

		if(_bInit)
			ruler_sys.postBroadcastReq(_ruler_broadcast_type_9, _currentInfo.getBattlePlayer(side).getPlayerId()
				, _currentInfo.getBattlePlayer(other_side).getPlayerId(), _currentInfo.getCountryId()
				, _reportList.front().getReportId());
	}

	void rulerInfo::postFinalBattleResp(playerDataPtr d, playerDataPtr targetP, string report_id)
	{
		rulerReport report;
		report.set(d, targetP, report_id, _currentInfo.getTimeNode() - _time_21_30 + _report_final_one, getReportCountId());

		switch(_currentInfo.getTimeNode())
		{
		case _time_21_30:
			battleRespAt2140(report);
			break;
		case _time_21_40:
			battleRespAt2145(report);
			break;
		case _time_21_45:
			battleRespAt2150(report);
			break;
		default:
			break;
		}
		saveCurrentInfo();
	}

	void rulerInfo::saveCurrentInfo()
	{
		_currentInfo.save();

		if(!_bInit)
		{
			if((_currentInfo.getTimeNode() + 1) % _time_max != _end_node)
				doTime((_currentInfo.getTimeNode() + 1) % _time_max);
			else
			{
				_bInit = true;
				initStateAndFlushTime(_end_node, _end_time);
			}
		}
	}

	void rulerInfo::setClientState(int state)
	{
		int temp = _client_state;
		_client_state = state;
		if(_bInit && temp != _client_state)
		{
			//action_show_sys.noticeClientUpdateBonusPar();
		}
	}

	void rulerInfo::setKingTitleLog(int player_id, const string& nickname)
	{
		vector<string> fields;
		fields.push_back(nickname);
		fields.push_back(boost::lexical_cast<string, int>(_title_king));
		fields.push_back(boost::lexical_cast<string, int>(_currentInfo.getCountryId()));
		fields.push_back(boost::lexical_cast<string, int>(getLastYear()));
		fields.push_back(boost::lexical_cast<string, int>(player_id));
		StreamLog::Log(rulerLogDBStr, -1, "", 0, "", "", fields, ruler_system::_set_title_log);
	}

	void rulerInfo::getKingValue(Json::Value& info)
	{
		info = Json::arrayValue;
		if (_termInfoList.empty())
		{
			info.append(-1);
			info.append(-1);
			info.append(-1);
			info.append(-1);
			info.append("");
			info.append(-1);
			info.append(-1);
		}
		else
		{
			_termInfoList.back().package(info);
		}
	}

	void rulerInfo::getOfficerVec(set<int>& officer_vec)
	{
		unsigned i = _title_king;
		++i;
		for (; i < _title_max; ++i)
		{
			int player_id = _titleInfoList[i].getPlayerId();
			if (player_id != -1)
				officer_vec.insert(player_id);
		}
	}
}