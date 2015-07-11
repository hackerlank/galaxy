#pragma once

#include "block.h"

namespace gg
{
	
	const static string playerBaseDBStr = "gl.player";

	const static string playerNameStr = "na";
	const static string playerFaceIDStr = "fid";
	const static string playerSilverStr = "sl";
	const static string playerGoldTicketStr = "glt";
	const static string playerGoldStr = "gl";
	const static string playerLevelStr = "lv";
	const static string playerAreaIDStr = "aid";
	const static string playerAreaPageStr = "ap";
	const static string playerPopulationStr = "po";
	const static string playerBoomStr = "bo";
//	const static string playerLoyaltyStr = "lo";
	const static string playerInvestStr = "in";
	const static string playerSharedStr = "srp";
	const static string playerSphereIDStr = "spi";
	const static string playerKejiStr = "kj";//科技点 军工
	const static string playerWeiWangStr = "ww";
	const static string playerExpStr = "exp";
	const static string playerFrontGuildStr = "fgu";
	const static string playerCreateRoleTimeStr = "crt";
	const static string playerLastLoginTimeStr = "llt";
	const static string playerWarStoryCurrentProcessStr = "cp";
	const static string playerSetNameCostStr = "snc";
	const static string playerProcessStr = "pss";
	const static string playerGenderStr = "gdr";
	const static string guildDonateStr = "gdn";
	const static string guildSDonateStr = "gsdn";
	const static string playerPlanetIDStr = "ppi";//星球皮肤ID
	
	namespace playerDefine
	{
		enum
		{
			male,
			female,
			other,
		};
	}

	class TTManager;
	typedef std::pair<int,int> PlayerPosition;

	class playerBase : public Block
	{
		friend class TTManager;
	public:
		static void initPublicData();
		playerBase(playerData& own);
		virtual void autoUpdate();
		virtual void update();
		virtual bool save();
		virtual bool get();
		void setDefaultFace();
		int setFaceID(int faceID);
		int alterBothGold(int num);
		int alterVipBothGold(int goldNum, int goldTicketNum, string orderID);
		int alterSilver(int num);
		int alterGold(int num);
		int alterGoldTicket(int num);
		void setPosition(const int aID, const int aPage);
		void setPosition(PlayerPosition pos);
		int alterPopulation(int num);		//人口
//		int alterLoyalty(int num);		//忠诚度
		int setSphere(int ID);
		int alterKeJi(int num);
		int alterWeiWang(int num);		//威望（荣誉值）
		int setName(string name, const bool is_gm = false);
		int addExp(int num);
		void setPlayerProcess(unsigned process);
		int setGender(int type);
		int alterInvest(const int num);
		int alterShared(const int num);
		void tickLogin();
		//
		int getShared();
		int getInvest();
		int getGender(){return gender;}
		unsigned getPlayerProcess(){return playerProcess_;}
		int getFaceID(){return faceID_;}
		int getAllGold(){return (goldTicket_ + gold_);}
		int getSilver(){return silver_;}
		int getGold(){return gold_;}
		int getGoldTicket(){return goldTicket_;}
		int getLevel(){return level_;}
		int getExp(){return exp_;}
		PlayerPosition getPosition(){return PlayerPosition(areaID_, areaPage_);}
//		int getPopulation(){return population_;}//人口
		int getCalPopu();
		int getBoom(){return boom_;}//繁荣
//		int getLoyalty(){return loyalty_;}//民忠
		int getSphereID(){return sphereID_;}//国家
		bool SphereVaild(){ return (sphereID_  > -1 && sphereID_ < 3); };
		int getKeJi(){return keji_;}//科技
		int getWeiWang(){return weiwang_;}//威望
		string getName(){return name_;}
		bool isVaild(){return name_.length() > 0;}
		int getSetNameCost();
		int planetID;
		int calMaxLv();
	private:
		static vector<int> playerUpgradeExp;
		void upgrade();
		void upgradeEvent(bool pay = true);
		int gender;
		unsigned playerProcess_;
		int setNameTimes;
		int silver_;
		int goldTicket_;
		int gold_;
		int level_;
		int areaID_;
		int areaPage_;
		int population_;
		int boom_;
		int invest_;
		int shared_;
		int sphereID_;
		int keji_;//科技点 军工
		int weiwang_;
		string name_;
		int faceID_;
		int exp_;
		int createRoleTime_;
		unsigned lastLoginTime_;
		void package(Json::Value& pack);
	};
}