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
		Pilots.save();//ǿ�Ʊ��汣������
		Pilots.doEnd();//����dirty����




		//end
		*Complete = true;
	}

	bool playerData::load()
	{
		bool ret = true;
//		LogS << playerID << color_pink(" load from mongo DB ...") << LogEnd;
		do 
		{
			//������Ϣ
			FalseBreak(Base);
			//�Ƽ�ϵͳ ս��ϵͳ ����ϵͳ ����Ƽ�,�������ȶ�ȡ
			TradeSkill.get();//����
			Science.get();
			PlayerSecretaries.get();
			//��Ʒ must be initial before pilots
			Item.get();
			//�佫ϵͳ
			Pilots.get();
			//����
//			Builds.get();
			//��ս
			Warstory.get();
			//����
			Embattle.get();
			//����
			Levy.get();
			//VIP
			Vip.get();
			//����ϵͳ
			Workshop.get();
			//������Ϣ
			Campaign.get();
			//������Ϣ
			Guild.get();
			//�о�ϵͳ
			Study.get();
			//�ʼ�
			Email.get();
			//Buff�б�
//			Buff.get();
			//�̵�ϵͳ
			Shop.get();
			//ǩ��ϵͳ
			Sign.get();
			//������Ϣ
			onlineAward.get();
			//������ϵͳ
			Arena.get();
			//��ְϵͳ
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