#pragma once 

#include "block.h"
#include <vector>
#include <boost/unordered_map.hpp> 
using namespace std;

namespace gg
{ 
	const static string player_workshop_db_name		  = "gl.player_workshop";
	const static string player_workshop_counter_field = "ctr";/*生产计数器*/
	const static string player_workshop_qianzhi_counter_field = "qctr";//强制生产次数  （计数器）
	const static string player_workshop_last_timestamp = "lt";/*最后一次生产时间戳*/
	const static string player_workshop_level_field	  = "lv";
	const static string player_workshop_exp_field	  = "exp";
	const static string player_workshop_total_free_count_field = "fc";

	const static string player_workshop_last_auto_sale_timestamp_field = "last";//最后一次自动卖出时间

	const static string player_goods_data_field  = "d";
	const static string player_goods_id_filed	 = "id";
	const static string player_goods_count_field = "ct";//单个物品数量

	//--- 只作为 协议返回字段
	const static string player_qianzhi_cost_gold = "qzcg";//当前强制生产所需要金币

	struct playerGoods
	{
		playerGoods(){
			id = 0;
			count = 0;
		}
		playerGoods(const int goods_type){
			memset(this, 0x0, sizeof(playerGoods));
			this->id = goods_type;
			count = 0;
		}
		bool operator == (const playerGoods& goods){
			return this->id == goods.id && this->count == goods.count;
		}

		int id;
		int count;
	};

	class TTManager;
	class playerWorkShop : public Block
	{
		friend class TTManager;
	public:
		playerWorkShop(playerData& own);
		virtual bool get();
		virtual bool save();
		virtual void update();
		virtual void autoUpdate();
		 
		int getPresentationCount();//获取免费生产次数
		void addPresentation(int count);//添加免费生产次数, 函数已 insert save set 与  addPackage
		int getGoodsCount();
		int getQianzhiCounter();//获取强制生产计数器  
		void updateYield(bool is_qianzhi);
		int getTotalFreeCounter(){ return total_free_counter; }
		void addTotalFreeCounter();
		int exp;
		int level;
		void addGoods(const int goods_type);//添加物品
		typedef boost::unordered_map< int, playerGoods > goodsMap;
		goodsMap goods_map;
		void package(Json::Value& pack);
		unsigned last_auto_sale_timestamp; //最后一次自动卖出物品时间
		unsigned last_timestamp;//最后一次生产时间 
	private:
		int counter;
		int qianzhi_counter;
		int total_free_counter;
	};
}
