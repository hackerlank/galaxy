#pragma once
#include <map>
#include <list>
#include "msg_base.h"
#include "activity_base.h"
#include "interplanetary_exploration.h"

#define  explore_sys (*gg::explore_manager::playerExploreMgr)
namespace gg{

	class explore_manager
		:public activity_base
	{
	public:
		explore_manager(void);
		~explore_manager(void);
		static explore_manager* const playerExploreMgr;

		void initData();
		void setExploreRewards(msg_json& m, Json::Value& r);
	    void queryExplore(msg_json& m,Json::Value& r);
		void updateExploreRewardsContent(msg_json& m, Json::Value& c);
		void updateLottery_fin(msg_json& m,Json::Value& r);
		void updateLottery_tag(msg_json& m, Json::Value& r);
		void femaleInsectNest(msg_json& m,Json::Value& r);
		void theAncientGodRemains(msg_json& m,Json::Value& r);
		int getMaterialsMax();
		void refreshPlayerData(playerDataPtr player);
	private:
		void updateBaseInfo(playerDataPtr d);
		void updateBaseInfo();
		void updateLottery(playerDataPtr d,int type);
		void getExploreContent();
		int setExploreRewards(Json::Value& msg);
		bool saveExploreContent(Json::Value& setJson);
	private:
		interplanetary_exploration female_insect_nest;		//母虫巢穴
		interplanetary_exploration the_ancient_god_remains;		//古神遗迹
		unsigned startTime;
		unsigned endTime;
		int materialsMax;				//物质上限

	private:
		int insertLotteryInfo_fin(Json::Value& content);
		int insertLotteryInfo_tag(Json::Value& content);
		void packageLotteryInfo_fin(Json::Value& msg);
		void packageLotteryInfo_tag(Json::Value& msg);
		void getLotteryInfo();
		bool saveLotteryInfo_fin();
		bool saveLotteryInfo_tag();
	private:
		list<Json::Value> finLotteryList;
		list<Json::Value> tagLotteryList;
	};

}
