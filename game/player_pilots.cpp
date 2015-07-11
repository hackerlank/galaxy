#include "player_pilots.h"
#include "playerManager.h"
#include "mongoDB.h"
#include "pilotManager.h"
#include "item_system.h"
#include "helper.h"
#include "file_system.h"
#include "science_system.h"
#include "record_system.h"
#include "task_system.h"
#include "guild_system.h"

using namespace mongo;

namespace gg
{
	//const static int PilotEquipMentPosition = 6;
	const static int playerEquipPosDivide = 6;
	const static int PilotEquipMentPosition = 6; // 12->6 2015-5-20

	const static string logPilot = "log_pilot";
	const static string logPilotEquip = "log_pilot_equip";
	enum{
		log_add_exp,
		log_now_exp,
		log_prew_lv,
		log_now_lv,
		log_active_pilot,
		log_enlist_pilot,
		log_fire_pilot,
		log_change_mode,
	};

	playerPilot playerPilots::NullPlayerPilot;
	const playerPilot playerPilots::NullPilotExtert;

	playerPilot::playerPilot()
	{
		memset(this, 0x0, sizeof(playerPilot) - sizeof(vector<playerItemPtr>));
		pilotID = -1;
		askForHelp = false;
		dirty = false;
		pilotEquipment.resize(PilotEquipMentPosition);
	}

	playerPilot::playerPilot(const int pID, const int cLV /* = 1 */)
	{
		memset(this, 0x0, sizeof(playerPilot) - sizeof(vector<playerItemPtr>));
		pilotID = pID;
		pilotLevel = cLV;
		pilotType = pilot_un_enlist;
		coreMode = -1;
		askForHelp = false;
		dirty = false;
		pilotEquipment.resize(PilotEquipMentPosition);
	}

	Json::Value playerPilot::package(const bool FullInfo /* = false */)const
	{
		Json::Value data;
		data[playerPilotRawIDStr] = pilotID;
		data[playerPilotLevelStr] = pilotLevel;
		data[playerPilotTypeStr] = pilotType;
		data[playerPilotExpStr] = pilotExp;
		data[pilotCoreBeginStr] = coreBegin;
		data[pilotCoreEndStr] = coreEnd;
		data[pilotModeStr] = coreMode;
		data[pilotAskHelpStr] = (askForHelp || coreMode < 0) ? false : true;
//		data[playerPilotCurrentHPStr] = currentHP;
		data[playerPilotCultureStr][0u] = addAttribute[0];
		data[playerPilotCultureStr][1u] = addAttribute[1];
		data[playerPilotCultureStr][2u] = addAttribute[2];
		data[playerPilotNewCultureStr][0u] = newAttribute[0];
		data[playerPilotNewCultureStr][1u] = newAttribute[1];
		data[playerPilotNewCultureStr][2u] = newAttribute[2];
		data[playerPilotHistoryCultureStr][0u] = historyMaxAttribute[0];
		data[playerPilotHistoryCultureStr][1u] = historyMaxAttribute[1];
		data[playerPilotHistoryCultureStr][2u] = historyMaxAttribute[2];
		data[playerPilotIsToInherit] = isToInherit;
		data[playerPilotIsByInherit] = isByInherit;
		data[playerPilotBattleValueStr] = battleValue;
		data[playerPilotEquipListStr] = Json::arrayValue;
		for (unsigned  i =0; i < pilotEquipment.size(); ++i)
		{
			playerItemPtr ptr = pilotEquipment[i];
			if (FullInfo)
			{
				if (ptr == playerItemPtr()) data[playerPilotEquipListStr].append(Json::objectValue);
				else data[playerPilotEquipListStr].append(ptr->toJson());
			}
			else
			{
				if (ptr == playerItemPtr()) data[playerPilotEquipListStr].append(-1);
				else data[playerPilotEquipListStr].append(ptr->getLocalID());
			}
		}
		return data;
	}

	void playerPilot::getGrowAdded(void* point) const
	{
		double added[characterNum];
		memset(added, 0x0, sizeof(added));
		unsigned len = playerPilots::GrowVec.getSize();
		for (unsigned i = 0; i < len; ++i)
		{
			added[i] = playerPilots::GrowVec[i][pilotLevel].asDouble();
		}
		added[idx_hp] += pilot_sys.getPilotConfig(pilotID).growUp * pilotLevel;
		memcpy(point, added, sizeof(added));
	}

	void playerPilot::calBattleValue(playerDataPtr player /* = playerDataPtr() */, const bool on_update /* = true */)
	{
		battleValue = 0;
		pilotConfig config = pilot_sys.getPilotConfig(pilotID);
		if (!config.isVaild())return;

		int initialAttribute[attributeNum];//基础 统勇智
		int extraAttribute[attributeNum];//额外 统勇智
		double initialCharacter[characterNum];//基础属性
		memcpy(initialAttribute, config.initialAttribute, sizeof(initialAttribute));//武将基础统勇智
		memcpy(extraAttribute, addAttribute, sizeof(addAttribute));//武将额外统勇智
		memcpy(initialCharacter, config.initialCharacter, sizeof(initialCharacter));//武将基础属性
		initialCharacter[idx_hp] += config.initialHP;

		//附加属性
		double grow[characterNum];
		double equip[characterNum];
		double secretaryAdded[characterNum];
		getGrowAdded(grow);
		getEquipAdded(equip);
		memset(secretaryAdded, 0x0, sizeof(secretaryAdded));
		if (player != NULL) player->PlayerSecretaries.getAttrs(secretaryAdded);
		playerScienceBuffer science_buffer;
		if(player != NULL) science_buffer = player->Science.getBufferNoFormat();
		double GuildSkillAdd[characterNum];
		memset(GuildSkillAdd, 0x0, sizeof(GuildSkillAdd));
		if (player != NULL)player->TradeSkill.battleAdd(GuildSkillAdd);

		int totalA[attributeNum];
		for (unsigned i = 0; i < attributeNum; ++i)
		{
			totalA[i] = initialAttribute[i] + extraAttribute[i];
		}

		double totalC[characterNum];
		for (unsigned i = 0; i < characterNum; ++i)
		{
			totalC[i] = initialCharacter[i] + grow[i] + equip[i] + science_buffer.initialCharacter[i] + GuildSkillAdd[i] + secretaryAdded[i];
		}

		int bval = 0;
		if (!config.isSup)
		{
			if (config.armsType == 0)
			{
				bval = int(totalC[idx_war] * 1);
			}
			else if (config.armsType == 1)
			{
				bval = int(totalC[idx_atk] * 2);
			}
			else
			{
				bval = int(totalC[idx_magic] * 4);
			}
		}

		battleValue = int (bval + totalC[idx_war_def] * 1 + totalC[idx_def] * 2 + totalC[idx_magic_def] * 4
			+ totalC[idx_hp] * 0.5 + (totalC[idx_dodge] + totalC[idx_block] + totalC[idx_crit] + totalC[idx_counter]) / 2
			+ (totalC[idx_phyAddHurt] + totalC[idx_phyCutHurt] + totalC[idx_warAddHurt]
			+ totalC[idx_warCutHurt] + totalC[idx_magicAddHurt] + totalC[idx_magicCutHurt]) * 6666
			+ (totalA[idx_dominance] + totalA[idx_power] + totalA[idx_intelligence]) * 20 + config.showValue);

		if (config.isSup)
		{
			battleValue += (41 * pilotLevel + 750);
		}

		battleValue = battleValue < 0 ? 0 : battleValue;
		battleValue = battleValue > 2000000000 ? 2000000000 : battleValue;//int 取值范围

		if (on_update)onBattleValueUpdate(player);
	}

	void playerPilot::onBattleValueUpdate(playerDataPtr player)
	{
		if (player == NULL)return;
		player->Embattle.updateBattleValue();
	}

