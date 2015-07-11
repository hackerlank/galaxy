#pragma once
#include "json/json.h"
#include "playerManager.h"
#include <boost/thread/detail/singleton.hpp>

using namespace na::msg;
#define science_sys (*gg::ScienceManager::scienceMgr) 
namespace gg
{

	const static string	science_config_id_key			= "id";
	const static string	science_config_comeOutLevel_key = "comeOutLevel";
	const static string	science_config_maxLevel_key		= "maxLevel";
	const static string science_config_updateLevel_key  = "updateLevel";
	const static string science_config_cost_key			= "cost";
	const static string science_config_effect_key		= "effect";
    const static string science_config_zhenxing_key     = "zhenxing";	 
	 
	 
	 class ScienceManager{
	 public:

		 ScienceManager();

		 static ScienceManager* const scienceMgr;
		 void initData();   
		 void reqUpdate(msg_json& m, Json::Value& r);//�ͻ��˸�������
		 void reqUpgrade(msg_json& m, Json::Value& r);//�ͻ����ύ�����Ƽ�����
		 scienceConfigMap& getConfig();
		 int getFormatMaping(const int fID);
	 private:
		 std::map<int, int> map_map;
		 scienceConfigMap config_map;
		    
	 };

}
