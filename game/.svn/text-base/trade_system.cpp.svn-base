#include "trade_system.h"
#include "timmer.hpp"
#include "response_def.h"
#include "playerManager.h"
#include "script.hpp"
#include "guild_system.h"
#include "mineHeader.h"
#include "commom.h"
#include "helper.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "record_system.h"
#include "chat_system.h"
#include "email_system.h"
#include "action_def.h"

namespace gg
{
	const static string strBuyDir = "./report/temp/trade/buy/";
	const static string strSaleDir = "./report/temp/trade/sale/";
	const static string strRichDir = "./report/temp/trade/rich";
	const static string strRankTodayDir = "./report/temp/trade/rank_today";
	const static string strRankHistoryDir = "./report/temp/trade/rank_history";

	static double BuySaleTimes = 1.0;
	static int EventTickID = -1;
	static bool ResBool = false;
	static Json::Value userdata = Json::nullValue;

	const static unsigned planetNum = 15;
	const static int eachPlanetNum = planetNum / 3;
	const static int planet[planetNum] =
	{
		0, 1, 2, 3, 4,
		1000, 1001, 1002, 1003, 1004,
		2000, 2001, 2002, 2003, 2004
	};
	static int EventPlanet[planetNum] =
	{
		0, 1, 2, 3, 4,
		1000, 1001, 1002, 1003, 1004,
		2000, 2001, 2002, 2003, 2004
	};
	const static int syncEventNum = 3;
	const static unsigned eventRandomNum = TradeEventCount * syncEventNum;
	static int EventRandom[eventRandomNum] =
	{
		tonghuopengzhang, tonghuopengzhang, tonghuopengzhang,
		xinpinrexiao, xinpinrexiao, xinpinrexiao,
		zhengfufuchi, zhengfufuchi, zhengfufuchi,
		haidaotuxi, haidaotuxi, haidaotuxi,
		jingweixuncha, jingweixuncha, jingweixuncha,
		tonghuojinsuo, tonghuojinsuo, tonghuojinsuo,
		fengmishangpin, fengmishangpin, fengmishangpin
	};
	static unsigned EventLastTime[TradeEventCount] =
	{
		1800,
		1800,
		900,
		1800,
		1800,
		1800,
		1800
	};
	const static int planetLimit[planetNum] =
	{
		20, 31, 41, 61, 81,
		20, 31, 41, 61, 81,
		20, 31, 41, 61, 81
	};

	const static int goodsList[tradeItemNum] =
	{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29
	};

	const static int itemIDBegin = 13003;
	const static int itemIDEnd = 13007;

	// 		item_itemName_13003	加速令
	// 		item_itemName_13004	侦查令
	// 		item_itemName_13005	防税令
	// 		item_itemName_13006	威慑令
	// 		item_itemName_13007	议价令

	bool planetTrade::item::Vaild()
	{
		return itemID >= 0 && itemID < tradeItemNum;
	}

	void planetTrade::TickPrice()
	{
		ForEach(itemMap, it, buy)
		{
			item& cItem = it->second;
			cItem.randomPrice();
			trade_sys.AddBuyPrice(cItem.itemID, cItem._Price());
		}
		ForEach(itemMap, it, sale)
		{
			item& cItem = it->second;
			cItem.randomPrice();
			trade_sys.AddSalePrice(planetID, cItem.itemID, cItem._modulePrice());
		}
	}

	Json::Value planetTrade::toJson()
	{
		Json::Value dataJson = Json::objectValue;
		do
		{
			dataJson["pid"] = planetID;
			Json::Value& buyJson = dataJson["buy"];
			Json::Value& saleJson = dataJson["sale"];
			buyJson = Json::arrayValue;
			saleJson = Json::arrayValue;
			ForEach(itemMap, it, buy)
			{
				item& cItem = it->second;
				Json::Value itemJson;
				itemJson["id"] = cItem.itemID;
				itemJson["p"] = cItem._Price();
				buyJson.append(itemJson);
			}
			ForEach(itemMap, it, sale)
			{
				item& cItem = it->second;
				Json::Value itemJson;
				itemJson["id"] = cItem.itemID;
				itemJson["p"] = cItem._modulePrice();
				saleJson.append(itemJson);
			}
		} while (false);
		return dataJson;
	}

	mongo::BSONObj planetTrade::toBson()
	{
		mongo::BSONObjBuilder builder;
		mongo::BSONArrayBuilder buyBuilder, saleBuilder;
		ForEach(itemMap, it, buy)
		{
			item& cItem = it->second;
			buyBuilder << BSON("id" << cItem.itemID << "p" << cItem._Price());
		}
		ForEach(itemMap, it, sale)
		{
			item& cItem = it->second;
			saleBuilder << BSON("id" << cItem.itemID << "p" << cItem._Price());
		}
		builder << "key" << planetID << "buy" << buyBuilder.arr() << "sale" << saleBuilder.arr();
		return builder.obj();
	}

	planetTrade::planetTrade(Json::Value& config, mongo::BSONObj& obj) :
		planetID(config["planetID"].asInt()), allowLevel(config["allowLevel"].asInt())
	{
		buy.clear();
		sale.clear();
		mongo::BSONObj emptyBson;
		{
			vector<mongo::BSONElement> elems;
			if (!obj.isEmpty())
			{
				checkNotEoo(obj["buy"])
				{
					elems = obj["buy"].Array();
				}
			}

			for (unsigned i = 0; i < config["buy"].size(); ++i)
			{
				Json::Value& buyJson = config["buy"][i];
				int pos = buyJson["planetID"].asInt();
				if (pos != planetID)continue;
				buyJson["module"] = buyJson["module"][0u].asDouble();
				if (i < elems.size())
				{
					mongo::BSONObj elemObj = elems[i].Obj();
					buy.insert(itemMap::value_type(buyJson["itemID"].asInt(),
						item(buyJson, elemObj)));
				}
				else
				{
					buy.insert(itemMap::value_type(buyJson["itemID"].asInt(),
						item(buyJson, emptyBson)));
				}
			}
		}

				{
					vector<mongo::BSONElement> elems;
					if (!obj.isEmpty())
					{
						checkNotEoo(obj["sale"])
						{
							elems = obj["sale"].Array();
						}
					}

					for (unsigned i = 0, j = 0; i < config["sale"].size(); ++i)
					{
						Json::Value& saleJson = config["sale"][i];
						int itemID = saleJson["itemID"].asInt();
						if (buy.find(itemID) != buy.end())continue;
						int pos = saleJson["planetID"].asInt();
						int dst = distance(pos);
						unsigned lsize = saleJson["module"].size();
						if (dst < 0 || dst >= (int)lsize)dst = 0;
						saleJson["module"] = saleJson["module"][dst];
						if (i < elems.size())
						{
							mongo::BSONObj elemObj = elems[j].Obj();
							++j;
							sale.insert(itemMap::value_type(itemID,
								item(elemObj, saleJson)));
						}
						else
						{
							sale.insert(itemMap::value_type(itemID,
								item(emptyBson, saleJson)));
						}
					}
				}
	}

