#pragma once

#include "block.h"
#include <vector>
#include "task_helper.h"
#include <bitset>
#include <boost/unordered_map.hpp>

namespace gg
{
	enum{
		_task_running = 0,
		_task_finished,
		_task_end,
	};

	class branchTaskRecord
	{
	public:
		branchTaskRecord(playerDataPtr d, branchTaskPtr ptr)
			: _task_id(ptr->getTaskId()), _task_ptr(ptr)
		{
			_state = ptr->updateTaskInfo(d, _task_value);
		}

		branchTaskRecord(int id, int state, int value, branchTaskPtr ptr)
			: _task_id(id), _state(state), _task_value(value), _task_ptr(ptr)
		{

		}

		bool operator<(const branchTaskRecord& rhs) const
		{
			if(_task_ptr->getPriority() != rhs.getTaskPtr()->getPriority())
				return _task_ptr->getPriority() < rhs.getTaskPtr()->getPriority();
			return _task_id < rhs.getTaskId();
		}

		int getTaskId() const { return _task_id; }
		int getState() const { return _state; }
		void setState(int state) const { _state = state; }
		int getValue() const { return _task_value; }
		branchTaskPtr getTaskPtr() const { return _task_ptr; }

		int updateTaskInfo(playerDataPtr d, int arg = -1) const { return _task_ptr->updateTaskInfo(d, _task_value, arg); }

		void package(Json::Value& info) const
		{
			info.append(_task_id);
			info.append(_task_value);
			info.append(_state);
		}

	private:
		int _task_id;
		mutable int _state;
		mutable int _task_value;
		branchTaskPtr _task_ptr;
	};

	typedef set<branchTaskRecord> branchTaskRecords;

	class mainTaskRecord
	{
		public:
			mainTaskRecord(int task_id, int state)
				: _task_id(task_id), _state(state){}
			mainTaskRecord(const mongo::BSONElement& obj);
			inline void getValue(Json::Value& info) const;
			inline mongo::BSONObj toBSON() const;

			bool operator<(const mainTaskRecord& rhs) const
			{
				return _task_id < rhs.getTaskId();
			}

			int getTaskId() const { return _task_id; }
			int getState() const { return _state; }
			void alterState(int state) const { _state = state; }

		private:
			int _task_id;
			mutable int _state;
	};

	typedef set<mainTaskRecord> mainTaskRecords;

	const static string taskDBStr = "gl.player_task";
	const static string task_main_task_list_field_str = "mtl";
	const static string task_main_task_id_field_str = "mi";
	const static string task_main_task_state_field_str = "ms";
	const static string task_branch_running_task_list_field_str = "bt";
	const static string task_branch_finished_task_list_field_str = "ft";
	const static string task_branch_task_count_field_str = "bc";
	const static string task_branch_max_task_type_field_str = "mt";
	const static string task_branch_special_records_field_str = "sr";

	const static string task_branch_task_type_field_str = "tt";
	const static string task_branch_task_id_field_str = "ti";
	const static string task_branch_task_state_field_str = "ts";
	const static string task_branch_task_value_field_str = "tv";

	class playerTask : public Block
	{
	public:
		enum{
			_client_task_num = 6,
		};

		playerTask(playerData& own);
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate(){}
		
		void playerLogin();

		bool commitMainTask(int task_id);
		void packageMainTask(Json::Value& msg);
		void packageBranchTask(Json::Value& msg);
		void updateMainTaskToPlayer();

		int getCurrentMainTaskId();
		void setCurrentMainTaskId(int id);

		bool checkBranchTask(int task_id);
		void addBranchTask(branchTaskPtr ptr);

		bool updateBranchTaskInfo(int type, int arg = 1);

		bool isBranchFinished();
		bool isBranchTaskIdRunning(int task_id);
		bool isBranchTaskTypeRunning(int type);
		bool isBranchTaskIdFinished(int task_id);

		void checkBranchTaskData();
		void checkMainTaskData();
		void resetBTstate();
		void checkBTconfig();

		const mainTaskRecords& getMainTaskRecords() const { return _mainTaskRecords; }

	private:
		mainTaskRecords _mainTaskRecords;

		branchTaskRecords _branch_running_records;
		vector<int> _branch_finished_records;

		int _branch_task_count;
		int _max_branch_task_type;

		bool _bInit;

		//boost::unordered_map<int, int> _special_records;

		std::bitset<_task_max> _branch_running_types;
	};
}