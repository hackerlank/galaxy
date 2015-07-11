#include "player_item.h"
#include "item_system.h"
#include "gate_game_protocol.h"
#include "playerManager.h"
#include "item_system.h"
#include "helper.h"
#include <cstdlib> 
#include "record_system.h"
#include "task_system.h"
#include "game_server.h"
#include "pilot_def.h"

using namespace mongo;

namespace gg
{
#define MIN(A,B) (A<B?A:B)
#define MAX(A,B) (A>B?A:B)

#define  playerItemCreate(Name)\
	playerItemPtr Name::Create(const int itemID, itemFactory* manager){\
	if(item_sys.getConfig(itemID).isEmpty())return playerItemPtr();\
	void* m = GGNew(sizeof(Name));\
	Name* point = new(m) Name(manager);\
	playerItemPtr playerItem = playerItemPtr(point, playerItem::destory);\
	point->FormatData(itemID);\
	return playerItem;\
}\
	playerItemPtr Name::Create(mongo::BSONObj& obj, itemFactory* manager){\
	int itemID = -1;\
	checkNotEoo(obj[playerLocalIDstr]){itemID = obj[playerLocalIDstr].Int() / itemOffset;}\
	if(item_sys.getConfig(itemID).isEmpty())return playerItemPtr();\
	void* m = GGNew(sizeof(Name));\
	Name* point = new(m) Name(manager);\
	playerItemPtr playerItem = playerItemPtr(point, playerItem::destory);\
	point->FormatData(obj);\
	return playerItem;\
}

	playerItemCreate(playerItem);

	const static string mysqlLogItem = "log_player_item";
	const static string mysqlLogWare = "log_ware";
	const static string mysqlLogItemTrack = "log_item_track";
	const static string mysqlLogItemFriendTrack = "log_fitem_track";
	const static string mysqlLogItemTotalTrack = "log_titem_track";

	int playerItem::Add(const int num, Params::ObjectValue& itemCig)
	{
		if (getPosition() != position_ware)return 0;
		int bStackNum = stackNum;
		int addNum = 0;
		if(num < 1 || !isVaild())return addNum;
		if(itemCig.isEmpty())return addNum;
		if(itemCig[ItemDefine::itemIDStr].asInt() != getItemID())return addNum;
		int totalStack = itemCig[ItemDefine::itemStackStr].asInt();
		if(stackNum >= totalStack)return addNum;
		addNum = MIN((totalStack - stackNum), num);
		stackNum += addNum;
		if(addNum > 0)dirty = true;
		if(Father != NULL)
		{
			playerDataPtr player = Father->getFatherPlayer();
			if(player != NULL)
			{
				//道具ID, 变化前数量, 产出消耗, 当前数量, 变化数量
				StreamLog::LogV(mysqlLogItemTrack, player, localID, bStackNum, 0, 
					boost::lexical_cast<string, int>(stackNum) , boost::lexical_cast<string, int>(addNum));
			}
		}
		return addNum;
	}

	int playerItem::Add(const int num)
	{
		Params::ObjectValue& config = item_sys.getConfig(getItemID());
		return Add(num, config);
	}

	int playerItem::Remove(const int num)
	{
		int rmNum = RemoveD(num);
		if(isVaild() && rmNum > 0 && Father != NULL)
		{
			Father->sortF(getItemID());
		}
		return rmNum;
	}

	int playerItem::RemoveD(const int num)
	{
		if (getPosition() != position_ware)return 0;
		int bStackNum = stackNum;
		int removeNum = 0;
		if(num < 1)return removeNum;
		int tmp = stackNum - num;
		removeNum = tmp < 0 ? stackNum : num;
		stackNum -= removeNum;
		if(! isVaild())RemoveOwn();
		if(removeNum > 0)dirty = true;
		if(Father != NULL)
		{
			playerDataPtr player = Father->getFatherPlayer();
			if(player != NULL)
			{
				//道具ID, 变化前数量, 产出消耗, 当前数量, 变化数量
				StreamLog::LogV(mysqlLogItemTrack, player, localID, bStackNum, 1, 
					boost::lexical_cast<string, int>(stackNum) , boost::lexical_cast<string, int>(removeNum));
			}
		}
		return removeNum;
	}

	int playerItem::Delete()
	{
		int tmp = stackNum;
		stackNum = 0;
		if(Father != NULL)
		{
			playerDataPtr player = Father->getFatherPlayer();
			if(player != NULL)
			{
				//道具ID, 变化前数量, 产出消耗, 当前数量, 变化数量
				StreamLog::LogV(mysqlLogItemTrack, player, localID, tmp, 1, 
					boost::lexical_cast<string, int>(stackNum) , boost::lexical_cast<string, int>(tmp));
			}
		}
		RemoveOwn();
		dirty = true;
		return tmp;
	}

	int playerItem::createLocalID(const int itemID)
	{
		int localID = itemID * itemOffset;
		if(Father != NULL)localID = Father->despatchIDF(itemID);
		return localID;
	}

