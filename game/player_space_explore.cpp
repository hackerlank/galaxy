#include "player_space_explore.h"
#include "helper.h"
#include "chat_system.h"
#include "action_def.h"
#include "action_format.hpp"
#include "email_system.h"
#include "record_system.h"
#include "gm_tools.h"
#include "item_system.h"
#include "params.hpp"
#include "activity_system.h"
#include "activity_game_param.h"

namespace gg
{
#define FIVE_TIMES_CRITICAL_VIP 7

	playerSpaceExplore::playerSpaceExplore(playerData& own) : Block(own)
	{
		todayOneHuntNotFreeUsedTimes_ = 0;
		totalTenNotFreeHuntsUsedTimes_ = 0;
		nextFreeOneHuntTime_ = 0;
		nextFreeTenHuntsTime_ = 0;
		nextDailyFreshTime_ = 0;
	}

	void playerSpaceExplore::autoUpdate()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (nextFreeTenHuntsTime_ == 0)
		{
//			nextFreeTenHuntsTime_ = cur_time + 86400 * 3;
			save();
		}

		Json::Value pack;
		package(pack);

		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::space_explore_player_info_resp, pack);
	}

	bool playerSpaceExplore::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(space_explore_def::strDbPlayerSpaceExplore, key);

		if (!obj.isEmpty())
		{
			checkNotEoo(obj[space_explore_def::strTodayOneHuntUsedTimes])
				todayOneHuntNotFreeUsedTimes_ = obj[space_explore_def::strTodayOneHuntUsedTimes].Int();
			checkNotEoo(obj[space_explore_def::strTotalTenHuntsUsedTimes])
				totalTenNotFreeHuntsUsedTimes_ = obj[space_explore_def::strTotalTenHuntsUsedTimes].Int();
			checkNotEoo(obj[space_explore_def::strNextFreeOneHuntTime])
				nextFreeOneHuntTime_ = obj[space_explore_def::strNextFreeOneHuntTime].Int();
			checkNotEoo(obj[space_explore_def::strNextFreeTenHuntsTime])
				nextFreeTenHuntsTime_ = obj[space_explore_def::strNextFreeTenHuntsTime].Int();
			checkNotEoo(obj[space_explore_def::strNextDailyFreshTime])
				nextDailyFreshTime_ = obj[space_explore_def::strNextDailyFreshTime].Int();

			return true;
		}

		return false;
	}

	bool playerSpaceExplore::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;

		obj << playerIDStr << own.playerID;
		obj << space_explore_def::strTodayOneHuntUsedTimes << todayOneHuntNotFreeUsedTimes_;
		obj << space_explore_def::strTotalTenHuntsUsedTimes << totalTenNotFreeHuntsUsedTimes_;
		obj << space_explore_def::strNextFreeOneHuntTime << nextFreeOneHuntTime_;
		obj << space_explore_def::strNextFreeTenHuntsTime << nextFreeTenHuntsTime_;
		obj << space_explore_def::strNextDailyFreshTime << nextDailyFreshTime_;

		return db_mgr.save_mongo(space_explore_def::strDbPlayerSpaceExplore, key, obj.obj());
	}

	void playerSpaceExplore::package(Json::Value& pack)
	{
		refreshData();
		pack[msgStr][0u] = 0;
//		pack[msgStr][1u][updateFromStr] = State::getState();

		pack[msgStr][1u][space_explore_def::strIsOpenSpaceExplore] = true;
		pack[msgStr][1u][space_explore_def::strNextFreeOneHuntTime] = nextFreeOneHuntTime_;
		pack[msgStr][1u][space_explore_def::strNextFreeTenHuntsTime] = nextFreeTenHuntsTime_;
		pack[msgStr][1u][space_explore_def::strIsFirstOne] = (nextFreeOneHuntTime_ == 0);
		pack[msgStr][1u][space_explore_def::strIsFirstTen] = (totalTenNotFreeHuntsUsedTimes_ == 0);

		bool isRedPoint = false;
		int cur_time = na::time_helper::get_current_time();
		if (nextFreeTenHuntsTime_ == 0)
		{
//			nextFreeTenHuntsTime_ = cur_time + 86400 * 3;
		}
		if (/*nextFreeOneHuntTime_ != 0 &&*/ cur_time >= nextFreeOneHuntTime_)
		{
			isRedPoint = true;
		}
// 		if (own.Vip.getVipLevel() >= 1 && nextFreeTenHuntsTime_ != 0 && cur_time >= nextFreeTenHuntsTime_)
// 		{
// 			isRedPoint = true;
// 		}

		pack[msgStr][1u][space_explore_def::strIsRedPoint] = isRedPoint;
	}

	int playerSpaceExplore::exploreOne(rewardResult& rr, Json::Value &failSimpleJson)
	{
		refreshData();
		int cur_time = na::time_helper::get_current_time();

		bool isFreeHunt = false;
		if (/*nextFreeOneHuntTime_ != 0 && */cur_time >= nextFreeOneHuntTime_)
			isFreeHunt = true;

		int beforeGold = own.Base.getAllGold();
		spaceExploreConfig thConfig = space_explore_sys.getConfig();
		if (isFreeHunt)
		{
			nextFreeOneHuntTime_ = cur_time + 86400;
		}
		else
		{
			if (own.Vip.getVipLevel() == 0 && own.Base.getLevel() < 35 && todayOneHuntNotFreeUsedTimes_>= 2)
			{
				return 1;
			}

			if (own.Base.getAllGold() < thConfig.oneHuntGoldCost_)
			{
				return 2;
			}
			else
			{
				todayOneHuntNotFreeUsedTimes_++;
				own.Base.alterBothGold(-thConfig.oneHuntGoldCost_);

				if (nextFreeOneHuntTime_ == 0)
					nextFreeOneHuntTime_ = cur_time + 86400;
			}
		}
		
		exploreRandom(rr);
		helper_mgr.insertSaveAndUpdate(this);


		//寻宝记录
		bool isUpdate = false;
		if (rr.rewardTimes_ > 1 && rr.rewardIndex_ == 0)
		{
			spaceRecord sr;
			sr.playerID_ = own.playerID;
			sr.reward_ = rr;
			space_explore_sys.insertRecord(sr);
			isUpdate = true;
		}

		if (rr.rewardIndex_ > 0)
		{
			Params::ObjectValue obj = item_sys.getConfig(thConfig.equipIDList_[rr.rewardIndex_ - 1]);
			int quality = obj[ItemDefine::itemQualityStr].asInt();
			if (quality >= 4)
			{
				spaceRecord sr;
				sr.playerID_ = own.playerID;
				sr.reward_ = rr;
				space_explore_sys.insertRecord(sr);
				isUpdate = true;
			}
		}

		if (isUpdate)
		{
			space_explore_sys.save();
			space_explore_sys.updateRecord(own.shared_from_this());
		}

		//邮件
		int ret = 0;
		Json::Value actionArr = Json::arrayValue;
		Json::Value Param;
		if (rr.rewardIndex_ == 0)
		{
			Json::Value actionItem;
			Json::Value Param;
			actionItem[ACTION::strActionID] = action_add_silver;
			actionItem[ACTION::strValue] = thConfig.silverBase_ * rr.rewardTimes_;
			actionArr.append(actionItem);
			actionFormat::actionDoJump(own.shared_from_this(), actionArr, Param);
		}
		else
		{
			Json::Value actionItem;
			actionItem[ACTION::strActionID] = action_add_item;
			actionItem[ACTION::addItemIDStr] = thConfig.equipIDList_[rr.rewardIndex_-1];
			actionItem[ACTION::addNumStr] = 1;
			actionArr.append(actionItem);
//			cout << __FUNCTION__ << ",actionArr:" << actionArr.toStyledString() << endl;
			vector<actionResult> vecRetCode = actionFormat::actionDoJump(own.shared_from_this(), actionArr, Param);
			bool hasSend = email_sys.sendDoJumpFailEmail(own.shared_from_this(), actionArr, vecRetCode, email_team_system_space_explore, failSimpleJson);
			if (hasSend)
				ret = 3;
		}
		
//单次寻宝流水：玩家名称、ID、VIP等级、操作时间、消耗钻石数、消耗前钻数、消耗后钻石数、是否暴击（否/2倍/5倍）、获得奖励。
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(beforeGold - own.Base.getAllGold())); //f1
		fields.push_back(boost::lexical_cast<string, int>(own.Vip.getVipLevel())); //f2
		fields.push_back(boost::lexical_cast<string, int>(rr.rewardTimes_)); //f3
		string s;
		s = boost::lexical_cast<string, int>(na::time_helper::get_current_time());
		fields.push_back(s); //f4
		fields.push_back(""); //f5
		Json::Value simpleAction = gm_tools_mgr.complex2simple(actionArr);
