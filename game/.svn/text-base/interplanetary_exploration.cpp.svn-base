#include "interplanetary_exploration.h"
#include "action_def.h"
#include "action_instance.hpp"
#include "gm_tools.h"
namespace gg{

	interplanetary_exploration::interplanetary_exploration(void)
	{
		m_Probability.clear();
		m_ProbabilityCopy.clear();
		m_ExploreRewardsMap.clear();
		m_Growth=0.1;		//每次未抽中高级奖励的增长值
		m_GrowthNum=0;		//现有成长数
		m_GrowthMaxNum=5;		//最大增长次数

		m_HQRange=0;		//高品质奖励的范围
		m_MaterialsConsumptionNum=0;	//一次探险的物资消耗数
		m_ExploreName="";		//探险名称
	}

	interplanetary_exploration::interplanetary_exploration(string name)
	{
		m_Probability.clear();
		m_ProbabilityCopy.clear();
		m_ExploreRewardsMap.clear();
		m_Growth=0.1;		//每次未抽中高级奖励的增长值
		m_GrowthNum=0;		//现有成长数
		m_GrowthMaxNum=5;		//最大增长次数

		m_HQRange=0;		//高品质奖励的范围
		m_MaterialsConsumptionNum=0;	//一次探险的物资消耗数
		m_ExploreName=name;		//探险名称
	}

	interplanetary_exploration::interplanetary_exploration(string name,int HQRange,int materialsConsumptionNum)
	{
		m_Probability.clear();
		m_ProbabilityCopy.clear();
		m_ExploreRewardsMap.clear();
		m_Growth=0.1;		//每次未抽中高级奖励的增长值
		m_GrowthNum=0;		//现有成长数
		m_GrowthMaxNum=5;		//最大增长次数

		m_HQRange=HQRange;		//高品质奖励的范围
		m_MaterialsConsumptionNum=materialsConsumptionNum;	//一次探险的物资消耗数
		m_ExploreName=name;		//探险名称
	}

	interplanetary_exploration::~interplanetary_exploration(void)
	{
		
	}

	
	int interplanetary_exploration::getExploreGoods()
	{
		int randNum = commom_sys.randomBetween(0, 1000);
		int total = 0;
		for (int i = 0; i<(int)m_Probability.size(); i++)
		{
			if(randNum-(total+m_Probability[i]*1000)<0)
				return i;
			total+=int(m_Probability[i]*1000);
		}
		if (m_Probability.size() != 0&&!m_Probability.empty())
			return m_Probability.size()-1;
		return -1;
	}

	ExploreRewards& interplanetary_exploration::startExplore()
	{
		int awardID = getExploreGoods();
		if(awardID==-1)
			return NullExploreRewards;
		//抽到高级物品
		if(awardID<m_HQRange){
			m_GrowthNum = 0;
			//概率重置
			this->recoverProbability();
		}else{
			//达到最大增长次数
			if(m_GrowthNum==m_GrowthMaxNum)
				return m_ExploreRewardsMap[awardID];
			//概率增长
			double total = 0;
			int length = m_Probability.size();
			double d1 = 0,d2 = 0;
			for (unsigned i = 0; i < m_Probability.size(); i++){
				if ((int)i < m_HQRange)
					d1 += m_Probability[i];
			}
			d2 = 1 - d1;
			double p = 0;
			for (unsigned i = 0;i<m_Probability.size();i++)
			{
				if ((int)i < m_HQRange){
					p = m_Growth*(m_Probability[i] / d1);
					m_Probability[i] += p;
				}
				else{
					p = m_Growth*(m_Probability[i] / d2);
					m_Probability[i] -= p;
				}
			}
			m_GrowthNum++;
		}
		ExploreRewardsMap::iterator it = m_ExploreRewardsMap.find(awardID);
		if ( it == m_ExploreRewardsMap.end())
			return NullExploreRewards;
		return it->second;
	}

