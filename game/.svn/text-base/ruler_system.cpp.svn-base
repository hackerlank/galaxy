#include "ruler_system.h"
#include "response_def.h"
#include "season_system.h"
#include "file_system.h"
#include "battle_def.h"
#include "battle_system.h"
#include "chat_system.h"
#include "arena_system.h"
#include "record_system.h"
#include "timmer.hpp"

using namespace mongo;

namespace gg
{
	ruler_system* const ruler_system::rulerSys = new ruler_system;

#define rulerCheckCountryId(d) \
	if(d->Base.getSphereID() < 0 || d->Base.getSphereID() > 2) \
		Return(r, -1);

	ruler_system::ruler_system()
	{
		_bInit = false;
		tick = na::time_helper::get_sys_time();
	}

	void ruler_system::initTimeDiffs()
	{
		_time_diff[_time_14_00] = 7 * HOUR + 25 * MINUTE;
		_time_diff[_time_21_25] = 5 * MINUTE;
		_time_diff[_time_21_30] = 10 * MINUTE;
		_time_diff[_time_21_40] = 5 * MINUTE;
		_time_diff[_time_21_45] = 5 * MINUTE;
		_time_diff[_time_21_50] = 4 * DAY + 10 * MINUTE - 8 * HOUR;
	}

	void ruler_system::initTimeNode()
	{
		unsigned now = na::time_helper::get_current_time();
		struct tm tm_struct = na::time_helper::toTm(now);
		unsigned secs = tm_struct.tm_hour * 3600 + tm_struct.tm_min * 60 + tm_struct.tm_sec;
		int season = season_sys.getSeason(now);
		if(season == season_quarter_one)
		{
			if(secs < 5 * HOUR)
			{
				_next_time_node = _time_14_00;
				_next_update_time = season_sys.getNextSeasonTime(season_quarter_one, 14);
			}
			else if(secs < 14 * HOUR)
			{
				_next_time_node = _time_14_00;
				_next_update_time = now - secs + 14 * HOUR;
			}
			else if(secs < 21 * HOUR + 25 * MINUTE)
			{
				_next_time_node = _time_21_25;
				_next_update_time = now - secs + 21 * HOUR + 25 * MINUTE;
			}
			else if(secs < 21 * HOUR + 30 * MINUTE)
			{
				_next_time_node = _time_21_30;
				_next_update_time = now - secs + 21 * HOUR + 30 * MINUTE;
			}
			else if(secs < 21 * HOUR + 40 * MINUTE)
			{
				_next_time_node = _time_21_40;
				_next_update_time = now - secs + 21 * HOUR + 40 * MINUTE;
			}
			else if(secs < 21 * HOUR + 45 * MINUTE)
			{
				_next_time_node = _time_21_45;
				_next_update_time = now - secs + 21 * HOUR + 45 * MINUTE;
			}
			else if(secs < 21 * HOUR + 50 * MINUTE)
			{
				_next_time_node = _time_21_50;
				_next_update_time = now - secs + 21 * HOUR + 50 * MINUTE;
			}
			else
			{
				_next_time_node = _time_14_00;
				_next_update_time = season_sys.getNextSeasonTime(season_quarter_one, 14);
			}
		}
		else
		{
			_next_time_node = _time_14_00;
			_next_update_time = season_sys.getNextSeasonTime(season_quarter_one, 14);
		}
	}

	void ruler_system::initData()
	{
		db_mgr.ensure_index(rulerDBStr, BSON(playerIDStr << 1));

		for(unsigned i = 0; i < 3; ++i)
			_rulerInfos.push_back(rulerInfo(i));

		initBroadcast();
		initTimeNode();
		initTimeDiffs();

		createReportDir();

		loadRulerReportListDB();
		loadRulerInfoListDB();
		loadRulerCurrentInfoDB();
		loadRulerTitleInfoListDB();

		Timer::AddEventSeasonCycleTime(boostBind(ruler_system::rulerBroadcast, this, (int)_broadcast_at_18_00), season_quarter_one, 18, 0, 0);
		Timer::AddEventSeasonCycleTime(boostBind(ruler_system::rulerBroadcast, this, (int)_broadcast_at_21_25), season_quarter_one, 21, 25, 5);
		Timer::AddEventSeasonCycleTime(boostBind(ruler_system::rulerBroadcast, this, (int)_broadcast_at_21_30), season_quarter_one, 21, 30, 5);
		Timer::AddEventSeasonCycleTime(boostBind(ruler_system::rulerBroadcast, this, (int)_broadcast_at_21_35), season_quarter_one, 21, 35, 5);
	}

