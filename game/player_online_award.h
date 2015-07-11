#pragma once
#include "player_base.h"
#include "block.h"
#include "file_system.h"
#include <boost/unordered_map.hpp>
namespace gg{
	const static string player_onlineaward_db_name="gl.player_online_award";		//数据库名

	const static string player_award_mongoDB = "aw";	//数据库中的列名
	const static string player_award_type = "at";	//奖励类型,以int型表示，从1开始，以每次递增的形式累加，每次累加1
	const static string player_award_total_cd = "tc";	//总cd时间
	const static string player_award_content = "c";	//奖励内容
	//const static string player_award_silver = "asl";	//奖励银币
	//const static string player_award_gold	= "agl";	//奖励金币
	//const static string player_award_keji	= "akj";	//奖励科技
	//const static string player_award_goldticket = "aglt";	//奖励金票
	//const static string player_award_credit	= "ac";	//奖励经验丹
	//const static string player_award_honor = "ah";	//奖励荣誉值
	//const static string player_award_supply = "as";	//奖励补给
	const static string player_award_limit_level = "lv";	//奖励限制等级

	const static string player_award_login_time = "li";	//用户登录时间
	const static string player_award_logout_time = "lo";	//用户登出时间
	const static string player_award_content_indicate = "aci";	//奖励内容指示
	const static string player_award_type_indicate = "ati";	//奖励类型指示
	const static string player_initial_time = "it";		//领取奖励的计时时间
	const static string player_award_cd_time = "cd";  //领取奖励剩余CD时间
	const static string player_get_award_num = "an";	//已领取奖励数量
	//玩家信息
	struct playerMsg{
		unsigned int playerLoginTime;		//用户登录时间
		unsigned int playerLogoutTime;		//用户登出时间
		int playerContentIndicate;			//奖励内容指示
		int awardTypeIndicate;	//奖励类型指示
		int awardNum;	//已领奖的数量
		unsigned int initialTime;//领奖时间
		unsigned int awardCD;	//领奖CD时间
		playerMsg(){
			playerLoginTime = 0;
			playerLogoutTime = 0;
			playerContentIndicate = 0;
			initialTime= 0;
			awardCD= 0;
			awardNum= 0;
			awardTypeIndicate = 1;
		}
	};

	struct Award{
		int actionID;
		int itemID;
		//vector<int> value;			//金币银币这几种资源的数量
		vector<int> addNum;			//物品的数量	
		Award(){
			actionID = -1;
			itemID = -1;
			//value.clear();
			addNum.clear();	
		}
	};
	//奖励内容
	struct AwardContent{
		int awardType;	//奖励类型
		int awardTotalCD;//总CD时间
		vector<int> levelLimit;	//限制等级
		vector<Award> m_Award;
		AwardContent(){
			awardType  = -1;
			awardTotalCD = -1;
			m_Award.clear();
			levelLimit.clear();
		}

	};
	

	//struct AwardContent{
	//	int awardType;	//奖励类型
	//	int awardTotalCD;//总CD时间
	//	vector<int> awardSilver;//银币
	//	vector<int> awardGold;	//金币
	//	vector<int> awardGoldTicket;//金票
	//	vector<int> awardKeJI;	//科技

	//	vector<int> awardCredit;	//经验丹
	//	vector<int> awardHonor;	//荣誉值
	//	vector<int> awardSupply;	//补给

	//	vector<int> levelLimit;	//限制等级
	//	AwardContent(){
	//		memset(this,0,sizeof(AwardContent));
	//	}
	//};
	typedef boost::unordered_map<int,AwardContent> AwardContentMap;
	class playerOnlineAward :
		public Block
	{
	public:
		playerOnlineAward(playerData &own);
		~playerOnlineAward(void);
		//领取奖励
		int getAward(playerDataPtr player, int typeIndicate,double ratio, Json::Value& fail_value);
		//重置奖励信息
		void resetAward(const AwardContentMap& contentMap);
		//记录用户登录时间
		void playerLogin();
		//玩家退出信息处理
		bool playerLogout();
		//设置数据库指示器现在指向哪个数据库
		//void setDBIndicate(int DBIndicate);
		void packagePlayerMsg(Json::Value &pack);
		//继承父类方法
		virtual void autoUpdate();
		virtual void update();
		virtual bool save();
		virtual bool get();
	private:

		//更新玩家信息
		bool updatePlayer(playerDataPtr player, double ratio, Json::Value& fail_value);
		//初始化奖励信息
		void initAwardMsg();
		//判断CD时间
		bool judgeCD(unsigned now);
		void package(Json::Value &pack);
		//打包actionDo json
		int packageActionDoJson(int type,double ratio,Json::Value& msg);
		//打包资源json
		int packageClientJson(int type,Json::Value& msg);
	private: 
		playerMsg m_PlayerMsg;
		AwardContentMap m_PlayerAwardContent;
	    //unsigned int  m_DbUpdateTime;	//记录上次数据库的更新时间
		//int m_DBIndicate;	//数据库指示器
		int goodsID;		//物品ID
		int limitBase;
		unsigned updateTime;
	};
}
