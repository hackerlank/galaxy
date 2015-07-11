#pragma once

#include "game_helper.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "block.h"
#include "json/json.h"
#include "mongoDB.h"
#include <boost/unordered_set.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "params.hpp"
#include <map>
#include "playerCD.h"
#include "pilot_def.h"
#define pro_length 3
using namespace std;

namespace gg
{
	const static int itemOffset = 1000;
	const static int maxCapacity = 120;
	const static int defaultCapacity = 35;

	const static string playerItemDBStr = "gl.player_item";
	const static string playerWareDBstr = "gl.player_ware";

	enum{
		position_ware = 0x0001 << 0,
		position_equip_space = 0x0001 << 1,
	};

	const static string playerItemDataStr = "d";//just update
	const static string playerItemBelongStr = "bl";//just update
	
	//base
	const static string playerBagCapacityStr = "ca";
	const static string playerLocalIDstr = "li";
	const static string playerStackNumStr = "n";
	const static string playerBagClassStr = "bc";
	//cd
	const static string playerUCDStr = "ucd";
	const static string playerUCDLockStr = "ucdl";

	//equip
	const static string playerItemLevelStr = "lv";
	const static string playerItemCrytalIDStr = "cti";

	//当前附加属性
	const static string EqCulSubProTypeArrStr[] = 
	{
		"csubt1",		//当前属性1的类型
		"csubt2",		//当前属性2的类型
		"csubt3"		//当前属性3的类型
	};
	const static string EqCulSubProNumArrStr[] =	  
	{
		"csubn1",		//当前属性1的数值
		"csubn2",		//当前属性2的数值
		"csubn3"		//当前属性3的数值
	};

	//改造的附加属性
	const static string EqNewSubProTypeArrStr[] =  
	{
		"nsubt1",		//改造出来属性1的类型
		"nsubt2",		//改造出来属性2的类型
		"nsubt3"		//改造出来属性3的类型
	};
	const static string EqNewSubProNumArrStr[] =   
	{
		"nsubn1",		//改造出来属性1的数值
		"nsubn2",		//改造出来属性2的数值
		"nsubn3"		//改造出来属性3的数值
	};

	namespace ItemDefine
	{
		enum
		{
			chip_item,
			chip_pilot,

			chip_end
		};

		//base
		const static string itemIDStr = "itemID";
		const static string itemQualityStr = "itemQuality";	//--------quality
		const static string itemStackStr = "itemStack";

		//goods
		const static string merchandise = "merchandise";

		const static string itemBuyPriceStr = "buyPrice";
		const static string itemBuyGoldStr = "buyGold";
		const static string itemSalePriceStr = "salePrice";

		//equipment
		const static string equipment = "equipment";

		const static string equipValueStr = "attriValue";
		const static string equipGrowStr = "attriGrow";
		const static string equipTypeStr = "equipType";
		const static string equipUpgrdeCostStr = "equipCost";
		const static string equipRevolutionStr = "equipEvolution";
		const static string equipRevolutionCostStr = "evolutionCost";
		const static string materialIDStr = "materialID";
		const static string materialCostStr = "materiaCost";
		const static string equipLimitStr = "equipLimit";

		//consume
		const static string consume = "consume";

//		const static string consumeLifeStr = "life";
//		const static string consumeCondition = "condition";//等级,团长,国王,统治者
//		const static string consumeVector = "vector";

		//chip
		const static string chip = "chip";

		const static string chipMixTypeStr = "mixType";
		const static string chipMixIDStr = "mixID";
		const static string chipNeedNumStr = "needNum";
		const static string chipIsCrystalStr = "isCrystal";

		//material
		const static string material = "material";

		const static string materialUseLevelStr = "useLevel";
		const static string itemProvideIntStr = "provideValue";

		//Disintegrator
		const static string disintegrator = "disintegrator";

		const static string disintegrateAction = "actionV";
		const static string criGoldCost = "criGoldCost";

		//crystal
		const static string crystal = "crystal";

		const static string crystalEquipLimitStr = "equipLimit";
		const static string crystalAddTimesStr = "effectTimes";

	}

#define  playerItemCreateDefine(Name)\
	static playerItemPtr Create(const int itemID, itemFactory* manager = NULL);\
	static playerItemPtr Create(mongo::BSONObj& obj, itemFactory* manager = NULL);

