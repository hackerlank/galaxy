#include "vip_system.h"
#include "file_system.h"
#include "response_def.h"
#include "item_system.h"
#include "gate_account_protocol.h"
#include "helper.h"
#include "record_system.h"
#include "activity_rebate.h"
#include "chat_system.h"

namespace gg
{
	vip_system* const vip_system::vipSys = new vip_system();

	vip_system::vip_system()
	{

	}

	vip_system::~vip_system()
	{

	}

	void vip_system::initData()
	{
		db_mgr.ensure_index(vip::vipDBStr, BSON(playerIDStr << 1));
		_vipConfigJson = na::file_system::load_jsonfile_val(vip::strVipDataPath);
		
		for (unsigned i = 0; i < _vipConfigJson[vip::strVipPrice].size(); i++)
		{
			_vipConfig.vip_prices.push_back(_vipConfigJson[vip::strVipPrice][i].asInt());
		}

		for (unsigned i = 0; i < _vipConfigJson[vip::strVipReward].size(); i++)
		{
			_vipConfig.vip_gift_action.push_back(_vipConfigJson[vip::strVipReward][i]);
		}

		Json::Value tempJson = _vipConfigJson[vip::strFirstChargeReward];
		actionVector actVec = actionFormat::formatActionVec(tempJson);
		_vipConfig.first_charge_action = actVec;

		Json::Value monthCardJson = na::file_system::load_jsonfile_val(vip::strMonthCardDataPath);
		_monthCardConfig._days = monthCardJson["days"].asInt();
		_monthCardConfig._reward = monthCardJson["reward"].asInt();
		_monthCardConfig._accumulate = monthCardJson["accumulate"].asBool();
	}

