#pragma once
#include "block.h"
//#include "world_boss_system.h"

namespace gg
{
	namespace world_boss_def
	{
		const static string strTotalDamage = "tdm";
		const static string strDamageReward = "drw";
		const static string strClearAttackCDTimes = "cat";
		const static string strLatestBossID = "lbid";
		const static string strLatestBossBeginTime = "lbbt";
		const static string strHasSeekBossHelp = "hsbh";
		const static string strStoryCapacityTime = "sct";
		const static string strnextStorageCapacityUpdateTime = "nst";
	}

	class world_boss_system;	
	struct myReportItem
	{
		myReportItem();
		int damage_;
		int reportLink_;
	};

	struct bossRewardItem 
	{
		bossRewardItem();
		int rewardSilver_;
		int rewardCrystal_;
		Json::Value toJson();
		void fromBson(mongo::BSONElement ele);
		mongo::BSONArray toBsonArr();
	};

	class playerWorldBoss : public Block
	{
	public:
		friend class world_boss_system;
		playerWorldBoss(playerData& own);
		void autoUpdate();
		bool get();
		bool save();
		void package(Json::Value& pack);
		bool getHasSeekBossHelp();
		void setHasSeekBossHelp(bool bVal);
		int getBeHelpBossTimes();
		void alterBeHelpBossTimes(int num);
	private:
		void refreshData();
		void reset();
		int totalDamage_;//��personalDamageRank_ͬ��
		bossRewardItem damageReward_;
		int battleCD_;
		int clearAttackCDTimes_;
		int normalInspireLayer_;
		int guildInspireLayer_;
		int curReportLink_;
		int latestBossID_;//����μӹ���bossս
		int latestBossBeginTime_;//���ÿ�βμ�boss��ʱ�䣬����ˢ�����������
		bool hasSeekBossHelp_;
		int storyCapacityTime_;
		int nextStorageCapacityUpdateTime_;
		int attackTimes_;

		vector<struct myReportItem> myReportList_;
	};
}

