#pragma once

#include <boost/function.hpp>
#include <list>
#include <set>
#include <vector>
#include <boost/thread/mutex.hpp>
using namespace std;

namespace gg
{
	template<typename T>
	class trigger
	{
	public:
		typedef std::multiset<T> itemList;
		typedef typename itemList::iterator itemIter;
		typedef boost::function<void(const T&)> Handler;

		trigger(bool* busy) : _busy(busy){}
		void init(Handler h){ _handler = h; }
		void push(const T& e)
		{
			_items.insert(e);
		}
		void pop(const T& e)
		{
			std::pair<itemIter, itemIter> pair_iter = _items.equal_range(e);
			itemIter bIter = pair_iter.first;
			for (; bIter != pair_iter.second; ++bIter)
			{
				if (*bIter == e)
				{
					_items.erase(bIter);
					break;
				}
			}
		}
		void check(unsigned now)
		{
			while (!(*_busy) && !_items.empty() && now >= _items.begin()->getTime())
			{
				T item = *_items.begin();
				_items.erase(_items.begin());
				_handler(item);
			}
		}

	private:
		Handler _handler;
		itemList _items;
		bool* _busy;
	};

	template<typename T, unsigned maxPriorty>
	class priortyQueue
	{
	public:
		typedef std::list<T> priortyList;
		bool empty() const
		{
			for (unsigned i = 0; i < maxPriorty; ++i)
			{
				if (!_priorityList[i].empty())
					return false;
			}
			return true;
		}
		const T& front() const
		{
			for (unsigned i = maxPriorty - 1; i >= 0; --i)
			{
				if (!_priorityList[i].empty())
					return _priorityList[i].front();
			}
			return null_item;
		}
		void push(const T& item)
		{
			_priorityList[item._priorty].push_back(item);
		}
		void pop()
		{
			for (unsigned i = maxPriorty - 1; i >= 0; --i)
			{
				if (!_priorityList[i].empty())
				{
					_priorityList[i].pop_front();
					return;
				}
			}
		}

	private:
		priortyList _priorityList[maxPriorty];
		T null_item;
	};

	enum
	{
		_left_side = 0,
		_right_side,
		_max_side
	};

	inline int divide32(int x, int y)
	{
		return (x - 1) / y + 1;
	}

	typedef long long i64;
	inline i64 divide64(i64 x, i64 y)
	{
		return (x - 1) / y + 1;
	}

	class mGraph
	{
	public:
		const static int max = 999999;
		mGraph(unsigned n)
		{
			_n = n;
			_ptr = new int[n * n];
			for (unsigned i = 0; i < n; ++i)
				for (unsigned j = 0; j < n; ++j)
					_ptr[i * n + j] = max;
		}
		~mGraph()
		{
			delete _ptr;
		}

		void load(int i, int j, int v){ _ptr[i * _n + j] = v; }

		unsigned getPointNum() const { return _n; }
		int getLine(int i, int j) const { return _ptr[i * _n + j]; }

	private:
		int* _ptr;
		unsigned _n;
	};

	typedef vector<int> mPath;
	typedef vector<mPath> mPathList;
	void shortestPath_DIJ(const mGraph& graph, mPathList& path_list, int start_id);

	class noncopyable
	{
	protected:
		noncopyable(){}
		~noncopyable(){}
	private:
		noncopyable(const noncopyable&);
		const noncopyable& operator=(const noncopyable&);
	};

	template<typename T>
	class mSingleton : public noncopyable
	{
	protected:
		mSingleton(){}
		virtual ~mSingleton(){}

	public:
		static T* shared()
		{
			if (_ptr == NULL)
			{
				boost::mutex::scoped_lock lock(_mutex);
				if (_ptr == NULL)
					_ptr = new T();
			}
			return _ptr;
		}
		void destroy()
		{
			if (_ptr)
			{
				boost::mutex::scoped_lock lock(_mutex);
				if (_ptr)
				{
					delete _ptr;
					_ptr = NULL;
				}
			}
		}

	private:
		static T* _ptr;
		static boost::mutex _mutex;
	};

	template<typename T>
	T* mSingleton<T>::_ptr = NULL;

	template<typename T>
	boost::mutex mSingleton<T>::_mutex;

#define mField(type, name, str) \
	type name; \
	inline string strOf##name() { return str; } \

#define mArray(type, name, size, str) \
	type name[size]; \
	inline string strOf##name() { return str; } \

#define getBegin(str) \
	mongo::BSONObj key = BSON(playerIDStr << own.playerID); \
	mongo::BSONObj obj = db_mgr.FindOne(str, key); \
	if (obj.isEmpty()) \
		return true; \

#define getEnd() \
	return true; \

#define saveBegin() \
	mongo::BSONObj key = BSON(playerIDStr << own.playerID); \
	mongo::BSONObjBuilder obj; \
	obj << playerIDStr << own.playerID; \

#define saveEnd(str) \
	return db_mgr.save_mongo(str, key, obj.obj()); \

// end
}