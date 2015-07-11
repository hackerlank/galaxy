#pragma once

#include <string.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <utility>

using namespace std;

namespace gg
{
	class battleMember;
	typedef boost::shared_ptr<battleMember> battleMemPtr;
	typedef vector<battleMemPtr> armyVec;

	class battleSide;
	typedef boost::shared_ptr<battleSide> battleSidePtr;

	struct Battlefield
	{
		Battlefield(){
			atkSide = battleSidePtr();
			defSide = battleSidePtr();
			ParamJson = Json::nullValue;
		}
		battleSidePtr atkSide;
		battleSidePtr defSide;
		Json::Value ParamJson;
	};

	struct BattlefieldMuilt
	{
		BattlefieldMuilt(){
			atkSide.clear();
			defSide.clear();
			ParamJson = Json::nullValue;
		}
		vector<battleSidePtr> atkSide;
		vector<battleSidePtr> defSide;
		Json::Value ParamJson;
	};
}