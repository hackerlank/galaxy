#include "activity_point.h"
#include "response_def.h"
#include "playerManager.h"
#include "gm_tools.h"

namespace gg
{
	act_point* const act_point::pointSys = new act_point();

	act_point::act_point()
	{
		set_to_default();
	}

	act_point::~act_point()
	{
	}

	void act_point::initData()
	{
		mongo::BSONObj key = BSON(playerIDStr << 1);
		db_mgr.ensure_index(point_def::strActDbPlayerPoint, key);
		get();
	}

	void act_point::package(Json::Value& update_json)
	{
		activity_base::package(update_json);

		int receiveDueTime = receiveDueTime_;
		if (gm_tools_mgr.isGmProtocal(State::getState()))
		{
			gm_tools_mgr.actTimePretreat(receiveDueTime,-1);
		}
		update_json[point_def::strReceiveDueTime] = receiveDueTime;

		update_json[point_def::strPointDataList] = Json::arrayValue;
		for (unsigned i = 0; i < vecPointData_.size(); i++)
		{
			pointData pd = vecPointData_[i];
			Json::Value dataJson;
			dataJson[point_def::strPackName] = pd.packName_;
			dataJson[point_def::strPackType] = pd.type_;
			dataJson[point_def::strPictureID] = pd.pictureID_;
			dataJson[point_def::strCondition] = pd.condition_;
			dataJson[point_def::strCanExchangeTimes] = pd.canExchangeTimes_;
			dataJson[point_def::strRewardAction] = pd.reward_;

			update_json[point_def::strPointDataList].append(dataJson);
		}
	}

	int act_point::modify(Json::Value& modify_json)
	{
		return 0;
	}

	void act_point::set_to_default()
	{
		activity_base::set_to_default();
		receiveDueTime_ = 0;
		vecPointData_.clear();
	}
	
	bool act_point::get()
	{
		mongo::BSONObj key = BSON(act_base_def::str_act_key << KEY_ACT_POINT);
		mongo::BSONObj obj = db_mgr.FindOne(act_base_def::str_act_db_total, key);
		if(!obj.isEmpty())
		{
			superGet(obj);
			checkNotEoo(obj[point_def::strReceiveDueTime])
				receiveDueTime_ = obj[point_def::strReceiveDueTime].Int();
			checkNotEoo(obj[point_def::strPointDataList])
			{
				vecPointData_.clear();
				for (unsigned i = 0; i < obj[point_def::strPointDataList].Array().size(); i++)
				{
					pointData pd;
					mongo::BSONElement objEle = obj[point_def::strPointDataList].Array()[i];
					checkNotEoo(objEle[point_def::strPackName])
						pd.packName_ = objEle[point_def::strPackName].String();
					checkNotEoo(objEle[point_def::strPackType])
						pd.type_ = objEle[point_def::strPackType].Int();
					checkNotEoo(objEle[point_def::strPictureID])
						pd.pictureID_ = objEle[point_def::strPictureID].Int();
					checkNotEoo(objEle[point_def::strCondition])
						pd.condition_ = objEle[point_def::strCondition].Int();
					checkNotEoo(objEle[point_def::strCanExchangeTimes])
						pd.canExchangeTimes_ = objEle[point_def::strCanExchangeTimes].Int();

					checkNotEoo(objEle[point_def::strRewardAction])
					{
						if (objEle[point_def::strRewardAction].type() == mongo::Array)
						{
							for (unsigned i = 0; i < objEle[point_def::strRewardAction].Array().size(); i++)
							{
								Json::Reader reader;
								Json::Value rwdJson;
								reader.parse(objEle[point_def::strRewardAction].Array()[i].Obj().jsonString(), rwdJson);
								pd.reward_.append(rwdJson);
							}
						}
					}
					vecPointData_.push_back(pd);
				}
			}
			return true;
		}
		return false;
	}

	bool act_point::save()
	{
		mongo::BSONObj key = BSON(act_base_def::str_act_key << KEY_ACT_POINT);
		mongo::BSONObjBuilder obj;

		superSave(obj);
		obj << act_base_def::str_act_key << KEY_ACT_POINT;
		obj << point_def::strReceiveDueTime << receiveDueTime_;
		
		mongo::BSONArrayBuilder arrBuider;
		for (unsigned i = 0; i < vecPointData_.size(); i++)
		{
			pointData pd = vecPointData_[i];
			mongo::BSONObjBuilder objItem;
			objItem << point_def::strPackName << pd.packName_;
			objItem << point_def::strPackType << pd.type_;
			objItem << point_def::strPictureID << pd.pictureID_;
			objItem << point_def::strCondition << pd.condition_;
			objItem << point_def::strCanExchangeTimes << pd.canExchangeTimes_;

			mongo::BSONArrayBuilder act_arr;
			for (unsigned idx_at = 0; idx_at < pd.reward_.size(); idx_at++)
			{
// 				string tmp = pd.reward_[idx_at].toStyledString();
// 				tmp = commom_sys.tighten(tmp);
				string tmp = commom_sys.json2string(pd.reward_[idx_at]);
				mongo::BSONObj obj_at = mongo::fromjson(tmp);
				act_arr << obj_at;
			}
			objItem << point_def::strRewardAction << act_arr.arr();

			arrBuider << objItem.obj();
		}

		obj << point_def::strPointDataList << arrBuider.arr();

		return db_mgr.save_mongo(act_base_def::str_act_db_total, key, obj.obj());
	}

	void act_point::gmActivityPointModifyReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;

		int start_time = js_msg[1][act_base_def::str_act_start_time].asInt();
		int end_time = js_msg[1][act_base_def::str_act_end_time].asInt();

