#include "pilotManager.h"
#include "file_system.h"
#include "json/json.h"
#include "response_def.h"
#include "helper.h"
#include "item_system.h"
#include "commom.h" 
#include "task_system.h"
#include "record_system.h"
#include "activity_system.h"
//#include <math.h>

namespace gg
{
	const static string lg_player_pilot = "log_pilot_mgr";

	pilotConfig::pilotConfig()
	{
		skill_1 = Params::VarPtr();
		skill_2 = Params::VarPtr();
		armsR.clear();
		resistList.clear();
	}

	void pilotManager::pilotsUpdateBase(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Pilots.update();
	}

	void pilotManager::pilotsUpdate(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		bool updatePilot = js_msg[0u].asBool();
		d->Pilots.update(updatePilot);
	}

	void pilotManager::pilotsSingleUpdate(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		if (!js_msg.isArray() || js_msg.size() > 20)
		{
			Return(r, -1);
		}
		bool err = true;
		for (unsigned i = 0; i < js_msg.size(); ++i)
		{
			int pilotID = js_msg[0u].asInt();
			bool ret = d->Pilots.singleUpdate(pilotID);
			err = (err && ret) ? false : true;
		}
		if (err)Return(r, 1);
		Return(r, 0);
	}

	void pilotManager::oneKeyEquip(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		if (!js_msg.isArray())Return(r, -1);
		if (!js_msg[0u].isInt() || !js_msg[1u].isInt())Return(r, -1);
		int pilotID_1 = js_msg[0u].asInt();
		int pilotID_2 = js_msg[1u].asInt();
		Return(r, d->Pilots.EqiuipChange(pilotID_1, pilotID_2));
	}

	void pilotManager::totalPilotReward(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int option = js_msg[0u].asInt();
	}

#define IfEquipMsgCheck(JSON)\
	if(JSON[0u].isInt() && JSON[1u].isInt() && JSON[2u].isInt())

	void pilotManager::pilotEquip(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		if(!js_msg.isArray())Return(r, -1);
		IfEquipMsgCheck(js_msg)
		{
			int pilotID = js_msg[0u].asInt();
			int equipID = js_msg[1u].asInt();
			int positionID = js_msg[2u].asInt();
			int res = d->Pilots.equip(pilotID, equipID, positionID);
			Return(r, res);
		}
		else
		{
			if(js_msg.size() < 1 || js_msg.size() > 6)Return(r, -1);
			for (unsigned i = 0; i < js_msg.size(); ++i)
			{
				Json::Value& equipJson = js_msg[i];
				IfEquipMsgCheck(equipJson)
				{
					int pilotID = equipJson[0u].asInt();
					int equipID = equipJson[1u].asInt();
					int positionID = equipJson[2u].asInt();
					int res = d->Pilots.equip(pilotID, equipID, positionID);
					Json::Value resJson;
					resJson.append(pilotID);
					resJson.append(equipID);
					resJson.append(positionID);
					resJson.append(res);
					r[msgStr][1u].append(resJson);
				}
			}
			Return(r, 0);
		}
	}

	void pilotManager::pilotOffEquip(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int pilotID = js_msg[0u].asInt();
		int positionID = js_msg[1u].asInt();
		int res = d->Pilots.offEquip(pilotID, positionID);
		Return(r, res);
	}
	 
	void pilotManager::pilotFire(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int pilotID = js_msg[0u].asInt();
		int res = d->Pilots.fire(pilotID);
		Return(r, res);
	}

