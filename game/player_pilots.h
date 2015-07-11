#pragma once 

#include "block.h"
#include <vector>
#include <boost/unordered_map.hpp>
#include "player_item.h"
#include "pilot_def.h"
#include "playerCD.h"

using namespace std;

namespace gg
{
	enum{
		pilot_show,
		pilot_un_enlist,
		pilot_enlist,
	};

	enum
	{
		pilot_type_weiwang,
		pilot_type_map,
		pilot_type_other,

		pilot_type_end
	};

	const static string playerPilotsDBStr = "gl.player_pilots";
	const static string playerPilotsListDBStr = "gl.player_pilots_list";
	const static string playerPilotDatasStr = "d";
	const static string playerPilotRawIDStr = "rid";
	const static string playerPilotLevelStr = "lv";
	const static string playerPilotTypeStr = "tp";
	const static string playerPilotExpStr = "exp";
	const static string pilotCoreBeginStr = "cb";
	const static string pilotCoreEndStr = "ce";
	const static string pilotModeStr = "cm";
	const static string pilotAskHelpStr = "afh";//当前模式是否已经请求帮助了
	//	const static string playerPilotCurrentHPStr = "chp";
	const static string playerPilotCultureStr = "cul";			//当前培养属性
	const static string playerPilotUseItemCDstr = "ucd";
	const static string playerPilotUseItemLockStr = "uil";
	const static string playerPilotEquipListStr = "el";
	const static string playerPilotEquipIndexStr  = "x";
	const static string playerPilotEquipMentIDStr = "ei";
	const static string playerPilotNewCultureStr = "ncul";		//新洗练出的属性
	const static string playerPilotHistoryCultureStr = "hcul";//历史培养最好属性值
	const static string playerPilotIsToInherit ="itoi";			//是传承者
	const static string playerPilotIsByInherit = "ibyi";		//是被传承者
	const static string playerPilotInheritTimesStr = "it";
	const static string playerPilotMaxAttributeStr = "ma";
	const static string playerPilotBattleValueStr = "bv";//战斗属性
	const static string playerPilotTotalNumRewardStr = "tnr";//总数奖励
	const static string playerPilotVersionStr = "ver";//数据版本

	const int startPilotID = 200;

	struct playerPilot
	{
		playerPilot();
		playerPilot(const int pID, const int cLV = 1);
		bool operator ==(const playerPilot& pP) const{
			return pilotID == pP.pilotID;
		}
		void calBattleValue(playerDataPtr player = playerDataPtr(), const bool on_update = true);
		void getGrowAdded(void* point) const;
		void getEquipAdded(void* point) const;
		void onBattleValueUpdate(playerDataPtr player);
		Json::Value package(const bool FullInfo = false)const;
		int pilotID;
		int pilotLevel;
		int pilotExp;
		unsigned coreBegin;
		unsigned coreEnd;
		int coreMode;
		bool askForHelp;
		int pilotType;
		int addAttribute[3];//洗出来的属性
		int newAttribute[3];//洗出来但没有使用的属性 
		int historyMaxAttribute[3];//历史洗练出来最高的值
		int isToInherit;
		int isByInherit;
		int battleValue;//战力
		bool dirty;
		vector<playerItemPtr> pilotEquipment;
	};

	class playerPilots : public Block
	{
	public: 
		static void initData();
		playerPilots(playerData& own);
		int EqiuipChange(const int pilotID_1, const int pilotID_2);
		void recalBattleValue(const bool update = true);
		void calBattleValue(const int pilotID, const bool update = true);
		bool canBattle(const int pilotID);
		int addExp(const int pilotID, const int exp);
		int addExpPreview(const int pilotID, const int exp);	//增加经验的预操作，英雄的经验是从零开始增加exp经验后等级变化.
		int getTotalExp(const int pilotID);					//计算出玩家总的经验值
		int updatePlayerPilot(playerPilot& cPilot);			//更新武将信息
		int inheritHero(playerPilot& cPilot,bool isSupremacy);
		int convertMode(const int pilotID, const int mode, const bool costResources = false);
		int enlist(const int pilotID);
		int fire(const int pilotID);
		int useItemCDClear(const bool costGold = true);
		int equip(const int pilotID, const int equipID, const int positionID);
		int offEquip(const int pilotID, const int positionID);
//		int coreFast(const int pilotID, const int times, const bool jumpCD = false);
		int upgradeAttribute(const int pilotID, const int upgradeType);//洗练
		int replaceAttribute(const int pilotID/*,bool isNotice = true*/);
		int clearAttribute(const int pilotID);
		int revertAttribute(const int pilotID);
		bool checkPilotActive(const int pilotID);
		bool checkPilotEnlist(const int pilotID); 
		int* getPilotHistoryMaxAttribute(const int pilotID);
		virtual bool save();
		virtual bool get();
		void update(const bool updatePilot = false);
		bool singleUpdate(const int pilotID);
		virtual void autoUpdate();
		int activePilot(const int pilotID);
		const playerPilot& getPlayerPilotExtert(const int pilotID);
		const static playerPilot NullPilotExtert;
		CDData getCD();
		bool IsLockUseItemCD();
		void addUseItemCD(unsigned s);
		bool canUpgrade(const int pilotID);
		void activeEnlistMode();
		static Params::ArrayValue GrowVec;
		int getCurrentPilotNum(){ return currentEnlistPilot; }
		int getMaxPolitLv();
		int getInheritTimes(){ return inheritTimes; }
		void addInheritTimes();
		int getMaxPolitEquipmentNum();
		int getMaxTotalAttribute(){ return maxTotalAttribute; }
		void setMaxTotalAttribute(int total);
		bool setAttribute(int pilotID, Json::Value attr);
		void Disassociate(playerItemPtr item);
		static int getMaxLv();
		void gmTotalSimplePack(Json::Value &pack);
		bool addCoreTime(const int pilotID, const unsigned time);
		void setPilotHelp(const int pilotID);
		virtual void doEnd();
	private:
		int getPilotType(int pilotID);
		int getMaxEnlistPilot();
		bool equipPosPass(const int pilotType, const bool fuzhu, const int pilotLV, const int equipType,  const int position);
		void updateExp(playerPilot& cPilot);
		bool addDelayUpdate(const int pilotID);
		bool addDelayUpdate(playerPilot& pP);
		void upgrade(playerPilot& cPilot);
	
		//Json::Value updatePilotJson;// 1
		typedef boost::unordered_set<int> updateSet;
		updateSet pilotSet;
		playerPilot& getPlayerPilot(const int pilotID);
		playerCD useItemCD;
		int currentEnlistPilot;
		int inheritTimes;
		static vector<int> pilotExpVec;
		struct upgradeMode
		{
			upgradeMode(){
				memset(this, 0x0, sizeof(upgradeMode));
			}
			double base;
			int cost;
			unsigned lastTime; 
			int vipLimit;
		};
		static vector<upgradeMode> modeVec;
		static vector<int> modeBase;
		static playerPilot NullPlayerPilot;
		void sendAllPilotsInfo();
		void sendPilotBaseInfo();
		typedef boost::unordered_map<int , playerPilot> playerPilotsMap;
		playerPilotsMap pilots;
		int maxTotalAttribute;
		int dataVersion;

	};

}