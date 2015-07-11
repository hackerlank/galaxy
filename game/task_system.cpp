#include "task_system.h"
#include "response_def.h"
#include "record_system.h"
#include "action_def.h"

namespace gg
{
	const string task_system::taskMainDataDirStr = "./instance/task/mainquest.json";
	const string task_system::taskBranchDataDirStr = "./instance/task/branchquest.json";
	task_system* const task_system::taskSys = new task_system;

	void task_system::initData()
	{
		db_mgr.ensure_index(taskDBStr, BSON(playerIDStr << 1));

		load_file();
	}

	void task_system::load_file()
	{
		Json::Value item = na::file_system::load_jsonfile_val(taskMainDataDirStr);

		for(unsigned j = 0; j < item.size(); ++j)
		{
			mainTaskInfo info;
			info.setFromValue(item[j]);
			_mainTaskMap.insert(make_pair(info.getTaskId(), info));
		}

		for(mainTaskMap::iterator iter = _mainTaskMap.begin(); iter != _mainTaskMap.end(); ++iter)
		{
			if(iter->second.getPreId() != 0)
			{
				mainTaskMap::iterator it = _mainTaskMap.find(iter->second.getPreId());
				it->second.setNextId(iter->second.getTaskId());     // if coredump, the file have error;
			}
			else
			{
				_mainTaskBeginId = iter->second.getTaskId();
			}
		}

		branchTaskPtr ptr(new branchTaskInfo(0));
		_branchTaskMap.insert(make_pair(ptr->getTaskId(), ptr));

		item = na::file_system::load_jsonfile_val(taskBranchDataDirStr);
		for(unsigned j = 0; j < item.size(); ++j)
		{
			branchTaskPtr ptr(new branchTaskInfo);
			ptr->setFromValue(item[j]);
			_branchTaskMap.insert(make_pair(ptr->getTaskId(), ptr));
		}

		for(branchTaskMap::iterator iter = _branchTaskMap.begin(); iter != _branchTaskMap.end(); ++iter)
		{
			if(iter->second->getPreId() != -1)
			{
				branchTaskMap::iterator it = _branchTaskMap.find(iter->second->getPreId());
				//cout << iter->second->getPreId() << endl;
				it->second->addNextId(iter->second->getTaskId());     // if coredump, the file have error;
			}
		}

		/*for(branchTaskMap::iterator iter = _branchTaskMap.begin(); iter != _branchTaskMap.end(); ++iter)
		{
			cout << "id: " << iter->first << "--> pre: " << iter->second->getPreId() << "--> next: ";
			const vector<int>& next_vec = iter->second->getNextTaskId();
			for(vector<int>::const_iterator iter = next_vec.begin(); iter != next_vec.end(); ++iter)
				cout << *iter << ", ";
			cout << endl;
		}*/

	}

	void task_system::openNextBranchTask(playerDataPtr d, int current_id)
	{
		branchTaskMap::iterator iter = _branchTaskMap.find(current_id);
		if (iter == _branchTaskMap.end())
			return;

		const vector<int>& next_id_vec = iter->second->getNextTaskId();

		for (vector<int>::const_iterator it = next_id_vec.begin(); it != next_id_vec.end(); ++it)
		{
			branchTaskMap::iterator next_it = _branchTaskMap.find(*it);
			if (next_it != _branchTaskMap.end())
			{
				d->Task.addBranchTask(next_it->second);
			}
		}
	}

