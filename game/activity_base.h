#pragma once
#include "json/json.h"
#include <string>
#include "mongoDB.h"

using namespace std;
namespace gg
{
#define KEY_ACT_GAME_PARAM 1
#define KEY_ACT_REBATE 2
#define KEY_ACT_POINT 3
#define KEY_ACT_GIFT_DEFINED 4

	namespace act_base_def
	{
		const static string str_act_db_total = "gl.activity_total";
		const static string str_act_start_time = "st";
		const static string str_act_end_time = "et";
		const static string str_act_key = "key";
	}

	struct act_time_info
	{
		act_time_info();
		int start_time;
		int end_time;
	};

	class activity_base
	{
	public:
		activity_base(){};
		~activity_base(){};
		virtual void package(Json::Value& update_json);
		int check(int start_time, int end_time);
		virtual int modify(Json::Value& modify_json);
		virtual void set_to_default();
		virtual bool is_overdue();
		bool superGet(mongo::BSONObj &obj);
		bool superSave(mongo::BSONObjBuilder &obj);
		act_time_info getActTime(){return act_time_;};
	protected:
		virtual bool is_valid();
		virtual bool get();
		virtual bool save();
		act_time_info act_time_;
	};

}