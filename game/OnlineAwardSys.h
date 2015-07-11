#pragma once
#include <boost/unordered_map.hpp>
#include "player_online_award.h"
#include "msg_base.h"
#include "mongoDB.h"
#include "file_system.h"
using namespace na::msg;


#define online_award_sys (*gg::OnlineAwardSys::onlineAwardMgr)

namespace gg {
	const static string award_content = "ac";	//奖励内容
	
	

	class OnlineAwardSys
	{
	public:
		static OnlineAwardSys* const onlineAwardMgr;
		OnlineAwardSys(void);
		~OnlineAwardSys(void);
		//得到奖励内容信息
		bool getAwardContent();
		//void UpdateAwardMsg(msg_json& m, Json::Value& r);
		//获取奖励
		void GetAward(msg_json& m, Json::Value& r);
		//重置奖励信息
		void ResetAwardMsg(msg_json& m,Json::Value& r);
		//初始化数据
		void initOnlineAward();
		//修改奖励内容
		void updateAwardMsg(msg_json& m,Json::Value&r);
		//启用新的奖励
		void startNewAward(msg_json& m,Json::Value&r);
		//保存奖励内容信息
		bool setAwardContent(vector<mongo::BSONElement> &els);
		//保存奖励内容信息
		bool setAwardContent(na::file_system::json_value_vec &els);
	private:
		AwardContentMap m_AwardContent;
		//int m_DBIndicate;
		double m_Ratio;
	};
}