	void vip_system::vipUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Vip.package(r);
	}

	void vip_system::pickUpVipGift(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int vip_level = js_msg[0u].asInt();
		bool need_broadcast = js_msg[1].asBool();
		if (vip_level < 1 || vip_level > (int)_vipConfig.vip_prices.size())
		{
			Return(r, -1);
		}

		if (d->Vip.getVipLevel() < vip_level)
		{
			Return(r, 2);
		}

		int res = d->Vip.pickupGift(vip_level);
		if (need_broadcast)
		{
			chat_sys.sendToAllVipGetGift(d, vip_level);
		}

		Return(r, res);
	}

	void vip_system::pickUpFirstGift(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		int res = d->Vip.pickupFirstGift();

		Return(r, res);
	}

	void vip_system::chargeGold(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;

		int player_id = 0;
		int gold,extra_gold,charge_type,charge_target;
		std::string order_id,pay_type_str,error_str;
		player_id	= js_msg[0u].asInt();//player_mgr.get_player_id_by_uid(uid);
		order_id	= js_msg[1u].asString();
		gold		= js_msg[2u].asInt();
//		pay_type_str= js_msg[3u].asString();
//		pay_amount	= js_msg[4u].asInt();
		extra_gold	= js_msg[3u].asInt();
		charge_type	= js_msg[4u].asInt();//新字段，标识要冲哪种类型的卡(0 普通充值, 1 月卡)
//		charge_target = js_msg[7u].asInt();//新字段，标识是给自己冲还是给好友冲
		charge_target = 0;

		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		playerDataPtr tempd = d;
		if (charge_target > 0)
		{
			playerDataPtr d_target = player_mgr.getPlayerMain(charge_target);
			if(playerDataPtr() == d || playerDataPtr() == d_target)
			{
				if(m._post_times > 0)Return(r, 1);
				player_mgr.postMessage(m);
				return;
			}
			tempd = d_target;
		}
		else
		{
			if(playerDataPtr() == d)
			{
				if(m._post_times > 0)Return(r, 1);
				player_mgr.postMessage(m);
				return;
			}
		}

		if(player_id == 0) 
		{
			r[msgStr][0u] = 1;
		}
		else
		{
			r[msgStr][0u] = 0;
			tempd->playerExplore.alterOrdinaryMaterials(gold);
// 			r[msgStr][1u] = order_id;
// 			r[msgStr][2u] = gold;
// 			r[msgStr][3u] = pay_type_str;
// 			r[msgStr][4u] = pay_amount;
// 			r[msgStr][5u] = error_str;
// 			r[msgStr][6u] = js_msg[7u]; //unit
// 			r[msgStr][7u] = extra_gold;
			if(charge_type == 1)
				tempd->Vip.buyMonthCard();

//			int previousGold = tempd->Base.getGold();
			int previousRechargeGold = tempd->Vip.getRechargeGold();

			//下面的修改钻石赠券的两条操作合并为一条
			tempd->Base.alterVipBothGold(gold, extra_gold, order_id);
// 			tempd->Base.alterGold(gold);
// 			if (extra_gold > 0)
// 			{
// 				tempd->Base.alterGoldTicket(extra_gold);
// 			}

			tempd->Vip.alterRechargeGold(gold, true);
			tempd->Vip.notifyFirstGift();
			//		tempd->Vip.charge(charge_type, gold);
//			tempd->Vip.setIsFirstTimeSeasonCard(first_charge_not_pick_up);
			helper_mgr.insertSaveAndUpdate(&d->Vip);
			rebate_sys.rebateRecharge(d, gold);
			point_sys.pointRecharge(d, gold);
			if (charge_target != 0)
			{
				helper_mgr.insertSaveAndUpdate(&tempd->Vip);
				player_mgr.sendToPlayer(tempd->playerID, protocol::l2c::charge_gold_resp, r);
			}

			vipChargeResp(d, charge_type, gold, extra_gold);

			string logData = r[msgStr].toStyledString();
			vector<string> fds;
			fds.push_back(order_id);
			fds.push_back(logData);
			fds.push_back(boost::lexical_cast<string, int>(charge_type));
			fds.push_back(boost::lexical_cast<string, int>(extra_gold));
			StreamLog::Log(vip::MysqlLogChargeGold, d, boost::lexical_cast<string, int>(previousRechargeGold), 
				boost::lexical_cast<string, int>(tempd->Vip.getRechargeGold()), fds, vip_log_recharge);
		}

	}

	vipConfig& vip_system::getVipConfig()
	{
		return _vipConfig;
	}

	Json::Value vip_system::getRewardJson(int vlevel)
	{
		if (vlevel < 0 || vlevel >= int(_vipConfigJson[vip::strVipReward].size()))
			return Json::Value::null;
		return _vipConfigJson[vip::strVipReward][vlevel];
	}

	//void vip_system::buyMonthCard(msg_json& m, Json::Value& r)
	//{
	//	AsyncGetPlayerData(m);

	//	d->Vip.buyMonthCard();
	//	Return(r, 0);
	//}

	void vip_system::monthCardReward(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		unsigned now = na::time_helper::get_current_time();
		//if(d->Vip.monthCard_.getTimes(now) == 0)
		//{
		//	d->Vip.buyMonthCard(_monthCardConfig._days);
		//	return;
		//}

		if(d->Vip.monthCard_.getTimes(now) <= 0)
			Return(r, 1);

		if(d->Vip.monthCard_.received(now))
			Return(r, 2);

		int before = d->Base.getAllGold();
		d->Vip.getCardReward(_monthCardConfig._reward);

		vector<string> fds;
		fds.push_back(boost::lexical_cast<string, int>(d->Vip.monthCard_.getTimes(now)));
		StreamLog::Log(vip::MysqlLogChargeGold, d, boost::lexical_cast<string, int>(before), 
			boost::lexical_cast<string, int>(d->Base.getAllGold()), fds, vip_log_month_card_reward);

		Return(r, 0);
	}

	void vip_system::vipChargeResp(playerDataPtr d, int type, int gold, int extra_gold)
	{
		Json::Value msg;
		if(type == -1)
			msg[msgStr][0u] = -1;
		else
		{
			msg[msgStr][0u] = 0;
			msg[msgStr][1u][vip::strChargeType] = type;
			msg[msgStr][1u][vip::strChargeGold] = gold;
			msg[msgStr][1u][vip::strChargeExtraGold] = extra_gold;
		}
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::vip_charge_resp, msg);
	}

	int vip_system::getVipLevelByChargeGold(int chargeGold)
	{
		int ret = 0;
		vipConfig& vipConfig = vip_sys.getVipConfig();
		for (int idx = vipConfig.vip_prices.size() - 1; idx >= 0; idx--)
		{
			if (chargeGold >= vipConfig.vip_prices[idx])
			{
				ret = idx;
				break;
			}
		}
		// 		if (ret > 8)
		// 			ret = 8;//vip暂时只开放到8级
		return ret;
	}

	bool vip_system::isVipByChargeGold(int chargeGold)
	{
		if(_vipConfig.vip_prices.size() <= 1)
			return false;
		return chargeGold >= _vipConfig.vip_prices[1];
	}
}