	class playerItem;
	typedef boost::shared_ptr<playerItem> playerItemPtr;
	typedef boost::unordered_map<int , playerItemPtr> playerItemMap;
	typedef boost::unordered_map<int, playerItemMap> itemFriendMap;

	class itemFactory
	{
	public:
		virtual playerDataPtr getFatherPlayer()=0;
		virtual void positionF(const int, const int)=0;
		virtual void removeF(playerItemPtr) = 0;
		virtual void addF(playerItemPtr, const bool) = 0;
		virtual void sortF(const int) = 0;
		virtual int despatchIDF(const int)=0;
		virtual void updateF(const int type = 2) = 0;
		virtual playerDataPtr getInstance_() = 0;
	};

	const static int EqPVSize = 3;
	struct EqProValue
	{
		EqProValue()
		{
			typePro = 0;
			valuePro =  0;
		}
		EqProValue(int type, int value)
		{
			typePro = type;
			valuePro = value;
		}
		bool operator==(const EqProValue& other)const{
			return typePro == other.typePro && valuePro == other.valuePro;
		}
		bool isVaild();
		int typePro;
		int valuePro;
	};

// 	class crystalExpand;
// 	typedef boost::shared_ptr<crystalExpand> cryExPtr;
// 	class crystalExpand
// 	{
// 		friend class playerItem;
// 	public:
// 		static cryExPtr Create()
// 		{
// 			void* m = GGNew(sizeof(crystalExpand));
// 			cryExPtr crystalPtr = cryExPtr(new(m)crystalExpand(), crystalExpand::Destory);
// 			return crystalPtr;
// 		}
// 		int belongEquip;
// 	private:
// 		static void Destory(crystalExpand* point)
// 		{
// 			point->~crystalExpand();
// 			GGDelete(point);
// 		}
// 		crystalExpand();
// 		~crystalExpand(){}
// 	};

	namespace WashSpecial
	{
		const static int GuideWashItem = 5999;
		const static EqProValue WashNull[EqPVSize] =
		{
			EqProValue(0, 0),
			EqProValue(0, 0),
			EqProValue(0, 0)
		};
		const static EqProValue Default[EqPVSize] =
		{
			EqProValue(idx_war_def, 104),
			EqProValue(idx_counter, 55),
			EqProValue(idx_block, 39)
		};
		const static EqProValue Wash1[EqPVSize] =
		{
			EqProValue(idx_war_def, 122),
			EqProValue(idx_def, 53),
			EqProValue(idx_war, 133)
		};
		const static EqProValue Wash2[EqPVSize] =
		{
			EqProValue(idx_war_def, 160),
			EqProValue(idx_def, 117),
			EqProValue(idx_war, 165)
		};
	}

	class equipmentExpand;
	typedef boost::shared_ptr<equipmentExpand> equipExPtr;
	class equipmentExpand
	{
		friend class playerItem;
	public:
		static equipExPtr Create(playerItem* manager = NULL)
		{
			void* m = GGNew(sizeof(equipmentExpand));
			equipExPtr eqExPtr = equipExPtr(new(m) equipmentExpand(manager), equipmentExpand::Destory);
			return eqExPtr;
		}
		inline int getLv(){return equipLevel;}
		inline int getBelong(){return belongPilot;}
		void setLv(const int Lv);
		void setBelong(const int belong, const bool initial = false);
		void getCulSubPro(EqProValue pro[EqPVSize], const bool withCrytal = false);
		void setCulSubPro(EqProValue pro[EqPVSize]);
		void getNewSubPro(EqProValue pro[EqPVSize]);
		void setNewSubPro(const EqProValue pro[EqPVSize]);
		void replaceSubPro();
		bool hasCrytal();
		inline int getCrytal(){ return crystalID; }
		void embedded(const int itemID);
		int takeOffCrytal();
	private:
		void FormatData(mongo::BSONObj& obj);
		static void Destory(equipmentExpand* point)
		{
			point->~equipmentExpand();
			GGDelete(point);
		}
		playerItem* const Father;
		equipmentExpand(playerItem* manager);
		~equipmentExpand(){}
		int equipLevel;
		int belongPilot;
		int crystalID;
		EqProValue culPro[EqPVSize],newPro[EqPVSize];
	};
	
