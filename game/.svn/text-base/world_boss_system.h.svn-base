#pragma once
#include "json/json.h"
#include "msg_base.h"
#include "block.h"
#include "war_story.h"

#define world_boss_sys (*gg::world_boss_system::worldBossSys)

using namespace na::msg;
namespace gg
{
	namespace world_boss_def
	{
		const static string strPersonalRankList = "prl";
		const static string strStorageCapacityList = "srl";
		const static string strMyRank = "mr";
		const static string strMyDamage = "md";
		const static string strCountryRankList = "crl";
		const static string strBossList = "bl";
		const static string strNormalInspire = "ni";;
		const static string strGuildInspire = "gi";
		const static string strRemainHP = "rhp";
		const static string strTotalReportList = "trl";
		const static string strMyReportList = "mrl";
		const static string strDamageState = "ds";
		const static string strBuffState = "bs";
		const static string strActivityState = "as";
		const static string strBossID = "bid";
		const static string strSeason = "se";
		const static string strNextSeason = "nse";
		const static string strPersonalReward = "prw";
		const static string strCountryReward = "crw";
		const static string strKillReward = "krw";
		const static string strRank = "rk";
		const static string strDamage = "dm";
		const static string strReward = "rw";
		const static string strBattleCD = "bcd";
		const static string strAtkName = "an";
		const static string strBeginTime = "bt";
		const static string strKillTime = "kt";
		const static string strKillTailName = "ktn";
		const static string strCurrentReport = "cr";
		const static string strReport = "r";
		const static string strHasStorageCapacity = "hsc";
		const static string strClearAttackCDCost = "cacdc";
		const static string strKejiInspireCost = "kjic";
		const static string strGoldInspireCost = "gic";
		const static string strDbWorldBoss = "gl.world_boss";
		const static string strDbPlayerWorldBoss = "gl.player_world_boss";
		const static string strKey = "key";
		const static string strWorldBossConfigFile = "./instance/worldBoss/worldBossConfig.json";
		const static string strWorldBoss1thDir = "./instance/worldBoss/panjunjujian/";
		const static string strWorldBoss2thDir = "./instance/worldBoss/yirenruqin/";
		const static string strWorldBossLog = "world_boss_log";

		//string of config
		const static string strMaxHP = "maxHP";

		enum 
		{
			state_not_start,
			state_ing,
			state_kill_not_end,
			state_waiting
		};

		enum
		{
			word_boss_fighter_number
		};

		enum 
		{
			buff_xianfazhiren,
			buff_wenzhawenda,
			buff_yuweiyouzai,
			buff_null
		};

		enum
		{
			damage_zhengchang,//正常
			damage_buan,//不安
			damage_dongyao,//动摇     
			damage_weidi,//畏敌
			damage_sangdan,//丧胆
			damage_hunluan,//混乱
			damage_qijia,//弃甲
			damage_kuisan,//溃散
			damage_yijitui//已击退
		};

	}

#define WORLD_BOSS_OPEN_LEVEL 23
	struct changeItem 
	{
		changeItem();
		changeItem(int startTime, int endTime, int changeNum);
		int startTime_;
		int endTime_;
		int changeNum_;
	};

	struct bossChangeRule 
	{
		int ChangeMax_;
		int ChangeLose_;
		vector<changeItem> changeWin_;
	};

	struct mapData;
	struct mapBossData:
		mapData
	{
		mapBossData();
		~mapBossData();
		int maxHP_;
	};
	typedef boost::shared_ptr<mapBossData> mapBossDataPtr;

	struct worldBossConfig
	{
		worldBossConfig();
		void init();
		double kejiInspireSucessProb_;
		double goldInspireSucessProb_;
		int kejiInspireCost_;
		int attackCD_;
		vector<int> clearAttackCDCostList_;
		bossChangeRule changeRule_;
		vector<bossRewardItem> personalRankRewardList_;
		vector<int> countryRankRewardSilverList_;
		int killRewardSilver_;
		vector <map< int, mapBossDataPtr> > npcList_;
	};

	struct storyCapacityItem
	{
		storyCapacityItem();
		int playerID_;
		string playerName_;
	};

	struct personalDamageItem
	{
		personalDamageItem();
		int playerID_;
		string playerName_;
		int damage_;
	};

	struct countryDamageItem
	{
		countryDamageItem();
		int countryID_;
		int damage_;
	};

	struct reportItem 
	{
		string atkName_;
		int damage_;
		int reportLink_;
	};

	struct bossDynamicData
	{
		bossDynamicData();
		void reset();
		void calBuffState(int &buff, double &rewardRate, int &buffTime);
		int bossID_;
		int remainHP_;//如果血量小于0，要记得设置成0
		int damageState_;
		int beginTime_;
		int killTime_;
		string killTailName_;
	};

	struct bossAnnouce
	{
		bossAnnouce();
		void refreshData();
		void reset();
		bool ba_20_35_;
		bool ba_21_00_;
		bool ba_21_04_55_;
		bool ba_drop_to_70_;
		bool ba_drop_to_50_;
		bool ba_drop_to_30_;
		bool ba_drop_to_20_;
		bool ba_drop_to_10_;
		unsigned nextUpdateTime_;
	};

