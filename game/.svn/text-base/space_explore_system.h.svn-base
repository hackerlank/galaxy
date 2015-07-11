#pragma once
#include "json/json.h"
#include "msg_base.h"
#include "player_space_explore.h"

#define space_explore_sys (*gg::space_explore_system::spaceExploreSys)

using namespace na::msg;
namespace gg
{
#define RANGE 10000
#define PRECISION 0.000001f

	namespace space_explore_def
	{
		const static string strDbPlayerSpaceExplore = "gl.player_space_explore";
		const static string strDbSpaceExplore = "gl.space_explore";
		const static string strKey = "key";
		const static string strFileConfig = "./instance/spaceExplore/spaceExplore.json";
		const static string strTodayOneHuntUsedTimes = "tohut";
		const static string strTotalTenHuntsUsedTimes = "tthut";
		const static string strNextFreeOneHuntTime = "nfoht";
		const static string strNextFreeTenHuntsTime = "nftht";
		const static string strNextDailyFreshTime = "ndft";
		const static string strIsRedPoint = "irp";
		const static string strIsFirstOne = "ifo";
		const static string strIsFirstTen = "ift";
		const static string strIsOpenSpaceExplore = "iose";
		const static string strRecordList = "rl";
		const static string strDbsSpaceExplore = "log_space_explore";

		enum 
		{
			log_one_explore,
			log_ten_explore
		};
	}

	struct rewardResult
	{
		rewardResult();
		int rewardIndex_;
		int rewardTimes_;
	};

	struct spaceRecord
	{
		int playerID_;
		rewardResult reward_;
	};

	struct spaceExploreConfig
	{
		spaceExploreConfig();
		int silverBase_;
		int oneHuntGoldCost_;
		int tenHuntsGoldCost_;
		int twoTimesCritical_;
		int fiveTimesCritical_;
		int silverProbability_;
		int firstTenExploreEquipIndex_;
		bool isSystemOpen_;
		vector<int> equipIDList_;
		vector<int> equipProbability_;
	};

	class space_explore_system
	{
	public:
		static space_explore_system* const spaceExploreSys;
		space_explore_system();
		~space_explore_system();
		void spaceExplorePlayerInfoReq(msg_json& m, Json::Value& r);
		void spaceExploreOneHuntReq(msg_json& m, Json::Value& r);
		void spaceExploreTenHuntReq(msg_json& m, Json::Value& r);
		void spaceExploreUpdateReq(msg_json& m, Json::Value& r);
		void initData();
		const spaceExploreConfig& getConfig(){ return config_; };
		void insertRecord(spaceRecord sr);
		void setSystemState();
		bool save();
		bool get();
		void updateRecord(playerDataPtr pdata);
	protected:
	private:
		spaceExploreConfig config_;
		vector<spaceRecord> recordList_;
	};
}

