#include "market_system.h"
#include "response_def.h"
#include "gm_tools.h"
#include "timmer.hpp"
#include "email_system.h"
#include "action_def.h"

namespace gg
{
	void commodity::load(const Json::Value& info)
	{
		_id = info["commodity_id"].asInt();
		_name = info["commodity_name"].asString();
		_price.load(info["price"]);
		_simpleValue = info["items"];
		_complexValue = gm_tools_mgr.simple2complex(_simpleValue);
		_crystal_limit = false;
		for (unsigned i = 0; i < _complexValue.size(); ++i)
		{
			if (_complexValue[i][ACTION::strActionID].asInt() == action_active_pilot || _complexValue[i][ACTION::strActionID].asInt() == action_add_pilot)
			{
				for (unsigned j = 0; j < _complexValue[i][ACTION::strPilotActiveList].size(); ++j)
					_pilots.push_back(_complexValue[i][ACTION::strPilotActiveList][j].asInt());
			}
			if (_complexValue[i][ACTION::strActionID].asInt() == action_add_item && _complexValue[i][ACTION::addItemIDStr].asInt() >= 13535
				&& _complexValue[i][ACTION::addItemIDStr].asInt() <= 13555)
			{
				_crystal_limit = true;
			}
		}

		_total_num = info["num"].asInt();
		_limit = info["buy_limit"].asInt();
		_remain_num = _total_num;
	}

	void commodity::load(const mongo::BSONElement& info)
	{
		_id = info["commodity_id"].Int();
		_name = info["commodity_name"].String();
		_price.load(info["price"]);
		vector<mongo::BSONElement> ele = info["items"].Array();
		for (unsigned idx = 0; idx < ele.size(); ++idx)
		{
			Json::Reader reader;
			Json::Value temp;
			reader.parse(ele[idx].Obj().jsonString(), temp);
			_simpleValue.append(temp);
		}
		_complexValue = gm_tools_mgr.simple2complex(_simpleValue);
		_crystal_limit = false;
		for (unsigned i = 0; i < _complexValue.size(); ++i)
		{
			if (_complexValue[i][ACTION::strActionID].asInt() == action_active_pilot || _complexValue[i][ACTION::strActionID].asInt() == action_add_pilot)
			{
				for (unsigned j = 0; j < _complexValue[i][ACTION::strPilotActiveList].size(); ++j)
					_pilots.push_back(_complexValue[i][ACTION::strPilotActiveList][j].asInt());
			}
			if (_complexValue[i][ACTION::strActionID].asInt() == action_add_item && _complexValue[i][ACTION::addItemIDStr].asInt() >= 13535
				&& _complexValue[i][ACTION::addItemIDStr].asInt() <= 13555)
			{
				_crystal_limit = true;
			}
		}
		_total_num = info["num"].Int();
		_limit = info["buy_limit"].Int();
		_remain_num = info["remain_num"].Int();
	}

	mongo::BSONObj commodity::toBSON() const
	{
		mongo::BSONObjBuilder obj;
		obj << "commodity_id" << _id << "commodity_name" << _name
			<< "price" << _price.toBSON() << "num" << _total_num << "buy_limit" << _limit
			<< "remain_num" << _remain_num;
		mongo::BSONArrayBuilder array_obj;
		for (unsigned i = 0; i < _simpleValue.size(); ++i)
		{
// 			string temp = _simpleValue[i].toStyledString();
// 			temp = commom_sys.tighten(temp);
			string temp = commom_sys.json2string(_simpleValue[i]);
			mongo::BSONObj obj_temp = mongo::fromjson(temp);
			array_obj.append(obj_temp);
		}
		obj << "items" << array_obj.arr();
		return obj.obj();
	} 

	void commodity::infoValue(Json::Value& info)
	{
		info.append(_id);
		info.append(_name);
		info.append(_price.getValue());
		info.append(_simpleValue);
		info.append(_total_num);
		info.append(_limit);
	}

