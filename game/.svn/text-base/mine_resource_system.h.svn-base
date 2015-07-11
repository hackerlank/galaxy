#pragma once
#include "json/json.h"
#include "msg_base.h"
#include "battle_member.h"
#include "block.h"
#include "war_story.h"

#define mine_res_sys (*gg::mine_resource_system::mineResSys)
//LOG
using namespace na::msg;
namespace gg
{
#define PAGE_SIZE 9
#define RANGE 10000
#define PRECISION 0.000001f
#define MAX_REPORT_ID 15
#define MINE_TYPE 6
#define NPC_NUMBER 5
#define START_NPC_ID 1101

	enum
	{
		LEVEL_20_30_LAYER = 0,
		LEVEL_31_40_LAYER,
		LEVEL_41_60_LAYER,
		LEVEL_61_80_LAYER,
		LEVEL_81_100_LAYER,
		LEVEL_101_120_LAYER
	};

	enum
	{
		scan_null,
		scan_general,
		scan_advance
	};

	namespace mine_res
	{
		const static string strMineResConfigPath = "./instance/mineResource/mineResource.json";
		const static string strMineResNpcDir = "./instance/mineResource/npc/";
		const static string strConfigBaseGold = "baseGold";
		const static string strConfigBaseWeiwang = "baseWeiwang";
		const static string strConfigGeneralScanRate = "generalScanRate";
		const static string strConfigAdvanceScanRate = "advanceScanRate";
		const static string strConfigResCoeff = "resourceCoeff";

		const static string strKey = "key";
		const static string strStartMineId = "smid";
		const static string strEndMineId = "emid";
		const static string strMineIDList = "midls";
		const static string strLayer = "ly";
		const static string strMineId = "mid";
		const static string strPageId = "pid";
		const static string strMaxPageId = "mpid";
		const static string strMineList = "mls";
		const static string strNpcList = "nl";
		const static string strMineMyInfo = "my";
		const static string strActBonus = "ab";

		const static string strReportList = "rls";
		const static string strReportAtkName = "atkn";
		const static string strReportDefName = "defn";
		const static string strReportAtkID = "atki";
		const static string strReportDefID = "defi";
		const static string strIsPlayer = "isp";
		const static string strReportAtkLink = "atkl";
		const static string strReportDefLink = "defl";
		const static string strReportResult = "res";
		const static string strReportTime = "rt";

		const static string strDBMineCommon = "gl.mine_res_common";
		const static string strDBMineID2Pid = "gl.mine_res_mid_pid";
		const static string strDBMinePlayerInfo = "gl.mine_res_player_info";

		const static string strQuality = "ql";
		const static string strBeRobTimes = "brt";
		const static string strTotalBeRobSilver = "tbrs";
		const static string strTotalBeRobWeiwang = "tbrw";
		const static string strRemainMineTimes = "rmt";
		const static string strRemainRobTimes = "rrt";
		const static string strStartMineTime_ = "smt";
		const static string strActExtraRate = "aer";
		const static string strScienceMineExtraRate = "smer";
		const static string strMineCD = "mcd";
		const static string strHasSeekMineCDHelp = "smcdh";
		const static string strHasSeekMineHelp = "smh";
		const static string strBeHelpMineTimes = "bhmt";
		const static string strRobCD = "rcd";
		const static string strMaxReportId = "mrid";
		const static string strNextUpdateTime = "nut";
		const static string strCurMineQuality_ = "cq";
		const static string strGainTime = "gt";
		const static string strGeneralScanTimes = "gst";
		const static string strAdvanceScanTimes = "ast";
		const static string strGeneralScanCost = "gsc";
		const static string strAdvanceScanCost = "asc";
		const static string strMineRewardSilver = "mrs";
		const static string strMineRewardWeiwang = "mrw";
		const static string strMineRewardExtraSilver = "mres";
		const static string strMineRewardExtraWeiwang = "mrew";
		const static string strReportMineResDir = "./report/engross/mineRes/";

		const static string strTotalSilver = "ts";
		const static string strTotalWeiwang = "tw";
		const static string strLoseSilver = "ls";
		const static string strLoseWeiwang = "lw";
		const static string strGainSilver = "gs";
		const static string strGainWeiwang = "gw";
		const static string strHelpSilver = "hs";
		const static string strHelpWeiwang = "hw";