	//各属性所对应的ID
	//0物理攻击（普通攻击）
	//1物理防御（普通防御）
	//2充能攻击（战法攻击）
	//3能量护甲（战法防御）
	//4原力攻击（策略攻击）
	//5原力屏障（策略防御）
	//6机动（闪避率）
	//7力场（抵挡率）
	//8聚能（暴击率）
	//9预警（反击率）
	//18耐久度（带兵数）
	//----11111111----攻击计算方式
	void playerPilot::getEquipAdded(void* point) const
	{
		double added[characterNum];
		memset(added, 0x0, sizeof(added));
		for (unsigned i = 0; i < playerEquipPosDivide && i < pilotEquipment.size(); ++i)
		{
			playerItemPtr cItem = pilotEquipment[i];
			if (cItem == playerItemPtr())continue;
			int itemID = cItem->getItemID();
			equipExPtr eqEx = cItem->getEquipEx();
			if (eqEx == NULL)continue;
			int equipLv = eqEx->getLv();
			if (equipLv < 1)continue;
			Params::ObjectValue& obj = item_sys.getConfig(itemID);
			if (obj.isEmpty() || !obj.isMember(ItemDefine::equipment))continue;
			EqProValue proValue[EqPVSize];
			eqEx->getCulSubPro(proValue, true);
			for (int n = 0; n < EqPVSize; ++n)
			{
				if (!proValue[n].isVaild())continue;
				int idx = proValue[n].typePro;
				if (idx < 0 || idx >= characterNum)continue;
				added[idx] += proValue[n].valuePro;
			}
			Params::Var& equipVar = obj[ItemDefine::equipment];
			for (unsigned n = 0; n < characterNum; ++n){
				added[n] += (equipVar[ItemDefine::equipValueStr][n].asDouble() +
					equipVar[ItemDefine::equipGrowStr][n].asDouble() * (equipLv - 1));
			}
		}
		memcpy(point, added, sizeof(added));
	}

	bool playerPilots::checkPilotEnlist(const int pilotID){
		playerPilotsMap::iterator it = pilots.find(pilotID);
		return !(it == pilots.end() || it->second.pilotType != pilot_enlist);
	}

	bool playerPilots::checkPilotActive(const int pilotID){
		playerPilotsMap::iterator it = pilots.find(pilotID);
		return it != pilots.end();
	}

	void playerPilots::updateExp(playerPilot& cPilot)
	{
		int playerLv = own.Base.getLevel();
		if(playerLv <  40)return;
		if(cPilot.pilotType != pilot_enlist)return;
		unsigned now = na::time_helper::get_current_time();
		if (playerLv >= (int)modeBase.size() || cPilot.coreBegin == 0)
		{
			cPilot.coreBegin = now;
			cPilot.dirty = true;
			helper_mgr.insertSaveAndUpdate(this);
			return;
		}
		if(cPilot.coreBegin >= now)return;
		int oldExp = cPilot.pilotExp;
		unsigned stepSecond = now - cPilot.coreBegin;//1分钟结算
		unsigned step = stepSecond / 60;
		unsigned leave = stepSecond % 60;
		if(cPilot.coreEnd > cPilot.coreBegin && cPilot.coreMode >= 0 && 
			cPilot.coreMode < (int)modeVec.size()){
				unsigned leaveStep = (cPilot.coreEnd - cPilot.coreBegin) / 60;
				unsigned minAdd = min(step, leaveStep);
				cPilot.pilotExp += (int)(modeBase[playerLv] * minAdd * modeVec[cPilot.coreMode].base);
				step = step - minAdd < 0 ? 0 : step - minAdd;
		}
		cPilot.pilotExp += (int)(modeBase[playerLv]/* * 0.1*/ * step);
		cPilot.coreBegin += (stepSecond - leave);
		if (cPilot.coreBegin >= cPilot.coreEnd)
		{
			cPilot.coreMode = -1;
			guild_sys.removeHelp(own.getOwnDataPtr(), Guild::HelpData::TYPECAL(Guild::GuildHelp::pilot_core, cPilot.pilotID));
		}
		if (oldExp != cPilot.pilotExp)
		{
			StreamLog::Log(logPilot, own, cPilot.pilotID, cPilot.pilotExp - oldExp, log_add_exp);
			StreamLog::Log(logPilot, own, cPilot.pilotID, cPilot.pilotLevel, log_prew_lv);
			upgrade(cPilot);
			StreamLog::Log(logPilot, own, cPilot.pilotID, cPilot.pilotExp, log_now_exp);
			StreamLog::Log(logPilot, own, cPilot.pilotID, cPilot.pilotLevel, log_now_lv);
			cPilot.dirty = true;
			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	void playerPilots::activeEnlistMode()
	{
		for (playerPilotsMap::iterator it = pilots.begin(); it != pilots.end(); ++it)
		{
			playerPilot& cPilot = it->second;
			if(cPilot.pilotType != pilot_enlist)continue;
			//if(cPilot.coreBegin != 0)continue;
			cPilot.coreBegin = na::time_helper::get_current_time();
			cPilot.coreMode = -1;
		}
	}
	
	bool playerPilots::addDelayUpdate(playerPilot& pP)
	{
		if (pP == NullPilotExtert)return false;
		pilotSet.insert(pP.pilotID);
		return true;
	}

	bool playerPilots::addDelayUpdate(const int pilotID)
	{
		playerPilot& cPilot = getPlayerPilot(pilotID);
		return addDelayUpdate(cPilot);
	}

	/*
	培养，  洗属性
	*/
	int playerPilots::upgradeAttribute(const int pilotID, const int upgradeType){

		playerPilotsMap::iterator it = pilots.find(pilotID);
		playerPilot& pilot = it->second;

		if (NullPlayerPilot == pilot) return 1;
		int new_attribute_total = 0;
		int (&newAttribute)[3] = pilot.newAttribute; 
		int (&historyAttribute)[3] = pilot.historyMaxAttribute;
		int condition2 = pilot.pilotLevel + 20;	    //条件2     武将等级加20
		for (unsigned i = 0; i < 3; ++i){
			int condition1 = pilot.addAttribute[i] + 5; //条件1   当前属性值+5 / 基础属性后面增加的值

			switch(upgradeType){
			case 1:
				
			case 2://加强洗练
				newAttribute[i] = commom_sys.randomBetween(1, pilot.addAttribute[i] + 4);//在（1,已加属性+4）随机
				break;
			case 3://白金洗练
				// 				如果 条件1 少于 条件2 *0.5		
				// 					则在
				// 					当前属性值基础上随机增加（1,4）
				// 					否则在
				// 					当前属性值 基础上随机增加（-8,4）
				if (condition1 < condition2 * 0.5){
					newAttribute[i] = pilot.addAttribute[i] + commom_sys.randomBetween(1, 4);
				}else{
					newAttribute[i] = pilot.addAttribute[i] + commom_sys.randomBetween(-8, 4);
				}

				break;
			case 4:

				// 				如果 条件1 少于 条件2 *0.8
				// 					则在
				// 					当前属性值 基础上随机增加 （3,5）
				// 					否则在
				// 					当前属性值基础上随机增加（-5,5）
			 
				if (condition1 < condition2 * 0.8){
					int value = commom_sys.randomBetween(3, 5);
					newAttribute[i] = pilot.addAttribute[i] +  value;
				}else{
					newAttribute[i] = pilot.addAttribute[i] + commom_sys.randomBetween(-5, 5);
				}
				break;
			}
			//当洗属性的时候遇到“属性值”大于等于“武将等级加20”则把洗出来的结果抹平为“武将等级+20”并提示“满”
			if (newAttribute[i] >= condition2){
				newAttribute[i] = condition2;
			}else if(0 > newAttribute[i]){//下限不出现负数，出现负数改为0
				newAttribute[i] = 0;
			}
			new_attribute_total += newAttribute[i];
		}
		//  更新洗练出来的最高历史记录值
// 			例如：A舰长——	统：50+0	勇：50+0	智：50+0，
		//	洗属性后替换	统：50+10	勇：50+20	智：50+3。
		//  再洗时替换		统：50+18   勇：50+12   智：50+8.
//          恢复属性（错误）统：50+18   勇：50+20   智：50+8。
// 			调整方案：
		//玩家第一次替换属性时，将三项属性之和与原属性的三项之和进行比较，取其大者定为最好属性，以后每次替换新属性时，都将新属性三项之和与最好属性三项之和进行比较，新属性和≥最好属性和时，将新属性替换为最好属性 
		int history_total = 0;
		for (unsigned i = 0; i < 3; ++i){
			history_total += historyAttribute[i]; 
		}
		if (new_attribute_total >= history_total){
			for (unsigned i = 0; i < 3; ++i){
				historyAttribute[i] = newAttribute[i];
			}
		}


		//addDelayUpdate(pilot);
		pilot.dirty = true;
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_polit_train_times);
		
		return 0;
	}

	//复原历史最高的洗练属性
	int playerPilots::revertAttribute(const int pilotID){
		playerPilotsMap::iterator it = pilots.find(pilotID);
		if (it == pilots.end()) return -1;
		playerPilot& pilot = it->second;
		 
		int attribute_total = 0;
		int history_attribute_total = 0;
		for (unsigned i = 0 ; i < 3; ++i){
			attribute_total += pilot.addAttribute[i];
			history_attribute_total += pilot.historyMaxAttribute[i];
		}

		if(history_attribute_total >  attribute_total){
			memcpy(pilot.addAttribute, pilot.historyMaxAttribute, sizeof(pilot.historyMaxAttribute));
			setMaxTotalAttribute(history_attribute_total);
			pilot.calBattleValue(own.getOwnDataPtr());
			pilot.dirty = true;
			//addDelayUpdate(pilot);
			return 0;
		}
		return 1;
	}

	/*
	切换属性（替换当前使用的属性   ）
	*/
//	int playerPilots::replaceAttribute(const int pilotID,bool isNotice){
	int playerPilots::replaceAttribute(const int pilotID)
	{
		playerPilotsMap::iterator it = pilots.find(pilotID);
		if(it == pilots.end()) return -1;
		playerPilot& pilot = it->second;
		
		int tmp_counter = 0;
		for (unsigned i = 0; i < 3; ++i){
			tmp_counter += pilot.newAttribute[i];
		}
		if( 0 >= tmp_counter)
			return 0;
		memcpy(pilot.addAttribute, pilot.newAttribute, sizeof(pilot.newAttribute));
		memset(pilot.newAttribute, 0, sizeof(pilot.newAttribute));
		setMaxTotalAttribute(tmp_counter);
		pilot.calBattleValue(own.getOwnDataPtr());
		pilot.dirty = true;
		//if(isNotice)addDelayUpdate(pilot);
		return 0;
	}

	int* playerPilots::getPilotHistoryMaxAttribute(const int pilotID){
		playerPilotsMap::iterator it = pilots.find(pilotID);
		if (it == pilots.end()) return NULL;
		return it->second.historyMaxAttribute;
	}

	int playerPilots::clearAttribute(const int pilotID){
		playerPilotsMap::iterator it = pilots.find(pilotID);
		if(it == pilots.end()) return -1;
		playerPilot& pilot = it->second;
		memset(pilot.newAttribute, 0, sizeof(pilot.newAttribute));
		pilot.dirty = true;
		//addDelayUpdate(pilot);
		return 0;
	}

	int playerPilots::enlist(const int pilotID)
	{
		playerPilots::playerPilotsMap::iterator it = pilots.find(pilotID);
		if(it == pilots.end())return -1;
		if(currentEnlistPilot >= getMaxEnlistPilot()) return 1;
		if (it->second.pilotType != pilot_un_enlist)return -1;		
		it->second.pilotType = pilot_enlist;
		++currentEnlistPilot;
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_polit_lv);
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_polit_num);

