#include "paper_system.h"
#include "response_def.h"
#include "mongoDB.h"
#include "chat_system.h"
#include "record_system.h"
#include "timmer.hpp"

namespace gg
{
	const static string strPaperSysDB = "gl.paper_sys";
	const static string strActivityPaperDB = "gl.activity_paper";
	const static string strLogPaper = "log_paper_record";

	const static string strActPaperID = "id";
	const static string strActPaperName = "an";
	const static string strPaperList = "ls";
	const static string strDispatchNum = "dn";
	const static string strTotalToken = "tt";
	const static string strLeaveMessage = "lm";//m->lm
	const static string strLimitID = "lid";
	const static string strBroadcast = "b";//no->b
	const static string strActStartTime = "st";
	const static string strActEndTime = "et";
	const static string strPreBroadcast = "pb";//pno->pb
	const static string strHasAddPaper = "ha";
	const static string strActPaperKey = "k";

	PaperManager* const PaperManager::paperSys = new PaperManager();

	PaperManager::PaperManager()
	{
		duration_ = 20;
	}

	void PaperManager::update(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int updateID = js_msg[0u].asInt();
		if (updateID == 0)
			d->Paper.updateToken();
		else if (updateID == 1)
			d->Paper.updateRecv();
		else if (updateID == 2)
			d->Paper.updateHistory();
	}

	void PaperManager::updateRewardList(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		//获取key
		string key = js_msg[0u].asString();
		PAPER::DataPtr data = getPaper(key);
		if (data == NULL)Return(r, 1);//无效的红包key
		r[msgStr][1u] = data->list_toJson();
		Return(r, 0);
	}

	void PaperManager::delPaper(const string key)
	{
		PAPER::DataPtr data = getPaper(key);
		if (data == NULL)return;
		Papers.erase(key);
		if (!data->Invalid())
		{
			RankList();
		}
		db_mgr.remove_collection(strPaperSysDB, BSON("k" << key));
	}

	void PaperManager::invalidPaper(const string key)
	{
		PAPER::DataPtr data = getPaper(key);
		if (data == NULL)return;
		bool valid_before = !data->Invalid();
		data->Alive = false;
		if (valid_before)//如果之前是有效的
		{
			RankList();
		}
		savePaper(data);
	}

	void PaperManager::updatePaperTable(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		Json::Value& json = r[msgStr][1u] = Json::arrayValue;
		unsigned  num = 0;
		bool update = false;
		for (unsigned i = 0; i < rank.size(); ++i)
		{
			PAPER::DataPtr paper = rank[i];
			if (paper->Invalid())//红包消耗完毕, 或者过期
			{
				update = true;
				continue;
			}
			if (!paper->canRob(d))continue;//已经领取过或者不满足条件
			json.append(paper->toJson());
			++num;
			if (num > 29)break;
		}
		if (update)RankList();
		Return(r, 0);
	}

#define NullPlayer(PLAYER, CODE)\
	if (PLAYER == NULL)\
	{\
		if (m._post_times > 0)Return(r, CODE);\
		player_mgr.postMessage(m);\
		return;\
	}\

