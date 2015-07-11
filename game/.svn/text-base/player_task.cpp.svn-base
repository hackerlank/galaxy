#include "player_task.h"
#include "helper.h"
#include "mongoDB.h"
#include "task_system.h"

namespace gg
{
	mainTaskRecord::mainTaskRecord(const mongo::BSONElement& obj)
	{
		_task_id = obj[task_main_task_id_field_str].Int();
		_state = obj[task_main_task_state_field_str].Int();
	}

	inline mongo::BSONObj mainTaskRecord::toBSON() const
	{
		return BSON(task_main_task_id_field_str << _task_id
			<< task_main_task_state_field_str << _state);
	}

	inline void mainTaskRecord::getValue(Json::Value& info) const
	{
		info.append(_task_id);
		info.append(_state);
	}

	playerTask::playerTask(playerData& own)
		: Block(own), _max_branch_task_type(0), _branch_task_count(0), _bInit(false)
	{
		_mainTaskRecords.insert(mainTaskRecord(task_sys.getMainTaskBeginId(), _task_running));
		_branch_finished_records.push_back(0);
	}

	bool playerTask::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(taskDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[task_main_task_list_field_str])
			{
				_mainTaskRecords.clear();
				vector<mongo::BSONElement> ele = obj[task_main_task_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
					_mainTaskRecords.insert(mainTaskRecord(ele[i]));
			}
			checkNotEoo(obj[task_branch_task_count_field_str]){_branch_task_count = obj[task_branch_task_count_field_str].Int();}
			checkNotEoo(obj[task_branch_max_task_type_field_str]){_max_branch_task_type = obj[task_branch_max_task_type_field_str].Int();}
			checkNotEoo(obj[task_branch_finished_task_list_field_str])
			{
				vector<mongo::BSONElement> ele = obj[task_branch_finished_task_list_field_str].Array();
				_branch_finished_records.clear();
				for (unsigned i = 0; i < ele.size(); ++i)
					_branch_finished_records.push_back(ele[i].Int());
			}
			checkNotEoo(obj[task_branch_running_task_list_field_str])
			{
				_branch_running_records.clear();
				vector<mongo::BSONElement> ele = obj[task_branch_running_task_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
				{
					int task_id = ele[i][task_branch_task_id_field_str].Int();
					int task_value = ele[i][task_branch_task_value_field_str].Int();
					int task_state = ele[i][task_branch_task_state_field_str].Int();
					branchTaskPtr ptr = task_sys.getBranchTaskPtr(task_id);
					if(ptr != branchTaskPtr())
					{
						branchTaskRecord record(task_id, task_state, task_value, ptr);
						_branch_running_records.insert(record);
						_branch_running_types.set(record.getTaskPtr()->getType());
					}	
				}
			}
		}

		return true;
	}

