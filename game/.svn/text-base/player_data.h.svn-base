#pragma once
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "time_helper.h"
#include "player_base.h"
#include "player_pilots.h"
#include "player_war_story.h"
#include "player_embattle.h"
#include "player_item.h"
#include "player_levy.h"
#include "player_vip.h"
#include "player_workshop.h"
#include "player_science.h"
#include "player_campaign.h"
#include "player_guild.h"
#include "player_study.h"
#include "player_email.h"
#include "player_shop.h"
#include "player_sign.h"
#include "player_office.h"
#include "player_online_award.h"
#include "player_arena.h"
#include "player_ruler.h"
#include "player_task.h"
#include "player_rebate.h"
#include "player_point.h"
#include "player_explore.h"
#include "player_ally.h"
#include "player_gift_defined.h"
#include "refresh_tick.h"
#include "player_starwar.h"
#include "player_crystal_exchange.h"
#include "player_inspect_tour.h"
#include "player_inviter.h"
#include "player_space_explore.h"
#include "player_world_boss.h"
#include "player_trade.h"
#include "player_market.h"
#include "DataNotice.hpp"
#include "player_secretary.h"
#include "player_activity.h"
#include "player_paper.h"

namespace gg
{
	enum CHAT
	{
		chat_all = -1,
		chat_kingdom = 0,
		chat_area = 1,
		chat_guild = 2,
		chat_player = 3,
		chat_team = 4,//组队
		chat_server = 5,//系统信息
		chat_end,
		chat_num,
	};

	enum DANMU
	{
		error_channel = -1,
		boss_channel = 0,

		end_channel,
	};


	class playerManager;
	class playerData : 
		public boost::enable_shared_from_this<playerData>
	{		
		VarWorkPtr Complete;
		friend class playerManager;
		friend class playerBase;
	public:
		static playerDataPtr CreatplayerData(const int player_id, const int net_id /* = -1 */)
		{
			void* m = GGNew(sizeof(playerData));
			playerDataPtr d = playerDataPtr(new(m) playerData(player_id), Destroy);
			d->netID = net_id;
			return d;
		}
		static void Destroy(playerData* p)
		{
			p->~playerData();
			GGDelete(p);
		}
		inline playerDataPtr getOwnDataPtr(){
			return shared_from_this();
		}
		inline bool isFirstLogin(){ return firstLogin; }
		bool isOperatBattle();
		bool isOperatBattleWithoutTimeLimit();
		void SetBattleState();
		void SetFreeSate();
		bool isBattleState(){return isBattle;}
		bool isVaild();
		bool load();
		void load_end();
		inline bool isOnline()
		{
			return netID >= 0;
		}
		inline VarWorkPtr _Handler()
		{
			return Complete;
		}
		void sendToClient(const short protocol, Json::Value& msg);
		const int playerID;
		unsigned getOffineTime(){return offlineTime;}
		unsigned getLoginTime(){return loginTime;}
		void tickLogin();
		int netID;
		unsigned ChatArray[chat_num];
		unsigned DanmuArray[end_channel];
		unsigned lastSharedTime;
		unsigned showTime[2];
		unsigned tickServerTime;

		playerBase Base;//个人信息
		playerPilots Pilots;//pilots
//		playerBuilds Builds;//building
		playerAllWarStory Warstory;//war story
		playerEmbattle Embattle;//embattle
		playerWare Item;//道具
		playerLevy Levy;//征收信息
		playerVip Vip;//VIP信息
		playerCampaign Campaign;//军令信息
		playerGuild Guild;//工会信息
		playerWorkShop Workshop;//作坊信息
		playerScience Science;//科技
		playerStudy Study;//刷书委派
		playerEmail Email;//邮件
//		playerBuffManager Buff;//玩家自身buff
		playerShop Shop;
		playerSign Sign;
		playerOffice Office;
		playerOnlineAward onlineAward;//在线奖励
		playerArena Arena;
		playerRuler Ruler;
		playerTask Task;
		playerRebate Rebate;
		playerPoint Points;
		player_explore playerExplore;	//探险
		playerAlly Ally;
		playerGiftDefined GiftDefined;
		player_crystal_exchange crystalEx;
		playerInspectTour InspectTour;
		player_inviter inviter;
		TTManager TickMgr;//时间触发器
		playerStarwar Starwar;
		playerSpaceExplore SpaceExplore;
		playerWorldBoss WorldBoss;
		playerTrade Trade;
		playerTradeBag TradeItem;
		playerTradeSkill TradeSkill;
		playerMarket PlayerMarket;
		playerSecretaries PlayerSecretaries;
		playerActivity PlayerActivity;
		playerPaper Paper;
	protected:
		playerData(const int pID);
		~playerData();
		bool isBattle;
		boost::system_time battleTick;
		unsigned offlineTime;
		unsigned loginTime;
		bool firstLogin;
	};
}