#include "action_instance.hpp"
#include "world_system.h"
#include "commom.h"
#include "chat_system.h"
#include "item_system.h"
#include "war_story.h"
#include "helper.h"

namespace gg
{
	int actionAddWWRes::Check(playerDataPtr d, Json::Value& Param)
	{
		return 0;
	}

	Json::Value actionAddWWRes::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson;
		int tmpValue = ((d->Base.getLevel() / 20) + 5) * 20 * value[ACTION::strValue].asInt();
		d->Base.alterWeiWang(tmpValue);
		doJson.append(getID());
		doJson.append(tmpValue);
		return doJson;
	}

	int actionAddKeJiRes::Check(playerDataPtr d, Json::Value& Param)
	{
		return 0;
	}

	Json::Value actionAddKeJiRes::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson;
		int tmpValue = ((d->Base.getLevel() / 20) + 5) * 20 * value[ACTION::strValue].asInt();
		d->Base.alterKeJi(tmpValue);
		doJson.append(getID());
		doJson.append(tmpValue);
		return doJson;
	}

	int actionAddSilverRes::Check(playerDataPtr d, Json::Value& Param)
	{
		return 0;
	}

	Json::Value actionAddSilverRes::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson;
		int tmpValue = ((d->Base.getLevel() / 20) + 5) * 20 * value[ACTION::strValue].asInt();
		d->Base.alterSilver(tmpValue);
		doJson.append(getID());
		doJson.append(tmpValue);
		return doJson;
	}

	int actionAddGoldRes::Check(playerDataPtr d, Json::Value& Param)
	{
		return 0;
	}

	Json::Value actionAddGoldRes::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson;
		int tmpValue = ((d->Base.getLevel() / 20) + 5) * 20 * value[ACTION::strValue].asInt();
		d->Base.alterBothGold(tmpValue);
		doJson.append(getID());
		doJson.append(tmpValue);
		return doJson;
	}

	static int cTotalSecretaryNeedPos = 0;
	static int cTotalNeedPos = 0;
	const static int changeChipID = 13001;
	const static int expID = 14035;
	int actionDealWar::Check(playerDataPtr d, Json::Value& Param)
	{
		if(!Param.isMember(ACTION::strPilotList) || !Param[ACTION::strPilotList].isArray())return -1;
		return 0;
	}

	Json::Value actionDealWar::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson = Json::nullValue;
		Params::ObjectValue& config = item_sys.getConfig(expID);
		if(config.isEmpty() || !config.isMember(ItemDefine::merchandise))return doJson;
		Json::Value& ListJson = Param[ACTION::strPilotList];
		doJson.append(action_add_item);
		doJson.append(changeChipID);
		int playerLV = d->Base.getLevel();
		int totoalExp = 0;
		const int addExp = value[ACTION::strValue].asInt();
		for (unsigned i = 0; i < ListJson.size(); i++)
		{//所有在阵上的武将都加经验
			if(!ListJson[i].isInt())continue;
			int pilotID = ListJson[i].asInt();
			const playerPilot& cPilot = d->Pilots.getPlayerPilotExtert(pilotID);
			if(cPilot == playerPilots::NullPilotExtert)continue;
			if(cPilot.pilotLevel >= playerLV)
			{
				totoalExp += addExp;
				continue;
			}
		}
		Params::Var& material = config[ItemDefine::material];
		int mod = (material[ItemDefine::itemProvideIntStr][d->Base.getLevel()].asInt() / 60);
		mod = mod < 1 ? 1 : mod;
		int num = totoalExp / mod;
		num = d->Item.calAddItemLess(changeChipID, num);
		doJson.append(num);
		itemVector vec = d->Item.addItem(changeChipID, num);
		if(vec.empty())
		{
			doJson = Json::nullValue;
		}
		return doJson;
	}

	int actionAddPilotExp::Check(playerDataPtr d, Json::Value& Param)
	{
		if(!Param.isMember(ACTION::strPilotList) || !Param[ACTION::strPilotList].isArray())return -1;
		return 0;
	}

	Json::Value actionAddPilotExp::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson = Json::nullValue;
		Json::Value& ListJson = Param[ACTION::strPilotList];
		Json::Value PilotInfo = Json::arrayValue;
		doJson.append(getID());
		doJson.append(value[ACTION::strValue].asInt());
		bool single = getID() == action_add_pilot_exp ? true : false;
		for (unsigned i = 0; i < ListJson.size(); i++)
		{//所有在阵上的武将都加经验
			if(!ListJson[i].isInt())continue;
			int pilotID = ListJson[i].asInt();
			const playerPilot& cPilot = d->Pilots.getPlayerPilotExtert(pilotID);
			if(cPilot == playerPilots::NullPilotExtert)continue;
			Json::Value pilotJson;
			pilotJson.append(cPilot.pilotID);
			pilotJson.append(cPilot.pilotLevel);
			pilotJson.append(cPilot.pilotExp);
			d->Pilots.addExp(pilotID, value[ACTION::strValue].asInt());
			pilotJson.append(cPilot.pilotLevel);
			pilotJson.append(cPilot.pilotExp);
			bool isUpper = cPilot.pilotLevel >=  d->Base.getLevel() ? true : false;
			pilotJson.append(isUpper);
			PilotInfo.append(pilotJson);
			if(single && i == 0)break;
		}
		if(PilotInfo.empty())doJson = Json::nullValue;
		else doJson.append(PilotInfo);
		return doJson;
	}

	int actionAddPilot::Check(playerDataPtr d, Json::Value& Param)
	{
		return  0;
	}

	Json::Value actionAddPilot::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson = Json::nullValue;
		doJson.append(getID());
		Json::Value activeJson = Json::arrayValue;
		Params::Var& ListVar = value[ACTION::strPilotActiveList];
		for (unsigned i = 0; i < ListVar.getSize(); i++)
		{
			int pilotID = ListVar[i].asInt();
			int res = d->Pilots.activePilot(pilotID);
			if(res != 0)continue;
			d->Pilots.enlist(pilotID);
			activeJson.append(pilotID);
		}
		if(activeJson.empty())doJson = Json::nullValue;
		else doJson.append(activeJson);
		return doJson;
	}

	int actionAddKeji::Check(playerDataPtr d, Json::Value& Param)
	{
		return 0;
	}

	Json::Value actionAddKeji::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson;
		int tmpValue = value[ACTION::strValue].asInt();
		if (Param.isMember(ACTION::strKejiBuffer))
		{
			tmpValue = int(tmpValue * (1 + Param[ACTION::strKejiBuffer].asDouble()));
		}
		d->Base.alterKeJi(tmpValue);
		doJson.append(getID());
		doJson.append(tmpValue);
		return doJson;
	}


	int actionActivePilot::Check(playerDataPtr d, Json::Value& Param)
	{
		return 0;
	}

	Json::Value actionActivePilot::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson = Json::nullValue;
		doJson.append(getID());
		Json::Value activeJson = Json::arrayValue;
		Params::Var& ListVar = value[ACTION::strPilotActiveList];
		bool bC = Param[ACTION::strFromWarStoryWin].asBool();
		const static string ParamListStr = "pl";
		Json::Value bCast;
		playerManager::playerDataVec vec;
		if(bC)
		{
			bCast[msgStr][0u] = 0;
			bCast[msgStr][1u][senderChannelStr] =chat_kingdom;
			bCast[msgStr][1u][chatBroadcastID] = BROADCAST::active_pilot_event;
			bCast[msgStr][1u][ParamListStr][0u] =(d->Base.getName());
			vec = player_mgr.onlinePlayerSphereID(d->Base.getSphereID());
		}
		for (unsigned i = 0; i < ListVar.getSize(); i++)
		{
			int pilotID = ListVar[i].asInt();
			int res = d->Pilots.activePilot(pilotID);
			if(res != 0)continue;
			activeJson.append(pilotID);
			if(bC)
			{
				bCast[msgStr][1u][ParamListStr][1u] =pilotID;
				MsgSignOnline(vec, bCast, gate_client::chat_broadcast_resp);
			}
		}
		if(activeJson.empty())doJson = Json::nullValue;
		else doJson.append(activeJson);
		return doJson;
	}

	int actionAddRoleExp::Check(playerDataPtr d, Json::Value& Param)
	{
		//		if(!Param.isArray() || !Param[0u].isInt())return -1;
		return 0;
	}

	Json::Value actionAddRoleExp::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson;
		int tmpValue = value[ACTION::strValue].asInt();
		if(!Param[ACTION::strAddPlayerExpRate].isNull())
			tmpValue = int(tmpValue * Param[ACTION::strAddPlayerExpRate].asDouble());
		doJson.append(getID());
		doJson.append(tmpValue);
		doJson.append(d->Base.getLevel());
		doJson.append(d->Base.getExp());
		d->Base.addExp(tmpValue);
		doJson.append(d->Base.getLevel());
		doJson.append(d->Base.getExp());
		doJson.append( (d->Base.getLevel() >= d->Base.calMaxLv()) );
		return doJson;
	}

	int actionAddItem::Check(playerDataPtr d, Json::Value& Param)
	{
		if(!value.isMember(ACTION::addNumStr) || !value.isMember(ACTION::addItemIDStr))return -1;
		int itemID = value[ACTION::addItemIDStr].asInt();
		int num = value[ACTION::addNumStr].asInt();
		if (Param.isMember(ACTION::strDisintegrateTimes))
		{
			num *= Param[ACTION::strDisintegrateTimes].asInt();
		}
		int needSpace = d->Item.addItemNeedSpace(itemID, num);
		cTotalNeedPos += needSpace;
		if(d->Item.getLeavePlace() < cTotalNeedPos)return 1;
		return 0;
	}

	Json::Value actionAddItem::Do(playerDataPtr d, Json::Value& Param)
	{
		int itemID = value[ACTION::addItemIDStr].asInt();
		int num = value[ACTION::addNumStr].asInt();
		if (Param.isMember(ACTION::strDisintegrateTimes))
		{
			int base = num * 5;
			num *= Param[ACTION::strDisintegrateTimes].asInt();
			//发送广播
			int cast_times = Param[ACTION::strDisintegrateCast].asInt();
			Json::Value jStr;
			jStr[playerNameStr] = d->Base.getName();
			jStr[ACTION::addNumStr] = base;
			for (int i = 0; i < cast_times; ++i)
			{
				chat_sys.sendToAllBroadCastCommon(BROADCAST::crystal_disintegration_cri, jStr);
			}
		}
		d->Item.addItem(itemID, num);
		Json::Value doJson;
		doJson.append(getID());
		doJson.append(itemID);
		doJson.append(num);
		if(Param.isMember(ACTION::strFromWarStoryWin) && Param[ACTION::strFromWarStoryWin].asBool())
		{
			Params::ObjectValue& obj = item_sys.getConfig(itemID);
			int quality = obj[ItemDefine::itemQualityStr].asInt();
			int channelID = chat_area;
			Json::Value objJson = Params::ObjectValue::toJson(obj);
			if (quality > 2 && objJson[ItemDefine::chip].isNull())
			{
				if(quality == 5)channelID = chat_all;
				else if(quality == 4)channelID = chat_kingdom;

				Json::Value bCast;
				bCast[msgStr][0u] = 0;
				bCast[msgStr][1u][senderChannelStr] = channelID;
				bCast[msgStr][1u][chatBroadcastID] = BROADCAST::reward_good_item;
				const static string ParamListStr = "pl";
				bCast[msgStr][1u][ParamListStr].append(d->Base.getName());
				if(Param.isMember(ACTION::strCurrentWarStoryID))bCast[msgStr][1u][ParamListStr].append(0);
				else bCast[msgStr][1u][ParamListStr].append(1);
				bCast[msgStr][1u][ParamListStr].append(itemID);
				bCast[msgStr][1u][ParamListStr].append(num);
				if (quality == 3)player_mgr.sendToArea(gate_client::chat_broadcast_resp, bCast, d->Base.getPosition().first);
				else if (quality == 4)player_mgr.sendToSphere(gate_client::chat_broadcast_resp, bCast, d->Base.getSphereID());
				else player_mgr.sendToAll(gate_client::chat_broadcast_resp, bCast);
			}
			if(Param.isMember(ACTION::strCurrentWarStoryID) && Param[ACTION::strCurrentWarStoryID].isInt())
			{
				int localID = Param[ACTION::strCurrentWarStoryID].asInt();
				Json::Value annouceJson;
				annouceJson[msgStr][0u] = 0;
				annouceJson[msgStr][1u] = d->Base.getName();
				annouceJson[msgStr][2u] = itemID;
				string s = annouceJson.toStyledString();
				msg_json mj(gate_client::war_story_team_annouce_gain_item_resp, s);
				war_story.sendToTeamList(localID, mj);
			}
		}		
		return doJson;
	}

	void actionAddItem::clearStaticData()
	{
		cTotalNeedPos = 0;
	}