	void pilotManager::pilotEnlist(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int proValue = 0;
		int pilotID = js_msg[0u].asInt();
		const pilotConfig& pG = getPilotConfig(pilotID);
		if(!pG.isVaild()) Return(r, -1);
		int costSilver = pG.enlistFee;
		if((proValue=d->Base.getSilver()) < costSilver)Return(r, 2);
		int res = d->Pilots.enlist(pilotID);
		if(res == 0)d->Base.alterSilver(-costSilver);
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string ,int>(action_add_silver));
		fields.push_back(boost::lexical_cast<string ,int>(pilotID));
		fields.push_back(boost::lexical_cast<string ,int>(0));
		fields.push_back(boost::lexical_cast<string ,int>(-1));
		fields.push_back(boost::lexical_cast<string ,int>(-1));
		fields.push_back(boost::lexical_cast<string ,int>(-1));
		StreamLog::Log(lg_player_pilot, d, boost::lexical_cast<string, int>(proValue), boost::lexical_cast<string, int>(proValue-costSilver),fields, 4);
		Return(r, res);
	}


	void pilotManager::upgradeAttribute(msg_json& m, Json::Value& r){
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int kejiPr = 0;			//科技之前的值
		int goldPr = 0;			//金币之前的值
		int goldTPr = 0;		//金票之前的值
		int pilotID		= js_msg[0u].asInt(); // 武将ID
		int upgradeType = js_msg[1u].asInt(); // 1普通洗练, 2,加强属性， 2 白金洗练, 3 至尊洗练
	 
		int costGold	= 0;
		int costKeji	= 480u;

		int lv = d->Base.getLevel();
		if (lv < 43) Return(r, -1);			//等级限制  43级
		if ((unsigned)lv > this->peiyanCostKejiConfigVect.size()){
			costKeji = this->peiyanCostKejiConfigVect[this->peiyanCostKejiConfigVect.size() - 1];
		}else{
			costKeji = this->peiyanCostKejiConfigVect[lv - 1];
		}

		if (upgradeType < 1 || upgradeType > 4) Return(r, 1)
		switch(upgradeType){
		case 2:
			costGold = 2;
			costKeji = 0;
			break;
		case 3:
			costGold  = 10;
			costKeji  = 0;
			break;
		case 4:
			costGold  = 50;
			costKeji  = 0;
			break;
		}
		
		if (costGold > d->Base.getAllGold() )Return(r, 2);//不够金币
		
		if (costKeji > d->Base.getKeJi()) Return(r, 3);//科技不够了

		if (!d->Pilots.checkPilotEnlist(pilotID)) Return(r, 4);//武将还没招募

		const pilotConfig& config = getPilotConfig(pilotID);
		if(!config.isVaild()) Return(r, -1);
		goldPr = d->Base.getGold();
		goldTPr = d->Base.getGoldTicket();
		kejiPr = d->Base.getKeJi();
		d->Base.alterBothGold(-costGold);
		d->Base.alterKeJi(-costKeji);
		if(0 != d->Pilots.upgradeAttribute(pilotID, upgradeType)) Return(r, -1);
		//记录日志
		bool recordSecflas = false;
		int cost;
		int costType = 0,proTemp = 0;
		if(costGold<=d->Base.getGoldTicket()){
			costType = (costKeji != 0 ? action_add_keji:action_add_gold_ticket);
			proTemp = (costType == action_add_keji? kejiPr:goldTPr);
			cost = costKeji+costGold;
		}else if(d->Base.getGoldTicket() == 0){
			costType = costKeji != 0 ? action_add_keji:action_add_gold;
			proTemp = (costType == action_add_keji? kejiPr:goldPr);
			cost = costKeji+costGold;
		}else{
			recordSecflas = true;
			proTemp = d->Base.getGoldTicket();
			cost = d->Base.getGoldTicket();
		}
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string ,int>(costType));
		fields.push_back(boost::lexical_cast<string ,int>(pilotID));
		fields.push_back(boost::lexical_cast<string ,int>(0));
		fields.push_back(boost::lexical_cast<string ,int>(d->Pilots.getPlayerPilotExtert(pilotID).newAttribute[0]));
		fields.push_back(boost::lexical_cast<string, int>(d->Pilots.getPlayerPilotExtert(pilotID).newAttribute[1]));
		fields.push_back(boost::lexical_cast<string, int>(d->Pilots.getPlayerPilotExtert(pilotID).newAttribute[2]));
		StreamLog::Log(lg_player_pilot, d, boost::lexical_cast<string, int>(proTemp), boost::lexical_cast<string, int>(proTemp-cost),fields, upgradeType);
		if(recordSecflas){
			fields[1] = boost::lexical_cast<string ,int>(action_add_gold);
			proTemp = d->Base.getGold();
			cost = (costKeji+costGold)-cost;
			StreamLog::Log(lg_player_pilot, d, boost::lexical_cast<string, int>(proTemp), boost::lexical_cast<string, int>(proTemp-cost),fields, upgradeType);
		}
		helper_mgr.insertSaveAndUpdate(&d->Pilots);
		activity_sys.updateActivity(d, 0, activity::_bring_up);
		Return(r, 0);
	}

	void pilotManager::replaceAttribute(msg_json& m, Json::Value& r){
		AsyncGetPlayerData(m);
		ReadJsonArray;
		vector<string> fields;
		int opType;
		int pro[3];
		int pilotID	= js_msg[0u].asInt(); // 武将ID
		bool is_clear = false;
		if (js_msg[1u] != Json::nullValue){
			is_clear = true;
		}
		
		if (0 >= pilotID) Return(r, -1);
		if (!is_clear){
			pro[0] = d->Pilots.getPlayerPilotExtert(pilotID).addAttribute[0];
			pro[1] = d->Pilots.getPlayerPilotExtert(pilotID).addAttribute[1];
			pro[2] = d->Pilots.getPlayerPilotExtert(pilotID).addAttribute[2];
			if (0 != d->Pilots.replaceAttribute(pilotID))
				Return(r, -1);
			opType = 7;
			fields.push_back(boost::lexical_cast<string, int>(pro[0]));
			fields.push_back(boost::lexical_cast<string, int>(pro[1]));
			fields.push_back(boost::lexical_cast<string, int>(pro[2]));
			fields.push_back(boost::lexical_cast<string, int>(d->Pilots.getPlayerPilotExtert(pilotID).addAttribute[0]));
			fields.push_back(boost::lexical_cast<string, int>(d->Pilots.getPlayerPilotExtert(pilotID).addAttribute[1]));
			fields.push_back(boost::lexical_cast<string, int>(d->Pilots.getPlayerPilotExtert(pilotID).addAttribute[2]));
		}else{
			pro[0] = d->Pilots.getPlayerPilotExtert(pilotID).newAttribute[0];
			pro[1] = d->Pilots.getPlayerPilotExtert(pilotID).newAttribute[1];
			pro[2] = d->Pilots.getPlayerPilotExtert(pilotID).newAttribute[2];
			if(0 != d->Pilots.clearAttribute(pilotID))
				Return(r, -1);
			opType = 8;
			fields.push_back(boost::lexical_cast<string, int>(pro[0]));
			fields.push_back(boost::lexical_cast<string, int>(pro[1]));
			fields.push_back(boost::lexical_cast<string, int>(pro[2]));
			fields.push_back(boost::lexical_cast<string, int>(d->Pilots.getPlayerPilotExtert(pilotID).newAttribute[0]));
			fields.push_back(boost::lexical_cast<string, int>(d->Pilots.getPlayerPilotExtert(pilotID).newAttribute[1]));
			fields.push_back(boost::lexical_cast<string, int>(d->Pilots.getPlayerPilotExtert(pilotID).newAttribute[2]));
		}
		
		StreamLog::Log(lg_player_pilot, d, boost::lexical_cast<string, int>(0), boost::lexical_cast<string, int>(0), fields, opType);
		helper_mgr.insertSaveAndUpdate(&d->Pilots);
		Return(r, 0);
	}
	
	void pilotManager::converMode(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int pilotID	= js_msg[0u].asInt(); // 武将ID
		int mode = js_msg[1u].asInt();
		int res = d->Pilots.convertMode(pilotID, mode, true);
		r[msgStr][1u] = mode;
		Return(r, res);
	}

	void pilotManager::useItemCDClear(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int res = d->Pilots.useItemCDClear();
		Return(r, res);
	}

	void pilotManager::pilotUpgrade(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int pilotID	= js_msg[0u].asInt(); // 武将ID
		int itemID = js_msg[1u].asInt();//使用物品id
		int times = js_msg[2u].asInt();// 使用物品次数
		int proValue = d->Item.getItemStack(itemID);
		if(d->Pilots.IsLockUseItemCD())Return(r, 1);
		if(!d->Pilots.canUpgrade(pilotID))Return(r, 2);

		if(times < 1)Return(r, -1);//之后要加上次数和等级限制
		if(! configUpgradeItem(itemID))Return(r, -1);

		Params::ObjectValue& config = item_sys.getConfig(itemID);
		if(config.isEmpty() || !config.isMember(ItemDefine::merchandise))Return(r, -1);
		Params::Var& material = config[ItemDefine::material];
		if(! d->Item.canRemoveItem(itemID, times))Return(r, 3);//道具不够
		int level = d->Base.getLevel();
		if(level < material[ItemDefine::materialUseLevelStr].asInt())Return(r, 4);//没有满足道具使用的等级
		int exp = material[ItemDefine::itemProvideIntStr][level].asInt() * times;
		exp = int(exp * (1 + activity_sys.getRate(activity::_tu_fei_meng_jin, d)));
		d->Pilots.addExp(pilotID, exp);
		d->Pilots.addUseItemCD(600 * (unsigned)times);
		d->Item.removeItem(itemID, times);
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string ,int>(action_add_item));
		fields.push_back(boost::lexical_cast<string ,int>(pilotID));
		fields.push_back(boost::lexical_cast<string ,int>(itemID));
		fields.push_back(boost::lexical_cast<string ,int>(-1));
		fields.push_back(boost::lexical_cast<string ,int>(-1));
		fields.push_back(boost::lexical_cast<string ,int>(-1));
		StreamLog::Log(lg_player_pilot, d, boost::lexical_cast<string, int>(proValue), boost::lexical_cast<string, int>(proValue-times),fields, 5);
		task_sys.updateBranchTaskInfo(d, _task_use_exp_item);
		Return(r, 0);
	}

