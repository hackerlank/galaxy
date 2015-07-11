#include "activity_gift_defined.h"
#include "player_gift_defined.h"
#include "response_def.h"
#include "playerManager.h"
#include "action_format.hpp"
#include "email_system.h"
#include "record_system.h"
#include "gm_tools.h"

#define GIFT_DEFINED_VIP_NUM 12
#define PERDAY_RECEIVE_TIMES 1
#define MAX_ACT_DEFINED_ID 10000
namespace gg
{//删除多余的礼包
	act_gift_defined* const act_gift_defined::giftDefinedSys = new act_gift_defined();

	singleDefinedPack singleDefinedPack::sNull_ = singleDefinedPack();
	act_gift_defined::act_gift_defined()
	{
		currentPackID_ = 1;
	}

	act_gift_defined::~act_gift_defined()
	{

	}

	void act_gift_defined::gmActivityGiftDefinedModifyReq( msg_json& m, Json::Value& r )
	{
		ReadJsonArray;

		Json::Value totalDataJson = js_msg[1];
		vector<singleDefinedPack> vecTotalPack;// = vecTotalPack_;
		
		vector<int> vecTotalID;
		for (unsigned i = 0; i < totalDataJson.size(); i++)
		{
			singleDefinedPack singlePack;
			Json::Value &dataJson = totalDataJson[i];
			if (dataJson.isNull())
				continue;
			int start_time = dataJson[act_base_def::str_act_start_time].asInt();
			int end_time = dataJson[act_base_def::str_act_end_time].asInt();
			int packID = dataJson[gift_defined_def::strPackID].asInt();

			if (packID <= 0)
				Return(r, -1);

			bool isClose = (start_time == 0 && end_time == 0);

			gm_tools_mgr.actTimePretreat(start_time);
			gm_tools_mgr.actTimePretreat(end_time);
			int ret = 0;
			
			if (start_time >= end_time && start_time != 0)
			{
				ret = 2;
			}

			unsigned cur_time = na::time_helper::get_current_time();
			if ((start_time != 0 || end_time != 0) && int(cur_time) > end_time)
			{
				ret = 3;
			}

			if (ret != 0)
			{
				Return(r, ret);
			}

			int levelLimit = dataJson[gift_defined_def::strLevelLimit].asInt();
			int totalReceiveTimes = dataJson[gift_defined_def::strTotalReceiveTimes].asInt();
			string giftDescript = dataJson[gift_defined_def::strGiftDescript].asString();
			string giftName = dataJson[gift_defined_def::strGiftName].asString();

			if ((int)ceil(double(end_time - start_time + 1) / 86400) < (totalReceiveTimes / PERDAY_RECEIVE_TIMES))
			{
				if (start_time != 0 || end_time != 0)
				{
					Return(r, 4);
				}
			}

			if (!dataJson[gift_defined_def::strGiftDataList].isArray())
				Return(r, -1);

			if ((start_time != 0 || end_time != 0) && dataJson[gift_defined_def::strGiftDataList].size() < GIFT_DEFINED_VIP_NUM + 1)
				Return(r, 5);

			if ((start_time != 0 || end_time != 0) && totalReceiveTimes <= 0)
				Return(r, 6);

			if ((start_time != 0 || end_time != 0) && giftDescript.size() > 500 * 2)
				Return(r, 7);

			if ((start_time != 0 || end_time != 0) && giftName.size() > 50 * 2)
				Return(r, 8);

			for (unsigned idx = 0; idx < dataJson[gift_defined_def::strGiftDataList].size(); idx++)
			{
				Json::Value tempJson = dataJson[gift_defined_def::strGiftDataList][idx];
				if (getPilotNum(tempJson[gift_defined_def::strGift]) > 1)
					Return(r, 9);
				giftDefinedData gdData;
				gdData.vipLevel_ = tempJson[gift_defined_def::strVipLevel].asInt();
				if (gdData.vipLevel_ != idx)
					Return(r, 12);
				gdData.gift_ = tempJson[gift_defined_def::strGift];
				if (gdData.gift_.size() > 4)
					Return(r, 10);
			}
			
// 			bool isfind = false;
// 			if (packID == -1)
// 			{
// 				isfind = true;
// 			}
// 			else
// 			{
// 				for (unsigned idx = 0; idx < vecTotalPack.size(); idx++)
// 				{
// 					singleDefinedPack tempPack = vecTotalPack[idx];
// 					if (tempPack.id_ == packID)
// 					{
// 						if (start_time == 0 && end_time == 0)
// 						{
// 							isfind = true;
// 							break;
// 						}
// 						else if (tempPack.act_time_.start_time == start_time)
// 						{
// 							isfind = true;
// 							break;
// 						}
// 					}
// 				}
// 			}
// 			if (!isfind) //后台维护id
// 				Return(r, 11);

			singlePack.giftDataList_.clear();
			for (unsigned idx = 0; idx < dataJson[gift_defined_def::strGiftDataList].size(); idx++)
			{
				Json::Value tempJson = dataJson[gift_defined_def::strGiftDataList][idx];
				giftDefinedData gdData;
				gdData.vipLevel_ = tempJson[gift_defined_def::strVipLevel].asInt();
				gdData.gift_ = tempJson[gift_defined_def::strGift];
				singlePack.giftDataList_[gdData.vipLevel_] = gdData;
			}
// 			if (packID == -1)
// 				singlePack.id_ = getNewID(vecTotalPack);
// 			else
				singlePack.id_ = packID;

			vecTotalID.push_back(singlePack.id_);
			singlePack.levelLimit_ = levelLimit;
			singlePack.totalReceiveTimes_ = totalReceiveTimes;
			singlePack.giftDescript_ = giftDescript;
			singlePack.giftName_ = giftName;

			singlePack.act_time_.start_time = start_time;
			singlePack.act_time_.end_time = end_time;

// 			if (packID == -1)
// 			{
				insertSortPack(vecTotalPack, singlePack);
// 			}
// 			else
// 			{
// 				for (unsigned idx = 0; idx < vecTotalPack.size(); idx++)
// 				{
// 					singleDefinedPack tempPack = vecTotalPack[idx];
// 					if (tempPack.id_ == packID && (tempPack.act_time_.start_time == start_time || isClose))
// 					{
// 						vecTotalPack[idx] = singlePack;
// 						break;
// 					}
// 				}
// 			}
		}
		vector<playerDataPtr> onlinePlayers = player_mgr.onlinePlayer();
		vecTotalPack_ = vecTotalPack;
// 		for (vector<singleDefinedPack>::iterator itPack = vecTotalPack_.begin(); itPack != vecTotalPack_.end();)
// 		{
// 			vector<int>::iterator itID = find(vecTotalID.begin(), vecTotalID.end(), itPack->id_);
// 			if (itID == vecTotalID.end())
// 				itPack = vecTotalPack_.erase(itPack);
// 			else
// 				itPack++;
// 		}
		refreshPackList();
		save();
		updateToClients(onlinePlayers);
		Return(r, 0);
	}

