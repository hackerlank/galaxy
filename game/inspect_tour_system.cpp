#include "inspect_tour_system.h"
#include "playerManager.h"
#include "response_def.h"
#include "action_format.hpp"
#include "email_system.h"

namespace gg
{
using namespace inspect_tour_def;
	inspectTourSystem * const inspectTourSystem::inspectTourSys = new inspectTourSystem();
	inspectTourSystem::inspectTourSystem()
	{

	}

	inspectTourSystem::~inspectTourSystem()
	{

	}

	void inspectTourSystem::initData()
	{

	}

	void inspectTourSystem::inspectTourUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->InspectTour.autoUpdate();
	}

	void inspectTourSystem::inspectTourSimpleFinishReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int eventID = js_msg[0u].asInt();
		if (!(eventID > type_simple_start && eventID < type_simple_end))
			Return(r, -1);

		if (!d->InspectTour.canOperate(eventID))
			Return(r, 1);

		simpleTourPtr simpleEvent = getSimpleTourConfig(eventID);
		if (simpleEvent == simpleTourPtr())
			Return(r, -1);

		vector<actionResult> vecRetCode;
		vecRetCode = actionFormat::actionDoJump(d, simpleEvent->simpleRewardJson);
		Json::Value failSimpleJson;
		email_sys.sendDoJumpFailEmail(d, simpleEvent->simpleRewardJson, vecRetCode, getEmailTeam(eventID), failSimpleJson);
		d->InspectTour.removeEvent(eventID);

		Return(r, 0);
	}

	void inspectTourSystem::inspectTourXingjidadaoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		if (!d->InspectTour.canOperate(type_xingjidadao))
			Return(r, 1);

		bool needFight = js_msg[0u].asBool();
		if (!needFight)
		{
			vector<actionResult> vecRetCode;
			vecRetCode = actionFormat::actionDoJump(d, rewardJsonXingjidadao[0]);
			Json::Value failSimpleJson;
			email_sys.sendDoJumpFailEmail(d, rewardJsonXingjidadao[0], vecRetCode, getEmailTeam(type_xingjidadao), failSimpleJson);
			d->InspectTour.removeEvent(type_xingjidadao);
			Return(r, 0);
		}
		else
		{

		}
	}

	void inspectTourSystem::inspectTourBeikunshangchuanReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		if (!d->InspectTour.canOperate(type_beikunshangchuan))
			Return(r, 1);

		int resIndex = js_msg[0].asInt();
		if (resIndex < 0 || resIndex > 2)
			Return(r, -1);

		vector<actionResult> vecRetCode;
		vecRetCode = actionFormat::actionDoJump(d, rewardJsonBeikunshangchuan[resIndex]);
		Json::Value failSimpleJson;
		email_sys.sendDoJumpFailEmail(d, rewardJsonBeikunshangchuan[resIndex], vecRetCode, getEmailTeam(type_beikunshangchuan), failSimpleJson);
		d->InspectTour.removeEvent(type_beikunshangchuan);

		Return(r, 0);
		
	}

	void inspectTourSystem::inspectTourFukuangyunshiReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		if (!d->InspectTour.canOperate(type_fukuangyunshi))
			Return(r, 1);

		int randomRes;
		int res = d->InspectTour.operateFukuangyunshi(randomRes);
		r[msgStr][1] = randomRes;
		if (res == 0)
			d->InspectTour.removeEvent(type_fukuangyunshi);

		Return(r, 0);
	}

	void inspectTourSystem::inspectTourXijiehaidaoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		if (!d->InspectTour.canOperate(type_xijiehaidao))
			Return(r, 1);

		int resIndex = commom_sys.randomBetween(0, 2);
		vector<actionResult> vecRetCode;
		vecRetCode = actionFormat::actionDoJump(d, rewardJsonXingjidadao[resIndex]);
		Json::Value failSimpleJson;
		email_sys.sendDoJumpFailEmail(d, rewardJsonXingjidadao[resIndex], vecRetCode, getEmailTeam(type_xijiehaidao), failSimpleJson);
		d->InspectTour.removeEvent(type_xijiehaidao);

		Return(r, 0);

	}

	void inspectTourSystem::inspectTourShenmishangduiReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		if (!d->InspectTour.canOperate(type_shenmishangdui))
			Return(r, 1);

		bool isEnough = false;
		if (!isEnough)
		{
			vector<actionResult> vecRetCode;
			vecRetCode = actionFormat::actionDoJump(d, rewardJsonShenmishangdui[0u]);
			Json::Value failSimpleJson;
			email_sys.sendDoJumpFailEmail(d, rewardJsonShenmishangdui[0u], vecRetCode, getEmailTeam(type_shenmishangdui), failSimpleJson);
		}
		else
		{
			//ºı…Ÿ◊ ‘¥
			vector<actionResult> vecRetCode;
			vecRetCode = actionFormat::actionDoJump(d, rewardJsonShenmishangdui[1]);
			Json::Value failSimpleJson;
			email_sys.sendDoJumpFailEmail(d, rewardJsonShenmishangdui[1], vecRetCode, getEmailTeam(type_shenmishangdui), failSimpleJson);
		}
		d->InspectTour.removeEvent(type_shenmishangdui);
		Return(r, 0);

	}

	simpleTourPtr inspectTourSystem::getSimpleTourConfig(int eventID)
	{
		if (!(eventID > type_simple_start && eventID < type_simple_end))
			return simpleTourPtr();

		map<int, simpleTourPtr>::iterator it = simpleTourInfo_.find(eventID);
		if (it != simpleTourInfo_.end())
			return it->second;

		return simpleTourPtr();
	}

	gg::emailTeam inspectTourSystem::getEmailTeam(int eventID)
	{
		switch (eventID)
		{
			//simple
		case type_zhanchangfeixu:
			return email_team_system_bag;
		case type_taikongxiulizhan:
			return email_team_system_bag;
		case type_taikongchongnengzhan:
			return email_team_system_bag;
		case type_qiujiuxinhao:
			return email_team_system_bag;
			//complex
		case type_xingjidadao:
			return email_team_system_bag;
		case type_beikunshangchuan:
			return email_team_system_bag;
		case type_fukuangyunshi:
			return email_team_system_bag;
		case type_xijiehaidao:
			return email_team_system_bag;
		case type_shenmishangdui:
			return email_team_system_bag;
		default:
			break;
		}
		return email_team_null;
	}

}


