#pragma once

#include "boost/thread/recursive_mutex.hpp"
#include "mongoDB.h"
#include "game_helper.hpp"

namespace gg
{
#define RegisterSysClassNoLock(Name) \
	typedef boost::shared_ptr<Name> Name##_ptr; \
	class _## Name \
	{ \
	public: \
	static Name##_ptr create() \
	{ \
	void* m = GGNew(sizeof(Name)); \
	return Name##_ptr(new(m) Name(), destory); \
		}\
		static void destory(Name* p)\
	{\
	GGDelete(p);\
		}\
	static Name##_ptr playerDataPtr()\
	{\
		static Name##_ptr ptr = create();\
		return ptr;\
	}\
};
	class mgrBase{
	public:
		boost::shared_mutex base_mutex;
		typedef boost::shared_lock<boost::shared_mutex> RL;
		typedef boost::unique_lock<boost::shared_mutex> WL;
	};
	
}