// 	void pilotManager::coreFast(msg_json& m, Json::Value& r)
// 	{
// 		AsyncGetPlayerData(m);
// 		ReadJsonArray;
// 		int pilotID	= js_msg[0u].asInt(); // 武将ID
// 		int times = js_msg[1u].asInt();
// 		int option = js_msg[2u].asInt();
// 
// 		if(!(times == 1 || times == 10))Return(r, -1);
// 		const playerPilot& cPilot = d->Pilots.getPlayerPilotExtert(pilotID);
// 		if(cPilot == playerPilots::NullPilotExtert)Return(r, -1);
// 		int cost = 0;
// 		int res = 0;
// 		if(option == 0){
// 			cost  = 20;
// 			if(d->Base.getAllGold() < cost)Return(r, 4);
// 			res = d->Pilots.coreFast(pilotID, times, true);
// 			if(res ==  0)d->Base.alterBothGold(-cost);
// 		}
// 		else{
// 			cost = getCoreFastCost(d->Base.getLevel());
// 			if(d->Base.getKeJi() < cost)Return(r, 5);
// 			res = d->Pilots.coreFast(pilotID, times, false);
// 			if(res ==  0)d->Base.alterKeJi(-cost);
// 		}
// 		Return(r, res);
// 	}

	void pilotManager::revertAttribute(msg_json& m, Json::Value& r){//还原属性
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int pilotID = js_msg[0u].asInt();
		int cost_gold = 20;
		if (d->Base.getAllGold() < cost_gold) Return(r, 1);//不够金币了。。。
		if (0 >= pilotID) Return(r, -1);

		
		int Ret = d->Pilots.revertAttribute(pilotID);

		if (0 == Ret){
			int gold = d->Base.getGold();
			int goldTicket =  d->Base.getGoldTicket();
			int proValue = d->Base.getGoldTicket();
			
			d->Base.alterBothGold(-cost_gold);
			//d->Base.alterBothGold(-cost_gold);
			helper_mgr.insertSaveAndUpdate(&d->Pilots);
			bool recordSecflas = false;
			int costAf  = 0;
			if(cost_gold<=goldTicket){
				proValue = goldTicket;
				costAf = proValue-cost_gold;
			}else{
				proValue = goldTicket;
				costAf = 0;
				cost_gold = cost_gold - proValue;
				recordSecflas = true;
			}
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string ,int>(action_add_gold_ticket));
			fields.push_back(boost::lexical_cast<string ,int>(pilotID));
			fields.push_back(boost::lexical_cast<string ,int>(0));
			fields.push_back(boost::lexical_cast<string ,int>(-1));
			fields.push_back(boost::lexical_cast<string ,int>(-1));
			fields.push_back(boost::lexical_cast<string ,int>(-1));
			StreamLog::Log(lg_player_pilot, d, boost::lexical_cast<string, int>(proValue), boost::lexical_cast<string, int>(costAf),fields, 6);
			if(recordSecflas){
				int proTemp = d->Base.getGold();
				StreamLog::Log(lg_player_pilot, d, boost::lexical_cast<string, int>(proTemp), boost::lexical_cast<string, int>(proTemp-cost_gold),fields, action_add_gold);
			}
			Return(r, 0);
		}else if (1 == Ret){
			Return(r, 0);
		}else{
			Return(r, -1);
		}
	}

	void pilotManager::gmAddPilotExpReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int pilotID = js_msg[0].asInt();
		int pilotExp = js_msg[1].asInt();

		if (d->Pilots.addExp(pilotID, pilotExp) == -1)
		{
			Return(r, 1);
		}

		Return(r, 0);
	}

	void pilotManager::gmSetPilotExtraAttributeReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int pilotID = js_msg[0].asInt();
		Json::Value attr = js_msg[1];

		if (!attr.isArray() || attr.size() != 3)
		{
			Return(r, -1);
		}

		if (!d->Pilots.setAttribute(pilotID, attr))
		{
			Return(r, 1);
		}

		Return(r, 0);
	}

	void pilotManager::gmPilotsListReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		
		r[msgStr][0u] = 0;
		d->Pilots.gmTotalSimplePack(r[msgStr][1u]);
	}

	void pilotManager::gmSinglePilotInfoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int pilotID = js_msg[0u].asInt();

		const playerPilot& battlePilot = d->Pilots.getPlayerPilotExtert(pilotID);
		if (battlePilot == playerPilots::NullPilotExtert)return;
		pilotConfig pilotConfig = pilot_sys.getPilotConfig(battlePilot.pilotID);
		if (!pilotConfig.isVaild())return;

		playerScienceBuffer science_buffer = d->Science.getBuffer();

		double initialCharacter[characterNum];
		double addCharacter[characterNum];
		double totalCharacter[characterNum];
		int initialAttribute[attributeNum];//基础 统勇智
		int addAttribute[attributeNum];//额外 统勇智
		memset(addCharacter, 0x0, sizeof(addCharacter));
		memcpy(initialAttribute, pilotConfig.initialAttribute, sizeof(initialAttribute));//武将基础统勇智
		memcpy(addAttribute, battlePilot.addAttribute, sizeof(addAttribute));//武将额外统勇智
		memcpy(initialCharacter, pilotConfig.initialCharacter, sizeof(initialCharacter));//武将基础属性
		initialCharacter[idx_hp] = pilotConfig.initialHP +
			pilotConfig.growUp * battlePilot.pilotLevel;// + science_buffer.hp;//最大HP
		for (unsigned i = 0; i < characterNum; ++i){
			addCharacter[i] += science_buffer.initialCharacter[i];
		}
