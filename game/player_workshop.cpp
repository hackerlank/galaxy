#include "mongoDB.h"
#include "playerManager.h"
#include "player_workshop.h"
#include "time_helper.h"
#include "helper.h"
#include "workshop_system.h"
#include "task_system.h"

using namespace mongo;

namespace gg
{

	playerWorkShop::playerWorkShop(playerData& own) : Block(own), counter(10), level(0), exp(0), last_timestamp(0), last_auto_sale_timestamp(0), total_free_counter(0), qianzhi_counter(0)
	{

	}

	int playerWorkShop::getGoodsCount(){
		int result = 0;
		for(goodsMap::iterator it = goods_map.begin(); it != goods_map.end(); ++it){
			result = result + it->second.count;
		}
		return result;
	}

	//获取免费生产次数
	int playerWorkShop::getPresentationCount(){
		own.TickMgr.Tick();
		return this->counter;
	}
	
	//函数已 insert save set 与  addPackage
	void playerWorkShop::addPresentation(int count){
		own.TickMgr.Tick();
		if(0 > this->counter) this->counter = 0;
		if(0> count) return;
		this->counter += count;
		
		helper_mgr.insertSaveAndUpdate(this);
// 		Json::Value msg;
// 		this->package(msg);
// 		this->addPackage(gate_client::workshop_update_resp, msg);
	}

	void playerWorkShop::updateYield(bool is_qianzhi){
		this->last_timestamp = na::time_helper::get_current_time();
		if(is_qianzhi)
			this->qianzhi_counter ++;
		else
		{
			this->counter--;
			addTotalFreeCounter();
		}
	}

	void playerWorkShop::addGoods(const int goods_type){
		goodsMap::iterator it = this->goods_map.find(goods_type);
		if(it != this->goods_map.end()){
			playerGoods& goods = it->second;
			goods.count++;
		}else{
			playerGoods goods = playerGoods(goods_type);
			goods.count = 1;
			this->goods_map[goods_type] = goods;
		}
	}

	int playerWorkShop::getQianzhiCounter(){
		own.TickMgr.Tick();
		return this->qianzhi_counter;
	}
	 
	void playerWorkShop::package(Json::Value& pack){
		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();
		Json::Value& goods_json = pack[msgStr][1u][player_goods_data_field];
		goods_json = Json::arrayValue;
		unsigned i = 0;
		for (goodsMap::iterator it = goods_map.begin(); it != goods_map.end(); ++it,++i)
		{
			int id =  it->second.id;
			int count = it->second.count;
			goods_json[i][player_goods_id_filed] = id;
			goods_json[i][player_goods_count_field] = count;
		}
		pack[msgStr][1u][player_qianzhi_cost_gold] = workshop_sys.getQianzhiYieldCostGold(this->own.getOwnDataPtr());
		pack[msgStr][1u][player_workshop_counter_field] = getPresentationCount();
		pack[msgStr][1u][player_workshop_level_field] = this->level;
		pack[msgStr][1u][player_workshop_exp_field] = this->exp;
		pack[msgStr][1u][player_workshop_qianzhi_counter_field] = getQianzhiCounter();
		pack[msgStr][1u][player_workshop_last_timestamp] = last_timestamp;
	}

	void playerWorkShop::update(){
// 		Json::Value msg;
// 		this->package(msg, gate_client::pilot_update_resp);
// 		this->addPackage(from, msg);
	}
	
	void playerWorkShop::autoUpdate()
	{
//		update(from);
	}

	/* ****** DB 操作********  */

	bool playerWorkShop::get(){
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(player_workshop_db_name, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[player_goods_data_field]){
				goods_map.clear();
				vector<BSONElement> els = obj[player_goods_data_field].Array();
				for (unsigned i = 0; i < els.size(); ++i)
				{
					playerGoods g;
					g.id = els[i][player_goods_id_filed].Int();
					g.count = els[i][player_goods_count_field].Int();
					goods_map[g.id] = g;
				}
			}

			checkNotEoo(obj[player_workshop_counter_field]){
				counter =  obj[player_workshop_counter_field].Int();
			}
			checkNotEoo(obj[player_workshop_level_field]){
				level = obj[player_workshop_level_field].Int();
			}
			checkNotEoo(obj[player_workshop_exp_field]){
				exp = obj[player_workshop_exp_field].Int();
			}
			checkNotEoo(obj[player_workshop_last_timestamp]){
				last_timestamp = (unsigned)obj[player_workshop_last_timestamp].Int();
			}
			checkNotEoo(obj[player_workshop_qianzhi_counter_field]){
				qianzhi_counter = obj[player_workshop_qianzhi_counter_field].Int();
			}
			checkNotEoo(obj[player_workshop_last_auto_sale_timestamp_field]){
				last_auto_sale_timestamp = (unsigned)obj[player_workshop_last_auto_sale_timestamp_field].Int();
			}
			checkNotEoo(obj[player_workshop_total_free_count_field]){
				total_free_counter = obj[player_workshop_total_free_count_field].Int();
			}
			return true;
		}				
		return false;
	}

	bool playerWorkShop::save(){
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID;

		{
			mongo::BSONArrayBuilder array_goods;
			for (goodsMap::iterator it = goods_map.begin(); it != goods_map.end(); ++it)
			{
				playerGoods& item = it->second;
				mongo::BSONObjBuilder b;
				b << player_goods_id_filed << item.id << player_goods_count_field << item.count;
				array_goods.append(b.obj());
			}
			obj << player_goods_data_field << array_goods.arr();
		}

		obj << player_workshop_counter_field << counter << player_workshop_level_field << level << player_workshop_exp_field << exp
			<< player_workshop_last_timestamp << last_timestamp << player_workshop_qianzhi_counter_field << qianzhi_counter 
			<< player_workshop_last_auto_sale_timestamp_field << last_auto_sale_timestamp << player_workshop_total_free_count_field << total_free_counter;
		return db_mgr.save_mongo(player_workshop_db_name, key, obj.obj());
	}

	void playerWorkShop::addTotalFreeCounter()
	{
		++total_free_counter;
		helper_mgr.insertSaveAndUpdate(this);

		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_free_produce_times);
	}

}