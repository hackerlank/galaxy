#pragma once

#define bgTask_sys (*gg::bgTask_system::bgTaskSys)

namespace gg
{
	typedef boost::unordered_map<int, bgTask::infoPtr> bgTask_map;

	struct bgTask_box
	{
		public:
			bgTask_box(const Json::Value& info);

			int _integral;
			reward _reward;
	};

	typedef vector<bgTask_box> bgTask_box_list;

	struct bgTask_detail
	{
		public:
			bgTask_detail() 
				: _key_id(-1){}

			void load(const Json::Value& info);
			mongo::BSONObj toBSON();
				
			int _key_id;
			unsigned _begin_time;
			unsigned _end_time;
			bgTask_map _bgTask_map;
			bgTask_box_list _box_list;

			int _begin_timer_id;
			int _end_timer_id;

			Json::Value _value;
	};

	const static string bgTask_info_list_db_str = "gl.bgTask_info_list";

	class bgTask_system
	{
		public:
			typedef boost::shared_ptr<bgTask_detail> detailPtr;
			typedef vector<detailPtr> detailPtr_list;
			typedef vector<int> task_id_list;

			static bgTask_system* bgTaskSys;

			void initData();
			
			void infoReq(msg_json& m, Json::Value& r);
			void taskRewardReq(msg_json& m, Json::Value& r);
			void integralRewardReq(msg_json& m, Json::Value& r);

			void gmInfoReq(msg_json& m, Json::Value& r);
			void gmModifyReq(msg_json& m, Json::Value& r);

			void update(playerDataPtr d, int type, int arg);
			bgTask::infoPtr getTaskPtr(int id);
			const task_id_list& getTaskIdList(int type);

			void initRecord(playerDataPtr d);
			int getKeyId() const { return _cur_bgTask_detail._key_id; }

			void tickBegin(int key_id);
			void tickEnd(int key_id);

		private:
			void info(playerDataPtr d);
			void loadDB();
			void saveDB();

		private:
			detailPtr_list _detailPtr_list;

			detailPtr _cur_ptr;
			task_id_list[bgTask::_bgTask_max];
	};
}
