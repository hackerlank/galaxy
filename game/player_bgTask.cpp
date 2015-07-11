#include "player_bgTask.h"

namespace gg
{
	namespace bgTask
	{
		record::record(const mongo::BSONElement& obj)
			_ptr(NULL)
		{
			_id = obj["i"].Int();
			_value = obj["v"].Int();
			_state = obj["s"].Int();
		}

		record::record(bgTask::infoPtr ptr, playerDataPtr d)
			: _ptr(ptr)
		{
			_id = _ptr->getId();
			_state = _ptr->update(d, _value, -1);
		}

		void record::package(Json::Value& info)
		{
			info.append(_id);
			info.append(_value);
			info.append(_state);
		}

		mongo::BSONObj record::toBSON()
		{
			return BSON(strOf_id() << _id
				<< strOf_value() << _value
				<< strOf_state() << _state);
		}
	}

	playerBgTask::playerBgTask(playerData& own)
		: Block(own)
	{
		
	}

	bool playerBgTask::get()
	{
		getBegin(bgTaskDBStr);
		checkNotObj(obj[strOf_key_id()])
			_key_id = obj[strOf_key_id()].Int();
		checkNotObj(obj[strOf_next_update_time()])
			_next_update_time = obj[strOf_next_update_time()].Int();
		checkNotObj(obj[strOf_record_map()])
		{
			vector<mongo::BSONElement> ele = obj[strOf_record_map()].Array();
			for(unsigned i = 0; i < ele.size(); ++i)
			{
				const mongo::BSONElement& temp = ele[i];
				int id = temp["i"].Int();
				_record_map.insert(make_pair(id, bgTask::record(temp)));
			}
		}
		getEnd();
	}

	bool playerBgTask::save()
	{
		saveBegin();
		obj << strOf_key_id() << _key_id << strOf_next_update_time() << _next_update_time;
		mongo::BSONArrayBuilder b;
		For_Each(record_map, iter, _record_map)
			b.append(iter->second.toBSON());
		obj << strOf_record_map() << b.arr();
		saveEnd(bgTaskDBStr);
	}

	void playerBgTask::autoUpdate()
	{
		update();
	}

	void playerBgTask::update()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		Json::Value& ref = msg[msgStr][1u];
		ForEach(record_map, iter, _record_map)
		{
			
		}
	}

	void playerBgTask::checkAndUpdate(unsigned now)
	{
		if(_key_id != bgTask_sys.getKeyId())
		{
			_key_id = bgTask_sys.getKeyId();
			
			if(_key_id != -1)
			{
				// get record
				_next_update_time = na::time_helper::get_current_time();
			}

			helper_mgr.insertSave();
		}	

		if(_key_id == -1)
			return;

		if(now >= _next_update_time)
		{
			_next_update_time = na::time_helper::get_current_time();
			
			
		}
	}

	bool playerBgTask::update(int type, int arg)
	{
		if(!_running_types.test(type))
			return false;
			
		const bgTask_system::task_id_list& id_list = bgTask_sys.getTaskIdList(type);
		
		bool save_flag = false;
		bool finish_flag = false;
		bool type_flag = false;
		record_map::iterator iter;
		ForEachC(bgTask_system::task_id_list, it, id_list)
		{
			iter = _record_map.find(*it);
			if(iter == _record_map.end())
			{
				bgTask::infoPtr ptr = bgTask_sys.getTaskPtr(*it);
				push(ptr);
				iter = _record_map.find(*it);
			}
			
			if(iter->second->getState() != bgTask::_running)
				continue;

			int temp = iter->second->getValue();
			iter->second->update(own.getOwnDataPtr(), arg);
			if(iter->second->getValue() != temp)
				save_flag = true;
			if(iter->second->getState() == bgTask::_finished)
				finish_flag = true;
			else
				type_flag = true;
		}

		if(save_flag)
			helper_mgr.insertSave(this);
		if(!type_flag)
			_running_types.reset(type);
		return finish_flag;
	}

	void playerBgTask::push(bgTask::infoPtr ptr)
	{
		bgTask::record rcd(ptr, own.getOwnDataPtr());
		_record_map.insert(make_pair(ptr->getId(), rcd));
	}
}
