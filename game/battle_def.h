#pragma once

#include "away_skill.h"
#include "pilot_def.h"
#include "battle_member.h"
#include <boost/unordered_set.hpp>

#include "pilotManager.h"
#include "war_story.h"
#include "guild_battle_system.h"
#include "starwar_system.h"
#include "world_boss_system.h"
#include "mongoDB.h"

using namespace std;

namespace gg
{
	enum battleAction
	{
		battle_action_clear_buff,//清理buff
		battle_action_effect_energy,//mp受到影响
		battle_action_set_dead_army,//部队死亡
		battle_action_use_skill,//使用技能
		battle_action_use_skill_by_counter,//反击
		battle_action_set_buff,//设置buff
		battle_action_skill_not_success,//使用技能失败
		battle_action_buff_resist,//buff优先拦截le攻击
		battle_action_self_resist,//本自身属性拦截
		battle_action_reduce_hp,//hp减少
		battle_action_add_hp,//hp增加
		battle_action_add_hp_exchange,//吸血
		battle_action_energy_special_add,//特殊加mp
		battle_action_energy_special_reduce,//特殊减mp
		battle_action_energy_special_effect,//特殊直接改mp
//		battle_action_alter_buff_value,//减少buff计数
		battle_action_buff_effect_resist,//buff免疫
	};

	struct battleBuff
	{
		battleBuff();			
		battleBuff(const int type, const int typeV, const int round, const double module);
		int buffType;
		int buffVal;
		int lastRound;
		double effectValue;
	};

	typedef std::map<int, battleBuff> BUFFMAP;
	typedef boost::unordered_set<int> ResistSet;

#define CheckVectorRange(v, i)\
	if(i >= 0 && (unsigned)(i) < v.size())

