#include "item_system.h"
#include <string>
#include "file_system.h"
#include "response_def.h"
#include "helper.h"
#include "mongoDB.h"
#include "commom.h"
#include "pilotManager.h"
#include "action_format.hpp"
#include "chat_system.h"
#include "task_system.h"
#include "record_system.h"
#include "game_server.h"
#include "player_crystal_exchange.h"
#include "player_item.h"
#include "activity_system.h"
#include "activity_game_param.h"

using namespace std;
using namespace Params;

namespace gg
{
	const static string lg_crystal_sys = "log_crystal";			//水晶log表
	const static string log_item_business = "log_item_business";//道具买卖


	item_system::item_system()
	{
		// 		upgradeRate = 7500;
		// 		isUp = true;
		//		lastUpdateTime = na::time_helper::get_sys_time();
	}

	void item_system::updateItemSystemInfo(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		packageBaseItemSys(d, r);
	}

	void item_system::packageBaseItemSys(playerDataPtr player, Json::Value& packJson)
	{
		if(player == NULL)return;
		const static string upgradeCDStr = "cd";
		const static string upgradeLockStr = "lk";
		packJson[msgStr][0u] = 0;
		packJson[msgStr][1u][updateFromStr] = State::getState();
		CDData CD = player->Item.getCD();
		packJson[msgStr][1u][upgradeCDStr] = CD.CD;
		packJson[msgStr][1u][upgradeLockStr] = CD.Lock;
	}

	void item_system::updatePlayerItems(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Item.update();
	}

