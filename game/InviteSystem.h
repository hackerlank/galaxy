#pragma once
#include <iostream>
#include <boost/unordered_map.hpp>
#include "json/json.h"
#include "msg_base.h"
#include "playerManager.h"

#define  MAX_SIZE_OF_CODE   20
#define  invite_sys (*gg::InviteSystem::playerInviteMgr)

using namespace std;


namespace gg{

	struct InviterReward
	{
		int id;
		int num;
		int	ltPeopleNum;
		Json::Value inviterReJson;
		InviterReward(){
			id = -1;
			num = 0;
			ltPeopleNum = 0;
		}

	};


	class InviteSystem
	{
	public:
		InviteSystem();
		~InviteSystem();
		void inintData();
		//根据key查找playerID
		int findPlayerID(string key);
		//更新邀请人的信息
		void updateInviterInfo(msg_json &m,Json::Value &r);
		//更新玩家信息
		void updatePlayerInviterInfo(msg_json &m,Json::Value &r);
		//输入邀请人的邀请码
		void inputInviteCode(msg_json &m,Json::Value &r);
		//领取奖励
		void getInviteReward(msg_json &m,Json::Value &r);
	public:
		static InviteSystem* const playerInviteMgr;
		static int reID;
	private:
		//得到数的位上的每个数 num:输入参数 eachNum: 输出参数
		void getNumberOfEach(int num,vector<int>& eachNum);
		//根据playerID生成邀请码
		//string produceInviteCode(int playerID);
		string produceInviteCode(int playerID);
		//把从客服端接收到的数字转成小写
		string UppercaseToLowercase(string str);
	private:
		const static char mapTable_char[2][10];
		const static char mapTable_num[7];
		//const static char fillCharacter[6];
		typedef boost::unordered_map<string, int> keyMap;
		keyMap key_map;
		typedef boost::unordered_map<int, InviterReward> InviterRewardMap;
		InviterRewardMap   ivReward;
	};

}