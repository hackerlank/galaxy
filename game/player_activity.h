#pragma once
#include "block.h"
#include "starwar_base.h"
#include <bitset>

namespace gg
{
	const static string activity_db_str = "gl.player_event";

	namespace activity
	{
		class mAcitivityInfo;

		class mRecord
		{
			public:
				//typedef boost::shared_ptr<mAcitivityInfo> activityPtr;

				mRecord(int id, int num) : _id(id), _num(num){}
				mRecord(const mongo::BSONElement& obj)
				{
					checkNotEoo(obj[strOf_id()])
						_id = obj[strOf_id()].Int();
					checkNotEoo(obj[strOf_num()])
						_num = obj[strOf_num()].Int();
				}

				int getId() const { return _id; }
				int getNum() const { return _num; }
				void addNum(int n = 1){ _num = _num + n; }

				void toBson(mongo::BSONObjBuilder& obj)
				{
					obj << strOf_id() << _id << strOf_num() << _num;
				}
				
				void package(Json::Value& info)
				{
					info.append(_id);
					info.append(_num);
				}

				//activityPtr getInfoPtr();

			private:
				mField(int, _id, "ei");
				mField(int, _num, "en");
				//activityPtr _info_ptr;
		};
	}

	typedef map<int, activity::mRecord> activityRecordMap;

	class playerActivity : public Block
	{
		public:
			playerActivity(playerData& own);
			virtual bool get();
			virtual bool save();
			virtual void autoUpdate(){ info(); }

			void info();

			void checkAndUpdate(unsigned now = na::time_helper::get_current_time());
			void updateDailyActivity(int id);
			void updateTimeLimitedActivity(int id);
			int getReward(int index);

		private:
			void package(Json::Value& info);
			void resetActivities();

			mField(unsigned, _next_update_time, "lrt");
			mField(activityRecordMap, _daily_records, "ec");
			mField(activityRecordMap, _time_limited_records, "eca");
			mField(unsigned, _points, "ta");
			mArray(bool, _rwRecord, 5, "rcd");

			bitset<64> _daily_states;
			bitset<64> _time_limited_states;
	};

}