	trade_system* const trade_system::trade_pointer = new trade_system();

	void trade_system::tradeFrist(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		++d->Trade.reqTimes;
		r[msgStr][1u] = d->Trade.reqTimes();
		Return(r, 0);
	}

	void trade_system::tradeUpdate(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		helper_mgr.insertUpdate(&d->Trade);
		//d->Trade.update();
	}

	void trade_system::tradeItemUpdate(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->TradeItem.update();
	}

	Json::Value trade_system::PackagePlanetEvent(const int level, bool seeall, const int Fromitem /* = -1 */)
	{
		Json::Value dataJson = Json::objectValue;
		dataJson[updateFromStr] = State::getState();
		dataJson["fid"] = Fromitem;
		unsigned now = na::time_helper::get_current_time();
		unsigned i = 0;
		Json::Value& eventJson = dataJson["pd"];
		eventJson = Json::arrayValue;
		ForEach(iuiMap, it, eventMap)
		{
			const Event& cEvent = it->second;
			if (now >= cEvent.endTime)continue;
			const planetTrade& cPlanet = getPlanet(it->first);
			if (cPlanet.allowLevel > level)continue;
			Json::Value& planetEvent = eventJson[i];
			if (seeall)
			{
				planetEvent.append(it->first);
				planetEvent.append(cEvent.ID);
				planetEvent.append(cEvent.eventParams);
			}
			else
			{
				planetEvent.append(-1);
				planetEvent.append(-1);
				planetEvent.append(Json::objectValue);
			}
			planetEvent.append(cEvent.endTime);
			++i;
		}
		return dataJson;
	}

	bool trade_system::hasEvent(const int level)
	{
		unsigned now = na::time_helper::get_current_time();
		int num = 0;
		ForEach(iuiMap, it, eventMap)
		{
			const Event& cEvent = it->second;
			if (now >= cEvent.endTime)continue;
			const planetTrade& cPlanet = getPlanet(it->first);
			if (cPlanet.allowLevel > level)continue;
			++num;
		}
		return num > 0;
	}

