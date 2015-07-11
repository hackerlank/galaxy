#pragma once
#include <bitset>
#include "block.h"

namespace gg
{
	namespace bgTask
	{
		class record
		{
			public:
				record(const mongo::BSONElement& obj);
				record(bgTask::infoPtr ptr, playerDataPtr d);

				void package(Json::Value& info);
				mongo::BSONObj toBSON();

				int getId() const { return _id; }
				int getValue() const { return _value; }
				int getState() const { return _state; }
				
				bgTask::infoPtr getPtr()
				{
					if(_ptr == NULL)
						_ptr = _getPtr();
					return _ptr;
				}

				void update(playerDataPtr d, int arg)
				{
					if(_ptr == NULL)
						_ptr = _getPtr();
					if(_ptr == NULL)
						return;

					_state = _ptr->update(d, _value, arg);
				}

			private:
				bgTask::infoPtr _getPtr();

				mField(int, _id, "i");
				mField(int, _value, "v");
				mField(int, _state, "s");
				bgTask::infoPtr _ptr;
		};
	}


	const static string bgTaskDBStr = "gl.player_arena";

	class playerBgTask : public Block
	{
		public:
			playerBgTask(playerData& own);
			virtual bool get();
			virtual bool save();
			virtual void autoUpdate();
			
			void update();

			void checkAndUpdate(unsigned now);

			bool update(int type, int arg);
			void push(bgTask::infoPtr ptr);

		private:
			typedef boost::unordered_map<int, bgTask::record> record_map;

			mField(record_map, _record_map, "rcd");
			mField(int, _key_id, "kid");
			mField(unsigned, _next_update_time, "nut");
			
			std::bitset<bgTask::_bgTask_max> _running_types;
	};
}