	namespace BUFFTRIGGER
	{
		void initBUFFTRIGGER();
	}
	class battleSide;
	class battleMember
	{
		friend class battleSide;
		friend void guild_battle_system::addBattleBuff(playerDataPtr pdata, battleSide& side);
		friend void starwar_system::addBattleBuff(playerDataPtr pdata, battleSide& side);
		friend void world_boss_system::addBattleBuff(playerDataPtr pdata, battleSide& side);
	public:
		static battleMemPtr Create(playerDataPtr player, const int pilotID, const int position);
		static battleMemPtr Create(const npcData& npc);
		static battleMemPtr CopyForTargetVec(battleMemPtr base){
			void* m = GGNew(sizeof(battleMember));
			return battleMemPtr(new(m) battleMember(base), destory);
		}
		battleMember(playerDataPtr player, const int pilotID, const int position);
		battleMember(const npcData& npc);
		battleMember(battleMemPtr mem);
		~battleMember(){}
		void resetState()
		{
			currentHp = getTotalAttri(idx_hp);
			PreWarBuff.clear();
			PostWarBuff.clear();
			energy = 0;
			signTarget.clear();
			targetIdx = 0;
			beenBalance = false;
			totalDamage = 0;
		}
		battleMemPtr copyState()
		{
			battleMemPtr newMem = Create();

			if (newMem != NULL)
			{
				memmove(newMem->armsModules, armsModules, sizeof(armsModules));
				newMem->armsR = armsR;
				newMem->rawID = rawID;
				newMem->armyIndex = armyIndex;
				newMem->armsType = armsType;
				newMem->armsLevel = armsLevel;
				newMem->skill_1 = skill_1;
				newMem->skill_2 = skill_2;
				memmove(newMem->initialAttribute, initialAttribute, sizeof(initialAttribute));
				memmove(newMem->addAttribute, addAttribute, sizeof(addAttribute));
				memmove(newMem->initialCharacter, initialCharacter, sizeof(initialCharacter));
				memmove(newMem->addCharacter, addCharacter, sizeof(addCharacter));
				newMem->coor.first = coor.first;
				newMem->coor.second = coor.second;
				newMem->pilotLv = pilotLv;
				newMem->currentHp = (int)newMem->getTotalChar(idx_hp);
			}
			return newMem;
		}
		bool isResist(const int buffID)
		{
			return ResistList.find(buffID) != ResistList.end();
		}
		mongo::BSONObj stateToBson()
		{
			mongo::BSONObjBuilder builder;
			builder << "rID" << rawID  << "aT" << armsType << "aL" << armsLevel << "crf" << coor.first <<
				"cfs" << coor.second << "adx" << armyIndex << "plv" << pilotLv;
			if (skill_1 != NULL)
			{
				builder << "sk1" << (*skill_1)[skillIDStr].asInt();
			}
			else
			{
				builder << "sk1" << -1;
			}
			if (skill_2 != NULL)
			{
				builder << "sk2" << (*skill_2)[skillIDStr].asInt();
			}
			else
			{
				builder << "sk2" << -1;
			}
			//resist
			mongo::BSONArrayBuilder resistBuilder;
			for (ResistSet::iterator it = ResistList.begin(); it != ResistList.end(); ++it)
			{
				resistBuilder << *it;
			}
			builder << "rest" << resistBuilder.arr();
			//armsR
			mongo::BSONArrayBuilder armsRBuilder;
			for (unsigned i = 0; i < armsR.size(); ++i)
			{
				armsRestraint& aR = armsR[i];
				mongo::BSONObjBuilder obj;
				obj << "am" << aR.atkModule << "dm" << aR.defModule;
				armsRBuilder << obj.obj();
			}
			builder << "aR" << armsRBuilder.arr();
			//armsModules
			mongo::BSONArrayBuilder armsModulesBuilder;
			for (unsigned i = 0; i < armsModulesNum; ++i)
			{
				armsModulesBuilder << armsModules[i];
			}
			builder << "aMs" << armsModulesBuilder.arr();
			//initialAttribute addAttribute
			mongo::BSONArrayBuilder iAttriBuilder, aAttriBuilder;
			for (unsigned i = 0; i < attributeNum; ++i)
			{
				iAttriBuilder << initialAttribute[i];
				aAttriBuilder << addAttribute[i];
			}
			builder << "iAttri" << iAttriBuilder.arr() << "aAttri" << aAttriBuilder.arr();
			//initialCharacter addCharacter
			mongo::BSONArrayBuilder iCharBuilder, aCharBuilder;
			for (unsigned i = 0; i < characterNum; ++i)
			{
				iCharBuilder << initialCharacter[i];
				aCharBuilder << addCharacter[i];
			}
			builder << "iChar" << iCharBuilder.arr() << "aChar" << aCharBuilder.arr();
			return builder.obj();
		}
		void bsonToState(mongo::BSONElement& elem)
		{
			rawID = elem["rID"].Int();
			armsType = elem["aT"].Int();
			armsLevel = elem["aL"].Int();
			armyIndex = elem["adx"].Int();
			pilotLv = elem["plv"].Int();
			coor.first = elem["crf"].Int();
			coor.second = elem["cfs"].Int();
			skill_1 = skill_sys.getSkill(elem["sk1"].Int());
			skill_2 = skill_sys.getSkill(elem["sk2"].Int());
			if (!elem["rest"].eoo())
			{
				ResistList.clear();
				vector<mongo::BSONElement> resist = elem["rest"].Array();
				for (unsigned i = 0; i < resist.size(); ++i)
				{
					ResistList.insert(resist[i].Int());
				}
			}
			vector<mongo::BSONElement> elemVec = elem["aR"].Array();
			//armsR
			armsR.clear();
			for (unsigned i = 0; i < elemVec.size(); ++i)
			{
				mongo::BSONElement& cE = elemVec[i];
				armsR.push_back(armsRestraint(cE["am"].Double(), cE["dm"].Double()));
			}
			//armsModules
			memset(armsModules, 0x0, sizeof(armsModules));
			elemVec = elem["aMs"].Array();
			for (unsigned i = 0; i < elemVec.size(); ++i)
			{
				mongo::BSONElement& cE = elemVec[i];
				armsModules[i] = cE.Double();
			}

			memset(initialAttribute, 0x0, sizeof(initialAttribute));
			memset(addAttribute, 0x0, sizeof(addAttribute));
			memset(initialCharacter, 0x0, sizeof(initialCharacter));
			memset(addCharacter, 0x0, sizeof(addCharacter));
			//initialAttribute addAttribute
			elemVec = elem["iAttri"].Array();
			for (unsigned i = 0; i < elemVec.size(); ++i)
			{
				mongo::BSONElement& cE = elemVec[i];
				initialAttribute[i] = cE.Int();
			}
			elemVec = elem["aAttri"].Array();
			for (unsigned i = 0; i < elemVec.size(); ++i)
			{
				mongo::BSONElement& cE = elemVec[i];
				addAttribute[i] = cE.Int();
			}
			//initialCharacter addCharacter
			elemVec = elem["iChar"].Array();
			for (unsigned i = 0; i < elemVec.size(); ++i)
			{
				mongo::BSONElement& cE = elemVec[i];
				initialCharacter[i] = cE.Double();
			}
			elemVec = elem["aChar"].Array();
			for (unsigned i = 0; i < elemVec.size(); ++i)
			{
				mongo::BSONElement& cE = elemVec[i];
				addCharacter[i] = cE.Double();
			}
		}
		int getID(){return rawID;}
		int getLv(){return pilotLv;}
		int getIdx(){return armyIndex;}
		int getHP(){return currentHp;}
		int getBaseAttri(const int idx)
		{
			if(idx < 0 || (unsigned)idx >= attributeNum)return 0;
			return initialAttribute[idx];
		}
		int getAddAttri(const int idx)
		{
			if(idx < 0 || (unsigned)idx >= attributeNum)return 0;
			return addAttribute[idx];
		}
		int getTotalAttri(const int idx)
		{
			if(idx < 0 || (unsigned)idx >= attributeNum)return 0;
			return initialAttribute[idx] + addAttribute[idx];
		}
		double getBaseChar(const int idx)
		{
			if(idx < 0 || (unsigned)idx >= characterNum)return 0.0;
			return initialCharacter[idx];
		}
		double getAddChar(const int idx)
		{
			if(idx < 0 || (unsigned)idx >= characterNum)return 0.0;
			return addCharacter[idx];
		}
		void setAddChar(const int idx, int num)
		{
			if (idx < 0 || (unsigned)idx >= characterNum)return;
			addCharacter[idx] = num;
		}
		double getTotalChar(const int idx)
		{
			if(idx < 0 || (unsigned)idx >= characterNum)return 0.0;
			return initialCharacter[idx] + addCharacter[idx];
		}
		int getArmsType(){return armsType;}
		int getArmsLevel(){return armsLevel;}
		Params::VarPtr skill_1;
		Params::VarPtr skill_2;
		double getArmsModule(const int module){
			if(module < 0 || (unsigned)module >= armsModulesNum)return 0.0;
			return armsModules[module];
		}
		int getEnergy(){return energy;}
		int alterHP(const int num);
		int setHP(int num);
		void alterEnergy(const int num, Json::Value& report, const int type = battle_action_effect_energy);
		void setEnergy(const int num, Json::Value& report, const int type = battle_action_effect_energy);
		pair<int ,int> getCoor(){return coor;}
		void removeAllBuff()
		{
			PreWarBuff.clear();
			PostWarBuff.clear();
		}
		void addPreBuff(Json::Value& report, const int ID, const int IDV, const int lastRound = 1, const double module = 0.0);
		void addPostBuff(const int ID, const int IDV = -1, const int lastRound = 1, const double module = 0.0);
		bool hasBuff(const int ID);
		double getBuffModule(const int ID);
		int getBuffVal(const int ID);
		void runPreBuff(Json::Value& report);
		void runPostBuff(Json::Value& report);
		void packagetTargetIdx(Json::Value& dataJson){
			for (unsigned i = 0; i < signTarget.size(); ++i)
			{
				dataJson.append(signTarget[i]->getIdx());
			}
		}
		double getArmsR(battleMemPtr ptr);
		void convertTarget(armyVec& vec){
			signTarget.clear();
			targetIdx = 0;
			signTarget = vec;
			for (unsigned i = 0; i < signTarget.size(); ++i)
			{
				signTarget[i]->beenBalance = false;
				signTarget[i]->totalDamage = 0;
			}
		}
		bool isMainTarget(battleMemPtr mem)
		{
			return getMainTarget() == mem;
		}
		battleMemPtr getMainTarget()
		{
			if(signTarget.empty())return battleMemPtr();
			return signTarget[0];
		}
		battleMemPtr getNextTarget()
		{
			if(targetIdx >= signTarget.size())return battleMemPtr();
			battleMemPtr mem = getCurrentTarget();
			++targetIdx;
			return mem;
		}
		battleMemPtr getCurrentTarget()
		{
			if(targetIdx >= signTarget.size())return battleMemPtr();
			return signTarget[targetIdx];
		}
		armyVec getAllTarget(){return signTarget;}
		bool isDead(){return currentHp <= 0;}
		bool isEnergyMember();
		void setArmySide(int side);
		double getDodgeRate(battleMemPtr target);
		double getBlockRate(battleMemPtr target);
		double getCriticalRate(battleMemPtr target);
		double getCounterRate(battleMemPtr target);

