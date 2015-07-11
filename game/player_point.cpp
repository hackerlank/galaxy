#include "player_point.h"
#include "activity_point.h"
#include "action_format.hpp"
#include "record_system.h"
#include "gm_tools.h"
#include "email_system.h"

namespace gg
{
#define INFINITE_NUM 10000
	playerPoint::playerPoint(playerData& own) : Block(own)
	{
		startTime_ = 0;
		myPoints_ = 0;
	}

	void playerPoint::autoUpdate()
	{
		refreshData();
		Json::Value msg;
		package(msg);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::point_player_info_update_resp, msg);
	}

	bool playerPoint::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(point_def::strActDbPlayerPoint, key);

		if (!obj.isEmpty())
		{
			checkNotEoo(obj[act_base_def::str_act_start_time])
				startTime_ = obj[act_base_def::str_act_start_time].Int();
			checkNotEoo(obj[point_def::strMyPoint])
				myPoints_ = obj[point_def::strMyPoint].Int();
			
			checkNotEoo(obj[point_def::strReceiveTimesList])
			{
				vecReceiveTimes_.clear();
				for (unsigned i = 0; i < obj[point_def::strReceiveTimesList].Array().size(); i++)
				{
					vecReceiveTimes_.push_back(obj[point_def::strReceiveTimesList].Array()[i].Int());
				}
			}
			return true;
		}

		return false;
	}

	bool playerPoint::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;

		obj << playerIDStr << own.playerID;
		obj << act_base_def::str_act_start_time << startTime_;
		obj << point_def::strMyPoint << myPoints_;

		mongo::BSONArrayBuilder timeArr;
		for (unsigned i = 0; i < vecReceiveTimes_.size(); i++)
		{
			timeArr << vecReceiveTimes_[i];
		}
		obj << point_def::strReceiveTimesList << timeArr.arr();

		db_mgr.save_mongo(point_def::strActDbPlayerPoint, key, obj.obj());
		return true;
	}

	void playerPoint::package(Json::Value& pack)
	{
		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();
//		pack[msgStr][1u][act_base_def::str_act_start_time] = startTime_;
		pack[msgStr][1u][point_def::strMyPoint] = myPoints_;

		pack[msgStr][1u][point_def::strReceiveTimesList] = Json::arrayValue;
		for (unsigned i = 0; i < vecReceiveTimes_.size(); i++)
		{
			pack[msgStr][1u][point_def::strReceiveTimesList].append(vecReceiveTimes_[i]);
		}
	}

	int playerPoint::exchangeGift(int exType, int exIndex, Json::Value &failArr)
	{
		refreshData();

		pointData ptData = point_sys.getPointData(exIndex);

		int exTimes = 1;
		if (exType == 1)//兑换10次
			exTimes = 10;

		if (ptData.canExchangeTimes_ < INFINITE_NUM && vecReceiveTimes_[exIndex] + exTimes > ptData.canExchangeTimes_)
		{//次数超标
			return 2;
		}

		if (myPoints_ < ptData.condition_ * exTimes)
		{
			return 3;
		}
		
		Json::Value complexAction = gm_tools_mgr.simple2complex(ptData.reward_);
		for (unsigned i = 0; i < complexAction.size(); i++)
		{
			Json::Value itemJson = complexAction[i];
			int aid = itemJson[ACTION::strActionID].asInt();
			if (aid == action_add_pilot)
			{
				for (unsigned j = 0; j < itemJson[ACTION::strPilotActiveList].size(); j++)
				{
					int pilotID = itemJson[ACTION::strPilotActiveList][j].asInt();
					if (own.Pilots.checkPilotEnlist(pilotID) || own.Pilots.checkPilotActive(pilotID))
					{
						return 5;
					}
				}
			}
			if (aid == action_add_weiwang && own.Base.getSphereID() == -1)
			{
				return 6;
			}
		}

		Json::Value Param;
		int curTimes = 0;
		bool isfailed = false;
		failArr = Json::arrayValue;
		do 
		{
			curTimes++;
			vector<actionResult> vecRetCode = actionFormat::actionDoJump(own.shared_from_this(), complexAction, Param);
			Json::Value failSimpleJson;
			isfailed = email_sys.sendDoJumpFailEmail(own.shared_from_this(), complexAction, vecRetCode, email_team_system_charge_point, failSimpleJson) ? true : isfailed;
			if (!failSimpleJson.empty())
			{
				failArr.append(failSimpleJson);
			}
		} while (curTimes < exTimes);

		int ret = 0;
		if (isfailed)
			ret = 4;

		alterMyPoint(-ptData.condition_ * exTimes);
		vecReceiveTimes_[exIndex] += exTimes;
		helper_mgr.insertSaveAndUpdate(this);

		//记录类型、发生时间、领取奖励、角色ID
// 		string s = ptData.reward_.toStyledString();
// 		s = commom_sys.tighten(s);
		string s = commom_sys.json2string(ptData.reward_);
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(ptData.condition_));
		fields.push_back(ptData.packName_);
		fields.push_back("");
		fields.push_back("");
		fields.push_back("");
		fields.push_back(s);

		for (int i = 0; i < exTimes; i++)
		{
			StreamLog::Log(point_def::strMysqlChargePointLog, own.shared_from_this(),
				(boost::lexical_cast<string, int>(na::time_helper::get_current_time())),
				"", fields,point_def::log_get_gift);
		}

		return ret;
	}

	bool playerPoint::refreshData()
	{
		act_time_info act_time = point_sys.getActTime();

		if (act_time.start_time != startTime_)
		{
			startTime_ = act_time.start_time;
			myPoints_ = 0;
			vecReceiveTimes_.clear();
			vecReceiveTimes_.resize(point_sys.getPointDataSize());
			for (unsigned i = 0; i < vecReceiveTimes_.size(); i++)
			{
				vecReceiveTimes_[i] = 0;
			}
			helper_mgr.insertSaveAndUpdate(this);
			return true;
		}

		if (vecReceiveTimes_.size() != point_sys.getPointDataSize())
		{//针对修改礼包个数的情况作特殊处理
			vector<int> tempTimes = vecReceiveTimes_;
			vecReceiveTimes_.clear();
			vecReceiveTimes_.resize(point_sys.getPointDataSize());
			for (unsigned i = 0; i < vecReceiveTimes_.size(); i++)
			{
				if (i < tempTimes.size())
				{
					vecReceiveTimes_[i] = tempTimes[i];
				}
				else
				{
					vecReceiveTimes_[i] = 0;
				}
			}
			return true;
		}
		return false;
	}

	void playerPoint::pointRecharge(int amount )
	{
		refreshData();
		alterMyPoint(amount);
		helper_mgr.insertSaveAndUpdate(this);
		helper_mgr.runSaveAndUpdate();

		vector<string> fds;
		fds.push_back(boost::lexical_cast<string, int>(myPoints_));
		StreamLog::Log(point_def::strMysqlChargePointLog, own.shared_from_this(), 
			(boost::lexical_cast<string,int>(na::time_helper::get_current_time())), 
			boost::lexical_cast<string, int>(amount), fds, point_def::log_charge);
	}

	void playerPoint::alterMyPoint(int num)
	{
		refreshData();
		int tmp = myPoints_;
		myPoints_ += num;
		myPoints_ = myPoints_ < 0 ? 0 : myPoints_;
		vector<string> fds;
		fds.push_back(boost::lexical_cast<string, int>(na::time_helper::get_current_time()));
		fds.push_back(boost::lexical_cast<string, int>(num));
		StreamLog::Log(point_def::strMysqlChargePointLog, own.getOwnDataPtr(), 
			boost::lexical_cast<string, int>(tmp),
			boost::lexical_cast<string, int>(myPoints_), point_def::log_point);
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerPoint::getMyPoint()
	{
		refreshData();
		return myPoints_;
	}

}


