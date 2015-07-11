#pragma once

#include "playerManager.h"
#include "json/json.h"
#include <string>

using namespace na::msg;
using namespace std;

#define chat_sys (*gg::chat_system::chatSys)

namespace gg
{
#define KEY_FORBIDDEN 1
#define KEY_ROLLING_BROADCAST 2

	namespace BROADCAST
	{
		enum
		{
			arean_report,         // 竞技场战报
			ruler_broadcast,    // 国王争霸战广播
			npc_team_invite,	//要塞战组队广播
			vip_get_gift,		//领取VIP奖励广播
			gm_chat_all,		//gm系统聊天
			gm_chat_single,		//gm私聊玩家(预留接口)
			first_pass_war_story,//地图首推广播
			guild_leader_replace,//公会会长被人顶替的广播
			player_change_name,//玩家改名的广播
			shared_report,//战报分享的广播
			guild_leader_change,//公会会长转让的广播
			guild_member_leave,//公会有人离开的广播
			guild_member_join,//公会有人加入的广播
			join_sphere_event,//加入国家的广播
			active_pilot_event,//收复征战时候的武将的广播
			reward_good_item,//获得黄装以上的广播
			arena_summary,       // 竞技场结算广播
			guild_battle_apply_replace,//我星盟对XX星域的攻打资格已被AAA星盟顶替
			guild_battle_apply_success,//我星盟已报名攻打XX星域，请大家积极备战，晚上20点准时参加。
			guild_battle_begin_join_team,//XXX星盟对AAA星盟的星域争夺战15分钟后开启
			guild_battle_fight_result,//AAA星盟战胜BBB星盟占据了XX星域
			world_pvp_broadcast,//XXX击败了A国的 (威胁度) CCC
			study_reward,//研究获得物品
			npc_team_bonus_time_12_30,//要塞特定时间额外加成公告
			npc_team_bonus_time_20_00,//要塞特定时间额外加成公告
			guild_battle_to_all_can_apply,//领地争夺战可以开始报名了【报名】
			guild_battle_to_guild_fight_will_com_19_50,//领地争夺战还有10分钟就开战了【入场】
			guild_battle_to_guild_fight_will_com_20_40,//领地争夺战还有10分钟就开战了【入场】
			guild_battle_to_guild_invite,//会长或副会长邀请入场
			crystal_disintegration_cri,		//水晶分解暴击广播
			guild_help_announce,//求助之后的通知
			starwar_prime_time_coming,   // 国战黄金时间开启前5分钟
			starwar_prime_time_begin,      // 国战黄金时间开启
			starwar_king_gather,                 // 国战集结令
			interplanetary_broadcast,		//星际探险广播
			starwar_kill_king,                        // 国战击杀国王
			starwar_exploit_summary,       // 国战功勋榜结算
			mine_res_bonus_12_30, //12:30-14:00精英时间内，时空矿场可获奖励加成
			mine_res_bonus_20_00, //20:00-23:00精英时间内，时空矿场可获奖励加成
			starwar_prime_time_summary,
			space_explore_five_times_critical, //寻宝系统金币五倍暴击
			space_explore_purple_equip, //寻宝系统紫装
			world_boss_will_come_20_35, //急报！ 30分钟后 巨型战舰入侵！各位指挥官做好迎敌准备！
			world_boss_will_come_21_00, //急报！5分钟后 巨型战舰入侵！各位指挥官做好迎敌准备！
			world_boss_will_come_21_04_55, //“急报！巨型战舰来袭！请各位总督速速前往战场！ 【进入】”
			world_boss_end_a1, //
			world_boss_end_a2, //
			world_boss_end_a3, //
			world_boss_end_b,
			world_boss_damage_state, //BOSS被打出负面状态
			world_boss_drop_percent, //BOSS被打到70%血量
			world_boss_guild_inspire, //"sd":{"na":"xxx"},XXXX为你发出了激励的呐喊，帮助你增加了1层星盟激励层数
			guild_battle_19_30,
			lucky_ranking, // 5分钟后 幸运排名结算，在幸运排名玩家可获得幸运奖励。
			arena_summary_comming, // 竞技场即将结算
			ruler_running_broadcast,
			trade_voucher_show, // 贸易商票展示
			secretary_show, // 秘书展示
			secretary_room_show, // 秘书室展示
			get_secretary_broadcast, // 获得五星秘书广播
			paper_notice,//红包广播
			gm_paper_notice,//gm红包广播
			paper_receive_notice,//红包领取通知

			broadcast_end
		};
	}

	namespace ROLLBROADCAST
	{
		enum
		{
			roll_gm_chat,
			roll_guild_battle_is_coming_20_00,
			roll_guild_battle_is_coming_20_10,
			roll_war_story_team_begin_12_30,
			roll_war_story_team_begin_20_00,
			roll_ruler_begin,
			roll_ruler_coming,
			roll_starwar_prime_time_coming,
			roll_starwar_prime_time_begin,
			roll_starwar_prime_time_running,
			roll_guild_battle_is_coming_19_30,
			roll_world_boss_21_00_05,
			roll_world_boss_21_04_55,
			roll_lucky_ranking,
			roll_arena_summary_comming_21_45,
			roll_arena_summary_comming_21_50,
			roll_arena_summary_comming_21_55,
			roll_ruler_running_broadcast_21_25,
			roll_ruler_running_broadcast_21_30,
			roll_ruler_running_broadcast_21_35,
			roll_paper_over_10000,

			roll_broadcast_end
		};
	}