		int totalDamage;
		bool beenBalance;
	protected:
		static void destory(battleMember* point)
		{
			point->~battleMember();
			GGDelete(point);
		}
		vector<armsRestraint> armsR;//兵种克制关系
		double armsModules[armsModulesNum];//兵种行动系数
	private:
		static battleMemPtr Create();
		battleMember();
		void initData();
		//需要设置
		int rawID;
		int armyIndex;
		int pilotLv;
		int currentHp;//当前带兵数
		int armsType;//兵种类型
		int armsLevel;//将星等级
		int initialAttribute[attributeNum];//基础 统勇智
		int addAttribute[attributeNum];//额外 统勇智
		double initialCharacter[characterNum];//基础属性
		double addCharacter[characterNum];//附加属性
		//不需要设置的
		int energy;
		armyVec signTarget;
		unsigned targetIdx;
		pair<int,int> coor;
		BUFFMAP PreWarBuff;
		BUFFMAP PostWarBuff;
		ResistSet  ResistList;
	};

	class battleSide;
	typedef boost::function< void (playerDataPtr, battleSide&) > bSideReCall;
#define	 BindSideReCall(Pointer ,Func) boost::bind(&Func, Pointer, _1, _2)
#define	 BindSideReCallParam(Pointer ,Func, P1, P2) boost::bind(&Func, Pointer, P1, P2)
#define BindSideReCallPlus(Func) boost::bind(&Func, this, _1, _2)
#define BindSideReCallPParam(Func, P1, P2) boost::bind(&Func, this, P1, P2)
	
