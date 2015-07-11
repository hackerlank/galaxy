#pragma once
#include "block.h"
#include "time_helper.h"

namespace gg
{
	namespace monthcard
	{
		const static string strNextUpdateTime = "nt";
		const static string strTimes = "ts";
		const static string strReceived = "rcv";
	}

	class monthCard
	{
	public:
		monthCard():_times(0), _received(false), _next_update_time(0){}
		void reset();
		int getTimes(unsigned now);
		bool received(unsigned now);
		void package(Json::Value& info);
		void alter();
		int getTimes();
		mongo::BSONObj toBSONObj();
		void setFromObj(const mongo::BSONElement& obj);

	private:
		void checkAndUpdate(unsigned now);

		unsigned _next_update_time;
		int _times;
		bool _received;
	};

	enum 
	{
		null_card,
		week_card,
		month_card,
		season_card
	};
	namespace vip
	{
		const static string vipDBStr = "gl.player_vip";
		const static string strRechargeGold = "rg";
		const static string strHasGetFirst = "gf";
		const static string strGiftList = "gl";
		const static string strFirstChargeNumber = "fcn";

		const static string strMonthCardInfo = "mc";
	}
	
	class playerVip : public Block
	{
	public:
		playerVip(playerData& own);
		bool save();
		bool get();
		void autoUpdate();
		void charge(int type, int gold);
		void package(Json::Value& pack);
		int getVipLevel();
		int pickupGift(int vlevel);
		int pickupFirstGift();
		int getRechargeGold(){ return rechargeGold_; };
		int getFirstChargeNUmber(){ return firstChargeNum_; };
		int alterRechargeGold(int num, bool isCharge = false);
		void setVip(const int level);
		void notifyFirstGift();

		void buyMonthCard();
		void getCardReward(int reward);

	public:
		monthCard monthCard_;

	private:
		int rechargeGold_;
		set <int> getGiftList_;
		bool hasGetFirstGift_;
		int firstChargeNum_;
	};
}