	void commodity::gmInfoValue(Json::Value& info)
	{
		info["commodity_id"] = _id;
		info["commodity_name"] = _name;
		info["price"] = _price.getValue();
		info["items"] = _simpleValue;
		info["num"] = _total_num;
		info["buy_limit"] = _limit;
	}

	bool commodity::isPriceLimit(playerDataPtr d)
	{
		switch (_price._type)
		{
			case 0:
				return d->Base.getSilver() < _price._cost;
			case 1:
				return d->Base.getAllGold() < _price._cost;
			case 2:
				return d->Base.getKeJi() < _price._cost;
			case 3:
				return d->Base.getWeiWang() < _price._cost;
			case 4:
				return d->Base.getGold() < _price._cost;
			default:
				return true;
		}
	}

	bool commodity::checkPilots(playerDataPtr d)
	{
		for (unsigned i = 0; i < _pilots.size(); ++i)
		{
			if (d->Pilots.checkPilotActive(_pilots[i]))
				return false;
		}
		return true;
	}

	void commodity::doPrice(playerDataPtr d)
	{
		switch (_price._type)
		{
			case 0:
				d->Base.alterSilver(0 - _price._cost);
				break;
			case 1:
				d->Base.alterBothGold(0 - _price._cost);
				break;
			case 2:
				d->Base.alterKeJi(0 - _price._cost);
				break;
			case 3:
				d->Base.alterWeiWang(0 - _price._cost);
				break;
			case 4:
				d->Base.alterGold(0 - _price._cost);
				break;
			default:
				break;
		}
	}

	bool commodity::isBuyLimit(playerDataPtr d)
	{
		return d->PlayerMarket.getNum(market_sys.getKeyId(), _id) >= _limit;
	}

	int commodity::getItems(playerDataPtr d)
	{
		if (isPriceLimit(d))
			return 2;
		if (_remain_num <= 0)
			return 3;
		if (isBuyLimit(d))
			return 4;
		unsigned now = na::time_helper::get_current_time();
		if (now < d->PlayerMarket.getCd(market_sys.getKeyId(), _id))
			return 5;

		if (!checkPilots(d))
			return 6;

		if (_crystal_limit && d->Base.getLevel() < 36)
			return 8;

		Json::Value param;
		actionResult result = actionFormat::actionDo(d, _complexValue, param);
		if (result.resultCode != 0 && result.breakID == action_add_item)
		{
			email_sys.sendSystemEmailCommon(d, email_type_system_attachment, email_team_system_bag_full_market, Json::Value::null, _simpleValue);
			market_system::_packageFull = true;
		}

		doPrice(d);
		d->PlayerMarket.alterNum(_id);
		--_remain_num;
		marketLog(d, (int)market_system::_log_deal, "", "", _id, _name, _price._type, _price._cost, _remain_num, _simpleValue.toStyledString());
		return 0;
	}

	market_system* const market_system::marketSys = new market_system;
	bool market_system::_packageFull = false;

	void market_system::initData()
	{
		objCollection objs = db_mgr.Query(market_info_db_str);
		if (objs.empty() || objs[0u]["key_id"].Int() == -1)
		{
			package();
			return;
		}

		const mongo::BSONObj& obj = objs[0u];
		_key_id = obj["key_id"].Int();
		_next_update_time = obj["next_update_time"].Int();
		vector<mongo::BSONElement> ele = obj["activity_time"].Array();
		for (unsigned i = 0; i < ele.size(); ++i)
			_activity_time[i] = ele[i].Int();
		ele = obj["business_time"].Array();
		for (unsigned i = 0; i < ele.size(); ++i)
			_business_time[i] = ele[i].Int();
		_business_time_work[0] = _business_time[0] / 100 * na::time_helper::HOUR + (_business_time[0] % 100) * na::time_helper::MINUTE;
		_business_time_work[1] = _business_time[1] / 100 * na::time_helper::HOUR + (_business_time[1] % 100) * na::time_helper::MINUTE;
		ele = obj["commodities"].Array();
		for (unsigned i = 0; i < ele.size(); ++i)
		{
			commodity com;
			com.load(ele[i]);
			_cMap.insert(make_pair(com.getId(), com));
		}
		package();
		_updateFlag = true;
		Timer::AddEventTickTime(boostBind(market_system::tickUpdate, this), _next_update_time);
		_timer_id = Timer::AddEventTickTimeID(boostBind(market_system::tickClose, this), _activity_time[1]);
	}

