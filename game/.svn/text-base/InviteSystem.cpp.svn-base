#include "InviteSystem.h"
#include "email_system.h"
#include "response_def.h"
#include "player_inviter.h"
#include "action_instance.hpp"
#include "action_format.hpp"
#include <commom.h>


namespace gg{


	InviteSystem* const InviteSystem::playerInviteMgr = new InviteSystem();
	int InviteSystem::reID = -1;

	const char InviteSystem::mapTable_char[2][10] = { 
		{ 'b', 'e', 'c', 'a', 'h', 'd', 'k', 'm', 'j', 'f' },
		{ 't', 'u', 'x', 'p', 'r', 'q', 'w', 'v', 'y', 's' }
	};
	const char InviteSystem::mapTable_num[7] = { '2', '3', '4', '5', '6', '7', '8'};
	/*const char InviteSystem::fillCharacter[6] = {
		'k', 'l', 'm', 'n', 'o', 'p'
		};
		*/
	const static string  inviterDataDirStr = "./instance/Invitereward/";
	const static string  inviteRewardStr = "inviteReward";	//邀请奖励
	const static string	 shareRewardStr = "shareReward";	//分享奖励	
	const static string	 bindRewardStr = "bindReward";		//绑定奖励
	const static string  evalRewardStr = "evalReward";		//评价奖励
	const static string  inputInviteStr = "inputInvite";		//输入邀请码奖励
	void InviteSystem::inintData(){
		//读取配置文件得到奖励信息

		//数据库插入索引
		db_mgr.ensure_index(player_inviter_db_name, BSON(playerIDStr << 1));
		ivReward.clear();
		na::file_system::json_value_vec jv;
		na::file_system::load_jsonfiles_from_dir(inviterDataDirStr, jv);
		if (jv.size() != 0){
			for (unsigned int i = 0; i < jv.size(); i++)
			{
				InviterReward iv;
				iv.id = jv[i]["id"].asInt();
				iv.num = 1;
				iv.ltPeopleNum = jv[i]["lp"].asInt();
				iv.inviterReJson = jv[i]["itemJson"];
				ivReward[iv.id] = iv;
				//更新玩家奖励信息
				player_inviter::gRewardInfo[iv.id] = iv.num;
				if (jv[i]["sign"].asString() == inputInviteStr)
					InviteSystem::reID = iv.id;
			}
		}


		//导入邀请码-玩家ID对应的key_map表
		{
			objCollection objs = db_mgr.Query(player_inviter_db_name);
			for (unsigned i = 0; i < objs.size(); ++i)
			{
				mongo::BSONObj& obj = objs[i];
				if (!obj.isEmpty()){
					
					string IC = obj[inviteCodeStr].String();
					int PID = obj[playerIDStr].Int();
					//cout << "IC:" << IC << "PID" << PID << endl;
					key_map[IC] = PID;
				}
			}
		}
	}


	InviteSystem::InviteSystem()
	{
	}


	InviteSystem::~InviteSystem()
	{
	}

	//根据key查找playerID
	int InviteSystem::findPlayerID(string key){
		//int pID = 0;
		//if (key == "")
		//	return -1;
		//int length = key.length();
		//const char *cKey = key.c_str();
		//for (int i = key.length()-1; i >= 0; i--){
		//	if (cKey[i] == 'x'){
		//		length = i;
		//		break;
		//	}
		//}
		//if (length > MAX_SIZE_OF_CODE)
		//	return -1;
		////得到之前随机生成的数rNum
		//int rNum = cKey[10] - '0';
		//for (int i = 0; i < length-1;i++)
		//{
		//	//通过列找到对应的数字
		//	int n = 0;
		//	for (; n < 10; n++){
		//		if (cKey[i] == mapTable_char[i%2][n])
		//			break;
		//	}
		//	if (10 == n)
		//		return -1;
		//	if (n < rNum)
		//		pID = (n + 10) - rNum + pID * 10;
		//	else
		//		pID = n - rNum + pID * 10;
		//}
		//cout << "pID:" << pID;
		//return pID;	


		keyMap::iterator it = key_map.find(key);
		if (it == key_map.end())
			return -1;
		return key_map[key];
	}

	void InviteSystem::getNumberOfEach(int num,vector<int>& eachNum){
		eachNum.clear();
		while (num!=0){
			eachNum.push_back(num % 10);
			num = num / 10;
		}
	}