// 	int actionAddBuff::Check(playerDataPtr d, Json::Value& Param)
// 	{
// 		BuffPtr buff = BuffManager::Create(value);
// 		if(!d->Buff.canAddBuff(buff))return 1;
// 		return 0;
// 	}
// 
// 	Json::Value actionAddBuff::Do(playerDataPtr d, Json::Value& Param)
// 	{
// 		BuffPtr buff = BuffManager::Create(value);
// 		Json::Value doJson = Json::nullValue;
// 		if(d->Buff.addBuff(buff))
// 		{
// 			doJson.append(getID());
// 			doJson.append(value[BUFF::strBuffID].asInt());
// 		}
// 		return doJson;
// 	}


	actionRateAddItem::actionRateAddItem(Json::Value& dataJson) : actionBase(dataJson)
	{
		ADD = false;
	}

	int actionRateAddItem::Check(playerDataPtr d, Json::Value& Param)
	{
		//检查本身value是否有问题
		if (!value.isMember(ACTION::addNumStr) || !value.isMember(ACTION::addItemIDStr))return -1;

		int res = 0;
		do 
		{
			//首推必定掉落
			if (Param.isMember(ACTION::strFromFirstWarStoryWin) && Param[ACTION::strFromFirstWarStoryWin].asBool())break;

			//buff增加掉落概率
			double extraRate = 0.0f;
			if (!Param[ACTION::strAddItemExtraRate].isNull())
			{
				extraRate = Param[ACTION::strAddItemExtraRate].asDouble();
			}

			//概率掉落
			if (commom_sys.randomOk(value[ACTION::strAddItemRate].asDouble() * (1 + extraRate)))break;
			
			//没有添加
			return 0;
		} while (false);
		
		int itemID = value[ACTION::addItemIDStr].asInt();
		int num = value[ACTION::addNumStr].asInt();
		int needSpace = d->Item.addItemNeedSpace(itemID, num);
		cTotalNeedPos += needSpace;
		if(d->Item.getLeavePlace() < cTotalNeedPos)return 1;
		ADD = true;
		return 0;
	}

	Json::Value actionRateAddItem::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson = Json::nullValue;
		if(!ADD)return doJson;
		int itemID = value[ACTION::addItemIDStr].asInt();
		int num = value[ACTION::addNumStr].asInt();
		d->Item.addItem(itemID, num);
		doJson.append(getID());
		doJson.append(itemID);
		doJson.append(num);
		if(Param.isMember(ACTION::strFromWarStoryWin) && Param[ACTION::strFromWarStoryWin].asBool())
		{
			Params::ObjectValue& obj = item_sys.getConfig(itemID);
			int quality = obj[ItemDefine::itemQualityStr].asInt();
			int channelID = chat_area;
			Json::Value objJson = Params::ObjectValue::toJson(obj);
			if (quality > 2 && objJson[ItemDefine::chip].isNull())
			{
				if(quality == 5)channelID = chat_all;
				else if(quality == 4)channelID = chat_kingdom;

				Json::Value bCast;
				bCast[msgStr][0u] = 0;
				bCast[msgStr][1u][senderChannelStr] = channelID;
				bCast[msgStr][1u][chatBroadcastID] = BROADCAST::reward_good_item;
				const static string ParamListStr = "pl";
				bCast[msgStr][1u][ParamListStr].append(d->Base.getName());
				if(Param.isMember(ACTION::strCurrentWarStoryID))bCast[msgStr][1u][ParamListStr].append(0);
				else bCast[msgStr][1u][ParamListStr].append(1);
				bCast[msgStr][1u][ParamListStr].append(itemID);
				bCast[msgStr][1u][ParamListStr].append(num);
				if (quality == 3)player_mgr.sendToArea(gate_client::chat_broadcast_resp, bCast, d->Base.getPosition().first);
				else if (quality == 4)player_mgr.sendToSphere(gate_client::chat_broadcast_resp, bCast, d->Base.getSphereID());
				else player_mgr.sendToAll(gate_client::chat_broadcast_resp, bCast);
			}
			if(Param.isMember(ACTION::strCurrentWarStoryID) && Param[ACTION::strCurrentWarStoryID].isInt())
			{
				int localID = Param[ACTION::strCurrentWarStoryID].asInt();
				Json::Value annouceJson;
				annouceJson[msgStr][0u] = 0;
				annouceJson[msgStr][1] = d->Base.getName();
				annouceJson[msgStr][2] = itemID;
				string s = annouceJson.toStyledString();
				msg_json mj(gate_client::war_story_team_annouce_gain_item_resp, s);
				war_story.sendToTeamList(localID, mj);
			}
		}		
		return doJson;
	}

	void actionRateAddItem::clearStaticData()
	{
		cTotalNeedPos = 0;
		ADD = false;
	}

	actionAddKejiYaoSai::actionAddKejiYaoSai(Json::Value& dataJson) : actionBase(dataJson)
	{
		RET = false;
	}
	
	actionAddKejiYaoSai::~actionAddKejiYaoSai(){}

	int actionAddKejiYaoSai::Check(playerDataPtr d, Json::Value& Param)
	{
		if(Param[ACTION::strYaoSaiParamBase].isDouble() || Param[ACTION::strYaoSaiParamBase].isInt())RET = true;
		return RET ? 0 : 1;
	}

	Json::Value actionAddKejiYaoSai::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson = Json::nullValue;
		if(!RET)return doJson;
		int fixed =  value[ACTION::strValue].asInt();
		int tmpValueBase = int(fixed * (1 + Param[ACTION::strYaoSaiParamBase].asDouble()));
		int tmpValeWinNum = int(fixed * (Param[ACTION::strYaoSaiParamWinNum].asDouble()));
		d->Base.alterKeJi(tmpValueBase + tmpValeWinNum);
		doJson.append(getID());
		doJson.append(tmpValueBase);
		doJson.append(tmpValeWinNum);
		return doJson;
	}

	void actionAddKejiYaoSai::clearStaticData()
	{
		RET = false;
	}

	actionAddSecretary::actionAddSecretary(Json::Value& dataJson) : actionBase(dataJson)
	{
		
	}

	int actionAddSecretary::Check(playerDataPtr d, Json::Value& Param)
	{
		/*Params::Var& ListVar = value[ACTION::strSecretaryList];
		cTotalSecretaryNeedPos += ListVar.getSize();
		return  d->PlayerSecretaries.canAddSecretary(cTotalSecretaryNeedPos)? 0 : 1;*/
		return 0;
	}

	Json::Value actionAddSecretary::Do(playerDataPtr d, Json::Value& Param)
	{
		Json::Value doJson = Json::nullValue;
		doJson.append(getID());
		Json::Value activeJson = Json::arrayValue;
		Params::Var& ListVar = value[ACTION::strSecretaryList];
		for (unsigned i = 0; i < ListVar.getSize(); i++)
		{
			int secID = ListVar[i].asInt();
			int res = d->PlayerSecretaries.addSecretatyWithoutCheck(secID);
			if (res != 0)continue;
			activeJson.append(secID);
		}
		if (activeJson.empty())doJson = Json::nullValue;
		else doJson.append(activeJson);
		return doJson;
	}

	void actionAddSecretary::clearStaticData()
	{
		cTotalSecretaryNeedPos = 0;
	}
}