		gm_tools_mgr.actTimePretreat(start_time);
		gm_tools_mgr.actTimePretreat(end_time);
		int ret = activity_base::check(start_time, end_time);

		if (ret == 1 && start_time == act_time_.start_time && end_time == act_time_.end_time)
		{
			ret = 0;
		}

		if (ret != 0)
		{
			Return(r, ret);
		}

		int receiveEndTime = js_msg[1][point_def::strReceiveDueTime].asInt();
		gm_tools_mgr.actTimePretreat(receiveEndTime);

		if (start_time != 0 && receiveEndTime <= end_time)
		{
			Return(r, 4);
		}

		{
			int limitNum = 0, repeatNum = 0;;
			for (unsigned i = 0; i < js_msg[1][point_def::strPointDataList].size(); i++)
			{
				Json::Value tempJson = js_msg[1][point_def::strPointDataList][i];
				pointData ptdata;
				ptdata.packName_ = tempJson[point_def::strPackName].asString();
				ptdata.type_ = tempJson[point_def::strPackType].asInt();
				ptdata.pictureID_ = tempJson[point_def::strPictureID].asInt();
				ptdata.canExchangeTimes_ = tempJson[point_def::strCanExchangeTimes].asInt();
				ptdata.reward_ = tempJson[point_def::strRewardAction];
				if (ptdata.type_ == 0)
				{
					if (ptdata.reward_.size() == 0 || ptdata.reward_.size() > 4)
						Return(r, 6);
					if (ptdata.canExchangeTimes_ < 1 || ptdata.canExchangeTimes_ > 10)
						Return(r, 7);
					limitNum++;
				}
				else if (ptdata.type_ == 1)
				{
					if (ptdata.reward_.size() != 1)
						Return(r, 8);
					if (ptdata.canExchangeTimes_ <= 10)
						Return(r, 9);
					repeatNum++;
				}
				else
				{
					Return(r, 5);
				}
			}

			if (limitNum > 20 || repeatNum > 20)
				Return(r, 10);
		}

		vecPointData_.clear();
		for (unsigned i = 0; i < js_msg[1][point_def::strPointDataList].size(); i++)
		{
			Json::Value tempJson = js_msg[1][point_def::strPointDataList][i];
			pointData ptdata;
			ptdata.packName_ = tempJson[point_def::strPackName].asString();
			ptdata.type_ = tempJson[point_def::strPackType].asInt();
			ptdata.pictureID_ = tempJson[point_def::strPictureID].asInt();
			ptdata.condition_ = tempJson[point_def::strCondition].asInt();
			ptdata.canExchangeTimes_ = tempJson[point_def::strCanExchangeTimes].asInt();
			ptdata.reward_ = tempJson[point_def::strRewardAction];
			vecPointData_.push_back(ptdata);
		}
		receiveDueTime_ = receiveEndTime;
		act_time_.start_time = start_time;
		act_time_.end_time = end_time;
		save();
		vector<playerDataPtr> onlinePlayers = player_mgr.onlinePlayer();
		update2clients(onlinePlayers);
		Return(r, 0);
	}

	void act_point::pointUpdateReq(msg_json& m, Json::Value& r)
	{
		Json::Value updateJson;
		package(updateJson[msgStr][1u]);
		r = updateJson;
		Return(r, 0);
	}

	void act_point::update2clients(vector<playerDataPtr> players)
	{
		for (vector<playerDataPtr>::iterator it = players.begin(); it != players.end(); it++)
		{
			update2client(*it);
			(*it)->Points.autoUpdate();
		}
	}

	void act_point::update2client(playerDataPtr player)
	{
		Json::Value updateJson;
		package(updateJson[msgStr][1u]);
		updateJson[msgStr][0u] = 0;

		string s = updateJson.toStyledString();
		na::msg::msg_json mj(player->playerID, player->netID, gate_client::point_update_resp, s);
		player_mgr.sendToPlayer(mj);
	}

	void act_point::pointExchangeReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int exType = js_msg[0u].asInt();
		int exIdx = js_msg[1u].asInt();

		if (!isActivityEnd())
		{//非活动时间
			Return(r, 1);
		}
		
		if (exIdx < 0 || exIdx >= int(vecPointData_.size()) || (exType != 0 && exType != 1))
		{
			Return(r, -1);
		}

		Json::Value failArr;
		int ret = d->Points.exchangeGift(exType, exIdx, failArr);

		r[msgStr][1u] = exType;
		r[msgStr][2u] = failArr;
		Return(r, ret);
	}

	void act_point::pointPlayerInfoUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Points.autoUpdate();
	}

	pointData act_point::getPointData(int exIndex)
	{
		return vecPointData_[exIndex];
	}

	void act_point::pointRecharge(playerDataPtr pdata, int amount)
	{
		if (!is_valid())
		{
			return;
		}

		pdata->Points.pointRecharge(amount);
		// 		pdata->Rebate.refreshData(act_time_);
		// 		pdata->Rebate.setRechargeAmount(pdata->Rebate.getRechargeAmount() + amount);
		// 		helper_mgr.insertSaveSet(&pdata->Rebate);
	}

	bool act_point::isActivityEnd()
	{
		int cur_time = int(na::time_helper::get_current_time());
		if (cur_time >= act_time_.start_time && cur_time <= receiveDueTime_)
		{
			return true;
		}
		return false;
	}


	pointData::pointData()
	{
		packName_ = "";
		pictureID_ = 0;
		condition_ = 0;
		canExchangeTimes_ = 0;
		reward_ = Json::arrayValue;
		type_ = 0;
	}
}