	void trade_system::tradeCenterUpdate(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < 31)Return(r, -1);
		if (!d->Guild.hasJoinGuild())Return(r, -1);//没有加入星盟
		Json::Value& eventJson = r[msgStr][1u];
		bool bE = d->Trade.hasBuff(zhencha);
		eventJson = PackagePlanetEvent(d->Base.getLevel(), bE);
		Return(r, 0);
	}

	planetTrade& trade_system::getPlanet(const int planetID)
	{
		static planetTrade NullPlanet;
		planetMap::iterator it = ownPlanet.find(planetID);
		if (it == ownPlanet.end())return NullPlanet;
		return it->second;
	}

	const static string strLogTrade = "log_trade_task";
	void trade_system::acceptTask(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < 31)Return(r, -1);
		if (!d->Guild.hasJoinGuild())Return(r, -1);//没有加入星盟
		if (d->Trade.hasTask())Return(r, 1);//已经有任务
		if (d->Trade.getRWTimes() >= 5)Return(r, 3);//每天只可以完成5次任务
		CallAndGetBool(mLua::Root("trade")["AcceptTask"](d->Base.getLevel()), ret);
		if (!ret)Return(r, 2);//没有合适的任务
		int start = mLua::PickMultiResult<int>(1);
		int end = mLua::PickMultiResult<int>(2);
		playerTrade& Trade = d->Trade;
		Trade.TaskState = 1;//顺序很重要
		Trade.PassVoucher = end;
		Trade.Voucher = start;
		StreamLog::Log(strLogTrade, d, Trade.Voucher(), Trade.PassVoucher());
		Return(r, 0);
	}

	void trade_system::insertSimData(simPlane sP)
	{
		bool insert = false;
		for (unsigned i = 0; i < simData.size(); ++i)
		{
			if (simData[i] == sP)
			{
				insert = true;
				simData[i] = sP;
			}
		}
		if (!insert)
		{
			simData.push_back(sP);
		}
		if (simData.size() > 21)
		{
			simData.erase(simData.begin());
		}
	}

	void trade_system::cancelTask(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (!d->Trade.hasTask())Return(r, 1);//没有任务
		playerTrade& Trade = d->Trade;
		Trade.TaskState = 0;
		Trade.Voucher = 0;
		Trade.PassVoucher = 0;
		d->TradeItem.clearBag();
		StreamLog::Log(strLogTrade, d, Trade.Voucher(), Trade.PassVoucher());
		Return(r, 0);
	}

	void trade_system::completeTask(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		Guild::guildPtr gPtr = guild_sys.getGuild(d->Guild.getGuildID());
		if (gPtr == NULL)Return(r, -1);
		playerTrade& Trade = d->Trade;
		if (!Trade.TaskComplete())Return(r, 1);//还没有任务
		if (Trade.getRWTimes() >= 5)Return(r, 2);//完成任务了,但是今天可以领取的奖励次数已经用完了
		CallAndGetBool(mLua::Root("trade")["completeTask"](d->Base.getLevel(), Trade.Voucher(), Trade.PassVoucher(), Trade.getRWTimes()), ret);
		if (!ret)Return(r, 3);//没有对应的奖励
		int xmjy = mLua::PickMultiResult<int>(1);
		int silver = mLua::PickMultiResult<int>(2);
		guild_sys.donateDefault(d, xmjy);
		d->Base.alterSilver(silver);
		int defaultSC = d->Guild.getFirst();
		StreamLog::LogV(strLogTrade, d, Trade.Voucher(), Trade.PassVoucher(), -1,
			boost::lexical_cast<string, int>(xmjy), boost::lexical_cast<string, int>(xmjy),
			boost::lexical_cast<string, int>(silver), gPtr->guildName, 
			boost::lexical_cast<string, int>(defaultSC),
			boost::lexical_cast<string, int>(gPtr->getScienceData(defaultSC).LV));
		Trade.tickRWTimes();
		Trade.PassTimes += 1;
		Trade.TaskState = 0;
		Trade.VoucherHistory += Trade.Voucher();
		Trade.Voucher = 0;
		Trade.PassVoucher = 0;
		d->TradeItem.clearBag();
		if (d->Item.canAddItem(13008, 2))
		{
			d->Item.addItem(13008, 2, false);
		}
		else
		{
			Json::Value item, mailValue;
			item[email::actionID] = action_add_item;
			item[email::itemID] = 13008;
			item[email::addNum] = 2;
			mailValue.append(item);
			email_sys.sendSystemEmailCommon(d, email_type_system_attachment, email_team_system_trade_reward_full, Json::Value::null, mailValue);
		}
		r[msgStr][1u].append(xmjy);
		r[msgStr][1u].append(xmjy);
		r[msgStr][1u].append(silver);
		r[msgStr][1u].append(2);
		Return(r, 0);
	}

	void trade_system::FastTask(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Vip.getVipLevel() < 3)Return(r, 4);//vip等级不足
		Guild::guildPtr gPtr = guild_sys.getGuild(d->Guild.getGuildID());
		if (gPtr == NULL)Return(r, -1);
		playerTrade& Trade = d->Trade;
		if (Trade.getRWTimes() >= 5)Return(r, 2);//今日无法继续完成任务了
		if (Trade.TaskComplete())Return(r, 1);//任务已经完成无需快完成
		int V = Trade.PassVoucher();
		int PassV = Trade.PassVoucher();
		if (!Trade.hasTask())
		{
			CallAndGetBool(mLua::Root("trade")["AcceptTask"](d->Base.getLevel()), ret);
			if (!ret)Return(r, 6);//没有合适的任务
			V = mLua::PickMultiResult<int>(2);
			PassV = V;
		}
		if (d->Base.getAllGold() < 50)Return(r, 5);//钻石不足
		CallAndGetBool(mLua::Root("trade")["completeTask"](d->Base.getLevel(), Trade.Voucher(), Trade.PassVoucher(), Trade.getRWTimes()), ret);
		if (!ret)Return(r, 3);//没有对应的奖励
		int xmjy = mLua::PickMultiResult<int>(1);
		int silver = mLua::PickMultiResult<int>(2);
		guild_sys.donateDefault(d, xmjy);
		d->Base.alterSilver(silver);
		int defaultSC = d->Guild.getFirst();
		StreamLog::LogV(strLogTrade, d, V, PassV, -1,
			boost::lexical_cast<string, int>(xmjy), boost::lexical_cast<string, int>(xmjy),
			boost::lexical_cast<string, int>(silver), gPtr->guildName,
			boost::lexical_cast<string, int>(defaultSC),
			boost::lexical_cast<string, int>(gPtr->getScienceData(defaultSC).LV));
		Trade.tickRWTimes();
		Trade.PassTimes += 1;
		Trade.TaskState = 0;
		Trade.VoucherHistory += PassV;
		Trade.Voucher = 0;
		Trade.PassVoucher = 0;
		d->Base.alterBothGold(-50);
		d->TradeItem.clearBag();
		if(d->Item.canAddItem(13008, 2))
		{
			d->Item.addItem(13008, 2, false);
		}
		else
		{
			Json::Value item, mailValue;
			item[email::actionID] = action_add_item;
			item[email::itemID] = 13008;
			item[email::addNum] = 2;
			mailValue.append(item);
			email_sys.sendSystemEmailCommon(d, email_type_system_attachment, email_team_system_trade_reward_full, Json::Value::null, mailValue);
		}
		r[msgStr][1u].append(xmjy);
		r[msgStr][1u].append(xmjy);
		r[msgStr][1u].append(silver);
		r[msgStr][1u].append(2);
		Return(r, 0);
	}

	void trade_system::upgradePlane(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int num = js_msg[0u].asInt();
		
		if (!d->Item.canRemoveItem(13008, num))Return(r, 1);

		int oldLv = d->Trade.PlaneLevel();
		int add_exp = num * 100;
		add_exp = d->Trade.addExp(add_exp);

		d->Item.removeItem(13008, num);

		StreamLog::LogV(strLogTrade, d, oldLv, d->Trade.PlaneLevel(), -1,
			boost::lexical_cast<string, int>(add_exp), boost::lexical_cast<string, int>(num));

		r[msgStr][1u] = add_exp;
		Return(r, 0);
	}

	void trade_system::showVoucher(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (!d->Trade.TaskComplete())Return(r, 2);

		unsigned now = na::time_helper::get_current_time();
		if (now < d->Trade.showCD)Return(r, 1);//cd中
		d->Trade.showCD = now + 60;

		Json::Value bCast;
		bCast[msgStr][0u] = 0;
		bCast[msgStr][1u][senderChannelStr] = -1;
		bCast[msgStr][1u][chatBroadcastID] = BROADCAST::trade_voucher_show;
		const static string ParamListStr = "pl";
		bCast[msgStr][1u][ParamListStr].append(d->Base.getName());
		bCast[msgStr][1u][ParamListStr].append(d->Trade.Voucher());
		bCast[msgStr][1u][ParamListStr].append(d->Trade.PassVoucher());
		player_mgr.sendToAll(gate_client::chat_broadcast_resp, bCast);

		Return(r, 0);
	}

	//0无移动平偏差 1正方向 -1反方向
	static int direct(int pos, int des)
	{
		if (pos == des)return 0;
		int current_kingdom = pos / 1000;
		int des_kingdom = des / 1000;
		if (current_kingdom != des_kingdom)return -1;
		if (pos > des)return -1;
		return 1;
	}

	void trade_system::move(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (!d->Guild.hasJoinGuild())Return(r, -1);//没有加入星盟
		if (d->Base.getLevel() < 31)Return(r, -1);
		if (d->Base.getSphereID() < 0 || d->Base.getSphereID() > 2)Return(r, -1);
		ReadJsonArray;
		int des = js_msg[0u].asInt();
		const planetTrade& desPlanet = getPlanet(des);
		if (desPlanet.allowLevel > d->Base.getLevel())Return(r, 3);
		int des_kingdom = des / 1000;
		int des_real_pos = des % 1000;
		if (des_kingdom < 0 || des_kingdom > 2 || des_real_pos < 0 || des_real_pos >= eachPlanetNum)Return(r, -1);
		int current_pos = d->Trade.getPosition();
		int current_des = d->Trade.Destination();
		if (current_des == des)Return(r, 1);//目前指定的就是这个目的地
		unsigned now = na::time_helper::get_current_time();
		if (now < d->Trade.FlyTime())
		{
			d->Trade.FlyTime.rawset(now);
		}
		bool old_state = d->Trade.Flying();
		if (old_state)
		{
			int current_direct = direct(current_pos, current_des);
			int will_direct = direct(current_pos, des);
			if (0 == current_direct)Return(r, -1);
			if (will_direct == 0)
			{
				will_direct = -current_direct;
			}
			if (current_direct == will_direct)
			{
				int will_pos = playerTrade::moveToNext(current_pos, current_des);
				if (will_pos / 1000 != current_pos / 1000)
				{
					const unsigned drua = unsigned(TradeFlyStep / d->Trade.Speed());
					const unsigned half = drua / 2;
					unsigned leave = now - d->Trade.FlyTime();
					if (leave > half && current_des / 1000 != des / 1000)
					{
						if (TradeFlyStep <= leave)Return(r, -1);
						leave = TradeFlyStep - leave;
						d->Trade.FlyTime = now - leave;
						d->Trade.moveToNext();
					}
				}
			}
			else
			{
				unsigned leave = now - d->Trade.FlyTime();
				if (TradeFlyStep <= leave)Return(r, -1);
				leave = TradeFlyStep - leave;
				d->Trade.FlyTime = now - leave;
				d->Trade.moveToNext();
			}
		}
		else
		{
			if (current_pos == des)Return(r, 2);//已经是当前目的地
			d->Trade.FlyTime = now;
		}
		if (old_state)d->Trade.Speed = 1.0;
		d->Trade.Destination = des;
		d->Trade.orgPosition = d->Trade.Position();
		insertSimData(simPlane(d->playerID, d->Base.getName(), d->Trade.PlaneLevel(),  d->Base.getSphereID(), d->Trade.Position(), d->Trade.Destination(), d->Trade.FlyTime()));
		Return(r, 0);
	}

	void trade_system::buy(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < 31)Return(r, -1);
		if (!d->Guild.hasJoinGuild())Return(r, -1);//没有加入星盟
		playerTrade& Trade = d->Trade;
		if (!Trade.doTask())Return(r, 1);//任务完成不能买
		if (Trade.Flying())Return(r, 2);//飞行中无法买卖
		ReadJsonArray;
		int itemID = js_msg[0u].asInt();
		int num = js_msg[1u].asInt();
		if (!d->TradeItem.canAddItem(itemID, num))Return(r, 3);//背包空间不足
		int pos = Trade.getPosition();
		planetTrade& planet = getPlanet(pos);
		if (planet.nullPlanet())Return(r, 4);//处于一个未知的位置
		runPlanetEvent(d);
		int silver = int(planet.buyItem(itemID) * BuySaleTimes) * num;
		if (silver <= 0)Return(r, 5);//无法在这个星球购买这个道具
		if (silver > Trade.Voucher())Return(r, 6);//贸易币不足
		d->TradeItem.addItem(itemID, num, silver);
		Trade.Voucher -= silver;
		Return(r, 0);
	}

	void trade_system::sale(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < 31)Return(r, -1);
		if (!d->Guild.hasJoinGuild())Return(r, -1);//没有加入星盟
		playerTrade& Trade = d->Trade;
		if (!Trade.hasTask())Return(r, 1);//没有任务不能卖东西
		if (Trade.Flying())Return(r, 2);//飞行中无法买卖
		ReadJsonArray;
		int itemID = js_msg[0u].asInt();
		int num = js_msg[1u].asInt();
		if (num < 1)Return(r, -1);
		tradeItem& cItem = d->TradeItem.getItem(itemID);
		if (cItem.nullItem())Return(r, -1);
		if (cItem.Num() < num)Return(r, 3);//指定道具不够
		int pos = Trade.getPosition();
		planetTrade& planet = getPlanet(pos);
		if (planet.nullPlanet())Return(r, 4);//处于一个未知的位置
		userdata["item"] = itemID;
		runPlanetEvent(d);
		int silver = planet.saleItem(itemID);
		if (d->Trade.hasBuff(yijia))BuySaleTimes += 0.1;
		Params::Var& var = trade_sys.PlaneConfig[1u];
		BuySaleTimes += var[d->Trade.PlaneLevel()].asDouble();
		silver = int(silver * BuySaleTimes) * num;
		if (silver <= 0)Return(r, 5);//无法再这个星球贩卖这个道具
		cItem.numAlter(-num);
		Trade.Voucher += silver;
		r[msgStr][1u] = silver;
		Return(r, 0);
	}

	void trade_system::updateTable(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		//if (d->Trade.Flying())Return(r, 1);
		do{
			if (!d->Trade.hasBuff(zhencha) && hasEvent(d->Base.getLevel()))
			{
				r[msgStr][1u] = -1;
				r[msgStr][2u] = Json::objectValue;
				r[msgStr][3u] = 0;
				break;
			}
			ReadJsonArray;
			int planetID = js_msg[0u].asInt();
			Event pEve = getEvent(planetID);
			r[msgStr][1u] = pEve.ID;
			r[msgStr][2u] = pEve.eventParams;
			r[msgStr][3u] = pEve.endTime;
		} while (false);
		Return(r, 0);
	}

	void trade_system::park(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < 31)Return(r, -1);
		if (!d->Guild.hasJoinGuild())Return(r, -1);//没有加入星盟
		playerTrade& Trade = d->Trade;
		if (Trade.Flying())Return(r, 1);//飞行中无法买卖
		int pos = Trade.getPosition();
		planetTrade& planet = getPlanet(pos);
		if (planet.nullPlanet())Return(r, 2);//处于一个未知的位置
		runPlanetEvent(d);
