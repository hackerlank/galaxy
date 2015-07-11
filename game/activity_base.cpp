#include "activity_base.h"
#include "time_helper.h"
#include "gm_tools.h"

namespace gg
{
	act_time_info::act_time_info()
	{
		start_time = 0;
		end_time = 0;
	}

	void activity_base::package(Json::Value& update_json)
	{
		int startTime = act_time_.start_time;
		int endTime = act_time_.end_time;
		if (gm_tools_mgr.isGmProtocal(State::getState()))
		{
			gm_tools_mgr.actTimePretreat(startTime, -1);
			gm_tools_mgr.actTimePretreat(endTime, -1);
		}

		update_json[act_base_def::str_act_start_time] = startTime;
		update_json[act_base_def::str_act_end_time] = endTime;
	}

	int activity_base::check(int start_time, int end_time)
	{
		if (is_valid() && (start_time !=0 || end_time != 0))
		{
			return 1;
		}

		if (start_time >= end_time && start_time != 0)
		{
			return 2;
		}

		unsigned cur_time = na::time_helper::get_current_time();
		if ((start_time !=0 || end_time != 0) && int(cur_time) > end_time)
		{
			return 3;
		}

		return 0;
	}

	int activity_base::modify(Json::Value& modify_json)
	{
// 		int start_time = modify_json[act_base_def::str_act_start_time].asInt();
// 		int end_time = modify_json[act_base_def::str_act_end_time].asInt();
// 
// 		act_time_.start_time = start_time;
// 		act_time_.end_time = end_time;
// 
// /*		save();*/
 		return 0;
	}

	bool activity_base::is_valid()
	{
		int cur_time = int(na::time_helper::get_current_time());
		if (cur_time >= act_time_.start_time && cur_time <= act_time_.end_time)
		{
			return true;
		}
		return false;
	}

	bool activity_base::is_overdue()
	{
		int cur_time = int(na::time_helper::get_current_time());
		if (cur_time <= act_time_.end_time)
		{
			return false;
		}
		return true;
	}

	bool activity_base::get()
	{
		return true;
	}

	bool activity_base::save()
	{
		return true;
	}

	bool activity_base::superGet(mongo::BSONObj &obj)
	{
		act_time_.start_time = obj[act_base_def::str_act_start_time].Int();
		act_time_.end_time = obj[act_base_def::str_act_end_time].Int();
		return true;
	}

	bool activity_base::superSave(mongo::BSONObjBuilder &obj)
	{
		obj << act_base_def::str_act_start_time << act_time_.start_time;
		obj << act_base_def::str_act_end_time << act_time_.end_time;
		return true;
	}

	void activity_base::set_to_default()
	{
		act_time_.start_time = 0;
		act_time_.end_time = 0;
	}

}