	void item_system::buyItem(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int itemID = js_msg[0u].asInt();
		int option = js_msg[1u].asInt();
		int buy = js_msg[2u].asInt();
		if(buy < 1)buy = 1;
		if(buy > 999)buy = 999;
		if(option < 0 || option > 1)Return(r, -1);
		if (itemID == WashSpecial::GuideWashItem)
		{
			int bT = d->Item.buyTimes(itemID);
			if (buy > 1 || bT > 0)	Return(r, 3);
		}
		ObjectValue& config = getConfig(itemID);
		if(config.isEmpty() || !config.isMember(ItemDefine::merchandise))Return(r, -1);
		Var& configVar = config[ItemDefine::merchandise];
		int buyPrice = configVar[ItemDefine::itemBuyPriceStr].asInt() * buy;
		int buyGold = configVar[ItemDefine::itemBuyGoldStr].asInt() * buy;
		if((option == 0 && buyPrice < 0) || (option == 1 && buyGold < 0))Return(r,-1);
		if(!d->Item.canAddItem(itemID, buy))Return(r, 1);
		if((option == 0 && buyPrice> d->Base.getSilver())  ||
			(option == 1 && buyGold > d->Base.getAllGold()) ){
				r[msgStr][1u] = option;
				Return(r, 2);
		}
		if(option == 0) d->Base.alterSilver(-buyPrice);
		else	d->Base.alterBothGold(-buyGold);
		itemVector vec = d->Item.addItem(itemID, buy);
		if(vec.empty())Return(r, -1);
		Json::Value& createItemJson = r[msgStr][1u];
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			createItemJson.append(vec[i]->getLocalID());
		}
		d->Item.tickBuyID(itemID, buy);
		StreamLog::LogV(log_item_business, d, itemID, buy, -1, boost::lexical_cast<string, int>(buyPrice), boost::lexical_cast<string, int>(buyGold));
		Return(r, 0);
	}

	void item_system::saleItem(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		int num = js_msg[1u].asInt();
		if(num < 1)Return(r, -1);
		playerItemPtr sale = d->Item.getItem(localID);
		if(sale == playerItemPtr())Return(r, -1);
		if(sale->getStack() < num)Return(r, -1);
		ObjectValue& config = getConfig(sale->getItemID());
		if(config.isEmpty() || !config.isMember(ItemDefine::merchandise))Return(r,-1);
		Var& configVar = config[ItemDefine::merchandise];
		int salePrice = configVar[ItemDefine::itemSalePriceStr].asInt() * num;
		if(salePrice < 0) Return(r,-1);
		equipExPtr eqExPtr = sale->getEquipEx();
		if (eqExPtr != NULL && eqExPtr->hasCrytal())Return(r, 1);//有水晶
		sale->Remove(num);
		if(eqExPtr != NULL)
		{
			//装备是禁止堆叠
			int eqLV = eqExPtr->getLv();
			salePrice += int(config[ItemDefine::equipment][ItemDefine::equipUpgrdeCostStr][eqLV - 2].asInt() * 0.8);
		}
		d->Base.alterSilver(salePrice);
		if(config.isMember(ItemDefine::equipment))
			task_sys.updateBranchTaskInfo(d, _task_equipment_lv);
		StreamLog::LogV(log_item_business, d, localID, num, -1, boost::lexical_cast<string, int>(salePrice));
		Return(r, 0);
	}
	
	void item_system::clearUpgradeCD(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		CDData CD = d->Item.getCD();
		int cost = GGCOMMON::costCDClear(CD.CD, 60);
		if(cost > d->Base.getAllGold())Return(r, 1);
		d->Item.clearCD();
		d->Base.alterBothGold(-cost);
		Json::Value udJson;
		packageBaseItemSys(d, udJson);
		d->sendToClient(gate_client::item_system_info_update_resp, udJson);
		Return(r, 0);
	}

	void item_system::crytalTakeOff(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int equipID = js_msg[0u].asInt();

		playerItemPtr equipItem = d->Item.getItem(equipID);
		if (equipItem == NULL || equipItem->getEquipEx() == NULL)Return(r, -1);

		equipExPtr equipex = equipItem->getEquipEx();
		if (equipex->hasCrytal())
		{
			int crytalID = equipex->getCrytal();
			if (!d->Item.canAddItem(crytalID, 1))Return(r, 1);
			equipex->takeOffCrytal();
			d->Item.addItem(crytalID);
			StreamLog::LogV(lg_crystal_sys, d, equipID, crytalID);
			Return(r, 0);
		}
		Return(r, -1);
	}

	void item_system::Embedded(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		if (d->Base.getAllGold() < 5)Return(r, 1);//金币不足

		int equipID = js_msg[0u].asInt();
		int crytalID = js_msg[1u].asInt();

		playerItemPtr equipItem = d->Item.getItem(equipID);
		if (equipItem == NULL || equipItem->getEquipEx() == NULL)Return(r, -1);
		if (equipItem->getEquipEx()->hasCrytal())Return(r, 4);//已经有水晶
		if (!d->Item.canRemoveItem(crytalID, 1))Return(r, 3);//水晶不够

		Params::ObjectValue& wItemConfig = item_sys.getConfig(crytalID);
		if (wItemConfig.isEmpty() || !wItemConfig.isMember(ItemDefine::crystal))Return(r, -1);;//不是水晶
		Params::Var& wItemVar = wItemConfig[ItemDefine::crystal];
		if (wItemVar[ItemDefine::crystalEquipLimitStr].asInt() > d->Base.getLevel())Return(r, 2);

		d->Base.alterBothGold(-5);
		d->Item.removeItem(crytalID, 1);
		equipItem->getEquipEx()->embedded(crytalID);

		StreamLog::LogV(lg_crystal_sys, d, equipID, crytalID);

		Return(r, 0);
	}

	//水晶分解
	void item_system::DisintegrationReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		bool isCri = js_msg[1u].asBool();
		int num = js_msg[2u].asInt();
		int eqLvTemp = 0;
		int criNum = 0;	//暴击倍数
		int criTimes[3] = { 0, 0, 0 };//暴击次数
		//2总督等级≥14级
		if (d->Base.getLevel() < 14)Return(r, 2);
		//3VIP4及以上的玩家开放暴击选项
		if (d->Vip.getVipLevel() < 4 && isCri)Return(r, 3);
		playerItemPtr uItem = d->Item.getItem(localID);
		if (uItem == NULL)Return(r, -1);
		if (uItem->getEquipEx() != NULL && uItem->getEquipEx()->hasCrytal())Return(r, 4);//有水晶
		if (num < 1 || uItem == playerItemPtr() || !uItem->isVaild() || uItem->getStack() < num)Return(r, -1);
		int GoldCost = 0;
		Params::ObjectValue& obj = getConfig(uItem->getItemID());
		if(isCri){
			if (!obj.isMember(ItemDefine::disintegrator))Return(r, -1);
			//GoldCost = obj[ItemDefine::itemSalePriceStr].asInt();
			GoldCost = int(std::ceil(obj[ItemDefine::disintegrator][ItemDefine::criGoldCost].asInt()*(1 + activity_sys.getRate(param_resolve_equip_cost, d))));
			GoldCost *= num;
			if (GoldCost > d->Base.getAllGold())Return(r, 1);//钻石不足
		}
		actionFormat::actionVec vec = getItemDisint(uItem->getItemID());
		if (vec.empty())Return(r, -1);
		//int sclv = d->Science.getScienceLv(crystal_refining);
		double rate = 0.25;
		if (isCri)rate = 0.1;
		Json::Value Param;
		for (int i = 0; i < num; ++i)
		{
			if (isCri)
			{
				if (commom_sys.randomOk(rate))
				{
					criNum += 5;
					++criTimes[2];
				}
				else
				{
					criNum += 2;
					++criTimes[1];
				}
			}
			else
			{
				if (commom_sys.randomOk(rate))
				{
					criNum += 2;
					++criTimes[1];
				}
				else
				{
					criNum += 1;
					++criTimes[0];
				}
			}
		}
		Param[ACTION::strDisintegrateTimes] = criNum;
		Param[ACTION::strDisintegrateCast] = criTimes[2];
		actionResult resAction = actionFormat::actionDo(d, vec, Param);
		if (resAction.OK())uItem->Remove(num);
		r[msgStr][1u][errorActionJson::breakIDStr] = resAction.breakID;
		r[msgStr][1u][errorActionJson::breakCodeStr] = resAction.resultCode;
		r[msgStr][1u]["isc"] = criNum;
		r[msgStr][1u]["ct"][0u] = criTimes[0];
		r[msgStr][1u]["ct"][1u] = criTimes[1];
		r[msgStr][1u]["ct"][2u] = criTimes[2];
		if (isCri && resAction.OK())
		{
			d->Base.alterBothGold(-GoldCost);
		}
		r[msgStr][2u] = 0;
		if (resAction.OK())
		{
			equipExPtr eqPtr = uItem->getEquipEx();
			if (eqPtr != NULL)
			{
				eqLvTemp = eqPtr->getLv();
				Var& configVar = obj[ItemDefine::merchandise];
				int reSilver = configVar[ItemDefine::itemSalePriceStr].asInt();
				reSilver += int(obj[ItemDefine::equipment][ItemDefine::equipUpgrdeCostStr][eqPtr->getLv() - 2].asInt() * 0.8);
				d->Base.alterSilver(reSilver);
				r[msgStr][2u] = reSilver;
			}
		}
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(criNum));
		fields.push_back(boost::lexical_cast<string, int>(eqLvTemp));
		fields.push_back(boost::lexical_cast<string, int>(num));
		StreamLog::Log(lg_crystal_sys, d, boost::lexical_cast<string, int>(localID), boost::lexical_cast<string, int>(isCri), fields);
		Return(r, 0);
	}

	//合成
	void item_system::composeItem(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int itemID = js_msg[0u].asInt();
		int cmNum = js_msg[1u].asInt();
		Params::ObjectValue& config = getConfig(itemID);
		if (config.isEmpty() || !config.isMember(ItemDefine::chip))Return(r, -1);
		Params::Var& configVar = config[ItemDefine::chip];
		int mixID = configVar[ItemDefine::chipMixIDStr].asInt();
		int mixType = configVar[ItemDefine::chipMixTypeStr].asInt();
		if (cmNum < 1 || mixType == ItemDefine::chip_pilot)cmNum = 1;
		if (cmNum > 999)cmNum = 999;
		int needNum = configVar[ItemDefine::chipNeedNumStr].asInt() * cmNum;
		if (configVar.isMember(ItemDefine::chipIsCrystalStr))
		{
			//1.总督等级大于等于51级
			if (d->Base.getLevel() < 51)Return(r, 1002);
			int targetLevel = configVar[ItemDefine::chipIsCrystalStr].asInt();
			if (targetLevel > 0)
			{
				int scLv = d->Science.getScienceLv(crystal_refining);
				if (targetLevel > scLv)Return(r, 1001);//合成水晶科技等级不足
			}
		}
		if (!d->Item.canRemoveItem(itemID, needNum))Return(r, 1);//道具不够
		int res = -1;
		if (mixType == ItemDefine::chip_item){
			Params::ObjectValue& mixConfig = getConfig(mixID);
			if (mixConfig.isEmpty())Return(r, -1);
			if (d->Item.addItem(mixID, cmNum).empty())Return(r, 2);//背包空间不足
			res = 0;
		}
		else if (mixType == ItemDefine::chip_pilot){
			const pilotConfig& pConfig = pilot_sys.getPilotConfig(mixID);
			if (!pConfig.isVaild())Return(r, -1);
			if (d->Pilots.checkPilotActive(mixID))Return(r, 3);//已经激活过了
			d->Pilots.activePilot(mixID);
			res = 0;
		}
		if (res == 0){
			d->Item.removeItem(itemID, needNum);
		}
		Return(r, res);
	}

	//水晶兑换
	void item_system::updateExchangeItemsInformation(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		//ReadJsonArray
		d->crystalEx.update();
	}

	void item_system::exchangeItems(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray
		int exchangeId = js_msg[0u].asInt();
		int num = js_msg[1u].asInt();
		bool method = js_msg[2u].asInt();
		//1总督等级≥14级
		if (d->Base.getLevel() < 14)
			Return(r, 1);
		//2是否达到已兑换的最大数量
		ExItemsMsg ex = player_crystal_exchange::getExItemsMsg(exchangeId);
		if (num > d->crystalEx.getRemainingNum(exchangeId))
			Return(r, 2);
		//3晶石或水晶数量足
		int exID = 0;
		int exchangeNum = 0;
		int totalCrystalNum = 0;
		itemVector itemVec;
		if (method){
			exchangeNum = ex.costCrystalNum;
			exID = ex.costCrystalID;
		}
		else{
			exchangeNum = ex.costSparNum;
			exID = ex.costSparID;		//晶石
		}
		totalCrystalNum = d->Item.getItemStack(exID);		//水晶
		if (totalCrystalNum < exchangeNum)
			Return(r, 3);
		//消耗晶石资源
		d->Item.removeItem(exID, exchangeNum);
		//4得到兑换物品,如果仓库满就发邮件
		int resultCode = d->crystalEx.exchageItems(ex.ID, ex.exItemsJson);
		if (resultCode == -1)
			Return(r, -1);
		if (resultCode == -2)
			Return(r, 2);

		//log
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(exchangeNum));
		fields.push_back(ex.exItemsJson.toStyledString());
		//fields.push_back(boost::lexical_cast<string, int>(-1));
		StreamLog::Log(lg_crystal_sys, d, boost::lexical_cast<string, int>(totalCrystalNum), boost::lexical_cast<string, int>(d->Item.getItemStack(exID)), fields, exID);
		//成功，更新客服端
		d->crystalEx.update();
		if (resultCode == 1)
			Return(r, 4);
		Return(r, 0);
	}


	void item_system::useItem(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		playerItemPtr uItem = d->Item.getItem(localID);
		if(uItem == playerItemPtr() || !uItem->isVaild())Return(r, 1);
		// 		Params::ObjectValue& config = getConfig(uItem->getItemID());
		// 		if(config.isEmpty() || !config.isMember(ItemDefine::consume))Return(r, -1);
		// 		Params::Var& vecVar = config[ItemDefine::consume][ItemDefine::consumeVector];
		// 		//检验使用限制
		// 		if(!config.isMember(ItemDefine::consumeVector))Return(r, -1);
		actionFormat::actionVec vec = getItemAction(uItem->getItemID());
		if(vec.empty())Return(r, -1);
		Json::Value Param = Json::arrayValue;
		Param = js_msg[1u];
		actionResult resAction = actionFormat::actionDo(d, vec, Param);
		if(resAction.breakID == 0){
			uItem->Remove(1);
		}
		r[msgStr][1u][errorActionJson::breakIDStr] = resAction.breakID;
		r[msgStr][1u][errorActionJson::breakCodeStr] = resAction.resultCode;
		Return(r, 0);
	}

	void item_system::buyCapacity(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		// 		ReadJsonArray;
		// 		int num = js_msg[0u].asInt();
		// 		if(num != 1)Return(r, -1);
		int capacity = d->Item.getCapacity();
		if(capacity >= maxCapacity)Return(r, 1);//背包空间满了
		int costGold = 5 + (capacity - defaultCapacity) * 10;
		costGold  =  costGold < 5 ? 5 : costGold;
		costGold = costGold > 100 ? 100 : costGold;
		if(d->Base.getAllGold() < costGold)Return(r, 2);//金币不足
		d->Base.alterBothGold(-costGold);
		d->Item.addCapacity(1);
		Return(r, 0);
	}

	void item_system::equipRevolution(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < 13)Return(r, -1);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		EqProValue culPro[3],newPro[3];
		//////////////////////////////////////////////////////////////////////////原装备
		playerItemPtr item = d->Item.getItem(localID);
		if(item == playerItemPtr())Return(r, -1);
		equipExPtr equipEx = item->getEquipEx();
		equipEx->getCulSubPro(culPro);
		equipEx->getNewSubPro(newPro);
		if(equipEx == NULL)Return(r, -1);
		int itemID = item->getItemID();
		Params::ObjectValue& equipConfig = getConfig(itemID);
		if(equipConfig.isEmpty() || !equipConfig.isMember(ItemDefine::equipment))Return(r, -1);
		Params::Var& equipVar = equipConfig[ItemDefine::equipment];
		if(equipVar[ItemDefine::equipRevolutionStr].asInt() < 0)Return(r, 1);//当前装备不可以进阶
		//////////////////////////////////////////////////////////////////////////材料
		int materialID = equipVar[ItemDefine::materialIDStr].asInt();
		int needNum = equipVar[ItemDefine::materialCostStr].asInt();
		int revolutionID = equipVar[ItemDefine::equipRevolutionStr].asInt();
		Params::ObjectValue& revolutionConfig = getConfig(revolutionID);
		if(revolutionConfig.isEmpty() || !revolutionConfig.isMember(ItemDefine::equipment))Return(r, -1); 
		Params::Var& revolutionVar = revolutionConfig[ItemDefine::equipment];
		if(revolutionVar[ItemDefine::equipTypeStr].asInt() != equipVar[ItemDefine::equipTypeStr].asInt())Return(r, -1);
		if(!d->Item.canRemoveItem(materialID, needNum))Return(r, 2);//没有足够的材料
		Params::ObjectValue& materialConfig = getConfig(materialID);
		if(materialConfig.isEmpty() || !materialConfig.isMember(ItemDefine::material))Return(r, -1);
		Params::Var& materialVar = materialConfig[ItemDefine::material];
		//////////////////////////////////////////////////////////////////////////检测武将
 		const playerPilot& pilot = d->Pilots.getPlayerPilotExtert(equipEx->getBelong());
 		if(pilot == playerPilots::NullPilotExtert)Return(r, -1);//没有归属武将
		int position = -1;//找出对应的位置
		for (unsigned  i =0; i < pilot.pilotEquipment.size(); ++i)
		{
			if(pilot.pilotEquipment[i] == item){
				position = i;
				break;
			}
		}
		if(position == -1)Return(r, -1);
		if(materialVar[ItemDefine::materialUseLevelStr].asInt() > pilot.pilotLevel)Return(r, 3);//武将等级太低
		//////////////////////////////////////////////////////////////////////////检查是否有足够的钱
		int costSilver = equipVar[ItemDefine::equipRevolutionCostStr].asInt();
		if(costSilver > d->Base.getSilver())Return(r, 4);//钱不够
		//////////////////////////////////////////////////////////////////////////扣除资源
		int crytalID = equipEx->takeOffCrytal();
		d->Item.removeItem(materialID, needNum);
		item->Delete();
		d->Base.alterSilver(-costSilver);
		//////////////////////////////////////////////////////////////////////////增加装备
		playerItemPtr addItem = d->Item.addItem(revolutionID, false);
		addItem->getEquipEx()->embedded(crytalID);
		revolutonConver(addItem->getEquipEx(), equipEx, revolutionVar, equipVar);
		//////////////////////////////////////////////////////////////////////////重新绑定装备状态
		d->Pilots.equip(equipEx->getBelong(), addItem->getLocalID(), position);
		//////////////////////////////////////////////////////////////////////////结果返回
		const static string resItemIDStr = "rid";
		r[msgStr][1u][resItemIDStr] = addItem->getLocalID();
		Params::ObjectValue& addItemConfig = getConfig(addItem->getItemID());
		int itemQuality = addItemConfig[ItemDefine::itemQualityStr].asInt();
		if(!(itemQuality<2)){
			equipExPtr equipExNew = addItem->getEquipEx();
			if(itemQuality==3)
				equipExNew->setCulSubPro(newPro);
			else{
				for(unsigned i = 0;i<EqPVSize;i++){
					int temp = (int)(culPro[i].valuePro*0.75);
					culPro[i].valuePro = 2*culPro[i].valuePro - temp;
				}
				equipExNew->setCulSubPro(culPro);
			}
		}
		task_sys.updateBranchTaskInfo(d, _task_advanced_times);
		task_sys.updateBranchTaskInfo(d, _task_equipment_color, itemQuality);
		task_sys.updateBranchTaskInfo(d, _task_equipment_lv);
		Return(r, 0);
	}

	void item_system::gmSetEquipLevelReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int localID = js_msg[0u].asInt();
		int eqLevel = js_msg[1u].asInt();

		playerItemPtr item = d->Item.getItem(localID);
		if(item == playerItemPtr())Return(r, -1);
		equipExPtr equipPtr = item->getEquipEx();
		if(equipPtr == NULL)Return(r, -1);
		equipPtr->setLv(eqLevel);
		Return(r, 0);
	}

	void item_system::gmSetEquipAddAttributeReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int localID = js_msg[0u].asInt();
		Json::Value attrJson1 = js_msg[1];
		Json::Value attrJson2 = js_msg[2];
		Json::Value attrJson3 = js_msg[3];
		EqProValue pro[EqPVSize];
		vector<int> proType;
		vector<int> proValue;

		if (attrJson1.size() != 2 || attrJson2.size() != 2 || attrJson3.size() != 2 || js_msg.size() != 4)
		{
			Return(r, -1);
		}
		pro[0].typePro = attrJson1[0u].asInt();
		pro[1].typePro = attrJson2[0u].asInt();
		pro[2].typePro = attrJson3[0u].asInt();

		pro[0].valuePro = attrJson1[1u].asInt();
		pro[1].valuePro = attrJson2[1u].asInt();
		pro[2].valuePro = attrJson3[1u].asInt();

		playerItemPtr item = d->Item.getItem(localID);
		if(item == playerItemPtr())Return(r, -1);

		ObjectValue itemObj = iMap[item->getItemID()];
		int itemQuality = itemObj[ItemDefine::itemQualityStr].asInt();
		if(itemQuality<3)
			Return(r, 1);
		equipExPtr equipEx = item->getEquipEx();
		equipEx->setNewSubPro(pro);

		if (attrJson1[0u].asInt() == attrJson2[0u].asInt()
			|| attrJson2[0u].asInt() == attrJson3[0u].asInt()
			|| attrJson3[0u].asInt() == attrJson1[0u].asInt())
		{
			Return(r, 3);
		}
		equipEx->replaceSubPro();
		Return(r, 0);
	}

	void item_system::gmPlayerItemUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int startIndex = js_msg[0u].asInt();
		int endIndex = js_msg[1].asInt();
		if (startIndex < 0 || endIndex < 0 || startIndex > endIndex)
			Return(r, -1);

		Json::Value updateJson, itemsJson;
		updateJson[msgStr][0u] = 0;
		updateJson[msgStr][1u][playerItemDataStr] = Json::arrayValue;
		Json::Value& updateItems = updateJson[msgStr][1u][playerItemDataStr];
		itemVector vec = d->Item.getAllItem(position_ware | position_equip_space);
		updateJson[msgStr][1u]["tn"] = (unsigned)vec.size();
		for (int  i = startIndex; i <= endIndex; ++i)
		{
			if (i > (int)vec.size() - 1)
				break;
			updateItems.append(vec[i]->toJson());
		}

		string s = updateJson.toStyledString();
		na::msg::msg_json mj(m._player_id, m._net_id, gate_client::gm_player_item_update_resp, s);
		game_svr->async_send_to_gate(mj);
	}

	actionFormat::actionVec item_system::getItemAction(const int itemID)
	{
		actionItemMap::iterator it = aIMap.find(itemID);
		if(it != aIMap.end())return it->second;
		return actionFormat::actionVec();
	}

	actionFormat::actionVec item_system::getItemDisint(const int itemID)
	{
		actionItemMap::iterator it = DstMap.find(itemID);
		if (it != DstMap.end())return it->second;
		return actionFormat::actionVec();
	}

	void item_system::revolutonConver(equipExPtr dest_, equipExPtr source_, Params::Var& configDest_, Params::Var& configSource_)
	{
		if (!configDest_.isMember(ItemDefine::equipUpgrdeCostStr))	return;
		int sourceLv = source_->getLv();
		int totalCost = configSource_[ItemDefine::equipUpgrdeCostStr][sourceLv - 2].asInt();
		Params::Var& var = configDest_[ItemDefine::equipUpgrdeCostStr];
		int maxLv = (int)var.getSize() + 1;
		int destIdx = -1;
		for (unsigned i = 0; i < var.getSize(); ++i)
		{
			if(totalCost < var[i].asInt())
			{
				break;
			}
			destIdx = i;
		}
		destIdx += 2;
		destIdx = destIdx < 1 ? 1 : destIdx;
		destIdx = destIdx > maxLv ? maxLv : destIdx;
		dest_->setLv(destIdx);
	}

	void item_system::equipSuccinct(msg_json& m, Json::Value& r)
	{
		return;
		AsyncGetPlayerData(m);
		ReadJsonArray;
	}

	void item_system::degradeEquipReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		bool isClean = js_msg[1u].asBool();
		playerItemPtr uItem = d->Item.getItem(localID);
		if (uItem == NULL || !uItem->isVaild())Return(r, -1);
		equipExPtr eqPtr = uItem->getEquipEx();
		if (eqPtr == NULL)Return(r, -1);
		int level = eqPtr->getLv();
		if (level < 2)Return(r, -1);
		Params::ObjectValue& obj = getConfig(uItem->getItemID());
		ObjectValue& config = getConfig(uItem->getItemID());
		if (config.isEmpty() || !config.isMember(ItemDefine::equipment)) Return(r, -1);
		Params::Var& costConfig = config[ItemDefine::equipment][ItemDefine::equipUpgrdeCostStr];
		int resSilver = 0;
		if (isClean)
		{
			resSilver = int(costConfig[level - 2].asInt() * 0.8);
			eqPtr->setLv(1);
		}
		else
		{
			resSilver = int((costConfig[level - 2].asInt() - costConfig[level - 3].asInt()) * 0.8);
			eqPtr->setLv(level - 1);
		}
		resSilver = resSilver < 0 ? 0 : resSilver;
		d->Base.alterSilver(resSilver);
		r[msgStr][1u] = resSilver;
		r[msgStr][2u] = eqPtr->getLv();
		Return(r, 0);
	}

	void item_system::upgradeEquip(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int VIPLV= d->Vip.getVipLevel();
		int localID = js_msg[0u].asInt();
		ObjectValue& config = getConfig(localID / itemOffset);		
		if(config.isEmpty() || !config.isMember(ItemDefine::equipment)) Return(r,-1);
		Params::Var& costConfig = config[ItemDefine::equipment][ItemDefine::equipUpgrdeCostStr];
		int maxlv =  (int)costConfig.getSize() + 1;
		playerItemPtr item = d->Item.getItem(localID);
		if(playerItemPtr() == item)Return(r, -1);
		equipExPtr equipPtr = item->getEquipEx();
		if(equipPtr == NULL)Return(r, -1);
		int level = equipPtr->getLv();
		if(level < 1)Return(r, -1);
		if(level >= d->Base.getLevel())Return(r, 100);//不能超过主角等级
		int costSilver = costConfig[level - 1].asInt() - costConfig[level - 2].asInt();
		if(d->Base.getSilver() < costSilver) Return(r, 101);//钱不够
		if(VIPLV < 1 && d->Item.getCD().Lock)Return(r, 102);//强化cd中
		if(level >= maxlv)Return(r, 1);//等级满了
		++level;
		if(VIPLV > 2 && commom_sys.randomOk(0.1))++level;
		level = level > maxlv ? maxlv : level;
		level = level > d->Base.getLevel() ? d->Base.getLevel() : level;
		r[msgStr][1u] = level - equipPtr->getLv();
		equipPtr->setLv(level);
		d->Base.alterSilver(-costSilver);
		if(VIPLV < 1)d->Item.addCD(30);
		activity_sys.updateActivity(d, 0, activity::_strengthen);
		task_sys.updateBranchTaskInfo(d, _task_equipment_lv);
		Json::Value udJson;
		packageBaseItemSys(d, udJson);
		d->sendToClient(gate_client::item_system_info_update_resp, udJson);
		Return(r, 0);
	}

	void item_system::replaceEqAttribut(msg_json& m,Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		playerItemPtr item = d->Item.getItem(localID);
		if(item == playerItemPtr())Return(r, -1);
		equipExPtr eqExPtr = item->getEquipEx();
		if(eqExPtr == NULL)Return(r, -1);
		ObjectValue& itemObj = getConfig(item->getItemID());
		int itemQuality = itemObj[ItemDefine::itemQualityStr].asInt();
		if(itemQuality<3)Return(r,-1);
		EqProValue eqVal[EqPVSize];
		eqExPtr->replaceSubPro();
		Return(r,0);

	}

	void item_system::keepEqAttribut(msg_json& m,Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		playerItemPtr item = d->Item.getItem(localID);
		if(item == playerItemPtr())Return(r, -1);
		equipExPtr eqExPtr = item->getEquipEx();
		if(eqExPtr == NULL)Return(r, -1);
		ObjectValue& itemObj = getConfig(item->getItemID());
		int itemQuality = itemObj[ItemDefine::itemQualityStr].asInt();
		if(itemQuality<3)Return(r,-1);
		EqProValue eqVal[EqPVSize];
		eqExPtr->setNewSubPro(eqVal);
		Return(r,0);
	}

	//普通改造
	bool compareEqPro(const EqProValue P1[EqPVSize], const EqProValue P2[EqPVSize])
	{
		bool ret = true;
		for (int i = 0; i < EqPVSize; ++i)
		{
			if (P1[i] == P2[i])continue;
			ret = false;
		}
		return ret;
	}
	void item_system::ordinaryTransformEquip(msg_json& m,Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int cost = 0;
		int localID = js_msg[0u].asInt();
		playerItemPtr item = d->Item.getItem(localID);
		if(item == playerItemPtr())Return(r,-1);
		equipExPtr equipEx = item->getEquipEx();
		if (equipEx == NULL)Return(r, -1);
		//判断装备是否穿戴
		const playerPilot& pilot = d->Pilots.getPlayerPilotExtert(equipEx->getBelong());
		if(pilot == playerPilots::NullPilotExtert)Return(r, -1);//没有归属武将
		int itemID = item->getItemID();
		ObjectValue itemObj = iMap[itemID];
		int itemQuality = itemObj[ItemDefine::itemQualityStr].asInt();
		//装备品质
		if(m_EqAttrBaseMap.find(itemQuality)==m_EqAttrBaseMap.end())	
			Return(r,1);

		if(d->Base.getSilver()<
			(cost = (int)ceil(m_EqAttrBaseMap[itemQuality].eq_silver*(1+activity_sys.getRate(param_transform_cost_silver,d)))))	
			Return(r,2);
		d->Base.alterSilver(-cost);
		if (WashSpecial::GuideWashItem == item->getItemID())
		{
			do 
			{
				EqProValue cCul[EqPVSize], cWash[EqPVSize];
				equipEx->getCulSubPro(cCul);
				equipEx->getNewSubPro(cWash);
				if (compareEqPro(cCul, WashSpecial::Default) && compareEqPro(cWash, WashSpecial::WashNull))
				{
					equipEx->setNewSubPro(WashSpecial::Wash1);
				}
				else if (compareEqPro(cCul, WashSpecial::Wash1) || compareEqPro(cWash, WashSpecial::Wash1))
				{
					equipEx->setNewSubPro(WashSpecial::Wash2);
				}
				else
				{
					break;
				}
				Return(r, 0);
			} while (false);
		}
		this->startTransfromEq(item,itemQuality,false);
		Return(r,0);
	}

	void item_system::directionalTransformEquip(msg_json& m,Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int localID = js_msg[0u].asInt();
		int cost = 0,eqProperty[3]={0};
		eqProperty[0] = js_msg[1u].asInt();
		eqProperty[1] = js_msg[2u].asInt();
		eqProperty[2] = js_msg[3u].asInt();

		playerItemPtr item = d->Item.getItem(localID);
		if(item == playerItemPtr())Return(r,-1);
		equipExPtr equipEx = item->getEquipEx();
		//判断装备是否穿戴
		const playerPilot& pilot = d->Pilots.getPlayerPilotExtert(equipEx->getBelong());
		if(pilot == playerPilots::NullPilotExtert)Return(r, -1);//没有归属武将
		int itemID = item->getItemID();
		ObjectValue itemObj = iMap[itemID];
		int itemQuality = itemObj[ItemDefine::itemQualityStr].asInt();
		//装备品质
		if(m_EqAttrBaseMap.find(itemQuality)==m_EqAttrBaseMap.end())	
			Return(r,1);

		//定向洗练只有开通VIP2以上才可以使用
		//if(d->Vip.getVipLevel()<2)	
		//	Return(r,3);
		int goldId = 0;
		for (int i=0;i<3;i++){
			if (eqProperty[i]==2)
				goldId++;
		}
		if(goldId==3)	
			Return(r,4);
		if(d->Base.getAllGold()<
			(cost=(int)ceil(m_EqAttrBaseMap[itemQuality].eq_gold[goldId]*(1+activity_sys.getRate(param_transform_cost_gold,d)))))
			Return(r,5);
		d->Base.alterBothGold(-cost);
		this->startTransfromEq(item,itemQuality,true,eqProperty);
		Return(r,0);
	}


	void item_system::startTransfromEq(playerItemPtr item,int itemQuality,bool method /*= false*/,int eqProperty[3])
	{
		if(method)
			this->directionalWashsPractice(item->getEquipEx(),itemQuality,eqProperty);//定向洗脸
		else
			this->ordinaryWashsPractice(item->getEquipEx(),itemQuality);	//普通洗脸
	}

	void item_system::Wash(EqProValue outVal[EqPVSize], int itemQuality)
	{
		vector<int> proVec(3,0);
		vector<int> colorVec(3,0);
		vector<int> proData(3,0);
		vector<int> colorTemp(1,0);
		vector<int> flagPro(11,-1), flagColor(11,-1);
		//装备品质
		if(m_EqAttrBaseMap.find(itemQuality)==m_EqAttrBaseMap.end())	
			return;
		EqRangeMap rangeMap = m_EqAttrBaseMap[itemQuality].eq_range;
		if(!m_AttributePro.attributeProVal.empty()&&!m_ColorPro.colorProVal.empty()&&!rangeMap.empty())
		{
			//确定属性的类型
			this->randProValue(proVec,3,flagPro,m_AttributePro.attributeProVal);
			//确定颜色
			this->randProValue(colorTemp,1,flagColor,m_ColorPro.colorProVal);
			colorVec[0] = colorTemp[0];flagColor[colorTemp[0]] = -1;
			this->randProValue(colorTemp,1,flagColor,m_ColorPro.colorProVal);
			colorVec[1] = colorTemp[0];flagColor[colorTemp[0]] = -1;
			this->randProValue(colorTemp,1,flagColor,m_ColorPro.colorProVal);
			colorVec[2] = colorTemp[0];flagColor[colorTemp[0]] = -1;
			//确定3个副属性值
			for (int i=0;i<3;i++)
			{
				if(proVec[i] == 10)
					proVec[i] = idx_hp;
				EqAttributeRange range = rangeMap[proVec[i]];
				int lowerValue = range.lowerValue[colorVec[i]];
				int upperValue =range.upperValue[colorVec[i]];
				proData[i] = commom_sys.randomBetween(lowerValue,upperValue);
			}		
			for (int i=0;i<EqPVSize;i++)
			{
				outVal[i].typePro = proVec[i];
				outVal[i].valuePro = proData[i];
			}
		}
	}

	void item_system::ordinaryWashsPractice(equipExPtr equipEx,int itemQuality)
	{
		if(equipEx==NULL)return;
		EqProValue pro[EqPVSize];
		Wash(pro, itemQuality);
		equipEx->setNewSubPro(pro);
	}


	void item_system::directionalWashsPractice(equipExPtr equipEx,int itemQuality,int eqProperty[3]/*=0*/)
	{
		if(equipEx == NULL)
			return;
		vector<int> proVec(3,0);
		vector<int> colorVec(3,0);
		vector<int> proData(3,0);
		vector<int> colorTemp(1,0);
		vector<int> flagPro(11,-1), flagColor(11,-1);
		EqProValue pro[EqPVSize];
		//装备品质
		if(m_EqAttrBaseMap.find(itemQuality)==m_EqAttrBaseMap.end())	
			return;
		EqRangeMap rangeMap = m_EqAttrBaseMap[itemQuality].eq_range;
		if(!m_AttributePro.attributeProVal.empty()&&!m_ColorPro.colorProVal.empty()&&!rangeMap.empty())
		{
			vector<int> proVecTemp;
			vector<int> proDataTemp;
			equipEx->getCulSubPro(pro);
			for(int i = 0;i<3;i++){
				proVecTemp.push_back(pro[i].typePro);
				proDataTemp.push_back(pro[i].valuePro);
			}
			for (int i=0;i<EqPVSize;i++){
				if(eqProperty[i]!=0){
					if(proVecTemp[i]==idx_hp)
						flagPro[10]=0;
					else
						flagPro[proVecTemp[i]]=0;
				}
			}
			//确定属性的类型
			this->randProValue(proVec,3,flagPro,m_AttributePro.attributeProVal);
			//确定颜色
			this->randProValue(colorTemp,1,flagColor,m_ColorPro.colorProVal);
			colorVec[0] = colorTemp[0];flagColor[colorTemp[0]] = -1;
			this->randProValue(colorTemp,1,flagColor,m_ColorPro.colorProVal);
			colorVec[1] = colorTemp[0];flagColor[colorTemp[0]] = -1;
			this->randProValue(colorTemp,1,flagColor,m_ColorPro.colorProVal);
			colorVec[2] = colorTemp[0];flagColor[colorTemp[0]] = -1;
			//确定3个副属性值
			for (int i=0;i<3;i++)
			{
				if(proVec[i] == 10)
					proVec[i] = idx_hp;
				if(eqProperty[i]==1)
					proVec[i] = proVecTemp[i];
				if(eqProperty[i]==2){
					proVec[i] = proVecTemp[i];
					proData[i] = proDataTemp[i];
				}else{
					EqAttributeRange range = rangeMap[proVec[i]];
					int lowerValue = range.lowerValue[colorVec[i]];
					int upperValue =range.upperValue[colorVec[i]];
					proData[i] = commom_sys.randomBetween(lowerValue,upperValue);
				}
			}
		}
		for (int i=0;i<EqPVSize;i++)
		{
			pro[i].typePro = proVec[i];
			pro[i].valuePro = proData[i];
		}
		equipEx->setNewSubPro(pro);
	}

	int item_system::randProValue(vector<int>& arr,int num,vector<int>& flag,vector<double> &vec)
	{
		if((unsigned)num>arr.size())
			return 1;
		if(vec.size()>flag.size())
			return 2;

		for (int i=0;i<num;i++)
		{
			bool isContinue = true;
			while(isContinue){
				double total = 0;
				double r = commom_sys.randomBetween(0, 1000)/1000.0f;
				for(unsigned j = 0;j<vec.size();j++){
					total += vec[j];
					if(r<total){
						if(flag[j]==-1){
							arr[i]=j;flag[j] = 0;
							isContinue = false;
							break;
						}
					}
				}
			}
		}
		return 0;
	}
	item_system* const item_system::itemSys = new item_system();

	static Json::Value NullJsonValue;
	static ObjectValue NullConfig = ObjectValue(NullJsonValue);
	ObjectValue& item_system::getConfig(const int itemID)
	{
		itemMap::iterator it = iMap.find(itemID);
		if(it == iMap.end())return NullConfig;
		return it->second;
	}


	const static string itemDataDirStr = "./instance/items/";
	const static string equipAttributeProb ="./instance/TransEquip/eq_attribute/attribute_effect.json";			//属性概率
	const static string equipAttributeColoer = "./instance/TransEquip/eq_attribute/attribute_color.json";				//颜色概率
	const static string equipAttributeVal = "./instance/TransEquip/eq_attribute_value";	//不同品质装备的每个属性所对应的数值范围（总共是11个属性）
	const static string crystalExchangeItem = "./instance/crystal/";

	void item_system::initData()
	{
		db_mgr.ensure_index(playerWareDBstr, BSON(playerIDStr << 1));
		db_mgr.ensure_index(playerItemDBStr, BSON(playerIDStr << 1 << playerLocalIDstr << 1));

		const static string equipAttributeStr = "attribute";
		const static string equipIndexStr = "attriIndex";

		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(itemDataDirStr, jv);
		for (unsigned i = 0; i < jv.size(); ++i)
		{
			Json::Value& item = jv[i];
			Json::Value actionJson = Json::nullValue;
			if(item.isMember(ItemDefine::consume) && item[ItemDefine::consume].isArray())
			{
				actionJson = item[ItemDefine::consume];
				item.removeMember(ItemDefine::consume);
			}
			Json::Value DstJson = Json::nullValue;
			if (item.isMember(ItemDefine::disintegrator))
			{
				if (item[ItemDefine::disintegrator].isMember(ItemDefine::disintegrateAction) && item[ItemDefine::disintegrator][ItemDefine::disintegrateAction].isArray())
				{
					DstJson = item[ItemDefine::disintegrator][ItemDefine::disintegrateAction];
					item[ItemDefine::disintegrator].removeMember(ItemDefine::disintegrateAction);
				}
			}
			if(item.isMember(ItemDefine::equipment) && item[ItemDefine::equipment].isObject())
			{
				Json::Value& equipment = item[ItemDefine::equipment];
				Json::Value attriJson = equipment[equipAttributeStr];
				for (unsigned i = 0;i < characterNum; ++i)
				{
					equipment[ItemDefine::equipValueStr][i] = 0.0;
					equipment[ItemDefine::equipGrowStr][i] = 0.0;
				}
				for (unsigned i = 0; i < attriJson.size(); ++i){
					int index = attriJson[i][equipIndexStr].asInt();
					if(index < 0 || index >= characterNum) continue;
					equipment[ItemDefine::equipValueStr][index] = attriJson[i][ItemDefine::equipValueStr].asDouble();
					equipment[ItemDefine::equipGrowStr][index] = attriJson[i][ItemDefine::equipGrowStr].asDouble();
				}
				item.removeMember(equipAttributeStr);
			}

			//

			ObjectValue itemObj(item);
			int itemID = itemObj[ItemDefine::itemIDStr].asInt();
			int itemStack =itemObj[ItemDefine::itemStackStr].asInt();
			int itemQuality = itemObj[ItemDefine::itemQualityStr].asInt();
			if(itemID < 0)continue;
			bool isEquip = itemObj.isMember(ItemDefine::equipment);
			if(isEquip && itemStack != 1)continue;
			if(itemStack < 1)continue;
			if(itemQuality < 0)continue;
			if(!actionJson.isNull())
			{
				actionFormat::actionVec vec = actionFormat::formatActionVec(actionJson);
				aIMap[itemID] = vec;
			}
			if (!DstJson.isNull())
			{
				actionFormat::actionVec vec = actionFormat::formatActionVec(DstJson);
				DstMap[itemID] = vec;
			}
			iMap[itemID] = itemObj;
		}
		LogS << iMap.size() << LogEnd;


		
		//装备附属配置文件
		//equipAttributeProb
		Json::Value pro = na::file_system::load_jsonfile_val(equipAttributeProb);
		for (unsigned i = 0; i < pro.size(); ++i)
		{
			m_AttributePro.attributeProVal.push_back(pro[i].asDouble());
		}
		//equipAttributeColoer
		Json::Value color = na::file_system::load_jsonfile_val(equipAttributeColoer);
		for (unsigned i = 0;i<color.size();++i)
		{
			m_ColorPro.colorProVal.push_back(color[i].asDouble());
		}
		//equipAttributeVal
		jv.clear();
		na::file_system::load_jsonfiles_from_dir(equipAttributeVal,jv);
		for (unsigned i=0;i<jv.size();i++)
		{
			EqAttributeBase base;
			base.eq_attr_id = jv[i]["id"].asInt();

			base.eq_silver = jv[i]["silver"].asInt();
			Json::Value& item = jv[i];
			if(item.isMember("gold") && item["gold"].isArray())
			{
				Json::Value& item1 = item["gold"];
				for (unsigned i=0;i<item1.size();i++)
				{
					base.eq_gold.push_back( item1[i].asInt());
				}
			}
			if(item.isMember("value")&&item["value"].isArray()){
				Json::Value& itemArry = item["value"];
				for (unsigned i=0;i<itemArry.size();i++)
				{
					Json::Value& itemArryVal = itemArry[i];
					EqAttributeRange range;
					range.proID = itemArryVal["pid"].asInt();
					Json::Value& itemValue = itemArryVal["lower"];
					for (unsigned j=0;j<itemValue.size();j++)
					{
						range.lowerValue.push_back(itemValue[j].asInt());
					}

					itemValue = itemArryVal["upper"];
					for (unsigned j=0;j<itemValue.size();j++)
					{
						range.upperValue.push_back(itemValue[j].asInt());
					}
					base.eq_range[range.proID] = range;	
				}
			}
			m_EqAttrBaseMap[base.eq_attr_id] = base;
		}

		//水晶兑换初始信息
		//数据库加入索引
		db_mgr.ensure_index(player_crystal_exchange_db_name, BSON(playerIDStr << 1));

		jv.clear();
		na::file_system::load_jsonfiles_from_dir(crystalExchangeItem, jv);
		ExItemsMap exItem;
		for (unsigned i = 0; i < jv.size(); i++)
		{
			ExItemsMsg msg;
			msg.ID = jv[i]["ID"].asInt();
			msg.costCrystalID = jv[i]["crystalID"].asInt();
			msg.costCrystalNum = jv[i]["crystalNum"].asInt();
			msg.costSparID = jv[i]["sparID"].asInt();
			msg.costSparNum = jv[i]["sparNum"].asInt();
			msg.maxNum = 1;
			msg.exItemsJson = jv[i]["itemJson"];
			//LogW << "!!!!!!!!" << msg.exItemsJson.toStyledString() << LogEnd;
			exItem[msg.ID] = msg;
		}
		player_crystal_exchange::setExItemsMsg(exItem);

	}
}