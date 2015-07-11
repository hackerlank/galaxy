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
		ExploreRewards& startExplore();    //��ʼ̽��
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
		void addPackageBack(Json::Value& msg);	//�������̨����Ϣ
		void clearExplorationMsg();
	private:
		//�ָ���ʼ����
		void recoverProbability();
		//void packageResource(Json::Value& j,resource& re);
		vector<double> m_Probability;
		vector<double> m_ProbabilityCopy;	//������ʼ����
		ExploreRewardsMap m_ExploreRewardsMap;
		double m_Growth;		//ÿ��δ���и߼�����������ֵ
		int m_GrowthNum;		//���гɳ���
		int m_GrowthMaxNum;		//�����������
		int m_HQRange;		//��Ʒ�ʽ����ķ�Χ
		int m_MaterialsConsumptionNum;	//һ��̽�յ�����������
		string m_ExploreName;		//̽������
	};

}