// 		s = simpleAction.toStyledString();
// 		s = commom_sys.tighten(s);
		s = commom_sys.json2string(simpleAction);
		fields.push_back(s); //f6
		StreamLog::Log(space_explore_def::strDbsSpaceExplore, own.shared_from_this(),
			boost::lexical_cast<string, int>(beforeGold),
			boost::lexical_cast<string, int>(own.Base.getAllGold()), fields, space_explore_def::log_one_explore);
		activity_sys.updateActivity(own.getOwnDataPtr(), 0, activity::_explore);
		return ret;
	}

	int playerSpaceExplore::exploreTen(vector<rewardResult>& rlist, Json::Value &failSimpleJson)
	{
		refreshData();
		spaceExploreConfig thConfig = space_explore_sys.getConfig();

		int cur_time = na::time_helper::get_current_time();
		bool isFreeHunt = false;
// 		if (cur_time >= nextFreeTenHuntsTime_)
// 			isFreeHunt = true;//去掉十次免费

		if (own.Vip.getVipLevel() < 1)
		{
			return 1;
		}
		
		int beforeGold = own.Base.getAllGold();
		if (isFreeHunt)
		{
//			nextFreeTenHuntsTime_ = cur_time + 86400 * 3;
		}
		else
		{
			int costGold = thConfig.tenHuntsGoldCost_;
			costGold = (int)ceil(costGold*(1 + activity_sys.getRate(param_space_ten_explore_discount, own.getOwnDataPtr())));
			if (own.Base.getAllGold() < costGold)
				return 2;
			else
				own.Base.alterBothGold(-costGold);
		}

		for (unsigned i = 0; i < 10; i++)
		{
			rewardResult rr;
			exploreRandom(rr);
			rlist.push_back(rr);
		}
		helper_mgr.insertSaveAndUpdate(this);

		if (!isFreeHunt && totalTenNotFreeHuntsUsedTimes_ == 0)
		{
			rlist[0].rewardIndex_ = thConfig.firstTenExploreEquipIndex_ + 1;
			rlist[0].rewardTimes_ = 1;
		}

		if (!isFreeHunt)
		{
			totalTenNotFreeHuntsUsedTimes_++;
		}

		//寻宝记录
		bool isUpdate = false;
		for (unsigned i = 0; i < rlist.size(); i++)
		{
			rewardResult rr = rlist[i];
			if (rr.rewardTimes_ > 1 && rr.rewardIndex_ == 0)
			{
				spaceRecord sr;
				sr.playerID_ = own.playerID;
				sr.reward_ = rr;
				space_explore_sys.insertRecord(sr);
				isUpdate = true;
			}

			if (rr.rewardIndex_ > 0)
			{
				Params::ObjectValue obj = item_sys.getConfig(thConfig.equipIDList_[rr.rewardIndex_ - 1]);
				int quality = obj[ItemDefine::itemQualityStr].asInt();
				if (quality >= 4)
				{
					spaceRecord sr;
					sr.playerID_ = own.playerID;
					sr.reward_ = rr;
					space_explore_sys.insertRecord(sr);
					isUpdate = true;
				}
			}
		}

		if (isUpdate)
		{
			space_explore_sys.save();
			space_explore_sys.updateRecord(own.shared_from_this());
		}

		//邮件
		Json::Value actionArr = Json::arrayValue;
		Json::Value Param;
		for (unsigned i = 0; i < rlist.size(); i++)
		{
			rewardResult rr = rlist[i];
			if (rr.rewardIndex_ == 0)
			{
				Json::Value actionItem;
				actionItem[ACTION::strActionID] = action_add_silver;
				actionItem[ACTION::strValue] = thConfig.silverBase_ * rr.rewardTimes_;
				actionArr.append(actionItem);
			}
			else
			{
				Json::Value actionItem;
				Json::Value Param;
				actionItem[ACTION::strActionID] = action_add_item;
				actionItem[ACTION::addItemIDStr] = thConfig.equipIDList_[rr.rewardIndex_ - 1];
				actionItem[ACTION::addNumStr] = 1;
				actionArr.append(actionItem);
			}
		}
		int ret = 0;
		vector<actionResult> vecRetCode = actionFormat::actionDoJump(own.shared_from_this(), actionArr, Param);
		bool hasSend = email_sys.sendDoJumpFailEmail(own.shared_from_this(), actionArr, vecRetCode, email_team_system_space_explore, failSimpleJson);
		if (hasSend)
			ret = 3;

//十次寻宝流水：玩家名称、ID、VIP等级、操作时间、消耗钻石数、消耗前钻石数、消耗后钻石数、是否暴击（否/2倍/5倍）、获得奖励。
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(beforeGold - own.Base.getAllGold())); //f1
		fields.push_back(boost::lexical_cast<string, int>(own.Vip.getVipLevel())); //f2
		Json::Value criticalJson;
		for (unsigned i = 0; i < rlist.size(); i++)
		{
			criticalJson.append(rlist[i].rewardTimes_);
		}
