#include "strategy_system.h"
#include "battle_system.h"
#include "time_helper.h"
#include "response_def.h"
#include "war_story.h"
#include "chat_system.h"

namespace gg
{
	void strategyPlayerInfo::set(playerDataPtr d, const string& report)
	{
		_player_id = d->playerID;
		_nickname = d->Base.getName();
		_country_id = d->Base.getSphereID();
		_lv = d->Base.getLevel();
		_report_id = report;
	}

	void strategyPlayerInfo::set(mongo::BSONElement& obj)
	{
		checkNotEoo(obj[strategy_player_id_field_str])
			_player_id = obj[strategy_player_id_field_str].Int();
		checkNotEoo(obj[strategy_nickname_field_str])
			_nickname = obj[strategy_nickname_field_str].String();
		checkNotEoo(obj[strategy_country_id_field_str])
			_country_id = obj[strategy_country_id_field_str].Int();
		checkNotEoo(obj[strategy_level_field_str])
			_lv = obj[strategy_level_field_str].Int();
		checkNotEoo(obj[strategy_report_id_field_str])
			_report_id = obj[strategy_report_id_field_str].String();
	}

	void strategyPlayerInfo::package(Json::Value& info)
	{
		info = Json::arrayValue;
		info.append(_player_id);
		info.append(_nickname);
		info.append(_country_id);
		info.append(_lv);
		info.append(_report_id);
	}

	mongo::BSONObj strategyPlayerInfo::toBSON()
	{
		mongo::BSONObj obj = BSON(strategy_player_id_field_str << _player_id
			<< strategy_nickname_field_str << _nickname
			<< strategy_country_id_field_str << _country_id
			<< strategy_level_field_str << _lv
			<< strategy_report_id_field_str << _report_id);
		return obj;
	}

	void firstStrategyPlayerInfo::set(mongo::BSONElement obj)
	{
		strategyPlayerInfo::set(obj);
		checkNotEoo(obj[strategy_time_field_str])
			_time = obj[strategy_time_field_str].Int();
	}

	void firstStrategyPlayerInfo::set(playerDataPtr d, const string& report)
	{
		strategyPlayerInfo::set(d, report);
		_time = na::time_helper::get_current_time();
	}

	void firstStrategyPlayerInfo::package(Json::Value& info)
	{
		strategyPlayerInfo::package(info);
		info.append(_time);
	}

	mongo::BSONObj firstStrategyPlayerInfo::toBSON()
	{
		mongo::BSONObj obj = BSON(strategy_player_id_field_str << getPlayerId()
			<< strategy_nickname_field_str << getNickname()
			<< strategy_country_id_field_str << getCountryId()
			<< strategy_level_field_str << getLv()
			<< strategy_report_id_field_str << getReportId()
			<< strategy_time_field_str << _time);
		return obj;
	}

	bool bestStrategyPlayerInfo::isBetter(playerDataPtr d, Battlefield field, Json::Value& report)
	{
		if(isEmpty())
			return true;
		if(d->Base.getLevel() != getLv())
			return d->Base.getLevel() < getLv();
		int rd = (int)report[BATTLE::strBigRound].size();
		if(rd != _round)
			return rd < _round;
		if(field.atkSide->getLostArmyNum() != _lost)
			return field.atkSide->getLostArmyNum() < _lost;
		return false;
	}

	mongo::BSONObj bestStrategyPlayerInfo::toBSON()
	{
		mongo::BSONObj obj = BSON(strategy_player_id_field_str << getPlayerId()
			<< strategy_nickname_field_str << getNickname()
			<< strategy_country_id_field_str << getCountryId()
			<< strategy_level_field_str << getLv()
			<< strategy_report_id_field_str << getReportId()
			<< strategy_round_field_str << _round
			<< strategy_lost_army_field_str << _lost
			<< strategy_time_field_str << getTime());
		return obj;
	}

	void bestStrategyPlayerInfo::set(playerDataPtr d, const string& report, Battlefield field, Json::Value& reportJson)
	{
		firstStrategyPlayerInfo::set(d, report);
		_round = (int)reportJson[BATTLE::strBigRound].size();
		_lost = field.atkSide->getLostArmyNum();
	}

	void bestStrategyPlayerInfo::set(mongo::BSONElement obj)
	{
		firstStrategyPlayerInfo::set(obj);
		checkNotEoo(obj[strategy_round_field_str])
			_round = obj[strategy_round_field_str].Int();
		checkNotEoo(obj[strategy_lost_army_field_str])
			_lost = obj[strategy_lost_army_field_str].Int();
	}

