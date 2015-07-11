#include "workshop_system.h"
#include "time_helper.h"
#include "mongoDB.h"
#include "helper.h"
#include "response_def.h" 
#include "file_system.h"
#include "war_story.h"
#include "task_system.h"
#include "record_system.h"
#include "activity_system.h"
#include "activity_game_param.h"
using namespace mongo;
namespace gg
{
	workshopManager* const workshopManager::workshopMgr =  new workshopManager();

	workshopManager::workshopManager() :sale_percent(40), nextSalePercent_(50),sale_isUP(true), last_update_percent_timestamp(0){
		tick = na::time_helper::get_sys_time();
	}

#define PRECISION 0.000001f

	const static string cost_gold_config_path = "./instance/workshop/cost_gold.json";
	const static string yield_exp_config_path = "./instance/workshop/exp.json";
	const static string yield_probability_config_path = "./instance/workshop/yield_probability.json";

	void workshopManager::initData(){
		vipLv = 0;
		levellimit = 29;
		this->get();
		Json::Value cost_gold_config_json = na::file_system::load_jsonfile_val(cost_gold_config_path);
		if (!cost_gold_config_json.isNull()){
			for(unsigned i = 0; i < cost_gold_config_json.size(); ++i){
				Json::Value& item = cost_gold_config_json[i];
				cost_gold_config_vect.push_back(item.asInt());
			}

		}

		Json::Value yield_exp_config_json = na::file_system::load_jsonfile_val(yield_exp_config_path);
		if (!yield_exp_config_json.isNull()){
			for(unsigned i = 0; i < yield_exp_config_json.size(); ++i){
				Json::Value& item = yield_exp_config_json[i];
				yield_exp_config_vect.push_back(item.asInt());
			}
		}


		int yield_config_error = 0;
		Json::Value yield_probability_json = na::file_system::load_jsonfile_val(yield_probability_config_path);
		if (!yield_probability_json.isNull() && yield_probability_json.size() >= 2u){
			if(yield_probability_json[0].size() >= 3){
				this->yield_probability[type_taiyan] = yield_probability_json[0][0].asUInt();
				this->yield_probability[type_fushe] = yield_probability_json[0][1].asUInt();
				this->yield_probability[type_liuzi] = yield_probability_json[0][2].asUInt();
				this->yield_probability[type_anwuzhi] = yield_probability_json[0][3].asUInt();
			}else{
				yield_config_error = 2;
			}

			if(yield_probability_json[1].size() >= 3){
				unsigned test = yield_probability_json[1][1].asUInt();
				this->yield_baoji_probability[type_taiyan] = yield_probability_json[1][0].asUInt();
				this->yield_baoji_probability[type_fushe] = yield_probability_json[1][1].asUInt();
				this->yield_baoji_probability[type_liuzi] = yield_probability_json[1][2].asUInt();
				this->yield_baoji_probability[type_anwuzhi] = yield_probability_json[1][3].asUInt();
			}else{
				yield_config_error = 3;
			}

		}else{
			yield_config_error = 1;
		}

		if(0 != yield_config_error){
			LogE << "yield probability  config error... error code:" << yield_config_error << LogEnd;
		}
	}

	void workshopManager::scheduleUpdate(boost::system_time& sys_time){//定时操作一些东西 

		if((sys_time - tick).total_seconds() < 10)return;
		tick = sys_time;
		/*物价变化
		在0.5和2.0之间循环单向往返浮动
		每半小时变化一次，一次循环时间为8-10小时。
		每次变化的数值为0.1-0.13中随机一个,小数精确到百分位

		当下一次的变化超过上下限时，则反向变化。例如：当前粮价0.6 下降，如果半小时后变化的数值为0.15，应降到0.45，但小于0.5，所以此时不再下降，而是转为上升并重新随机取值。

		*/ 
		unsigned now = na::time_helper::get_current_time();
		if (now >= last_update_percent_timestamp){
			bool loop = false;
			bool is_change = false;
			int loop_counter = 0;
			sale_percent = nextSalePercent_;
			int tempNextPercent = nextSalePercent_;
			tm t = na::time_helper::toTm(now);
			unsigned nextHalfHourTime = now - 60 * t.tm_min - t.tm_sec;
			nextHalfHourTime = (now >= (nextHalfHourTime + 1800) ? (nextHalfHourTime + 3600) : (nextHalfHourTime + 1800));
			while(true){
				loop_counter += 1;
				if (loop_counter >= 1000){
					nextSalePercent_ = workshop_config::limit_min;
					last_update_percent_timestamp = nextHalfHourTime;
					LogE << "workshopManager::scheduleUpdate loop limit, business logic error. timestamp:" << last_update_percent_timestamp << LogEnd;
					break;
				}
				int random_number = commom_sys.randomBetween(workshop_config::random_min, workshop_config::random_max);
//				double random_number = 10;

				if (sale_isUP)
					tempNextPercent += random_number;
				else
					tempNextPercent -= random_number;

				if (tempNextPercent < workshop_config::limit_min){
					if(!is_change){
						sale_isUP = true;//!sale_isUP;
						is_change = true;
						tempNextPercent = workshop_config::limit_min;
					}
				}
				if (tempNextPercent > workshop_config::limit_max){
					if(!is_change){
						sale_isUP = false;// !sale_isUP;
						is_change = true;
						tempNextPercent = workshop_config::limit_max;
					}
				}
				last_update_percent_timestamp = nextHalfHourTime;
				nextSalePercent_ = tempNextPercent;
//				LogW << "current:" << sale_percent << ", next:" << nextSalePercent_ << LogEnd;
				break;
			}
			this->save();
		}
		
	}

