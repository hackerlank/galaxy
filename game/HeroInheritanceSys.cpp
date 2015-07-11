#include "HeroInheritanceSys.h"
#include "gate_game_protocol.h"
#include "mongoDB.h"
#include "helper.h"
#include "response_def.h"
#include "file_system.h"
#include "record_system.h"

using namespace gate_client;
using namespace mongo;
namespace gg{

	enum{
		toInheritMsg = 1,
		byInheritMsgBefore,
		byInheritMsgAfter
	};


    HeroInheritanceSys* const HeroInheritanceSys::playerInheritMgr=new HeroInheritanceSys();

	const static string heroInheritanceLogTableName = "log_heroInheritance";

	HeroInheritanceSys::HeroInheritanceSys(void)
	{
	}


	HeroInheritanceSys::~HeroInheritanceSys(void)
	{
	}

	void HeroInheritanceSys::startInheritance(msg_json& m,Json::Value &r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray
		//UnAcceptRetrun(js_msg[0u].isInt(), -1);
		int toInheritId = js_msg[0u].asInt();
		int byInheritId = js_msg[1u].asInt();
		int isSupremacy = js_msg[2u].asBool();
		//int isContinue = js_msg[3u].asBool();
		int res = this->InheritanceOperate(d,toInheritId,byInheritId,isSupremacy);
		if(res!=0)
			Return(r,res);
		//d->Pilots.update();
		Return(r,0);
	}

	//void HeroInheritanceSys::inheritingThePreview(msg_json& m,Json::Value &r)
	//{
	//	AsyncGetPlayerData(m);
	//	ReadJsonArray
	//	int toInheritId = js_msg[0u].asInt();
	//	int byInheritId = js_msg[1u].asInt();
	//	bool isSupremacy = js_msg[2u].asBool();
	//	//bool isContinue = js_msg[3u].asBool();
	//	int res = this->InheritanceOperate(d,toInheritId,byInheritId,isSupremacy);
	//	r[inheritLevelStr] = m_inherit.inheritanceLevel;
	//	r[inHeritproperty1] = m_inherit.property1;
	//	r[inHeritproperty2] = m_inherit.property2;
	//	r[inHeritproperty3] = m_inherit.property3;
	//	LogW<<"Test Inherit:"<<r.toStyledString()<<LogEnd;
	//	Return(r,res);
	//}
	

