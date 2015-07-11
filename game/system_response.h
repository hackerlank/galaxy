#pragma once

#include "mongoDB.h"
#include "playerManager.h"
#include "response_def.h"
#include "msg_base.h"
#include "battle_def.h"

using namespace na::msg;

#define resp_sys (*gg::system_response::respSys)

namespace gg
{
	extern void saveDev();
	class system_response
	{
	public:
		static system_response* const respSys;
		
		void initData();
		//battle
		void teamPVERespon(BattlefieldMuilt field, Json::Value report);
		void PVERespon(Battlefield field, Json::Value report);
		void PVPRespon(const string file, Json::Value report, Battlefield field);
		void noticePlayerGuild(msg_json& m, Json::Value& r);
		void upgradeToLeaderNoticeReq(msg_json& m, Json::Value& r);

		void OverBattle(vector<int> IDList);
		void OverBattleBoss(vector<int> IDList);

		//player role system
		void roleUpdateClient(msg_json& m, Json::Value& r);
		void createRole(msg_json& m, Json::Value& r);
//		void updateCDList(msg_json& m, Json::Value& r);
		void playerLogin(msg_json& m, Json::Value& r);
		void syncAccount(msg_json& m, Json::Value& r);
		void playerProcess(msg_json& m, Json::Value& r);
		void setGender(msg_json& m, Json::Value& r);
		void joinKingdom(msg_json& m, Json::Value& r);
		void joinMinKingdom(msg_json& m, Json::Value& r);

		//gm¹¦ÄÜ
		void serverTimeGet(msg_json& m, Json::Value& r);
		void serverTimeSet(msg_json& m, Json::Value& r);

		void levy(msg_json& m, Json::Value& r);
		void updateLevy(msg_json& m, Json::Value& r);
		void levyCDClear(msg_json& m, Json::Value& r);

		void invest(msg_json& m, Json::Value& r);

//		void chargeGold(msg_json& m, Json::Value& r);

		void setFaceIdReq(msg_json& m, Json::Value& r);
		void changeName(msg_json& m, Json::Value& r);
		void gmMotifyName(msg_json& m, Json::Value& r);
		void gmAddWarStroy(msg_json& m, Json::Value& r);
		void gmMotifyKingdom(msg_json& m, Json::Value& r);
		//system method
		void gateRestart(msg_json& m, Json::Value& r);
		
		//system call 
		void asyncSystemPlayerData(playerDataPtr player);
		void asyncSystemPlayerData(playerData& player);
	private:
		void systemUpdate();
		void sendLoginUpdate(playerDataPtr player);
		Params::ArrayValue investPopulation;
	};
}