		const static string mysqlLogMineRes = "log_mine_resource";
	}

	enum
	{
		mine_res_log_mine,//开始采矿，已经去掉，保留
		mine_res_log_mine_result,
		mine_res_log_rob,
		mine_res_log_general_scan,
		mine_res_log_advance_scan,
		mine_res_log_clear_fight_cd
	};

	struct mineReport
	{
		mineReport();
		string atkName;
		string defName;
		int atkID;
		int defID;
		int result;
		int battleTime;
		string atkReportLink;
		string defReportLink;
		bool isPlayer;
		void package(Json::Value& pack);
	};

	struct mineItem
	{
		mineItem();
		int player_id_;
		int playerLevel_;
//		string playerGuild_;
		int mineID_;
		int quality_;
		int beRobTimes_;
		int totalBeRobSilver_;
		int totalBeRobWeiwang_;
		int startMineTime_;
		double actExtraRate_;
		double ScienceMineExtraRate_;
		int remainMineTimes_;
		int remainRobTimes_;
		int mineCD_;
		bool hasSeekMineCDHelp_;
		bool hasSeekMineHelp_;
		int beHelpMineTimes_;
		int robCD_;
		int maxReportId_;
		int nextUpdateTime_;
		int generalScanTimes_;
		int advanceScanTimes_;
		bool ising_;
		vector<mineReport> reportList_;
		void packageLargeBaseInfo(playerDataPtr pdata, Json::Value &pack, bool isScienceMineInflu, bool isScienceRobInflu, double mineCoeff = 0.0f, double robCoeff = 0.0f);
		void packageLargeNpcBaseInfo(playerDataPtr pdata, mapDataPtr& mData, Json::Value &pack, bool isScienceMineInflu, bool isScienceRobInflu, double robCoeff = 0.0f);
		void packageLittleBaseInfo(playerDataPtr pdata, Json::Value &pack);
		void packageReport(Json::Value &pack);
		bool isGainTime();
		bool isMining();
		void gain(playerDataPtr pdata, int cur_time = 0);
		int beginToMine(int layer, playerDataPtr pdata);
		int scanMine(int scan_option);
		string getReportName(playerDataPtr pdata);
		int getMineRewardSilver(playerDataPtr pdata, int level, bool isAddBase, bool isActInflu, bool isScienceMineInflu, bool isScienceRobInflu, double mineCoeff = 0.0f, double robCoeff = 0.0f);
		int getMineRewardWeiwang(playerDataPtr pdata, int level, bool isAddBase, bool isActInflu, bool isScienceMineInflu, bool isScienceRobInflu, double mineCoeff = 0.0f, double robCoeff = 0.0f);
		int getGeneralScanCost();
		int getAdvanceScanCost();
		void insertReport(mineReport& mReport);
		void reset();
		bool dailyRefresh();
		//清除cd的求助接口，已经不用，保留
		bool getHasSeekMineCDHelp();
		void setHasSeekMineCDHelp(bool bVal);
		//挖矿的求助接口
		bool getHasSeekMineHelp();
		void setHasSeekMineHelp(bool bVal);
		int getBeHelpMineTimes();
		void alterBeHelpMineTimes(int num);
	};

	struct mineScanItem
	{
		int generalScanRate;
		int advanceScanRate;
		double resCoeff;
	};

	struct mineConfig
	{
		vector <mineScanItem> mineScan;
		int baseSilver;
		int baseWeiwang;
		int generalScanSilverCost;
		int advanceScanGoldCost;
	};

	struct mineAreaKey
	{
		mineAreaKey() : mineLayer(-1), mineId(-1){};
		mineAreaKey(int layer, int id) : mineLayer(layer), mineId(id){};
		int mineLayer;
		int mineId;
		bool operator< (const mineAreaKey& k) const
		{
			if(mineLayer != k.mineLayer)return mineLayer < k.mineLayer;
			return mineId < k.mineId;
		}
	};

	struct layerItem 
	{
		layerItem();
		vector<int> mineIDList_;
// 		int startMineId;
// 		int endMineId;
	};

	struct mineAnnouceBroadCast
	{
		mineAnnouceBroadCast();
		bool br_12_30_;
		bool br_20_00_;

		unsigned nextUpdateTime_;
		void reset();
		void refresh();
	};

