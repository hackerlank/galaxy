#pragma once

#include "game_helper.hpp"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "away_skill.h"
#include "msg_base.h"
#include "playerManager.h"
#include "pilot_def.h"

using namespace na::msg;
using namespace std;

#define pilot_sys (*gg::pilotManager::pilotMgr) 

namespace gg
{
	struct pilotBase{
		pilotBase(){
			memset(this, 0x0, sizeof(pilotBase));
			rawID = -1;
			maxLevel = 120;
		}
		int rawID;
		int growUp;
		int enlistFee;
		int initialHP;
		int initialLevel;
		int maxLevel;
		int pilotType;
		bool isSup;
		int armsType;//兵种类型
		int initialAttribute[attributeNum];//统 勇 智
		//战力系数
		int showValue;
		//0物理系数 1物理防御系数 2战法系数 3战法防御系数 4魔法系数 5魔法防御系数
		double armsModules[armsModulesNum];
		//0普攻 1普防 2战攻 3战防 4策攻 5策防 6闪避 7格挡 8暴击
		//9反击 10物理增伤 11物理减伤 12战法增伤 13战法减伤 14魔法增伤 15魔法减伤
		//16治疗量系数 17被治疗量系数
		double initialCharacter[characterNum];
	};

	struct pilotConfig : public pilotBase{
		pilotConfig();
		bool isVaild() const {
			return rawID >= 0;
		}
		vector<armsRestraint> armsR;
		Params::VarPtr skill_1;
		Params::VarPtr skill_2;
		boost::unordered_set<int> resistList;
	};

	class pilotManager
	{
	public:
		static pilotManager* const pilotMgr;
		void initPilot();
		const pilotConfig& getPilotConfig(const int pilotID);

		void pilotsUpdateBase(msg_json& m, Json::Value& r);
		void pilotsUpdate(msg_json& m, Json::Value& r);
		void pilotsSingleUpdate(msg_json& m, Json::Value& r);
		void pilotEnlist(msg_json& m, Json::Value& r);
		void pilotFire(msg_json& m, Json::Value& r);
		void pilotEquip(msg_json& m, Json::Value& r);
		void pilotOffEquip(msg_json& m, Json::Value& r);
		void upgradeAttribute(msg_json& m, Json::Value& r);
		void replaceAttribute(msg_json& m, Json::Value& r);
		void revertAttribute(msg_json& m, Json::Value& r);
//		void coreFast(msg_json& m, Json::Value& r);
		void converMode(msg_json& m, Json::Value& r);
		void useItemCDClear(msg_json& m, Json::Value& r);
		void pilotUpgrade(msg_json& m, Json::Value& r);
		void oneKeyEquip(msg_json& m, Json::Value& r);
		void totalPilotReward(msg_json& m, Json::Value& r);

		//gm
		void gmAddPilotExpReq(msg_json& m, Json::Value& r);
		void gmSetPilotExtraAttributeReq(msg_json& m, Json::Value& r);
		void gmPilotsListReq(msg_json& m, Json::Value& r);
		void gmSinglePilotInfoReq(msg_json& m, Json::Value& r);

		int getCoreFastExp(int playerLv);
		int getMaxPilotId();
	private:
		bool configUpgradeItem(const int itemID);
		int getCoreFastCost(int playerLv);
		vector<int> coreFastCostVec, coreFastExp;
		vector<int> peiyanCostKejiConfigVect;//培养所需科技点配置 跟玩家等级有关
		typedef boost::unordered_map< int , pilotConfig > pilotsMap;
		typedef boost::unordered_map< int , int > upgradeItemMap;
		pilotsMap map_;
		upgradeItemMap uItemMap;
		int maxPilotId_;
	};
}