#pragma once
#include <map>
#include "msg_base.h"
#include <boost/shared_ptr.hpp>
#include "block.h"
#include "activity_base.h"

#define gift_defined_sys (*gg::act_gift_defined::giftDefinedSys)

using namespace na::msg;
using namespace std;

namespace gg
{
	namespace gift_defined_def
	{
		const static string strGiftDefinedList = "gls";
		const static string strGiftDataList = "ls";
		const static string strLevelLimit = "lm";
		const static string strPackID = "id";
		const static string strCurrentID = "cid";
		const static string strGiftDescript = "gd";
		const static string strGiftName = "gn";
		const static string strTotalReceiveTimes = "ttrt";
		const static string strPerDayReceiveTimes = "pdrt";
		const static string strVipLevel = "vl";
		const static string strGift = "gf";
		const static string strMyTotalRemainReceiveTimes = "mrt";
		const static string strMyTodayRemainReceiveTimes = "mrtd";
		const static string strMyGift = "mgf";
		const static string strMysqlLogGiftDefined = "log_gift_defined";

		enum
		{
			log_pickup_gift
		};
	}
	
	struct giftDefinedData
	{
		int vipLevel_;
		Json::Value gift_;
	};

	struct singleDefinedPack
	{
		singleDefinedPack();
		bool operator== (singleDefinedPack& singlePack);
		bool isValid();
		bool isOverdue();
		static singleDefinedPack sNull_;
		map<int, giftDefinedData> giftDataList_;
		int id_;
		int levelLimit_;
		int totalReceiveTimes_;
		string giftName_;
		string giftDescript_;
		act_time_info act_time_;
	};

	class act_gift_defined
	{
	public:
		static act_gift_defined* const giftDefinedSys;
		act_gift_defined();
		~act_gift_defined();
		virtual void package(Json::Value& updateListJson);
		void initData();

		void gmActivityGiftDefinedModifyReq(msg_json& m, Json::Value& r);
		void giftDefinedUpdateReq(msg_json& m, Json::Value& r);
		void giftDefinedPickupGiftReq(msg_json& m, Json::Value& r);
		void giftDefinedPlayerInfoUpdateReq(msg_json& m, Json::Value& r);
		singleDefinedPack& getSinglePack(int packID);
	private:
		void insertSortPack(vector<singleDefinedPack> &vecTotalPack, singleDefinedPack singlePack);
		void deleteOldData();
		void updateToClients(vector<playerDataPtr> players);
		int getPilotNum(Json::Value actionJson);
		Json::Value base2actual(Json::Value &baseAction, playerDataPtr player);
		virtual bool get();
		virtual bool save();
		int getNewID(vector<singleDefinedPack> vecTotalPack);
		void refreshPackList();
		vector<singleDefinedPack> vecTotalPack_;
		int currentPackID_;
	};
}


