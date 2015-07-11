#include "player_vip.h"
#include "vip_system.h"
#include "item_system.h"
#include "record_system.h"
#include "email_system.h"
#include "gm_tools.h"
#include "ally_system.h"
#include "activity_system.h"
namespace gg
{
	void monthCard::alter()
	{
		--_times;
		_received = true;
	}

	mongo::BSONObj monthCard::toBSONObj()
	{
		return BSON(monthcard::strNextUpdateTime << _next_update_time 
			<< monthcard::strReceived << _received
			<< monthcard::strTimes << _times);
	}

	void monthCard::setFromObj(const mongo::BSONElement& obj)
	{
		checkNotEoo(obj[monthcard::strNextUpdateTime])
			_next_update_time = obj[monthcard::strNextUpdateTime].Int();
		checkNotEoo(obj[monthcard::strReceived])
			_received = obj[monthcard::strReceived].Bool();
		checkNotEoo(obj[monthcard::strTimes])
			_times = obj[monthcard::strTimes].Int();
	}

	void monthCard::package(Json::Value& info)
	{
		unsigned now = na::time_helper::get_current_time();
		info[monthcard::strTimes] = getTimes(now);
		info[monthcard::strReceived] = received(now);
	}

	void monthCard::reset()
	{
		unsigned now = na::time_helper::get_current_time();
		checkAndUpdate(now);
		_next_update_time = na::time_helper::get_next_update_time(now);

		if (vip_sys.getMonthCardConfig()._accumulate)
		{
			if (_times <= 0)
				_received = false;
			_times += vip_sys.getMonthCardConfig()._days;
		}
		else
		{
			_times = vip_sys.getMonthCardConfig()._days;
			_received = false;
		}
	}

	int monthCard::getTimes(unsigned now)
	{
		checkAndUpdate(now);
		return _times;
	}

	int monthCard::getTimes()
	{
		unsigned now = na::time_helper::get_current_time();
		return getTimes(now);
	}

	bool monthCard::received(unsigned now)
	{
		checkAndUpdate(now);
		return _received;
	}

	void monthCard::checkAndUpdate(unsigned now)
	{
		if(_times <= 0)
			return;

		if(now >= _next_update_time)
		{
			int diff = (now - _next_update_time) / na::time_helper::DAY;
			_times = _times > diff? _times - diff : 0;
			if(_times > 0 && !_received)
				--_times;

			_received = false;
			_next_update_time = na::time_helper::get_next_update_time(now);
		}
	}


	playerVip::playerVip(playerData& own) : Block(own)
	{
		hasGetFirstGift_ = false;
		rechargeGold_ = 0;
		getGiftList_.clear();
		firstChargeNum_ = 0;
	}

	bool playerVip::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		mongo::BSONArrayBuilder ar;
		obj << playerIDStr << own.playerID;

		obj << vip::strRechargeGold << rechargeGold_;
		obj << vip::strHasGetFirst << hasGetFirstGift_;
		obj << vip::strFirstChargeNumber << firstChargeNum_;

		for (set<int>::iterator it = getGiftList_.begin(); it != getGiftList_.end(); it++)
		{
			ar << *it;
		}
		obj << vip::strGiftList << ar.arr() << vip::strMonthCardInfo << monthCard_.toBSONObj();

