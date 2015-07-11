#include "filter.h"
#include "file_system.h"
#include "gate_game_protocol.h"

namespace ft
{
	sSMapManager::sSMapManager()
	{
		ownMap.clear();
	}

	sSMapManager::~sSMapManager()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	cbMapManager::cbMapManager()
	{
		ownMap.clear();
	}

	cbMapManager::~cbMapManager()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	McbMapManager::McbMapManager()
	{
		ownMap.clear();
	}

	McbMapManager::~McbMapManager()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	Filter* const Filter::FilterPoint = new Filter();

	Filter::Filter()
	{

	}

	Filter::~Filter()
	{

	}
	typedef boost::unordered_set<size_t> TMPSET;
	void Filter::initData()
	{
		Per_1.initData("./instance/filter/chat_words.json");
		Per_2.initData("./instance/filter/name_words.json");
	}

	bool Filter::filtName(string& str, size_t& filt_sz)
	{
		return Per_2.filt(str, filt_sz);
	}

	bool Filter::filtChat(string& str, size_t& filt_sz)
	{
		return Per_1.filt(str, filt_sz);
	}

	//////////////////////////////////////////////////////////////////////////

	Filter::Performer::Performer()
	{

	}

	Filter::Performer::~Performer()
	{

	}

	void Filter::Performer::initData(string dir)
	{
		Vector.clear();
		Manager = McbMapManager::Create();

		TMPSET tmps;
		Json::Value wordJson = na::file_system::load_jsonfile_val(dir);
		for (unsigned i = 0; i < wordJson.size(); ++i)
		{
			string str = wordJson[i].asString();
			if (str.length() < 1)continue;
			Manager->insert(str);
			tmps.insert(str.length());
		}
		for (TMPSET::iterator it = tmps.begin(); it != tmps.end(); ++it )
		{
			Vector.push_back(*it);
		}
		std::sort(Vector.begin(), Vector.end(), greater<size_t>());
	}

	bool Filter::Performer::filt(string& str, size_t& filt_sz)
	{
		char* c = const_cast<char*>(str.c_str());
		return filt(0, c, str.length(), filt_sz);
	}

	bool Filter::Performer::filt(unsigned begin, char* c, size_t sz, size_t& filt_sz)
	{
		size_t _szt = Vector.size();
		unsigned next_begin = 0;
		for (unsigned idx = begin; idx < _szt; ++idx)
		{
			size_t len = Vector[idx];
			if(sz < len)continue;
			if(next_begin ==  0)
			{
				next_begin = idx;
			}
			if(Manager->filt(c, len))
			{
				c += len;
				sz -= len;
				filt_sz += len;
				filt(next_begin, c, sz, filt_sz);
				return true;
			}
		}
		if(sz > 0)
		{
			--sz;
			++c;
			return filt(next_begin, c, sz, filt_sz);
		}
		return false;
	}

}