	string strategyPlayerInfos::_index[_strategy_last_pass_report_id_end];

	bool strategyPlayerInfos::initIndex()
	{
		for(int i = 0; i < _strategy_last_pass_report_id_end; ++i)
			_index[i] = boost::lexical_cast<string , int>(i);
		return true;
	}

	strategyPlayerInfos::strategyPlayerInfos(int local_id)
		: _local_id(local_id), _current_id(_strategy_last_pass_report_id_begin - 1)
	{
		static bool flag = initIndex();
	}

	bool strategyPlayerInfos::save()
	{		
		mongo::BSONObj key = BSON(strategy_local_id_field_str << _local_id);

		mongo::BSONObjBuilder obj;
		obj << strategy_local_id_field_str << _local_id
			<< strategy_current_id_field_str << _current_id
			<< strategy_first_pass_player_field_str << _first_pass_player.toBSON()
			<< strategy_best_pass_player_field_str << _best_pass_player.toBSON();
		{
			mongo::BSONArrayBuilder b;
			for(list<strategyPlayerInfo>::iterator iter = _last_pass_players.begin(); iter != _last_pass_players.end(); ++iter)
			{
				b.append(iter->toBSON());
			}
			obj << strategy_last_pass_player_field_str << b.arr();
		}

		return db_mgr.save_mongo(strategy_info_db_str, key, obj.obj());
	}

	void strategyPlayerInfos::setFromBSON(mongo::BSONObj& obj)
	{
		checkNotEoo(obj[strategy_local_id_field_str])
			_local_id = obj[strategy_local_id_field_str].Int();
		checkNotEoo(obj[strategy_current_id_field_str])
			_current_id = obj[strategy_current_id_field_str].Int();
		checkNotEoo(obj[strategy_first_pass_player_field_str])
			_first_pass_player.set(obj[strategy_first_pass_player_field_str]);
		checkNotEoo(obj[strategy_best_pass_player_field_str])
			_best_pass_player.set(obj[strategy_best_pass_player_field_str]);
		checkNotEoo(obj[strategy_last_pass_player_field_str])
		{
			vector<mongo::BSONElement> ele = obj[strategy_last_pass_player_field_str].Array();
			vector<strategyPlayerInfo> playerInfos;
			for(unsigned i = 0; i < ele.size(); ++i)
			{
				strategyPlayerInfo info;
				info.set(ele[i]);
				playerInfos.push_back(info);
			}
			unsigned begin = _current_id - _strategy_last_pass_report_id_begin + 1;
			if(begin >= _strategy_last_pass_num)
				begin = 0;
			for(unsigned i = begin; i < playerInfos.size(); ++i)
				_last_pass_players.push_back(playerInfos[i]);
			for(unsigned i = 0; i < begin; ++i)
				_last_pass_players.push_back(playerInfos[i]);
		}
	}

	void strategyPlayerInfos::package(Json::Value& info)
	{
		if(!_first_pass_player.isEmpty())
			_first_pass_player.package(info[strategy_first_pass_player_field_str]);
		else
			info[strategy_first_pass_player_field_str] = Json::nullValue;

		if(!_best_pass_player.isEmpty())
			_best_pass_player.package(info[strategy_best_pass_player_field_str]);
		else
			info[strategy_best_pass_player_field_str] = Json::nullValue;

		if(!_last_pass_players.empty())
		{
			info[strategy_last_pass_player_field_str]  = Json::arrayValue;
			Json::Value& ref = info[strategy_last_pass_player_field_str];
			for(list<strategyPlayerInfo>::iterator iter = _last_pass_players.begin(); iter != _last_pass_players.end(); ++iter)
			{
				Json::Value last;
				iter->package(last);
				ref.append(last);
			}
		}
		else
			info[strategy_last_pass_player_field_str] = Json::arrayValue;
	}

	bool strategyPlayerInfos::updateFirstPassPlayer(playerDataPtr d, Battlefield field, Json::Value& report)
	{
		if(_first_pass_player.isEmpty())
		{
			int local_id = field.defSide->getSideID();
			string filename = reportStrategyPVEDir;
			string temp;
			temp += boost::lexical_cast<string, int>(local_id);
			temp += "/";
			temp += _index[_strategy_first_pass_report_id];
			filename += temp;
//			battle_sys.DealStrategyReport(filename, report);
			_Battle_Post(boost::bind(&battle_system::DealStrategyReport, battle_system::battleSys, 
				filename, report));
			field.atkSide->setFileName(temp);
			_first_pass_player.set(d, _index[_strategy_first_pass_report_id]);
			return true;
		}
		return false;
	}