	Json::Value playerItem::toJson()
	{
		Json::Value item;
		item[playerLocalIDstr] = localID;
		item[playerStackNumStr] = stackNum;
		item[playerBagClassStr] = position;
		if(equipPtr != NULL)
		{
			item[playerItemLevelStr] = equipPtr->getLv();
			item[playerItemBelongStr] = equipPtr->getBelong();
			item[playerItemCrytalIDStr] = equipPtr->getCrytal();
			EqProValue val[EqPVSize];
			equipPtr->getCulSubPro(val);
			for (int i =0; i < EqPVSize; ++i)
			{
				if(val[i].isVaild())
				{
					item[EqCulSubProTypeArrStr[i]] = val[i].typePro;
					item[EqCulSubProNumArrStr[i]] = val[i].valuePro;
				}
			}

			equipPtr->getNewSubPro(val);
			for (int i =0; i < EqPVSize; ++i)
			{
				if(val[i].isVaild())
				{
					item[EqNewSubProTypeArrStr[i]] = val[i].typePro;
					item[EqNewSubProNumArrStr[i]] = val[i].valuePro;
				}
			}
		}
		return item;
	}

	mongo::BSONObj playerItem::toBson()
	{
		mongo::BSONObjBuilder builder;
		if(Father == NULL)return builder.obj();
		playerDataPtr player = Father->getFatherPlayer();
		if(player == NULL)return builder.obj();
		int FatherID = player->playerID;
		builder << playerLocalIDstr << localID << playerStackNumStr << stackNum << 
			playerBagClassStr << position << playerIDStr << FatherID;
		if(equipPtr != NULL)
		{
			builder << playerItemLevelStr << equipPtr->getLv() << playerItemCrytalIDStr
				<< equipPtr->getCrytal();

			EqProValue val[EqPVSize];
			equipPtr->getCulSubPro(val);
			for (int i =0; i < EqPVSize; ++i)
			{
				builder << EqCulSubProTypeArrStr[i] << val[i].typePro << 
					EqCulSubProNumArrStr[i] << val[i].valuePro;
			}

			equipPtr->getNewSubPro(val);
			for (int i =0; i < EqPVSize; ++i)
			{
				builder << EqNewSubProTypeArrStr[i] << val[i].typePro << 
					EqNewSubProNumArrStr[i] << val[i].valuePro;
			}
		}
		return builder.obj();
	}

	void playerItem::FormatData(mongo::BSONObj& obj)
	{		
		checkNotEoo(obj[playerLocalIDstr]){localID = obj[playerLocalIDstr].Int();}
		checkNotEoo(obj[playerStackNumStr]){stackNum = obj[playerStackNumStr].Int();}
		checkNotEoo(obj[playerBagClassStr]){position = obj[playerBagClassStr].Int();}
		completeItem();
		if(equipPtr != NULL)
		{
			equipPtr->FormatData(obj);
		}
		dirty = false;
		AddOwn(true);
	}

	void playerItem::FormatData(const int itemID)
	{
		Params::ObjectValue& config = item_sys.getConfig(itemID);
		if(config.isEmpty())return;
		localID = createLocalID(itemID);
		completeItem(config);
		dirty = true;
		AddOwn();
		if(Father != NULL)
		{
			playerDataPtr player = Father->getFatherPlayer();
			if(player != NULL)
			{
				//道具ID, 变化前数量, 产出消耗, 当前数量, 变化数量
				StreamLog::LogV(mysqlLogItemTrack, player, localID, 0, 0, "1", "1");
			}
		}
	}

	void playerItem::completeItem()
	{
		Params::ObjectValue& obj = item_sys.getConfig(getItemID());
		if(!obj.isEmpty())completeItem(obj);
	}

#define CheckKeyIsNull(key) if(mParam.find(key) == NullValueMember)

	void playerItem::completeItem(Params::Var& itemConfig)
	{
		if(itemConfig.isMember(ItemDefine::equipment)){
			equipPtr = equipmentExpand::Create(this);
			EqProValue pro[EqPVSize];
			if (getItemID() == WashSpecial::GuideWashItem)
			{
				memcpy(equipPtr->culPro, WashSpecial::Default, sizeof(WashSpecial::Default));
				memset(equipPtr->newPro, 0x0, sizeof(equipPtr->newPro));
			}
			else if(itemConfig[ItemDefine::itemQualityStr].asInt()==2)
			{
				item_sys.Wash(pro, 3);
				memcpy( equipPtr->newPro, pro, sizeof(pro));
			}
			else if(itemConfig[ItemDefine::itemQualityStr].asInt() > 2)
			{
				item_sys.Wash(pro, itemConfig[ItemDefine::itemQualityStr].asInt());
				memcpy(equipPtr->culPro, pro, sizeof(pro));
				memset(equipPtr->newPro, 0x0, sizeof(equipPtr->newPro));
			}
		}else{
			equipPtr = equipExPtr();
		}

// 		if (itemConfig.isMember(ItemDefine::crystal)){
// 			crystalPtr = crystalExpand::Create();
// 		}else{
// 			crystalPtr = cryExPtr();
// 		}
	}