		it->second.coreBegin = na::time_helper::get_current_time();
		it->second.coreEnd = 0;
		//addDelayUpdate(it->second);
		it->second.dirty = true;
		helper_mgr.insertSaveAndUpdate(this);
		StreamLog::Log(logPilot, own, "enlist", pilotID, log_enlist_pilot);
		return 0;
	}

	int playerPilots::useItemCDClear(const bool costGold /* = true */)
	{
		if(costGold){
			unsigned cCD = useItemCD.getCD().CD;
			int costGlod = GGCOMMON::costCDClear(cCD);
			if(own.Base.getAllGold() < costGlod)return 1;
			own.Base.alterBothGold(-costGlod);
		}
		useItemCD.clearCD();		
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

#define ExChange(_1_ID, _1_Eq, _1_EqL, _2_ID, _2_Eq, _2_EqL, Pos) \
	_2_EqL[Pos] = _1_Eq; \
	if (_1_Eq != NULL)\
 	{\
 	_1_Eq->getEquipEx()->setBelong(_2_ID); \
 	}
	
// 	\
//  	_2_EqL[Pos + playerEquipPosDivide] = _1_Crystal; \
// 	if (_1_Crystal != NULL)\
// 	{\
//  	_1_Crystal->getCrystalEx()->belongPilot = _2_ID; \
//  	}

	int playerPilots::EqiuipChange(const int pilotID_1, const int pilotID_2)
	{
		const pilotConfig& pC_1 = pilot_sys.getPilotConfig(pilotID_1);
		const pilotConfig& pC_2 = pilot_sys.getPilotConfig(pilotID_2);
		if (!pC_1.isVaild() || !pC_2.isVaild())return -1;
		unsigned index = 0;
//		if (pC_1.armsType != pC_2.armsType)index = 1;//不同种族
		playerPilot& plt_1 = getPlayerPilot(pilotID_1);
		playerPilot& plt_2 = getPlayerPilot(pilotID_2);
		if (plt_1 == NullPilotExtert || plt_2 == NullPilotExtert)return -1;
		vector<playerItemPtr>& eq_1 = plt_1.pilotEquipment;
		vector<playerItemPtr>& eq_2 = plt_2.pilotEquipment;
		int num = 0;
		for (unsigned i = 0; i < playerEquipPosDivide; ++i)
		{
			playerItemPtr ceq_1 = eq_1[i];
			//playerItemPtr crystal_1 = eq_1[i + playerEquipPosDivide];
			playerItemPtr ceq_2 = eq_2[i];
			//playerItemPtr crystal_2 = eq_2[i + playerEquipPosDivide];
			if (ceq_1 != NULL)
			{
				Params::ObjectValue& itemC = item_sys.getConfig(ceq_1->getItemID());
				if (itemC.isEmpty() || !itemC.isMember(ItemDefine::equipment))return -1;//不是装备
				Params::Var& wItemVar = itemC[ItemDefine::equipment];
				if (!equipPosPass(pC_2.armsType, pC_2.isSup, plt_2.pilotLevel, wItemVar[ItemDefine::equipTypeStr].asInt(), i))continue;//
			}
			if (ceq_2 != NULL)
			{
				Params::ObjectValue& itemC = item_sys.getConfig(ceq_2->getItemID());
				if (itemC.isEmpty() || !itemC.isMember(ItemDefine::equipment))return -1;//不是装备
				Params::Var& wItemVar = itemC[ItemDefine::equipment];
				if (!equipPosPass(pC_1.armsType, pC_1.isSup, plt_1.pilotLevel, wItemVar[ItemDefine::equipTypeStr].asInt(), i))continue;//不能完全互换装备
			}
			if (ceq_1 == NULL && ceq_2 == NULL)continue;
			ExChange(pilotID_1, ceq_1, eq_1, pilotID_2, ceq_2, eq_2, i);
			ExChange(pilotID_2, ceq_2, eq_2, pilotID_1, ceq_1, eq_1, i);
			++num;
		}
		if (num == 0)return 1;//完全不能互换
// 		for (unsigned i = index; i < playerEquipPosDivide; ++i)
// 		{
// 			playerItemPtr ceq_1 = eq_1[i];
// 			playerItemPtr crystal_1 = eq_1[i + playerEquipPosDivide];
// 			playerItemPtr ceq_2 = eq_2[i];
// 			playerItemPtr crystal_2 = eq_2[i + playerEquipPosDivide];
//  			ExChange(pilotID_1, ceq_1, crystal_1, eq_1, pilotID_2, ceq_2, crystal_2, eq_2, i);
//  			ExChange(pilotID_2, ceq_2, crystal_2, eq_2, pilotID_1, ceq_1, crystal_1, eq_1, i);
// 		}
		plt_1.calBattleValue(own.getOwnDataPtr());
		plt_2.calBattleValue(own.getOwnDataPtr());
		//addDelayUpdate(plt_1);
		//addDelayUpdate(plt_2);
		plt_1.dirty = true;
		plt_2.dirty = true;
		helper_mgr.insertSaveAndUpdate(this);
		string str_1, str_2;
		for (int i = 0; i < PilotEquipMentPosition; ++i)
		{
			playerItemPtr item_1 = eq_1[i];
			if (item_1 == NULL)str_1 += "-1,";
			else str_1 += (boost::lexical_cast<string, int>(item_1->getLocalID()) + ",");
			playerItemPtr item_2 = eq_2[i];
			if (item_2 == NULL)str_2 += "-1,";
			else str_2 += (boost::lexical_cast<string, int>(item_2->getLocalID()) + ",");
		}
		StreamLog::LogV(logPilotEquip, own.getOwnDataPtr(), pilotID_1, pilotID_2, -1,
			str_1, str_2);
		return 0;
	}

	int playerPilots::equip(const int pilotID, const int equipID, const int positionID)
	{
		//找出对应的武将
		if(positionID < 0 || positionID >= PilotEquipMentPosition)return -1;
		const pilotConfig& pilotCig = pilot_sys.getPilotConfig(pilotID);
		if(!pilotCig.isVaild())return -1;
		playerPilot& cPilot = getPlayerPilot(pilotID);
		if(cPilot == NullPlayerPilot)return -1;
		//找出准备装备的装备物品
		playerItemPtr willEquip = own.Item.getItem(equipID);
		//原先装备在该装备位置上的装备物品
		playerItemPtr beenEquip = cPilot.pilotEquipment[positionID];
		if (willEquip == beenEquip)
		{
			if (willEquip == NULL)
				return -1;
			return 0;//同一个道具...
		}
		if(willEquip == playerItemPtr())return -1;
		if(willEquip->getPosition() != position_ware)return -1;//已经被别人装备了
		int beenEqID = -1;
		if (beenEquip != NULL)beenEqID = beenEquip->getLocalID();
// 		if (positionID >= playerEquipPosDivide)
// 		{
// 			if (beenEquip != NULL)return -1;
// 			//在对应位置上是否有装备
// 			playerItemPtr matchEquip = cPilot.pilotEquipment[positionID - playerEquipPosDivide];
// 			if (matchEquip == NULL)return -1;	//对应位置上面没有装备
// 			if (own.Base.getAllGold() < 5)return 3;//金币不足
// 			playerItemPtr strippedEq = own.Item.StrippedEquipModel(willEquip);
// 			int res = -1;
// 			do 
// 			{
// 				if (strippedEq == NULL)break;
// 				cryExPtr wEqPtr = strippedEq->getCrystalEx();
// 				if (wEqPtr == NULL)break;//不是水晶
// 				if (wEqPtr->belongPilot != -1)break;//已经被别人装备了
// 				Params::ObjectValue& wItemConfig = item_sys.getConfig(strippedEq->getItemID());
// 				if (wItemConfig.isEmpty() || !wItemConfig.isMember(ItemDefine::crystal))break;//不是水晶
// 				Params::Var& wItemVar = wItemConfig[ItemDefine::crystal];
//  				if (wItemVar[ItemDefine::crystalEquipLimitStr].asInt() > own.Base.getLevel())
//  				{
//  					res = 2;
//  					break;
//  				}
// 				res = 0;
// 			} while (false);
// 			if (res != 0)
// 			{
// 				own.Item.sortF(strippedEq->getItemID());
// 				return res;
// 			}
// 
// 			cryExPtr wEqPtr = strippedEq->getCrystalEx();
// 			wEqPtr->belongPilot = cPilot.pilotID;
// 			strippedEq->setPosition(position_equip_space);
// 			cPilot.pilotEquipment[positionID] = strippedEq;
// 
// 			own.Base.alterBothGold(-5);
// 		}
// 		else
// 		{
			equipExPtr wEqPtr = willEquip->getEquipEx();
			if (wEqPtr == NULL)return -1;//不是装备
			if (wEqPtr->getBelong() != -1)return -1;//已经被别人装备了
			Params::ObjectValue& wItemConfig = item_sys.getConfig(willEquip->getItemID());
			if (wItemConfig.isEmpty() || !wItemConfig.isMember(ItemDefine::equipment))return -1;//不是装备
			Params::Var& wItemVar = wItemConfig[ItemDefine::equipment];

			//武将等级是否满足装备使用等级限制
			//		if(wItemVar[ItemDefine::equipLimitStr].asInt() > cPilot.pilotLevel) return 2;
			if (wItemVar[ItemDefine::equipLimitStr].asInt() > own.Base.getLevel()) return 2;
			//对应位置上能否装上该装备
			if (!equipPosPass(pilotCig.armsType, pilotCig.isSup, cPilot.pilotLevel, wItemVar[ItemDefine::equipTypeStr].asInt(), positionID))return 1;

			if (beenEquip != playerItemPtr()){
				beenEquip->setPosition(position_ware);
				cPilot.pilotEquipment[positionID] = playerItemPtr();
				equipExPtr bEqPtr = beenEquip->getEquipEx();
				if (bEqPtr != NULL)bEqPtr->setBelong(-1);
			}
			wEqPtr->setBelong(cPilot.pilotID);
			willEquip->setPosition(position_equip_space);
			cPilot.pilotEquipment[positionID] = willEquip;
//		}
		cPilot.calBattleValue(own.getOwnDataPtr());
		//addDelayUpdate(cPilot);
		cPilot.dirty = true;
		helper_mgr.insertSaveAndUpdate(this);
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_equipment_num);
		string str;
		for (int i = 0; i < PilotEquipMentPosition; ++i)
		{
			playerItemPtr item = cPilot.pilotEquipment[i];
			if (item == NULL)str += "-1,";
			else str += (boost::lexical_cast<string, int>(item->getLocalID()) + ",");
		}
		StreamLog::LogV(logPilotEquip, own.getOwnDataPtr(), cPilot.pilotID, str, positionID, 
			boost::lexical_cast<string, int>(equipID), boost::lexical_cast<string, int>(beenEqID));
		return 0;
	}

	int playerPilots::convertMode(const int pilotID, const int mode, const bool costResources /* = false */)
	{
		if(own.Base.getLevel() < 40)return 2;//主角等级不足
		if(mode < 0 || mode >= (int)modeVec.size())return -1;
		int VIPLV = own.Vip.getVipLevel();
		if(VIPLV < modeVec[mode].vipLimit)return 3;//vip等级不足
		if(costResources && own.Base.getAllGold() < modeVec[mode].cost)return 1;//钱不够

		playerPilot& cPilot = getPlayerPilot(pilotID);
		if(cPilot == NullPilotExtert)return -1;
		updateExp(cPilot);
		cPilot.coreBegin = na::time_helper::get_current_time();
		cPilot.coreEnd = cPilot.coreBegin + modeVec[mode].lastTime * 60;
		cPilot.coreMode = mode;
		cPilot.askForHelp = false;
		if(costResources)own.Base.alterBothGold(-modeVec[mode].cost);
		//addDelayUpdate(cPilot);
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_auto_upgrade_mode, mode);
		cPilot.dirty = true;
		helper_mgr.insertSaveAndUpdate(this);
		StreamLog::Log(logPilot, own, pilotID, mode, log_change_mode);
		guild_sys.removeHelp(own.getOwnDataPtr(), Guild::HelpData::TYPECAL(Guild::GuildHelp::pilot_core, pilotID));
		return 0;
	}

