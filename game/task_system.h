#pragma once

#include <boost/unordered_map.hpp>
#include "json/json.h"
#include "msg_base.h"
#include "reward.h"
#include "playerManager.h"
#include "task_helper.h"

#define task_sys (*gg::task_system::taskSys)

namespace gg
{
	typedef boost::unordered_map<int, mainTaskInfo> mainTaskMap;
	typedef boost::unordered_map<int, branchTaskPtr> branchTaskMap;

	const static string taskLogDBStr = "log_task";

	class task_system
	{
	public:
		enum{
			_log_main_task = 0,
			_log_branch_task
		};


		task_system() : _mainTaskBeginId(0) {}
		static task_system* const taskSys;
		static const string taskMainDataDirStr;
		static const string taskBranchDataDirStr;

		void taskMainUpdateReq(msg_json& m, Json::Value& r);
		void taskMainCommitReq(msg_json& m, Json::Value& r);
		void taskBranchUpdateReq(msg_json& m, Json::Value& r);
		void taskBranchCommitReq(msg_json& m, Json::Value& r);
		void taskBranchTipsReq(msg_json& m, Json::Value& r);

		int getMainTaskBeginId(){ return _mainTaskBeginId; }
		int getBranchTaskCount(){ return _branchTaskMap.size(); }
		int getNextMainTaskId(int task_id);
		branchTaskPtr getBranchTaskPtr(int task_id);
		bool getMainTaskInfo(int task_id, mainTaskInfo& info);

		void initData();

		void updateBranchTaskInfo(playerDataPtr d, int type, int arg = 1);
		void openNextBranchTask(playerDataPtr d, int current_id);

	private:
		void load_file();
		void updateMain(playerDataPtr d);
		void updateBranch(playerDataPtr d);

		mainTaskMap _mainTaskMap;
		branchTaskMap _branchTaskMap;

		int _mainTaskBeginId;
	};
}