	void ruler_system::createReportDir()
	{
		boost::filesystem::path path_dir(reportRulerPVPDir);
		if(!boost::filesystem::exists(path_dir))
			boost::filesystem::create_directory(path_dir);
	}

	void ruler_system::loadRulerReportListDB()
	{
		objCollection objs = db_mgr.Query(ruler_report_list_db_str);

		for(unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];

			int type = obj[ruler_report_type_field_str].Int();
			if(type == _report_not_set)
				continue;

			int country_id = obj[ruler_count_id_field_str].Int() / 10000;
			int report_id = boost::lexical_cast<int , string>(obj[ruler_report_id_field_str].String());

			if(report_id % 10000 > _rulerInfos[country_id].getMaxReportId())
				_rulerInfos[country_id].setMaxReportId(report_id % 10000);

			rulerReport report;
			report.setFromBSONObj(obj);
			_rulerInfos[country_id].addReport(report);
		}

		for(unsigned i = 0; i < 3; ++i)
		{
			if(_rulerInfos[i].getMaxReportId() != 0)
			{
				_rulerInfos[i].sortReportList();
			}
		}
	}

	void ruler_system::loadRulerInfoListDB()
	{
		objCollection objs = db_mgr.Query(ruler_term_info_list_db_str);

		for(unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];
			
			int country_id = obj[ruler_count_id_field_str].Int() / 10000;
			rulerTermInfo termInfo;
			termInfo.setFromBSONObj(obj);
			_rulerInfos[country_id].addTermInfo(termInfo);
		}
	}

	void ruler_system::loadRulerCurrentInfoDB()
	{
		objCollection objs = db_mgr.Query(ruler_current_info_db_str);

		for(unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];

			int country_id = obj[ruler_country_id_field_str].Int();
			_rulerInfos[country_id].setCurrentInfo(obj);
		}
	}

	void ruler_system::loadRulerTitleInfoListDB()
	{
		objCollection objs = db_mgr.Query(ruler_title_info_list_db_str);

		for(unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];

			int country_id = obj[ruler_title_field_str].Int() / 10000;
			int title = obj[ruler_title_field_str].Int() % 10000;
			_rulerInfos[country_id].setTitle(title, obj);
		}
	}

	void ruler_system::updateTimeNode()
	{
		_next_update_time += _time_diff[_next_time_node];

		++_next_time_node;
		_next_time_node %= _time_max;
	}

	void ruler_system::checkAndUpdate(boost::system_time& tmp)
	{
		if((tmp - tick).total_seconds() < 10)return;
		tick = tmp;

		if(!_bInit)
		{
			_bInit = true;

			for(rulerInfos::iterator iter = _rulerInfos.begin(); iter != _rulerInfos.end(); ++iter)
				iter->repairCurrentInfo(_next_time_node, _next_update_time);
			
			for(rulerInfos::iterator iter = _rulerInfos.begin(); iter != _rulerInfos.end(); ++iter)
			{
				if(!iter->isInited())
				{
					_bInit = false;
					break;
				}
			}
		}
		else
		{
			unsigned now = na::time_helper::get_current_time();
			if(now >= _next_update_time)
			{
				for(unsigned i = 0; i < 3; ++i)
					_rulerInfos[i].doTime(_next_time_node);

				updateTimeNode();
			}

			if(now >= _next_broadcast_time)
				doBroadcast();
		}
	}

	void ruler_system::initBroadcast()
	{
		unsigned now = na::time_helper::get_current_time();
		struct tm tm_struct = na::time_helper::toTm(now);
		unsigned secs = tm_struct.tm_hour * 3600 + tm_struct.tm_min * 60 + tm_struct.tm_sec;
		int season = season_sys.getSeason(now);
		if(season == season_quarter_one)
		{
			if(secs < 5 * HOUR)
			{
				_next_broadcast_state = _time_at_14_00;
				_next_broadcast_time = season_sys.getNextSeasonTime(season_quarter_one, 14);
			}
			else if(secs < 14 * HOUR)
			{
				_next_broadcast_state = _time_at_14_00;
				_next_broadcast_time = now - secs + 14 * HOUR;
			}
			else if(secs < 16 * HOUR)
			{
				_next_broadcast_state = _time_at_16_00;
				_next_broadcast_time = now - secs + 16 * HOUR;
			}
			else if(secs < 18 * HOUR)
			{
				_next_broadcast_state = _time_at_18_00;
				_next_broadcast_time = now - secs + 18 * HOUR;
			}
			else if(secs < 21 * HOUR)
			{
				_next_broadcast_state = _time_at_21_00;
				_next_broadcast_time = now - secs + 21 * HOUR;
			}
			else
			{
				_next_broadcast_state = _time_at_14_00;
				_next_broadcast_time = season_sys.getNextSeasonTime(season_quarter_one, 14);
			}
		}
		else
		{
			_next_broadcast_state = _time_at_14_00;
			_next_broadcast_time = season_sys.getNextSeasonTime(season_quarter_one, 14);
		}
	}

	void ruler_system::doBroadcast()
	{
		if(_next_broadcast_state == _time_at_14_00 || _next_broadcast_state == _time_at_16_00)
			_next_broadcast_time += 2 * HOUR;
		else if(_next_broadcast_state == _time_at_18_00)
			_next_broadcast_time += 3 * HOUR;
		else
			_next_broadcast_time += (4 * DAY - 7 * HOUR);

		if(_next_broadcast_state != _time_at_21_00)
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_ruler_begin);
		else
			chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_ruler_coming);

		++_next_broadcast_state;
		_next_broadcast_state %= _time_at_max;
	}

	void ruler_system::package(playerDataPtr& d, Json::Value& pack)
	{
		d->Ruler.package(pack[msgStr][1u]);
		_rulerInfos[d->Base.getSphereID()].packageCurrentInfo(pack[msgStr][1u]);
		_rulerInfos[d->Base.getSphereID()].packageReportList(pack[msgStr][1u]);
		pack[msgStr][1u][ruler_next_flush_time_field_str] = _rulerInfos[d->Base.getSphereID()].getNextFlushTime();
		unsigned now = na::time_helper::get_current_time();
		pack[msgStr][1u][ruler_betting_cost_type_field_str] = getBettingCostType(now);
		pack[msgStr][1u][ruler_client_state_field_str] = _rulerInfos[d->Base.getSphereID()].getClientState();
		pack[msgStr][1u][ruler_new_report_field_str] = _rulerInfos[d->Base.getSphereID()].isNew(d);
	}

	void ruler_system::rulerUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		rulerCheckCountryId(d);

		updateToPlayer(d);
	}

	void ruler_system::updateToPlayer(playerDataPtr& d)
	{
		unsigned now = na::time_helper::get_current_time();
		d->Ruler.checkAndUpdate(now);

		Json::Value msg;
		package(d, msg);
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ruler_update_resp, msg); 
	}

	void ruler_system::rulerTermInfoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		rulerCheckCountryId(d);

		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		UnAcceptRetrun(js_msg[1u].isInt(), -1);
		int begin = js_msg[0u].asInt();
		int count = js_msg[1u].asInt();
		if(begin < 1 || count < 1)
			Return(r, -1);
		
		Json::Value msg;
		_rulerInfos[d->Base.getSphereID()].packageTermInfoList(msg[msgStr][1u], begin, count);
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ruler_term_info_resp, msg); 
	}

	void ruler_system::rulerChallengeReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		UnAcceptRetrun(js_msg[1u].isInt(), -1);
		int side = js_msg[0u].asInt();
		int country_id = js_msg[1u].asInt();
		
		if((side < 0 || side > 1)
			|| (country_id < 0 || country_id > 2)              // 输入是否合法
			|| (_next_time_node != _time_21_25 && _next_time_node != _time_21_30))           // 是否候选人时间
			Return(r, -1);

		if(_rulerInfos[country_id].getPlayerId(_right_side) == m._player_id         
			|| _rulerInfos[country_id].getPlayerId(_left_side) == m._player_id)      // 已是候选人
			Return(r, 1);

		int target_player_id = _rulerInfos[country_id].getPlayerId(side);

		if(target_player_id == -1)                                  // 候选人为空
		{
			playerDataPtr d = player_mgr.getPlayerMain(m._player_id);
			if(d == playerDataPtr())
			{
				if(m._post_times < 1)
				{
					player_mgr.postMessage(m);
					return;
				}
				else
					Return(r, -1);
			}

			if(d->Base.getSphereID() != country_id)     
				Return(r, -1);

			_rulerInfos[country_id].setBattlePlayerAndSave(d, side, "-1");
			updateToPlayer(d);
			Json::Value msg;
			msg[msgStr][0u] = 0;
			msg[msgStr][1u] = 1;
			msg[msgStr][2u] = "-1";
			player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ruler_challenge_resp, msg);
			return;
		}
		
		playerDataPtr d = player_mgr.getPlayerMain(m._player_id);
		playerDataPtr targetP = player_mgr.getPlayerMain(target_player_id);

		if(d == playerDataPtr() || targetP == playerDataPtr())
		{
			if(m._post_times < 1)
			{
				player_mgr.postMessage(m);
				return;
			}
			else
				Return(r, -1);
		}

		if(d->Base.getSphereID() != country_id)
			Return(r, -1);

		unsigned now = na::time_helper::get_current_time();

		if(!d->Ruler.checkCd(now))                                 // cd
			Return(r, 2);

		if(d->isOperatBattle() || targetP->isOperatBattle())         // 是否正在战斗
			Return(r, 3);

		Battlefield field;
		field.atkSide = battleSide::Create(d);
		field.defSide = battleSide::Create(targetP);
		if(field.atkSide == battleSidePtr() || field.defSide == battleSidePtr())return;
		field.atkSide->setFileName(getFileName(d->Base.getSphereID()));
		d->Ruler.updateAfterBattle(now);
		battle_sys.PostBattle(field, battle_ruler_type);
	}

	void ruler_system::rulerFinalBattleResp(const int battleResult, Battlefield field)
	{
		State::setState(gate_client::ruler_final_battle_inner_resp);
		NumberCounter::Step();

		playerDataPtr d = player_mgr.getPlayerMain(field.atkSide->getSideID());
		playerDataPtr targetP = player_mgr.getPlayerMain(field.defSide->getSideID());
		if(playerDataPtr() == d || playerDataPtr() == targetP)return;

		if(battleResult == 1)
			_rulerInfos[d->Base.getSphereID()].postFinalBattleResp(d, targetP, field.atkSide->getFileName());
		else
			_rulerInfos[d->Base.getSphereID()].postFinalBattleResp(targetP, d, field.atkSide->getFileName());

		helper_mgr.runSaveAndUpdate();
	}

	void ruler_system::rulerChallengeResp(const int battleResult, Battlefield field)
	{
		State::setState(gate_client::ruler_challenge_req);
		NumberCounter::Step();
		playerDataPtr d = player_mgr.getPlayerMain(field.atkSide->getSideID());
		playerDataPtr targetP = player_mgr.getPlayerMain(field.defSide->getSideID());
		if(playerDataPtr() == d || playerDataPtr() == targetP)return;

		updateAfterBattle(d, targetP, battleResult, field);

		updateToPlayer(d);

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = battleResult;
		msg[msgStr][2u] = field.atkSide->getFileName();
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ruler_challenge_resp, msg);

		vector<string> fields;
		fields.push_back(targetP->Base.getName());
		fields.push_back(boost::lexical_cast<string, int>(battleResult));
		StreamLog::Log(rulerLogDBStr, d, "", "", fields, _challenge_log);

		if(battleResult == 1 && rand()%100 < 30)
			postBroadcastReq(_ruler_broadcast_type_0, d->playerID, targetP->playerID, d->Base.getSphereID(), "");

		helper_mgr.runSaveAndUpdate();
	}

	void ruler_system::updateTitleInfoToPlayer(playerDataPtr& d)
	{
		Json::Value msg;
		_rulerInfos[d->Base.getSphereID()].packageTitleInfoList(msg[msgStr][1u]);
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ruler_title_info_resp, msg);
	}

	void ruler_system::rulerTitleInfoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		rulerCheckCountryId(d);

		updateTitleInfoToPlayer(d);
	}
	
	void ruler_system::rulerSetTitleReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		int title = js_msg[0u].asInt();

		playerDataPtr d = player_mgr.getPlayerMain(m._player_id);
		playerDataPtr targetP;
		if(title == _no_title)                                                    // 自动封官
		{
			if(d == playerDataPtr())
			{
				if(m._post_times < 1)
				{
					player_mgr.postMessage(m);
					return;
				}
				else
					Return(r, -1);
			}
		}
		else                                                                                // 手动封官
		{
			UnAcceptRetrun(js_msg[1u].isString(), -1);
			string name = js_msg[1u].asString();
			targetP = player_mgr.getPlayerMainByName(name);

			if(playerDataPtr() == d || playerDataPtr() == targetP)
			{
				if(m._post_times < 1)
				{
					player_mgr.postMessage(m);
					return;
				}
				else
					Return(r, 7);
			}
		}

		rulerCheckCountryId(d);

		if(d->Ruler.getTitle() != _title_king)  // not king
			Return(r, 1);

		if(title == _no_title)
		{
			if(!_rulerInfos[d->Base.getSphereID()].checkAllTitles())            // 手动封官后不能自动封官
				Return(r, 5);

			postToAutoSetTitle(d);
			return;
		}

		if(title <= _title_king || title >= _title_max)
			Return(r, -1);

		if(targetP->Base.getSphereID() != d->Base.getSphereID())       // 非本国玩家
			Return(r, 2);

		if(targetP->Ruler.getTitle() != _no_title)  // 玩家已封官
			Return(r, 3);

		if(!_rulerInfos[d->Base.getSphereID()].checkTitleAndSet(targetP, title))  // 该官职已被封
			Return(r, 4);

		targetP->Ruler.setTitle(title);
		updateTitleInfoToPlayer(d);

		vector<string> fields;
		fields.push_back(targetP->Base.getName());
		fields.push_back(boost::lexical_cast<string, int>(title));
		fields.push_back(boost::lexical_cast<string, int>(d->Base.getSphereID()));
		fields.push_back(boost::lexical_cast<string, int>(_rulerInfos[d->Base.getSphereID()].getLastYear()));
		fields.push_back(boost::lexical_cast<string, int>(targetP->playerID));
		StreamLog::Log(rulerLogDBStr, d, "", "", fields, ruler_system::_set_title_log);

		postBroadcastReq(_ruler_broadcast_type_10, targetP->playerID, -1, targetP->Base.getSphereID(), "", title);

		Return(r, 0);
	}

	void ruler_system::rulerAutoSetTitleResp(playerDataPtr d)
	{
		updateTitleInfoToPlayer(d);
		Json::Value msg;
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ruler_set_title_info_resp, msg);
	}

	void ruler_system::rulerBettingReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		rulerCheckCountryId(d);

		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		int option = js_msg[0u].asInt();

		if(_rulerInfos[d->Base.getSphereID()].getClientState() != _client_state_betting_time
			|| option < _bet_left_side_one 
			|| option > _bet_right_side_two)
			Return(r, -1);

		if(d->Ruler.getBetOption() != _no_bet)   // have bet
			Return(r, 1);

		int country_id = d->Base.getSphereID();

		if(_rulerInfos[country_id].getPlayerId(_left_side) == d->playerID   
			|| _rulerInfos[country_id].getPlayerId(_right_side) == d->playerID)   // one of battle player
			Return(r, 2);

		int cost = getBetNumber(option);
		int before = d->Base.getSilver();

		if(cost > before)   // gold not enough
			Return(r, 3);
		
		d->Base.alterSilver(0 - cost);
		d->Ruler.setBetOption(option);
		d->Ruler.setBetNumber(cost);

		updateToPlayer(d);

		int side = (option == _bet_left_side_one || option == _bet_left_side_two)? _left_side : _right_side;  
		string name = _rulerInfos[country_id].getName(side);

		vector<string> fields;
		fields.push_back(name);
		fields.push_back(boost::lexical_cast<string, int>(cost));
		StreamLog::Log(rulerLogDBStr, d, boost::lexical_cast<string, int>(before), boost::lexical_cast<string, int>(d->Base.getSilver()), fields, _betting_log);

		Return(r, 0);
	}

	void ruler_system::rulerGetRewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		rulerCheckCountryId(d);

		if(d->Ruler.getRewardType() <= gg::_no_reward || d->Ruler.getRewardType() > _loser_reward)
			Return(r, -1);

		bool flag = false;
		if(d->Ruler.getRewardType() == _bet_win_reward)
			flag = true;

		int before = d->Base.getSilver();
		unsigned now = na::time_helper::get_current_time();
		int getsilver = d->Ruler.getReward(getBettingCostType(now));
		if(getsilver == 0)
			Return(r, 2);
		updateToPlayer(d);

		if(flag)
		{
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(getsilver));
			StreamLog::Log(rulerLogDBStr, d, boost::lexical_cast<string, int>(before), boost::lexical_cast<string, int>(d->Base.getSilver()), fields, _betting_reward_log);
		}

		r[msgStr][1u] = getsilver;
		Return(r, 0);
	}

	void ruler_system::rulerClearCdReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		rulerCheckCountryId(d);

		int result = d->Ruler.clearCd();
		if(result != 0)
			Return(r, result);

		updateToPlayer(d);
		Return(r, 0);
	}

	int ruler_system::getBetNumber(int option)
	{
		unsigned now = na::time_helper::get_current_time();
		if(option == _bet_left_side_one || option == _bet_right_side_one)
			return 20000 + getBettingCostType(now) * 20000;
		if(option == _bet_left_side_two || option == _bet_right_side_two)
			return 40000 + getBettingCostType(now) * 40000;
		return 0;
	}

	void ruler_system::updateAfterBattle(playerDataPtr& d, playerDataPtr& targetP, const int battleResult, Battlefield field)
	{
		if(battleResult == 1)
		{
			int side = (targetP->playerID == _rulerInfos[d->Base.getSphereID()].getPlayerId(_left_side))? _left_side : _right_side;
			_rulerInfos[d->Base.getSphereID()].setBattlePlayerAndSave(d, side, field.atkSide->getFileName());
			if(side == _left_side)
				_rulerInfos[d->Base.getSphereID()].addReportAndSave(d, targetP, field.atkSide->getFileName(), _report_left_battle);
			else
				_rulerInfos[d->Base.getSphereID()].addReportAndSave(d, targetP, field.atkSide->getFileName(), _report_right_battle);
		}
	}

	string ruler_system::getFileName(int country_id)
	{
		return boost::lexical_cast<string , int>(_rulerInfos[country_id].getReportId());
	}

	int ruler_system::getPlayerByTitle(int title, int country_id)
	{
		return _rulerInfos[country_id].getPlayerByTitle(title);
	}

	int ruler_system::getWinnerSide(int country_id)
	{
		return _rulerInfos[country_id].getWinnerSide();
	}

	bool ruler_system::isFinished(int country_id)
	{
		return _rulerInfos[country_id].isFinished();
	}

	int ruler_system::getBettingCostType(unsigned now)
	{
		static unsigned next_update_time = 0;

		if(now >= next_update_time)
		{
			int days = (now - season_sys.getServerOpenTime()) / DAY;
			_betting_cost_type = (days / 90) > _betting_cost_three ? _betting_cost_three : (days / 90);
			if(_betting_cost_type == _betting_cost_three)
				next_update_time = 2000000000;
			else
				next_update_time = (_betting_cost_type + 1) * 90 * DAY + season_sys.getServerOpenTime();
		}
		return _betting_cost_type;
	}

	void ruler_system::rulerAutoSetTitleInnerReq(msg_json& m, Json::Value& r)
	{
		playerDataPtr d = player_mgr.getPlayerMain(m._player_id);
		ReadJsonArray;
		
		vector<playerDataPtr> vec;
		for(unsigned i = 0; i < js_msg.size(); ++i)
		{
			playerDataPtr targetP = player_mgr.getPlayerMain(js_msg[i].asInt());
			if(targetP != playerDataPtr())
				vec.push_back(targetP);
		}

		if(d == playerDataPtr() || vec.size() != js_msg.size())
		{
			if(m._post_times < 2)
			{
				player_mgr.postMessage(m);
				return;
			}
			else
			{
				Json::Value msg;
				msg[msgStr][0u] = -1;
				player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ruler_set_title_info_resp, msg);
				return;
			}
		}

		if(!_rulerInfos[d->Base.getSphereID()].checkAllTitles())            // 手动封官后不能自动封官
		{
			Json::Value msg;
			msg[msgStr][0u] = 5;
			player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ruler_set_title_info_resp, msg);
			return;
		}

		int title = _title_king + 1;
		for(vector<playerDataPtr>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			if(title == _title_max)
				break;
			if((*iter)->playerID == d->playerID)
				continue;
			(*iter)->Ruler.setTitle(title);

			_rulerInfos[d->Base.getSphereID()].checkTitleAndSet(*iter, title);

			vector<string> fields;
			fields.push_back((*iter)->Base.getName());
			fields.push_back(boost::lexical_cast<string, int>((*iter)->Arena.getRank()));
			fields.push_back(boost::lexical_cast<string, int>(title));
			fields.push_back(boost::lexical_cast<string, int>(d->Base.getSphereID()));
			fields.push_back(boost::lexical_cast<string, int>(_rulerInfos[d->Base.getSphereID()].getLastYear()));
			fields.push_back(boost::lexical_cast<string, int>((*iter)->playerID));
			StreamLog::Log(rulerLogDBStr, d, "", "", fields, ruler_system::_auto_set_title_log);

			postBroadcastReq(_ruler_broadcast_type_10, (*iter)->playerID, -1, (*iter)->Base.getSphereID(), "", title);

			++title;
		}
		
		rulerAutoSetTitleResp(d);
	}

	void ruler_system::rulerFinalBattleInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int country_id = js_msg[0u].asInt();

		int player_id = _rulerInfos[country_id].getPlayerId(_left_side);
		int target_player_id = _rulerInfos[country_id].getPlayerId(_right_side);

		if(player_id == -1 || target_player_id == -1)
			return;

		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		playerDataPtr targetP = player_mgr.getPlayerMain(target_player_id);
		if(d == playerDataPtr() || targetP == playerDataPtr())
		{
			if(m._post_times < 3)
				player_mgr.postMessage(m);
			else
				LogE << __FUNCTION__ << " fail to load player data: " << player_id << ", " << target_player_id << LogEnd;
			return;
		}
		
		if(d->isOperatBattle() || targetP->isOperatBattle())
		{
			_rulerInfos[country_id].postFinalBattleReq();
			return;
		}

		int side = _rulerInfos[d->Base.getSphereID()].getFinalBattleSide();

		Battlefield field;
		if(side == _left_side)
		{
			field.atkSide = battleSide::Create(d);
			field.defSide = battleSide::Create(targetP);
		}
		else
		{
			field.atkSide = battleSide::Create(targetP);
			field.defSide = battleSide::Create(d);
		}
		if(field.atkSide == battleSidePtr() || field.defSide == battleSidePtr())return;
		field.atkSide->setFileName(getFileName(d->Base.getSphereID()));
		battle_sys.PostBattle(field, battle_ruler_final_type);
	}

	void ruler_system::postToAutoSetTitle(playerDataPtr d)
	{
		Json::Value msg;
		arena_sys.getAutoTitledPlayers(d->Base.getSphereID(), msg[msgStr]);
		if(msg[msgStr].size() == 0)
		{
			Json::Value msg;
			msg[msgStr][0u] = 6;
			player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ruler_set_title_info_resp, msg);
			return;
		}

		string str = msg.toStyledString();
		na::msg::msg_json auto_m(d->playerID, -1, gate_client::ruler_auto_set_title_info_inner_req, str);
		player_mgr.postMessage(auto_m);
	}

	int ruler_system::getFinalBattlePlayer(int country_id, int side)
	{
		return _rulerInfos[country_id].getFinalBattlePlayer(side);
	}

	void ruler_system::postBroadcastReq(int type, int a_player_id, int b_player_id, int country_id, string report_id, int title)
	{
		Json::Value pid_array;
		if(a_player_id != -1)
			pid_array.append(a_player_id);
		if(b_player_id != -1)
			pid_array.append(b_player_id);

		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(type);
		msg[msgStr].append(pid_array);
		msg[msgStr].append(country_id);
		msg[msgStr].append(report_id);
		msg[msgStr].append(title);
		string str = msg.toStyledString();
		na::msg::msg_json m(gate_client::ruler_broadcast_inner_req, str);
		player_mgr.postMessage(m);
	}

	void ruler_system::rulerBroadcastInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int type = js_msg[0u].asInt();
		const Json::Value& pid_array = js_msg[1u];
		int country_id = js_msg[2u].asInt();
		string report = js_msg[3u].asString();
		int title = js_msg[4u].asInt();
		vector<playerDataPtr> vec;

		for(unsigned i = 0; i < pid_array.size(); ++i)
		{
			playerDataPtr d = player_mgr.getPlayerMain(pid_array[i].asInt());
			if(d != playerDataPtr())
				vec.push_back(d);
		}

		if(vec.size() != pid_array.size())
		{
			if(m._post_times < 2)
				player_mgr.postMessage(m);
			return;
		}

		Json::Value msg;
		msg[ruler_broadcast_type_field_str] = type;
		if(vec.size() > 0)
			chat_sys.packageSender(vec[0], msg["AAA"]);
		if(vec.size() > 1)
			chat_sys.packageSender(vec[1], msg["BBB"]);

		if(report != "")
			msg[ruler_report_id_field_str] = report;
		if(country_id != -1)
			msg[ruler_country_id_field_str] = country_id;
		if(title != -1)
			msg[ruler_title_field_str] = title;

		if(type == _ruler_broadcast_type_0 || type == _ruler_broadcast_type_10)
			chat_sys.chatRulerBroadcastResp(msg, 0, country_id);
		else
			chat_sys.chatRulerBroadcastResp(msg, -1);
	}

	void ruler_system::updatePlayerInfo(playerDataPtr d, int type)
	{
		if(d->Base.getSphereID() >= 0 && d->Base.getSphereID() <= 2)
			_rulerInfos[d->Base.getSphereID()].alterPlayerInfo(d, type);
	}

	void ruler_system::rulerBroadcastTestReq(msg_json& m, Json::Value& r)
	{
		postBroadcastReq(_ruler_broadcast_type_0, 187695123, 187695129, 0, "");
		postBroadcastReq(_ruler_broadcast_type_1, -1, -1, -1, "");
		postBroadcastReq(_ruler_broadcast_type_2, -1, -1, -1, "");
		postBroadcastReq(_ruler_broadcast_type_3, 187695123, -1, 0, "");
		postBroadcastReq(_ruler_broadcast_type_4, 187695123, -1, 0, "");
		postBroadcastReq(_ruler_broadcast_type_5, -1, -1, 0, "");
		postBroadcastReq(_ruler_broadcast_type_6, 187695123, 187695129, 0, "1");
		postBroadcastReq(_ruler_broadcast_type_7, 187695123, 187695129, 0, "1");
		postBroadcastReq(_ruler_broadcast_type_8, 187695123, 187695129, 0, "1");
		postBroadcastReq(_ruler_broadcast_type_9, 187695123, 187695129, 0, "1");
		postBroadcastReq(_ruler_broadcast_type_10, 187695123, -1, 0, "", 2);
		postBroadcastReq(_ruler_broadcast_type_11, -1, -1, -1, "");
	}

	void ruler_system::getValue(Json::Value& info, int country_id)
	{
		if (country_id == -1)
		{
			info = Json::arrayValue;
			for (rulerInfos::iterator iter = _rulerInfos.begin(); iter != _rulerInfos.end(); ++iter)
			{
				Json::Value temp;
				iter->getKingValue(temp);
				info.append(temp);
			}
		}
		else
		{
			_rulerInfos[country_id].getKingValue(info);
		}
	}

	void ruler_system::getOfficerVec(set<int>& officer_vec, int country_id)
	{
		_rulerInfos[country_id].getOfficerVec(officer_vec);
	}

	void ruler_system::rulerBroadcast(int type)
	{
		if (type != _broadcast_at_21_35)
		{
			Json::Value msg;
			msg[msgStr][0u] = 0;
			msg[msgStr][1u][chatBroadcastID] = BROADCAST::ruler_running_broadcast;
			msg[msgStr][1u][senderChannelStr] = -1;
			msg[msgStr][1u]["pl"].append(type);
			player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);
		}

		if (type != _broadcast_at_18_00)
		{
			int r_type = -1;
			switch (type)
			{
				case _broadcast_at_21_25:
					r_type = ROLLBROADCAST::roll_ruler_running_broadcast_21_25;
					break;
				case _broadcast_at_21_30:
					r_type = ROLLBROADCAST::roll_ruler_running_broadcast_21_30;
					break;
				case _broadcast_at_21_35:
					r_type = ROLLBROADCAST::roll_ruler_running_broadcast_21_35;
					break;
				default:
					return;
			}
			chat_sys.sendToAllRollBroadCastCommon(r_type);
		}
	}
}