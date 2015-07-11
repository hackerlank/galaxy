#pragma once

#include <boost/unordered_map.hpp>
#include <vector>
#include "action_instance.hpp"
#include "json/json.h"

using namespace std;

namespace gg
{
	typedef boost::unordered_map<int , actionPtr> actionMap;
	typedef vector<actionPtr> actionVector;

	namespace errorActionJson
	{
		const static string breakIDStr = "bid";
		const static string breakCodeStr = "bcd";
	}

	struct actionResult
	{
		actionResult(){
			memset(this, 0x0, sizeof(actionResult));
		}
		bool OK(){
			return (breakID == 0 && resultCode == 0);
		}
		int breakID;
		int resultCode;
	};
		
	class actionFormat
	{
	private:
		typedef boost::unordered_map< int, delegateCreate > actionFormatMap;
		static actionFormatMap FormatMap;
	public:
		typedef vector<actionPtr> actionVec;
		static void initialActionMap()
		{
			FormatMap[action_add_silver] = &(actionAddSilver::Create);
			FormatMap[action_add_gold_ticket] = &(actionAddGoldTicket::Create);
			FormatMap[action_add_gold] = &(actionAddGold::Create);
			FormatMap[action_add_keji] = &(actionAddKeji::Create);
			FormatMap[action_add_weiwang] = &(actionAddWeiWang::Create);
			FormatMap[action_add_role_exp] = &(actionAddRoleExp::Create);
			FormatMap[action_add_item] = &(actionAddItem::Create);
			FormatMap[action_add_pilot_exp] = &(actionAddPilotExp::Create);
			FormatMap[action_add_mutil_pilot_exp] = &(actionAddPilotExp::Create);
			FormatMap[action_deal_war] = &(actionDealWar::Create);
			FormatMap[action_add_work_times] = &(actionAddWork::Create);
			FormatMap[action_rate_add_item] = &(actionRateAddItem::Create);
//			FormatMap[action_buff_begin] = &(actionAddBuff::Create);
			FormatMap[action_active_pilot] = &(actionActivePilot::Create);
			FormatMap[action_add_pilot] = &(actionAddPilot::Create);
			FormatMap[action_add_keji_yaosai] = &(actionAddKejiYaoSai::Create);
			FormatMap[action_add_junling] = &(actionAddJunling::Create);
			FormatMap[action_set_vip] = &(actionSetVip::Create);
			FormatMap[action_add_silver_vip_resource] = &(actionAddSilverRes::Create);
			FormatMap[action_add_gold_vip_resource] = &(actionAddGoldRes::Create);
			FormatMap[action_add_keji_vip_resource] = &(actionAddKeJiRes::Create);
			FormatMap[action_add_weiwang_vip_resource] = &(actionAddWWRes::Create);
			FormatMap[action_add_secretary] = &(actionAddSecretary::Create);
			FormatMap[action_add_paper] = &(actionAddPaper::Create);
		}

		static vector<actionResult> actionDoJump(playerDataPtr player, Json::Value& actionData)
		{
			Json::Value Param;
			return actionDoJump(player, actionData, Param);
		}

		static actionResult actionDo(playerDataPtr player, Json::Value& actionData)
		{
			Json::Value Param;
			return actionDo(player, actionData, Param);
		}

		static vector<actionResult> actionDoJump(playerDataPtr player, Json::Value& actionData, Json::Value& Param)
		{
			actionVec vec = formatActionVec(actionData);
			return actionDoJump(player, vec, Param);
		}

		static actionResult actionDo(playerDataPtr player, Json::Value& actionData, Json::Value& Param)
		{
			actionVec vec = formatActionVec(actionData);
			return actionDo(player, vec, Param);
		}

		static vector<actionResult> actionDoJump(playerDataPtr player, actionVec vec, Json::Value& Param)
		{
			LastDoJson = Json::arrayValue;
			vector<actionResult> vecRes;
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				actionPtr action = vec[i];
				actionResult res;
				res.resultCode = action->Check(player, Param);
				res.breakID = action->getID();
				vecRes.push_back(res);
				if(res.resultCode != 0)
				{
					action->clearStaticData();
					continue;
				}
				Json::Value doJson = action->Do(player, Param);
				action->clearStaticData();
				if(!doJson.isNull())LastDoJson.append(doJson);
			}
			return vecRes;
		}

		static actionResult actionDo(playerDataPtr player, actionVec vec)
		{
			Json::Value Param;
			return actionDo(player, vec, Param);
		}

		static actionResult actionDo(playerDataPtr player, actionVec vec, Json::Value& Param)
		{
			LastDoJson = Json::arrayValue;
			actionResult res;
			int breakID = 0;
			do{
				for (unsigned i = 0; i < vec.size(); ++i)
				{
					actionPtr action = vec[i];
					int resCode = action->Check(player, Param);
					if(resCode != 0){
						breakID = action->getID();
						res.breakID = breakID;
						res.resultCode = resCode;
						break;
					}
				}
				if(breakID != 0)break;
				for (unsigned i = 0; i < vec.size(); ++i)
				{
					actionPtr action = vec[i];
					Json::Value doJson = action->Do(player, Param);
					if(doJson.isNull())continue;
					LastDoJson.append(doJson);
				}
			}while(false);
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				actionPtr action = vec[i];
				action->clearStaticData();
			}
			return res;
		}

		static actionVec formatActionVec(Json::Value& dataJson)
		{
			actionVec vec;
			if(!dataJson.isArray())return vec;
			for (unsigned i = 0; i < dataJson.size(); ++i)
			{
				Json::Value& var = dataJson[i];
				if(!var.isObject())continue;
				int actionID = var[ACTION::strActionID].asInt();
				if(actionID < 1 || actionID >= action_end)continue;
				actionPtr action = formationAction(var);
				if(actionPtr() == action)continue;
				vec.push_back(action);
			}
			return vec;
		}

		static Json::Value getLastDoJson()
		{
			return LastDoJson;
		}

	protected:
		static actionPtr formationAction(Json::Value& dataJson)
		{
			actionFormat::actionFormatMap::iterator it = actionFormat::FormatMap.find(dataJson[ACTION::strActionID].asInt());
			if(it != FormatMap.end()) return it->second(dataJson);
			return actionPtr();
		}

		static Json::Value LastDoJson;
	};


}