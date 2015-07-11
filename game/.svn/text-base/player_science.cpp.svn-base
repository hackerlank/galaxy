#include "mongoDB.h"
#include "player_science.h"
#include "science_system.h"
#include "playerManager.h"
#include "task_system.h"

using namespace mongo;

namespace gg
{
	void playerScience::update(){
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		Json::Value& tech_list = msg[msgStr][1u][player_science_data_field];
		tech_list = Json::arrayValue;
		scienceConfigMap& config_map = science_sys.getConfig(); 
		
		for(scienceMaps::iterator it = science_maps.begin(); it != science_maps.end(); ++it){
			Json::Value data;
			scienceConfigMap::iterator config_it = config_map.find(it->second.id);
			unsigned need_keji = 0;
			if (config_map.end() != config_it){
				if (config_it->second.cost_config.size() >= (unsigned)(it->second.lv + 1)){
					need_keji = config_it->second.cost_config[it->second.lv];
				}
			}
			data[player_science_id_field] = it->second.id;
			data[player_science_lv_field] = it->second.lv;
			data[player_science_need_keji_field] = need_keji;
			tech_list.append(data);
		}
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::science_update_resp, msg);
	}
	void playerScience::initData(){
		
	}

	void playerScience::autoUpdate(){
		this->update();
	}

	void playerScience::add(playerScienceItem item){
		scienceMaps::iterator it = science_maps.find(item.id);
		if (science_maps.end() == it){
			science_maps[item.id] = item;
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_science_num);
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_science_lv);
		}
	}

	void playerScience::del(int id){
		scienceMaps::iterator it = science_maps.find(id);
		if (science_maps.end() != it){
			science_maps.erase(id);
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_science_num);
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_science_lv);
		}
	}

	bool playerScience::set_level(int id, int lv)
	{
		scienceConfigMap& cMap = science_sys.getConfig();
		scienceConfigMap::iterator sc_it = cMap.find(id);
		if (sc_it == cMap.end())return false;
		ConfigEntity& config = sc_it->second;
		
		scienceMaps::iterator it = science_maps.find(id);
		if (it != science_maps.end())
		{
			it->second.lv = lv;
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_science_lv);
			return true;
		}
		else
		{
			playerScienceItem item;
			item.id = id;
			item.lv = lv;
			add(item);
		}

		if (config.isAttri())
		{
			own.Embattle.updateBattleValue();
		}

		return false;
	}

	bool playerScience::get(){
		mongo::BSONObj key = BSON(playerIDStr << own.playerID); 
		mongo::BSONObj obj = db_mgr.FindOne(playerScienceDBStr, key);
		if(obj.isEmpty())return false;
		science_maps.clear();
		checkNotEoo(obj[player_science_data_field]){
			vector<BSONElement> techno_vect = obj[player_science_data_field].Array();
			for(unsigned i = 0;i < techno_vect.size(); ++i){
				playerScienceItem item;
				item.id = techno_vect[i][player_science_id_field].Int();
				item.lv = techno_vect[i][player_science_lv_field].Int();
				science_maps[item.id] = item;
			}
		}
		return true;
	}

	int playerScience::embattleLV(const int id)
	{
		int sID = science_sys.getFormatMaping(id);
		return getScienceLv(sID);
	}

	playerScienceBuffer playerScience::getBufferNoFormat(){
		playerScienceBuffer result;

		scienceConfigMap& config_map = science_sys.getConfig();
		int formation_id = own.Embattle.getCurrentFormat();
		for (playerScience::scienceMaps::iterator it = this->science_maps.begin(); it != this->science_maps.end(); ++it){
			int id = it->second.id;
			scienceConfigMap::iterator config_it = config_map.find(id);
			if (config_it == config_map.end())
				continue;
			playerScienceItem& science_item = it->second;
			ConfigEntity& config = config_it->second;
			if (config.zhenxing == 0){
				for (unsigned i = 0; i < config.effect.size(); ++i){
					int effect_id = static_cast<int>(config.effect[i][0]);
					double value = config.effect[i][1];
					if (effect_id >= 0 && effect_id < characterNum)
						result.initialCharacter[effect_id] += science_item.lv * value;
				}
			}
		}
		return result;
	}

	playerScienceBuffer playerScience::getBuffer(){
		playerScienceBuffer result;
		
		scienceConfigMap& config_map = science_sys.getConfig();
		int formation_id = own.Embattle.getCurrentFormat();
		for (playerScience::scienceMaps::iterator it = this->science_maps.begin(); it != this->science_maps.end(); ++it){
			int id = it->second.id;
			scienceConfigMap::iterator config_it = config_map.find(id);
			if (config_it == config_map.end())
				continue;
			playerScienceItem& science_item = it->second;
			ConfigEntity& config = config_it->second;
			if (config.zhenxing == 0 || formation_id == config.zhenxing){
				for(unsigned i =0; i < config.effect.size(); ++i){
					int effect_id = static_cast<int>(config.effect[i][0]);
					double value = config.effect[i][1];
// 					if (HP_BUFFER_EFFECT_ID == effect_id){//增加hp
// 						result.hp += static_cast<int>(science_item.lv * value);
// 					}else if (effect_id >= 0 && effect_id <= 17){//0到17都是给武将加属性
// 						result.initialCharacter[effect_id] += science_item.lv * value;
// 					}
					if (effect_id >= 0 && effect_id < characterNum)
						result.initialCharacter[effect_id] += science_item.lv * value;
				}
			}
		}

		return result;
	}

	//获取科技点加成   百分比  (加成 80% 则返回  0.8)
	double playerScience::getKeJiBuffer(){
		double result = 0.0f;
		scienceConfigMap& config_map = science_sys.getConfig();
		for (playerScience::scienceMaps::iterator it = this->science_maps.begin(); it != this->science_maps.end(); ++it){
			int id = it->second.id;
			scienceConfigMap::iterator config_it = config_map.find(id);
			if (config_it == config_map.end())
				continue;
			playerScienceItem& science_item = it->second;
			ConfigEntity& config = config_it->second;

			for(unsigned i =0; i < config.effect.size(); ++i){
				int effect_id = static_cast<int>(config.effect[i][0]);
				double value = config.effect[i][1];
				if (KEJI_BUFFER_EFFECT_ID == effect_id && 0 < value ){
					result += (science_item.lv * (value+0.00000001));
				}
			}

		}
		return result;
	}
	 
	bool playerScience::save(){
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		mongo::BSONArrayBuilder array_techno;

		for (scienceMaps::iterator it = science_maps.begin(); it != science_maps.end(); ++it){  
			mongo::BSONObjBuilder item;
			item << player_science_id_field << it->second.id << player_science_lv_field << it->second.lv;
			array_techno.append(item.obj());
		}

		obj << playerIDStr << own.playerID << player_science_data_field << array_techno.arr(); 
		return db_mgr.save_mongo(playerScienceDBStr, key, obj.obj());
	}

	int playerScience::getMaxScienceLv()
	{
		int max_lv = 0;
		for(scienceMaps::iterator iter = science_maps.begin(); iter != science_maps.end(); ++iter)
		{
			if(iter->second.lv > max_lv)
				max_lv = iter->second.lv;
		}
		return max_lv;
	}

	int playerScience::getScienceLv(int scienceID)
	{
		scienceMaps::iterator it = science_maps.find(scienceID);
		if (it != science_maps.end())
			return it->second.lv;
		return 0;
	}

}