// 		string s = criticalJson.toStyledString();
// 		s = commom_sys.tighten(s);
		string s = commom_sys.json2string(criticalJson);
		fields.push_back(s); //f3
		s = boost::lexical_cast<string, int>(na::time_helper::get_current_time());
		fields.push_back(s); //f4
		fields.push_back(""); //f5
		Json::Value simpleAction = gm_tools_mgr.complex2simple(actionArr);
// 		s = simpleAction.toStyledString();
// 		s = commom_sys.tighten(s);
		s = commom_sys.json2string(simpleAction);
		fields.push_back(s); //f6
		StreamLog::Log(space_explore_def::strDbsSpaceExplore, own.shared_from_this(),
			boost::lexical_cast<string, int>(beforeGold),
			boost::lexical_cast<string, int>(own.Base.getAllGold()), fields, space_explore_def::log_ten_explore);
		for (unsigned i = 0; i < 10; ++i)
			activity_sys.updateActivity(own.getOwnDataPtr(), 0, activity::_explore);
		return ret;
	}

	void playerSpaceExplore::exploreRandom(rewardResult& rr)
	{
		spaceExploreConfig thConfig = space_explore_sys.getConfig();
		int randomIndex = commom_sys.randomBetween(0, RANGE-1);
		if (randomIndex < thConfig.silverProbability_)
		{
			rr.rewardIndex_ = 0;
			int randomTimes = commom_sys.randomBetween(0, RANGE - 1);
			if (randomTimes < thConfig.fiveTimesCritical_)
			{
				rr.rewardTimes_ = 5;
// 				if (own.Vip.getVipLevel() < FIVE_TIMES_CRITICAL_VIP)
// 					rr.rewardTimes_ = 2;//VIP7的限制去掉，所有玩家都有概率爆5倍金币

				if (rr.rewardTimes_ == 5)
				{
					Json::Value chatContent;
					chatContent[playerNameStr] = own.Base.getName();
					chatContent[playerSilverStr] = thConfig.silverBase_;
//					chat_sys.sendToAllBroadCastCommon(BROADCAST::space_explore_five_times_critical, chatContent);
				}
			}
			else if (randomTimes < thConfig.twoTimesCritical_)
			{
				rr.rewardTimes_ = 2;
			}
			else
			{
			}
			return;
		}

		for (unsigned i = 0; i < thConfig.equipProbability_.size(); i++)
		{
			if (randomIndex < thConfig.equipProbability_[i])
			{
				rr.rewardIndex_ = i + 1;
				Params::ObjectValue obj = item_sys.getConfig(thConfig.equipIDList_[i]);
				int quality = obj[ItemDefine::itemQualityStr].asInt();
				Json::Value objJson = Params::ObjectValue::toJson(obj);
				if (quality == 5 && objJson[ItemDefine::chip].isNull())
				{
					Json::Value chatContent;
					chatContent[playerNameStr] = own.Base.getName();
					chatContent["eq"] = thConfig.equipIDList_[i];
//					chat_sys.sendToAllBroadCastCommon(BROADCAST::space_explore_purple_equip, chatContent);
				}
				break;
			}
		}
	}

	void playerSpaceExplore::refreshData()
	{
		maintainDailyData();
	}

	void playerSpaceExplore::maintainDailyData()
	{
		int cur_time = na::time_helper::get_current_time();
		if (cur_time > nextDailyFreshTime_)
		{
			todayOneHuntNotFreeUsedTimes_ = 0;

			boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
			tm t = boost::posix_time::to_tm(p);
			unsigned standard_time = cur_time - 3600 * t.tm_hour - 60 * t.tm_min - t.tm_sec;
			if (t.tm_hour >= 5)
			{
				nextDailyFreshTime_ = unsigned(na::time_helper::get_next_time(standard_time, 1, 5));
			}
			else
			{
				nextDailyFreshTime_ = unsigned(na::time_helper::get_next_time(standard_time, 0, 5));
			}
		}
	}

}
