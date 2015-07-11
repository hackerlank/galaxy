#include "shop_system.h"
#include "file_system.h"
#include "mongoDB.h"
#include "time_helper.h"
#include "action_def.h"
#include "response_def.h"
#include "task_system.h"
#include "item_system.h"
#include "action_format.hpp"
#include "record_system.h"
#include "season_system.h"
#include "activity_system.h"
#include "activity_game_param.h"
#include "email_system.h"
#include "gm_tools.h"
namespace gg
{
	namespace shop
	{
		void condition::setFromValue(const Json::Value& var)
		{
			if(var["type2"] != Json::nullValue)
				_type = var["type2"].asInt();
			if(var["condition1"] != Json::nullValue)
				_arg1 = var["condition1"].asInt();
			if(var["condition2"] != Json::nullValue)
				_arg2 = var["condition2"].asInt();
		}

		bool condition::check(playerDataPtr d)
		{
			switch(_type){
				case _cnn_not_set:
					return true;
				case _cnn_role_lv:
					return d->Base.getLevel() >= _arg1;
				case _cnn_vip_lv:
					return d->Vip.getVipLevel() >= _arg1;
				case _cnn_ruler_title:
					return d->Ruler.getTitle() == _arg1;
				case _cnn_king:
					return false;
				case _cnn_office_level:
					return d->Office.getOffcialLevel() >= _arg1;
				default:
					return false;
			}
		}

		void limits::setFromValue(const Json::Value& var)
		{
			if(var["Lifelong"] != Json::nullValue)
				_total = var["Lifelong"].asInt();
			if(var["individual "] != Json::nullValue)
				_day = var["individual "].asInt();
			if(var["service"] != Json::nullValue)
				_num = var["service"].asInt();
			if (var["service"] != Json::nullValue)
				_all_num = var["service"].asInt();
		}

		int limits::check(playerDataPtr d, int gid)
		{
			if(_num == 0)
				return _error_lmt_num;

			int total_num = 0;
			int day_num = 0;
			d->Shop.getNum(gid, day_num, total_num);
			if(_day > 0 && day_num >= _day)
				return _error_lmt_day;
			if(_total > 0 && total_num >= _total)
				return _error_lmt_total;
			return _success;
		}

		void limits::alterNum(int num) const
		{
			_num = num;
		}

		Json::Value item::transfer(const Json::Value& var)
		{
			Json::Value ret = Json::arrayValue;
			ret.append(var["type1"].asInt());
			ret.append(var["id"].asInt());
			ret.append(1);
			return ret;
		}

		void item::setFromValue(const Json::Value& var, int tabbar)
		{
			_tabbar = tabbar;
			if(var["position"] != Json::nullValue)
				_gid = var["position"].asInt() + 1000 * tabbar;

			Json::Value trans = transfer(var);
			_goods.setValue(trans);

			if(var["price"] != Json::nullValue)
				_price = var["price"].asInt();
			_lmt.setFromValue(var);
			_cnn.setFromValue(var);
		}

		bool item::checkPrice(playerDataPtr d)
		{
			if(_tabbar == _honor_products)
				return d->Base.getWeiWang() >= _price;
			if (_tabbar == _paper_products)
				return d->Base.getGold() >= _price;
			if (_tabbar == _crystal_products)
				return d->Base.getAllGold() >= (int)ceil((double)_price + (double)_price * activity_sys.getRate(param_crystal_products_cost, playerDataPtr()));
			if (_tabbar == _purchase_products)
				return d->Base.getAllGold() >= (int)ceil((double)_price + (double)_price * activity_sys.getRate(param_purchase_products_cost, playerDataPtr()));
			return d->Base.getAllGold() >= _price;
		}

		int item::checkOther(playerDataPtr d)
		{
			if (_goods.getType() == action_add_item && _tabbar == _crystal_products)
				return d->Item.canAddItem(_goods.getId(), 1)? _success : _error_package_full;
			return _success;
		}