	void interplanetary_exploration::addPackage(Json::Value& msg)
	{
		Json::Value& json_array = msg;
		int i = 0;
		for(ExploreRewardsMap::iterator it = m_ExploreRewardsMap.begin();it!=m_ExploreRewardsMap.end();it++,i++){
			json_array[i][0] = it->first;
			for (unsigned k = 0;k<it->second.rewardResource.size();k++)
			{
				int id=0, iId=0, num=0;
				Json::Value& v = it->second.rewardResource[k];
				//cout << "#######" << v.toStyledString() << endl;
				json_array[i][1] = id = v[ACTION::strActionID].asInt();
				if (id == action_add_item){
					iId = v[ACTION::addItemIDStr].asInt();
					num = v[ACTION::addNumStr].asInt();
				} 
				else if (id == action_add_pilot){
					for (unsigned n = 0; n < v[ACTION::strPilotActiveList].size();n++)
						iId = v[ACTION::strPilotActiveList][n].asInt();
					num = 1;
				}
				else if (id == action_add_secretary)
				{
					for (unsigned n = 0; n < v[ACTION::strSecretaryList].size(); n++)
						iId = v[ACTION::strSecretaryList][n].asInt();
					num = 1;
				}
				else{
					iId = 0;
					num = v[ACTION::strValue].asInt();
				}
				json_array[i][2] = iId;
				json_array[i][3]= num;
			}
		}
	}

	void interplanetary_exploration::addPackage(Json::Value& msg,const ExploreRewards& exploreRewards)
	{
		Json::Value& json_array = msg;
		json_array = Json::arrayValue;
		json_array[0] = exploreRewards.ID;
		const Json::Value& v = exploreRewards.rewardResource;
		int id = 0, iId = 0, num = 0;
		for (unsigned i = 0;i<v.size();i++)
		{
			id = v[i][ACTION::strActionID].asInt();
			if (id == action_add_item){
				iId = v[i][ACTION::addItemIDStr].asInt();
				num = v[i][ACTION::addNumStr].asInt();
			}
			else if (id == action_add_pilot){
				for (unsigned n = 0; n < v[i][ACTION::strPilotActiveList].size(); n++)
					iId = v[i][ACTION::strPilotActiveList][n].asInt();
				num = 1;
			}
			else if (id == action_add_secretary)
			{
				for (unsigned n = 0; n < v[i][ACTION::strSecretaryList].size(); n++)
					iId = v[i][ACTION::strSecretaryList][n].asInt();
				num = 1;
			}
			else{
				iId = 0;
				num = v[i][ACTION::strValue].asInt();
			}
			json_array[1] = id;
			json_array[2] = iId;
			json_array[3] = num;
		}
	}


	void interplanetary_exploration::addPackageBack(Json::Value& msg)
	{
		Json::Value& json_array = msg;
		int i = 0;
		for(ExploreRewardsMap::iterator it = m_ExploreRewardsMap.begin();it!=m_ExploreRewardsMap.end();it++,i++){
			json_array[i]["ID"] = it->first;
			Json::Value& reArray  = json_array[i]["re"];
			reArray = gm_tools_mgr.complex2simple(it->second.rewardResource);
			/*for (unsigned k = 0;k<it->second.rewardResource.size();k++)
			{
				packageResource(reArray[k],it->second.rewardResource[k]);
			}*/
			json_array[i]["pr"] = m_Probability[i];
		}
	}


	double interplanetary_exploration::getGrowth()
	{
		return m_Growth;
	}


	int interplanetary_exploration::getGrowthMaxNum()
	{
		return m_GrowthMaxNum;
	}

	int interplanetary_exploration::getHQRange()
	{
		return m_HQRange;
	}

	int interplanetary_exploration::getMaterialsConsumptionNum()
	{
		return m_MaterialsConsumptionNum;
	}

	void interplanetary_exploration::setExploreProbability(vector<double>& probability)
	{
		m_ProbabilityCopy=m_Probability = probability;
		return ;
	}

	void interplanetary_exploration::setExploreRewards(ExploreRewardsMap& exploreRewards)
	{
		m_ExploreRewardsMap = exploreRewards;
		return ;
	}

	void interplanetary_exploration::setGrowth(double num)
	{
		m_Growth = num;
	}

	void interplanetary_exploration::setGrowthMaxNum(int num)
	{
		m_GrowthMaxNum = num;
	}


	void interplanetary_exploration::setHQRange(int num)
	{
		m_HQRange = num;
	}

	void interplanetary_exploration::setMaterialsConsumptionNum(int num)
	{
		m_MaterialsConsumptionNum = num;
	}

	std::string interplanetary_exploration::getExploreName()
	{
		return m_ExploreName;
	}

	void interplanetary_exploration::setExploreName(string name)
	{
		m_ExploreName = name;
	}

	void interplanetary_exploration::recoverProbability()
	{
		m_Probability = m_ProbabilityCopy;
	}

	void interplanetary_exploration::clearExplorationMsg()
	{
		m_Probability.clear();
		m_ProbabilityCopy.clear();	//保留初始概率
		m_ExploreRewardsMap.clear();
	}

}
