#pragma once
#include <boost/unordered_map.hpp>
#include <vector>
#include "block.h"
#include "json/json.h"

#define  inherit_sys (*gg::HeroInheritanceSys::playerInheritMgr)
namespace gg{
	const static string inheritLevelStr = "iLevel";
	const static string inHeritproperty1 = "iPro1";
	const static string inHeritproperty2 = "iPro2";
	const static string inHeritproperty3 = "iPro3";
	struct InheritanceContent{
		int inheritanceLevel;
		int exp;
		int property1;
		int property2;
		int property3;
		InheritanceContent(){
			inheritanceLevel=0;
			exp = 0;
			property1=0;
			property2=0;
			property3=0;
		}
	};

	class HeroInheritanceSys
	{
	public:
		HeroInheritanceSys(void);
		~HeroInheritanceSys(void);
		static HeroInheritanceSys* const playerInheritMgr;
		void startInheritance(msg_json& m,Json::Value &r);
		//void inheritingThePreview(msg_json& m,Json::Value &r);
		int InheritanceOperate(playerDataPtr d,int toInheritId,int byInheritId,bool isSupremacy=false,bool isContinue = false);
	private:
		InheritanceContent m_inherit;
		//int m_ToInheritId;
		//int m_ByInheritId;
		//int M_IsSupremacy;
		//int m_IsContinue;
	};

}