	int act_gift_defined::getPilotNum(Json::Value actionJson)
	{
		if (!actionJson.isArray())
			return 2;

		int num = 0;
		for (unsigned i = 0; i < actionJson.size(); i++)
		{
			if (actionJson[i][email::actionID].asInt() == action_add_pilot)
			{
				num += actionJson[i][email::pilotActiveList].size();
			}
		}
		return num;
	}

	void act_gift_defined::giftDefinedUpdateReq( msg_json& m, Json::Value& r )
	{
		refreshPackList();
		Json::Value updateJson;
		package(updateJson[msgStr][1u]);
		r = updateJson;
		Return(r, 0);
	}

	void act_gift_defined::giftDefinedPickupGiftReq( msg_json& m, Json::Value& r )
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		refreshPackList();
		int packID = js_msg[0u].asInt();
		r[msgStr][1u] = packID;

		singleDefinedPack& curPack = getSinglePack(packID);
		if (curPack == singleDefinedPack::sNull_)
			Return(r, -1);

		if (!curPack.isValid())
			Return(r, 1);

		d->GiftDefined.refreshData(vecTotalPack_);
		singlePlayerPack &playerPack = d->GiftDefined.getAndUpdateSinglePack(curPack);

		if (playerPack.playerTodayReceiveTimes_ >= PERDAY_RECEIVE_TIMES)
			Return(r, 2);
		if (playerPack.playerTotalReceiveTimes_ >= curPack.totalReceiveTimes_)
			Return(r, 3);
		if (d->Base.getLevel() < curPack.levelLimit_)
			Return(r, 4);