	void workshopManager::update(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
//		d->Workshop.check_and_reset_state();
		Json::Value msg;
		this->package(msg, d);
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::workshop_update_resp, msg);
	}
	
	void workshopManager::package(Json::Value& msg, playerDataPtr d){
		msg[msgStr][1u][workshop_goods_percent_field] = this->sale_percent;
		msg[msgStr][1u][workshop_kucun_field] = d->Workshop.getGoodsCount();
		msg[msgStr][1u][workshop_container_size] = this->getKuCunCount(d);
		msg[msgStr][1u][workshop_last_update_percent_timestamp_field] = this->last_update_percent_timestamp;
		bool isUp;
		if (nextSalePercent_ > sale_percent)
			isUp = true;
		else if (nextSalePercent_ < sale_percent)
			isUp = false;
		else
		{
			if (nextSalePercent_ == workshop_config::limit_max)
				isUp = false;
			else if (nextSalePercent_ == workshop_config::limit_min)
				isUp = true;
			else
				isUp = true;
		}
		msg[msgStr][1u][workshop_goods_isUp_field] = isUp;
		d->Workshop.package(msg);
	}

	int workshopManager::getKuCunCount(playerDataPtr d){

		/*测试自己加一个建筑。。。*/
		
		//playerBuild building = d->Builds.getBuildOther(building_ware_center);
		//if (playerBuild::NullValue == building){
		//	d->Builds.activeBuilding(building_ware_center);
		//	building = d->Builds.getBuildOther(building_ware_center);
		//}
		////int code = d->_builds.activeBuilding(building_ware_center, 9); 
		//if(playerBuild::NullValue == building)return 0;
		//return building.buildLV * 2;

		/*测试自己加一个建筑。。。*/ 
		return d->Base.getLevel() * 2;
	}
	//
	const static string production_log = "log_production_sys";
	void workshopManager::yield(msg_json& m, Json::Value& r){// 搓篮子
		//错误码：
		/*
			1：库存已满
			2：免费生产已经用完了 
			3：不够金币消费
			4: 
		*/

		AsyncGetPlayerData(m);
		ReadJsonArray;
		if(d->Base.getLevel()<levellimit)
			Return(r,101);
		int proValue = 0;
		bool baoji = js_msg[0u].asBool();//是否够选了爆击
		bool qianzhi = js_msg[1u].asBool();//强制生产 
//		d->Workshop.check_and_reset_state();
		/*先判断库存
		库存上限  =建筑等级*2  (所有物品)
		*/
		int current_count = d->Workshop.getGoodsCount();
		int kucun = this->getKuCunCount(d);
		if(current_count >= kucun)Return(r, 1);//库存已满

		/*判断今天剩余免费次数足够不*/
		if(!qianzhi && 0 >= d->Workshop.getPresentationCount()) Return(r, 2);//免费生产已经用完了 
		
		if (d->Workshop.level < 5 && baoji ) Return(r, 4);//生产工艺不足5级时,应隐藏暴击勾选项;服务端应做限制;
		
		int cost_gold = 0;

		if(baoji)
			cost_gold += 2;
		
		//强制生产加金币 
		/*
		强制生产按钮：当每天10次免费生产用完后，免费生产按钮变为强制生产按钮，上面显示所需的资源消耗。
		刷新时间：每天早上5点。
		*/
		if(qianzhi)
			cost_gold += getQianzhiYieldCostGold(d);

		//检查是否够金币
		if(cost_gold >(proValue=d->Base.getAllGold())) Return(r, 3);//不够金币消费
		//扣除金币
		d->Base.alterBothGold(-cost_gold);

		//开始生产...
		/* 
			各个概率是独立的，比如我现在生产槽品质是40，那么我每次生产会生产出太阳能，同时有20%概率生产出辐射能，有20%概率生产出离子流
		*/
		 
		unsigned probability_1 = 0;
		unsigned probability_2 = 0;
		unsigned probability_3 = 0;
		unsigned probability_4 = 0;
		if(baoji){
			probability_1 = this->yield_baoji_probability[type_taiyan];
			probability_2 = this->yield_baoji_probability[type_fushe];
			probability_3 = this->yield_baoji_probability[type_liuzi];
			probability_4 = this->yield_baoji_probability[type_anwuzhi];
		}else{
			probability_1 = this->yield_probability[type_taiyan];
			probability_2 = this->yield_probability[type_fushe];
			probability_3 = this->yield_probability[type_liuzi];
			probability_4 = this->yield_probability[type_anwuzhi];
		}
		/* 处理特殊情况：  仓库不够则舍弃多余生产出来的产品 */
		unsigned create_count = 0u;
		//生产太阳能 
		unsigned random_number = static_cast<unsigned>(commom_sys.randomBetween(1, 100));
		if(random_number <= probability_1){
			d->Workshop.addGoods(type_taiyan);
			current_count	++;
			create_count	++;
		}
		
		
		if (5 <= d->Workshop.level /* && d->_workshop.level <= 30*/ && current_count < kucun ){
			unsigned random_number = static_cast<unsigned>(commom_sys.randomBetween(1, 100));
			if(random_number <= probability_2){
				d->Workshop.addGoods(type_fushe);//辐射能
				current_count	++;
				create_count	++;
            }
		}
		if(8 < d->Workshop.level /* && d->_workshop.level <= 50*/ && current_count < kucun){
			unsigned random_number = static_cast<unsigned>(commom_sys.randomBetween(1, 100));
			if(random_number <= probability_3){
				d->Workshop.addGoods(type_liuzi);//离子流
				current_count	++;
				create_count	++;
			}
		}
		if(12 < d->Workshop.level  && current_count < kucun){
			unsigned random_number = static_cast<unsigned>(commom_sys.randomBetween(1, 100));
			if(random_number <= probability_4){
				d->Workshop.addGoods(type_anwuzhi);//暗物质
				current_count	++;
				create_count	++;
			}
		}

		bool full_level = this->addExp(d, create_count);//添加经验
		//更新最后生产时间 并 累计免费生产次数
		d->Workshop.updateYield(qianzhi);
		  
		helper_mgr.insertSaveAndUpdate(&d->Workshop);
		this->update(m, r);
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string ,int>(d->Workshop.level));
		fields.push_back(boost::lexical_cast<string ,int>(10-d->Workshop.getPresentationCount()+d->Workshop.getQianzhiCounter()));
		fields.push_back(boost::lexical_cast<string ,int>(baoji));
		StreamLog::Log(production_log, d, boost::lexical_cast<string, int>(proValue), boost::lexical_cast<string, int>(proValue-cost_gold),fields,qianzhi);
		if(qianzhi)
			activity_sys.updateActivity(d, 0, activity::_force_produce);
		else
			activity_sys.updateActivity(d, 0, activity::_produce);
		
        if (full_level){
            Return(r, 100);
        }

		Return(r, 0);
	}
	const static string sale_log = "log_production_sale_sys";
	void workshopManager::sales(msg_json& m, Json::Value& r){//卖出物品
		AsyncGetPlayerData(m);
		ReadJsonArray; 
//		warStory::warSys->battleWithPlayer(m,r);
		bool yuanxiao = js_msg[0u].asBool();//是否够选了远销

		double real_sale_percent = double(sale_percent)/100;

		//if(yuanxiao && !helper_mgr.isOpenVipQuarter(d)) Return(r, 1);//远销需要开通VIP季卡
		if (yuanxiao && 6 > d->Vip.getVipLevel()) Return(r, 1);//vip 等级不够

		if(yuanxiao)
			real_sale_percent = 1.0f;

		if (0 >= d->Workshop.getGoodsCount()) Return(r, 2);//没有东西卖出

		
		int taiyan_silver = this->getGoodsBasePrice(type_taiyan, d->Workshop.level);
		int fushe_silver = this->getGoodsBasePrice(type_fushe, d->Workshop.level);
		int liuzi_silver = this->getGoodsBasePrice(type_liuzi, d->Workshop.level);
		int anwuzhi_silver = this->getGoodsBasePrice(type_anwuzhi, d->Workshop.level);
		 
		double total_silver = 0;
		for(playerWorkShop::goodsMap::iterator it = d->Workshop.goods_map.begin(); it != d->Workshop.goods_map.end(); ++it){
			playerGoods& player_goods = it->second;
			int count = player_goods.count;
			switch (player_goods.id)
			{
			case type_taiyan:
				total_silver += taiyan_silver * count;
				break;
			case type_fushe:
				total_silver += fushe_silver * count;
				break;
			case type_liuzi:
				total_silver += liuzi_silver * count;
				break;
			case type_anwuzhi:
				total_silver += anwuzhi_silver * count;
				break;
			}
			player_goods.count = 0;//卖出后数量设置为0；
		}
		
		total_silver = this->getGoodsPrice(total_silver, d->Base.getLevel(), false, real_sale_percent); 


		//const playerBuild& build = d->Builds.getBuildOther(building_ware_center);

		//total_silver = this->getGoodsPrice(total_silver, build.buildLV, false, real_sale_percent); 
		int proValue = d->Base.getSilver();
		d->Base.alterSilver((int)total_silver);
		helper_mgr.insertSaveAndUpdate(&d->Workshop);
		this->update(m, r);
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string ,int>((int)total_silver));
		StreamLog::Log(sale_log, d, boost::lexical_cast<string, int>(proValue), boost::lexical_cast<string, int>(proValue+(int)total_silver),fields, yuanxiao);
		Return(r, 0);
	}

	void workshopManager::autoSaleReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (!autoSale(d))
			Return(r, 1);
	}

	bool workshopManager::autoSale(playerDataPtr player)
	{//自动卖出(在每次发送角色更新协议时候调用)
		/*自动卖出
		条件：每天凌晨5点，玩家总库存商品>0。
		方式：系统自动卖出所有商品。（“是否热销”一律取值0，物价取值1.2）
		*/
		tm last_auto_time = na::time_helper::toTm(player->Workshop.last_auto_sale_timestamp);
		unsigned now_timestamp = na::time_helper::get_current_time();
		tm now_time = na::time_helper::toTm(now_timestamp); 
		unsigned the_day_timestamp = now_timestamp - now_time.tm_hour * 3600 - now_time.tm_min * 60 - now_time.tm_sec;
		unsigned the_day_5timestamp = the_day_timestamp + 5 * 3600;
		bool is_sale = false;
		/*
		if(0 != player._workshop.last_auto_sale_timestamp  && last_auto_time.tm_yday != now_time.tm_yday && 0 != player._workshop.goods_map.size() && 5 <= now_time.tm_hour)
			is_sale = true;
// 		else if(last_auto_time.tm_year == now_time.tm_year && last_auto_time.tm_yday == now_time.tm_yday &&  0 != d->_workshop.goods_map.size() && 5 >= last_auto_time.tm_hour)
// 			is_sale = true;
		else if (0 == player._workshop.last_auto_sale_timestamp  && 0 != player._workshop.goods_map.size() && 5 <= now_time.tm_hour && 5 > last_yield_time.tm_hour && last_yield_time.tm_yday == now_time.tm_yday)
			is_sale = true;
		else if (0 == player._workshop.last_auto_sale_timestamp && 0 != player._workshop.goods_map.size() && 5 <= now_time.tm_hour && last_yield_time.tm_yday != now_time.tm_yday)
			is_sale = true;
			*/

		//(上次自动卖出时间 小于 今天5点   并且  目前时间 超过了 5点)   或者  上次自动卖出时间 在昨天5点之前
		if (  (player->Workshop.last_auto_sale_timestamp < the_day_5timestamp && now_timestamp > the_day_5timestamp)   ||   player->Workshop.last_auto_sale_timestamp <=  the_day_5timestamp - 3600 * 24  ){
			is_sale = true;
		}

		if(!is_sale)
			return false;

		int taiyan_silver = this->getGoodsBasePrice(type_taiyan, player->Workshop.level);
		int fushe_silver = this->getGoodsBasePrice(type_fushe, player->Workshop.level);
		int liuzi_silver = this->getGoodsBasePrice(type_liuzi, player->Workshop.level);
		int anwuzhi_silver = this->getGoodsBasePrice(type_anwuzhi, player->Workshop.level);
		 
		double total_silver = 0;
		for(playerWorkShop::goodsMap::iterator it = player->Workshop.goods_map.begin(); it != player->Workshop.goods_map.end(); ++it){
			playerGoods& player_goods = it->second;
			int count = player_goods.count;
			switch (player_goods.id)
			{
			case type_taiyan:
				total_silver += taiyan_silver * count;
				break;
			case type_fushe:
				total_silver += fushe_silver * count;
				break;
			case type_liuzi:
				total_silver += liuzi_silver * count;
				break;
			case type_anwuzhi:
				total_silver += anwuzhi_silver * count;
				break;
			}
			player_goods.count = 0;//卖出后数量设置为0；
		}

		int player_level = player->Base.getLevel();
		vipLv = player->Vip.getVipLevel();
		//const playerBuild& build = player->Builds.getBuildOther(building_ware_center);
		total_silver = this->getGoodsPrice(total_silver, player_level, true); //加上热销产品、物价变化得出最后卖出金币
		player->Workshop.last_auto_sale_timestamp = na::time_helper::get_current_time();

		if((total_silver) == 0)
			return false;
		player->Base.alterSilver((int)total_silver);
		helper_mgr.insertSaveAndUpdate(&player->Workshop);
		Json::Value r, updateJson;
		updateJson[0u] = 0;
		updateJson[1u] = int(total_silver);
		r[msgStr] = updateJson;
		player->sendToClient(gate_client::workshop_auto_resp, r);
		return true;
	}
    /*
     * 工艺等级不能再升级 返回false
     * */
	bool workshopManager::addExp(playerDataPtr d, const int exp){
		if(0 > exp)
			return false;
		if(0 == yield_exp_config_vect.size())
			return false;
		bool full_level = false;
		int max_level = yield_exp_config_vect.size();
		if(d->Workshop.level >= max_level)
			return false;

		int real_exp = d->Workshop.exp;
		real_exp += exp;
		
		int max_exp = yield_exp_config_vect[yield_exp_config_vect.size() - 1];
		
		if(real_exp > max_exp){
			real_exp = max_exp;
		}
		d->Workshop.exp = real_exp;
		
		int real_level = d->Workshop.level;
		for(unsigned i = real_level; i < yield_exp_config_vect.size(); ++i){
			int value = yield_exp_config_vect[i];
			if(real_exp >= value){
				real_level = i + 1;
			}else{
				break;
			}
		}
		if (real_level >= d->Base.getLevel()){
			full_level = true;
			real_level = d->Base.getLevel();

			if (yield_exp_config_vect.size() < static_cast<unsigned>(real_level)){
				real_level = yield_exp_config_vect.size() - 1;
			}
			d->Workshop.exp = yield_exp_config_vect[real_level - 1];
		}

		d->Workshop.level = real_level;
		task_sys.updateBranchTaskInfo(d, _task_produce_lv);
		helper_mgr.insertSaveAndUpdate(&d->Workshop);
         
		return full_level;
	}

	int workshopManager::getQianzhiYieldCostGold(playerDataPtr d){//强制生产所消耗的金币数
		int result = 0;
		if(0 == cost_gold_config_vect.size())
			return 0;

		if(0 == d->Workshop.getQianzhiCounter()){
			result = cost_gold_config_vect[0];
		}else if (static_cast<unsigned>(d->Workshop.getQianzhiCounter()) > cost_gold_config_vect.size()){
			result = cost_gold_config_vect[cost_gold_config_vect.size() - 1];
		}else{
			result =  cost_gold_config_vect[d->Workshop.getQianzhiCounter() - 1];
		}
		//打折金币
		double discount = activity_sys.getRate(param_force_produce_cost,d);
		result = (int)ceil(result*(1+discount));	//这两行代码不能合一
		result = (result<1? 1:result);
//		if(d->Buff.hasBuff(force_work_cost_gold_rate_buff))return (int)(result * d->Buff.getValueDouble(force_work_cost_gold_rate_buff));
		return result;
	}

	int workshopManager::getGoodsBasePrice(int goods_type, int workshop_level){
		/*基础价格

		Ⅰ型纯水(太阳能) 1580+75*工艺等级
		Ⅱ型纯水(辐射能) 1580+80*工艺等级
		Ⅲ型纯水(离子流) 1580+90*工艺等级
		Ⅳ型纯水(暗物资) 1580+105*工艺等级

		  */
		if (0 > workshop_level){
			workshop_level = 0;
		}

		int result = 0;
		switch(goods_type){
		case type_taiyan:
			result = 1580 + int(workshop_level * 75.0f);
			break;
		case type_fushe:
			result = 1580 + int(workshop_level * 80.0f);
			break;
		case type_liuzi:
			result = 1580 + int(workshop_level * 90.0f);
			break;
		case type_anwuzhi:
			result = 1580 + int(workshop_level * 105.0f);
			break;
		}
		return result;
	}

	//目前因为热销面对所有产品，所以可以用这函数直接计算总价， 之前热销只面对单种产品 所以每种物品单价都各需调用这个方法
	double workshopManager::getGoodsPrice(double base_price, int player_level ,bool is_auto_sales){ 
		return this->getGoodsPrice(base_price, player_level, is_auto_sales, double(sale_percent)/100);
	}

	double workshopManager::getGoodsPrice(double base_price, int player_level ,bool is_auto_sales, double use_sale_percent){ 
		/*
			自动卖出以最低物价   
			没有热销加成
		*/
		//单价=基础价格 *（1+主角等级*0.2%+是否热销*30%）* (1 + 物价)
			
		double result = 0;
		if (is_auto_sales){
			double radio = 0.5f;
			if(vipLv>5)
				radio = 1.0f;
			result = base_price * (1 + player_level * 0.0029f) * (1+radio);
		}else{
			//热销产品
			int season = season_sys.getSeason(); 
			/*热销商品：
			根据夏季设定热销商品 （各个商品售价+30%）
			*/
			float hot_sale = 0;
			if(season_quarter_two == season){
				hot_sale = 0.3f;
			}
			result = float(base_price * (1 + player_level * 0.0029f + hot_sale) * (1 + use_sale_percent));
		}
		return result;
	}

	void workshopManager::get(){
		mongo::BSONObj key = BSON(workshop_global_db_key_str << 1);
		mongo::BSONObj obj = db_mgr.FindOne(workshop_global_db_str, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[workshop_goods_percent_field]){
				double temp_percent;
				if (obj[workshop_goods_percent_field].type() == NumberInt)						
					temp_percent = static_cast<double>(obj[workshop_goods_percent_field].Int());
				else if (obj[workshop_goods_percent_field].type() == NumberDouble)
					temp_percent = static_cast<double>(obj[workshop_goods_percent_field].Double());
				else
					temp_percent = workshop_config::limit_min;

				if (temp_percent < 1 + PRECISION)
					sale_percent = int((temp_percent + PRECISION) * 100);
				else
					sale_percent = int(temp_percent + PRECISION);
			}
			checkNotEoo(obj[workshop_next_goods_percent_field]){
				double temp_next_percent;
				if (obj[workshop_next_goods_percent_field].type() == NumberInt)
					temp_next_percent = static_cast<double>(obj[workshop_next_goods_percent_field].Int());
				else if (obj[workshop_next_goods_percent_field].type() == NumberDouble)
					temp_next_percent = static_cast<double>(obj[workshop_next_goods_percent_field].Double());
				else
					temp_next_percent = workshop_config::limit_min + workshop_config::random_min;

				if (temp_next_percent < 1 + PRECISION)
					nextSalePercent_ = int((temp_next_percent + PRECISION) * 100);
				else
					nextSalePercent_ = int(temp_next_percent + PRECISION);
			}
			checkNotEoo(obj[workshop_goods_isUp_field]){
				sale_isUP = obj[workshop_goods_isUp_field].Bool();
			}
			checkNotEoo(obj[workshop_last_update_percent_timestamp_field]){
				last_update_percent_timestamp = (unsigned)obj[workshop_last_update_percent_timestamp_field].Int();
			}
		}
	}

	void workshopManager::save(){
		mongo::BSONObj key = BSON(workshop_global_db_key_str << 1);
		mongo::BSONObj obj = BSON(workshop_global_db_key_str << 1
			<< workshop_goods_percent_field << sale_percent
			<< workshop_goods_isUp_field << sale_isUP
			<< workshop_last_update_percent_timestamp_field << last_update_percent_timestamp
			<< workshop_next_goods_percent_field << nextSalePercent_);
		db_mgr.save_mongo(workshop_global_db_str, key, obj);
	}
}