	namespace SHOW
	{
		enum
		{
			show_begin,
			
			show_pilot,
			show_item,

			show_end
		};
	}

	const static string chatKey = "key";
	const static string chatDbName = "gl.chat_system";
	const static string chatForbiddenList = "fl";
	const static string chatRollBroadcastId = "rid";
	const static string chatStartTime = "st";
	const static string chatEndTime = "et";
	const static string chatRollTimePeriodList = "rl";
	const static string chatRollBroadCastList = "rbl";
	const static string chatRollBroadCastInterVal = "bi";
	const static string chatRollBroadCastContent = "bc";
	const static string chatBroadcastID = "cbid";
	const static string chatRollBroadcastID = "rbid";
	const static string chatRollTimes = "rt";
	const static string chatShowID = "csid";

	const static string senderChannelStr = "cn";

	struct forbiddenPlayer 
	{
		int playerID;
		unsigned startTime;
		unsigned endTime;
	};

	struct timePeriod 
	{
		timePeriod();
		int startFromZeroTime_;
		int endFromZeroTime_;
	};

	struct rollBroacast
	{
		rollBroacast();
		bool isValid(unsigned tmp);
		int id_;
		int broadCastID_;
		int channel_;
		unsigned startZeroTime_;
		unsigned endZeroTime_;
		unsigned interval_;
		unsigned rollTimes_;
		vector<timePeriod> vecPeriod_;
		Json::Value contentJson_;//后台设置的string,要封装在senderWordsStr字段，如果要适用其他非后台设置的滚动公告，可能要修改读写数据库方式

		unsigned latestSendTime_;
	};

	class chat_system
	{
	public:
		static chat_system* const chatSys;
		void ChatShow(msg_json& m, Json::Value& r);
		void playerChatVoice(msg_json& m, Json::Value& r);
		void playerChat(msg_json& m, Json::Value& r);
		void gmForbidSpeakerReq(msg_json& m, Json::Value& r);
		void gmAllowSpeakerReq(msg_json& m, Json::Value& r);
		void gmForbidSpeakerUpdateReq(msg_json& m, Json::Value& r);
		void gmRollBroadcastUpdateReq(msg_json& m, Json::Value& r);
		void gmRollBroadcastSetReq(msg_json& m, Json::Value& r);
		void gmRollBroadcastDelReq(msg_json& m, Json::Value& r);
		void chatTableUpdate(msg_json& m, Json::Value& r);

		void chatRulerBroadcastResp(Json::Value& data, int channel, int arg = 0);
		void chatStrategyBroadcastResp(playerDataPtr d, int local_id, string& report);
		void sendToAllArenaSummary(playerDataPtr d, int rank, int silver, int weiwang, int gold);

		//public:
		void sendReportToAll(playerDataPtr d, playerDataPtr target, string filename);
		void sendToAllVipGetGift(playerDataPtr pdata, int vipLevel);
		void sendToAll(playerDataPtr sender, string& str);

		void sendToAllBroadCastCommon(int broadcastID, Json::Value chatContent = Json::Value::null);
		void sendToTeamBroadCastCommon(int broadcastID, Json::Value chatContent = Json::Value::null);
		void sendToAreaBroadCastCommon(int areaID, int broadcastID, Json::Value chatContent = Json::Value::null);
		void sendToGuildBroadCastCommon(int guildID, int broadcastID, Json::Value chatContent = Json::Value::null);
		void sendToSpereBroadCastCommon(int spereID, int broadcastID, Json::Value chatContent = Json::Value::null);

		void sendToAllRollBroadCastCommon(int broadcastID, int times = 3, Json::Value chatContent = Json::Value::null);
		void sendToAreaRollBroadCastCommon(int areaID, int broadcastID, int times = 3, Json::Value chatContent = Json::Value::null);
		void sendToGuildRollBroadCastCommon(int guildID, int broadcastID, int times = 3, Json::Value chatContent = Json::Value::null);
		void sendToSpereRollBroadCastCommon(int spereID, int broadcastID, int times = 3, Json::Value chatContent = Json::Value::null);

		void sendToAllByGM(string& str);
		int sendToPlayer(const int player_id, playerDataPtr sender,string& str);
		int sendToPlayerByGm(const string player_name, string& str);
		void initData();
		void chatUpdate(boost::system_time& tmp);
		int chatOption(const int chatID, const short protocol, Json::Value& msg, const int limitID = -1, const string targetNA = "");
		int chatOption(const int chatID, const short protocol, Json::Value& msg, playerDataPtr player, const string targetNA = "");

		void packageSender(playerDataPtr sender, Json::Value& senderJson);
		bool isForbidden(int player_id);
		void addSystemRollBroadCast(int broadcastID, int interval, vector<timePeriod> vecPeriod, int channel = chat_all, int rollTimes = 3, Json::Value chatContent = Json::Value::null);
	private:
#ifndef _PUBLISH_
		bool gmCommand(playerDataPtr player, string& words);
#endif
		bool saveForbiddenPlayer();
		bool getForbiddenPlayer();
		bool saveRollBroacast();
		bool getRollBroacast();
		void packageRollBroacast(Json::Value& pack);
		void packageForbiddenPlayer(Json::Value& pack);
		int getNewRollId(vector<rollBroacast> &vecRoll);
		void chatUpdateSingle(vector <rollBroacast> &singleVec);
		
		vector <forbiddenPlayer> forbidPlayers;
		vector <rollBroacast> vecRollBroadcast;
		vector <rollBroacast> vecSystemRollBroadcast_;
		boost::system_time tick;
	};
}