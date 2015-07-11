#include "player_sign.h"
#include "helper.h"
#include "sign_system.h"

namespace gg
{
	playerSign::playerSign(playerData& own)
		: Block(own), _is_first(true), _next_update_time(0), _total_days(0)
	{

	}

	void playerSign::checkAndUpdate()
	{
		unsigned now = na::time_helper::get_current_time();
		if(now >= _next_update_time)
		{
			_total_days += 1;
			_next_update_time = na::time_helper::get_next_update_time(now);

			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	void playerSign::alterData(int type, int index)
	{
		if(type == 0)
			_records.set(index - 1);
		else
			_records.set(_cycle_days + index - 1);

		if(_records.count() == _cycle_days + _total_rewards)
		{
			_records.reset();
			_is_first = false;
			_total_days -= _cycle_days;
		}

		helper_mgr.insertSaveAndUpdate(this);
	}

	bool playerSign::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(player_sign_db_str, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[player_sign_total_days_field_str]){_total_days = obj[player_sign_total_days_field_str].Int();}
			checkNotEoo(obj[player_sign_is_first_time_field_str]){_is_first = obj[player_sign_is_first_time_field_str].Bool();}
			checkNotEoo(obj[player_sign_next_update_time_field_str]){_next_update_time = obj[player_sign_next_update_time_field_str].Int();}
			checkNotEoo(obj[player_sign_records_field_str]){_records = record_set(obj[player_sign_records_field_str].String());}	
		}
		return true;
	}

	bool playerSign::save(){
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = BSON(playerIDStr << own.playerID << 
			player_sign_total_days_field_str << _total_days <<
			player_sign_is_first_time_field_str << _is_first << 
			player_sign_next_update_time_field_str << _next_update_time << 
			player_sign_records_field_str << _records.to_string());
		return db_mgr.save_mongo(player_sign_db_str, key, obj);
	}

	void playerSign::package(Json::Value& msg)
	{
		msg[player_sign_total_days_field_str] = _total_days > _cycle_days? _cycle_days : _total_days;
		msg[player_sign_is_first_time_field_str] = _is_first;

		msg[player_sign_records_per_day_field_str] = Json::arrayValue;
		Json::Value& ref1 = msg[player_sign_records_per_day_field_str];
		for(int i = 0; i < _cycle_days; ++i)
			ref1.append((int)_records.test(i));

		msg[player_sign_records_total_field_str] = Json::arrayValue;
		Json::Value& ref2 = msg[player_sign_records_total_field_str];
		for(int i = _cycle_days; i < _cycle_days + _total_rewards; ++i)
			ref2.append((int)_records.test(i));
	}

	void playerSign::update()
	{
		checkAndUpdate();

		Json::Value msg;
		msg[msgStr][0u] = 0;
		package(msg[msgStr][1u]);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::sign_update_resp, msg);
	}

	bool playerSign::checkRecord(int type, int index)
	{
		if(type == 0)
			return !_records.test(index - 1);
		else 
			return !_records.test(index + _cycle_days - 1);
	}

	void playerSign::playerLogin()
	{
		update();
	}

	void playerSign::playerLogout()
	{
		checkAndUpdate();
	}
}