		int item::check(playerDataPtr d)
		{
			if(!_cnn.check(d))
				return _error_cnn;
			
			int result = _lmt.check(d, _gid);
			if(result != _success)
				return result;

			if(!checkPrice(d))
				return _error_price;

			//result = checkOther(d);
			return result;
		}

		int item::getItem(playerDataPtr d)
		{
			//if (_tabbar == _purchase_products || _tabbar == _hot_products || _tabbar == _paper_products)
			//{

			if (_tabbar == _crystal_products)
			{
				Json::Value simple_v = Json::arrayValue;
				simple_v.append(_goods.getValue());
				Json::Value cv = gm_tools_mgr.simple2complex(simple_v);
				Json::Value param;
				actionResult result = actionFormat::actionDo(d, cv, param);
				shop_sys.param() = Json::arrayValue;
				if (result.resultCode != 0)
				{
					email_sys.sendSystemEmailCommon(d, email_type_system_attachment, email_team_system_bag_full_shop, Json::Value::null, simple_v);
					shop_sys.param().append(Json::arrayValue);
					shop_sys.param().append(simple_v);
				}
				else
				{
					shop_sys.param().append(simple_v);
					shop_sys.param().append(Json::arrayValue);
				}
			}
			else
			{
				actionFormat::actionVec vec = item_sys.getItemAction(_goods.getId());
				Json::Value param;
				vector<actionResult> vecRetCode = actionFormat::actionDoJump(d, vec, param);
				shop_sys.param() = Json::arrayValue;
				shop_sys.param().append(gm_tools_mgr.lastDo2simple(actionFormat::getLastDoJson()));
				Json::Value fail_value = Json::arrayValue;
				email_sys.sendDoJumpFailEmail(d, vec, vecRetCode, email_team_system_bag_full_shop, fail_value);
				shop_sys.param().append(fail_value);
			}

				/*if (actionFormat::actionDo(d, vec).breakID != 0)
				{
				Json::Value mail = _goods.getValue();
				shop_sys.param() = mail;
				Json::Value temp;
				temp.append(mail);
				email_sys.sendSystemEmailCommon(d, email_type_system_attachment, email_team_system_bag_full_shop, Json::Value::null, temp);
				}*/
				/*}
				else
				_goods.getReward(d);*/

			int price = _price;
			if (_tabbar == _crystal_products)
				price = (int)std::ceil((double)_price + (double)_price * activity_sys.getRate(param_crystal_products_cost, playerDataPtr()));
			if (_tabbar == _purchase_products)
				price = (int)std::ceil((double)_price + (double)_price * activity_sys.getRate(param_purchase_products_cost, playerDataPtr()));

			if (_tabbar == _honor_products)
				d->Base.alterWeiWang(0 - price);
			else if (_tabbar == _paper_products)
				d->Base.alterGold(0 - price);
			else
				d->Base.alterBothGold(0 - price);

			return 0;
		}
	}

	using namespace shop;

	const string shop_system::shopHotFileStr = "./instance/shop/hot.json";
	const string shop_system::shopCrystalFileStr = "./instance/shop/crystal.json";
	const string shop_system::shopCrystalFileStr2 = "./instance/shop/crystal_2.json";
	const string shop_system::shopPurchaseFileStr = "./instance/shop/purchase.json";
	const string shop_system::shopHonorFileStr = "./instance/shop/honor.json";
	const string shop_system::shopPaperFileStr = "./instance/shop/paper.json";
	shop_system* const shop_system::shopSys = new shop_system;

	void shop_system::initData()
	{
		loadFile();
		loadDB();
	}

	void shop_system::loadFile()
	{
		loadFile(shopHotFileStr, _hot_products);
		loadFile(shopPurchaseFileStr, _purchase_products);
		loadFile(shopHonorFileStr, _honor_products);
		unsigned now = na::time_helper::get_current_time();
		loadFile(shopCrystalFileStr, _crystal_products, now);
		loadFile(shopCrystalFileStr2, _crystal_products, now);
		loadFile(shopPaperFileStr, _paper_products);
	}