	void task_system::taskMainUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		updateMain(d);
	}

	void task_system::updateMain(playerDataPtr d)
	{
		d->Task.updateMainTaskToPlayer();
	}

	void task_system::taskMainCommitReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		int task_id = js_msg[0u].asInt();

		if (!d->Task.commitMainTask(task_id))
			Return(r, 1);

		mainTaskMap::iterator iter = _mainTaskMap.find(task_id);
		if(iter == _mainTaskMap.end())
			Return(r, 2);

		vector<int> before;
		const vector<rewardItem>& rewards = iter->second.getReward().getItems();

		for(unsigned i = 0; i < rewards.size(); ++i)
			before.push_back(rewardItem::getItemNum(d, rewards[i].getType(), rewards[i].getId()));
		
		iter->second.getReward(d);

		for(unsigned i = 0; i < rewards.size(); ++i)
		{
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(task_id));
			fields.push_back(boost::lexical_cast<string, int>(getNextMainTaskId(task_id)));
			int type = rewards[i].getType();
			fields.push_back(boost::lexical_cast<string, int>(type));
			if(type == action_add_item)
			{
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getId()));
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getNum()));
			}
			else if(type == action_active_pilot)
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getId()));
			else
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getNum()));

			StreamLog::Log(taskLogDBStr, d, boost::lexical_cast<string, int>(before[i]), boost::lexical_cast<string, int>(rewardItem::getItemNum(d, rewards[i].getType(), rewards[i].getId()))
				, fields, _log_main_task);
		}

		updateMain(d);

		Return(r, 0);
	}

	int task_system::getNextMainTaskId(int task_id)
	{
		mainTaskMap::iterator iter = _mainTaskMap.find(task_id);
		return iter->second.getNextId();
	}

	void task_system::taskBranchUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		updateBranch(d);
	}

	void task_system::updateBranch(playerDataPtr d)
	{
		Json::Value msg;
		d->Task.packageBranchTask(msg[msgStr][1u]);
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::task_branch_update_resp, msg); 
	}

	void task_system::taskBranchCommitReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);

		int task_id = js_msg[0u].asInt();
		branchTaskMap::iterator iter = _branchTaskMap.find(task_id);
		if(iter == _branchTaskMap.end())
			Return(r, 1);

		if(!d->Task.checkBranchTask(task_id))
			Return(r, 2);

		vector<int> before;
		const vector<rewardItem>& rewards = iter->second->getReward().getItems();

		for(unsigned i = 0; i < rewards.size(); ++i)
			before.push_back(rewardItem::getItemNum(d, rewards[i].getType(), rewards[i].getId()));

		iter->second->getReward(d);
		const vector<int>& next_id_vec = iter->second->getNextTaskId();
		string next_id_str;
		bool is_first = true;
		for(vector<int>::const_iterator it = next_id_vec.begin(); it != next_id_vec.end(); ++it)
		{
			branchTaskMap::iterator next_it = _branchTaskMap.find(*it);
			if(next_it != _branchTaskMap.end())
			{
				if(!is_first)
					next_id_str += ",";
				else
					is_first = false;
				next_id_str += boost::lexical_cast<string, int>(*it);

				//d->Task.addBranchTask(next_it->second);
			}
		}

		for(unsigned i = 0; i < rewards.size(); ++i)
		{
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(task_id));
			fields.push_back(next_id_str);
			int type = rewards[i].getType();
			fields.push_back(boost::lexical_cast<string, int>(type));
			if(type == action_add_item)
			{
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getId()));
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getNum()));
			}
			else if(type == action_active_pilot)
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getId()));
			else
				fields.push_back(boost::lexical_cast<string, int>(rewards[i].getNum()));

			StreamLog::Log(taskLogDBStr, d, boost::lexical_cast<string, int>(before[i]), boost::lexical_cast<string, int>(rewardItem::getItemNum(d, rewards[i].getType(), rewards[i].getId()))
				, fields, _log_branch_task);
		}

		//branchTaskInfo info = *(iter->second);

		updateBranch(d);
		Return(r, 0);
	}

	void task_system::taskBranchTipsReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		d->Task.checkBranchTaskData();

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = d->Task.isBranchFinished();
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::task_branch_tips_resp, msg);
	}

	void task_system::updateBranchTaskInfo(playerDataPtr d, int type, int arg)
	{
		if(d->Task.updateBranchTaskInfo(type, arg))
		{
			Json::Value msg;
			msg[msgStr][0u] = 0;
			msg[msgStr][1u] = true;
			player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::task_branch_tips_resp, msg); 
		}
	}

	branchTaskPtr task_system::getBranchTaskPtr(int task_id)
	{
		branchTaskMap::iterator iter = _branchTaskMap.find(task_id);
		if(iter == _branchTaskMap.end())
			return branchTaskPtr();
		return iter->second;
	}

	bool task_system::getMainTaskInfo(int task_id, mainTaskInfo& info)
	{
		mainTaskMap::iterator iter = _mainTaskMap.find(task_id);
		if (iter == _mainTaskMap.end())
			return false;
		info = iter->second;
		return true;
	}
}