//		addCharacter[idx_hp] += science_buffer.hp;
		//武将额外属性

		//装备
		double equipAdd[characterNum];
		battlePilot.getEquipAdded(equipAdd);
		for (unsigned i = 0; i < characterNum; ++i)
		{
			//if(i == idx_hp)maxHp += (int)equipAdd[i];
			addCharacter[i] += equipAdd[i];
		}
		//成长属性
		double growAdd[characterNum];
		battlePilot.getGrowAdded(growAdd);
		for (unsigned i = 0; i < characterNum; ++i)
		{
			//if(i == idx_hp)maxHp += (int)growAdd[i];
			addCharacter[i] += growAdd[i];
		}
		int currentHp = int(initialCharacter[idx_hp] + addCharacter[idx_hp]);

		for (unsigned i = 0; i < characterNum; ++i)
		{
			totalCharacter[i] = initialCharacter[i] + addCharacter[i];
		}

		if (pilotConfig.armsType == 0)
		{
			totalCharacter[0] = -1;
			totalCharacter[4] = -1;
		}
		else if (pilotConfig.armsType == 1)
		{
			totalCharacter[2] = -1;
			totalCharacter[4] = -1;
		}
		else
		{
			totalCharacter[0] = -1;
			totalCharacter[2] = -1;
		}

		Json::Value singlePack = battlePilot.package();
		singlePack["hp"] = currentHp;
		string strAtkDefData = "ad";
		for (unsigned i = 0; i < 10; i++)
			singlePack[strAtkDefData].append(int(totalCharacter[i]));

		itemVector vec = d->Item.getAllItem(position_equip_space);
		singlePack[playerPilotEquipListStr] = Json::arrayValue;
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			equipExPtr equipPtr = vec[i]->getEquipEx();
			if (equipPtr == equipExPtr())
				continue;
			if (equipPtr->getBelong() != pilotID)
				continue;
			singlePack[playerPilotEquipListStr].append(vec[i]->toJson());
		}

		r[msgStr][1] = singlePack;
		Return(r, 0);
	}

	pilotManager* const pilotManager::pilotMgr = new pilotManager();