	void PaperManager::dispatchPaper(msg_json& m, Json::Value& r)
	{
		if (rank.size() > 10000)Return(r, 1);//服务器红包总量硬上限, 请稍后再试
		ReadJsonArray;
		if (js_msg.size() != 6)Return(r, -1);
		int chanelID = js_msg[0u].asInt();//频道ID
		if (chanelID < chat_all || chanelID > chat_player || chanelID == chat_area)Return(r, -1);
		int dispatchNum = js_msg[1u].asInt();//分派数量 <= 总红包数量
		if (dispatchNum < 1 || dispatchNum > 50)Return(r, -1);//参数错误
		int totalToken = js_msg[2u].asInt();//token总数
		if (totalToken < dispatchNum)Return(r, -1);//参数错误
		string message = js_msg[3u].asString();//留言
		string name = js_msg[4u].asString();//私聊人名字
		
		size_t len = commom_sys.UTF8Len(message);
		if (len < 1 || len > 30)Return(r, 2);//留言太长

		playerDataPtr player = player_mgr.getPlayerMain(m._player_id, m._net_id);
		playerDataPtr target = playerDataPtr();

		/////////////////////////////////////////////////////////////
		//确认单人还是多人
		if (chanelID == chat_player)
		{
			target = player_mgr.getPlayerMainByName(name);
			NullPlayer(target, 3);//私聊人不存在(不是不在线,是这个玩家根本不存在)
		}
		NullPlayer(player, -1);
		if (!player->Paper.canActivePaper())Return(r, 4);
		if (chanelID == chat_kingdom && !player->Base.SphereVaild())Return(r, 5);//没有加入势力
		if (chanelID == chat_guild && !player->Guild.hasJoinGuild())Return(r, 6);//没有加入公会
		//////////////////////////////////////////////////////////////////////////
		//数据读取完毕
		if (player->Vip.getVipLevel() < 1)Return(r, 7);//非vip
		if (player->Paper.getToken() < totalToken)Return(r, 8);//红包币不足

		//系统数据
		PAPER::DataPtr paper = PAPER::Create();
		if (paper == NULL)Return(r, -1);//
		unsigned now = na::time_helper::get_current_time();
		string key = createKey();
		if (key.empty())Return(r, 9);//发送红包太频繁(1是改时间导致的2快速发送和取消红包)
		paper->PaperKey = key;
		paper->faceID = player->Base.getFaceID();
		paper->subjectID = player->playerID;
		paper->Applicable = chanelID;
		paper->subjectName = player->Base.getName();
		paper->Leave = 0;
		paper->Total = dispatchNum;
		paper->Token = totalToken;
		paper->totalToken = totalToken;
		paper->Deadline = now + na::time_helper::DAY;
		paper->Message = message;
		if (chanelID == chat_player)paper->Param.append(target->playerID);
		else if (chanelID == chat_kingdom)paper->Param.append(player->Base.getSphereID());
		else if (chanelID == chat_guild)paper->Param.append(player->Guild.getGuildID());
		addPaper(paper);
		//个人数据
		PlayerPaper::Data pData;
		pData.key = key;
		pData.applicable = paper->Applicable;
		pData.leave = 0;
		pData.total = dispatchNum;
		pData.token = totalToken;
		pData.totalToken = totalToken;
		pData.time = now;
		player->Paper.dispatchRecord(pData);
		player->Paper.alterToken(-totalToken);
		//发送到聊天栏
		Json::Value PaperCast;
		PaperCast[msgStr][0u] = 0;
		PaperCast[msgStr][1u][senderChannelStr] = chanelID;
		PaperCast[msgStr][1u][chatBroadcastID] = BROADCAST::paper_notice;
		const static string ParamListStr = "pl";
		PaperCast[msgStr][1u][ParamListStr].append(player->Base.getName());
		PaperCast[msgStr][1u][ParamListStr].append(dispatchNum);
		PaperCast[msgStr][1u][ParamListStr].append(player->Base.getSphereID());
		PaperCast[msgStr][1u][ParamListStr].append(key);
		chat_sys.chatOption(chanelID, gate_client::chat_broadcast_resp, PaperCast, player, name);
		Return(r, 0);
	}

	void PaperManager::gmPaperUpdateReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int actID = js_msg[0u].asInt();

		Json::Value paperJson;
		for (vector<actPaperData>::iterator it = vecActPaperList_.begin(); it != vecActPaperList_.end(); it++)
		{
			if (actID == it->actID_)
			{
				paperJson = it->toJson();
				break;
			}
		}