	bool strategyPlayerInfos::updateBestPassPlayer(playerDataPtr d, Battlefield field, Json::Value& report)
	{
		if(_best_pass_player.isBetter(d, field, report))
		{
			int local_id = field.defSide->getSideID();
			string filename = reportStrategyPVEDir;
			filename += boost::lexical_cast<string , int>(local_id);
			filename += "/";
			filename += _index[_strategy_best_pass_report_id];
//			battle_sys.DealStrategyReport(filename, report);
			_Battle_Post(boost::bind(&battle_system::DealStrategyReport, battle_system::battleSys, 
				filename, report));
			_best_pass_player.set(d, _index[_strategy_best_pass_report_id], field, report);
			return true;
		}
		return false;
	}

	bool strategyPlayerInfos::updateLastPassPlayer(playerDataPtr d, Battlefield field, Json::Value& report)
	{
		int local_id = field.defSide->getSideID();
		if(d->Warstory.currentProcess < local_id)
		{
			_last_pass_players.push_front(strategyPlayerInfo());

			while(_last_pass_players.size() > _strategy_last_pass_num)
				_last_pass_players.pop_back();

			++_current_id;
			if(_current_id >= _strategy_last_pass_num + _strategy_last_pass_report_id_begin)
				_current_id = _strategy_last_pass_report_id_begin;

			string filename = reportStrategyPVEDir;
			filename += boost::lexical_cast<string , int>(local_id);
			filename += "/";
			filename += _index[_current_id];
//			battle_sys.DealStrategyReport(filename, report);
			_Battle_Post(boost::bind(&battle_system::DealStrategyReport, battle_system::battleSys, 
				filename, report));
			_last_pass_players.front().set(d, _index[_current_id]);
			return true;
		}
		return false;
	}

	strategy_system* const strategy_system::strategySys = new strategy_system;

	void strategy_system::initData()
	{
		db_mgr.ensure_index(strategy_info_db_str, BSON(strategy_local_id_field_str << 1));

		objCollection objs = db_mgr.Query(strategy_info_db_str);

		for(unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];
			checkNotEoo(obj[strategy_local_id_field_str])
			{
				int local_id = obj[strategy_local_id_field_str].Int();
				strategyPlayerInfos info(local_id);
				info.setFromBSON(obj);
				_maps.insert(make_pair(local_id, info));
			}
		}

		string path = reportStrategyPVEDir;
		boost::filesystem::path path_dir(path);
		if(!boost::filesystem::exists(path))
			boost::filesystem::create_directory(path);

		vector<int> allMap = war_story.getAllMapLocalID();
		for(vector<int>::iterator iter = allMap.begin(); iter != allMap.end(); ++iter)
		{
			string path = reportStrategyPVEDir;
			path += boost::lexical_cast<string , int>(*iter);
			path += "/";
			boost::filesystem::path path_dir(path);
			if(!boost::filesystem::exists(path))
				boost::filesystem::create_directory(path);
		}
	}

	void strategy_system::updateInfo(playerDataPtr d, Battlefield field, Json::Value& report, int type)
	{
		int local_id = field.defSide->getSideID();
		LocalId2PlayerInfos::iterator iter;
		if((iter = _maps.find(local_id)) == _maps.end())
		{
			_maps.insert(make_pair(local_id, strategyPlayerInfos(local_id)));
			iter = _maps.find(local_id);
		}

		bool flag = false;
		if(iter->second.updateFirstPassPlayer(d, field, report))
		{
			flag = true;
			if(type == 1 || type == 2)
			{
				string str = field.atkSide->getFileName();
				chat_sys.chatStrategyBroadcastResp(d, local_id, str);
			}
		}
		if(iter->second.updateLastPassPlayer(d, field, report))
			flag = true;
		if(iter->second.updateBestPassPlayer(d, field, report))
			flag = true;
		if(flag)
			iter->second.save();
	}

	void strategy_system::strategyUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		int local_id = js_msg[0u].asInt();

		LocalId2PlayerInfos::iterator iter = _maps.find(local_id);
		if(iter == _maps.end())
			Return(r, 1);

		Json::Value msg;
		iter->second.package(msg[msgStr][1u]);
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::strategy_update_resp, msg);
	}
}
