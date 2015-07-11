#pragma once

#include "playerManager.h"
#include "json/json.h"
#include "params.hpp"

#define jl_sys (*gg::junlingSystem::junlingSys)

namespace gg
{
	class junlingSystem
	{
	public:
		static junlingSystem* const junlingSys;
		void initData();
		void update(msg_json& m, Json::Value& r);
		void clearCD(msg_json& m, Json::Value& r);
		void BuyJunling(msg_json& m, Json::Value& r);
		int getCost(playerDataPtr player);
	private:
		bool canBuy(playerDataPtr player, const int num);
		int getCost(playerDataPtr player, int& num);
		vector<int> VipLimit;
		vector<int> GoldCost;
	};
}