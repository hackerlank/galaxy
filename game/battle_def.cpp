#include "battle_def.h"
#include "player_science.h"
#include "pilotManager.h"

namespace gg
{
	//battle buff
	battleBuff::battleBuff()
	{
		memset(this, 0x0, sizeof(battleBuff));
	}

	battleBuff::battleBuff(const int type, const int typeV, const int round, const double module)
	{
		buffType = type;
		buffVal = typeV;
		lastRound = round;
		effectValue = module;
	}

	//////////////////////////////////////////////////////////////////////////
	battleMemPtr battleMember::Create()
	{
		void* m = GGNew(sizeof(battleMember));
		return battleMemPtr(new(m)battleMember(), destory);
	}

	battleMemPtr battleMember::Create(const npcData& npc) 
	{
		void* m = GGNew(sizeof(battleMember));
		return battleMemPtr(new(m) battleMember(npc), destory);
	}

	battleMemPtr battleMember::Create(playerDataPtr player, const int pilotID, const int position)
	{
		void* m = GGNew(sizeof(battleMember));
		return battleMemPtr(new(m) battleMember(player, pilotID, position), destory);
	}

	namespace BUFFTRIGGER
	{
		typedef boost::function< void (battleMember&, battleBuff, Json::Value&) > BFUNC;
		typedef std::map<int, BFUNC> BUFFMAP;
		BUFFMAP BTRIGGER;

		void bAlterHP(battleMember& mem, battleBuff buff, Json::Value& report)
		{
			int value = mem.alterHP((int)buff.effectValue);
			Json::Value buffJson;
			buffJson.append(mem.getIdx());
			buffJson.append(buff.buffType);
			buffJson.append(mem.getHP());
			buffJson.append((int)buff.effectValue);
			report.append(buffJson);
		}

		void bAlterMP(battleMember& mem, battleBuff buff, Json::Value& report)
		{
			mem.alterEnergy((int)buff.effectValue, report);
		}

		void bSetMP(battleMember& mem, battleBuff buff, Json::Value& report)
		{
			mem.setEnergy((int)buff.effectValue, report,battle_action_energy_special_effect);
		}

		void initBUFFTRIGGER()
		{
			BTRIGGER[continuous_add_hp_buff] = &bAlterHP;
			BTRIGGER[continuous_reduce_hp_buff] = &bAlterHP;
			BTRIGGER[continuous_reduce_energy_buff] = &bAlterMP;
			BTRIGGER[reconver_backup_energy] = &bSetMP;
		}

		void BUFFDEAL(battleMember& mem, battleBuff buff, Json::Value& report)
		{
			int id = buff.buffType;
			BUFFMAP::iterator it = BTRIGGER.find(id);
			if(it == BTRIGGER.end())return;
			it->second(mem, buff, report);
		}
	}

	void battleMember::runPreBuff(Json::Value& report)
	{
		Json::Value clearJson;
		clearJson[0u] = armyIndex;
		clearJson[1u] = battle_action_clear_buff;
		clearJson[2u] = Json::arrayValue;
		clearJson[3u] = Json::arrayValue;
		for (BUFFMAP::iterator it = PreWarBuff.begin(); it != PreWarBuff.end(); )
		{
			BUFFMAP::iterator tmp = it;
			++it;
			BUFFTRIGGER::BUFFDEAL(*this, tmp->second, clearJson[3u]);
			--tmp->second.lastRound;
			if(tmp->second.lastRound > 0)continue;
			clearJson[2u].append(tmp->second.buffVal);
			PreWarBuff.erase(tmp);
		}
		if(clearJson[2u].empty() && clearJson[3u].empty())return;
		report.append(clearJson);
	}

