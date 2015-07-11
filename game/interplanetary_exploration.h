#pragma once
#include "block.h"
#include <boost/unordered_map.hpp>
namespace gg{
	
	struct ExploreRewards{
		int ID;
	    Json::Value rewardResource;
		ExploreRewards(){
			ID = -1;
			rewardResource = Json::nullValue;
		}
		bool operator == (const ExploreRewards &reward) const{
			return (ID==reward.ID);
		}
	};
	static ExploreRewards NullExploreRewards;
	typedef  boost::unordered_map<int,ExploreRewards>  ExploreRewardsMap;
	class interplanetary_exploration
	{
	public:
		interplanetary_exploration(void);
		interplanetary_exploration(string name);
		interplanetary_exploration(string name,int HQRange,int materialsConsumptionNum);
		~interplanetary_exploration(void);
		int getExploreGoods();
		ExploreRewards& startExplore();    //开始探险
		double getGrowth();
		int getGrowthMaxNum();
		int getHQRange();
		int getMaterialsConsumptionNum();
		string getExploreName();
		void setGrowth(double num);
		void setGrowthMaxNum(int num);
		void setHQRange(int num);
		void setMaterialsConsumptionNum(int num);
		void setExploreName(string name);
		void setExploreProbability(vector<double>& probability);
		void setExploreRewards(ExploreRewardsMap& exploreRewards);
		void addPackage(Json::Value& msg);
		void addPackage(Json::Value& msg,const ExploreRewards& exploreRewards);
		void addPackageBack(Json::Value& msg);	//打包给后台的消息
		void clearExplorationMsg();
	private:
		//恢复初始概率
		void recoverProbability();
		//void packageResource(Json::Value& j,resource& re);
		vector<double> m_Probability;
		vector<double> m_ProbabilityCopy;	//保留初始概率
		ExploreRewardsMap m_ExploreRewardsMap;
		double m_Growth;		//每次未抽中高级奖励的增长值
		int m_GrowthNum;		//现有成长数
		int m_GrowthMaxNum;		//最大增长次数
		int m_HQRange;		//高品质奖励的范围
		int m_MaterialsConsumptionNum;	//一次探险的物资消耗数
		string m_ExploreName;		//探险名称
	};

}