	class mine_resource_system
	{
	public:
		static mine_resource_system* const mineResSys;
		mine_resource_system();
		~mine_resource_system();
		void mineResUpdateReq(msg_json& m, Json::Value& r);
		void mineResTurnPageReq(msg_json& m, Json::Value& r);
		void mineResFightRecordReq(msg_json& m, Json::Value& r);
		void mineResClearFightCdReq(msg_json& m, Json::Value& r);
		void mineResSingleUpdateReq(msg_json& m, Json::Value& r);
		void mineResFightReq(msg_json& m, Json::Value& r);
		void mineResFreeScanReq(msg_json& m, Json::Value& r);
		void mineResGeneralScanReq(msg_json& m, Json::Value& r);
		void mineResAdvanceScanReq(msg_json& m, Json::Value& r);
		void mineResMineReq(msg_json& m, Json::Value& r);
		void initData();
		void mineResFightCallback(const int battleResult, Battlefield field, Json::Value &report);
		void mineResFightEnding(const int battleResult, Battlefield field, playerDataPtr atkData);
		bool levelUp(playerDataPtr pdata);
		mineConfig getConfig(){ return mineConfig_; };
		double getRobExtraRate(playerDataPtr pdata);
		double getMineExtraRate(playerDataPtr pdata);
		bool clearPlayerMineCD(playerDataPtr player, unsigned s, bool& NoData);
		bool clearPlayerMineCD(playerDataPtr player, unsigned s);
		bool savePlayerMineInfo(int playerId, mineItem& mItem);
		bool getPlayerMineInfo(playerDataPtr pdata, mineItem& mItem);
		int getNewMineId(int layer);
		bool getRewardTime(unsigned &start_time, unsigned &end_time);
		void mineResLoopUpdate(boost::system_time& tmp);
	protected:
	private:
		bool getPlayerMineInfo(int player_id, mineItem& mItem);
		void initMineList();
		void initPlayerMineInfo();
		void initMID2PID();
		void initReportPath();
		void initConfig();
		void initNpc();
		bool insertMineID(int layer, int mineid);
		bool removeMineID(int layer, int mineid);
		bool saveMineList(int layer);
		bool insertMineIDpID(int layer, int mineId, int player_id);
		void removeMineIDpID(int layer, int mineId);
//		void getMineItemsByPage(int layer, int pageId, vector<mineItem>& mineList, vector<int>& npcList);
		void getMineItemsByPage(int layer, int pageId, vector<mineItem>& mineList, vector<int>& npcList);
		int getCurrentPage(int layer, int mineId);
		int getMaxPage(int layer);
		int mineId2vecIndx(int layer, int mineId);
		int level2layer(int level);
		int npcid2layer(int npcid);
		void createMineItem(playerDataPtr pdata, mineItem& mItem);
		void createMineNpcItem(mapDataPtr mData, mineItem& mItem, playerDataPtr pdata);
		bool refreshMineListBegin(int layer);
		bool refreshMineListEnd(int layer);
		bool finishSingleMine(playerDataPtr pdata);
		int getPlayerIdByLMMap(int layer, int mineID);
		mineConfig mineConfig_;
		map<int, layerItem> layerMap_;//layer-beginendid
		map <mineAreaKey, int> mineIDpIDmap_;//layer-mineid-玩家id
		map <int, mineItem> playerMineInfoMap_;//玩家id-玩家信息
		map <int, vector<int> > refreshList_;
		map< int, mapDataPtr> npcMap_;
		boost::system_time tick_;
		int duration_;
		mineAnnouceBroadCast br_;

		static mapDataPtr create()
		{
			void* p = GGNew(sizeof(mapData));
			return mapDataPtr(new(p) mapData(), destory);
		}

		static void destory(mapData* point)
		{
			point->~mapData();
			GGDelete(point);
		}

	};

#define AsyncRefreshMine(MSGJSON, LAYER) \
	bool isfind = refreshMineListBegin(LAYER);\
	if (isfind)\
	{\
		if (MSGJSON._post_times < 2)\
		{\
			player_mgr.postMessage(MSGJSON);\
			return;\
		}\
		else\
		{\
			refreshList_[LAYER].clear();\
			Return(r, -1);\
		}\
	}\
	refreshMineListEnd(LAYER);\

}

