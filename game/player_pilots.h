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
	const static string pilotAskHelpStr = "afh";//��ǰģʽ�Ƿ��Ѿ����������
	//	const static string playerPilotCurrentHPStr = "chp";
	const static string playerPilotCultureStr = "cul";			//��ǰ��������
	const static string playerPilotUseItemCDstr = "ucd";
	const static string playerPilotUseItemLockStr = "uil";
	const static string playerPilotEquipListStr = "el";
	const static string playerPilotEquipIndexStr  = "x";
	const static string playerPilotEquipMentIDStr = "ei";
	const static string playerPilotNewCultureStr = "ncul";		//��ϴ����������
	const static string playerPilotHistoryCultureStr = "hcul";//��ʷ�����������ֵ
	const static string playerPilotIsToInherit ="itoi";			//�Ǵ�����
	const static string playerPilotIsByInherit = "ibyi";		//�Ǳ�������
	const static string playerPilotInheritTimesStr = "it";
	const static string playerPilotMaxAttributeStr = "ma";
	const static string playerPilotBattleValueStr = "bv";//ս������
	const static string playerPilotTotalNumRewardStr = "tnr";//��������
	const static string playerPilotVersionStr = "ver";//���ݰ汾

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
		int addAttribute[3];//ϴ����������
		int newAttribute[3];//ϴ������û��ʹ�õ����� 
		int historyMaxAttribute[3];//��ʷϴ��������ߵ�ֵ
		int isToInherit;
		int isByInherit;
		int battleValue;//ս��
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
		int addExpPreview(const int pilotID, const int exp);	//���Ӿ����Ԥ������Ӣ�۵ľ����Ǵ��㿪ʼ����exp�����ȼ��仯.
		int getTotalExp(const int pilotID);					//���������ܵľ���ֵ
		int updatePlayerPilot(playerPilot& cPilot);			//�����佫��Ϣ
		int inheritHero(playerPilot& cPilot,bool isSupremacy);
		int convertMode(const int pilotID, const int mode, const bool costResources = false);
		int enlist(const int pilotID);
		int fire(const int pilotID);
		int useItemCDClear(const bool costGold = true);
		int equip(const int pilotID, const int equipID, const int positionID);
		int offEquip(const int pilotID, const int positionID);
//		int coreFast(const int pilotID, const int times, const bool jumpCD = false);
		int upgradeAttribute(const int pilotID, const int upgradeType);//ϴ��
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