	void battleMember::runPostBuff(Json::Value& report)
	{
		Json::Value clearJson;
		clearJson[0u] = armyIndex;
		clearJson[1u] = battle_action_clear_buff;
		clearJson[2u] = Json::arrayValue;
		clearJson[3u] = Json::arrayValue;
		for (BUFFMAP::iterator it = PostWarBuff.begin(); it != PostWarBuff.end(); )
		{
			BUFFMAP::iterator tmp = it;
			++it;
			BUFFTRIGGER::BUFFDEAL(*this, tmp->second, clearJson[3u]);
			--tmp->second.lastRound;
			if(tmp->second.lastRound > 0)continue;
			//clearJson[2u].append(tmp->second.buffVal);
			PostWarBuff.erase(tmp);
		}
		if(clearJson[2u].empty() && clearJson[3u].empty())return;
		report.append(clearJson);
	}

	battleMember::battleMember(playerDataPtr player, const int pilotID, const int position)
	{
		initData();
		if(position < 0 || position > 8)return;
		const playerPilot& battlePilot = player->Pilots.getPlayerPilotExtert(pilotID);
		if(battlePilot == playerPilots::NullPilotExtert)return;
		pilotConfig pilotConfig = pilot_sys.getPilotConfig(battlePilot.pilotID);
		if(!pilotConfig.isVaild())return;

		playerScienceBuffer science_buffer = player->Science.getBuffer();

		rawID = pilotConfig.rawID;//武将id
		pilotLv = battlePilot.pilotLevel;
		armyIndex = position;//位置
		coor.first = position / 3;
		coor.second = position % 3;
		armsType = pilotConfig.armsType;//兵种类型
		armsLevel = battlePilot.pilotLevel;//部队星级
		vector<int> proId,proNum;
		memcpy(initialAttribute, pilotConfig.initialAttribute, sizeof(initialAttribute));//武将基础统勇智
		memcpy(addAttribute, battlePilot.addAttribute, sizeof(addAttribute));//武将额外统勇智
		memcpy(initialCharacter, pilotConfig.initialCharacter, sizeof(initialCharacter));//武将基础属性
		initialCharacter[idx_hp] = pilotConfig.initialHP;// +
			//pilotConfig.growUp * battlePilot.pilotLevel;// + science_buffer.hp;//最大HP
		for (unsigned i = 0;i < characterNum; ++i){
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
		//秘书加成
		double secretaryAdd[characterNum];
		player->PlayerSecretaries.getAttrs(secretaryAdd);
		for (unsigned i = 0; i < characterNum; ++i)
			addCharacter[i] += secretaryAdd[i];
		//成长属性
		double growAdd[characterNum];
		battlePilot.getGrowAdded(growAdd);
		for (unsigned i = 0; i < characterNum; ++i)
		{
			//if(i == idx_hp)maxHp += (int)growAdd[i];
			addCharacter[i] += growAdd[i];
		}
		//星盟技能
		double GuildSkillAdd[characterNum];
		player->TradeSkill.battleAdd(GuildSkillAdd);
		for (unsigned i = 0; i < characterNum; ++i)
		{
			//if(i == idx_hp)maxHp += (int)growAdd[i];
			addCharacter[i] += GuildSkillAdd[i];
		}
		currentHp = (int)getTotalChar(idx_hp);
		memcpy(armsModules, pilotConfig.armsModules, sizeof(armsModules));//兵种行动系数
		armsR = pilotConfig.armsR;//兵种克制关系
		//武将技能
		skill_1 = pilotConfig.skill_1;
		skill_2 = pilotConfig.skill_2;
		//免疫
		ResistList = pilotConfig.resistList;
	}

	battleMember::battleMember()
	{
		initData();
	}

	battleMember::battleMember(const npcData& npc)
	{
		initData();

		pilotConfig pConfig = pilot_sys.getPilotConfig(npc.rawID);
		if(!pConfig.isVaild())return;
		if(npc.armyIndex < 0 || npc.armyIndex > 8)return;

		rawID = npc.rawID;
		//位置
		armyIndex = npc.armyIndex;
		coor.first = armyIndex / 3;
		coor.second = armyIndex % 3;
		//武将等级
		pilotLv = npc.armsLevel;
		//兵种类型
		armsType = pConfig.armsType;
		//部队星级
		armsLevel = npc.armsLevel;
		//武将基础统勇智
		memcpy(initialAttribute, pConfig.initialAttribute, sizeof(initialAttribute));
		//武将基础属性
		memcpy(initialCharacter, pConfig.initialCharacter, sizeof(initialCharacter));
		memset(initialCharacter, 0x0, sizeof(double) * idx_dodge);

		//武将额外统勇智
		memcpy(addAttribute, npc.initialAttribute, sizeof(addAttribute));

		//武将额外属性
		memcpy(addCharacter, npc.initialCharacter, sizeof(addCharacter));
		//武将最大HP
		//maxHp = npc.Hp;
		addCharacter[idx_hp] += npc.Hp;
		//武将当前HP
		currentHp = (int)getTotalChar(idx_hp);

		//兵种行动系数
		memcpy(armsModules, pConfig.armsModules, sizeof(armsModules));

		//兵种克制关系
		armsR = pConfig.armsR;

		//武将技能
		skill_1 = pConfig.skill_1;
		skill_2 = pConfig.skill_2;

		//免疫
		ResistList = pConfig.resistList;
	}

	battleMember::battleMember(battleMemPtr mem)
	{
		rawID = mem->getID();
		armyIndex = mem->getIdx();
		coor = mem->getCoor();
	}

	void battleMember::initData()
	{
		//public
		beenBalance = false;
		totalDamage = 0;

		//protected
		memset(armsModules,0x0,sizeof(armsModules));
		armsR.clear();

		//private
		rawID = -1;
		armyIndex = -1;
		currentHp =  0;
//		maxHp = 0;
		armsType = 0;
		armsLevel = 1;	
		skill_1 = Params::VarPtr();
		skill_2 = Params::VarPtr();
		memset(initialAttribute,0x0,sizeof(initialAttribute));
		memset(addAttribute,0x0,sizeof(addAttribute));
		memset(initialCharacter,0x0,sizeof(initialCharacter));
		memset(addCharacter,0x0,sizeof(addCharacter));
		energy = 0;
		signTarget.clear();
		targetIdx = 0;
		coor.first = -1;
		coor.second = -1;
		PreWarBuff.clear();
		PostWarBuff.clear();
		ResistList.clear();
	}

	double  battleMember::getCounterRate(battleMemPtr target)
	{
		double mine = getTotalChar(idx_counter);
		double other = target->getTotalChar(idx_counter);
		double rate = mine / (1.0 + mine + other * 5.0);
		double compare = mine /  10000.0;
		double res = rate > compare ? compare : rate;
		res = res < 0.0 ? 0.0 : res;
		return res;
	}

	double battleMember::getCriticalRate(battleMemPtr target)
	{
		double mine = getTotalChar(idx_crit);
		double other = target->getTotalChar(idx_crit);
		double rate = mine / (1.0 + mine + other * 3.0);
		double compare = mine /  10000.0;
		double res = rate > compare ? compare : rate;
		res = res < 0.0 ? 0.0 : res;
		return res;
	}

	double battleMember::getBlockRate(battleMemPtr target)
	{
		double mine = getTotalChar(idx_block);
		double other = target->getTotalChar(idx_block);
		double rate = mine / (1.0 + mine + other * 6.0);
		double compare = mine /  10000.0;
		double res = rate > compare ? compare : rate;
		res = res < 0.0 ? 0.0 : res;
		return res;
	}

	double battleMember::getDodgeRate(battleMemPtr target)
	{
		double mine = getTotalChar(idx_dodge);
		double other = target->getTotalChar(idx_dodge);
		double rate = mine / (1.0 + mine + other * 6.0);
		double compare = mine /  10000.0;
		double res = rate > compare ? compare : rate;
		res = res < 0.0 ? 0.0 : res;
		return res;
	}

	bool battleMember::isEnergyMember()
	{
		return skill_2 != NULL;
	}

	void battleMember::setArmySide(int side)
	{
		if(side < 0)side = 0;
		if(side > 0)side = 1;
		armyIndex = coor.first * 3 + coor.second + side * 9;
	}

	double battleMember::getArmsR(battleMemPtr ptr)
	{
		double module = 0.0;
		CheckVectorRange(armsR, ptr->getArmsType()){
			module += armsR[ptr->getArmsType()].atkModule;
		}
		CheckVectorRange(ptr->armsR, armsType){
			module -= ptr->armsR[armsType].defModule;
		}
		return 1.0 + module;
	}

	void battleMember::setEnergy(const int num, Json::Value& report, const int type /* = battle_action_effect_energy */)
	{
		if(skill_2 == NULL)return;
		int tmp = energy;
		energy = num;
		energy = energy < 0 ? 0 :energy;
		energy = energy > 300 ? 300 : energy;
		Json::Value dataJson;
		dataJson.append(armyIndex);
		dataJson.append(type);
		dataJson.append(energy);
		dataJson.append(energy - tmp);
		report.append(dataJson);
	}

	void battleMember::alterEnergy(const int num, Json::Value& report, const int type /* = battle_action_effect_energy */)
	{
		if(skill_2 == NULL)return;
		int tmp = energy;
		energy += num;
		energy = energy < 0 ? 0 :energy;
		energy = energy > 300 ? 300 : energy;
		Json::Value dataJson;
		dataJson.append(armyIndex);
		dataJson.append(type);
		dataJson.append(energy);
		dataJson.append(energy - tmp);
		report.append(dataJson);
	}

	int battleMember::alterHP(const int num)
	{
		int tmpHP = currentHp;
		currentHp += num;
		currentHp = currentHp < 0 ? 0 : currentHp;
		currentHp = currentHp > (int)getTotalChar(idx_hp) ? (int)getTotalChar(idx_hp) : currentHp;
		return currentHp - tmpHP;
	}

	int battleMember::setHP(int num)
	{
		num = num < 0 ? 0 : num;
		num = num >(int)getTotalChar(idx_hp) ? (int)getTotalChar(idx_hp) : num;
		currentHp = num;
		return 0;
	}

	void battleMember::addPreBuff(Json::Value& report, const int ID, const int IDV, const int lastRound /* = 1 */, const double module /* = 0.0 */)
	{
		BUFFMAP::iterator it = PreWarBuff.find(ID);
		if(it == PreWarBuff.end())
		{
			PreWarBuff[ID] = battleBuff(ID, IDV, lastRound, module);
			Json::Value buffJson;
			buffJson.append(getIdx());
			buffJson.append(battle_action_set_buff);
			buffJson.append(IDV);
			report.append(buffJson);
			return;
		}
		double val = it->second.lastRound * it->second.effectValue;
		double nval = lastRound * module;
		if(nval > val)
		{
// 			Json::Value rmJson;
// 			rmJson.append(getIdx());
// 			rmJson.append(battle_action_alter_buff_value);
// 			rmJson.append(it->second.buffVal);
// 			report.append(rmJson);
// 			Json::Value buffJson;
// 			buffJson.append(getIdx());
// 			buffJson.append(battle_action_set_buff);
// 			buffJson.append(IDV);
// 			report.append(buffJson);
			it->second = battleBuff(ID, IDV, lastRound, module);
		}
	}

	void battleMember::addPostBuff(const int ID, const int IDV /* = -1 */, const int lastRound /* = 1 */, const double module /* = 0.0 */)
	{
		BUFFMAP::iterator it = PostWarBuff.find(ID);
		if(it == PostWarBuff.end())
		{
			PostWarBuff[ID] = battleBuff(ID, IDV, lastRound, module);
			return;
		}
		double val = it->second.lastRound * it->second.effectValue;
		double nval = lastRound * module;
		if(nval > val)
		{
			it->second = battleBuff(ID, IDV, lastRound, module);
		}
	}

	bool battleMember::hasBuff(const int ID)
	{
		return (PreWarBuff.find(ID) != PreWarBuff.end() || 
			PostWarBuff.find(ID) != PostWarBuff.end());
	}

	int battleMember::getBuffVal(const int ID)
	{
		BUFFMAP::iterator it = PreWarBuff.find(ID);
		if(it == PreWarBuff.end())return -1;
		return it->second.buffVal;
	}

	double battleMember::getBuffModule(const int ID)
	{
		BUFFMAP::iterator it = PreWarBuff.find(ID);
		if(it == PreWarBuff.end())return 0.0;
		return it->second.effectValue;
	}

	//////////////////////////////////////////////////////////////////////////
	//battle side
	void battleSide::resetFormat()
	{
		rangeBegin = -1;
		rangeEnd = -1;
		for (unsigned x = 0; x < 3; ++x)
			for(unsigned y = 0; y < 3; ++y)
				format[x][y] = battleMemPtr();
	}

	armyVec battleSide::getMember(const int TYPE)
	{
		armyVec vec;
		bool alive = TYPE & MO::mem_alive;
		bool energy = TYPE & MO::mem_energy;
		bool nfenergy = TYPE & MO::mem_nfenetgy;
		for (unsigned  i = 0; i < memVec.size(); ++i)
		{
			battleMemPtr mem = memVec[i];
			if(alive && mem->isDead())continue;
			if(energy && mem->skill_2 == NULL)continue;
			if(nfenergy && (mem->skill_2 == NULL || mem->getEnergy() >= 100))continue;
			vec.push_back(memVec[i]);
		}
		return vec;
	}

	void battleSide::initData()
	{
		Loop = 0;
		memVec.clear();
		sideName = "";
		faceID = -1;
		sideLevel = 1;
		sideValue = 999;
		sideID = -1;
		netID = -1;
		fileName = "";
		rangeBegin =-1;
		rangeEnd = -1;
		winNum = 0;
		maxWin = 0;
		isPlayer = false;
		sideType = 0;
		inspireNum = -1;
		resetFormat();
	}

	battleSide::battleSide(playerDataPtr player, bSideReCall hanlder /* = NULL */) :
		createTime(na::time_helper::get_current_time()), sphereID(player->Base.getSphereID())
	{
		initData();
		isPlayer = true;
		vector<int> pilotVec = player->Embattle.getCurrentFormatPilot();
		this->sideID = player->playerID;
		this->netID = player->netID;
		this->faceID = player->Base.getFaceID();
		this->sideName = player->Base.getName();
		this->sideLevel = player->Base.getLevel();
		this->sideValue = player->Embattle.getCurrentBV();
		for(unsigned  i = 0; i < pilotVec.size(); ++i){
			int pilotID = pilotVec[i];
			battleMemPtr mem = battleMember::Create(player, pilotID, i);
			if(mem == battleMemPtr() || mem->getID() < 0)continue;
			memVec.push_back(mem);
		}
		if(hanlder != NULL)
		{
			hanlder(player, *this);
		}
	}

	battleSide::battleSide(mapDataPtr mD) :
		createTime(na::time_helper::get_current_time()), sphereID(4)
	{
		initData();
		this->sideID = mD->localID;
		this->faceID = mD->faceID;
		this->sideName = mD->mapName;
		this->sideLevel = mD->mapLevel;
		this->sideValue = mD->mapValue;
		this->sideType = mD->mapType;
		npcDataList& npcVec = mD->npcArmy;
		for (unsigned  i = 0; i < npcVec.size(); ++i)
		{
			const npcData& npc = npcVec[i];
			battleMemPtr mem = battleMember::Create(npc);
			if(mem == battleMemPtr() || mem->getID() < 0)continue;
			memVec.push_back(mem);
		}
	}

	battleSide::battleSide(teamMapDataPtr mD, mapDataPtr npc) :
		createTime(na::time_helper::get_current_time()), sphereID(npc->sphereID)
	{
		initData();
		this->sideID = npc->localID;
		this->faceID = npc->faceID;
		this->sideName = npc->mapName;
		this->sideLevel = npc->mapLevel;
		this->sideValue = npc->mapValue;
		this->maxWin = mD->teamWinMax;
		//this->sphereID = npc->sphereID;
		npcDataList& npcVec = npc->npcArmy;
		for (unsigned  i = 0; i < npcVec.size(); ++i)
		{
			const npcData& npc = npcVec[i];
			battleMemPtr mem = battleMember::Create(npc);
			if(mem == battleMemPtr() || mem->getID() < 0)continue;
			memVec.push_back(mem);
		}
	}

	int battleSide::getLostArmyNum()
	{
		int num = 0;
		for (unsigned  i = 0; i < memVec.size(); ++i)
		{
			if(memVec[i]->isDead())
				++num;
		}
		return num;
	}

	void battleSide::dealDeadAndPackage(Json::Value& dataJson)
	{
		static battleMemPtr NullMemPtr = battleMemPtr();
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j){
				battleMemPtr mem = format[i][j];
				if(mem == battleMemPtr())continue;
				if(mem->getHP() <= 0){
					format[i][j] = NullMemPtr;
					Json::Value deadJson;
					deadJson.append(mem->getIdx());
					deadJson.append(battle_action_set_dead_army);
					dataJson.append(deadJson);
				}
			}
	}