		int vipLevel = d->Vip.getVipLevel();

		Json::Value simpleAction = curPack.giftDataList_[vipLevel].gift_;
		Json::Value actualAction = base2actual(simpleAction, d);
		Json::Value complexAction = gm_tools_mgr.simple2complex(actualAction);
		Json::Value Param;
		actionResult resAction = actionFormat::actionDo(d, complexAction, Param);

		if (resAction.resultCode != 0)
		{
			Return(r, 5);
//			email_sys.sendSystemEmailCommon(d, email_type_system_attachment, email_team_system_bag_full, Json::Value::null, simpleAction);
		}

		playerPack.playerTodayReceiveTimes_++;
		playerPack.playerTotalReceiveTimes_++;
		d->GiftDefined.save();

		//领取时间、奖励内容：奖励类型、奖励数量（科技点、荣誉、信用点、星币、补给、经验芯片、图纸碎片）/奖励ID（英雄、装备）/等级）
		string s = actualAction.toStyledString();
		StreamLog::Log(gift_defined_def::strMysqlLogGiftDefined, d, 
			(boost::lexical_cast<string,int>(na::time_helper::get_current_time())), 
			s, gift_defined_def::log_pickup_gift);

		vector<playerDataPtr> players;
		players.push_back(d);
		updateToClients(players);