	int HeroInheritanceSys::InheritanceOperate(playerDataPtr d,int toInheritId,int byInheritId,bool isSupremacy/*=false*/,bool isContinue /*= false*/)
	{
		if(toInheritId == byInheritId)
			return -1;
		playerPilot toInheritPilot = d->Pilots.getPlayerPilotExtert(toInheritId);
		playerPilot byInheritPilot = d->Pilots.getPlayerPilotExtert(byInheritId);
		if(toInheritPilot==playerPilots::NullPilotExtert||byInheritPilot==playerPilots::NullPilotExtert)
			return -2;
		int exp = d->Pilots.getTotalExp(toInheritId);
		//LogW<<"hero inheritance exp before:"<<exp<<"------------------"<<LogEnd;
		int property1 = (int)(toInheritPilot.addAttribute[0]*0.5);
		int property2 = (int)(toInheritPilot.addAttribute[1]*0.5);
		int property3 = (int)(toInheritPilot.addAttribute[2]*0.5);
		int commonExp = (int)(exp*0.7);
		int supremacyExp = (int)(exp*0.9);
		int r=0;
		if(isSupremacy){
			if((r=d->Pilots.addExpPreview(byInheritPilot.pilotID,supremacyExp))!=-1)
				m_inherit.inheritanceLevel = r;
			m_inherit.property1 = property1;
			m_inherit.property2 = property2;
			m_inherit.property3 = property3;
			m_inherit.exp = supremacyExp;
		}else{
			if((r=d->Pilots.addExpPreview(byInheritPilot.pilotID,commonExp))!=-1)
				m_inherit.inheritanceLevel = r;
			m_inherit.exp = commonExp;
		}
		if(toInheritPilot.isToInherit == 1)
			return 1;
		if(byInheritPilot.isByInherit == 1)
			return 2;
		int silver  =  d->Base.getLevel()*1500+toInheritPilot.pilotLevel*1000;
		if(!isSupremacy&&d->Base.getSilver()<silver)
			return 3;
		if(isSupremacy&&d->Base.getAllGold()<500)
			return 4;
		if(!isSupremacy&&m_inherit.inheritanceLevel<=byInheritPilot.pilotLevel)
			return 5;
		if(isSupremacy&&(property1<=byInheritPilot.addAttribute[0])&&
			(property2<=byInheritPilot.addAttribute[1])&&
			(property3<=byInheritPilot.addAttribute[2])&&
			(m_inherit.inheritanceLevel<=byInheritPilot.pilotLevel))
			return 6;
		else if(isSupremacy){
			int temp = d->Pilots.getTotalExp(byInheritId);
			m_inherit.exp = m_inherit.exp>temp ? m_inherit.exp:temp;
		}
		/*if(!isContinue&&isSupremacy&&(m_inherit.inheritanceLevel<=byInheritPilot.pilotLevel))
			return 7;*/
		toInheritPilot.isToInherit = 1;
		d->Pilots.updatePlayerPilot(toInheritPilot);
		byInheritPilot.pilotLevel = 1;
		byInheritPilot.pilotExp = m_inherit.exp;
		byInheritPilot.isByInherit = 1;
		if(isSupremacy){
			byInheritPilot.addAttribute[0] = m_inherit.property1;
			byInheritPilot.addAttribute[1] = m_inherit.property2;
			byInheritPilot.addAttribute[2] = m_inherit.property3;	
			int attribute_total = m_inherit.property1+m_inherit.property2+m_inherit.property3;;
			//玩家第一次替换属性时，将三项属性之和与原属性的三项之和进行比较，取其大者定为最好属性，以后每次替换新属性时，都将新属性三项之和与最好属性三项之和进行比较，新属性和≥最好属性和时，将新属性替换为最好属性 
			int history_total = 0;
			for (unsigned i = 0; i < 3; ++i){
				history_total += byInheritPilot.historyMaxAttribute[i]; 
			}
			if (attribute_total > history_total){
				for (unsigned i = 0; i < 3; ++i){
					byInheritPilot.historyMaxAttribute[i] = byInheritPilot.addAttribute[i];
				}
			}
		}
		int costBefor = 0,costAfter = 0;
		int tag = 0;
		if(isSupremacy){
			tag = 1;
			costBefor = d->Base.getAllGold();
			d->Base.alterBothGold(-500);
			costAfter = costBefor - 500;
		}
		else{
			tag = 0;
			costBefor = d->Base.getSilver();
			d->Base.alterSilver(-silver);
			costAfter = costBefor - silver;
		}
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string ,int>(tag));
		fields.push_back(boost::lexical_cast<string, int>(toInheritPilot.pilotID));
		fields.push_back(boost::lexical_cast<string, int>(toInheritPilot.pilotLevel));
		fields.push_back(boost::lexical_cast<string, int>(toInheritPilot.addAttribute[0]));
		fields.push_back(boost::lexical_cast<string, int>(toInheritPilot.addAttribute[1]));
		fields.push_back(boost::lexical_cast<string, int>(toInheritPilot.addAttribute[2]));
		StreamLog::Log(heroInheritanceLogTableName, d, boost::lexical_cast<string, int>(costBefor), boost::lexical_cast<string, int>(costAfter),fields, toInheritMsg);
		
		fields.clear();
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.pilotID));
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.pilotLevel));
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.addAttribute[0]));
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.addAttribute[1]));
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.addAttribute[2]));
		StreamLog::Log(heroInheritanceLogTableName, d, "", "",fields, byInheritMsgBefore);
		//LogW<<"hero inheritance exp:"<<m_inherit.exp<<"------------------"<<LogEnd;
		d->Pilots.inheritHero(byInheritPilot,isSupremacy);
//		d->Pilots.addDelayUpdate(toInheritPilot.pilotID);
//		d->Pilots.addDelayUpdate(byInheritPilot.pilotID);
		
		fields.clear();
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.pilotID));
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.pilotLevel));
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.addAttribute[0]));
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.addAttribute[1]));
		fields.push_back(boost::lexical_cast<string, int>(byInheritPilot.addAttribute[2]));
		StreamLog::Log(heroInheritanceLogTableName, d, "", "",fields, byInheritMsgAfter);
		return 0;
	}
}