//		d->TradeItem.update();
		Event eve = getEvent(pos);
		r[msgStr][1u] = planet.toJson();
		r[msgStr][2u].append(eve.ID);
		r[msgStr][2u].append(eve.eventParams);
		r[msgStr][2u].append(eve.endTime);
		r[msgStr][3u] = EventTickID;
		Return(r, 0);
	}

	void trade_system::runItemEvent(playerDataPtr player, const int itemID)
	{
		ResBool = false;
		HandlerIt it = HandlerMap.find(itemID);
		if (it == HandlerMap.end())return;
		it->second(player);
	}

	void trade_system::runPlanetEvent(playerDataPtr player)
	{
		BuySaleTimes = 1.0;
		EventTickID = -1;
		int pos = player->Trade.getPosition();
		const int eventID = getEvent(pos).ID;
		HandlerIt it = HandlerMap.find(eventID);
		if (it == HandlerMap.end())return;
		it->second(player);
		userdata = Json::nullValue;
	}

	void trade_system::useItem(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < 31)Return(r, -1);
		if (!d->Guild.hasJoinGuild())Return(r, -1);//没有加入星盟
		ReadJsonArray;
		int itemID = js_msg[0u].asInt();
		if (itemID < itemIDBegin || itemID > itemIDEnd)Return(r, -1);
		r[msgStr][1u] = itemID;
		if (!d->Item.canRemoveItem(itemID, 1))Return(r, 1);//道具不足
		runItemEvent(d, itemID);
		if (ResBool)
		{
			StreamLog::Log(strLogTrade, d, itemID, 1);
			d->Item.removeItem(itemID, 1);
			Return(r, 0);
		}
		Return(r, 2);//无法继续使用
	}

	void trade_system::Teleport(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < 31)Return(r, -1);
		if (!d->Guild.hasJoinGuild())Return(r, -1);//没有加入星盟
		if (d->Trade.Berth())Return(r, 1);//不需要清除CD
		int distance = planetTrade::distance(d->Trade.getPosition(), d->Trade.Destination());
		if (distance <= 0)Return(r, 1);//不需要清除CD
		int dis_seconds = int(distance * TradeFlyStep / d->Trade.Speed());
		unsigned now = na::time_helper::get_current_time();
		if (now > d->Trade.FlyTime())
		{ 
			int limit = now - d->Trade.FlyTime();
			if (limit >= dis_seconds)Return(r, -1);
			dis_seconds -= limit;
		}
		int cost = dis_seconds / 60;
		cost = dis_seconds % 60 ? cost + 1 : cost;
		if (cost > d->Base.getAllGold())Return(r, 2);//钻石不足
		d->Trade.Position = d->Trade.Destination();
		d->Trade.Speed = 1.0;
		d->Base.alterBothGold(-cost);
		Return(r, 0);
	}

	void trade_system::tradeUpdateSkill(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->TradeSkill.update();
	}

	void trade_system::updateSimData(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		r[msgStr][0u] = 0;
		Json::Value& simJson = r[msgStr][1u];
		simJson = Json::arrayValue;
		for (unsigned i = 0; i < simData.size(); ++i)
		{
			simPlane& sP = simData[i];
			if (sP.ID == d->playerID)continue;
			simJson.append(sP.toJson());
		}
	}

	void trade_system::priceCreate(bool force)
	{
		ForEach(planetMap, it, ownPlanet)
		{
			planetTrade& cTrade = it->second;
			cTrade.TickPrice();
		}
		if (!force)
		{
			PriceTick += 600;
			saveTick();
			Timer::AddEventTickTime(boost::bind(&trade_system::priceCreate, trade_system::trade_pointer, false), PriceTick);
		}
		savePrice();
		writePrice();
	}

	void trade_system::updategName(Rich key, const string name)
	{
		RichIter it = _RichList.find(key);
		if (it != _RichList.end())
		{
			Rich nRich(*it);
			nRich.gName = name;
			_RichList.erase(it);
			_RichList.insert(nRich);
			//writeRich(true);//立马更新富豪榜
			saveRich();
		}
	}

	void trade_system::updatepName(Rank key, const string name)
	{
		{
			RankIter it = _TodayList.find(key);
			if (it != _TodayList.end())
			{
				Rank nRank(key);
				nRank.playerName = name;
				_TodayList.erase(it);
				_TodayList.insert(nRank);
				saveRankToday();
			}
		}
		{
			RankIter it = _HistoryList.find(key);
			if (it != _HistoryList.end())
			{
				Rank nRank(key);
				nRank.playerName = name;
				_HistoryList.erase(it);
				_HistoryList.insert(nRank);
				saveRankHistory();
			}
		}
	}

	void trade_system::updatepName(Rich key, const string name)
	{
		RichIter it = _RichList.find(key);
		if (it != _RichList.end())
		{
			Rich nRich(key);
			nRich.pName = name;
			_RichList.erase(it);
			_RichList.insert(nRich);
			//writeRich(true);//立马更新富豪榜
			saveRich();
		}
	}

	void trade_system::insertRankHistory(const int playerID, Rank value)
	{
		bool update = false;
		for (RankIter it = _HistoryList.begin(); it != _HistoryList.end(); ++it)
		{
			const Rank& rank = *it;
			if (rank.playerID == playerID)
			{
				update = true;
				_HistoryList.erase(it);
				break;
			}
		};
		{
			update = true;
			_HistoryList.insert(value);
		};
		{
			do
			{
				if (_HistoryList.size() > 50)
				{
					update = true;
					_HistoryList.erase(--_HistoryList.end());
				}
				break;
			} while (true);
		};
		if (update)
		{
			saveRankHistory();
		}
	}

	void trade_system::insertRankToday(const int playerID, Rank value)
	{
		bool update = false;
		for (RankIter it = _TodayList.begin(); it != _TodayList.end();++it)
		{
			const Rank& rank = *it;
			if (rank.playerID == playerID)
			{
				update = true;
				_TodayList.erase(it);
				break;
			}
		};
		{
			update = true;
			_TodayList.insert(value);
		};
		{
			do
			{
				if (_TodayList.size() > 50)
				{
					update = true;
					_TodayList.erase(--_TodayList.end());
				}
				break;
			} while (true);
		};
		if (update)
		{
			saveRankToday();
		}
	}

	void trade_system::insertRich(const int playerID, Rich value)
	{
		bool update = false;
		for (RichIter it = _RichList.begin(); it != _RichList.end(); ++it)
		{
			const Rich& rich = *it;
			if (rich.playerID == playerID)
			{
				update = true;
				_RichList.erase(it);
				break;
			}
		};
		{
			update = true;
			_RichList.insert(value);
		};
		{
			do
			{
				if (_RichList.size() > 50)
				{
					update = true;
					_RichList.erase(--_RichList.end());
				}
				break;
			} while (true);
		};
		if (update)
		{
			saveRich();
		}
	}

	void trade_system::eventCreate(bool force)
	{
		eventMap.clear();

		commom_sys.random_array(EventPlanet, planetNum);
		commom_sys.random_array(EventRandom, eventRandomNum);

		unsigned now = na::time_helper::get_current_time();
		for (unsigned i = 0; i < 3; ++i)
		{
			int eventID = EventRandom[i];
			eventMap[EventPlanet[i]] = Event(eventID, now + EventLastTime[eventID - 1]);
			if (eventID == fengmishangpin)
			{
				planetTrade& planet = getPlanet(EventPlanet[i]);
				eventMap[EventPlanet[i]].eventParams["item"] = planet.randomSaleItemID();
			}
		}

		if (!force)
		{
			EventTick += 3600;
			saveTick();
			Timer::AddEventTickTime(boost::bind(&trade_system::eventCreate, trade_system::trade_pointer, false), EventTick);
		}
		saveEvent();
	}