	namespace MO
	{
		enum
		{
			mem_null = 0,
			mem_alive = (0x0001 << 0),
			mem_energy = (0x0001 << 1),
			mem_nfenetgy = (0x0001 << 2),
		};
	}

	class battleSide
	{
		friend void guild_battle_system::addBattleBuff(playerDataPtr pdata, battleSide& side);
	public:
		static battleSidePtr Create()
		{
			void *m = GGNew(sizeof(battleSide));
			return battleSidePtr(new(m)battleSide(), destory);
		}
		static battleSidePtr Create(teamMapDataPtr mD, mapDataPtr npc){
			void *m = GGNew(sizeof(battleSide));
			return battleSidePtr(new(m) battleSide(mD, npc), destory);
		}
		static battleSidePtr Create(mapDataPtr mD){
			void *m = GGNew(sizeof(battleSide));
			return battleSidePtr(new(m) battleSide(mD), destory);
		}
		static battleSidePtr Create(playerDataPtr player, bSideReCall hanlder = NULL){
			void *m = GGNew(sizeof(battleSide));
			return battleSidePtr(new(m) battleSide(player, hanlder), destory);
		}
		static battleSidePtr Create(playerDataPtr player, teamMapDataPtr mD, bSideReCall handler = NULL){
			void *m = GGNew(sizeof(battleSide));
			return battleSidePtr(new(m) battleSide(player, mD, handler), destory);
		}
		enum
		{
			atk_side,
			def_side,
		};
		void resetState()
		{
			for (unsigned i = 0; i < memVec.size(); ++i)
				memVec[i]->resetState();
		}
		battleSidePtr copyState(const bool setPlayerSign = false, const int max = 4)
		{
			battleSidePtr newPtr = Create();
			if (newPtr != NULL)
			{
				newPtr->isPlayer = setPlayerSign;
				newPtr->faceID = faceID;
				newPtr->sideID = sideID;
				newPtr->sideName = sideName;
				newPtr->sideLevel = sideLevel;
				newPtr->sideValue = sideValue;
				newPtr->sideType = sideType;
				armyVec& newVec = newPtr->memVec;
				newVec.clear();
				for (unsigned i = 0; i < memVec.size(); ++i)
				{
					battleMemPtr mem = memVec[i]->copyState();
					if (mem == NULL)continue;
					newVec.push_back(mem);
					newPtr->format[mem->getCoor().first][mem->getCoor().second] = mem;
				}
				newPtr->rangeBegin = rangeBegin;
				newPtr->rangeEnd = rangeEnd;
				newPtr->maxWin = max;
				newPtr->inspireNum = inspireNum;
				memcpy((void*)&(newPtr->sphereID), &sphereID, sizeof(int));
			}
			return newPtr;
		}
		mongo::BSONObj stateToBson(const int NpcID)
		{
			mongo::BSONObjBuilder builder;
			builder << /*"ip" << isPlayer <<*/ "fid" << faceID << "pi" << sideID <<
				"sn" << sideName << "sl" << sideLevel << "sv" << sideValue <<
				"st" << sideType << "rb" << rangeBegin << "re"  << rangeEnd <<
				/*"mw" << maxWin <<*/ "in" << inspireNum << "crt" << createTime <<
				"nid" << NpcID << "spi" << sphereID;
			mongo::BSONArrayBuilder arrBuilder;
			for (unsigned i = 0; i < memVec.size(); ++i)
			{
				arrBuilder << memVec[i]->stateToBson();
			}
			builder << "al" << arrBuilder.arr();
			return builder.obj();
		}
		void bsonToState(mongo::BSONObj& obj)
		{
			//isPlayer = obj["ip"].Bool();
			faceID = obj["fid"].Int();
			sideID = obj["pi"].Int();
			sideName = obj["sn"].String();
			sideLevel = obj["sl"].Int();
			sideValue = obj["sv"].Int();
			sideType = obj["st"].Int();
			rangeBegin = obj["rb"].Int();
			rangeEnd = obj["re"].Int();
			//maxWin = obj["mw"].Int();
			inspireNum = obj["in"].Int();
			unsigned ctime = (unsigned)obj["crt"].Int();
			memcpy((void*)&createTime, &ctime, sizeof(unsigned));
			int kid = obj["spi"].Int();
			memcpy((void*)&sphereID, &kid, sizeof(int));
			vector<mongo::BSONElement> elemVec = obj["al"].Array();
			for (int i = 0; i < 3; ++i)	for (int j = 0; i < 3; ++i)	format[i][j] = battleMemPtr();
			memVec.clear();
			for (unsigned i = 0; i < elemVec.size(); ++i)
			{
				battleMemPtr mem = battleMember::Create();
				if (mem == NULL)continue;
				mem->bsonToState(elemVec[i]);
				memVec.push_back(mem);
				format[mem->coor.first][mem->coor.second] = mem;
			}
		}
		int getStar();
		int getLoss();
		double getLossRate();
		double getLeftRate();
		int getWinNum(){return winNum;}
		bool SetWinAndGoOn(){
			++winNum;
			return winNum < maxWin;
		}
		int netID;
		int getPlayerID()
		{
			if(isPlayer)return sideID;
			return -1;
		}
		int getSideID(){ return sideID; }
		void setSideID(int id){ sideID = id; }
		armyVec getMember(const int TYPE = MO::mem_alive);
		battleMemPtr getMember(const pair<int, int> c){
			if(c.first <  0 || c.first > 2 || c.second < 0 || c.second > 2)return battleMemPtr();
			return format[c.first][c.second];
		}
		bool isMember(const int idx) const{
			return (idx >= rangeBegin && idx <= rangeEnd);
		}
		void initBattle(const int side);
		int aliveNum();
		void setFileName(const string& filename){ fileName = filename; }
		string getFileName(){ return fileName; }
		int getFaceID(){return faceID;}
		string getName(){ return sideName; }
		void setName(string name){ sideName = name; }
		int getPlayerLevel(){return sideLevel;}
		int getBattleValue(){return sideValue;}
		bool isPlayerSide(){return isPlayer;};
		int getSideType(){return sideType;}
		battleMemPtr getNext();
		battleMemPtr getCurrent(){
			if(Loop < 0 || Loop > 8)return battleMemPtr();
			int X = Loop / 3;
			int Y = Loop % 3;
			return getMember(pair<int, int>(X, Y));
		}
		void resetLoop(){Loop = 0;}
		void dealDeadAndPackage(Json::Value& dataJson);
		int getLostArmyNum();
		inline int getInsNum(){return inspireNum;}
		const unsigned createTime;
		const int sphereID;
	protected:
		static void destory(battleSide* side)
		{
			side->~battleSide();
			GGDelete(side);
		}
		battleSide(teamMapDataPtr mD, mapDataPtr npc);
		battleSide(mapDataPtr mD);
		battleSide(playerDataPtr player, bSideReCall hanlder = NULL);
		battleSide(playerDataPtr player, teamMapDataPtr mD, bSideReCall hanlder = NULL) :
			createTime(na::time_helper::get_current_time()), sphereID(player->Base.getSphereID()){
			new(this) battleSide(player, hanlder);
			this->maxWin += mD->teamWinMax;
		}
		~battleSide(){}
		void initData();
	private:
		battleSide() :
			createTime(na::time_helper::get_current_time()), sphereID(-1)
		{
			initData();
		}
		void resetFormat();
		bool isPlayer;
		int faceID;
		int sideID;
		string sideName;
		int sideLevel;
		int sideValue;
		int sideType;
		int Loop;
		battleMemPtr format[3][3];
		armyVec memVec;
		string fileName;
		int rangeBegin;
		int rangeEnd;
		int maxWin;
		int winNum;
		int inspireNum;
	};
}