		Return(r, 0);
	}

	Json::Value act_gift_defined::base2actual(Json::Value &baseAction, playerDataPtr player)
	{
		Json::Value actualAction;

		actualAction = Json::arrayValue;
		for (unsigned idx = 0; idx < baseAction.size(); idx++)
		{
			Json::Value &singleAction = actualAction[idx];
			singleAction = baseAction[idx];

			if (!singleAction.isMember(email::actionItemType))
				continue;

			if (singleAction[email::actionItemType].asInt() == 1)
			{
				if (singleAction.isMember(email::value))
				{
					singleAction[email::value] = singleAction[email::value].asInt() * player->Base.getLevel();
				}
				if (singleAction.isMember(ACTION::strValue))
				{
					singleAction[ACTION::strValue] = singleAction[ACTION::strValue].asInt() * player->Base.getLevel();
				}
			}
		}

		return actualAction;
	}

	void act_gift_defined::updateToClients(vector<playerDataPtr> players)
	{
		//发给客户端的，不需要进行时间转换
		for (unsigned playerIdx = 0; playerIdx < players.size(); playerIdx++)
		{
			playerDataPtr player = players[playerIdx];
			if (player == playerDataPtr())
				continue;
			player->GiftDefined.refreshData(vecTotalPack_);
			int vipLevel = player->Vip.getVipLevel();

			Json::Value singleJson, totalJson = Json::arrayValue, r;
			for (unsigned packIdx = 0; packIdx < vecTotalPack_.size(); packIdx++)
			{
				singleDefinedPack tempPack = vecTotalPack_[packIdx];
				singleJson[gift_defined_def::strPackID] = tempPack.id_;
				int startTime = tempPack.act_time_.start_time;
				int endTime = tempPack.act_time_.end_time;
				singleJson[act_base_def::str_act_start_time] = startTime;
				singleJson[act_base_def::str_act_end_time] = endTime;
				singleJson[gift_defined_def::strGiftDescript] = tempPack.giftDescript_;
				singleJson[gift_defined_def::strGiftName] = tempPack.giftName_;
				singleJson[gift_defined_def::strLevelLimit] = tempPack.levelLimit_;
				singlePlayerPack &playerPack = player->GiftDefined.getAndUpdateSinglePack(tempPack);
				singleJson[gift_defined_def::strMyTotalRemainReceiveTimes] = tempPack.totalReceiveTimes_ - playerPack.playerTotalReceiveTimes_;
				singleJson[gift_defined_def::strMyTodayRemainReceiveTimes] = PERDAY_RECEIVE_TIMES - playerPack.playerTodayReceiveTimes_;

				Json::Value actualAction = base2actual(tempPack.giftDataList_[vipLevel].gift_, player);
				singleJson[gift_defined_def::strMyGift] = actualAction;
				totalJson.append(singleJson);
			}
			r[msgStr][0u] = 0;
			r[msgStr][1] = totalJson;

			string s = r.toStyledString();
			na::msg::msg_json mj(player->playerID, player->netID, gate_client::gift_defined_player_info_update_resp, s);
			player_mgr.sendToPlayer(mj);
		}
	}

	void act_gift_defined::giftDefinedPlayerInfoUpdateReq( msg_json& m, Json::Value& r )
	{
		AsyncGetPlayerData(m);
		refreshPackList();
		vector<playerDataPtr> players;
		players.push_back(d);
		updateToClients(players);
	}

	void act_gift_defined::package(Json::Value& updateListJson)
	{
		updateListJson = Json::arrayValue;
		for (unsigned packIdx = 0; packIdx < vecTotalPack_.size(); packIdx++)
		{
			singleDefinedPack definedPack = vecTotalPack_[packIdx];
			Json::Value updateSingleJson;
			updateSingleJson[gift_defined_def::strPackID] = definedPack.id_;
			int startTime = definedPack.act_time_.start_time;
			int endTime = definedPack.act_time_.end_time;
			gm_tools_mgr.actTimePretreat(startTime, -1);
			gm_tools_mgr.actTimePretreat(endTime, -1);
			updateSingleJson[act_base_def::str_act_start_time] = startTime;
			updateSingleJson[act_base_def::str_act_end_time] = endTime;
			updateSingleJson[gift_defined_def::strLevelLimit] = definedPack.levelLimit_;
			updateSingleJson[gift_defined_def::strGiftDescript] = definedPack.giftDescript_;
			updateSingleJson[gift_defined_def::strGiftName] = definedPack.giftName_;
			updateSingleJson[gift_defined_def::strTotalReceiveTimes] = definedPack.totalReceiveTimes_;
			updateSingleJson[gift_defined_def::strPerDayReceiveTimes] = PERDAY_RECEIVE_TIMES;

			Json::Value &listJson = updateSingleJson[gift_defined_def::strGiftDataList];
			listJson = Json::arrayValue;
			for (map<int, giftDefinedData>::iterator it = definedPack.giftDataList_.begin(); it != definedPack.giftDataList_.end(); it++)
			{
				giftDefinedData& giftItem = it->second;
				Json::Value itemJson;
				itemJson[gift_defined_def::strVipLevel] = giftItem.vipLevel_;
				itemJson[gift_defined_def::strGift] = giftItem.gift_;
				listJson.append(itemJson);
			}
			updateListJson.append(updateSingleJson);
		}
	}

	void act_gift_defined::initData()
	{
		deleteOldData();
		mongo::BSONObj key = BSON(playerIDStr << 1);
		db_mgr.ensure_index(gift_defined_def::strActDbPlayerGiftDefined, key);
		get();
	}

	bool act_gift_defined::get()
	{
		mongo::BSONObj key = BSON(act_base_def::str_act_key << KEY_ACT_GIFT_DEFINED);
		mongo::BSONObj obj = db_mgr.FindOne(act_base_def::str_act_db_total, key);
		if(!obj.isEmpty())
		{
			checkNotEoo(obj[gift_defined_def::strGiftDefinedList])
			{
				vecTotalPack_.clear();
				for (unsigned packIdx = 0; packIdx < obj[gift_defined_def::strGiftDefinedList].Array().size(); packIdx++)
				{
					mongo::BSONElement singleObj = obj[gift_defined_def::strGiftDefinedList].Array()[packIdx];
					singleDefinedPack sdp;					
					checkNotEoo(singleObj[gift_defined_def::strPackID])
						sdp.id_ = singleObj[gift_defined_def::strPackID].Int();
					checkNotEoo(singleObj[act_base_def::str_act_start_time])
						sdp.act_time_.start_time = singleObj[act_base_def::str_act_start_time].Int();
					checkNotEoo(singleObj[act_base_def::str_act_end_time])
						sdp.act_time_.end_time = singleObj[act_base_def::str_act_end_time].Int();
					checkNotEoo(singleObj[gift_defined_def::strLevelLimit])
						sdp.levelLimit_ = singleObj[gift_defined_def::strLevelLimit].Int();
					checkNotEoo(singleObj[gift_defined_def::strTotalReceiveTimes])
						sdp.totalReceiveTimes_ = singleObj[gift_defined_def::strTotalReceiveTimes].Int();
					checkNotEoo(singleObj[gift_defined_def::strGiftName])
						sdp.giftName_ = singleObj[gift_defined_def::strGiftName].String();
					checkNotEoo(singleObj[gift_defined_def::strGiftDescript])
						sdp.giftDescript_ = singleObj[gift_defined_def::strGiftDescript].String();

					checkNotEoo(singleObj[gift_defined_def::strGiftDataList])
					{
						for (unsigned i = 0; i < singleObj[gift_defined_def::strGiftDataList].Array().size(); i++)
						{
							giftDefinedData gdData;
							mongo::BSONElement objEle = singleObj[gift_defined_def::strGiftDataList].Array()[i];
							checkNotEoo(objEle[gift_defined_def::strVipLevel])
								gdData.vipLevel_ = objEle[gift_defined_def::strVipLevel].Int();
							checkNotEoo(objEle[gift_defined_def::strGift])
							{
								if (objEle[gift_defined_def::strGift].type() == mongo::Array)
								{
									for (unsigned j = 0; j < objEle[gift_defined_def::strGift].Array().size(); j++)
									{
										Json::Reader reader;
										Json::Value rwdJson;
										reader.parse(objEle[gift_defined_def::strGift].Array()[j].Obj().jsonString(), rwdJson);
										gdData.gift_.append(rwdJson);
									}
								}
							}
							sdp.giftDataList_[gdData.vipLevel_] = gdData;
						}
					}
					insertSortPack(vecTotalPack_, sdp);
				}
			}
			checkNotEoo(obj[gift_defined_def::strCurrentID])
			{
				currentPackID_ = obj[gift_defined_def::strCurrentID].Int();
			}
			refreshPackList();
		}

		return false;
	}

	bool act_gift_defined::save()
	{
		mongo::BSONObj key = BSON(act_base_def::str_act_key << KEY_ACT_GIFT_DEFINED);
		mongo::BSONObjBuilder obj;
		mongo::BSONArrayBuilder arrBuider;

		obj << act_base_def::str_act_key << KEY_ACT_GIFT_DEFINED;
		obj << gift_defined_def::strCurrentID << currentPackID_;

		for (unsigned packIdx = 0; packIdx < vecTotalPack_.size(); packIdx++)
		{
			singleDefinedPack sdp = vecTotalPack_[packIdx];
			mongo::BSONObjBuilder obj, singleObj;

			singleObj << gift_defined_def::strPackID << sdp.id_;
			singleObj << act_base_def::str_act_start_time << sdp.act_time_.start_time;
			singleObj << act_base_def::str_act_end_time << sdp.act_time_.end_time;
			singleObj << gift_defined_def::strLevelLimit << sdp.levelLimit_;
			singleObj << gift_defined_def::strTotalReceiveTimes << sdp.totalReceiveTimes_;
			singleObj << gift_defined_def::strGiftName << sdp.giftName_;
			singleObj << gift_defined_def::strGiftDescript << sdp.giftDescript_;

			mongo::BSONArrayBuilder arrGift;
			for (map<int, giftDefinedData>::iterator it = sdp.giftDataList_.begin(); it != sdp.giftDataList_.end(); it++)
			{
				giftDefinedData gdData = it->second;
				mongo::BSONObjBuilder objItem;
				objItem << gift_defined_def::strVipLevel << gdData.vipLevel_;

				mongo::BSONArrayBuilder act_arr;
				for (unsigned idx_at = 0; idx_at < gdData.gift_.size(); idx_at++)
				{
// 					string tmp = gdData.gift_[idx_at].toStyledString();
// 					tmp = commom_sys.tighten(tmp);
					string tmp = commom_sys.json2string(gdData.gift_[idx_at]);
					mongo::BSONObj obj_at = mongo::fromjson(tmp);
					act_arr << obj_at;
				}
				objItem << gift_defined_def::strGift << act_arr.arr();
				arrGift << objItem.obj();
			}

			singleObj << gift_defined_def::strGiftDataList << arrGift.arr();
			arrBuider << singleObj.obj();
		}
		obj << gift_defined_def::strGiftDefinedList << arrBuider.arr();

		return db_mgr.save_mongo(act_base_def::str_act_db_total, key, obj.obj());
	}

	void act_gift_defined::deleteOldData()
	{
		mongo::BSONObj key = BSON(act_base_def::str_act_key << KEY_ACT_GIFT_DEFINED);
		mongo::BSONObj obj = db_mgr.FindOne(act_base_def::str_act_db_total, key);

		bool isDelete = false;
		if (!obj.isEmpty())
		{
			checkNotEoo(obj[act_base_def::str_act_start_time])
				isDelete = true;
			checkNotEoo(obj[act_base_def::str_act_end_time])
				isDelete = true;
		}
		if (isDelete)
		{
			db_mgr.remove_collection(act_base_def::str_act_db_total, key);
			db_mgr.drop_the_collection(gift_defined_def::strActDbPlayerGiftDefined);
		}
	}

	int act_gift_defined::getNewID(vector<singleDefinedPack> vecTotalPack)
	{
		if (currentPackID_ > MAX_ACT_DEFINED_ID)
		{
			currentPackID_ = 1;
		}
		bool isFind = false;
		do 
		{
			isFind = false;
			for (unsigned i = 0; i < vecTotalPack.size(); i++)
			{
				singleDefinedPack defPack = vecTotalPack[i];
				if (currentPackID_ == defPack.id_)
				{
					isFind = true;
					currentPackID_++;
					break;
				}
			}
		} while (isFind);
		return currentPackID_;
	}

	singleDefinedPack& act_gift_defined::getSinglePack(int packID)
	{
		for (unsigned i = 0; i < vecTotalPack_.size(); i++)
		{
			singleDefinedPack& single = vecTotalPack_[i];
			if (single.id_ == packID)
				return single;
		}

		return singleDefinedPack::sNull_;
	}

	bool packSort(singleDefinedPack sPack1, singleDefinedPack sPack2)
	{
		return sPack1.act_time_.start_time > sPack2.act_time_.start_time;
	}

	void act_gift_defined::insertSortPack(vector<singleDefinedPack> &vecTotalPack, singleDefinedPack singlePack)
	{
		vecTotalPack.push_back(singlePack);
		sort(vecTotalPack.begin(), vecTotalPack.end(), packSort);
	}

	void act_gift_defined::refreshPackList()
	{
		for (vector<singleDefinedPack>::iterator it = vecTotalPack_.begin(); it != vecTotalPack_.end();)
		{
			if (it->isOverdue())
				it = vecTotalPack_.erase(it);
			else
				it++;
		}
	}

	singleDefinedPack::singleDefinedPack()
	{
		id_ = -2;
		levelLimit_ = 0;
		totalReceiveTimes_ = 0;
		giftDescript_ = "";
		giftName_ = "";
		act_time_.start_time = 0;
		act_time_.end_time = 0;
	}

	bool singleDefinedPack::operator==(singleDefinedPack& singlePack)
	{
		return singlePack.id_ == id_;
	}

	bool singleDefinedPack::isValid()
	{
		int curTime = na::time_helper::get_current_time();
		if (curTime >= act_time_.start_time && curTime <= act_time_.end_time)
			return true;

		return false;
	}

	bool singleDefinedPack::isOverdue()
	{
		int curTime = na::time_helper::get_current_time();
		if (curTime > act_time_.end_time)
			return true;

		return false;
	}

}