		r[msgStr][1u] = paperJson;
		Return(r, 0);
	}

	void PaperManager::gmDispatchPaper(msg_json& m, Json::Value& r)
	{
		if (rank.size() > 10000)Return(r, 1);//服务器红包总量硬上限, 请稍后再试
		ReadJsonArray;

		Json::Value paperParam = js_msg[0u];

		int actID = paperParam[strActPaperID].asInt();//频道ID
		string actName = paperParam[strActPaperName].asString();//频道ID
		int chanelID = paperParam[senderChannelStr].asInt();//频道ID
		if (chanelID < chat_all || chanelID > chat_player)Return(r, -1);
		int dispatchNum = paperParam[strDispatchNum].asInt();//分派数量 <= 总红包数量
		if (dispatchNum < 1 || dispatchNum > 500)Return(r, 7);//参数错误
		int totalToken = paperParam[strTotalToken].asInt();//token总数
		if (totalToken < dispatchNum)Return(r, -1);//参数错误
		string message = paperParam[strLeaveMessage].asString();//留言
		int limitID = paperParam[strLimitID].asInt();//限制ID
		string name = paperParam[playerNameStr].asString();//私聊人名字
		string notice = paperParam[strBroadcast].asString();//布告

		int actStartTime = paperParam[strActStartTime].asInt();//开启时间
		int actEndTime = paperParam[strActEndTime].asInt();//结束时间
		string preNotice = paperParam[strPreBroadcast].asString();//预先滚动公告和聊天公告

		{
			size_t len = commom_sys.UTF8Len(notice);
			if (len < 1 || len > 20)Return(r, 5);//布告太长
		}

		{
			size_t len = commom_sys.UTF8Len(preNotice);
			if (len < 1 || len > 20)Return(r, 6);//预先公告太长
		}

		{
			size_t len = commom_sys.UTF8Len(message);
			if (len < 1 || len > 30)Return(r, 2);//留言太长
		}

		playerDataPtr target = playerDataPtr();

		/////////////////////////////////////////////////////////////
		//确认单人还是多人
		if (chanelID == chat_player)
		{
			target = player_mgr.getPlayerMainByName(name);
			NullPlayer(target, 3);//私聊人不存在(不是不在线,是这个玩家根本不存在)
		}

		actPaperData actData;
		actData.actID_ = actID;
		actData.actName_ = actName;
		actData.chanelID_ = chanelID;
		actData.dispatchNum_ = dispatchNum;
		actData.totalToken_ = totalToken;
		actData.leaveMessage_ = message;
		actData.limitID_ = limitID;
		actData.name_ = name;
		actData.broadcast_ = notice;
		actData.actStartTime_ = actStartTime + na::time_helper::timeZone() * 3600;
		actData.actEndTime_ = actEndTime + na::time_helper::timeZone() * 3600;
		actData.preBroadcast_ = preNotice;

		bool isfind = false;
		string tempKey;
		for (vector<actPaperData>::iterator it = vecActPaperList_.begin(); it != vecActPaperList_.end(); it++)
		{
			if (it->actID_ == actID)
			{
				tempKey = it->paperKey_;
				vecActPaperList_.erase(it);
				isfind = true;
				break;
			}
		}

		if (actStartTime == 0 && actEndTime == 0)
		{//这两个设置为0时表示删除该活动
			if (!isfind)
			{
				Return(r, 8);
			}
			else
			{
				string pkey = tempKey;
				if (pkey == "")
					Return(r, 0);
				PAPER::DataPtr data = getPaper(pkey);
				if (data == NULL)
					Return(r, 0);
				data->Alive = false;
				savePaper(data);
			}
		}
		else
		{
			vecActPaperList_.push_back(actData);
		}

		//不用这种定时器，因为事件id不好管理
// 		Timer::AddEventTickTime(boost::bind(&PaperManager::sendPreGmPaperBroadCast, this, actData), actData.actStartTime_ - 60 * 5);
// 		Timer::AddEventTickTime(boost::bind(&PaperManager::sendPreGmPaperBroadCast, this, actData), actData.actStartTime_ - 60 * 1);
// 		Timer::AddEventTickTime(boost::bind(&PaperManager::sendGmPaper, this, actData), actData.actStartTime_);
 		saveActivityPaper();
		Return(r, 0);
	}

	void PaperManager::DeleteOldPaper()
	{
		unsigned now = na::time_helper::get_current_time();
		for (PaperMap::iterator it = Papers.begin(); it != Papers.end(); )
		{
			PaperMap::iterator old = it;
			PAPER::DataPtr data = it->second;
			++it;
			if (now > data->Deadline && now - data->Deadline > 7776000)//3个月
			{
				Papers.erase(old);
				mongo::BSONObj key = BSON("k" << data->PaperKey);
				db_mgr.remove_collection(strPaperSysDB, key);
			}
		}
		unsigned tickTime = na::time_helper::get_time_zero(na::time_helper::get_current_time()) + na::time_helper::DAY + 10800;
		Timer::AddEventTickTime(boost::bind(&PaperManager::DeleteOldPaper, this), tickTime);
	}

	void PaperManager::scheduleUpdate(boost::system_time& sys_time)
	{
		if ((sys_time - tick_).total_seconds() < duration_)return;

		tick_ = sys_time;
		duration_ = 20;

		int cur_time = int(na::time_helper::get_current_time());
		
		bool hasModify = false;
		for (vector<actPaperData>::iterator it = vecActPaperList_.begin(); it != vecActPaperList_.end(); it++)
		{
			int starTime = it->actStartTime_;
			if (!it->sendFirstPreNotice_)
			{
				if (starTime - 6 * 60 + 30 <= cur_time && cur_time < starTime - 5 * 60)
				{
					duration_ = 1;
				}
				if (starTime - 5 * 60 <= cur_time && cur_time < starTime - 4 * 60)
				{
					sendPreGmPaperBroadCast(*it);
					it->sendFirstPreNotice_ = true;
				}
			}
			if (!it->sendSecondPreNotice_ && it->actStartTime_ - 1 * 60 <= cur_time && cur_time < it->actStartTime_)
			{
				sendPreGmPaperBroadCast(*it);
				it->sendSecondPreNotice_ = true;
				duration_ = 1;
			}
			if (!it->hasAddPaper_ && it->actStartTime_ <= cur_time && cur_time <= it->actEndTime_)
			{
				string s;
				na::msg::msg_json mj(gate_client::paper_inner_send_gm_paper_req, s);
				player_mgr.postMessage(mj);
			}
		}
	}

	void PaperManager::initActPaper()
	{
		readActivityPaper();
		refreshActivityPaper();
	}

	void PaperManager::refreshActivityPaper()
	{
		int curTime = na::time_helper::get_current_time();
		for (vector<actPaperData>::iterator it = vecActPaperList_.begin(); it != vecActPaperList_.end();)
		{
			if (curTime > it->actEndTime_)
			{
				it = vecActPaperList_.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	void PaperManager::readActivityPaper()
	{
		string strKey = "key";
		mongo::BSONObj key = BSON(strKey << strKey);
		mongo::BSONObj obj = db_mgr.FindOne(strActivityPaperDB, key);
		
		checkNotEoo(!obj[strPaperList])
			return;

		vecActPaperList_.clear();
		for (unsigned i = 0; i < obj[strPaperList].Array().size(); i++)
		{
			actPaperData pd;
			mongo::BSONElement singleObj = obj[strPaperList].Array()[i];
			checkNotEoo(singleObj[strActPaperID])
				pd.actID_ = singleObj[strActPaperID].Int();
			checkNotEoo(singleObj[strActPaperName])
				pd.actName_ = singleObj[strActPaperName].String();
			checkNotEoo(singleObj[strActPaperKey])
				pd.paperKey_ = singleObj[strActPaperKey].String();
			checkNotEoo(singleObj[strActStartTime])
				pd.actStartTime_ = singleObj[strActStartTime].Int();
			checkNotEoo(singleObj[strActEndTime])
				pd.actEndTime_ = singleObj[strActEndTime].Int();
			checkNotEoo(singleObj[senderChannelStr])
				pd.chanelID_ = singleObj[senderChannelStr].Int();
			checkNotEoo(singleObj[strDispatchNum])
				pd.dispatchNum_ = singleObj[strDispatchNum].Int();
			checkNotEoo(singleObj[strLimitID])
				pd.limitID_ = singleObj[strLimitID].Int();
			checkNotEoo(singleObj[strLeaveMessage])
				pd.leaveMessage_ = singleObj[strLeaveMessage].String();
			checkNotEoo(singleObj[playerNameStr])
				pd.name_ = singleObj[playerNameStr].String();
			checkNotEoo(singleObj[strBroadcast])
				pd.broadcast_ = singleObj[strBroadcast].String();
			checkNotEoo(singleObj[strPreBroadcast])
				pd.preBroadcast_ = singleObj[strPreBroadcast].String();
			checkNotEoo(singleObj[strTotalToken])
				pd.totalToken_ = singleObj[strTotalToken].Int();
			checkNotEoo(singleObj[strTotalToken])
				pd.hasAddPaper_ = singleObj[strHasAddPaper].Bool();

			vecActPaperList_.push_back(pd);
		}
	}

	void PaperManager::saveActivityPaper()
	{
		string strKey = "key";
		mongo::BSONObj key = BSON(strKey << strKey);
		mongo::BSONObjBuilder obj;

		obj << strKey << strKey;
		mongo::BSONArrayBuilder barr;
		for (unsigned i = 0; i < vecActPaperList_.size(); i++)
		{
			actPaperData pd = vecActPaperList_[i];
			mongo::BSONObjBuilder singleObj;
			singleObj << strActPaperID << pd.actID_;
			singleObj << strActPaperName << pd.actName_;
			singleObj << strActPaperKey << pd.paperKey_;
			singleObj << strActStartTime << pd.actStartTime_; 
			singleObj << strActEndTime << pd.actEndTime_;
			singleObj << senderChannelStr << pd.chanelID_;
			singleObj << strDispatchNum << pd.dispatchNum_;
			singleObj << strLimitID << pd.limitID_;
			singleObj << strLeaveMessage << pd.leaveMessage_;
			singleObj << playerNameStr << pd.name_;
			singleObj << strBroadcast << pd.broadcast_;
			singleObj << strPreBroadcast << pd.preBroadcast_;
			singleObj << strTotalToken << pd.totalToken_;
			singleObj << strHasAddPaper << pd.hasAddPaper_;
			barr << singleObj.obj();
		}
		obj << strPaperList << barr.arr();

		db_mgr.save_mongo(strActivityPaperDB, key, obj.obj());
	}

	void PaperManager::sendPreGmPaperBroadCast(actPaperData actData)
	{
		if (actData.preBroadcast_ != "")
		{
			LogI << __FUNCTION__ << ",begin to paper broadcast " << LogEnd;
			string senderWordsStr = "w";
			Json::Value contentJson;
			contentJson[senderWordsStr] = actData.preBroadcast_;
			if (actData.chanelID_ == chat_all)
			{
				chat_sys.sendToAllBroadCastCommon(BROADCAST::gm_chat_all, contentJson);
				chat_sys.sendToAllRollBroadCastCommon(ROLLBROADCAST::roll_gm_chat, 3, contentJson);
			}
			else if (actData.chanelID_ == chat_kingdom)
			{
				chat_sys.sendToSpereBroadCastCommon(actData.limitID_, BROADCAST::gm_chat_all, contentJson);
				chat_sys.sendToSpereRollBroadCastCommon(actData.limitID_, ROLLBROADCAST::roll_gm_chat, 3, contentJson);
			}
			else if (actData.chanelID_ == chat_guild)
			{
				chat_sys.sendToGuildBroadCastCommon(actData.limitID_, BROADCAST::gm_chat_all, contentJson);
				chat_sys.sendToGuildRollBroadCastCommon(actData.limitID_, ROLLBROADCAST::roll_gm_chat, 3, contentJson);
			}
			else if (actData.chanelID_ == chat_area)
			{
				chat_sys.sendToAreaBroadCastCommon(actData.limitID_, BROADCAST::gm_chat_all, contentJson);
				chat_sys.sendToAreaRollBroadCastCommon(actData.limitID_, ROLLBROADCAST::roll_gm_chat, 3, contentJson);
			}
		}
	}

	void PaperManager::innerSendGmPaperReq(msg_json& m, Json::Value& r)
	{
		bool isNullPlayer = false;
		for (vector<actPaperData>::iterator it = vecActPaperList_.begin(); it != vecActPaperList_.end(); it++)
		{
			actPaperData actData = *it;
			if (actData.chanelID_ != chat_player)
				continue;
			playerDataPtr target = player_mgr.getPlayerMainByName(actData.name_);
			if (target == playerDataPtr())
				isNullPlayer = true;
		}

		if (isNullPlayer)
		{
			if (m._post_times > 0)
				Return(r, -1);
			player_mgr.postMessage(m);
			return;
		}

		for (vector<actPaperData>::iterator it = vecActPaperList_.begin(); it != vecActPaperList_.end(); it++)
		{
			sendGmPaper(*it);
			it->hasAddPaper_ = true;
		}
		refreshActivityPaper();
		saveActivityPaper();
	}

	void PaperManager::sendGmPaper(actPaperData &actData)
	{
		//系统数据
		PAPER::DataPtr paper = PAPER::Create();
		if (paper == NULL)
		{
			LogE << __FUNCTION__ << "create paper failed" << LogEnd;
			return;
		}

		unsigned now = na::time_helper::get_current_time();
		string key = createKey();
		if (key.empty())
		{
			LogE << __FUNCTION__ << "createKey failed" << LogEnd;
			return;
		}
		playerDataPtr target = playerDataPtr();
		if (actData.chanelID_ == chat_player)
		{
			target = player_mgr.getPlayerMainByName(actData.name_);
			if (target == playerDataPtr())
			{
				LogE << __FUNCTION__ << "getPlayerMainByName failed" << LogEnd;
				return;
			}
		}
		actData.paperKey_ = key;
		paper->PaperKey = key;
		paper->Applicable = actData.chanelID_;
		paper->subjectName = "operation_name_0";
		paper->Leave = 0;
		paper->Total = actData.dispatchNum_;
		paper->Token = actData.totalToken_;
		paper->totalToken = actData.totalToken_;
		paper->Deadline = now + na::time_helper::DAY;
		paper->Message = actData.leaveMessage_;
		if (actData.chanelID_ == chat_player)paper->Param.append(target->playerID);
		else if (actData.chanelID_ == chat_kingdom)paper->Param.append(actData.limitID_);
		else if (actData.chanelID_ == chat_guild)paper->Param.append(actData.limitID_);
		addPaper(paper);

		//发送到聊天栏
		Json::Value PaperCast;
		PaperCast[msgStr][0u] = 0;
		PaperCast[msgStr][1u][senderChannelStr] = actData.chanelID_;
		PaperCast[msgStr][1u][chatBroadcastID] = BROADCAST::gm_paper_notice;
		const static string ParamListStr = "pl";
		PaperCast[msgStr][1u][ParamListStr].append(paper->subjectName);
		PaperCast[msgStr][1u][ParamListStr].append(actData.dispatchNum_);
		PaperCast[msgStr][1u][ParamListStr].append(actData.limitID_);
		PaperCast[msgStr][1u][ParamListStr].append(key);
		PaperCast[msgStr][1u]["an"] = actData.broadcast_;
		chat_sys.chatOption(actData.chanelID_, gate_client::chat_broadcast_resp, PaperCast, actData.limitID_, actData.name_);
	}

	void PaperManager::robPaper(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		string key = js_msg[0u].asString();
		PAPER::DataPtr paper = getPaper(key);
		if (paper == NULL || paper->Invalid())Return(r, 1);
		playerDataPtr player = player_mgr.getPlayerMain(m._player_id, m._net_id);
		playerDataPtr source = playerDataPtr();
		if (!paper->isSystem())source = player_mgr.getPlayerMain(paper->subjectID);
		if ( player == NULL || (!paper->isSystem() && source == NULL) )
		{
			if (m._post_times > 0)Return(r, -1);
			player_mgr.postMessage(m);
			return;
		}
		if (!paper->canRob(player))Return(r, 2);//领取过了
		int leave_token = paper->Token - paper->Total + paper->Leave;
		++paper->Leave;
		int loop = paper->Total - paper->Leave;
		leave_token = leave_token < 0 ? 0 : leave_token;
		double weight = commom_sys.randomBetween(1, 100);
		double total_weight = weight;
		for (int i = 0; i < loop; ++i)
		{
			total_weight += (double)commom_sys.randomBetween(1, 100);
		}
		int get_token = int( (weight / total_weight) * leave_token + 1.5 );//四舍五入
		get_token = (get_token > paper->Token || paper->LastOne()) ? paper->Token : get_token;
// 		int min_token = paper->empty() ? paper->Token : 1;
// 		int max_token = paper->Token - paper->Total + paper->Leave;
// 		int get_token = commom_sys.randomBetween(min_token, max_token);
		paper->Token -= get_token;
		player->Paper.alterToken(get_token);
		PAPER::Record record_sys(player, key, paper->Applicable);
		record_sys.reward = get_token;
		paper->RCList.push_back(record_sys);
		savePaper(paper);
		Json::Value& json = r[msgStr][1u];
		if (source != NULL)
		{
			source->Paper.robRecord(key, paper->Leave, paper->Token, get_token);
			PAPER::Record record(source, key, paper->Applicable);
			record.reward = get_token;
			player->Paper.recvRecord(record);

			Json::Value bCast;
			bCast[msgStr][0u] = 0;
			bCast[msgStr][1u][senderChannelStr] = chat_server;
			bCast[msgStr][1u][chatBroadcastID] = BROADCAST::paper_receive_notice;
			const static string ParamListStr = "pl";
			bCast[msgStr][1u][ParamListStr].append(player->Base.getName());
			bCast[msgStr][1u][ParamListStr].append(get_token);
			source->sendToClient(gate_client::chat_broadcast_resp, bCast);
		}
		else
		{
			PAPER::Record record;
			record.time = na::time_helper::get_current_time();
			record.name = paper->subjectName;
			record.reward = get_token;
			record.application = paper->Applicable;
			record.key = paper->PaperKey;
			player->Paper.recvRecord(record);
		}
		StreamLog::LogV(strLogPaper, player, get_token, paper->subjectName, 0,
			boost::lexical_cast<string, int>(paper->Token), boost::lexical_cast<string, int>(paper->Total - paper->Leave),
			paper->PaperKey);//领取红包
		json.append(paper->faceID);
		json.append(paper->subjectName);
		json.append(paper->Applicable);
		json.append(get_token);
		json.append(paper->Message);
		Return(r, 0);
	}

	void PaperManager::recoveryPaper(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int get_token = 0;
		for (unsigned i = 0; i < js_msg.size() && i < 10; ++i)
		{
			if (!js_msg[i].isString())break;
			string key = js_msg[i].asString();
			get_token += d->Paper.recoveryPaper(key);
		}
		if (get_token > 0)
		{
			d->Paper.updateHistory();
		}
		r[msgStr][2u] = js_msg.size();
		r[msgStr][1u] = get_token;
		Return(r, 0);
	}

	void PaperManager::paperSendagain(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		string key = js_msg[0u].asString();
		if (!d->Paper.checkKey(key))Return(r, 1);//不是你的连接不能发送
		PAPER::DataPtr data = getPaper(key);
		if (data == NULL)Return(r, 2);//红包不存在或者已经失效
		unsigned now = na::time_helper::get_current_time();
		if (d->Paper.CD > now)Return(r, 3);

		d->Paper.CD = now + 600;

		Json::Value PaperCast;
		PaperCast[msgStr][0u] = 0;
		PaperCast[msgStr][1u][senderChannelStr] = data->Applicable;
		PaperCast[msgStr][1u][chatBroadcastID] = BROADCAST::paper_notice;
		const static string ParamListStr = "pl";
		PaperCast[msgStr][1u][ParamListStr].append(d->Base.getName());
		PaperCast[msgStr][1u][ParamListStr].append(data->LeavePaperNum());
		PaperCast[msgStr][1u][ParamListStr].append(d->Base.getSphereID());
		PaperCast[msgStr][1u][ParamListStr].append(data->PaperKey);
		if (data->Applicable == chat_all)
		{
			player_mgr.sendToAll(gate_client::chat_broadcast_resp, PaperCast);
		}
		else if (data->Applicable == chat_kingdom)
		{
			player_mgr.sendToSphere(gate_client::chat_broadcast_resp, PaperCast, data->Param[0u].asInt());
		}
		else if (data->Applicable == chat_guild)
		{
			player_mgr.sendToGuild(gate_client::chat_broadcast_resp, PaperCast, data->Param[0u].asInt());
		}
		else if (data->Applicable == chat_player)
		{
			player_mgr.sendToPlayer(data->Param[0u].asInt(), gate_client::chat_broadcast_resp, PaperCast);
		}
		Return(r, 0);
	}

	void PaperManager::paperExchange(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int num = js_msg[0u].asInt();
		if (num < 1)Return(r, -1);
		if (num > d->Paper.getToken())Return(r, 1);//红包币不足以兑换

		d->Paper.alterToken(-num);
		d->Base.alterGoldTicket(num);
		r[msgStr][1u] = num;
		StreamLog::Log(strLogPaper, d, d->Paper.getToken(), num, 1);//兑换红包币
		Return(r, 0);
	}

	bool desc(PAPER::DataPtr d1, PAPER::DataPtr d2)
	{
		return d1->Deadline > d2->Deadline;
	}

	void PaperManager::initData()
	{
		db_mgr.ensure_index(strPaperSysDB, BSON("k" << 1));
		db_mgr.ensure_index(PlayerPaper::strPlayerPaperDB, BSON(playerIDStr << 1 << "tp" << 1));

		Papers.clear();

		objCollection objs = db_mgr.Query(strPaperSysDB);
		for (unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];
			PAPER::DataPtr data = PAPER::Create();
			if (data == NULL)abort();
			data->fomat(obj);
			Papers[data->PaperKey] = data;
		}
		RankList();
		initActPaper();

		unsigned tickTime = na::time_helper::get_time_zero(na::time_helper::get_current_time()) + na::time_helper::DAY + 10800;
		Timer::AddEventTickTime(boost::bind(&PaperManager::DeleteOldPaper, this), tickTime);
	}

	string PaperManager::createKey()
	{
		int tick = 0;
		unsigned now = na::time_helper::get_current_time();
		string time_key = boost::lexical_cast<string, unsigned>(now);
		string key = "";
		do 
		{
			string c_key = time_key + boost::lexical_cast<string, int>(commom_sys.randomBetween(1, 30000));
			if (getPaper(c_key) == NULL)
			{
				key = c_key;
				break;
			}
			++tick;
		} while (tick < 10);
		return key;
	}

	void PaperManager::addPaper(PAPER::DataPtr paper)
	{
		if (paper == NULL)return;
		Papers[paper->PaperKey] = paper;
		rank.insert(rank.begin(), paper);
		savePaper(paper);
	}

	void PaperManager::RankList()
	{
		rank.clear();
		for (PaperMap::iterator it = Papers.begin(); it != Papers.end(); ++it)
		{
			PAPER::DataPtr data = it->second;
			if (data->Invalid())continue;
			rank.push_back(data);
		}
		std::sort(rank.begin(), rank.end(), desc);
	}

	void PaperManager::savePaper(PAPER::DataPtr paper)
	{
		db_mgr.save_mongo(strPaperSysDB, BSON("k" << paper->PaperKey),
			paper->toBson());
	}

	PAPER::DataPtr PaperManager::getPaper(const string key)
	{
		PaperMap::iterator it = Papers.find(key);
		if (it == Papers.end())return PAPER::DataPtr();
		PAPER::DataPtr paper = it->second;
		return paper;
	}


	actPaperData::actPaperData()
	{
		hasAddPaper_ = false;
		sendFirstPreNotice_ = false;
		sendSecondPreNotice_ = false;
	}

	actPaperData::~actPaperData()
	{

	}

	Json::Value actPaperData::toJson()
	{
		Json::Value retJson;
		retJson[strActPaperID] = actID_;
		retJson[strActPaperName] = actName_;
		retJson[senderChannelStr] = chanelID_;
		retJson[strDispatchNum] = dispatchNum_;
		retJson[strTotalToken] = totalToken_;
		retJson[strLeaveMessage] = leaveMessage_;
		retJson[strLimitID] = limitID_;
		retJson[playerNameStr] = name_;
		retJson[strBroadcast] = broadcast_;
		retJson[strActStartTime] = actStartTime_;
		retJson[strActEndTime] = actEndTime_;
		retJson[strPreBroadcast] = preBroadcast_;

		return retJson;
	}

}