	class playerWare;
	class playerItem : 
		public boost::enable_shared_from_this<playerItem>
	{
		friend class playerWare;
	public:
		playerItemCreateDefine(playerItem)
		int getLocalID(){
			if(localID < itemOffset)return -1;
			return localID;}
		int getItemID(){
			if(localID < itemOffset)return -1;
			return localID / itemOffset;
		}
		int getStack(){return stackNum;}
		int Add(const int num, Params::ObjectValue& itemCig);
		int Add(const int num);
		int Remove(const int num);
		int Delete();
		virtual Json::Value toJson();
		virtual mongo::BSONObj toBson();
		bool isVaild()
		{
			if(stackNum < 1 || localID < itemOffset)return false;
			return true;
		}
		inline equipExPtr getEquipEx(){return equipPtr;}
//		inline cryExPtr getCrystalEx(){return crystalPtr;}
		inline int getPosition(){return position;}
		void setPosition(const int pos);
		void updateAndsaveMine();
		void updateMine();
		playerDataPtr getInstance_();
		bool dirty;
	protected:		
		static void destory(playerItem* iB)
		{
			iB->~playerItem();
			GGDelete(iB);
		}
		playerItem(itemFactory* manager) : Father(manager)
		{
			dirty = true;
			stackNum = 1;
			localID = 0;
			position = position_ware;
		}
		virtual ~playerItem(){}
		void AddOwn(const bool direct = false){
			if(isVaild() && Father != NULL){
				Father->addF(shared_from_this(), direct);
			}
		}
		void RemoveOwn(){
			if(Father != NULL)
			{
				Father->removeF(shared_from_this());
			}
			memset((void*)&Father, 0x0, sizeof(Father));
		}
		int localID;//物品id //唯一标示
		int stackNum;//数量
		int position;//位置
		int createLocalID(const int itemID);
		virtual void FormatData(mongo::BSONObj& obj);
		virtual void FormatData(const int itemID);
		int RemoveD(const int num);
	private:
		equipExPtr equipPtr;
//		cryExPtr crystalPtr;
		itemFactory* const Father;
		void completeItem();
		void completeItem(Params::Var& itemConfig);
	};

	typedef vector< playerItemPtr > itemVector;
	class playerPilots;
	class playerWare : public Block , public itemFactory , protected playerCD
	{
		friend class playerPilots;
	public:
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate();
		virtual void doEnd();
		void update();

		virtual void addCD(unsigned s);
		virtual void clearCD();
		virtual CDData& getCD();

		playerItemPtr getItem(const int localID, const int positionLimit = position_ware | position_equip_space);
		itemVector getAllItem(const int positionLimit = position_ware);
		itemVector getItemVec(const int itemID, const int positionLimit = position_ware);
		int getItemStack(const int itemID, const int positionLimit = position_ware);
		itemVector addItemLess(const int itemID, const int num);
		playerItemPtr addItem(const int itemID, const bool check = true);
		itemVector addItem(const int itemID, const int num, const bool check = true);
		void removeItem(const int itemID, const int num);
		int getLeavePlace();
		int getUsePlace(){return wareUse;}
		int getCapacity(){return capacity;}
		int calAddItemLess(const int itemID, const int num);
		int addItemNeedSpace(const int itemID, const int num);
		bool canAddItem(const int itemID, const int num);
		bool canRemoveItem(const int itemID, const int num);
		void addCapacity(const int num);
		itemVector getEquip(const int color =-1);
		int getEquipNum(const int color = -1);
		int getMaxLvEquip(const int color = -1);
		~playerWare();
		playerWare(playerData& own);
		void tickBuyID(const int itemID, const int num = 1);
		int buyTimes(const int itemID);
		playerItemPtr StrippedEquipModel(playerItemPtr);
	protected:
		itemVector sortOut(const int itemID);
	private:
		virtual playerDataPtr getFatherPlayer();
		virtual void positionF(const int, const int);
		virtual void updateF(const int type = 2);
		virtual void removeF(playerItemPtr);
		virtual void addF(playerItemPtr, const bool);
		virtual void sortF(const int);
		virtual playerDataPtr getInstance_();
		virtual int despatchIDF(const int);
		playerItemMap TotalMap;
		itemFriendMap FriendMap;
		int capacity;
		int wareUse;
		playerItemMap saveItem, updateItem;
		typedef boost::unordered_map<int, int> FItemStackMap;
		FItemStackMap isMap, tsMap;
		bool initState;
		FItemStackMap buyLimit;
	};

}