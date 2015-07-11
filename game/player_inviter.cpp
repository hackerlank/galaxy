#include <iostream>
#include "player_inviter.h"
#include "mongoDB.h"
#include "playerManager.h"
#include "helper.h"
#include "InviteSystem.h"

using namespace mongo;
namespace gg{

	const int player_inviter::countLv = LIMIT_LV;
	const int player_inviter::maxPeopleNum = MAX_PEOPLE_NUM;
	RewardInfoMap player_inviter::gRewardInfo;					//全局变量
	player_inviter::player_inviter(playerData &own) :Block(own)
	{
		inviterID = -1;						//邀请人ID
		getMasonryNum = 0;					//得到的砖石数	
		totalMasonryNum = 0;				//总的砖石数
		playerNumOfMatch = 0;				//满足条件的玩家数
		alterFlag = false;						//更新标志
		inviteCode = "";					//邀请码
		//byInveiterID.clear();			//邀请的人ID
		rewardInfo.clear();			//奖励信息
		rewardInfo = player_inviter::gRewardInfo;
	}


	player_inviter::~player_inviter()
	{
	}

	void player_inviter::autoUpdate()
	{
		update();
	}

	bool player_inviter::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID;
		obj << inviterIDStr << inviterID;
		obj << inviteCodeStr << inviteCode;
		obj << getMasonryNumStr << getMasonryNum;
		obj << totalMasonryNumStr << totalMasonryNum;
		obj << playerNumOfNowStr << playerNumOfNow;
		obj << playerNumOfMatchStr << playerNumOfMatch;
		obj << alterFlagStr << alterFlag;
		//mongo::BSONArrayBuilder arry1;
		////byInveiterIDStr = "biid";
		//for (unsigned i = 0; i < byInveiterID.size();i++)
		//{
		//	arry1 << byInveiterID[i];
		//}
		//obj << byInveiterIDStr << arry1.arr();
		//rewardInfoStr = "rif";
		mongo::BSONArrayBuilder arry2;
		for (RewardInfoMap::iterator it = rewardInfo.begin(); it != rewardInfo.end(); it++)
		{
			mongo::BSONObjBuilder objite;
			objite << rewardIDStr << it->first;
			objite << getRewardNumStr << it->second;
			arry2.append(objite.obj());
		}
		obj << rewardInfoStr << arry2.arr();
		bool flag = false;
		return (flag = db_mgr.save_mongo(player_inviter_db_name, key, obj.obj()));
	}

	bool player_inviter::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(player_inviter_db_name, key);
		if (!obj.isEmpty()){
			inviterID = obj[inviterIDStr].Int();
			inviteCode = obj[inviteCodeStr].String();
			getMasonryNum = obj[getMasonryNumStr].Int();
			totalMasonryNum = obj[totalMasonryNumStr].Int();
			playerNumOfNow = obj[playerNumOfNowStr].Int();
			playerNumOfMatch = obj[playerNumOfMatchStr].Int();
			alterFlag = obj[alterFlagStr].Bool();
			/*vector<BSONElement> els1 = obj[byInveiterIDStr].Array();
			for (unsigned i = 0; i < els1.size;i++)
			{
			byInveiterID.push_back(els1[i].Int());
			}*/
			vector<BSONElement> els2 = obj[rewardInfoStr].Array();
			for (unsigned i = 0; i < els2.size(); i++){
				rewardInfo[els2[i][rewardIDStr].Int()] = els2[i][getRewardNumStr].Int();
			}
		}
		return true;
	}

	void player_inviter::update()
	{
		Json::Value msg;
		int nowGetMaNum = 0;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		msg[msgStr][1u][inviteCodeStr] = inviteCode;
		msg[msgStr][1u][playerNumOfMatchStr] = playerNumOfMatch;
		msg[msgStr][1u][totalMasonryNumStr] = totalMasonryNum;
		if ((nowGetMaNum = totalMasonryNum - getMasonryNum) > 0)
		{
			own.Base.alterGoldTicket(nowGetMaNum);
			getMasonryNum = totalMasonryNum;
			save();
		}
		msg[msgStr][1u][nowGetMasonryNumStr] = nowGetMaNum;
		msg[msgStr][1u][inviterIDStr] = inviterID;
		msg[msgStr][1u][rewardInfoStr] = Json::arrayValue;
		Json::Value &arr = msg[msgStr][1u][rewardInfoStr];
		int i = rewardInfo.size() - 1 -1;
		for (RewardInfoMap::iterator it = rewardInfo.begin(); it != rewardInfo.end(); it++)
		{
			if (it->first == InviteSystem::reID)
				continue;
			arr[i][rewardIDStr] = it->first;
			arr[i][getRewardNumStr] = it->second;
			i--;
		}
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::invite_sys_info_update_resp, msg);
		//LogW << msg.toStyledString() << LogEnd;
	}

	void player_inviter::packageReward(int reID, Json::Value& msg){
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		msg[msgStr][1u][rewardInfoStr] = Json::arrayValue;
		Json::Value &arr = msg[msgStr][1u][rewardInfoStr];
		RewardInfoMap::iterator it = rewardInfo.find(reID);
		if (it == rewardInfo.end())
			return;
		arr[0][rewardIDStr] = it->first;
		arr[0][getRewardNumStr] = it->second;
	}

	void player_inviter::setInviterID(int playerID)
	{
		inviterID = playerID;
		helper_mgr.insertSaveAndUpdate(this);
	}

	void player_inviter::updateInviterPeopleNumOfNowInfo(){
			//发送协议更新数据
		if (playerNumOfNow >= maxPeopleNum)
			return;
		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(inviterID);
		msg[msgStr].append(OpID::alterPeopleNumOfNow);
		msg[msgStr].append(1);		//更新数量
		string str = msg.toStyledString();
		na::msg::msg_json m(gate_client::inviter_info_update_oneself_update_req, str);
		player_mgr.postMessage(m);
	}

	void player_inviter::updateInviterPeopleNumOfMatchInfo()
	{
		if (own.Base.getLevel() >= player_inviter::countLv&&!alterFlag&&inviterID != -1)
		{
			alterFlag = true;
			save();
			//发送协议更新数据
			Json::Value msg;
			msg[msgStr] = Json::arrayValue;
			msg[msgStr].append(inviterID);
			msg[msgStr].append(OpID::alterPeopleNumOfMacth);
			msg[msgStr].append(1);		//更新数量
			string str = msg.toStyledString();
			na::msg::msg_json m(gate_client::inviter_info_update_oneself_update_req, str);
			player_mgr.postMessage(m);
		}
	}


	void player_inviter::updateInviterGetMasonryNum(int num)
	{
		//发送协议更新数据
		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(inviterID);
		msg[msgStr].append(OpID::alterMasonryNum);
		msg[msgStr].append(num);		//更新数量
		string str = msg.toStyledString();
		na::msg::msg_json m(gate_client::inviter_info_update_oneself_update_req, str);
		player_mgr.postMessage(m);
	}

	void player_inviter::alterPlayerNumOfNow(){
		playerNumOfNow++;
		save();
	}

	void player_inviter::alterPlayerNumOfMatch()
	{
		playerNumOfMatch++;
		save();
	}

	void player_inviter::alterTotalMasonryNum(int MasonryNum)
	{
		if (MasonryNum > 0){
			totalMasonryNum += MasonryNum;
			save();
		}

	}

	int player_inviter::getInviterID()
	{
		return inviterID;
	}

	int player_inviter::getTotalMasonryNum()
	{
		return totalMasonryNum;
	}


	void player_inviter::alterRewardInfo(int rewardID)
	{
		RewardInfoMap::iterator it = rewardInfo.find(rewardID);
		if (it != rewardInfo.end()&&it->second>0){
			it->second--;
			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	int player_inviter::getRewardInfo(int rewardID)
	{
		RewardInfoMap::iterator it = rewardInfo.find(rewardID);
		if (it != rewardInfo.end()){
			return it->second;
		}
		return -1;
	}

	int player_inviter::getPlayerNumOfMatch()
	{
		return playerNumOfMatch;
	}

	void player_inviter::setInviteCode(string code)
	{
		inviteCode = code;
		helper_mgr.insertSaveAndUpdate(this);
	}

	string player_inviter::getInviteCode(){
		return inviteCode;
	}

	bool player_inviter::numIsFull()
	{
		return (playerNumOfMatch >= maxPeopleNum);
	}


}