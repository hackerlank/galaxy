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
			arean_report,         // ������ս��
			ruler_broadcast,    // ��������ս�㲥
			npc_team_invite,	//Ҫ��ս��ӹ㲥
			vip_get_gift,		//��ȡVIP�����㲥
			gm_chat_all,		//gmϵͳ����
			gm_chat_single,		//gm˽�����(Ԥ���ӿ�)
			first_pass_war_story,//��ͼ���ƹ㲥
			guild_leader_replace,//����᳤���˶���Ĺ㲥
			player_change_name,//��Ҹ����Ĺ㲥
			shared_report,//ս������Ĺ㲥
			guild_leader_change,//����᳤ת�õĹ㲥
			guild_member_leave,//���������뿪�Ĺ㲥
			guild_member_join,//�������˼���Ĺ㲥
			join_sphere_event,//������ҵĹ㲥
			active_pilot_event,//�ո���սʱ����佫�Ĺ㲥
			reward_good_item,//��û�װ���ϵĹ㲥
			arena_summary,       // ����������㲥
			guild_battle_apply_replace,//�����˶�XX����Ĺ����ʸ��ѱ�AAA���˶���
			guild_battle_apply_success,//�������ѱ�������XX�������һ�����ս������20��׼ʱ�μӡ�
			guild_battle_begin_join_team,//XXX���˶�AAA���˵���������ս15���Ӻ���
			guild_battle_fight_result,//AAA����սʤBBB����ռ����XX����
			world_pvp_broadcast,//XXX������A���� (��в��) CCC
			study_reward,//�о������Ʒ
			npc_team_bonus_time_12_30,//Ҫ���ض�ʱ�����ӳɹ���
			npc_team_bonus_time_20_00,//Ҫ���ض�ʱ�����ӳɹ���
			guild_battle_to_all_can_apply,//�������ս���Կ�ʼ�����ˡ�������
			guild_battle_to_guild_fight_will_com_19_50,//�������ս����10���ӾͿ�ս�ˡ��볡��
			guild_battle_to_guild_fight_will_com_20_40,//�������ս����10���ӾͿ�ս�ˡ��볡��
			guild_battle_to_guild_invite,//�᳤�򸱻᳤�����볡
			crystal_disintegration_cri,		//ˮ���ֽⱩ���㲥
			guild_help_announce,//����֮���֪ͨ
			starwar_prime_time_coming,   // ��ս�ƽ�ʱ�俪��ǰ5����
			starwar_prime_time_begin,      // ��ս�ƽ�ʱ�俪��
			starwar_king_gather,                 // ��ս������
			interplanetary_broadcast,		//�Ǽ�̽�չ㲥
			starwar_kill_king,                        // ��ս��ɱ����
			starwar_exploit_summary,       // ��ս��ѫ�����
			mine_res_bonus_12_30, //12:30-14:00��Ӣʱ���ڣ�ʱ�տ󳡿ɻ����ӳ�
			mine_res_bonus_20_00, //20:00-23:00��Ӣʱ���ڣ�ʱ�տ󳡿ɻ����ӳ�
			starwar_prime_time_summary,
			space_explore_five_times_critical, //Ѱ��ϵͳ����屶����
			space_explore_purple_equip, //Ѱ��ϵͳ��װ
			world_boss_will_come_20_35, //������ 30���Ӻ� ����ս�����֣���λָ�ӹ�����ӭ��׼����
			world_boss_will_come_21_00, //������5���Ӻ� ����ս�����֣���λָ�ӹ�����ӭ��׼����
			world_boss_will_come_21_04_55, //������������ս����Ϯ�����λ�ܶ�����ǰ��ս���� �����롿��
			world_boss_end_a1, //
			world_boss_end_a2, //
			world_boss_end_a3, //
			world_boss_end_b,
			world_boss_damage_state, //BOSS���������״̬
			world_boss_drop_percent, //BOSS����70%Ѫ��
			world_boss_guild_inspire, //"sd":{"na":"xxx"},XXXXΪ�㷢���˼������ź���������������1�����˼�������
			guild_battle_19_30,
			lucky_ranking, // 5���Ӻ� �����������㣬������������ҿɻ�����˽�����
			arena_summary_comming, // ��������������
			ruler_running_broadcast,
			trade_voucher_show, // ó����Ʊչʾ
			secretary_show, // ����չʾ
			secretary_room_show, // ������չʾ
			get_secretary_broadcast, // �����������㲥
			paper_notice,//����㲥
			gm_paper_notice,//gm����㲥
			paper_receive_notice,//�����ȡ֪ͨ

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
		Json::Value contentJson_;//��̨���õ�string,Ҫ��װ��senderWordsStr�ֶΣ����Ҫ���������Ǻ�̨���õĹ������棬����Ҫ�޸Ķ�д���ݿⷽʽ

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