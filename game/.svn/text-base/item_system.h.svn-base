#pragma once

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include "msg_base.h"
#include "params.hpp"
#include "playerManager.h"
#include "action_format.hpp"

using namespace na::msg;

#define item_sys (*gg::item_system::itemSys)

namespace gg
{

	//属性概率
	struct AttributePro{
		vector<double> attributeProVal;
		AttributePro(){
			attributeProVal.clear();
		}
	};
	//颜色概率
	struct ColorPro{
		vector<double> colorProVal;
		ColorPro(){
			colorProVal.clear();
		}
	};

	//属性值
	struct EqAttributeRange{		
		int	proID;
		vector<int> lowerValue;		//每个颜色数值的范围下限值
		vector<int> upperValue;		//每个颜色数值的范围上限值
		EqAttributeRange(){
			proID = 0;
			lowerValue.clear();
			upperValue.clear();
		}
	};
	typedef boost::unordered_map<int,EqAttributeRange> EqRangeMap;
	//装备属性的基本信息
	struct EqAttributeBase{	
		int eq_attr_id;
		int eq_silver;
		vector<int> eq_gold;
		EqRangeMap  eq_range;
		EqAttributeBase(){
			eq_attr_id = 0;
			eq_silver = 0;
			eq_gold.clear();
			eq_range.clear();
		}
	};
	typedef boost::unordered_map<int,EqAttributeBase> EqAttributeBaseMap;

	class item_system
	{
	public:
		void updateItemSystemInfo(msg_json& m, Json::Value& r);
		void updatePlayerItems(msg_json& m, Json::Value& r);
		void buyItem(msg_json& m, Json::Value& r);
		void saleItem(msg_json& m, Json::Value& r);
		void upgradeEquip(msg_json& m, Json::Value& r);
		void equipSuccinct(msg_json& m, Json::Value& r);
		void equipRevolution(msg_json& m, Json::Value& r);
		void buyCapacity(msg_json& m, Json::Value& r);
		void useItem(msg_json& m, Json::Value& r);
		void composeItem(msg_json& m, Json::Value& r);
		void clearUpgradeCD(msg_json& m, Json::Value& r);
		void DisintegrationReq(msg_json& m, Json::Value& r);
		void Embedded(msg_json& m, Json::Value& r);
		void crytalTakeOff(msg_json& m, Json::Value& r);

		//装备兑换
		void updateExchangeItemsInformation(msg_json& m, Json::Value& r);
		void exchangeItems(msg_json& m, Json::Value& r);

		void degradeEquipReq(msg_json& m, Json::Value& r);


		//gm
		void gmSetEquipLevelReq(msg_json& m, Json::Value& r);
		void gmSetEquipAddAttributeReq(msg_json& m, Json::Value& r);
		void gmPlayerItemUpdateReq(msg_json& m, Json::Value& r);

		void packageBaseItemSys(playerDataPtr player, Json::Value& packJson);
		//装备属性覆盖
		void replaceEqAttribut(msg_json& m,Json::Value& r);
		//装备属性维持
		void keepEqAttribut(msg_json& m,Json::Value& r);
		//装备普通改造
		void ordinaryTransformEquip(msg_json& m,Json::Value& r);
		//装备定向改造
		void directionalTransformEquip(msg_json& m,Json::Value& r);

		//开始改造
		void startTransfromEq(playerItemPtr item,int itemQuality,bool method = false,int eqProperty[3]=0);
		//普通洗练
		void ordinaryWashsPractice(equipExPtr equipEx,int itemQuality);
		void Wash(EqProValue outVal[EqPVSize], int itemQuality);
		//定向洗练
		void directionalWashsPractice(equipExPtr equipEx,int itemQuality,int eqProperty[3]=0);
		int randProValue(vector<int>& arr,int num,vector<int>& flag,vector<double> &vec);
		
		item_system();
		static item_system* const itemSys;
		void initData();
		Params::ObjectValue& getConfig(const int itemID);
		actionFormat::actionVec getItemAction(const int itemID);
		actionFormat::actionVec getItemDisint(const int itemID);

	private:
		
		void revolutonConver(equipExPtr dest_, equipExPtr source_, Params::Var& configDest_, Params::Var& configSource_);
		typedef boost::unordered_map<int , Params::ObjectValue > itemMap;
		typedef boost::unordered_map<int ,actionFormat::actionVec> actionItemMap;
		actionItemMap aIMap, DstMap;
		itemMap iMap;
		AttributePro m_AttributePro;
		ColorPro  m_ColorPro;
		EqAttributeBaseMap m_EqAttrBaseMap;

	};
}