	void shop_system::loadFile(const string& filename, int tabbar, unsigned now)
	{
		Json::Value val = na::file_system::load_jsonfile_val(filename);
		for(unsigned i = 0; i < val.size(); ++i)
		{
			if(val[i]["trueorfalse"].asInt() == 0)
				continue;

			item it;
			it.setFromValue(val[i], tabbar);
			/*if(_items.find(it.gid()) != _items.end())
				throw("load file error");*/

			if (tabbar == _crystal_products && filename == shopCrystalFileStr)
			{
				_crystal[it.gid()] = it;
				if (now - season_sys.getServerOpenTime() >= 6 * na::time_helper::DAY + 17 * na::time_helper::HOUR)
					_items[it.gid()] = it;
			}
			if (tabbar == _crystal_products && filename == shopCrystalFileStr2)
			{
				_crystal2[it.gid()] = it;
				if (now - season_sys.getServerOpenTime() < 6 * na::time_helper::DAY + 17 * na::time_helper::HOUR)
					_items[it.gid()] = it;
			}
			if (tabbar != _crystal_products)
			{
				_items[it.gid()] = it;
				_backup[it.gid()] = it;
			}
		}
	}

	void shop_system::loadDB()
	{
		objCollection objs = db_mgr.Query(shopGoodsDBStr);

		for(unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];
			int gid = obj[shop_goods_id_field_str].Int();
			if(gid == -1)
			{
				_next_update_time = obj[shop_next_update_time_field_str].Int();
				continue;
			}
			int num = obj[shop_num_field_str].Int();

			ItemMap::iterator iter = _items.find(gid);
			if(iter != _items.end())
				iter->second.lmt().alterNum(num);
		}
	}

	void shop_system::checkAndUpdate()
	{
		unsigned now = na::time_helper::get_current_time();
		if(now >= _next_update_time)
		{
			alterNextUpdateTime(na::time_helper::get_next_update_time(now, 22));
			resetItemNum();
		}
	}

	void shop_system::alterNextUpdateTime(unsigned nt)
	{
		_next_update_time = nt;
		mongo::BSONObj key = BSON(shop_goods_id_field_str << -1);
		mongo::BSONObj obj = BSON(shop_goods_id_field_str << -1
			<< shop_next_update_time_field_str << _next_update_time);

		db_mgr.save_mongo(shopGoodsDBStr, key, obj);
	}

	void shop_system::resetItemNum()
	{
		_items = _backup;
		unsigned now = na::time_helper::get_current_time();
		if (now - season_sys.getServerOpenTime() >= 6 * na::time_helper::DAY + 17 * na::time_helper::HOUR)
		{
			for (ItemMap::iterator iter = _crystal.begin(); iter != _crystal.end(); ++iter)
				_items.insert(make_pair(iter->first, iter->second));
		}
		else
		{
			for (ItemMap::iterator iter = _crystal2.begin(); iter != _crystal2.end(); ++iter)
				_items.insert(make_pair(iter->first, iter->second));
		}
		for(ItemMap::iterator iter = _items.begin(); iter != _items.end(); ++iter)
			save(iter->second);
	}

	void shop_system::save(const item& it)
	{
		mongo::BSONObj key = BSON(shop_goods_id_field_str << it.gid());
		mongo::BSONObj obj = BSON(shop_goods_id_field_str << it.gid()
			<< shop_num_field_str << it.lmt().num());

		db_mgr.save_mongo(shopGoodsDBStr, key, obj);
	}

	void shop_system::shopUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		int gid = js_msg[0u].asInt();

		checkAndUpdate();
		unsigned now = na::time_helper::get_current_time();
		d->Shop.checkAndUpdate(now);

		ItemMap::iterator iter = _items.find(gid);
		if(iter == _items.end())
			Return(r, -1);

		update(d, gid);
	}

	void shop_system::shopDealReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		int gid = js_msg[0u].asInt();

		checkAndUpdate();

		unsigned now = na::time_helper::get_current_time();
		d->Shop.checkAndUpdate(now);

		ItemMap::iterator iter = _items.find(gid);
		if(iter == _items.end())
			Return(r, -1);                                               // 找不到该商品

		int result = iter->second.check(d);
		if(result != _success)
			Return(r, result);

		int before = iter->second.tabbar() == _honor_products? 
			d->Base.getWeiWang() : d->Base.getAllGold();
		int item_id = iter->second.getItem(d);
		int after = iter->second.tabbar() == _honor_products? 
			d->Base.getWeiWang() : d->Base.getAllGold();

		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(iter->second.goods().getType()));
		fields.push_back(boost::lexical_cast<string, int>(iter->second.goods().getId()));
		fields.push_back(boost::lexical_cast<string, int>(iter->second.goods().getNum()));
		fields.push_back(boost::lexical_cast<string, int>(iter->second.price()));
		fields.push_back(boost::lexical_cast<string, int>(iter->second.tabbar() != _honor_products? 0 : 1));
		StreamLog::Log(shopLogDBStr, d, boost::lexical_cast<string, int>(before), boost::lexical_cast<string, int>(after), fields, _log_shop_deal);

		//if(iter->second.tabbar() == _purchase_products)
		//{
		//	actionFormat::actionVec vec = item_sys.getItemAction(iter->second.goods().getId());
		//	Json::Value param;
		//	actionFormat::actionDoJump(d, vec, param);

		//	/*Json::Value msg;
		//	msg[msgStr] = Json::arrayValue;
		//	msg[msgStr].append(item_id);
		//	string str = msg.toStyledString();
		//	na::msg::msg_json m(d->playerID, d->netID, gate_client::player_item_use_req, str);
		//	player_mgr.postMessage(m);*/
		//}
		alterItemNum(iter->second);
		d->Shop.updateAfterDeal(gid);
		task_sys.updateBranchTaskInfo(d, _task_shop_buy_times);
		update(d, gid);
		if(iter->second.tabbar() == _hot_products)
			info(d);

		if (iter->second.tabbar() == _purchase_products)
			activity_sys.updateActivity(d, 0, activity::_buy_limited_resources);

		r[msgStr][1u] = _param[0u];
		r[msgStr][2u] = _param[1u];
		_param = Json::nullValue;

		Return(r, 0);
	}

	void shop_system::shopInfoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		checkAndUpdate();

		unsigned now = na::time_helper::get_current_time();
		d->Shop.checkAndUpdate(now);

		info(d);
	}

	void shop_system::buyMonthCardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		
		if (d->Base.getGold() < 300)
			Return(r, 1);
		if (d->Vip.monthCard_.getTimes(na::time_helper::get_current_time()) > 0)
			Return(r, 2);

		d->Base.alterGold(-300);
		d->Vip.buyMonthCard();
		Return(r, 0);
	}

	void shop_system::alterItemNum(const shop::item& it)
	{
		if(it.lmt().num() > 0)
		{
			it.lmt().alterNum(it.lmt().num() - 1);
			save(it);
		}
	}

	void shop_system::update(playerDataPtr d, int gid)
	{	
		ItemMap::iterator iter = _items.find(gid);
		if(iter == _items.end())
			return;

		Json::Value msg;
		msg[msgStr][1u][shop_num_field_str] = iter->second.lmt().num();
		msg[msgStr][1u][shop_limit_num_field_str] = iter->second.lmt().all();
		d->Shop.packageBuyTimes(msg[msgStr][1u], gid);
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::shop_update_resp, msg);
	}

	void shop_system::info(playerDataPtr d)
	{
		Json::Value msg;
		msg[msgStr][1u] = Json::arrayValue;
		for(ItemMap::iterator iter = _items.begin(); iter != _items.end(); ++iter)
		{
			if(iter->second.tabbar() != _hot_products)
				continue;

			int day_num = 0;
			int total_num = 0;
			d->Shop.getNum(iter->second.gid(), day_num, total_num);
			if(total_num < iter->second.lmt().total())
				msg[msgStr][1u].append(iter->second.gid());
		}
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::shop_info_resp, msg); 
	}

	void shop_system::gmShopInfoReq(msg_json& m, Json::Value& r)
	{
		Return(r, -1);
	}

	void shop_system::gmShopModifyReq(msg_json& m, Json::Value& r)
	{
		Return(r, -1);
	}
}