#define refreshTickKey -100
#define planetEventKey -101
	const static string strDBSysTrade = "gl.trade_system";
	const static string strDBRichTrade = "gl.trade_rich";

	void trade_system::savePrice()
	{
		for (planetMap::iterator it = ownPlanet.begin(); it != ownPlanet.end(); ++it)
		{
			planetTrade& pTrade = it->second;
			mongo::BSONObj key = BSON("key" << pTrade.planetID);
			db_mgr.save_mongo(strDBSysTrade, key, pTrade.toBson());
		}
	}

	void trade_system::saveEvent()
	{
		mongo::BSONObj key = BSON("key" << planetEventKey);
		mongo::BSONObjBuilder builder;
		builder << "key" << planetEventKey;
		mongo::BSONArrayBuilder arrBuilder;
		for (iuiMap::iterator it = eventMap.begin(); it != eventMap.end(); ++it)
		{
			string pmstr = commom_sys.json2string(it->second.eventParams);
			arrBuilder << BSON("pid" << it->first << "id" << 
				it->second.ID << "tt" << it->second.endTime <<
				"pm" << mongo::fromjson(pmstr));
		}
		builder << "d" << arrBuilder.arr();
		db_mgr.save_mongo(strDBSysTrade, key, builder.obj());
	}

	void trade_system::saveTick()
	{
		mongo::BSONObj key = BSON("key" << refreshTickKey);
		mongo::BSONObjBuilder builder;
		builder << "key" << refreshTickKey <<
			"pt" << PriceTick << "et" << EventTick <<
			"rt" << rankTick;
		db_mgr.save_mongo(strDBSysTrade, key, builder.obj());
	}

	void trade_system::world_tonghuopengzhang(playerDataPtr player)
	{
		if (!player->Trade.hasTask())return;
		int planetID = player->Trade.getPosition();
		Event eve = getEvent(planetID);
		if (eve.ID != tonghuopengzhang)return;
		if (player->Trade.hasBuff(tonghuopengzhang))return;
		EventTickID = tonghuopengzhang;
		player->Trade.Voucher += 10000;
		player->Trade.addBuff(tonghuopengzhang, eve.endTime);
	}

	void trade_system::world_xinpinrexiao(playerDataPtr player)
	{
		if (!player->Trade.hasTask())return;
		if (player->Trade.Flying())return;
		int planetID = player->Trade.getPosition();
		if (player->Trade.getPosition() != planetID)return;
		Event eve = getEvent(planetID);
		if (eve.ID != xinpinrexiao)return;
		if (player->Trade.hasBuff(xinpinrexiao))return;
		EventTickID = xinpinrexiao;
		player->Trade.Voucher += 5000;
		player->Trade.addBuff(xinpinrexiao, eve.endTime);
	}

	void trade_system::world_zhengfufuchi(playerDataPtr player)
	{
		do
		{
			if (player->Trade.Flying())break;
			int planetID = player->Trade.getPosition();
			Event eve = getEvent(planetID);
			if (eve.ID != zhengfufuchi)break;
			BuySaleTimes = 1.05;
		} while (false);
		BuySaleTimes = 1.0;
	}

	void trade_system::world_haidaotuxi(playerDataPtr player)
	{
		if (!player->Trade.hasTask())return;
		if (player->Trade.Flying())return;
		int planetID = player->Trade.getPosition();
		if (player->Trade.getPosition() != planetID)return;
		Event eve = getEvent(planetID);
		if (eve.ID != haidaotuxi)return;
		if (player->Trade.hasBuff(weishe))return;//保护
		if (player->Trade.hasBuff(haidaotuxi))return;
		EventTickID = haidaotuxi;
		int v = player->Trade.Voucher();
		v = v - 10000 < 0 ? 0 : v - 10000;
		player->Trade.Voucher = v;
		player->Trade.addBuff(haidaotuxi, eve.endTime);
	}

	void trade_system::world_jingweixuncha(playerDataPtr player)
	{
		if (!player->Trade.hasTask())return;
		if (player->Trade.Flying())return;
		int planetID = player->Trade.getPosition();
		if (player->Trade.getPosition() != planetID)return;
		Event eve = getEvent(planetID);
		if (eve.ID != jingweixuncha)return;
		if (player->Trade.hasBuff(fangshui))return;
		if (player->Trade.hasBuff(jingweixuncha))return;
		EventTickID = jingweixuncha;
		int v = player->Trade.Voucher();
		v = v - 5000 < 0 ? 0 : v - 5000;
		player->Trade.Voucher = v;
		player->Trade.addBuff(jingweixuncha, eve.endTime);
	}

	void trade_system::world_tonghuojinsuo(playerDataPtr player)
	{
		do
		{
			if (player->Trade.Flying())break;
			int planetID = player->Trade.getPosition();
			Event eve = getEvent(planetID);
			if (eve.ID != zhengfufuchi)break;
			BuySaleTimes = 0.95;
		} while (false);
		BuySaleTimes = 1.0;
	}

	void trade_system::world_fengmishangpin(playerDataPtr player)
	{
		do
		{
			if (player->Trade.Flying())break;
			int planetID = player->Trade.getPosition();
			Event eve = getEvent(planetID);
			if (eve.ID != fengmishangpin)break;
			Json::Value config = eve.eventParams;
			if (userdata["item"].asInt() != config["item"].asInt())break;
			BuySaleTimes = 1.3;
		} while (false);
		BuySaleTimes = 1.0;
	}

	void trade_system::item_jiasu(playerDataPtr player)
	{
		if (player->Trade.Flying())return;//飞行状态无法使用加速令
		if (player->Trade.Speed > 1.0)return;//已经加速
		player->Trade.Speed *= 2;
		ResBool = true;
	}

	void trade_system::item_zhencha(playerDataPtr player)
	{
		player->Trade.addBuff(zhencha, 1800);
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1u] = PackagePlanetEvent(player->Base.getLevel(), true, 13004);
		player->sendToClient(gate_client::trade_center_event_update_resp, updateJson);
		ResBool = true;
	}

	void trade_system::item_fangshui(playerDataPtr player)
	{
		player->Trade.addBuff(fangshui, 3600);
		ResBool = true;
	}

	void trade_system::item_weishe(playerDataPtr player)
	{
		player->Trade.addBuff(weishe, 3600);
		ResBool = true;
	}

	void trade_system::item_yijia(playerDataPtr player)
	{
		player->Trade.addBuff(yijia, 1800);
		ResBool = true;
	}

	template<typename yType>
	bool isEcho(const yType& vec, const int playerID)
	{
		for (typename yType::const_iterator it = vec.begin(); it != vec.end(); ++it)
		{
			if (it->playerID == playerID)
			{
				return true;
			}
		};
		return false;
	}


