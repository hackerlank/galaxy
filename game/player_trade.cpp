#include "player_trade.h"
#include "helper.h"
#include "mongoDB.h"
#include "trade_system.h"
#include "guild_system.h"
#include "file_system.h"
#include "activity_system.h"

namespace gg
{
	int tradeItem::numAlter(const int num, const int price /* = 0 */)
	{
		if (num == 0)return 0;
		int aver = averPrice();
		int futureStack = num + stackNum();
		if (num > 0 && futureStack > 0)
		{
			aver = (aver * stackNum() + price) / futureStack;
		}
		int oldNum = stackNum();
		futureStack = futureStack < 0 ? 0 : futureStack;
		stackNum = futureStack;
		averPrice = aver;
		int alter = stackNum() - oldNum;
		own.TradeItem.itemUpdate(*this, alter);
		return alter;
	}


#define dUFunc boost::bind(&playerTrade::dataUpdate, this)
#define tVouFunc boost::bind(&playerTrade::VoucherAlter, this, _1, _2)

	playerTrade::playerTrade(playerData& own) : Block(own),
		Voucher(0), PassVoucher(0), TaskState(0), Position(0),
		Destination(0), FlyTime(0u), TodayRewardTimes(0),
		Speed(1.0), VoucherHistory(0), PassTimes(0),
		orgPosition(0), reqTimes(0), VoucherMaxToday(0),
		VoucherMaxHis(0), PlaneLevel(1), PlaneExp(0)
	{
		showCD = na::time_helper::get_current_time();
		historyTime = na::time_helper::get_current_time();
		vTTime = historyTime;
		vHTime = historyTime;

		_buffMap.clear();

		PlaneLevel.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc)
			.AddTypeUpdate(tVouFunc);
		PlaneExp.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc)
			.AddTypeUpdate(tVouFunc);
		VoucherMaxToday.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc)
			.AddTypeUpdate(tVouFunc);
		VoucherMaxHis.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc)
			.AddTypeUpdate(tVouFunc);
		Voucher.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc)
			.AddTypeUpdate(tVouFunc);
		PassVoucher.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc);
		TaskState.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc);
		Position.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc);
		Destination.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc);
		FlyTime.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc);
		TodayRewardTimes.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc);
		Speed.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc);
		PassTimes.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc)
			.AddTypeUpdate(boost::bind(&playerTrade::PassTimesAlter, this, _1, _2));
		VoucherHistory.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc)
			.AddTypeUpdate(boost::bind(&playerTrade::HistoryVoucherAlter, this, _1, _2));
		orgPosition.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc);
		reqTimes.AddHandler(own._Handler())
			.AddDataUpdate(dUFunc)
			.AddTypeUpdate(boost::bind(&playerTrade::reqTimesAlter, this, _1, _2));
	}

	playerTrade::~playerTrade()
	{
	}

	void playerTrade::autoUpdate()
	{
		update();
	}

	const static string strVoucher = "v";
	const static string strPassVoucher = "pv";
	const static string strDestination = "dt";
	const static string strFlyTime = "ft";
	const static string strPosition = "pos";
	const static string strOrgPostition = "op";
	const static string strTodayRewardTimes = "trt";
	const static string strBuffList = "bl";
	const static string strTaskState = "ts";
	const static string strSpeed = "spd";
	const static string strHistoryPass = "hp";
	const static string strHistoryTotalVoucher = "hv";
	const static string strTodayVoucher = "stv";
	const static string strHistoryVoucher = "shv";
	const static string strHistoryTotalVoucherCT = "hvct";
	const static string strTodayVoucherCT = "stct";
	const static string strHistoryVoucherCT = "shct";
	const static string strReqTimes = "rts";
	const static string strPlaneLv = "plv";
	const static string strPlaneExp = "pexp";

	void playerTrade::update()
	{
		//player_trade_update_resp
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		Json::Value& dataJson = updateJson[msgStr][1u];
		dataJson[updateFromStr] = State::getState();
		dataJson[strPosition] = getPosition();
		dataJson[strVoucher] = Voucher();
		dataJson[strFlyTime] = FlyTime();
		dataJson[strDestination] = Destination();
		dataJson[strPassVoucher] = PassVoucher();
		dataJson[strTaskState] = TaskState();
		dataJson[strTodayRewardTimes] = TodayRewardTimes();
		dataJson[strSpeed] = Speed();
		dataJson[strHistoryTotalVoucher] = VoucherHistory();
		dataJson[strHistoryPass] = PassTimes();
		dataJson[strOrgPostition] = orgPosition();
		dataJson[strPlaneLv] = PlaneLevel();
		dataJson[strPlaneExp] = PlaneExp();
		//dataJson[strReqTimes] = reqTimes();
		unsigned i = 0;
		Json::Value& bLJson = dataJson[strBuffList];
		bLJson = Json::arrayValue;
		for (buffMap::iterator it = _buffMap.begin(); it != _buffMap.end(); ++it)
		{
			bLJson[i].append(it->first);
			bLJson[i].append(it->second());
			++i;
		}
		own.sendToClient(gate_client::player_trade_update_resp, updateJson);
	}

	bool playerTrade::save()
	{
		mongo::BSONObjBuilder builder;
		builder << playerIDStr << own.playerID << strVoucher << Voucher() << strPassVoucher << PassVoucher() <<
			strPosition << Position() << strDestination << Destination() << strFlyTime << FlyTime() << strTaskState <<
			TaskState() << strTodayRewardTimes << TodayRewardTimes() << strSpeed << Speed() << strHistoryPass <<
			PassTimes() << strHistoryTotalVoucher << VoucherHistory() << strHistoryTotalVoucherCT << historyTime <<
			strOrgPostition << orgPosition() << strReqTimes << reqTimes() << strTodayVoucherCT << vTTime <<
			strHistoryVoucherCT << vHTime << strTodayVoucher << VoucherMaxToday() << strHistoryVoucher <<
			VoucherMaxHis() << strPlaneExp << PlaneExp() << strPlaneLv << PlaneLevel();
		{
			mongo::BSONArrayBuilder arrBuilder;
			unsigned now = na::time_helper::get_current_time();
			for (buffMap::iterator it = _buffMap.begin(); it != _buffMap.end();)
			{
				buffMap::iterator oldIt = it;
				++it;
				if (oldIt->second <= now)continue;
				arrBuilder << BSON("1" << oldIt->first << "2" << oldIt->second());
			}
			builder << strBuffList << arrBuilder.arr();
		}
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		return db_mgr.save_mongo(strDBPlayerTrade, key, builder.obj());
	}

	bool playerTrade::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(strDBPlayerTrade, key);
		if (!obj.isEmpty()){
			checkNotEoo(obj[strHistoryTotalVoucher]){ VoucherHistory.rawset(obj[strHistoryTotalVoucher].Int()); }
			checkNotEoo(obj[strTodayVoucher]){ VoucherMaxToday.rawset(obj[strTodayVoucher].Int()); }
			checkNotEoo(obj[strHistoryVoucher]){ VoucherMaxHis.rawset(obj[strHistoryVoucher].Int()); }
			checkNotEoo(obj[strHistoryPass]){ PassTimes.rawset((unsigned)obj[strHistoryPass].Int()); }
			checkNotEoo(obj[strVoucher]){ Voucher.rawset(obj[strVoucher].Int()); }
			checkNotEoo(obj[strSpeed]){ Speed.rawset(obj[strSpeed].Double()); }
			checkNotEoo(obj[strTaskState]){ TaskState.rawset(obj[strTaskState].Int()); }
			checkNotEoo(obj[strPassVoucher]){ PassVoucher.rawset(obj[strPassVoucher].Int()); }
			checkNotEoo(obj[strPosition]){ Position.rawset(obj[strPosition].Int()); }
			checkNotEoo(obj[strOrgPostition]){ orgPosition.rawset(obj[strOrgPostition].Int()); }
			checkNotEoo(obj[strDestination]){ Destination.rawset(obj[strDestination].Int()); }
			checkNotEoo(obj[strFlyTime]){ FlyTime.rawset(obj[strFlyTime].Int()); }
			checkNotEoo(obj[strHistoryTotalVoucherCT]){ historyTime = (unsigned)obj[strHistoryTotalVoucherCT].Int(); }
			checkNotEoo(obj[strTodayVoucherCT]){ vTTime = (unsigned)obj[strTodayVoucherCT].Int(); }
			checkNotEoo(obj[strHistoryVoucherCT]){ vHTime = (unsigned)obj[strHistoryVoucherCT].Int(); }
			checkNotEoo(obj[strTodayRewardTimes]){ TodayRewardTimes.rawset(obj[strTodayRewardTimes].Int()); }
			checkNotEoo(obj[strReqTimes]){ reqTimes.rawset(obj[strReqTimes].Int()); }
			checkNotEoo(obj[strPlaneLv]){ PlaneLevel.rawset(obj[strPlaneLv].Int()); }
			checkNotEoo(obj[strPlaneExp]){ PlaneExp.rawset(obj[strPlaneExp].Int()); }
			checkNotEoo(obj[strBuffList])
			{
				vector<mongo::BSONElement> elems = obj[strBuffList].Array();
				for (unsigned i = 0; i < elems.size(); ++i)
				{
					_buffMap.insert(buffMap::value_type(elems[i]["1"].Int(), spUInt((unsigned)elems[i]["2"].Int(), own._Handler(), dUFunc)));
				}
			}
			return true;
		}
		return false;
	}

	void playerTrade::dataUpdate()
	{
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerTrade::HistoryVoucherAlter(int oV, int nV)
	{
		const static int limit = 1000000000;
		int num = std::abs(nV - oV);
		if (num < limit)
		{
			unsigned now = na::time_helper::get_time_zero(na::time_helper::get_current_time());
			unsigned ttick = na::time_helper::get_time_zero(vTTime);
			if (now > ttick && now - ttick >= na::time_helper::ONEDAY)
			{
				VoucherMaxToday.rawset(0);
			}
			if (num > VoucherMaxToday())
			{
				vTTime = na::time_helper::get_current_time();
				VoucherMaxToday = num;
				trade_system::Rank value(VoucherMaxToday(), vTTime, own.playerID, own.Base.getName());
				trade_sys.insertRankToday(own.playerID, value);
			}
			if (num > VoucherMaxHis())
			{
				vHTime = na::time_helper::get_current_time();
				VoucherMaxHis = num;
				trade_system::Rank value(VoucherMaxHis(), vHTime, own.playerID, own.Base.getName());
				trade_sys.insertRankHistory(own.playerID, value);
			}
		}
		
		if (VoucherHistory > limit)VoucherHistory.rawset(limit);
		if (oV < limit && VoucherHistory() > 0)
		{
			historyTime = na::time_helper::get_current_time();
			trade_system::Rich value(VoucherHistory(), PassTimes(), historyTime, own.playerID, 
				own.Base.getName(), guild_sys.getGuildName(own.Guild.getGuildID()), own.Base.getSphereID());
			trade_sys.insertRich(own.playerID, value);
		}
	}

	void playerTrade::reqTimesAlter(int oV, int nV)
	{
		if (nV > 400000000)reqTimes.rawset(oV);
	}

	void playerTrade::PassTimesAlter(int oV, int nV)
	{
		activity_sys.updateActivity(own.getOwnDataPtr(), 0, activity::_trade);
	}

	int playerTrade::getRWTimes()
	{
		own.TickMgr.Tick();
		return TodayRewardTimes();
	}

	void playerTrade::tickRWTimes()
	{
		own.TickMgr.Tick();
		TodayRewardTimes += 1;
	}

	void playerTrade::VoucherAlter(int oV, int nV)
	{
		if (TaskState == 1 && Voucher >= PassVoucher())
		{
			TaskState = 2;
		}
	}

	void playerTrade::moveToNext(bool tickTime /* = false */)
	{
		Position = moveToNext(Position(), Destination());
		if (tickTime)
		{
			FlyTime = na::time_helper::get_current_time();
		}
	}

	int playerTrade::moveToNext(const int pos, const int des)
	{
		if (pos == des || pos < 0 || des < 0)return pos;
		int current_kingdom = pos / 1000;
		int des_kingdom = des / 1000;
		if (current_kingdom < 0 || des_kingdom < 0 || current_kingdom > 2 || des_kingdom > 2)return pos;
		int current_real_pos = pos % 1000;
		int des_real_pos = des % 1000;
		int now_pos = pos;
		if (current_kingdom == des_kingdom)//相同国家
		{
			if (des_real_pos > current_real_pos)
			{
				++now_pos;//位移+1
			}
			else
			{
				--now_pos;//位移-1
			}
		}
		else//不同国家
		{
			if (0 == current_real_pos)
			{
				now_pos = des_kingdom * 1000;
			}
			else
			{
				--now_pos;//位移减到0
			}
		}
		return now_pos;
	}

	int playerTrade::getPosition()
	{
		if (internalIsBerth())return Position();
		unsigned now = na::time_helper::get_current_time();
		if (FlyTime >= now)return Position();
		int current_kingdom = Position() / 1000;
		int des_kingdom = Destination() / 1000;
		if (current_kingdom < 0 || des_kingdom < 0 || current_kingdom > 2 || des_kingdom > 2)return Position();
		unsigned start = FlyTime();
		const unsigned drua = unsigned(TradeFlyStep / Speed());
		int pos = Position();
		bool alter = false;
		unsigned count = 0;
		do 
		{
			if (start >= now || now - start < drua)break;
			alter = true;
			start += drua;
			pos = moveToNext(pos, Destination());
			if (internalIsBerth())break;
			++count;
			if (count > 100)break;//防止死循环
		} while (true);
		if (alter)
		{
			FlyTime = start;
			Position = pos;
		}
		if (internalIsBerth())
		{
			Speed = 1.0;
//			FlyTime = 0;
		}
		return Position();
	}

	unsigned playerTrade::_FlyTime()
	{
		getPosition();
		return FlyTime();
	}

	bool playerTrade::Flying()
	{
		return getPosition() != Destination();
	}
	bool playerTrade::Berth()
	{
		return getPosition() == Destination();
	}

	void playerTrade::addBuff(const int buffID, const unsigned endTime /* = 0 */)
	{
		if (endTime == 0)return;
		unsigned now = na::time_helper::get_current_time();
		buffMap::iterator it = _buffMap.find(buffID);
		if (it != _buffMap.end() && now >= it->second())
		{
			_buffMap.erase(it);
			it = _buffMap.end();
		}
		if (buffID > trade_item_buff_start && buffID < trade_item_buff_end)
		{
			if (it == _buffMap.end())
			{
				_buffMap.insert(buffMap::value_type(buffID, spUInt(endTime + now, own._Handler(), dUFunc)));
				helper_mgr.insertSaveAndUpdate(this);
			}
			else
			{
				it->second += endTime;
			}
			return;
		}
		if (buffID > trade_world_buff_start && buffID < trade_world_buff_end)
		{
			if (it == _buffMap.end())
			{
				_buffMap.insert(buffMap::value_type(buffID, spUInt(endTime, own._Handler(), dUFunc)));
				helper_mgr.insertSaveAndUpdate(this);
			}
			else
			{
				it->second = endTime;
			}
			return;
		}
	}

	void playerTrade::updateRichgName(const string nName)
	{
		trade_sys.updategName(trade_system::Rich(VoucherHistory(), PassTimes(), historyTime, own.playerID), nName);
	}

	void playerTrade::updateRichpName(const string nName)
	{
		trade_sys.updatepName(trade_system::Rich(VoucherHistory(), PassTimes(), historyTime, own.playerID), nName);
	}

	int playerTrade::addExp(const int num)
	{
		if (num < 0)return 0;
		Params::Var& var = trade_sys.PlaneConfig[0u];
		if (PlaneLevel >= (int)var.getSize())return 0;
		int exp = PlaneExp() + num;
		int level = PlaneLevel();
		int max_level = (int)var.getSize();
		for (int i = PlaneLevel(); i < max_level; ++i)
		{
			int cost_exp = var[i].asInt();
			if (exp >= cost_exp)
			{
				exp -= cost_exp;
				level = i + 1;
				continue;
			}
			break;
		}
		if (PlaneLevel != level)
		{
			PlaneLevel = level;
			own.TradeItem.update();
		}
		int  add_exp = num;
		if (PlaneLevel >= max_level)
		{
			add_exp -= exp;
			exp = 0;
		}
		PlaneExp = exp;
		return add_exp;
	}

	bool playerTrade::hasBuff(const int buffID)
	{
		buffMap::iterator it = _buffMap.find(buffID);
		if (it == _buffMap.end())
		{
			return false;
		}
		unsigned now = na::time_helper::get_current_time();
		if (it != _buffMap.end() && it->second <= now)
		{
			_buffMap.erase(it);
			return false;
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////item
#define tItemUpdate boost::bind(&playerTradeBag::dataUpdate, &own.TradeItem)
	tradeItem::tradeItem(playerData& o, const int Id, const int num, const int aver) :
		own(o), itemID(Id), stackNum(num), averPrice(aver)
	{
		itemID.AddHandler(own._Handler())
			.AddDataUpdate(tItemUpdate);
		stackNum.AddHandler(own._Handler())
			.AddDataUpdate(tItemUpdate);
		averPrice.AddHandler(own._Handler())
			.AddDataUpdate(tItemUpdate);
	}

	tradeItem::tradeItem(playerData& o) :
		own(o), itemID(-1), stackNum(0), averPrice(0)
	{

	}

	playerTradeBag::playerTradeBag(playerData& own) : Block(own)
	{
		totalN = 0;
		capacity = 15;
		Bag.clear();
		rmItem.clear();
	}

	void playerTradeBag::doEnd()
	{
		rmItem.clear();
	}

	bool playerTradeBag::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(strTradeItemDB, key);
		if (!obj.isEmpty()){
			checkNotEoo(obj["ca"]){ capacity = obj["ca"].Int(); }
			checkNotEoo(obj["d"])
			{
				vector<mongo::BSONElement> elems = obj["d"].Array();
				for (unsigned i = 0; i < elems.size(); ++i)
				{
					mongo::BSONElement& el = elems[i];
					int num = el["n"].Int();
 					Bag.insert(itemMap::value_type(el["id"].Int(),
 						tradeItem(own, el["id"].Int(), num, el["av"].Int())));
					totalN += num;
				}
			}
			return true;
		}
		return false;
	}

	bool playerTradeBag::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID << "ca" << capacity;
		mongo::BSONArrayBuilder arrBuilder;
		for (itemMap::iterator it = Bag.begin(); it != Bag.end(); ++it)
		{
			tradeItem& cItem = it->second;
			arrBuilder << BSON("id" << cItem.getID() << "n" << cItem.Num() <<
				"av" << cItem.AverPrice());
		}
		obj << "d" << arrBuilder.arr();
		return db_mgr.save_mongo(strTradeItemDB, key, obj.obj());
	}

	void playerTradeBag::autoUpdate()
	{
		update();
	}

	int playerTradeBag::totalNum()
	{
		return totalN;
	}

	void playerTradeBag::update()
	{
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1u][updateFromStr] = State::getState();
		updateJson[msgStr][1u]["ca"] = getCapacity();
		Json::Value& itemJson = updateJson[msgStr][1u]["d"];
		itemJson = Json::arrayValue;
		unsigned  i = 0;
		for (itemMap::iterator it = Bag.begin(); it != Bag.end(); ++it)
		{
			tradeItem& cItem = it->second;
			//if (cItem.nullItem())continue;
			itemJson[i].append(cItem.getID());
			itemJson[i].append(cItem.Num());
			itemJson[i].append(cItem.AverPrice());
			++i;
			rmItem.erase(cItem.getID());
		}
		for (removeItem::iterator it = rmItem.begin(); it != rmItem.end(); ++it)
		{
			itemJson[i].append(*it);
			itemJson[i].append(0);
			itemJson[i].append(0);
			++i;
		}
		own.sendToClient(gate_client::player_trade_item_update_resp, updateJson);
	}

	tradeItem& playerTradeBag::getItem(const int itemID)
	{
		itemMap::iterator it = Bag.find(itemID);
		if (it == Bag.end())
		{
			static tradeItem NullItem(own);
			return NullItem;
		}
		return it->second;
	}

	bool playerTradeBag::canAddItem(const int itemID, const int num)
	{
		if (itemID < 0 || itemID >= tradeItemNum)return false;
		if (num < 1 || num > (getCapacity() - totalN))return false;
		return true;
	}

	int playerTradeBag::getCapacity()
	{
		Params::Var& var = trade_sys.PlaneConfig[2u];
		return capacity + var[own.Trade.PlaneLevel()].asInt();
	}

	bool playerTradeBag::canDelItem(const int itemID, const int num)
	{
		if (itemID < 0 || itemID >= tradeItemNum)return false;
		tradeItem& item = getItem(itemID);
		if (item.nullItem())return false;
		if (item.Num() < num)return false;
		return true;
	}

	void playerTradeBag::clearBag()
	{
		Bag.clear();
		totalN = 0;
	}

	bool playerTradeBag::delItem(const int itemID, const int num)
	{
		tradeItem& item = getItem(itemID);
		if (item.nullItem())return false;
		item.numAlter(-num);
		return true;
	}

	bool playerTradeBag::addItem(const int itemID, const int num, const int totalCost)
	{
//		if (!canAddItem(itemID, num))return false;
		itemMap::iterator it = Bag.find(itemID);
		if (it == Bag.end())
		{
 			it = Bag.insert(itemMap::value_type(itemID,
 				tradeItem(own, itemID, 0, 0))).first;
		}
		tradeItem& cItem = it->second;
		cItem.numAlter(num, totalCost);
		return true;
	}

	void playerTradeBag::itemUpdate(tradeItem& item, const int num)
	{
		totalN += num;
		totalN = totalN < 0 ? 0 : totalN;
		if (item.nullItem())
		{
			rmItem.insert(item.getID());
			Bag.erase(item.getID());
		}
	}

	void playerTradeBag::dataUpdate()
	{
		helper_mgr.insertSaveAndUpdate(this);
	}


	//////////////////////////////////////////////////////////////////////////
	///技能
	vector< vector<double> > playerTradeSkill::ATTRI;
	static int SkillMaxLevel[TradeSkill::IDX];

	void playerTradeSkill::initSkill()
	{
		LogS << "init trade skill ..." << LogEnd;
		Json::Value skillJson = na::file_system::load_jsonfile_val("./instance/trade/skill_attri.json");
		for (unsigned i = 0;i < TradeSkill::IDX && i < skillJson.size(); ++i)
		{
			Json::Value& attriJson = skillJson[i];
			vector<double> vec;
			for (unsigned n = 0; n < attriJson.size(); ++n)
			{
				vec.push_back(attriJson[n].asDouble());
			}
			ATTRI.push_back(vec);
		}

		for (unsigned i = 0; i < TradeSkill::IDX; ++i)
		{
			SkillMaxLevel[i] = (int)ATTRI[i].size() - 1;
			SkillMaxLevel[i] = SkillMaxLevel[i] < 0 ? 0 : SkillMaxLevel[i];
		}
	}

	int playerTradeSkill::getSkill(const int id)
	{
		if (id < 0 || id >= TradeSkill::IDX)return 0;
		return SKILL[id];
	}

	bool playerTradeSkill::setSkill(const int id, const int level)
	{
		if (id < 0 || id >= TradeSkill::IDX)return false;
		if (level <  0 || level > SkillMaxLevel[id])return false;
		SKILL[id] = level;
		helper_mgr.insertSaveAndUpdate(this);
		return true;
	}

	void playerTradeSkill::battleAdd(double* point)
	{
		double added[characterNum];
		memset(added, 0x0, sizeof(added));
		for (int i = 0; i < TradeSkill::IDX &&  i < characterNum; ++i)
		{
			added[i] = ATTRI[i][SKILL[i]];
		}
		memcpy(point, added, sizeof(added));
	}

	playerTradeSkill::playerTradeSkill(playerData& own) : Block(own)
	{
		memset(SKILL, 0x0, sizeof(SKILL));
	}

	bool playerTradeSkill::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder builder;
		builder << playerIDStr << own.playerID;
		mongo::BSONArrayBuilder arrBuidler;
		for (int i = 0; i < TradeSkill::IDX; ++i)
		{
			arrBuidler << SKILL[i];
		}
		builder << "sk" << arrBuidler.arr();
		return db_mgr.save_mongo(strDBTradeSkill, key, builder.obj());
	}

	bool playerTradeSkill::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(strDBTradeSkill, key);
		if (!obj.isEmpty())
		{
			vector<mongo::BSONElement> elems = obj["sk"].Array();
			for (unsigned i = 0; i < elems.size(); ++i)
			{
				mongo::BSONElement& elem = elems[i];
				SKILL[i] = elem.Int();
			}
			return true;
		}
		return false;
	}

	void playerTradeSkill::autoUpdate()
	{
		update();
	}

	void playerTradeSkill::update()
	{
		Json::Value updateJson;
		updateJson[msgStr][0u] = 0;
		Json::Value& dataJson = updateJson[msgStr][1u];
		dataJson[updateFromStr] = State::getState();
		for (int i = 0; i < TradeSkill::IDX; ++i)
		{
			dataJson["sk"][i].append(i);
			dataJson["sk"][i].append(SKILL[i]);
		}
		own.sendToClient(gate_client::guild_trade_skill_update_resp, updateJson);
	}
}