	bool playerTask::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);

		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID 
			<< task_branch_task_count_field_str << _branch_task_count
			<< task_branch_max_task_type_field_str << _max_branch_task_type;

		{
			mongo::BSONArrayBuilder array_task;
			for (mainTaskRecords::const_iterator iter = _mainTaskRecords.begin(); iter != _mainTaskRecords.end(); ++iter)
				array_task.append(iter->toBSON());
			obj << task_main_task_list_field_str << array_task.arr();
		}

		mongo::BSONArrayBuilder finished_array;
		for(vector<int>::iterator iter = _branch_finished_records.begin(); iter != _branch_finished_records.end(); ++iter)
			finished_array.append(*iter);
		obj << task_branch_finished_task_list_field_str << finished_array.arr();

		mongo::BSONArrayBuilder running_array;
		for(branchTaskRecords::iterator iter = _branch_running_records.begin(); iter != _branch_running_records.end(); ++iter)
		{
			mongo::BSONObj o = BSON(task_branch_task_id_field_str << iter->getTaskId()
				<< task_branch_task_state_field_str << iter->getState()
				<< task_branch_task_value_field_str << iter->getValue());
			running_array.append(o);
		}
		obj << task_branch_running_task_list_field_str << running_array.arr();

		return db_mgr.save_mongo(taskDBStr, key, obj.obj());
	}

	bool playerTask::commitMainTask(int task_id)
	{
		mainTaskRecords::const_iterator iter = _mainTaskRecords.find(mainTaskRecord(task_id, _task_finished));
		if (iter == _mainTaskRecords.end() || iter->getState() != _task_finished)
			return false;

		_mainTaskRecords.erase(iter);
		//int next_id = task_sys.getNextMainTaskId(iter->getTaskId());
		//if (next_id != -1)
		//	_mainTaskRecords.erase(iter);
		//else
		//	iter->alterState(_task_end);

		helper_mgr.insertSaveAndUpdate(this);
		return true;
	}

	void playerTask::playerLogin()
	{
		checkBranchTaskData();
		updateMainTaskToPlayer();
	}

	void playerTask::packageMainTask(Json::Value& msg)
	{
		msg[task_main_task_list_field_str] = Json::arrayValue;

		Json::Value& ref = msg[task_main_task_list_field_str];
		for (mainTaskRecords::const_iterator iter = _mainTaskRecords.begin(); iter != _mainTaskRecords.end(); ++iter)
		{
			Json::Value temp;
			iter->getValue(temp);
			ref.append(temp);
		}
	}

	void playerTask::updateMainTaskToPlayer()
	{
		checkMainTaskData();

		Json::Value msg;
		packageMainTask(msg[msgStr][1u]);
		msg[msgStr][0u]= 0;
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::task_main_update_resp, msg); 
	}

	void playerTask::packageBranchTask(Json::Value& msg)
	{
		msg[task_branch_running_task_list_field_str] = Json::arrayValue;
		Json::Value& ref = msg[task_branch_running_task_list_field_str];

		int num = 0;
		for(branchTaskRecords::iterator iter = _branch_running_records.begin(); iter != _branch_running_records.end(); ++iter)
		{
			if(++num > _client_task_num)
				break;
			Json::Value info;
			iter->package(info);
			ref.append(info);
		}
	}

	bool playerTask::checkBranchTask(int task_id)
	{
		int num = 0;
		bool flag = false;
		for(branchTaskRecords::iterator iter = _branch_running_records.begin(); iter != _branch_running_records.end(); ++iter)
		{
			if(++num > _client_task_num)
				break;
			if(iter->getTaskId() == task_id)
			{
				if(iter->getState() == _task_finished)
				{
					flag = true;
					_branch_finished_records.push_back(task_id);
					int type = iter->getTaskPtr()->getType();
					_branch_running_records.erase(iter);
					if(!isBranchTaskTypeRunning(type))
					{
						_branch_running_types.reset(type);
					}
					helper_mgr.insertSaveAndUpdate(this);
				}
				break;
			}
		}
		return flag;
	}

	void playerTask::addBranchTask(branchTaskPtr ptr)
	{
		_branch_running_records.insert(branchTaskRecord(own.getOwnDataPtr(), ptr));
		_branch_running_types.set(ptr->getType());
		helper_mgr.insertSaveAndUpdate(this);
	}

	bool playerTask::isBranchFinished()
	{
		int num = 0;
		for(branchTaskRecords::iterator iter = _branch_running_records.begin(); iter != _branch_running_records.end(); ++iter)
		{
			if(++num > _client_task_num)
				break;
			if(iter->getState() == _task_finished)
				return true;
		}
		return false;
	}

	bool playerTask::updateBranchTaskInfo(int type, int arg /* = 1 */)
	{
		if(!_branch_running_types.test(type))
			return false;

		bool flag = false;
		bool broadcast = false;
		int num = 0;
		for(branchTaskRecords::iterator iter = _branch_running_records.begin(); iter != _branch_running_records.end(); ++iter)
		{
			++num;
			if(iter->getTaskPtr()->getType() == type)
			{
				//updateSpecialValue(type, gets);
				if(iter->getState() == _task_running)
				{
					int temp = iter->getValue();
					iter->setState(iter->updateTaskInfo(own.getOwnDataPtr(), arg));
					if (iter->getState() == _task_finished)
					{
						if (num <= _client_task_num)
							broadcast = true;
						task_sys.openNextBranchTask(own.getOwnDataPtr(), iter->getTaskId());
					}

					if(iter->getValue() != temp)
						flag = true;
				}
			}
		}
		if(flag)
			helper_mgr.insertSaveAndUpdate(this);

		return broadcast;
	}

	bool playerTask::isBranchTaskTypeRunning(int type)
	{
		for(branchTaskRecords::iterator iter = _branch_running_records.begin(); iter != _branch_running_records.end(); ++iter)
		{
			if(iter->getTaskPtr()->getType() == type)
				return true;
		}
		return false;
	}

	bool playerTask::isBranchTaskIdRunning(int task_id)
	{
		for(branchTaskRecords::iterator iter = _branch_running_records.begin(); iter != _branch_running_records.end(); ++iter)
		{
			if(iter->getTaskId() == task_id)
				return true;
		}
		return false;
	}

	bool playerTask::isBranchTaskIdFinished(int task_id)
	{
		for(vector<int>::iterator iter = _branch_finished_records.begin(); iter != _branch_finished_records.end(); ++iter)
		{
			if(*iter == task_id)
				return true;
		}
		return false;
	}

	void playerTask::checkMainTaskData()
	{
		mainTaskRecords::reverse_iterator iter = _mainTaskRecords.rbegin();
		if (iter->getState() == _task_running)
		{
			mainTaskInfo info;
			if (!task_sys.getMainTaskInfo(iter->getTaskId(), info))
				return;
			if (info.isCommit(own.getOwnDataPtr()))
			{
				iter->alterState(_task_finished);
				int task_id = task_sys.getNextMainTaskId(iter->getTaskId());
				if (task_id != -1)
					_mainTaskRecords.insert(mainTaskRecord(task_id, _task_running));
				else
					_mainTaskRecords.insert(mainTaskRecord(-1, iter->getTaskId()));
			}
		}

		if (_mainTaskRecords.rbegin()->getTaskId() == -1)
		{
			int task_id = task_sys.getNextMainTaskId(iter->getState());
			if(task_id != -1)
			{
				_mainTaskRecords.clear();
				_mainTaskRecords.insert(mainTaskRecord(task_id, _task_running));
			}
		}
	}

	void playerTask::resetBTstate()
	{
		for(branchTaskRecords::iterator iter = _branch_running_records.begin(); iter != _branch_running_records.end(); ++iter)
		{
			iter->setState(iter->updateTaskInfo(own.getOwnDataPtr(), 0));
		}
	}

	void playerTask::checkBTconfig()
	{
		if(_branch_task_count != task_sys.getBranchTaskCount() 
			|| _max_branch_task_type != _task_max)
		{
			_branch_task_count = task_sys.getBranchTaskCount();
			_max_branch_task_type = _task_max;

			for(vector<int>::iterator iter = _branch_finished_records.begin(); iter != _branch_finished_records.end(); ++iter)
			{
				branchTaskPtr ptr = task_sys.getBranchTaskPtr(*iter);
				if(ptr != branchTaskPtr())
				{
					const vector<int>& next = ptr->getNextTaskId();
					for(vector<int>::const_iterator it = next.begin(); it != next.end(); ++it)
					{
						if(!isBranchTaskIdRunning(*it) && !isBranchTaskIdFinished(*it))
						{
							branchTaskPtr next_task_ptr = task_sys.getBranchTaskPtr(*it);
							if(next_task_ptr != branchTaskPtr())
							{
								_branch_running_records.insert(branchTaskRecord(own.getOwnDataPtr(), next_task_ptr));
								_branch_running_types.set(next_task_ptr->getType());
							}
						}
					}
				}
			}
		}
	}

	void playerTask::checkBranchTaskData()
	{
		if(!_bInit)
		{
			_bInit = true;
			resetBTstate();
			checkBTconfig();
		}	
	}
}