	void playerItem::setPosition(const int pos)
	{
		if(pos == position_equip_space || pos == position_ware)
		{
			dirty = true;
			if(Father != NULL && position != pos)
			{
				Father->positionF(position, pos);
			}
			bool isSort = false;
			if (position != position_ware && pos == position_ware)
			{
				isSort = true;
			}
			position = pos;
			if(Father != NULL)
			{
				if (isSort)
				{
					Father->sortF(getItemID());
				}
				else
				{
					Father->updateF();
				}
			}
		}
	}

	playerDataPtr playerItem::getInstance_()
	{
		if (Father == NULL)
		{
			return playerDataPtr();
		}
		return Father->getInstance_();
	}

	void playerItem::updateMine()
	{
		dirty = true;
		if (Father != NULL)
		{
			Father->updateF(1);
		}
	}

	void playerItem::updateAndsaveMine()
	{
		dirty = true;
		if(Father != NULL)
		{
			Father->updateF();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool 	EqProValue::isVaild()
	{
		return (!(typePro == 0 && valuePro == 0) && typePro >= 0 && typePro < characterNum);
	}

	//////////////////////////////////////////////////////////////////////////
// 	crystalExpand::crystalExpand()
// 	{
// 		belongEquip = -1;
// 	}
	//////////////////////////////////////////////////////////////////////////
	equipmentExpand::equipmentExpand(playerItem* manager) : Father(manager)
	{
		equipLevel = 1;
		belongPilot = -1;
		crystalID = -1;
	}

	void equipmentExpand::setBelong(const int belong, const bool initial /* = false */)
	{
		belongPilot = belong;
		if (!initial && Father != NULL)
		{
			Father->updateMine();
		}
	}

	void equipmentExpand::getCulSubPro(EqProValue pro[EqPVSize], const bool withCrytal /* = false */)
	{
		EqProValue with[EqPVSize];
		memcpy(with, culPro, sizeof(culPro));
		if (withCrytal)
		{
			Params::ObjectValue& config = item_sys.getConfig(crystalID);
			if (!config.isEmpty() && config.isMember(ItemDefine::crystal))
			{
				double times = config[ItemDefine::crystal][ItemDefine::crystalAddTimesStr].asDouble();
				for (int n = 0; n < EqPVSize; ++n)
				{
					if (!with[n].isVaild())continue;
					with[n].valuePro = int(with[n].valuePro * times);
				}
			}
		}
		memcpy(pro, with, sizeof(with));
	}

	void equipmentExpand::setCulSubPro(EqProValue pro[EqPVSize])
	{
		memcpy(culPro, pro, sizeof(culPro));
		if(Father != NULL)
		{
			Father->updateAndsaveMine();
			playerDataPtr player = Father->getInstance_();
			if (player != NULL)
			{
				player->Pilots.calBattleValue(getBelong());
			}
		}
	}

	void equipmentExpand::getNewSubPro(EqProValue pro[EqPVSize])
	{
		memcpy(pro, newPro, sizeof(newPro));
	}

	void equipmentExpand::setNewSubPro(const EqProValue pro[EqPVSize])
	{
		memcpy(newPro, pro, sizeof(newPro));
		if(Father != NULL)
		{
			Father->updateAndsaveMine();
		}
	}

	void equipmentExpand::setLv(const int Lv)
	{
		equipLevel = Lv;
		if(Father != NULL)
		{
			Father->updateAndsaveMine();
			playerDataPtr player = Father->getInstance_();
			if (player != NULL)
			{
				player->Pilots.calBattleValue(getBelong());
			}
		}
	}

	void equipmentExpand::replaceSubPro()
	{
		if(newPro->isVaild()){
			memcpy(culPro, newPro, sizeof(newPro));
			memset(newPro,0,sizeof(newPro));
			if(Father != NULL)
			{
				Father->updateAndsaveMine();
				playerDataPtr player = Father->getInstance_();
				if (player != NULL)
				{
					player->Pilots.calBattleValue(getBelong());
				}
			}
		}
	}

	bool equipmentExpand::hasCrytal()
	{
		Params::ObjectValue& config = item_sys.getConfig(crystalID);
		bool ret = config.isMember(ItemDefine::crystal);
		if (!ret && Father != NULL)
		{
			crystalID = -1;
			Father->updateAndsaveMine();
		}
		return ret;
	}

	void equipmentExpand::embedded(const int itemID)
	{
		crystalID = itemID;
		if (Father != NULL)
		{
			Father->updateAndsaveMine();
			playerDataPtr player = Father->getInstance_();
			if (player != NULL)player->Pilots.calBattleValue(belongPilot);
		}
	}

	int equipmentExpand::takeOffCrytal()
	{
		int old = crystalID;
		crystalID = -1;
		if (Father != NULL)
		{
			Father->updateAndsaveMine();
			playerDataPtr player = Father->getInstance_();
			if (player != NULL)player->Pilots.calBattleValue(belongPilot);
		}
		return old;
	}

	void equipmentExpand::FormatData(mongo::BSONObj& obj)
	{
		checkNotEoo(obj[playerItemLevelStr]){equipLevel = obj[playerItemLevelStr].Int();}
		checkNotEoo(obj[playerItemCrytalIDStr]){ crystalID = obj[playerItemCrytalIDStr].Int(); }

		checkNotEoo(obj[EqCulSubProTypeArrStr[0]]){culPro[0].typePro = obj[EqCulSubProTypeArrStr[0]].Int();}
		checkNotEoo(obj[EqCulSubProTypeArrStr[1]]){culPro[1].typePro = obj[EqCulSubProTypeArrStr[1]].Int();}
		checkNotEoo(obj[EqCulSubProTypeArrStr[2]]){culPro[2].typePro = obj[EqCulSubProTypeArrStr[2]].Int();}
		checkNotEoo(obj[EqCulSubProNumArrStr[0]]){culPro[0].valuePro = obj[EqCulSubProNumArrStr[0]].Int();}
		checkNotEoo(obj[EqCulSubProNumArrStr[1]]){culPro[1].valuePro = obj[EqCulSubProNumArrStr[1]].Int();}
		checkNotEoo(obj[EqCulSubProNumArrStr[2]]){culPro[2].valuePro = obj[EqCulSubProNumArrStr[2]].Int();}

		checkNotEoo(obj[EqNewSubProTypeArrStr[0]]){newPro[0].typePro = obj[EqNewSubProTypeArrStr[0]].Int();}
		checkNotEoo(obj[EqNewSubProTypeArrStr[1]]){newPro[1].typePro = obj[EqNewSubProTypeArrStr[1]].Int();}
		checkNotEoo(obj[EqNewSubProTypeArrStr[2]]){newPro[2].typePro = obj[EqNewSubProTypeArrStr[2]].Int();}
		checkNotEoo(obj[EqNewSubProNumArrStr[0]]){newPro[0].valuePro = obj[EqNewSubProNumArrStr[0]].Int();}
		checkNotEoo(obj[EqNewSubProNumArrStr[1]]){newPro[1].valuePro = obj[EqNewSubProNumArrStr[1]].Int();}
		checkNotEoo(obj[EqNewSubProNumArrStr[2]]){newPro[2].valuePro = obj[EqNewSubProNumArrStr[2]].Int();}
	}

	//////////////////////////////////////////////////////////////////////////new player item
	playerWare::playerWare(playerData& own) : Block(own), playerCD(-1, 600)
	{
		updateItem.clear();
		saveItem.clear();
		TotalMap.clear();
		FriendMap.clear();
		capacity = defaultCapacity;
		wareUse = 0;
		initState = true;
	}
	playerWare::~playerWare(){}

	playerDataPtr playerWare::getFatherPlayer()
	{
		return own.getOwnDataPtr();
	}

	void playerWare::positionF(const int pre, const int now)
	{
		if(now == position_ware && pre != position_ware)++wareUse;
		else if (now != position_ware && pre == position_ware)--wareUse;
		else return;
		wareUse = wareUse < 0 ? 0 : wareUse;
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerWare::updateF(const int type /* = 2 */)
	{
		if (type == 0)
		{
			helper_mgr.insertSave(this);
		}
		else if (type == 1)
		{
			helper_mgr.insertUpdate(this);
		}
		else
		{
			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	void playerWare::removeF(playerItemPtr item)
	{
		if(item == NULL)return;
		TotalMap.erase(item->getLocalID());
		FriendMap[item->getItemID()].erase(item->getLocalID());
		if(item->getPosition() == position_ware)
		{
			--wareUse;
			wareUse = wareUse < 0 ? 0 : wareUse;
		}
		if (item->getPosition() == position_equip_space)
		{
			own.Pilots.Disassociate(item);
		}
		if (item->getEquipEx() != NULL && item->getEquipEx()->hasCrytal())
		{
			addItem(item->getEquipEx()->getCrytal(), false);
		}
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerWare::addF(playerItemPtr item, const bool direct)
	{
		if(item == NULL || !item->isVaild())return;
		TotalMap[item->getLocalID()] = item;
		FriendMap[item->getItemID()][item->getLocalID()] = item;
		saveItem[item->getLocalID()] = item;
		updateItem[item->getLocalID()] = item;
		if (item->getPosition() == position_ware)
		{
			++wareUse;
		}
		if(!direct)helper_mgr.insertSaveAndUpdate(this);
	}

	void playerWare::sortF(const int itemID)
	{
		sortOut(itemID);
		helper_mgr.insertSaveAndUpdate(this);
	}

	playerDataPtr playerWare::getInstance_()
	{
		return own.getOwnDataPtr();
	}

	int playerWare::despatchIDF(const int itemID)
	{
		int localID = itemID * itemOffset;
		int times = 0;
		do 
		{
			if(getItem(localID) == NULL)break;
			++localID;
			++ times; 
		} while (times < 1000);
		if(times >= 1000)
		{
			LogE << "despatch item localID failed ..." << own.playerID << " itemID:" <<
				itemID << LogEnd;
			return -1;
		}
		return localID;
	}

#define NullItem playerItemPtr()
	playerItemPtr playerWare::StrippedEquipModel(playerItemPtr item)
	{
		if (item == NULL || item->getPosition() != position_ware)return NullItem;
		item->Remove(1);
		return playerItem::Create(item->getItemID(), this);
	}

	playerItemPtr playerWare::getItem(const int localID, const int positionLimit /* = position_ware | position_pilot */)
	{
		playerItemMap::iterator it = TotalMap.find(localID);
		if(it == TotalMap.end())return NullItem;
		int pos = it->second->getPosition();
		if (pos == (position_ware & positionLimit) ||
			pos == (position_equip_space & positionLimit))
				return it->second;
		return NullItem;
	}

	itemVector playerWare::getAllItem(const int positionLimit /* = position_ware */)
	{
		itemVector vec;
		for (playerItemMap::iterator it = TotalMap.begin(); it != TotalMap.end(); ++it)
		{
			int pos = it->second->getPosition();
			if (pos == (position_ware & positionLimit) ||
				pos == (position_equip_space & positionLimit))
						vec.push_back(it->second);
		}
		return vec;
	}

	itemVector playerWare::getItemVec(const int itemID, const int positionLimit /* = position_ware */)
	{
		itemVector vec;
		itemFriendMap::iterator Fit = FriendMap.find(itemID);
		if(Fit == FriendMap.end())return vec;
		playerItemMap& fMap = Fit->second;
		for (playerItemMap::iterator it = fMap.begin(); it != fMap.end(); ++it)
		{
			int pos = it->second->getPosition();
			if (pos == (position_ware & positionLimit) ||
				pos == (position_equip_space & positionLimit))
					vec.push_back(it->second);
		}
		return vec;
	}

	int playerWare::getItemStack(const int itemID, const int positionLimit /* = position_ware */)
	{
		itemVector vec = getItemVec(itemID);
		int num = 0;
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			num += vec[i]->getStack();
		}
		return num;
	}

	playerItemPtr playerWare::addItem(const int itemID, const bool check /* = true */)
	{
		itemVector vec = addItem(itemID, 1, check);
		if(vec.empty())return NullItem;
		return vec[0];
	}

	bool playerWare::canRemoveItem(const int itemID, const int num)
	{
		int total = getItemStack(itemID);
		return total >= num;
	}

	void playerWare::removeItem(const int itemID, const int num)
	{
		if(num < 1 || !canRemoveItem(itemID, num))return;
		int tmp = num;
		itemVector vec = getItemVec(itemID);
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			playerItemPtr playerItem = vec[i];
			if (playerItem == playerItemPtr())continue;
			tmp -= playerItem->RemoveD(tmp);
			if (tmp <= 0)break;
		}
		helper_mgr.insertSaveAndUpdate(this);
		sortOut(itemID);
	}

	void playerWare::addCapacity(const int num)
	{
		capacity += num;
		capacity = capacity < 0 ? 0 : capacity;
		capacity = capacity > maxCapacity ? maxCapacity : capacity;
		StreamLog::Log(mysqlLogWare, own, num, capacity);
		helper_mgr.insertSaveAndUpdate(this);
	}

	itemVector playerWare::getEquip(const int color )
	{
		itemVector vec;
		for (playerItemMap::iterator it = TotalMap.begin(); it != TotalMap.end(); ++it)
		{
			playerItemPtr item = it->second;
			if(item == NULL)continue;
			equipExPtr eqExPtr = item->getEquipEx();
			if(eqExPtr == NULL)continue;
			if(color < 0 || 
				item_sys.getConfig(item->getItemID())[ItemDefine::itemQualityStr].asInt() == color)
				vec.push_back(item);
		}
		return vec;
	}

	int playerWare::getEquipNum(const int color /* = -1 */)
	{
		return (int)getEquip(color).size();
	}

	int playerWare::getMaxLvEquip(const int color /* = -1 */)
	{
		itemVector vec = getEquip(color);
		int level = vec.empty() ? 0 : vec[0]->getEquipEx()->getLv();
		for (unsigned i = 1 ; i < vec.size(); ++i)
		{
			if(vec[i]->getEquipEx()->getLv() > level)
			{
				level = vec[i]->getEquipEx()->getLv();
			}
		}
		return level;
	}

	void playerWare::tickBuyID(const int itemID, const int num)
	{
		if (num < 1)return;
		FItemStackMap::iterator it = buyLimit.find(itemID);
		if (it == buyLimit.end())buyLimit[itemID] = num;
		else it->second = it->second + num;
		helper_mgr.insertSave(this);
	}

	int playerWare::buyTimes(const int itemID)
	{
		FItemStackMap::iterator it = buyLimit.find(itemID);
		if (it == buyLimit.end())return 0;
		return it->second;
	}

	itemVector playerWare::addItem(const int itemID, const int num, const bool check /* = true */)
	{
		itemVector resVec;
		if (num < 1)return resVec;
		if(check && !canAddItem(itemID, num))return resVec;
		int tmp = num;
		Params::ObjectValue& config = item_sys.getConfig(itemID);
		if(config.isEmpty())return resVec;
		int stackNum = config[ItemDefine::itemStackStr].asInt();
		itemVector vec = getItemVec(itemID);
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			playerItemPtr playerItem = vec[i];
			if (playerItem == playerItemPtr())continue;
			int cutNum = playerItem->Add(tmp, config);
			tmp -= cutNum;
			if (cutNum > 0)
			{
				resVec.push_back(playerItem);
			}
			if (tmp <= 0)break;
		}
		do{
			if(tmp < 1)break;
			do 
			{
				playerItemPtr playerItem = playerItem::Create(itemID, this);
				if(playerItem == playerItemPtr())continue;
				resVec.push_back(playerItem);
				--tmp;
				tmp -= playerItem->Add(tmp, config);
			} while (tmp > 0);
		}while(false);
		if(! resVec.empty())helper_mgr.insertSaveAndUpdate(this);
		return resVec;
	}

	int playerWare::getLeavePlace()
	{
		return capacity < wareUse ? 0 : capacity - wareUse;
	}

	itemVector playerWare::addItemLess(const int itemID, const int num)
	{
		const int finalNum = calAddItemLess(itemID, num);
		return addItem(itemID, finalNum, false);
	}

	int playerWare::calAddItemLess(const int itemID, const int num)
	{
		int finalNum = 0;
		Params::ObjectValue& config = item_sys.getConfig(itemID);
		if(config.isEmpty())return finalNum;
		const int itemStack  = config[ItemDefine::itemStackStr].asInt();
		if(itemStack < 1)return finalNum;
		int itemFdNum = getItemStack(itemID) + num; 
		int useSpace = (int)getItemVec(itemID).size();
		int needSpace = itemFdNum / itemStack;
		const int leaveAddNum = itemFdNum % itemStack;
		needSpace = leaveAddNum == 0 ? needSpace : needSpace + 1;
		if(needSpace < useSpace)return finalNum;
		needSpace -= useSpace;
		int leaveSpace = getLeavePlace();
		if(leaveSpace >= needSpace)return num;
		else
		{
			//已经堆叠进入原始道具的数量 + 剩余背包空间 * 最大堆叠数量
			finalNum = (num - needSpace * itemStack - leaveAddNum) + leaveSpace * itemStack;
		}
		return finalNum;
	}

	const static int ImpossiableBagSize = 99999;
	int playerWare::addItemNeedSpace(const int itemID, const int num)
	{
		if(num < 1) return ImpossiableBagSize;
		Params::ObjectValue& config = item_sys.getConfig(itemID);
		if(config.isEmpty())return ImpossiableBagSize;
		int itemStack  = config[ItemDefine::itemStackStr].asInt();
		if(itemStack < 1)return ImpossiableBagSize;
		int itemFdNum = getItemStack(itemID) + num; 
		int useSpace = (int)getItemVec(itemID).size();
		int needSpace = itemFdNum / itemStack;
		needSpace = itemFdNum % itemStack == 0 ? needSpace : needSpace + 1;
		if(needSpace < useSpace)return ImpossiableBagSize;
		needSpace -= useSpace;
		return needSpace;
	}

	bool playerWare::canAddItem(const int itemID, const int num)
	{
		if(num < 1) return false;
		int needSpace = addItemNeedSpace(itemID, num);
		return getLeavePlace() >= needSpace;
	}

	itemVector playerWare::sortOut(const int itemID)
	{
		itemVector resVec,vec;
		Params::ObjectValue& config = item_sys.getConfig(itemID);
		if (config.isEmpty())return resVec;
		int total = 0;
		int stackMax = config[ItemDefine::itemStackStr].asInt();
		if (stackMax < 2)return resVec;
		vec = getItemVec(itemID);
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			playerItemPtr playerItem = vec[i];
			if (playerItem == playerItemPtr())continue;
			total += playerItem->getStack();
		}
		int needFull = total / stackMax;
		int leftItem = total % stackMax;
		int FullNum = 0;
		for (unsigned i = 0; i < vec.size(); ++i)
		{
			playerItemPtr playerItem = vec[i];
			if (playerItem == playerItemPtr())continue;
			if (FullNum < needFull){
				playerItem->Add(stackMax, config);
				++FullNum;
			}
			else if (FullNum == needFull){
				++FullNum;
				int tmp = leftItem - playerItem->getStack();
				if (tmp >= 0)playerItem->Add(tmp, config);
				else playerItem->RemoveD(-tmp);
			}
			else{
				playerItem->Delete();
			}
			resVec.push_back(playerItem);
		}
		return resVec;
	}

	bool playerWare::get()
	{
		{
			mongo::BSONObj key = BSON(playerIDStr << own.playerID);
			mongo::BSONObj obj = db_mgr.FindOne(playerWareDBstr, key);
			if(!obj.isEmpty())
			{
				checkNotEoo(obj[playerBagCapacityStr]){capacity = obj[playerBagCapacityStr].Int();}
				checkNotEoo(obj[playerUCDStr]){Cdd.CD = (unsigned)obj[playerUCDStr].Int();}
				checkNotEoo(obj[playerUCDLockStr]){Cdd.Lock = obj[playerUCDLockStr].Bool();}
				checkNotEoo(obj["lm"]){
					vector<mongo::BSONElement> vec = obj["lm"].Array();
					for (unsigned i = 0; i < vec.size(); ++i)
					{
						mongo::BSONElement& elem = vec[i];
						buyLimit[elem["id"].Int()] = elem["n"].Int();
					}
				}
			}
		}
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		objCollection objs = db_mgr.Query(playerItemDBStr, key);
		for (unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];
			if(!obj.isEmpty())
			{
				playerItem::Create(obj, this);
			}
		}
		isMap.clear();
		tsMap.clear();
		for (itemFriendMap::iterator it = FriendMap.begin(); it != FriendMap.end(); ++it)
		{
			isMap[it->first] = getItemStack(it->first);
			tsMap[it->first] = getItemStack(it->first, position_equip_space | position_ware);
		}
		return true;
	}

	void playerWare::doEnd()
	{
		for (playerItemMap::iterator it = TotalMap.begin(); it != TotalMap.end(); ++it)
		{
			playerItemPtr item = it->second;
			if (item == NULL)continue;
			item->dirty = false;
		}
	}

	bool playerWare::save()
	{
		{
			mongo::BSONObj key = BSON(playerIDStr << own.playerID);
			mongo::BSONArrayBuilder arrBuilder;
			for (FItemStackMap::iterator it = buyLimit.begin(); it != buyLimit.end(); ++it)
			{
				arrBuilder << BSON("id" << it->first << "n" << it->second);
			}
			mongo::BSONObj value = BSON(playerIDStr << own.playerID << 
				playerBagCapacityStr << capacity << playerUCDStr << Cdd.CD <<
				playerUCDLockStr << Cdd.Lock << "lm" << arrBuilder.arr());
			db_mgr.save_mongo(playerWareDBstr, key, value);
		}
		boost::unordered_set<int> mset;
		for (playerItemMap::iterator it = saveItem.begin(); it != saveItem.end();++it)
		{
			playerItemPtr item = it->second;
			if(item == NULL || !item->dirty)continue;
			mongo::BSONObj key = BSON(playerIDStr << own.playerID
				<< playerLocalIDstr << item->getLocalID());
			mongo::BSONObj value = item->toBson();
			if(item->isVaild())db_mgr.save_mongo(playerItemDBStr, key, value);
			else db_mgr.remove_collection(playerItemDBStr, key);
			equipExPtr exPtr = item->getEquipEx();
			//item->dirty = false;
			mset.insert(item->getItemID());
			if(exPtr != NULL)
			{
				string f1,f2;
				EqProValue pro[EqPVSize];
				exPtr->getCulSubPro(pro);
				for (int i = 0; i < EqPVSize; ++i)
				{
					f1 += (" [" + boost::lexical_cast<string, int>(pro[i].typePro)
						+ "," + boost::lexical_cast<string, int>(pro[i].valuePro) + "]");
				}
				exPtr->getNewSubPro(pro);
				for (int i = 0; i < EqPVSize; ++i)
				{
					f2 += (" [" + boost::lexical_cast<string, int>(pro[i].typePro)
						+ "," + boost::lexical_cast<string, int>(pro[i].valuePro) + "]");
				}
				StreamLog::LogV(mysqlLogItem, own.getOwnDataPtr(), item->getLocalID(), item->getStack(),
					item->getPosition(), f1, f2, boost::lexical_cast<string, int>(exPtr->getLv()), boost::lexical_cast<string, int>(exPtr->getBelong()));
			}
			else
			{
				StreamLog::Log(mysqlLogItem, own.getOwnDataPtr(), item->getLocalID(), item->getStack(),
					item->getPosition());
			}
		}
		for (boost::unordered_set<int>::iterator it = mset.begin(); it != mset.end(); ++it)
		{
			{
				int old = 0;
				FItemStackMap::iterator fit = isMap.find(*it);
				if (fit != isMap.end())
				{
					old = fit->second;
				}
				int now = getItemStack(*it);
				int tag = now > old ? 0 : 1;
				StreamLog::LogV(mysqlLogItemFriendTrack, own.getOwnDataPtr(), old, now, tag,
					boost::lexical_cast<string, int>(*it));
			}

			{
				int old = 0;
				FItemStackMap::iterator fit = tsMap.find(*it);
				if (fit != tsMap.end())
				{
					old = fit->second;
				}
				int now = getItemStack(*it, position_ware | position_equip_space);
				int tag = now > old ? 0 : 1;
				StreamLog::LogV(mysqlLogItemTotalTrack, own.getOwnDataPtr(), old, now, tag,
					boost::lexical_cast<string, int>(*it));
			}
		}

		saveItem = TotalMap;
		isMap.clear();
		for (itemFriendMap::iterator it = FriendMap.begin(); it != FriendMap.end(); ++it)
		{
			isMap[it->first] = getItemStack(it->first, position_equip_space | position_ware);
		}
		return true;
	}

	void playerWare::autoUpdate()
	{
		unsigned num = 0;
		Json::Value updateJson;
		updateJson[msgStr][0u] = 1;
		Json::Value& dataJson = updateJson[msgStr][1u][playerItemDataStr];
		dataJson = Json::arrayValue;
		for (playerItemMap::iterator it = updateItem.begin(); it != updateItem.end(); ++it)
		{
			playerItemPtr item = it->second;
			if (item == NULL || !item->dirty)continue;
			dataJson.append(item->toJson());
			++num;
			if (num > 30)
			{
				own.sendToClient(gate_client::player_item_update_resp, updateJson);
				dataJson = Json::arrayValue;
				num = 0;
			}
		}
		if(num > 0)
		{
			own.sendToClient(gate_client::player_item_update_resp, updateJson);
		}
		updateItem = TotalMap;
		
		Json::Value updateBase;
		updateBase[msgStr][0u] = 0;
		updateBase[msgStr][1u][playerBagCapacityStr] = capacity;
		own.sendToClient(gate_client::player_item_update_resp, updateBase);
	}

	void playerWare::addCD(unsigned s)
	{
		playerCD::addCD(s);
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerWare::clearCD()
	{
		playerCD::clearCD();
		helper_mgr.insertSaveAndUpdate(this);
	}

	CDData& playerWare::getCD()
	{
		return playerCD::getCD();
	}

	void playerWare::update()
	{
		if (initState)
		{
			initState = false;
			for (playerItemMap::iterator it = TotalMap.begin(); it != TotalMap.end();)
			{
				playerItemPtr item = it->second;
				++it;
				if (item == NULL  || item->getPosition() != position_equip_space)continue;
				equipExPtr eqPtr = item->getEquipEx();
				if (eqPtr == NULL || eqPtr->getBelong() == -1)
				{
					item->setPosition(position_ware);
				}
// 				cryExPtr cstPtr = item->getCrystalEx();
// 				if (cstPtr != NULL && cstPtr->belongEquip == -1)
// 				{
// 					item->setPosition(position_ware);
// 				}
			}
		}

		unsigned num = 0;
		Json::Value updateJson;
		updateJson[msgStr][0u] = 1;
		Json::Value& dataJson = updateJson[msgStr][1u][playerItemDataStr];
		dataJson = Json::arrayValue;
		for (playerItemMap::iterator it = TotalMap.begin(); it != TotalMap.end();++it)
		{
			playerItemPtr item = it->second;
			if(item == NULL)continue;
			dataJson.append(item->toJson());
			++num;
			if(num > 30)
			{
				own.sendToClient(gate_client::player_item_update_resp, updateJson);
				dataJson = Json::arrayValue;
				num = 0;
			}
		}
		if(num > 0)
		{
			own.sendToClient(gate_client::player_item_update_resp, updateJson);
		}

		Json::Value updateBase;
		updateBase[msgStr][0u] = 0;
		updateBase[msgStr][1u][playerBagCapacityStr] = capacity;
		own.sendToClient(gate_client::player_item_update_resp, updateBase);
	}
	
// 	void playerBagBase::produceSubPro(playerItemPtr playerItem)
// 	{
// 		Params::ObjectValue& config = item_sys.getConfig(playerItem->getItemID());
// 		//判断所买物品是否是装备
// 		if(config.isMember(ItemDefine::equipment))
// 		{
// 			int itemQuality = config[ItemDefine::itemQualityStr].asInt();
// 			if(itemQuality>=3){
// 				vector<int> v1,v2;
// 				playerItem->getEqCulSubPro(v1,v2);
// 				if((v1.size()==v2.size())&&(v1.size()==0)){
// 					item_sys.startTransfromEq(playerItem,itemQuality);
// 					item_sys.replaceAttribut(playerItem);
// 				}
// 			}
// 		}
// 	}

}