	//生成邀请码
	string InviteSystem::produceInviteCode(int playerID){
		string key;
		int randNum1 = commom_sys.randomBetween(1000000, 9999999);
		int randNum2 = commom_sys.randomBetween(100, 999);
		vector<int> randNumVec1, randNumVec2;
		getNumberOfEach(randNum1, randNumVec1);
		getNumberOfEach(randNum2, randNumVec2);
		char cKey[MAX_SIZE_OF_CODE];		//存储得到的字符
		bool flag = false;
		while (true)
		{
			//cout <<"randNum1:"<< randNum1 <<",randNum2"<< randNum2 << endl;
			memset(cKey, 0, sizeof(cKey));			
			if (randNumVec1.size() != 7 || randNumVec2.size() != 3)
				return "";
			for (unsigned i = 0; i < 10; i++){
				if (i < 7){
					cKey[i] = mapTable_char[i % 2][randNumVec1[i]];
				}
				else{
					cKey[i] = mapTable_num[randNumVec2[i % 3] % 7];
				}
			}
			cKey[10] = '\0';
			keyMap::iterator it = key_map.find(cKey);
			if (it == key_map.end()){
				key_map[cKey] = playerID;
				break;
			}
			if (flag){
				randNum1 = commom_sys.randomBetween(1000000, 9999999);
				getNumberOfEach(randNum1, randNumVec1);
				flag = !flag;
			}
			else{
				randNum2 = commom_sys.randomBetween(100, 999);
				getNumberOfEach(randNum2, randNumVec2);
				flag = !flag;
			}
		}
		return cKey;
	}

	//根据playerID生成邀请码
	//string InviteSystem::produceInviteCode(int playerID){
	//	////生成邀请码规则
	//	///*
	//	//	1.得到playerID的每一位上的数cKey[MAX_SIZE_OF_CODE]
	//	//	2.随机生成一个5位的randNum和个位的rNum，并得到randNum的每一位上的数randVec
	//	//	3.根据randVec[i]%3确定row,(cKey[i]+rNum)%10确定col
	//	//	4.根据row,col得到playerID每一位在mapTable中对应的值
	//	//	5.把得出来的数组成string存入key_map中
	//	//*/
	//	//int randNum = commom_sys.randomBetween(10000, 99999);
	//	//int rNum = commom_sys.randomBetween(2, 8);
	//	//vector<int> playerIDVec;
	//	//vector<int> randVec;
	//	//getNumberOfEach(playerID, playerIDVec);
	//	//getNumberOfEach(randNum, randVec);
	//	//char cKey[MAX_SIZE_OF_CODE];		//存储得到的字符
	//	//memset(cKey, 0, sizeof(cKey));
	//	//int length = playerIDVec.size();
	//	//reverse(playerIDVec.begin(),playerIDVec.end());
	//	//for (int i = 0; i < length; i++)
	//	//	cout << playerIDVec[i] << " ";
	//	//cout << endl;
	//	//for (unsigned i = 0; i <10; i++)
	//	//{
	//	//		if (i < length){
	//	//			if (i < 7){
	//	//				cKey[i] = mapTable_char[i%2][(playerIDVec[i] + rNum) % 10];
	//	//			}
	//	//			else{
	//	//				cKey[i] = mapTable_num[(playerIDVec[i]) % 7];
	//	//			}
	//	//		}
	//	//		else{
	//	//			if (i < 7){
	//	//				if (i == length)
	//	//					cKey[length] = 'x';
	//	//				else
	//	//					cKey[i] = mapTable_char[i%2][(playerIDVec[i / length] + rNum) % 10];
	//	//			}
	//	//			else{
	//	//				cKey[i] = mapTable_num[(playerIDVec[i / length]) % 7];
	//	//			}
	//	//		}			
	//	//}
	//	//
	//	//cKey[10] = rNum+'0';
	//	//cKey[11] = '\0';
	//	//return cKey;
	//	return "";
	//}

	//把从客服端接收到的数字转成小写
	string InviteSystem::UppercaseToLowercase(string str){
		const char *cKey = str.c_str();
		int length = str.length();
		if (length > MAX_SIZE_OF_CODE)
			return "";
		char cStr[MAX_SIZE_OF_CODE];
		memset(cStr, 0, sizeof(cStr));
		for (int i = 0; i < length; i++){
			if (cKey[i] >= 'A'&&cKey[i] <= 'Z')
				cStr[i] = cKey[i] - ('A' - 'a');
			else
				cStr[i] = cKey[i];
			if (!((cStr[i]>='0' && cStr[i]<='9') || (cStr[i]>='a' && cStr[i]<='z')))
				return "";
		}
		cStr[length] = '\0';
		return cStr;
	}