#define TradeBind(Func) boost::bind(&trade_system::Func, trade_system::trade_pointer, _1)
	void trade_system::initData()
	{
		LogS << "init trade system ..." << LogEnd;

		{
			Json::Value val = na::file_system::load_jsonfile_val("./instance/trade/plane.json");
			Params::ArrayValue tmp(val);
			PlaneConfig = tmp;
		}

		//效果应用注册
		HandlerMap.clear();
		HandlerMap[tonghuopengzhang] = TradeBind(world_tonghuopengzhang);
		HandlerMap[xinpinrexiao] = TradeBind(world_xinpinrexiao);
		HandlerMap[zhengfufuchi] = TradeBind(world_zhengfufuchi);
		HandlerMap[haidaotuxi] = TradeBind(world_haidaotuxi);
		HandlerMap[jingweixuncha] = TradeBind(world_jingweixuncha);
		HandlerMap[tonghuojinsuo] = TradeBind(world_tonghuojinsuo);
		HandlerMap[fengmishangpin] = TradeBind(world_fengmishangpin);

		HandlerMap[13003] = TradeBind(item_jiasu);
		HandlerMap[13004] = TradeBind(item_zhencha);
		HandlerMap[13005] = TradeBind(item_fangshui);
		HandlerMap[13006] = TradeBind(item_weishe);
		HandlerMap[13007] = TradeBind(item_yijia);

		db_mgr.ensure_index(strTradeItemDB, BSON(playerIDStr << 1)); 
		db_mgr.ensure_index(strDBPlayerTrade, BSON(playerIDStr << 1));
		db_mgr.ensure_index(strDBTradeSkill, BSON(playerIDStr << 1));
		db_mgr.ensure_index(strDBSysTrade, BSON("key" << 1));
		db_mgr.ensure_index(strDBRichTrade, BSON("key" << 1));

		{
			mongo::BSONObj key = BSON("key" << "key");
			mongo::BSONObj obj = db_mgr.FindOne(strDBRichTrade, key);
			_RichList.clear();
			bool repair = false;
			if (!obj.isEmpty())
			{
				vector<mongo::BSONElement> elems = obj["val"].Array();
				for (unsigned i = 0; i < elems.size(); ++i)
				{
					mongo::BSONElement& el = elems[i];
					int playerID = el["pid"].Int();
					if (isEcho(_RichList, playerID))
					{
						repair = true;
						continue;
					}
					_RichList.insert(Rich(el["v"].Int(), el["pt"].Int(), (unsigned)el["ct"].Int(), el["pid"].Int(),
						el["pn"].String(), el["gn"].String(), el["kid"].Int()));
				}
			}
			if (repair)
			{
				saveRich();
			}
		};

		{
			mongo::BSONObj key = BSON("key" << "key_h");
			mongo::BSONObj obj = db_mgr.FindOne(strDBRichTrade, key);
			_HistoryList.clear();
			bool repair = false;
			if (!obj.isEmpty())
			{
				vector<mongo::BSONElement> elems = obj["val"].Array();
				for (unsigned i = 0; i < elems.size(); ++i)
				{
					mongo::BSONElement& el = elems[i];
					int playerID = el["pid"].Int();
					if (isEcho(_HistoryList, playerID))
					{
						repair = true;
						continue;
					}
					_HistoryList.insert(Rank(el["v"].Int(), (unsigned)el["ct"].Int(), el["pid"].Int(),
						el["pn"].String()));
				}
			}
			if (repair)
			{
				saveRankHistory();
			}
		};

		{
			mongo::BSONObj key = BSON("key" << "key_t");
			mongo::BSONObj obj = db_mgr.FindOne(strDBRichTrade, key);
			_TodayList.clear();
			bool repair = false;
			if (!obj.isEmpty())
			{
				vector<mongo::BSONElement> elems = obj["val"].Array();
				for (unsigned i = 0; i < elems.size(); ++i)
				{
					mongo::BSONElement& el = elems[i];
					int playerID = el["pid"].Int();
					if (isEcho(_TodayList, playerID))
					{
						repair = true;
						continue;
					}
					_TodayList.insert(Rank(el["v"].Int(), (unsigned)el["ct"].Int(), el["pid"].Int(),
						el["pn"].String()));
				}
			}
			if (repair)
			{
				saveRankToday();
			}
		};


		{
			eventMap.clear();
			mongo::BSONObj key = BSON("key" << planetEventKey);
			mongo::BSONObj obj = db_mgr.FindOne(strDBSysTrade, key);
			if (!obj.isEmpty())
			{
				vector<mongo::BSONElement> elems = obj["d"].Array();
				unsigned now = na::time_helper::get_current_time();
				for (unsigned i = 0; i < elems.size(); ++i)
				{
					mongo::BSONElement& el = elems[i];
					int id = el["id"].Int();
					unsigned tick_time = (unsigned)el["tt"].Int();
					if (tick_time <= now)continue;
					Event cEvent(id, tick_time);
					checkNotEoo(el["pm"])
					{
						cEvent.eventParams = commom_sys.string2json(el["pm"].Obj().jsonString());
					}
					eventMap[el["pid"].Int()] = cEvent;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//历史价格
		na::file_system::try_create_directory(strBuyDir);
		buyHistory.clear();
		for (int i = 0; i < tradeItemNum; ++i)
		{
			int itemID = goodsList[i];
			Json::Value value = zip_read_json(strBuyDir + boost::lexical_cast<string, int>(itemID));
			if (value == Json::nullValue)continue;
			priceVec vec;
			for (unsigned i = 0; i < value.size(); ++i)
			{
				vec.push_back(value[i].asInt());
			}
			buyHistory[itemID] = vec;
		}

		saleHistory.clear();
		for (int i = 0; i < planetNum; ++i)
		{
			int planetID = planet[i];
			string rootPath = strSaleDir + boost::lexical_cast<string, int>(planetID)+"/";
			na::file_system::try_create_directory(rootPath);
			jsonMap& cMap = saleHistory[planetID];
			for (unsigned i = 0; i < tradeItemNum; ++i)
			{
				int itemID = goodsList[i];
				Json::Value value = zip_read_json(rootPath + boost::lexical_cast<string, int>(itemID));
				if (value == Json::nullValue)continue;
				priceVec vec;
				for (unsigned i = 0; i < value.size(); ++i)
				{
					vec.push_back(value[i].asInt());
				}
				cMap[itemID] = vec;
			}
		}


		//道具
		//////////////////////////////////////////////////////////////////////////
		{
			do{
				Json::Value config = na::file_system::load_jsonfile_val("./instance/trade/trade_item.json");
				if (config == Json::nullValue)break;
				std::map<int, Json::Value> planetConfigMap;
				for (unsigned i = 0; i < planetNum; i++)
				{
					int pId = planet[i];
					Json::Value planetJson;
					planetJson["planetID"] = pId;
					planetJson["buy"] = Json::arrayValue;
					planetJson["sale"] = config;
					planetJson["allowLevel"] = planetLimit[i];
					planetConfigMap[pId] = planetJson;
				}
				for (unsigned i = 0; i < config.size(); i++)
				{
					Json::Value& itemJson = config[i];
					int pos = itemJson["planetID"].asInt();
					planetConfigMap[pos]["buy"].append(itemJson);
				}
				ownPlanet.clear();
				for (unsigned i = 0; i < planetNum; i++)
				{
					int pId = planet[i];
					Json::Value& planetConfig = planetConfigMap[pId];
					mongo::BSONObj key = BSON("key" << pId);
					mongo::BSONObj obj = db_mgr.FindOne(strDBSysTrade, key);
					ownPlanet.insert(planetMap::value_type(
						pId, planetTrade(planetConfig, obj)));
				}
			} while (false);
		}

		//////////////////////////////////////////////////////////////////////////
		//事件生成
		{
			mongo::BSONObj key = BSON("key" << refreshTickKey);
			mongo::BSONObj obj = db_mgr.FindOne(strDBSysTrade, key);
			if (obj.isEmpty())
			{
				unsigned tickTime = na::time_helper::get_current_time();
				tm now_tm = na::time_helper::toTm(tickTime);
				tickTime -= (now_tm.tm_min * 60 + now_tm.tm_sec);
				PriceTick = tickTime + 600;//10分钟刷新一次
				EventTick = tickTime + 3600;//1个小时刷新一次
				rankTick = tickTime - now_tm.tm_hour * 3600 + na::time_helper::DAY;
				saveTick();
				eventCreate(true);
				priceCreate(true);
			}
			else
			{
				PriceTick = (unsigned)obj["pt"].Int();
				EventTick = (unsigned)obj["et"].Int();
				rankTick = (unsigned)obj["rt"].Int();
			}
		}
		Timer::AddEventTickTime(boost::bind(&trade_system::priceCreate, trade_system::trade_pointer, false), PriceTick);
		Timer::AddEventTickTime(boost::bind(&trade_system::eventCreate, trade_system::trade_pointer, false), EventTick);
		Timer::AddEventTickTime(boost::bind(&trade_system::writeRank, trade_system::trade_pointer, false), rankTick);
	}

	void trade_system::saveRich()
	{
		mongo::BSONObj key = BSON("key" << "key");
		mongo::BSONObjBuilder builder;
		builder << "key" << "key";
		mongo::BSONArrayBuilder arrBuilder;
		ForEach(RichList, it, _RichList)
		{
			const Rich& cRich = *it;
			arrBuilder << BSON("pid" << cRich.playerID << "pn" << cRich.pName << "gn" << cRich.gName <<
				"ct" << cRich.createTime << "kid" << cRich.force << "pt" << cRich.passTime << "v" << cRich.voucher);
		}
		builder << "val" << arrBuilder.arr();
		db_mgr.save_mongo(strDBRichTrade, key, builder.obj());
	}

	void trade_system::saveRankToday()
	{
		mongo::BSONObj key = BSON("key" << "key_t");
		mongo::BSONObjBuilder builder;
		builder << "key" << "key_t";
		mongo::BSONArrayBuilder arrBuilder;
		ForEach(RankList, it, _TodayList)
		{
			const Rank& cRank = *it;
			arrBuilder << BSON("pid" << cRank.playerID << "pn" << cRank.playerName
				<< "ct" << cRank.createTime << "v" << cRank.voucher);
		}
		builder << "val" << arrBuilder.arr();
		db_mgr.save_mongo(strDBRichTrade, key, builder.obj());
	}

	void trade_system::saveRankHistory()
	{
		mongo::BSONObj key = BSON("key" << "key_h");
		mongo::BSONObjBuilder builder;
		builder << "key" << "key_h";
		mongo::BSONArrayBuilder arrBuilder;
		ForEach(RankList, it, _HistoryList)
		{
			const Rank& cRank = *it;
			arrBuilder << BSON("pid" << cRank.playerID << "pn" << cRank.playerName
				<< "ct" << cRank.createTime << "v" << cRank.voucher);
		}
		builder << "val" << arrBuilder.arr();
		db_mgr.save_mongo(strDBRichTrade, key, builder.obj());
	}

	const static string strTradeRank = "log_trade_rank";
	void trade_system::writeRank(bool force)
	{
		//富豪榜
		string data = "[";
		ForEach(RichList, it, _RichList)
		{
			const Rich& cRich = *it;
			string cRstr = "[\"" + cRich.pName + "\"," + boost::lexical_cast<string, int>(cRich.force) +
				",\"" + cRich.gName + "\"," + boost::lexical_cast<string, int>(cRich.passTime) + "," +
				boost::lexical_cast<string, int>(cRich.voucher) + "," + boost::lexical_cast<string, int>(cRich.playerID) +"]";
			data += cRstr;
			if (it == --_RichList.end())continue;
			data += ",";
		}
		data += "]";
		zip_write(strRichDir, data);

		//历史最佳单票
		data.clear();
		data = "[";
		ForEach(RankList, it, _HistoryList)
		{
			const Rank& cRank = *it;
			string cRstr = "[\"" + cRank.playerName + "\"," +
				boost::lexical_cast<string, int>(cRank.voucher) + 
				"," + boost::lexical_cast<string, int>(cRank.playerID) + "]";
			data += cRstr;
			if (it == --_HistoryList.end())continue;
			data += ",";
		}
		data += "]";
		zip_write(strRankHistoryDir, data);

		//当日最佳单票
		data.clear();
		data = "[";
		ForEach(RankList, it, _TodayList)
		{
			const Rank& cRank = *it;
			string cRstr = "[\"" + cRank.playerName + "\"," +
				boost::lexical_cast<string, int>(cRank.voucher) +
				"," + boost::lexical_cast<string, int>(cRank.playerID) + "]";
			data += cRstr;
			if (it == --_TodayList.end())continue;
			data += ",";
		}
		data += "]";
		zip_write(strRankTodayDir, data);

		if (!force)
		{
			rankTick += na::time_helper::DAY;
			saveTick();
			int rank = 1;
			ForEach(RankList, it, _TodayList)
			{
				const Rank& cRank = *it;
				StreamLog::Log(strTradeRank, cRank.playerID, cRank.playerName, 0, rank, cRank.voucher, 0);
				++rank;
			}
			_TodayList.clear();
			saveRankToday();
			Timer::AddEventTickTime(boost::bind(&trade_system::writeRank, trade_system::trade_pointer, false), rankTick);
		}
	}

	void trade_system::writePrice()
	{
		ForEach(jsonMap, it, buyHistory)
		{
			priceVec& vec = it->second;
			string data = "[";
			for (unsigned i = 0; i < vec.size(); ++i)
			{
				data += boost::lexical_cast<string, int>(vec[i]);
				if (i + 1 == vec.size())continue;
				data += ",";
			}
			data += "]";
			zip_write(strBuyDir + boost::lexical_cast<string, int>(it->first), data);
		}

		ForEach(i_jsonMap, it, saleHistory)
		{
			jsonMap& cMap = it->second;
			ForEach(jsonMap, jit, cMap)
			{
				priceVec& vec = jit->second;
				string data = "[";
				for (unsigned i = 0; i < vec.size(); ++i)
				{
					data += boost::lexical_cast<string, int>(vec[i]);
					if (i + 1 == vec.size())continue;
					data += ",";
				}
				data += "]";
				zip_write(strSaleDir + boost::lexical_cast<string, int>(it->first) + "/" + boost::lexical_cast<string, int>(jit->first),
					data);
			}
		}
	}

	void trade_system::AddBuyPrice(const int itemID, const int price)
	{
		// 		jsonMap::iterator it = buyHistory.find(itemID);
		// 		if (it == buyHistory.end())return;
		//		priceVec& value = it->second;
		priceVec& value = buyHistory[itemID];
		value.push_back(price);
		if (value.size() > 100)
		{
			value.erase(value.begin());
		}
	}

	void trade_system::AddSalePrice(const int planetID, const int itemID, const int price)
	{
		// 		i_jsonMap::iterator it = saleHistory.find(planetID);
		// 		if (it == saleHistory.end())return;
		// 		jsonMap& jMap = it->second;
		// 		jsonMap::iterator jit = jMap.find(itemID);
		// 		if (jit == jMap.end())return;
		//		priceVec& value = jit->second;
		priceVec& value = saleHistory[planetID][itemID];
		value.push_back(price);
		if (value.size() > 100)
		{
			value.erase(value.begin());
		}
	}

	trade_system::Event trade_system::getEvent(const int planetID)
	{
		iuiMap::iterator it = eventMap.find(planetID);
		if (it == eventMap.end())return Event(0);
		unsigned now = na::time_helper::get_current_time();
		if (now >= it->second.endTime)
		{
			eventMap.erase(it);
			saveEvent();
			return Event(0);
		}
		return it->second;
	}
}