	struct updateClientManager
	{
		updateClientManager();
		void addPlayer(playerDataPtr player);
		void removePlayer(playerDataPtr player);
		set<playerDataPtr> joinPlayerList_;
		bool isJoinPlayerListClear_;
	};

	class world_boss_system
	{
	public:
		static world_boss_system* const worldBossSys;
		world_boss_system();
		~world_boss_system();
		void initData();
		bool save();
		bool get();
		void worldBossUpdateClientReq(msg_json& m, Json::Value& r);
		void worldBossAttackReq(msg_json& m, Json::Value& r);
		void worldBossRewardReq(msg_json& m, Json::Value& r);
		void worldBossKejiInspireReq(msg_json& m, Json::Value& r);
		void worldBossGoldInspireReq(msg_json& m, Json::Value& r);
		void worldBossClearAttackCdReq(msg_json& m, Json::Value& r);
		void worldBossInnerDealEndReq(msg_json& m, Json::Value& r);
		void worldBossStorageCapacityUpdateReq(msg_json& m, Json::Value& r);
		void worldBossStorageCapacityReq(msg_json& m, Json::Value& r);
		void worldBossCloseUiReq(msg_json& m, Json::Value& r);
		void worldBossInnerAutoAttackReq(msg_json& m, Json::Value& r);
		void worldBossLoopUpdate(boost::system_time& tmp);
		void addBattleBuff(playerDataPtr pdata, battleSide& side);
		int getReportLinkAndIncrease();
		int getReportLinkWithoutIncrease();
		void worldBossAttackCallback(const int battleResult, Battlefield field, Json::Value &report);
		void worldBossEnding(const int battleResult, Battlefield field, playerDataPtr atkData);
		bool getActivityTime(unsigned &start_time, unsigned &end_time);
		string getTotalReportFileName();
		string getPlayerReportFileName(playerDataPtr player);
		bool isBossActValid();
		const bossDynamicData& getLatestBoss();
		void setBossInBattleState(bool st);
		void worldBossOver();
		bool getBossInBattleState();
		void updateInfoSingleClient(playerDataPtr pdata, int updateType = gate_client::world_boss_update_client_resp, bool updateNextBoss = false);
		bool addRobot_;
	protected:
	private:
		void initNpc();
		void updateInfoToClients(bool updateAll = false);
		void updateMsgToClients(bool updateAll, Json::Value& msgJson, const short protocol);
		void updateStorageToClient(playerDataPtr pdata);
		void bossBegin();
		void bossEnd();
		void clearOldData();
		bossDynamicData& getBossData(int curSeason);
		mapBossDataPtr getBossNpcConfig(int curSeason, int bossID);
		int getLatestSeason();
		int getNextSeason();
		int getMaxBossID(int curSeason);
		void divideBlood(bossDynamicData& bossData, bool isInit);
		double state2DamageRate(int st);//通过战斗前状态计算伤害比
		int damageRate2state(int totalHP, int damage);//通过战斗后伤害比更新状态
		int getPersonalRank(playerDataPtr player);
		int getCountryRank(int country);
		void dealEndThings();
		bossRewardItem calRoundDamageReward(bossDynamicData& bossData, mapBossDataPtr bossPtr, playerDataPtr pdata, int damage);
		int calClearAttackCdCost(int times);
		bossRewardItem calPersonalRankReward(playerDataPtr player, int rank);
		bossRewardItem calCountryRankReward(int rank);
		bossRewardItem calKillReward(bool isKill);
		bossRewardItem calTailReward(string name);
		bossRewardItem calTotalRewardExceptDamageByPlayer(playerDataPtr player, bool isKill);
		int calNextBossID(int sea);
		int calGoldInspireCost();
		int calKejiInspireCost(playerDataPtr player);
		countryDamageItem getItemByCountry(int country);
		int calStorageAttack(playerDataPtr pdata);
		bool insertPersonalRankList(vector<personalDamageItem> &plist, personalDamageItem &pinfo);
		bool insertCountryRankList(vector<countryDamageItem> &plist, countryDamageItem &pinfo);
		
		vector<storyCapacityItem> storageCapacityList_;
		vector<personalDamageItem> personalDamageRank_;
		vector<countryDamageItem> countryDamageRank_;//跟玩家一样有变动就进行排序
		vector<reportItem> totalReportList_;
		vector<bossDynamicData> bossData_;
		worldBossConfig bossConfig_;
		battleSidePtr bossSide_;
		updateClientManager updateClientMgr_;
		int curSeason_;
		bool isBossInBattle_;
		boost::mutex queueMutex_;
		int totalReportLink_;

		queue<msg_json> bossMsgQueue_;
		int activityState_;
		int duration_;
		boost::system_time tick_;
		bossAnnouce annouce_;

		static mapBossDataPtr create()
		{
			void* p = GGNew(sizeof(mapBossData));
			return mapBossDataPtr(new(p)mapBossData(), destory);
		}

		static void destory(mapBossData* point)
		{
			point->~mapBossData();
			GGDelete(point);
		}

	};

}

