#pragma once

#include <boost/unordered_map.hpp>
//#include <vector>
#include "skill_def.h"
#include "params.hpp"

using namespace std;

#define skill_sys (*gg::skillManager::skiMgr)
namespace gg
{
	class skillManager
	{
	public:
		static skillManager* const skiMgr;
		void initSkillData();
		Params::VarPtr getSkill(const int sID);
	private:
		typedef boost::unordered_map<int, Params::VarPtr> SkillMap;
		SkillMap map_;
	};
}