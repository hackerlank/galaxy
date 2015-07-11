#pragma once
//取消同步事件机制,调用不够直观


#include "playerManager.h"
#include <set>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "core_helper.h"
#include "json/json.h"

#define helper_mgr (*gg::helper::helperMgr)

namespace gg
{
	class helper
	{
	public:
		static helper* const helperMgr;
		helper();

		void runSaveAndUpdate();
		void insertUpdate(Block* pointer);
		void insertSave(Block* pointer);
		void insertSaveAndUpdate(Block* pointer);

	protected:
		typedef boost::unordered_set< Block* > SaveSet;
		//typedef std::set< Block* > SaveSet;
		SaveSet save, update;
	private:
	};

	class sendMember
	{
	public:
		sendMember(const int playerID, const int netID);
		sendMember& operator=(const sendMember& source)
		{
			memmove(this, &source, sizeof(sendMember));
			return *this;
		}
		~sendMember(){}
		const int _pID;
		const int _nID;
	};

	typedef std::vector<sendMember> sendList;

	void MsgSignOnline(sendList& vec, Json::Value& msgJson, const short protocol, const int threadID = LogicThreadID);
	void MsgSignOnline(playerManager::playerDataVec& vec, Json::Value& msgJson, const short protocol, const int threadID = LogicThreadID);

	template<typename TYPE, typename MAPTYPE>
	std::vector<TYPE> MapToVector(MAPTYPE& tMap)
	{
		typedef std::vector<TYPE> VECTYPE;
		typedef MAPTYPE _MAPTYPE;
		typedef typename _MAPTYPE::iterator MAPIT;
		VECTYPE  vec;
		for (MAPIT it = tMap.begin(); it != tMap.end();++it)
		{
			vec.push_back(it->second);
		}
		return vec;
	}

	extern void zip_write(string path, Json::Value data);
	extern void zip_write(string path, string str);

	extern Json::Value zip_read_json(string path);
}