#define checkAndContinue(json)\
	if(json.isNull())\
	continue;
#define checkNotNullFomatInt(val ,json)\
	if(! json.isNull()){\
		val = json.asInt();\
	}

#define checkNotNullFomatDouble(val ,json)\
	if(! json.isNull()){\
		val = json.asDouble();\
	}

	const static string pilotRawIDStr = "rawID";
	const static string pilotGrowUpStr = "grow";
	const static string pilotFeeStr = "enlistFee";
	const static string pilotInitialHPStr = "initialHP";
	const static string pilotMaxLevelStr = "maxLevel";
	const static string pilotSkill1Str = "skill_1";
	const static string pilotSkill2Str = "skill_2";
	const static string armsTypeStr = "armsType";
	const static string pilotPuGongStr = "PuGong";
	const static string pilotPuFangStr = "PuFang";
	const static string pilotZhanGongStr = "ZhanGong";
	const static string pilotZhanFangStr = "ZhanFang";
	const static string pilotMoGongStr = "MoGong";
	const static string pilotMoFangStr = "MoFang";
	const static string pilotShanBiStr = "ShanBi";
	const static string pilotGeDangStr = "GeDang";
	const static string pilotBaoJiStr = "BaoJi";
	const static string pilotFanJiStr = "FanJi";
	const static string pilotWuLiZengShangStr = "WuLiZengShang";
	const static string pilotWuLiJianShangStr = "WuLiJianShang";
	const static string pilotZhanFaZengShangStr = "ZhanFaZengShang";
	const static string pilotZhanFaJianShangStr = "ZhanFaJianShang";
	const static string pilotMoFaZengShangStr = "MoFaZengShang";
	const static string pilotMoFaJianShangStr = "MoFaJianShang";
	const static string pilotZhiLiaoXiShuStr = "ZhiLiaoXiShu";
	const static string pilotBeiZhiLiaoXiShuStr = "BeiZhiLiaoXiShu";
	const static string pilotJiChuTongStr = "JiChuTong";
	const static string pilotJiChuYongStr = "JiChuYong";
	const static string pilotJiChuZhiStr = "JiChuZhi";
	const static string pilotWuliGongJiXiShuStr = "WuliGongJiXiShu";
	const static string pilotWuliFangYuXiShuStr = "WuliFangYuXiShu";
	const static string pilotZhanFaGongJiXiShuStr = "ZhanFaGongJiXiShu";
	const static string pilotZhanFaFangYuXiShuStr = "ZhanFaFangYuXiShu";
	const static string pilotMoFaGongJiXiShuStr = "MoFaGongJiXiShu";
	const static string pilotMoFaFangYuXiShuStr = "MoFaFangYuXiShu";
	const static string armsRestraintStr = "armsRestraint";
	const static string pilotTypeStr = "pilotType";
	const static string pilotInitialLevelStr = "initiaLV";
	const static string pilotResistStr = "resist";

	const static string soldierIDStr = "armsId";

	const static string pilotEquipLimitStr = "./instance/pilotEquip/config.json";
	const static string pilotDataDirStr = "./instance/pilots/";
	const static string soildierDataDirStr = "./instance/soldier/";

	const static string peiyanCostKejiConfigPath = "./instance/pilots_extend/peiyan_cost_keji.json";
	const static string coreFastCostConfigPath = "./instance/pilots_extend/tufei_cost_keji.json";
	const static string coreFastExpConfigPath = "./instance/pilots_extend/tufei_exp.json";
	const static string pilotUpgradeItemConfigPath = "./instance/pilots_extend/upgrade_item.json";
	const static string pilotShowConfigPath = "./instance/soldiershow/soldiershow.json";
	void pilotManager::initPilot()
	{
		db_mgr.ensure_index(playerPilotsDBStr, BSON(playerIDStr << 1));
		db_mgr.ensure_index(playerPilotsListDBStr, BSON(playerIDStr << 1 << playerPilotRawIDStr << 1));

		Json::Value uItemJson = na::file_system::load_jsonfile_val(pilotUpgradeItemConfigPath);
		for (unsigned i = 0; i < uItemJson.size(); ++i)
		{
			int uItemID = uItemJson[i].asInt();
			uItemMap[uItemID] = uItemID;
		}

		na::file_system::json_value_vec jv0;
		na::file_system::load_jsonfiles_from_dir(soildierDataDirStr, jv0);
		pilotsMap tempConfig;
		tempConfig.clear();
		for (unsigned i = 0; i < jv0.size(); ++i)
		{
			Json::Value& pilot = jv0[i];

			pilotConfig pilotG;
			checkNotNullFomatInt(pilotG.rawID, pilot[soldierIDStr]);
			checkNotNullFomatInt(pilotG.armsType, pilot[armsTypeStr]);

			checkNotNullFomatDouble(pilotG.initialCharacter[6], pilot[pilotShanBiStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[7], pilot[pilotGeDangStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[8], pilot[pilotBaoJiStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[9], pilot[pilotFanJiStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[10], pilot[pilotWuLiZengShangStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[11], pilot[pilotWuLiJianShangStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[12], pilot[pilotZhanFaZengShangStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[13], pilot[pilotZhanFaJianShangStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[14], pilot[pilotMoFaZengShangStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[15], pilot[pilotMoFaJianShangStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[16], pilot[pilotZhiLiaoXiShuStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[17], pilot[pilotBeiZhiLiaoXiShuStr]);

			checkNotNullFomatDouble(pilotG.armsModules[0], pilot[pilotWuliGongJiXiShuStr]);
			checkNotNullFomatDouble(pilotG.armsModules[1], pilot[pilotWuliFangYuXiShuStr]);
			checkNotNullFomatDouble(pilotG.armsModules[2], pilot[pilotZhanFaGongJiXiShuStr]);
			checkNotNullFomatDouble(pilotG.armsModules[3], pilot[pilotZhanFaFangYuXiShuStr]);
			checkNotNullFomatDouble(pilotG.armsModules[4], pilot[pilotMoFaGongJiXiShuStr]);
			checkNotNullFomatDouble(pilotG.armsModules[5], pilot[pilotMoFaFangYuXiShuStr]);

			if(! pilot[armsRestraintStr].isNull()){
				for (unsigned i = 0; i < pilot[armsRestraintStr].size(); ++i)
				{
					armsRestraint m;
					m.atkModule = pilot[armsRestraintStr][i][0u].asDouble();
					m.defModule = pilot[armsRestraintStr][i][1u].asDouble();
					pilotG.armsR.push_back(m);
				}
			}
			tempConfig[pilotG.rawID] = pilotG;
		}

		
		Json::Value sConfig = na::file_system::load_jsonfile_val(pilotShowConfigPath);
		std::map<int, int> sVMap;
		for (unsigned i = 0; i < sConfig.size(); ++i)
		{
			sVMap[sConfig[i][soldierIDStr].asInt()] = sConfig[i]["GongJi"].asInt() +
				sConfig[i]["WuFang"].asInt() + sConfig[i]["ZhanFang"].asInt()+ 
				sConfig[i]["MoFang"].asInt();
		}


		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(pilotDataDirStr, jv);

		maxPilotId_ = -1;
		for (unsigned i = 0; i < jv.size(); ++i)
		{
			Json::Value& pilot = jv[i];
			
			pilotConfig pilotG;
			int armsID = pilot[soldierIDStr].asInt();
			pilotsMap::iterator it = tempConfig.find(armsID);
			if(it != tempConfig.end()){
				pilotConfig& config = it->second;
				memcpy(pilotG.initialCharacter, config.initialCharacter, sizeof(pilotG.initialCharacter));
				memcpy(pilotG.armsModules, config.armsModules, sizeof(pilotG.armsModules));
				pilotG.armsType = config.armsType;
				pilotG.armsR = config.armsR;
			}

			checkNotNullFomatInt(pilotG.rawID, pilot[pilotRawIDStr]);
			checkNotNullFomatInt(pilotG.growUp, pilot[pilotGrowUpStr]);
			checkNotNullFomatInt(pilotG.enlistFee, pilot[pilotFeeStr]);
			checkNotNullFomatInt(pilotG.maxLevel, pilot[pilotMaxLevelStr]);	
			checkNotNullFomatInt(pilotG.initialHP, pilot[pilotInitialHPStr]);	
			checkNotNullFomatInt(pilotG.pilotType, pilot[pilotTypeStr]);	
			checkNotNullFomatInt(pilotG.initialLevel, pilot[pilotInitialLevelStr]);
			if(pilotG.initialLevel < 1)pilotG.initialLevel = 1;

			checkNotNullFomatInt(pilotG.initialAttribute[0], pilot[pilotJiChuTongStr]);
			checkNotNullFomatInt(pilotG.initialAttribute[1], pilot[pilotJiChuYongStr]);
			checkNotNullFomatInt(pilotG.initialAttribute[2], pilot[pilotJiChuZhiStr]);

			checkNotNullFomatDouble(pilotG.initialCharacter[0], pilot[pilotPuGongStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[1], pilot[pilotPuFangStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[2], pilot[pilotZhanGongStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[3], pilot[pilotZhanFangStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[4], pilot[pilotMoGongStr]);
			checkNotNullFomatDouble(pilotG.initialCharacter[5], pilot[pilotMoFangStr]);
			pilotG.showValue = 0;
			if (sVMap.find(armsID) != sVMap.end())
			{
				pilotG.showValue = sVMap[armsID];
			}
			pilotG.isSup = pilot["fuzhu"].asBool();

			//技能
			int skillID = pilot[pilotSkill1Str].asInt();
			pilotG.skill_1 = skill_sys.getSkill(skillID);

			skillID = pilot[pilotSkill2Str].asInt();
			pilotG.skill_2  = skill_sys.getSkill(skillID);

			//免疫
			if (!pilot[pilotResistStr].isNull())
			{
				for (unsigned i = 0; i < pilot[pilotResistStr].size(); ++i)
				{
					pilotG.resistList.insert(pilot[pilotResistStr][i].asInt());
				}
			}

			map_[pilotG.rawID] = pilotG;

			if (pilotG.rawID > maxPilotId_)
			{
				maxPilotId_ = pilotG.rawID;
			}
		}

// 		Json::Value equipJson = na::file_system::load_jsonfile_val(pilotEquipLimitStr);
// 		if(! equipJson.isNull()){
// 			for (unsigned i = 0; i < equipJson.size(); ++i)
// 			{
// 				Json::Value& equs = equipJson[i];
// 				pilotEquipConfigs pEC;
// 				pEC.resize(PilotEquipMentPosition);
// 				for (unsigned n = 0; n < equs.size() && n < (unsigned)PilotEquipMentPosition; ++n){
// 					Json::Value cEqu = equs[n];
// 					for(unsigned k = 0; k < cEqu.size() ; ++k)
// 						pEC[n].push_back(cEqu[k].asInt());
// 				}
// 				pEMap_[i] = pEC;
// 			}
//		}

		//培养所需科技点配置 跟玩家等级有关
		Json::Value peiyanCostKejiJson = na::file_system::load_jsonfile_val(peiyanCostKejiConfigPath);
		if (!peiyanCostKejiJson.isNull()){
			peiyanCostKejiConfigVect.clear();
			for(unsigned i = 0; i < peiyanCostKejiJson.size(); ++i){
				Json::Value& item = peiyanCostKejiJson[i];
				peiyanCostKejiConfigVect.push_back(item.asInt());
			}
		}

		
		Json::Value coreFastCostJson = na::file_system::load_jsonfile_val(coreFastCostConfigPath);
		if (!coreFastCostJson.isNull()){
			coreFastCostVec.clear();
			for(unsigned i = 0; i < coreFastCostJson.size(); ++i){
				Json::Value& item = coreFastCostJson[i];
				coreFastCostVec.push_back(item.asInt());
			}
		}

		Json::Value coreFastExpJson = na::file_system::load_jsonfile_val(coreFastExpConfigPath);
		if (!coreFastExpJson.isNull()){
			for(unsigned i = 0; i < coreFastExpJson.size(); ++i){
				Json::Value& item = coreFastExpJson[i];
				coreFastExp.push_back(item.asInt());
			}
		}
	}

	int pilotManager::getCoreFastCost(int playerLv)
	{
		if(playerLv < 1)return *coreFastCostVec.begin();
		if(playerLv >= (int)coreFastCostVec.size())return coreFastCostVec.back();
		return coreFastCostVec[playerLv];
	}

	bool pilotManager::configUpgradeItem(const int itemID)
	{
		return uItemMap.find(itemID) != uItemMap.end();
	}

	int pilotManager::getCoreFastExp(int playerLv)
	{
		if(playerLv < 1)return *coreFastExp.begin();
		if(playerLv >= (int)coreFastExp.size())return coreFastExp.back();
		return coreFastExp[playerLv];
	}

	const pilotConfig& pilotManager::getPilotConfig(const int pilotID)
	{
		const static pilotConfig NullValue;
		pilotsMap::iterator it = map_.find(pilotID);
		if(it != map_.end())return it->second;
		return NullValue;
	}

	int pilotManager::getMaxPilotId()
	{
		return maxPilotId_;
	}
}
