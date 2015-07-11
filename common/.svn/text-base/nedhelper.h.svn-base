#pragma once

#include "nedmalloc/nedmalloc.h"
#include <vector>
#ifndef WIN32
#include "DebugInfo.h"
#endif
#ifdef _DEVELOPMENT_
#include <boost/unordered_set.hpp>
#include <boost/thread/mutex.hpp>
#endif
#ifdef DEBUG_INFO
#include "time_helper.h"
#endif
using namespace std;
namespace galaxyMemory
{
	const static size_t KB = 1024u;
	const static size_t MB = 1048576u;
	const static size_t GB = 1073741824u;
#ifdef _DEVELOPMENT_
	typedef boost::unordered_set<void*>  NSET;
	typedef struct nedPool_s
	{
		nedPool_s();
		nedalloc::nedpool* pool;
		NSET set;
	}nedPool;
#endif
	class helper
	{
	public:
		static void* NEWMEM(size_t n);
		static void STOP();
#ifdef DEBUG_INFO
		static void PRINTSTATE();
		static void PRINTSTATE(boost::system_time& tmp);
#endif
#ifdef _DEVELOPMENT_
		static void initial(const size_t block = 8, const size_t ntrun = 512 * MB, const int thread = 0);//defalut 1gb 0 thread means it extends on demand
		static void DELETEMEM(void* mem);
#else		
		static bool initial(const size_t block = GB, const int thread = 0);//defalut 1gb 0 thread means it extends on demand
		static void DELETEMEM(void* mem);
#endif
	private:
		static boost::mutex mutex;
#ifdef DEBUG_INFO
		static boost::system_time tick;
#endif
#ifdef _DEVELOPMENT_
		static bool CREATEPOOL(nedalloc::nedpool** pool);
		static void SORTPOOL();
		static vector<nedPool> 	MemPool;
		static int nThread;
		static size_t nTrun;
#else
		static nedalloc::nedpool* MemPool;
		static size_t MallocSize;
		static size_t MallocTimes;
#endif
		
	};
}