	void InviteSystem::updateInviterInfo(msg_json &m, Json::Value &r)
	{
		if (m._net_id >= 0)return;
		ReadJsonArray;
		int inviterID = js_msg[0].asInt();
		int opType = js_msg[1].asInt();
		int alterNum = js_msg[2].asInt();
		//找到玩家
		playerDataPtr d = player_mgr.getPlayerMain(inviterID);
		if (d == playerDataPtr()){
			if (m._post_times < 2)
				player_mgr.postMessage(m);
			return;
		}
		if (opType == OpID::alterPeopleNumOfNow){
			d->inviter.alterPlayerNumOfNow();
		}
		if (opType == OpID::alterPeopleNumOfMacth){
			d->inviter.alterPlayerNumOfMatch();
		}
		if (opType == OpID::alterMasonryNum){
			d->inviter.alterTotalMasonryNum(alterNum);
		}
	}

	void InviteSystem::updatePlayerInviterInfo(msg_json &m, Json::Value &r)
	{
		AsyncGetPlayerData(m);
		//是否有邀请码，没有则生成
		if (d->inviter.getInviteCode() == "")
		{
			//生成邀请码
			d->inviter.setInviteCode(produceInviteCode(d->playerID));
		}
		d->inviter.update();
		//Return(r, 0);
	}


	void InviteSystem::inputInviteCode(msg_json &m, Json::Value &r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		string ivCodeTemp = js_msg[0].asString();
		if (ivCodeTemp.size() != 10)
			Return(r, -1);
		string ivCode = UppercaseToLowercase(ivCodeTemp);
		int inviterID = 0;
		//已有邀请人，返回1
		if (d->inviter.getInviterID() != -1)
			Return(r, 1);
		//邀请码为空
		if ((inviterID = findPlayerID(ivCode)) == -1)
			Return(r, -1);
		//输入的是自己
		if (inviterID == d->playerID)
			Return(r, 2);
		//LogW << "inviterID:" << inviterID << LogEnd;
		//根据ID找到玩家,绑定邀请人
		playerDataPtr iv = player_mgr.getPlayerMain(inviterID);
		if (iv == playerDataPtr()){
			if (m._post_times < 1){
				player_mgr.postMessage(m);
				return;
			}
			else
				Return(r, 3);				//不存在该玩家
		}
		//对方人数已满，返回3
		if (iv->inviter.numIsFull())
			Return(r, 4);
		d->inviter.setInviterID(inviterID);
		d->inviter.updateInviterPeopleNumOfNowInfo();
		d->inviter.updateInviterPeopleNumOfMatchInfo();
	
		//领取绑定奖励
		InviterRewardMap::iterator it = ivReward.find(InviteSystem::reID);
		if (it == ivReward.end())
			Return(r, -1);			//非法操作
		//找到玩家奖励信息
		if (d->inviter.getRewardInfo(InviteSystem::reID) <= 0)
			Return(r, 5);			//已经领取过该奖励
		Json::Value param;
		vector<actionResult> vecRetCode = actionFormat::actionDoJump(d, it->second.inviterReJson, param);
		Json::Value failSimpleJson;
		email_sys.sendDoJumpFailEmail(d, it->second.inviterReJson, vecRetCode, email_team_system_bag_full_online, failSimpleJson);
		for (unsigned i = 0; i < vecRetCode.size(); i++)
		{
			if (vecRetCode[i].resultCode != 0)
				Return(r, 6);		//背包空间不够，发送邮件
		}
		//更新玩家奖励信息
		d->inviter.alterRewardInfo(InviteSystem::reID);
		r[msgStr][1u] = it->second.inviterReJson;
		//d->inviter.packageReward(InviteSystem::reID, r);

		Return(r, 0);
	}


	void InviteSystem::getInviteReward(msg_json &m, Json::Value &r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int reId = js_msg[0].asInt();
		InviterRewardMap::iterator it = ivReward.find(reId);
		if (it == ivReward.end())
			Return(r, -1);			//非法操作
		//找到玩家奖励信息
		if (d->inviter.getRewardInfo(reId) <= 0)
			Return(r, 1);			//已经领取过该奖励
		if (d->inviter.getPlayerNumOfMatch() < ivReward[reId].ltPeopleNum)
			Return(r, 2);			//人数不满足条件
		//领取奖励d
		Json::Value param;
		vector<actionResult> vecRetCode = actionFormat::actionDoJump(d, it->second.inviterReJson, param);
		Json::Value failSimpleJson;
		email_sys.sendDoJumpFailEmail(d, it->second.inviterReJson, vecRetCode, email_team_system_bag_full_online, failSimpleJson);
		for (unsigned i = 0; i < vecRetCode.size(); i++)
		{
			if (vecRetCode[i].resultCode != 0)
				Return(r,3);		//背包空间不够，发送邮件
		}
		//更新玩家奖励信息
		d->inviter.alterRewardInfo(reId);
		Return(r, 0);
	}

}