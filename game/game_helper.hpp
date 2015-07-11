#pragma once

#include "nedhelper.hpp"
#include "Glog.h"
#include "block.h"

namespace gg
{
	inline static void* GGNew(size_t n)
	{
		void* point = ::GLMEMORY::GLNew(n);
		return point;
	}

	inline static void GGDelete(void* pointer)
	{
		::GLMEMORY::GLDelete(pointer);
	}

	template<typename T>
	class destroyer
	{
		public:
			static void run(T* ptr)
			{
				ptr->~T();
				GGDelete(ptr);
			}
	};

	template<typename T>
	class creator
	{
		public:
			typedef boost::shared_ptr<T> ptr;
			static ptr run()
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(), destroyer<T>::run);
			}
			template<typename arg1>
			static ptr run(arg1 a1)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1), destroyer<T>::run);
			}
			template<typename arg1, typename arg2>
			static ptr run(arg1 a1, arg2 a2)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1, a2), destroyer<T>::run);
			}
			template<typename arg1, typename arg2, typename arg3>
			static ptr run(arg1 a1, arg2 a2, arg3 a3)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1, a2, a3), destroyer<T>::run);
			}
			template<typename arg1, typename arg2, typename arg3, typename arg4>
			static ptr run(arg1 a1, arg2 a2, arg3 a3, arg4 a4)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1, a2, a3, a4), destroyer<T>::run);
			}
			template<typename arg1, typename arg2, typename arg3, typename arg4, typename arg5>
			static ptr run(arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1, a2, a3, a4, a5), destroyer<T>::run);
			}
			template<typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6>
			static ptr run(arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1, a2, a3, a4, a5, a6), destroyer<T>::run);
			}
			template<typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7>
			static ptr run(arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1, a2, a3, a4, a5, a6, a7), destroyer<T>::run);
			}
			template<typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8>
			static ptr run(arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7, arg8 a8)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1, a2, a3, a4, a5, a6, a7, a8), destroyer<T>::run);
			}
			template<typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8, typename arg9>
			static ptr run(arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7, arg8 a8, arg9 a9)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1, a2, a3, a4, a5, a6, a7, a8, a9), destroyer<T>::run);
			}
			template<typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8, typename arg9, typename arg10>
			static ptr run(arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7, arg8 a8, arg9 a9, arg10 a10)
			{
				void* m = GGNew(sizeof(T));
				return ptr(new(m)T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10), destroyer<T>::run);
			}
	};
}