	void market_system::gmMarketUpdateReq(msg_json& m, Json::Value& r)
	{
		r[msgStr][1u] = _gmCache;
		//LogI << "update: " << _gmCache.toStyledString() << LogEnd;
		Return(r, 0);
	}

	void market_system::gmMarketModifyReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		const Json::Value& temp = js_msg[1u];
		//LogI << "modify: " << temp.toStyledString() << LogEnd;
		if (temp == Json::nullValue)
			_key_id = -1;
		else
		{			
			_key_id = temp["key_id"].asInt();
			_activity_time[0] = temp["activity_time"][0u].asUInt() + na::time_helper::timeZone() * na::time_helper::HOUR;
			_activity_time[1] = temp["activity_time"][1u].asUInt() + na::time_helper::timeZone() * na::time_helper::HOUR;
			_business_time[0] = temp["business_time"][0u].asInt();
			_business_time[1] = temp["business_time"][1u].asInt();
			_business_time_work[0] = _business_time[0] / 100 * na::time_helper::HOUR + (_business_time[0] % 100) * na::time_helper::MINUTE;
			_business_time_work[1] = _business_time[1] / 100 * na::time_helper::HOUR + (_business_time[1] % 100) * na::time_helper::MINUTE;
			const Json::Value& commodities = temp["commodities"];
			_cMap.clear();
			for (unsigned i = 0; i < commodities.size(); ++i)
			{
				commodity com;
				com.load(commodities[i]);
				_cMap.insert(make_pair(com.getId(), com));
			}
		}
		if (_timer_id != -1)
		{
			Timer::RemoveEvent(_timer_id);
			_timer_id = -1;
		}
		_timer_id = Timer::AddEventTickTimeID(boostBind(market_system::tickClose, this), _activity_time[1]);
		if (!_updateFlag)
		{
			Timer::AddEventTickTime(boostBind(market_system::tickUpdate, this), _next_update_time);
			_updateFlag = true;
		}

