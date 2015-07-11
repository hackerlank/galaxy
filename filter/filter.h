#pragma once

#include <boost/unordered_map.hpp>
#include <iostream>
#include <boost/unordered_set.hpp>
#include <boost/shared_ptr.hpp>
#include "nedhelper.hpp"
#include <string.h>
#include <vector>
using namespace std;

#define FILTER (*ft::Filter::FilterPoint)

namespace ft
{
	typedef boost::unordered_set<string> sSMap;
	class sSMapManager;
	typedef boost::shared_ptr<sSMapManager> sSMMPtr;

	typedef boost::unordered_map< char, sSMMPtr > cbMap;
	class cbMapManager;
	typedef boost::shared_ptr<cbMapManager> cbMMPtr;

	typedef boost::unordered_map< size_t, cbMMPtr > McbMap;
	class McbMapManager;
	typedef boost::shared_ptr<McbMapManager> McbMMPtr;

	class Filter
	{
		friend class McbMapManager;
	public:
		static Filter* const FilterPoint;
		void initData();
		bool filtName(string& str, size_t& filt_sz);
		bool filtChat(string& str, size_t& filt_sz);
	private:
		Filter();
		~Filter();
		class Performer
		{
			friend class McbMapManager;
			friend void Filter::initData();
		public:
			Performer();
			~Performer();
			bool filt(string& str, size_t& filt_sz);
		private:
			bool filt(unsigned begin, char* c, size_t sz, size_t& filt_sz);
			void initData(string dir);
			McbMMPtr Manager;
			vector<size_t> Vector;
		};
		Performer Per_1, Per_2;
	};

	class McbMapManager
	{
		friend void Filter::Performer::initData(string);
		friend class cbMapManager;
	public:
		static McbMMPtr Create()
		{
			void* point = ::GLMEMORY::GLNew(sizeof(McbMapManager));
			return McbMMPtr(new(point) McbMapManager(), Destory);
		}
		inline bool filt(char* c, size_t sz);
	private:
		static void Destory(McbMapManager* point)
		{
			point->~McbMapManager();
			::GLMEMORY::GLDelete(point);
		}
		McbMapManager();
		~McbMapManager();
		inline cbMMPtr getCarry(size_t sz);
		inline void insert(string& str);
		inline McbMap::iterator END();
		McbMap ownMap;
	};

	class cbMapManager
	{
		friend class sSMapManager;
		friend void McbMapManager::insert(string&);
	public:
		static cbMMPtr Create()
		{
			void* point = ::GLMEMORY::GLNew(sizeof(cbMapManager));
			return cbMMPtr(new(point) cbMapManager(), Destory);
		}
		inline bool filt(char* c, size_t sz);
	private:
		static void Destory(cbMapManager* point)
		{
			point->~cbMapManager();
			::GLMEMORY::GLDelete(point);
		}
		~cbMapManager();
		cbMapManager();
		inline sSMMPtr getCarry(char c);
		inline void insert(string& str);
		inline cbMap::iterator END();
		cbMap ownMap;
	};

	class sSMapManager
	{
		friend void cbMapManager::insert(string&);
	public:
		static sSMMPtr Create()
		{
			void* point = ::GLMEMORY::GLNew(sizeof(sSMapManager));
			return sSMMPtr(new(point) sSMapManager(), Destory);
		}
		inline bool filt(char* c, size_t sz);
	private:
		static void Destory(sSMapManager* point)
		{
			point->~sSMapManager();
			::GLMEMORY::GLDelete(point);
		}
		~sSMapManager();
		sSMapManager();
		inline bool match(char* c, size_t sz);
		inline void insert(string& str);
		inline sSMap::iterator END();
		sSMap ownMap;
	};

	#include "filter.inl"
}