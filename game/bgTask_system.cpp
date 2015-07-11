#include "bgTask_system.h"

namespace gg
{
	bgTask_box::bgTask_box(const Json::Value& info)
	{
		_integral = info["i"].asInt();
		_reward.setValue(info["r"]);
	}

	void bgTask_detail::load(const Json::Value& info)
	{
		_key_id = info["kid"].asInt();
		_begin_time = info["bt"].asUInt();
		_end_time = info["et"].asUInt();
		const Json::Value& page = info["pg"];
		for(unsigned i = 0; i < page.size(); ++i)
		{
			bool is_daily = page[i]["r"].asBool();
			const Json::Value& tl = page[i]["l"];
			for(unsigned j = 0; j < tl.size(); ++j)
			{
				bgTask::infoPtr ptr = creator<bgTask::taskInfo>::run();
				ptr->load(tl[i], is_daily);	
				_bgTask_map.insert(make_pair(ptr->getId(), ptr));
			}
		}
		const Json::Value& box = info["bx"];
		for(unsigned i = 0; i < box.size(); ++i)
			_box_list.push_back(bgTask_box(box[i]));

		_value = info;

		_begin_timer_id = Timer::AddEventTickTimeID(boostBind(bgTask_system::tickBegin, this, _key_id), _begin_time);
		_end_timer_id = Timer::AddEventTickTimeID(boostBind(bgTask_system::tickEnd, this, _key_id), _end_time);
	}
	
	mongo::BSONObj bgTask_detail::toBSON()
	{
		string str = common_sys.json2string(_value);
		return mongo::fromjson(str);
	}	

	bgTask_system* const bgTask_system::bgTaskSys = new bgTask_system;

	void bgTask_system::initData()
	{
		loadDB();
	}

	void bgTask_system::info(playerDataPtr d)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		if(_cur_ptr == NULL)
			msg[msgStr][1u]["kid"] = -1;
		else
			msg[msgStr][1u] = _cur_ptr->_value;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::bgTask_info_resp, msg);
	}

	void bgTask_system::infoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		info(d);
	}

	void bgTask_system::taskRewardReq(msg_json& m, Json::Value& r)
	{

	}

	void bgTask_system::integralRewardReq(msg_json& m, Json::Value& r)
	{

	}

	void bgTask_system::gmInfoReq(msg_json& m, Json::Value& r)
	{
		Json::Value msg = Json::arrayValue;
		ForEach(detailPtr_list, iter, _detailPtr_list)
			msg.append((*iter)->_value);
		r[msgStr][1u] = msg;
		Return(r, 0);
	}

	void bgTask_system::gmModifyReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int key_id = js_msg["kid"].asInt();
		ForEach(detailPtr_list, iter, _detailPtr_list)
		{
			if((*iter)->_key_id == key_id)
			{
				if((*iter) == _cur_ptr)
				{
					_cur_ptr = NULL;
					_client_info = Json::nullValue;
					_client_info["kid"] = -1;
				}

				if((*iter)->_begin_timer_id != -1)
					Timer::RemoveEvent((*iter)->_begin_timer_id);
				if((*iter)->_end_timer_id != -1)
					Timer::RemoveEvent((*iter)->_end_timer_id);
				_detail_list.erase(iter);
				break;
			}
		}
		unsigned end_time = js_msg["et"].asUInt();
		if(end_time != 0)
		{
			detailPtr ptr = creator<bgTask_detail>::run();
			ptr->load(js_msg);
			detailPtr_list.push_back(ptr);
		}

		saveDB();
		Return(r, 0);
	}

	void bgTask_system::update(playerDataPtr d, int type, int arg)
	{
		if(_cur_ptr == NULL)
			return;
		if(d->BgTask.update(type, arg))
		{
			// red point
		}
	}

	bgTask::infoPtr bgTask_system::getTaskPtr(int id)
	{
		if(_cur_ptr == NULL)
			return bgTask::infoPtr();
		bgTask_map::iterator iter = _cur_ptr->_bgTask_map.find(id);
		if(iter == _cur_ptr->_bgTask_map.end())
			return bgTask::infoPtr();
		return iter->second;
	}

	void bgTask_system::initRecord(playerDataPtr d)
	{
		ForEach(bgTask_map, iter, _cur_bgTask_detail._bgTask_map)
			d->BgTask.push(iter->second);
	}

	void bgTask_system::tickBegin(int key_id)
	{
		ForEach(detailPtr_list, iter, _detailPtr_list)
		{
			if((*iter)->_key_id == key_id)
			{
				_cur_ptr = *iter;
				_cur_ptr->_begin_timer_id = -1;
				break;
			}
		}
	}

	void bgTask_system::tickEnd(int key_id)
	{
		ForEach(detailPtr_list, iter, _detailPtr_list)
		{
			if((*iter)->_key_id == key_id)
			{
				if(_cur_ptr == (*iter))
					_cur_ptr = NULL;
				_detailPtr_list.erase(iter);	
				saveDB();
				break;
			}
		}
	}

	void bgTask_system::loadDB()
	{
		objCollection objs = db_mgr.Query(bgTask_info_list_db_str);
		if(!objs.empty())
		{
			const mongo::BSONObj& obj = objs.front();
			checkNotObj(obj["v"])
			{
				vector<mongo::BSONElement> ele = obj["v"].Array();
				for(unsigned i = 0; i < ele.size(); ++i)
				{
					string str = ele[i].jsonString();
					Json::Value temp;
					Json::Reader reader;
					reader.parse(str, temp);
					bgTask::infoPtr ptr = creator<bgTask::taskInfo>::run();
					ptr->load(temp);
					detailPtr_list.push_back(ptr);
				}
			}
		}
	}

	void bgTask_system::saveDB()
	{
		mongo::BSONObj key = BSON("i" << 0);
		mongo::BSONObjBuilder obj;
		obj << "i" << 0;
		if(!_detailPtr_list.empty())
		{
			mongo::BSONArrayBuilder array_b;
			ForEach(detailPtr_list, iter, _detailPtr_list)
				array_b.append((*iter)->toBSON());
			obj << "v" << array_b.arr();
		}
		db_mgr.save_mongo(bgTask_info_list_db_str, key, obj.obj());
	}
}