		package();
		save();
		info(playerDataPtr());
		Return(r, 0);
	}

	void market_system::infoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		info(d);
	}

	void market_system::updateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		update(d);
	}

	void market_system::dealReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int key_id = js_msg[0u].asInt();
		int item_id = js_msg[1u].asInt();

		unsigned now = na::time_helper::get_current_time();
		struct tm t = na::time_helper::toTm(now);
		unsigned secs = t.tm_hour  * na::time_helper::HOUR + t.tm_min * na::time_helper::MINUTE + t.tm_sec;
		if (secs < _business_time_work[0] || secs > _business_time_work[1])
			Return(r, 7);

		if (key_id != _key_id)
		{
			info(d);
			Return(r, 1);
		}

		commodityMap::iterator iter = _cMap.find(item_id);
		if (iter == _cMap.end())
		{
			info(d);
			Return(r, 1);
		}

		int result = iter->second.getItems(d);
		if (result == 0)
			save();

		update(d);
		if (_packageFull)
		{
			_packageFull = false;
			r[msgStr][1u] = 1;
		}
		Return(r, result);
	}

	void market_system::info(playerDataPtr d)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = _clientCache;
		if (d == playerDataPtr())
			player_mgr.sendToAll(gate_client::market_info_resp, msg);
		else
			player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::market_info_resp, msg);
	}

	void market_system::update(playerDataPtr d)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		if (_clientCache == Json::nullValue)
			msg[msgStr][1u] = Json::nullValue;
		else
		{
			msg[msgStr][1u]["kid"] = _key_id;
			msg[msgStr][1u]["cmd"] = Json::arrayValue;
			Json::Value& ref = msg[msgStr][1u]["cmd"];
			for (commodityMap::iterator iter = _cMap.begin(); iter != _cMap.end(); ++iter)
			{
				Json::Value temp;
				temp.append(iter->first);
				temp.append(d->PlayerMarket.getCd(_key_id, iter->first));
				temp.append(iter->second.getRemainNum());
				temp.append(d->PlayerMarket.getNum(_key_id, iter->first));
				ref.append(temp);
			}
		}
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::market_update_resp, msg);
	}

	void market_system::package()
	{
		_gmCache = Json::nullValue;
		_clientCache = Json::nullValue;
		_clientCache["kid"] = _key_id;
		if (_key_id == -1)
			return;

		_gmCache["key_id"] = _key_id;
		_gmCache["activity_time"].append(_activity_time[0] - na::time_helper::timeZone() * na::time_helper::HOUR);
		_gmCache["activity_time"].append(_activity_time[1] - na::time_helper::timeZone() * na::time_helper::HOUR);
		_gmCache["business_time"].append(_business_time[0]);
		_gmCache["business_time"].append(_business_time[1]);
		_gmCache["commodities"] = Json::arrayValue;
		Json::Value& ref_1 = _gmCache["commodities"];
		for (commodityMap::iterator iter = _cMap.begin(); iter != _cMap.end(); ++iter)
		{
			Json::Value temp;
			iter->second.gmInfoValue(temp);
			ref_1.append(temp);
		}

		_clientCache["at"].append(_activity_time[0]);
		_clientCache["at"].append(_activity_time[1]);
		_clientCache["bt"].append(_business_time[0]);
		_clientCache["bt"].append(_business_time[1]);
		_clientCache["cmd"] = Json::arrayValue;
		Json::Value& ref_2 = _clientCache["cmd"];
		for (commodityMap::iterator iter = _cMap.begin(); iter != _cMap.end(); ++iter)
		{
			Json::Value temp;
			iter->second.infoValue(temp);
			ref_2.append(temp);
		}
	}

	void market_system::tickUpdate()
	{
		if (_key_id == -1)
		{
			_updateFlag = false;
			return;
		}
		for (commodityMap::iterator iter = _cMap.begin(); iter != _cMap.end(); ++iter)
			iter->second.resetNum();
		_next_update_time = na::time_helper::get_next_update_time(na::time_helper::get_current_time());
		save();
		Timer::AddEventTickTime(boostBind(market_system::tickUpdate, this), _next_update_time);
	}

	void market_system::tickClose()
	{
		_key_id = -1;
		_timer_id = -1;
		package();
		save();
		info(playerDataPtr());
	}

	void market_system::save()
	{
		mongo::BSONObj key = BSON("index" << 0);
		mongo::BSONObjBuilder obj;
		obj << "index" << 0 << "key_id" << _key_id;
		if (_key_id != -1)
		{
			obj << "next_update_time" << _next_update_time;
			{
				mongo::BSONArrayBuilder array_temp;
				array_temp.append(_activity_time[0]);
				array_temp.append(_activity_time[1]);
				obj << "activity_time" << array_temp.arr();
			}
			{
				mongo::BSONArrayBuilder array_temp;
				array_temp.append(_business_time[0]);
				array_temp.append(_business_time[1]);
				obj << "business_time" << array_temp.arr();
			}
			{
				mongo::BSONArrayBuilder array_temp;
				for (commodityMap::iterator iter = _cMap.begin(); iter != _cMap.end(); ++iter)
					array_temp.append(iter->second.toBSON());
				obj << "commodities" << array_temp.arr();
			}
		}
		db_mgr.save_mongo(market_info_db_str, key, obj.obj());
	}
}