// 	int playerPilots::coreFast(const int pilotID, const int times, const bool jumpCD /* = false */)
// 	{
// 		if(times < 0)return -1;
// 		if(! jumpCD && surmountCD.getCD().Lock)return 3;
// 		playerPilot& cPilot = getPlayerPilot(pilotID);
// 		if(cPilot == NullPilotExtert)return -1;
// 		if(cPilot.pilotLevel < 51)return 1;
// 		if((unsigned)cPilot.pilotCoreLv >= CoreExpVec.size())return 2;
// 		cPilot.pilotCoreExp += pilot_sys.getCoreFastExp(own.Base.getLevel());
// 		upgradeCore(cPilot);
// 		if(! jumpCD)surmountCD.addCD(600);
// 		addSinglePilotPackage(cPilot);
// 		helper_mgr.insertSaveSet(this);
// 		return 0;
// 	}

	int  playerPilots::offEquip(const int pilotID, const int positionID)
	{
		//先找到指定的武将
		if(positionID < 0 || positionID >= PilotEquipMentPosition)return -1;
		const pilotConfig& pilotCig = pilot_sys.getPilotConfig(pilotID);
		if(!pilotCig.isVaild())return -1;
		playerPilot& cPilot =getPlayerPilot(pilotID);
		if(cPilot == NullPlayerPilot)return -1;
		//看下这个位置有没有装备
		playerItemPtr beenEquip = cPilot.pilotEquipment[positionID];
		if(beenEquip == playerItemPtr())return -1;
		//if(own.Item.getLeavePlace() < 1)return 1;//背包满了,不可以卸下装备
		if (!own.Item.canAddItem(beenEquip->getItemID(), 1))return 1;
		if (beenEquip->getEquipEx() != NULL)
		{
			beenEquip->getEquipEx()->setBelong(-1);
		}
// 		else if (beenEquip->getCrystalEx() != NULL)
// 		{
// 			beenEquip->getCrystalEx()->belongEquip = -1;
// 		}
		else
		{
			return -1;
		}
		beenEquip->setPosition(position_ware);
		cPilot.pilotEquipment[positionID] = playerItemPtr();
		cPilot.calBattleValue(own.getOwnDataPtr());
		//addDelayUpdate(cPilot);
		cPilot.dirty = true;
		helper_mgr.insertSaveAndUpdate(this);
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_equipment_num);
		string str;
		for (int i = 0; i < PilotEquipMentPosition; ++i)
		{
			playerItemPtr item = cPilot.pilotEquipment[i];
			if (item == NULL)str += "-1,";
			else str += (boost::lexical_cast<string, int>(item->getLocalID()) + ",");
		}
		StreamLog::LogV(logPilotEquip, own.getOwnDataPtr(), cPilot.pilotID, str, positionID, boost::lexical_cast<string, int>(beenEquip->getLocalID()));
		return 0;
	}

	int playerPilots::fire(const int pilotID)
	{
		string str;
		playerPilot& cPilot = getPlayerPilot(pilotID);
		if(cPilot == NullPlayerPilot)return -1;
		if(currentEnlistPilot <= 1) return -1;
		if(cPilot.pilotType == pilot_un_enlist)return -1;
		int num = 0;
		for(unsigned i = 0; i < cPilot.pilotEquipment.size(); ++i)
		{
			playerItemPtr equip = cPilot.pilotEquipment[i];
			if(equip == playerItemPtr())continue;
			++num;
		}
		//查看背包是否还有空间
		if(num > 0 && num > own.Item.getLeavePlace())return 1;
		for(unsigned i = 0; i < cPilot.pilotEquipment.size(); ++i)
		{
			playerItemPtr cEquip = cPilot.pilotEquipment[i];
			if(cEquip == playerItemPtr())continue;
			cEquip->setPosition(position_ware);
			if(cEquip->getEquipEx() != NULL)
			{
				cEquip->getEquipEx()->setBelong(-1);
			}
// 			if (cEquip->getCrystalEx() != NULL)
// 			{
// 				cEquip->getCrystalEx()->belongEquip = -1;
// 			}
			str += (boost::lexical_cast<string, int>(cEquip->getLocalID()) + ",");
			cPilot.pilotEquipment[i] = playerItemPtr();
		}
		updateExp(cPilot);
		cPilot.pilotType = pilot_un_enlist;
		cPilot.coreMode = -1;
		cPilot.askForHelp = false;
		--currentEnlistPilot;
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_fire_polit);
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_polit_num);
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_polit_lv);
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_equipment_num);
		cPilot.calBattleValue(own.getOwnDataPtr());
		//addDelayUpdate(cPilot);
		cPilot.dirty = true;
		helper_mgr.insertSaveAndUpdate(this);
		StreamLog::Log(logPilot, own, "fire", pilotID, log_fire_pilot);
		own.Embattle.removeFirePilot(pilotID);
		guild_sys.removeHelp(own.getOwnDataPtr(), Guild::HelpData::TYPECAL(Guild::GuildHelp::pilot_core, pilotID));
		StreamLog::Log(logPilotEquip, own, cPilot.pilotID, str);
		return 0;
	}

	void playerPilots::recalBattleValue(const bool update /* = true */)
	{
		for (playerPilotsMap::iterator it = pilots.begin(); it != pilots.end(); ++it)
		{
			playerPilot& cPilot = it->second;
			cPilot.calBattleValue(own.getOwnDataPtr(), false);
		}
		if (update)
		{
			sendAllPilotsInfo();
			own.Embattle.updateBattleValue(update);
		}
	}

	void playerPilots::calBattleValue(const int pilotID, const bool update /* = true */)
	{
		playerPilot& cPilot = getPlayerPilot(pilotID);
		if (cPilot == NullPlayerPilot)return;
		cPilot.calBattleValue(own.getOwnDataPtr());
		if (update)
		{
			if (cPilot.dirty)
			{
				helper_mgr.insertSaveAndUpdate(this);
			}
			else
			{
				addDelayUpdate(cPilot);
				helper_mgr.insertUpdate(this);
			}
		}
	}

	bool playerPilots::canBattle(const int pilotID)
	{
		if(pilotID < 0)return false;
		const playerPilot& sPilot = getPlayerPilot(pilotID);
		if(sPilot == NullPlayerPilot || sPilot.pilotType != pilot_enlist)return false;
		return true;
	}

	int playerPilots::updatePlayerPilot(playerPilot& cPilot)
	{
		playerPilotsMap::iterator it = pilots.find(cPilot.pilotID);
		if(it == pilots.end())return -1;
		cPilot.dirty = true;
		pilots[cPilot.pilotID] = cPilot;
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	int playerPilots::addExp(const int pilotID, const int exp)
	{
		//LogW<<"test exp::"<<exp<<LogEnd;
		if(exp < 1)return -1;
		playerPilot&  cPilot = getPlayerPilot(pilotID);
		if(cPilot == NullPlayerPilot)return -1;
		cPilot.pilotExp += exp;
		int level = cPilot.pilotLevel;
		upgrade(cPilot);
		cPilot.dirty = true;
		//addDelayUpdate(cPilot);
		helper_mgr.insertSaveAndUpdate(this);
		StreamLog::Log(logPilot, own, cPilot.pilotID, exp, log_add_exp);
		StreamLog::Log(logPilot, own, cPilot.pilotID, cPilot.pilotExp, log_now_exp);
		StreamLog::Log(logPilot, own, cPilot.pilotID, level, log_prew_lv);
		StreamLog::Log(logPilot, own, cPilot.pilotID, cPilot.pilotLevel, log_now_lv);
		return cPilot.pilotLevel - level;
	}

	int playerPilots::getTotalExp(const int pilotID)
	{
		playerPilot&  cPilot = getPlayerPilot(pilotID);
		if(cPilot == NullPlayerPilot)return -1;
		int exp = 0;
		for (int i = 0; i < cPilot.pilotLevel; ++i)
		{
			exp+=pilotExpVec[i];
		}
		return exp+cPilot.pilotExp;
	}

	int playerPilots::addExpPreview(const int pilotID, const int exp)
	{
		if(exp < 1)return -1;
		playerPilot  cPilot = getPlayerPilot(pilotID);
		if(cPilot == NullPlayerPilot)return -1;
		cPilot.pilotLevel = 1;
		cPilot.pilotExp = exp;
		upgrade(cPilot);
		return cPilot.pilotLevel;
	}
	void playerPilots::upgrade(playerPilot& cPilot)
	{
		if(cPilot.pilotLevel < 1)return;
		int old_lv = cPilot.pilotLevel;
		for (unsigned i = (unsigned)cPilot.pilotLevel; i < pilotExpVec.size(); ++i)
		{
			int tmp = cPilot.pilotExp - pilotExpVec[i];
			if(tmp < 0)break;
			++cPilot.pilotLevel;
			cPilot.pilotExp = tmp;
		}
		if((unsigned)cPilot.pilotLevel >= pilotExpVec.size() 
			|| cPilot.pilotLevel >= own.Base.getLevel())cPilot.pilotExp = 0;
		if(cPilot.pilotLevel > own.Base.getLevel())cPilot.pilotLevel = own.Base.getLevel();
		if (cPilot.pilotLevel > old_lv)
		{
			cPilot.calBattleValue(own.getOwnDataPtr());
			cPilot.dirty = true;
			//addDelayUpdate(cPilot);
		}
		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_polit_lv);
	}

	const static unsigned useItemLockTime = 28800;
	playerPilots::playerPilots(playerData& own) : Block(own), useItemCD(-1, useItemLockTime)
	{
		//updatePilotJson = Json::nullValue;
		maxTotalAttribute = 0;
		inheritTimes = 0;
		dataVersion = 1;
		pilotConfig pG = pilot_sys.getPilotConfig(startPilotID);
		if(pG.isVaild()){
			playerPilot p = playerPilot(pG.rawID, pG.initialLevel);
			p.pilotType = pilot_enlist;
			p.calBattleValue();
			pilots[p.pilotID] = p;
			currentEnlistPilot = 1;
		}
	}

	Params::ArrayValue playerPilots::GrowVec;
	vector<int> playerPilots::pilotExpVec;
	const static string pilotExpConfigPath = "./instance/pilots_extend/pilot_exp.json";
	const static string pilotGrowConfigPath = "./instance/pilots_extend/pilot_grow.json";
	vector<playerPilots::upgradeMode> playerPilots::modeVec;
	vector<int> playerPilots::modeBase;
	const static string pilotCoreModeConfigPath = "./instance/pilots_extend/core_mode.json";
	const static string pilotCoreBaseConfigPath = "./instance/pilots_extend/core_base.json";

	int playerPilots::getMaxLv()
	{
		return (int)pilotExpVec.size();
	}

	void playerPilots::initData()
	{
		LogS << "start initial pilot config ..." << LogEnd;

		Json::Value pilotGrowJson = na::file_system::load_jsonfile_val(pilotGrowConfigPath);
		Params::ArrayValue arr(pilotGrowJson);
		GrowVec = arr;

		pilotExpVec.clear();
		Json::Value pilotExpJson = na::file_system::load_jsonfile_val(pilotExpConfigPath);
		for (unsigned i = 0; i < pilotExpJson.size(); ++i)
			pilotExpVec.push_back(pilotExpJson[i].asInt());

		modeVec.clear();
		Json::Value pilotCoreModeJson = na::file_system::load_jsonfile_val(pilotCoreModeConfigPath);
		unsigned size = pilotCoreModeJson["base"].size();
		for (unsigned i = 0; i < size; ++i){
			upgradeMode core;
			core.base = pilotCoreModeJson["base"][i].asDouble();
			core.cost = pilotCoreModeJson["cost"][i].asInt();
			core.lastTime = pilotCoreModeJson["lastTime"][i].asUInt();
			core.vipLimit = pilotCoreModeJson["vipLimit"][i].asInt();
			modeVec.push_back(core);
		}

		modeBase.clear();
		Json::Value pilotCoreBaseJson = na::file_system::load_jsonfile_val(pilotCoreBaseConfigPath);
		for (unsigned i = 0; i < pilotCoreBaseJson.size(); ++i){
			modeBase.push_back(pilotCoreBaseJson[i].asInt());
		}

	}
	 
	const playerPilot& playerPilots::getPlayerPilotExtert(const int pilotID)
	{
		playerPilotsMap::iterator it = pilots.find(pilotID);
		if(it == pilots.end())return NullPilotExtert;
		return it->second;
	}

	CDData playerPilots::getCD()
	{
		return useItemCD.getCD();
	}

	bool playerPilots::IsLockUseItemCD()
	{
		return useItemCD.getCD().Lock;
	}

	void playerPilots::addUseItemCD(unsigned s)
	{
		useItemCD.addCD(s);
		helper_mgr.insertSaveAndUpdate(this);
	}

	bool playerPilots::canUpgrade(const int pilotID)
	{
		playerPilot cPilot = getPlayerPilot(pilotID);
		if(cPilot == NullPilotExtert)return false;
		updateExp(cPilot);
		if(cPilot.pilotLevel >= own.Base.getLevel())return false;
		if(cPilot.pilotLevel >= (int)pilotExpVec.size())return false;
		return true;
	}

	playerPilot& playerPilots::getPlayerPilot(const int pilotID)
	{
		playerPilotsMap::iterator it = pilots.find(pilotID);
		if(it == pilots.end())return (NullPlayerPilot = playerPilot());
		return it->second;
	}

	int playerPilots::getMaxEnlistPilot()
	{
		return own.Office.getPilotCanEnlistNum();
	}

	//0锐抓 1装甲 2护盾 3晶核 4涂层 5船饰
	bool playerPilots::equipPosPass(const int pilotType, const bool fuzhu, const int pilotLV, const int equipType, const int position)
	{
		if(position < 0 || position >= PilotEquipMentPosition)return false;
		if ((pilotType == 0 && position == 0 && equipType == 2) ||
			(pilotType == 1 && position == 0 && equipType == 0) ||
			(pilotType == 2 && position == 0 && equipType == 4))
		{
			if (fuzhu && pilotLV < 40)return false;
			return true;
		}
		if(own.Base.getLevel() > 21 && position == 1 && equipType == 1)return true;
		if(position == 2 && equipType == 3)return true;
		if(own.Base.getLevel() > 32 && position == 3 && equipType == 5)return true;
		if(own.Base.getLevel() > 44 && position == 4 && equipType == 6)return true;
		if(own.Base.getLevel() > 54 && position == 5 && equipType == 7)return true;
		return false;
	}

	int playerPilots::activePilot(const int pilotID)
	{
		if(checkPilotActive(pilotID))return 1;//已经有武将
		pilotConfig pg = pilot_sys.getPilotConfig(pilotID);
		if(!pg.isVaild())return -1;
		playerPilot acPilot = playerPilot(pilotID, pg.initialLevel);
		acPilot.calBattleValue(own.getOwnDataPtr());
		acPilot.dirty = true;
		pilots[pilotID] = acPilot;
		//addDelayUpdate(pilots[pilotID]);
		StreamLog::Log(logPilot, own, getPilotType(pilotID), pilotID, log_active_pilot);
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

// 	void playerPilots::reBindEquipment(playerItemPtr oldEquip, playerItemPtr newEquip)
// 	{
// 		playerPilot& cPilot = getPlayerPilot(oldEquip->getEquipBelong());
// 		if(cPilot == NullPlayerPilot)return;
// 		oldEquip->setEquipBelong(-1);
// 		newEquip->setEquipBelong(cPilot.pilotID);
// 		for (unsigned  i =0; i < cPilot.pilotEquipment.size(); ++i){
// 			if(cPilot.pilotEquipment[i] == oldEquip){
// 				cPilot.pilotEquipment[i] = newEquip;
// 				break;
// 			}
// 		}
// 		addDelayUpdate(cPilot);
// 		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_equipment_num);
// 		helper_mgr.insertSaveSet(this);
// 	}

	void playerPilots::autoUpdate()
	{
		for (playerPilotsMap::iterator it = pilots.begin(); it != pilots.end(); ++it)
		{
			const playerPilot& cPilot = it->second;
			if (!cPilot.dirty)continue;
			pilotSet.insert(cPilot.pilotID);
		}//添加新选项
		if (!pilotSet.empty())
		{
			Json::Value updateJson;
			updateJson[msgStr][0u] = 1;
			updateJson[msgStr][1u][updateFromStr] = State::getState();
			Json::Value& pVal = updateJson[msgStr][1u][playerPilotDatasStr];
			int num = 0;
			for (updateSet::iterator it = pilotSet.begin(); it != pilotSet.end(); ++it)
			{
				playerPilot& cPilot = getPlayerPilot(*it);
				if (cPilot == NullPilotExtert)continue;
				updateExp(cPilot);
				pVal.append(cPilot.package());
				++num;
				if (num > 20)
				{
					own.sendToClient(gate_client::pilot_update_resp, updateJson);
					num = 0;
					pVal = Json::arrayValue;
				}
			}
			if (num > 0)own.sendToClient(gate_client::pilot_update_resp, updateJson);
		}
		sendPilotBaseInfo();
	}

	void playerPilots::update(const bool updatePilot /* = false */)
	{
		if(updatePilot)sendAllPilotsInfo();
		sendPilotBaseInfo();
	}

	bool playerPilots::singleUpdate(const int pilotID)
	{
		bool ret = addDelayUpdate(pilotID);
		helper_mgr.insertUpdate(this);
		return ret;
	}

	void playerPilots::sendAllPilotsInfo()
	{
		Json::Value part_json;
		part_json[msgStr][0u] = 1;//添加
		part_json[msgStr][1u][updateFromStr] = State::getState();
		Json::Value& pilots_json = part_json[msgStr][1u][playerPilotDatasStr];
		pilots_json = Json::arrayValue;
		unsigned num = 0;
		for (playerPilotsMap::iterator it = pilots.begin(); it != pilots.end(); )
		{
			updateExp(it->second);
			pilots_json.append(it->second.package());
			++num;
			++it;
			if(num > 20 || it == pilots.end()){
				player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::pilot_update_resp, part_json);
				pilots_json.clear();
				num = 0;
			}
		}
	}

	void playerPilots::setPilotHelp(const int pilotID)
	{
		playerPilot& cPilot = getPlayerPilot(pilotID);
		if (cPilot == NullPlayerPilot)return;
		cPilot.askForHelp = true;
		cPilot.dirty = true;
		//addDelayUpdate(cPilot);
		helper_mgr.insertSaveAndUpdate(this);
	}

	bool playerPilots::addCoreTime(const int pilotID, const unsigned time)
	{
		playerPilot& cPilot = getPlayerPilot(pilotID);
		updateExp(cPilot);
		if (cPilot == NullPlayerPilot || cPilot.coreMode < 0)
		{
			//addDelayUpdate(cPilot);
			cPilot.dirty = true;
			helper_mgr.insertSaveAndUpdate(this);
			return false;
		}
		cPilot.coreEnd += time;
		//addDelayUpdate(cPilot);
		cPilot.dirty = true;
		helper_mgr.insertSaveAndUpdate(this);
		return true;
	}

	void playerPilots::gmTotalSimplePack(Json::Value &pack)
	{
		pack = Json::arrayValue;
		for (playerPilotsMap::iterator it = pilots.begin(); it != pilots.end(); it++)
		{
			Json::Value singlePack;
			singlePack[playerPilotRawIDStr] = it->second.pilotID;
			singlePack[playerPilotTypeStr] = it->second.pilotType;
			pack.append(singlePack);
		}
	}

	void playerPilots::sendPilotBaseInfo()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		CDData uCD = useItemCD.getCD();
		msg[msgStr][1u][playerPilotUseItemCDstr] = uCD.CD;
		msg[msgStr][1u][playerPilotUseItemLockStr] = uCD.Lock;
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::pilot_update_resp, msg);
	}

	bool playerPilots::get()
	{		
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(playerPilotsDBStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[playerPilotUseItemCDstr]){useItemCD.getCD().CD = (unsigned)obj[playerPilotUseItemCDstr].Int();}
			checkNotEoo(obj[playerPilotUseItemLockStr]){useItemCD.getCD().Lock = obj[playerPilotUseItemLockStr].Bool();}
			checkNotEoo(obj[playerPilotInheritTimesStr]){inheritTimes = obj[playerPilotInheritTimesStr].Int();}
			checkNotEoo(obj[playerPilotMaxAttributeStr]){maxTotalAttribute = obj[playerPilotMaxAttributeStr].Int();}
			checkNotEoo(obj[playerPilotVersionStr]){ dataVersion = obj[playerPilotVersionStr].Int(); }
			pilots.clear();
			bool dirty = false;
			objCollection pCollect;
			if (dataVersion == 0)
			{
				dirty = true;
				dataVersion = 1;
				checkNotEoo(obj[playerPilotDatasStr]){ 
					vector<BSONElement> bel = obj[playerPilotDatasStr].Array();
					for (unsigned i = 0; i < bel.size(); ++i)
					{
						pCollect.push_back(bel[i].Obj());
					}
				}
			}
			else
			{
				pCollect = db_mgr.Query(playerPilotsListDBStr, key);
			}

			for (unsigned i = 0; i < pCollect.size(); ++i)
			{
				playerPilot p;
				p.dirty = dirty;
				mongo::BSONObj cObj = pCollect[i];
				p.pilotID = cObj[playerPilotRawIDStr].Int();
				p.pilotLevel = cObj[playerPilotLevelStr].Int();
				p.pilotType = cObj[playerPilotTypeStr].Int();
				p.pilotExp = cObj[playerPilotExpStr].Int();
				p.coreBegin = (unsigned)cObj[pilotCoreBeginStr].Int();
				p.coreEnd = (unsigned)cObj[pilotCoreEndStr].Int();
				p.coreMode = cObj[pilotModeStr].Int();
				checkNotEoo(cObj[pilotAskHelpStr])p.askForHelp = cObj[pilotAskHelpStr].Bool();
				checkNotEoo(cObj[playerPilotIsToInherit])
					p.isToInherit = cObj[playerPilotIsToInherit].Int();
				checkNotEoo(cObj[playerPilotIsByInherit])
					p.isByInherit = cObj[playerPilotIsByInherit].Int();
				checkNotEoo(cObj[playerPilotCultureStr]){
					vector<BSONElement> attris = cObj[playerPilotCultureStr].Array();//当前所使用的属性
					for (unsigned n = 0; n < attris.size() && n < 3; ++n)
						p.addAttribute[n] = attris[n].Int();
				}

				checkNotEoo(cObj[playerPilotNewCultureStr]){
					vector<BSONElement> new_attributes = cObj[playerPilotNewCultureStr].Array();//洗出来的属性但没有切换当前属性
					for (unsigned n = 0; n < new_attributes.size() && n < 3; ++n)
						p.newAttribute[n] = new_attributes[n].Int();
				}
				checkNotEoo(cObj[playerPilotHistoryCultureStr]){
					vector<BSONElement> history_attributes = cObj[playerPilotHistoryCultureStr].Array();//历史陪练出来的最高属性值
					for (unsigned n = 0; n < history_attributes.size() && n < 3; ++n){
						p.historyMaxAttribute[n] = history_attributes[n].Int();
					}
				}
				playerWare& Bag = own.Item;
				checkNotEoo(cObj[playerPilotEquipListStr])
				{
					vector<BSONElement> equ = cObj[playerPilotEquipListStr].Array();
					for (unsigned n = 0; n < equ.size(); ++n)
					{
						int idx = equ[n][playerPilotEquipIndexStr].Int();
						if (idx < 0 || idx >= PilotEquipMentPosition) continue;
						int localID = equ[n][playerPilotEquipMentIDStr].Int();
						playerItemPtr cItem = Bag.getItem(localID);
						if (cItem == playerItemPtr())continue;
						if (cItem->getPosition() != position_equip_space)continue;
						equipExPtr ptr = cItem->getEquipEx();
						if (ptr == NULL || ptr->getBelong() != -1)continue;
						ptr->setBelong(p.pilotID, true);
						p.pilotEquipment[idx] = cItem;
					}
				}
				p.calBattleValue(own.getOwnDataPtr(), false);
				pilots[p.pilotID] = p;
			}
			
			currentEnlistPilot = 0;
			for (playerPilotsMap::iterator it = pilots.begin(); it != pilots.end(); ++it){
				if(it->second.pilotType == pilot_enlist)
					++currentEnlistPilot;
			}
			return true;
		}
		return false;
	}

	void playerPilots::doEnd()
	{
		for (playerPilotsMap::iterator it = pilots.begin(); it != pilots.end(); ++it)
		{
			playerPilot& cPilot = it->second;
			cPilot.dirty = false;
		}
	}

	bool playerPilots::save()
	{
		for (playerPilotsMap::iterator it = pilots.begin(); it != pilots.end(); ++it)
		{
			const playerPilot& cPilot = it->second;
			if (!cPilot.dirty)continue;
			mongo::BSONObj key = BSON(playerIDStr << own.playerID 
				<< playerPilotRawIDStr << cPilot.pilotID);
			mongo::BSONArrayBuilder arr;
			arr << cPilot.addAttribute[0] << cPilot.addAttribute[1] << cPilot.addAttribute[2];

			mongo::BSONArrayBuilder arr2; 
			arr2 << cPilot.newAttribute[0] << cPilot.newAttribute[1] << cPilot.newAttribute[2];

			mongo::BSONArrayBuilder arr3;
			arr3 << cPilot.historyMaxAttribute[0] << cPilot.historyMaxAttribute[1] << cPilot.historyMaxAttribute[2];

			mongo::BSONArrayBuilder equipments;
			for(int  i = 0; i < PilotEquipMentPosition; ++i)
			{
				mongo::BSONObjBuilder equip;
				playerItemPtr cItem = cPilot.pilotEquipment[i];
				if(cItem != playerItemPtr())
				{
					equip << playerPilotEquipIndexStr << i << playerPilotEquipMentIDStr << cItem->getLocalID();
					equipments << equip.obj();
				}
			}
 
			mongo::BSONObjBuilder b;
			b << playerPilotRawIDStr <<  cPilot.pilotID << playerPilotLevelStr << cPilot.pilotLevel <<
				playerPilotTypeStr << cPilot.pilotType << playerPilotExpStr << cPilot.pilotExp <<
				pilotCoreBeginStr << cPilot.coreBegin << pilotCoreEndStr << cPilot.coreEnd <<
				pilotModeStr << cPilot.coreMode  <<	playerPilotCultureStr << arr.arr() <<
				playerPilotEquipListStr << equipments.arr() << playerPilotNewCultureStr <<
				arr2.arr() << playerPilotHistoryCultureStr << arr3.arr()<<playerPilotIsToInherit<<
				cPilot.isToInherit<<playerPilotIsByInherit<<cPilot.isByInherit << 
				pilotAskHelpStr << cPilot.askForHelp << playerIDStr << own.playerID;
			db_mgr.save_mongo(playerPilotsListDBStr, key, b.obj());
		}

		//base infomation
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = BSON(
			playerIDStr << own.playerID << playerPilotUseItemCDstr << useItemCD.getCD().CD
			<< playerPilotUseItemLockStr << useItemCD.getCD().Lock /*<< playerPilotDatasStr << array_pilots.arr()*/
			<< playerPilotInheritTimesStr << inheritTimes << playerPilotMaxAttributeStr << maxTotalAttribute <<
			playerPilotVersionStr << dataVersion
			);
		return db_mgr.save_mongo(playerPilotsDBStr, key, obj);
	}

	int playerPilots::inheritHero(playerPilot& cPilot,bool isSupremacy)
	{
		upgrade(cPilot);
		cPilot.dirty = true;
		pilots[cPilot.pilotID] = cPilot;
		addInheritTimes();
		helper_mgr.insertSaveAndUpdate(this);
		return cPilot.pilotLevel;
	}

	int playerPilots::getMaxPolitLv()
	{
		int max_lv = 0;
		for(playerPilotsMap::iterator iter = pilots.begin(); iter != pilots.end(); ++iter)
		{
			if(iter->second.pilotLevel > max_lv)
				max_lv = iter->second.pilotLevel;
		}
		return max_lv;
	}

	void playerPilots::addInheritTimes()
	{
		++inheritTimes;
		helper_mgr.insertSaveAndUpdate(this);

		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_hero_inheritance_times);
	}

	int playerPilots::getMaxPolitEquipmentNum()
	{
		int max_num = 0;
		for(playerPilotsMap::iterator iter = pilots.begin(); iter != pilots.end(); ++iter)
		{
			int num = 0;
			const vector<playerItemPtr>& item_vec = iter->second.pilotEquipment;
			for (unsigned i = 0; i < playerEquipPosDivide; ++i)
			{
				if (item_vec[i] != playerItemPtr())
					++num;
			}
			if(num > max_num)
				max_num = num;
		}
		return max_num;
	}

	void playerPilots::setMaxTotalAttribute(int total)
	{
		if(total > maxTotalAttribute)
		{
			maxTotalAttribute = total;
			helper_mgr.insertSaveAndUpdate(this);

			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_train_points);
		}
	}

	bool playerPilots::setAttribute(int pilotID, Json::Value attr)
	{
		playerPilot& pp = getPlayerPilot(pilotID);
		
		if (pp == playerPilot())
		{
			return false;
		}
		for (unsigned i = 0; i < attr.size(); i++)
		{
			pp.addAttribute[i] = attr[i].asInt();
		}

		//addDelayUpdate(pp);
		pp.dirty = true;
		helper_mgr.insertSaveAndUpdate(this);
		return true;
	}

	int playerPilots::getPilotType( int pilotID )
	{
		if (21 <= pilotID && pilotID <= 89)
			return pilot_type_weiwang;
		if (201 <= pilotID && pilotID <= 221)
			return pilot_type_map;
		return pilot_type_other;
	}

	void playerPilots::Disassociate(playerItemPtr item)
	{
		if (item == NULL || item->getPosition() != position_equip_space)return;
		int pID = -1;
		if (item->getEquipEx() != NULL)//是装备
		{
			pID = item->getEquipEx()->getBelong();
		}
		else
		{
			return;
		}
		playerPilot& cPilot = getPlayerPilot(pID);
		if (cPilot == NullPlayerPilot)return;
		for (unsigned i = 0; i < cPilot.pilotEquipment.size(); ++i)
		{
			if (cPilot.pilotEquipment[i] == item)
			{
				cPilot.pilotEquipment[i] = playerItemPtr();
				//addDelayUpdate(cPilot);
				cPilot.dirty = true;
				helper_mgr.insertSaveAndUpdate(this);
				break;
			}
		}
	}

}