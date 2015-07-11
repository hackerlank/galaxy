#include "away_skill.h"
#include "file_system.h"

namespace gg
{
	const static string skillDataDirStr = "./instance/skills/";

	Params::VarPtr skillManager::getSkill(const int sID)
	{
		SkillMap::iterator it = map_.find(sID);
		if(it != map_.end())return it->second;
		return Params::VarPtr();
	}

	skillManager* const skillManager::skiMgr = new skillManager();

	void skillManager::initSkillData()
	{
		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(skillDataDirStr, jv);

		for (unsigned i = 0; i < jv.size(); ++i)
		{
			Json::Value& skill = jv[i];
			if(skill[skillIDStr].isNull() && !skill[skillIDStr].isInt())	continue;
			int skillID = skill[skillIDStr].asInt();
			map_.insert(SkillMap::value_type(skillID, Params::ObjectValue::Create(skill)));
		}
	}
}