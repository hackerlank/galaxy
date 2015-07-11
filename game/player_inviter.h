#pragma once
#include "block.h"
#include <boost/unordered_map.hpp>

#define  LIMIT_LV    50					//奖励的限制等级,第二次修改（从40限制到50）
#define  MAX_PEOPLE_NUM   30			//最大限制人数
namespace gg{

	const static string player_inviter_db_name = "gl.player_inviter";
	const static string inviterIDStr = "iid";		
	const static string inviteCodeStr = "ic";
	const static string getMasonryNumStr = "gmn";
	const static string totalMasonryNumStr = "tmn";
	const static string byInveiterIDStr = "biid";
	const static string rewardInfoStr = "rif";
	const static string rewardIDStr = "rid";
	const static string getRewardNumStr = "grn";
	const static string playerNumOfNowStr = "pnn";
	const static string playerNumOfMatchStr = "pnm";
	const static string nowGetMasonryNumStr = "ngm";
	const static string alterFlagStr = "afs";

	namespace OpID{
		enum OpType{
			alterPeopleNumOfNow,			//更新总人数
			alterPeopleNumOfMacth,			//更新符合条件的人数
			alterMasonryNum					//更新得到砖石数
		};
	}
	
	typedef boost::unordered_map<int, int> RewardInfoMap;


	class player_inviter :
		public Block
	{
	public:
		player_inviter(playerData &own);
		~player_inviter();
		virtual void autoUpdate();
		virtual bool save();
		virtual bool get();
		void update();
		void packageReward(int reID,Json::Value& msg);

		void setInviterID(int playerID);
		void setInviteCode(string code);
		void updateInviterPeopleNumOfNowInfo();	//更新总邀请人人数信息
		void updateInviterPeopleNumOfMatchInfo();	//更新符合条件邀请人人数信息
		void updateInviterGetMasonryNum(int num);	//更新邀请人得到砖石数量
		void alterRewardInfo(int rewardID);		//修改奖励信息
		void alterPlayerNumOfNow();
		void alterPlayerNumOfMatch();	//修改邀请的人人数
		void alterTotalMasonryNum(int MasonryNum);		//修改得到的奖励数
		int getInviterID();
		int getTotalMasonryNum();
		int getRewardInfo(int rewardID);
		int getPlayerNumOfMatch();
		bool numIsFull();					//人数是否满了
		string getInviteCode();
	public:
		static RewardInfoMap gRewardInfo;				//全局变量	
	private:	
		const static int maxPeopleNum;		//能达到的最大数
		const static int countLv;			//统计玩家数的限制等级
	private:
		int inviterID;						//邀请人ID
		int getMasonryNum;					//得到的砖石数	
		int totalMasonryNum;				//总的砖石数
		int playerNumOfNow;
		int playerNumOfMatch;				//满足条件的玩家数
		bool alterFlag;						//更新标志
		string inviteCode;					//邀请码
		vector<int> byInveiterID;			//邀请的人ID
		RewardInfoMap rewardInfo;			//奖励信息
	};

}