	battleMemPtr battleSide::getNext()
	{
		if(Loop < 0 || Loop > 8)return battleMemPtr();
		for(int i = Loop; i < 9; ++i){
			int X = i / 3;
			int Y = i % 3;
			battleMemPtr mem = getMember(pair<int, int>(X, Y));
			if(mem != battleMemPtr()){
				Loop = i + 1;
				return mem;
			}
		}
		return battleMemPtr();
	}

	int battleSide::aliveNum()
	{
		int num = 0;
		for (int x = 0; x < 3;  ++x)
			for (int y = 0; y < 3;  ++y){
				if(format[x][y] != NULL)	++num;
			}
			return num;
	}

	void battleSide::initBattle(const int side)
	{
		resetFormat();
		if(side == atk_side){
			rangeBegin = 0;
			rangeEnd = 8;
		}else
			if(side == def_side){
				rangeBegin = 9;
				rangeEnd = 17;
			}
			for (unsigned i = 0; i < memVec.size(); ++i)
			{
				memVec[i]->setArmySide(side);
				memVec[i]->removeAllBuff();
				if(!memVec[i]->isDead()){
					int x = memVec[i]->getCoor().first;
					int y = memVec[i]->getCoor().second;
					format[x][y] = memVec[i];
				}
			}
	}

	int battleSide::getStar()
	{
		double rate = getLossRate();
		if(rate < 0.2)return 5;
		if(rate < 0.4)return 4;
		if(rate < 0.6)return 3;
		if(rate < 0.8)return 2;
		return 1;
	}

	int battleSide::getLoss()
	{
		int total = 0;
		int current = 0;
		for (unsigned i = 0;  i < memVec.size(); ++i)
		{
			battleMemPtr mem = memVec[i];
			current += mem->getHP();
			total += (int)mem->getTotalChar(idx_hp);
		}
		return total - current < 0 ? 0 : total - current;
	}

	double battleSide::getLossRate()
	{
		return 1.0 - getLeftRate();
	}

	double battleSide::getLeftRate()
	{
		int total = 0;
		int current = 0;
		for (unsigned i = 0;  i < memVec.size(); ++i)
		{
			battleMemPtr mem = memVec[i];
			current += mem->getHP();
			total += (int)mem->getTotalChar(idx_hp);
		}
		double rate = (double)current/(double)total;
		return rate;
	}
}