		return db_mgr.save_mongo(vip::vipDBStr, key, obj.obj());
	}

	bool playerVip::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		//LogI << __FUNCTION__ << ", own.playerID:" << own.playerID << LogEnd;
		mongo::BSONObj obj = db_mgr.FindOne(vip::vipDBStr, key);
		if(!obj.isEmpty())
		{
			checkNotEoo(obj[vip::strRechargeGold])
			{
				rechargeGold_ =  obj[vip::strRechargeGold].Int();
			}
			checkNotEoo(obj[vip::strHasGetFirst])
			{
				hasGetFirstGift_ =  obj[vip::strHasGetFirst].Bool();
			}
			checkNotEoo(obj[vip::strFirstChargeNumber])
			{
				firstChargeNum_ = obj[vip::strFirstChargeNumber].Int();
			}
			checkNotEoo(obj[vip::strGiftList])
			{
				vector<mongo::BSONElement> blist = obj[vip::strGiftList].Array();

				getGiftList_.clear();
				for (unsigned i = 0; i < blist.size(); i++)
				{
					getGiftList_.insert(blist[i].Int());
				}
			}
			checkNotEoo(obj[vip::strMonthCardInfo])
				monthCard_.setFromObj(obj[vip::strMonthCardInfo]);

			return true;
		}

		return false;
	}

	void playerVip::autoUpdate()
	{
		Json::Value msg;
		package(msg);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::vip_update_resp, msg);
	}

	void playerVip::package(Json::Value& pack)
	{
		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();
		pack[msgStr][1u][vip::strRechargeGold] = rechargeGold_;
		pack[msgStr][1u][vip::strHasGetFirst] = hasGetFirstGift_;

		pack[msgStr][1u][vip::strGiftList] = Json::arrayValue;
		for (set<int>::iterator it = getGiftList_.begin(); it != getGiftList_.end(); it++)
		{
			pack[msgStr][1u][vip::strGiftList].append(*it);
		}

		monthCard_.package(pack[msgStr][1u][vip::strMonthCardInfo]);
	}

	void playerVip::setVip(const int level)
	{
		vipConfig& vipConfig = vip_sys.getVipConfig();
		const int maxlv = vipConfig.vip_prices.size() - 1;
		int setl = level < 1 ? 0 : level;
		setl = setl > maxlv ? maxlv : setl;
		rechargeGold_ = vipConfig.vip_prices[setl];
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerVip::getVipLevel()
	{
		int ret = 0;
		vipConfig& vipConfig = vip_sys.getVipConfig();
		for (int idx = vipConfig.vip_prices.size() - 1; idx >= 0; idx--)
		{
			if (rechargeGold_ >= vipConfig.vip_prices[idx])
			{
				ret = idx;
				break;
			}
		}
// 		if (ret > 8)
// 			ret = 8;//vip暂时只开放到8级
		return ret;
	}

	int playerVip::pickupGift(int vlevel)
	{
		set<int>::iterator it = find(getGiftList_.begin(), getGiftList_.end(), vlevel);
		if (it != getGiftList_.end())
		{
			return 1;
		}
		
		int ret = 0;
		vipConfig &vip_conf = vip_sys.getVipConfig();
		Json::Value parm;
		actionResult resultCode = actionFormat::actionDo(own.getOwnDataPtr(), vip_conf.vip_gift_action[vlevel], parm);
		if (resultCode.resultCode != 0)
		{
			Json::Value simpleAction = gm_tools_mgr.complex2simple(vip_conf.vip_gift_action[vlevel]);
			email_sys.sendSystemEmailCommon(own.shared_from_this(), email_type_system_attachment, 
				email_team_system_bag_full_vip_gift, Json::Value::null, simpleAction);
			ret = 3;
		}

		getGiftList_.insert(vlevel);
		helper_mgr.insertSaveAndUpdate(this);

		Json::Value tempJson = vip_sys.getRewardJson(vlevel);
		Json::Value simpleJson = gm_tools_mgr.complex2simple(tempJson);
// 		string s = simpleJson.toStyledString();
// 		s = commom_sys.tighten(s);
		string s = commom_sys.json2string(simpleJson);
		vector<string> fds;
		fds.push_back("");//f1
		fds.push_back("");//f2
		fds.push_back("");//f3
		fds.push_back("");//f4
		fds.push_back("");//f5
		fds.push_back(s);//f6
		StreamLog::Log(vip::MysqlLogChargeGold, own.shared_from_this(), boost::lexical_cast<string, int>(this->getVipLevel()), 
			boost::lexical_cast<string, int>(vlevel), fds, vip_log_get_vip_gift);

		return ret;
	}

	int playerVip::pickupFirstGift()
	{

		if (getVipLevel() <= 0)
		{
			return 1;
		}
		if (hasGetFirstGift_)
		{
			return 2;
		}

		vipConfig &vip_conf = vip_sys.getVipConfig();
		Json::Value parm;
		actionFormat::actionDo(own.getOwnDataPtr(), vip_conf.first_charge_action, parm);
		hasGetFirstGift_ = true;
		helper_mgr.insertSaveAndUpdate(this);

		return 0;
	}

	int playerVip::alterRechargeGold(int num, bool isCharge /*= false*/)
	{
		int tmp = rechargeGold_;
		rechargeGold_ += num;
		rechargeGold_ = rechargeGold_ < 0 ? 0 : rechargeGold_;
//		StreamLog::Log(mysqlLogSilver, own, tmp, rechargeGold_);
		if (isCharge && firstChargeNum_ <= 0){
			firstChargeNum_ = num;
			own.inviter.updateInviterGetMasonryNum(int(num*0.2));
		}
		helper_mgr.insertSaveAndUpdate(this);

		if(num > 0)
			ally_sys.postToUpdateInfo(own.getOwnDataPtr());

		return 0;
	}

	void playerVip::notifyFirstGift()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1] = hasGetFirstGift_;
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::vip_notify_first_gift_resp, msg);
	}

	void playerVip::buyMonthCard()
	{
		monthCard_.reset();
		activity_sys.updateActivity(own.getOwnDataPtr(), 0, activity::_buy_month_card);
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerVip::getCardReward(int reward)
	{
		own.Base.alterGoldTicket(reward);
		monthCard_.alter();
		activity_sys.updateActivity(own.getOwnDataPtr(), 0, activity::_get_month_card);
		helper_mgr.insertSaveAndUpdate(this);
	}
}



