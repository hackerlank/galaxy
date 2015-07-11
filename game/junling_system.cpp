#include "junling_system.h"
#include "response_def.h"
#include "time_helper.h"
#include "helper.h"
#include "file_system.h"
#include "activity_game_param.h"
#include "activity_system.h"

namespace gg
{
	junlingSystem* const junlingSystem::junlingSys = new junlingSystem();

	bool junlingSystem::canBuy(playerDataPtr player, const int num)
	{
		int vl = player->Vip.getVipLevel();
		if(vl < 0 || vl >= (int)VipLimit.size())return false;
		int mb = VipLimit[vl];
		if(mb < num + player->Campaign.getBuyToday())return false;
		return true;
	}

	int junlingSystem::getCost(playerDataPtr player)
	{
		int num = 1;
		return getCost(player, num);
	}

	int junlingSystem::getCost(playerDataPtr player, int& num)
	{
		int gold = 0;
		int idx = player->Campaign.getBuyToday();
		int sz = (int)GoldCost.size();
		for (int i = 0; i < num; ++i)
		{
			idx += i;
			if(idx >= sz){num = i; break;}
			gold += GoldCost[idx];
		}
		return gold;
	}

	void junlingSystem::BuyJunling(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int num = js_msg[0u].asInt();
		if(num != 1)Return(r, -1)
		if(!canBuy(d, num))Return(r, 1);//已经到达购买上限
		int cost = getCost(d, num);
		cost = (int)ceil(cost*(1+activity_sys.getRate(param_vip_buy_junling, d)));
		if(cost > d->Base.getAllGold())Return(r, 2);//金币不够
		d->Base.alterBothGold(-cost);
		d->Campaign.alterJunling(num);
		d->Campaign.alterBuyToday(num);
		Return(r, 0);
	}

	void junlingSystem::update(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Campaign.update();
	}

	void junlingSystem::clearCD(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		unsigned now = na::time_helper::get_current_time();
		CDData cd = d->Campaign.getCD().getCD();
		int cost = GGCOMMON::costCDClear(cd.CD, 60);
		if(cost > d->Base.getAllGold())Return(r, 1);
		d->Base.alterBothGold(-cost);
		d->Campaign.getCD().clearCD();
		helper_mgr.insertSaveAndUpdate(&d->Campaign);
		Return(r, 0);
	}


	const static string VipLimitConfigDir = "./instance/junling/vip_limit.json";
	const static string GoldCostConfigDir = "./instance/junling/gold_cost.json";
	void junlingSystem::initData()
	{
		VipLimit.clear();
		GoldCost.clear();
		{
			Json::Value config = na::file_system::load_jsonfile_val(VipLimitConfigDir);
			for (unsigned i = 0; i < config.size(); ++i)
			{
				VipLimit.push_back(config[i].asInt());
			}
		}
		{
			Json::Value config = na::file_system::load_jsonfile_val(GoldCostConfigDir);
			for (unsigned i = 0; i < config.size(); ++i)
			{
				GoldCost.push_back(config[i].asInt());
			}
		}
	}
}