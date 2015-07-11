#include "player_data.h"
#include "time_helper.h"
#include "playerManager.h"
#include "Glog.h"
#include "DataNotice.hpp"

namespace gg
{
#define FalseBreak(DoName) \
	if(!(DoName.get()))\
	{\
		ret = false;\
		break;\
	}

	static void destoryWork(bool* pointer)
	{
		gg::GGDelete(pointer);
	}

	playerData::playerData(const int pID) : Complete(VarWorkPtr(new(gg::GGNew(sizeof(bool))) bool(false), destoryWork)),
		offlineTime(na::time_helper::get_current_time()),
		playerID(pID), Base(*this), /*Builds(*this),*/Science(*this), Pilots(*this), Embattle(*this), 
		Item(*this), Levy(*this), Vip(*this), Workshop(*this), Campaign(*this), Guild(*this),
		Study(*this),/* CDList(*this),*/ /*Buff(*this), */Shop(*this), Sign(*this), Email(*this), Office(*this),
		onlineAward(*this), Arena(*this), Ruler(*this),Task(*this),
		Rebate(*this), Points(*this), playerExplore(*this), Warstory(*this), Ally(*this), GiftDefined(*this), crystalEx(*this),
		TickMgr(*this), InspectTour(*this), Starwar(*this), inviter(*this), SpaceExplore(*this), WorldBoss(*this),
		Trade(*this), TradeItem(*this), TradeSkill(*this), PlayerMarket(*this), PlayerSecretaries(*this), PlayerActivity(*this),
		Paper(*this)
	{
		isBattle = false;
		memset(ChatArray, 0x0, sizeof(ChatArray));
		memset(DanmuArray, 0x0, sizeof(DanmuArray));
		loginTime = na::time_helper::get_current_time();
		showTime[0] = na::time_helper::get_current_time() - 60;
		showTime[1] = showTime[0];
		lastSharedTime = na::time_helper::get_current_time() - 300;
		tickServerTime = na::time_helper::get_current_time() + 240;
		firstLogin = false;
	}

	playerData::~playerData()
	{
		//LogS << "player:" << playerID << "  has been free ~" << LogEnd;
		*Complete = false;
	}

	void playerData::sendToClient(const short protocol, Json::Value& msg)
	{
		player_mgr.sendToPlayer(playerID, netID, protocol, msg);
	}

	bool playerData::isOperatBattle()
	{
		boost::system_time tmp = na::time_helper::get_sys_time();
		return ((tmp - battleTick).total_milliseconds() < 2333 || isBattle);
	}

	bool playerData::isOperatBattleWithoutTimeLimit()
	{
		return isBattle;
	}

	void playerData::SetBattleState()
	{
		isBattle = true;
		battleTick = na::time_helper::get_sys_time();
	}

	void playerData::SetFreeSate()
	{
		isBattle = false;
	}

	bool playerData::isVaild()
	{
		bool ret = false;
		do 
		{
			if(playerID <= 0)break;
			if(!Base.isVaild())break;
			ret = true;
		} while (false);
		return ret;
	}

	void playerData::tickLogin()
	{
		Base.tickLogin();
		loginTime = na::time_helper::get_current_time();
	}

	void playerData::load_end()
	{
		if (isOnline())
		{

		}

		//...etc
		Pilots.save();//强制保存保存数据
		Pilots.doEnd();//清理dirty数据




		//end
		*Complete = true;
	}

	bool playerData::load()
	{
		bool ret = true;
//		LogS << playerID << color_pink(" load from mongo DB ...") << LogEnd;
		do 
		{
			//基本信息
			FalseBreak(Base);
			//科技系统 战力系统 秘书系统 必须科技,秘书优先读取
			TradeSkill.get();//技能
			Science.get();
			PlayerSecretaries.get();
			//物品 must be initial before pilots
			Item.get();
			//武将系统
			Pilots.get();
			//建筑
//			Builds.get();
			//征战
			Warstory.get();
			//布阵
			Embattle.get();
			//征收
			Levy.get();
			//VIP
			Vip.get();
			//生产系统
			Workshop.get();
			//军令信息
			Campaign.get();
			//工会信息
			Guild.get();
			//研究系统
			Study.get();
			//邮件
			Email.get();
			//Buff列表
//			Buff.get();
			//商店系统
			Shop.get();
			//签到系统
			Sign.get();
			//奖励信息
			onlineAward.get();
			//竞技场系统
			Arena.get();
			//官职系统
			Office.get();
			//active sys
			Ruler.get();
			Task.get();
			Rebate.get();
			Points.get();
			playerExplore.get();
			Ally.get();
			GiftDefined.get();
			TickMgr.get();
			Starwar.get();
			crystalEx.get();
			inviter.get();
			SpaceExplore.get();
			WorldBoss.get();
			Trade.get();
			TradeItem.get();
			PlayerMarket.get();
			PlayerActivity.get();
			Paper.get();
		} while (false);
//		LogS << playerID << color_pink(" load end ...") << LogEnd;
		return ret;
	}
}