#include "player_science.h"
#include "science_system.h" 
#include "file_system.h"
#include "response_def.h"
#include "helper.h"
#include "task_system.h"

namespace gg{

	const static string science_config_path = "./instance/science";

	ScienceManager* const ScienceManager::scienceMgr = new ScienceManager();

	ScienceManager::ScienceManager(){

	}

	/*加载配置文件*/
	void ScienceManager::initData(){
		na::file_system::json_value_vec json_vect;
		na::file_system::load_jsonfiles_from_dir(science_config_path, json_vect);
		for (unsigned i = 0; i < json_vect.size(); ++i){
			ConfigEntity entity;
			Json::Value& item = json_vect[i];
			entity.id = item[science_config_id_key].asInt();
			entity.comeOutLevel = item[science_config_comeOutLevel_key].asInt();
			entity.maxLevel = item[science_config_maxLevel_key].asInt();
			entity.updateLevel = item[science_config_updateLevel_key].asInt();
			entity.zhenxing = item[science_config_zhenxing_key].asInt();

			for (unsigned k = 0; k < item[science_config_cost_key].size(); ++k){
				int cost_value = item[science_config_cost_key][k].asInt();
				entity.cost_config.push_back(cost_value);
			}

			for (unsigned k = 0; k < item[science_config_effect_key].size(); ++k){
				vector<double> effect_item_config;
				for (unsigned z = 0; z < item[science_config_effect_key][k].size(); ++z){
					effect_item_config.push_back(item[science_config_effect_key][k][z].asFloat());
				}
				entity.effect.push_back(effect_item_config);
			}
			config_map[entity.id] = entity;
			if (entity.zhenxing > 0 && entity.zhenxing < 100)
				map_map[entity.zhenxing] = entity.id;
		}
	}

	void ScienceManager::reqUpdate(msg_json& m, Json::Value& r){//客户端科技基础更新 
		AsyncGetPlayerData(m);
		bool auto_active = false;
		for (scienceConfigMap::iterator it = config_map.begin(); it != config_map.end(); ++it){

			if (d->Science.science_maps.end() == d->Science.science_maps.find(it->second.id) && d->Base.getLevel() >= it->second.comeOutLevel){//  auto active science
				playerScienceItem science_item;
				science_item.id = it->second.id;
				science_item.lv = 0;
				d->Science.add(science_item);
				auto_active = true;
			}
		}
		if (auto_active) helper_mgr.insertSaveAndUpdate(&d->Science);

		task_sys.updateBranchTaskInfo(d, _task_science_num);
		d->Science.update();
	}

	void ScienceManager::reqUpgrade(msg_json& m, Json::Value& r){//升级科技
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int keji_id = js_msg[0u].asInt();
		if (d->Base.getLevel() < 7)Return(r, 1);//角色等级需要超过7级


		//const playerBuild& building = d->Builds.getBuildOther(building_institute);
		//if(playerBuild::NullValue == building )Return(r, 2);//科技中心还没激活


		scienceConfigMap::iterator it = config_map.find(keji_id);
		if (it == config_map.end())Return(r, -1);//配置文件中没有找到该科技的配置

		ConfigEntity& config = it->second;
		int playerLevel = d->Base.getLevel();
		//判断该科技是否已经激活了


		if (config.comeOutLevel > playerLevel)Return(r, 3);//科技还没激活


		playerScience::scienceMaps::iterator science_it = d->Science.science_maps.find(keji_id);
		if (science_it == d->Science.science_maps.end()){
			playerScienceItem science_item;
			science_item.id = keji_id;
			science_item.lv = 0;
			d->Science.add(science_item);
		}



		science_it = d->Science.science_maps.find(keji_id);


		//判断是否达到满级
		if (static_cast<unsigned>(science_it->second.lv + 1) > config.cost_config.size())Return(r, 5);//满级

		//可升级的级数 = int(主角当前等级/升级间隔)
		if (science_it->second.lv + 1 > static_cast<int>(playerLevel / config.updateLevel))Return(r, 4);


		//判断科技点是否足够
		int cost_keji = config.cost_config[science_it->second.lv];
		if (cost_keji > d->Base.getKeJi())Return(r, 6);

		//扣除科技点升级
		d->Base.alterKeJi(-cost_keji);

		science_it->second.lv += 1;
		task_sys.updateBranchTaskInfo(d, _task_science_lv);
		task_sys.updateBranchTaskInfo(d, _task_science_num);
		helper_mgr.insertSaveAndUpdate(&d->Science);
		bool is_attri = config.isAttri();
		if (is_attri)
		{
			//这里要重新计算全部武将
			d->Pilots.recalBattleValue();
		}
		r[msgStr][0u] = 0;
	}



	//void ScienceManager::playerUpdateEventTrigger(playerDataPtr player){
	//const playerBuild& build = player->Builds.getBuildOther(building_institute);
	//if(playerBuild::NullValue == build && player->Base.getLevel() >= 10){//激活科技中心
	//	player->Builds.activeBuilding(building_institute);
	//}
	//}

	int ScienceManager::getFormatMaping(const int fID)
	{
		if (map_map.find(fID) == map_map.end())return -1;
		return map_map[fID];
	}

	scienceConfigMap& ScienceManager::getConfig(){
		return this->config_map;
	}

}
