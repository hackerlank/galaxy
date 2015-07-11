#include "email_system.h"
#include "player_data.h"
#include "playerManager.h"
#include "response_def.h"
#include "helper.h"
#include "record_system.h"
#include "task_system.h"
#include "guild_system.h"
#include "chat_system.h"
#include "action_def.h"
#include "player_guild.h"
#include "gm_tools.h"
#include "player_email.h"

namespace gg
{
	email_system* const email_system::emailSys = new email_system();

	email_system::email_system()
	{
	}

	email_system::~email_system()
	{
	}

	void email_system::initData()
	{
		db_mgr.ensure_index(email::emailDBStr, BSON(playerIDStr << 1));
	}

	void email_system::sendSystemEmailCommon( playerDataPtr d, emailType email_type, emailTeam email_team, Json::Value contentJson, Json::Value attachJson )
	{
		playerEmailItemPtr common_email = playerEmail::createPlayerEmailItemPtr();
		common_email->type = email_type;
		common_email->team = email_team;
		common_email->content = contentJson;

		if (attachJson != Json::Value::null)
		{
			attachManager attachMgr;
			attachMgr.attachAction = attachJson;
			attachMgr.packagePictureId = getAttachPictureID(attachJson);
			common_email->attachments.push_back(attachMgr);
		}

		sendToPlayer(d,common_email);
	}

	void email_system::sendSystemEmailByGm( playerDataPtr d, vector<attachManager>& attachs, Json::Value contentJson /*= Json::Value::null*/ )
	{
		playerEmailItemPtr gm_email = playerEmail::createPlayerEmailItemPtr();
		gm_email->type = email_type_system_attachment;
		gm_email->team = email_team_system_gm;
		gm_email->content = contentJson;
		if (attachs.size() > 0)
			gm_email->attachments = attachs;

		sendToPlayer(d,gm_email);
	}

	void email_system::sendPlayerEmailByPlayer(playerDataPtr sender, playerDataPtr receiver, Json::Value contentJson)
	{
		playerEmailItemPtr emailItem = playerEmail::createPlayerEmailItemPtr();
		emailItem->type = email_type_player;
		emailItem->team = email_team_player;
		emailItem->content = contentJson;
		emailItem->sender_name = sender->Base.getName();
		emailItem->sender_id = sender->playerID;

		sendToPlayer(receiver, emailItem);
		task_sys.updateBranchTaskInfo(sender, _task_email_times);
	}

	void email_system::sendPlayerEmailByGuildLeader(playerDataPtr sender, vector<playerDataPtr> receiver, Json::Value contentJson)
	{
		for (unsigned i = 0; i < receiver.size(); i++)
		{
			playerEmailItemPtr emailItem = playerEmail::createPlayerEmailItemPtr();;
			emailItem->type = email_type_player;
			emailItem->team = email_team_guild;
			emailItem->content = contentJson;
			emailItem->sender_name = sender->Base.getName();
			emailItem->sender_id = sender->playerID;
			sendToPlayer(receiver[i], emailItem);
		}
		task_sys.updateBranchTaskInfo(sender, _task_email_times);

	}

	int email_system::sendToPlayer( playerDataPtr d, playerEmailItemPtr mail_ptr, bool is_check_size /*= true*/ )
	{
		if (is_check_size)
		{
//			if (mail.isMember(email::content))
			{
// 				std::string content;
// 				if (mail[email::content].isObject())
// 				{
// 					content = 
// 				}
// 				else
// 				{
// 					content = mail[email::content].asString();
// 				}
// 				if (content.length() > MAX_MAIL_CONTENT_WORD_NUM)
// 					content = content.substr(0,MAX_MAIL_CONTENT_WORD_NUM);
// 				mail[email::content] = content;
			}
		}

		mail_ptr->send_time = (int)na::time_helper::get_current_time();

		int result = 0;

		//sent mail_update_notice
		Json::Value resl_notice_json;
		resl_notice_json = Json::arrayValue;//one email
		string s_notice = resl_notice_json.toStyledString();

		if (mail_ptr->team != email_team_system_mine_res_gain
			&& mail_ptr->team != email_team_system_mine_res_atk
			&& mail_ptr->attachments.size() > 0)
		{
			player_mgr.sendToPlayer(d->playerID, gate_client::mail_newMailNotify_resp, resl_notice_json);
		}

		if(!d->Email.add_email_to_db_emaillist(mail_ptr))
			result = -1;

		return result;
	}

	void email_system::mailUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Email.autoUpdate();
	}

	void email_system::writeToPlayerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;

		string receivePlayerNickName = js_msg[0u].asString();
		Json::Value leaverWordJson = js_msg[1u];

		playerDataPtr d_sender = player_mgr.getPlayerMain(m._player_id);
		playerDataPtr d_receive = player_mgr.getPlayerMainByName(receivePlayerNickName);
		if (d_sender == playerDataPtr() || d_receive == playerDataPtr())
		{
			if(m._post_times > 0)Return(r, 1);
			player_mgr.postMessage(m);
			return;
		}

		if (chat_sys.isForbidden(m._player_id))
			Return(r, 2);
		sendPlayerEmailByPlayer(d_sender, d_receive, leaverWordJson);

		Return(r, 0);
	}

	void email_system::writeToAllGuildMemberReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		Json::Value leaverWordJson = js_msg[0u];
		int guildID = d->Guild.getGuildID();
		Guild::guildPtr gPtr = guild_sys.getGuild(guildID);
		int official = gPtr->getOffcial(d->playerID);

		if (!(official >= Guild::guild_leader && official <= Guild::guild_old_member))
		{
			Return(r, 1);
		}

		if (chat_sys.isForbidden(m._player_id))
			Return(r, 2);

		if(official == Guild::guild_leader && d->Guild.getEmailTimes() > 19)Return(r, 2);
		if(official == Guild::guild_vice_leader && d->Guild.getEmailTimes() > 9)Return(r, 2);
		if(official == Guild::guild_old_member && d->Guild.getEmailTimes() > 4)Return(r, 2);

		vector<int> memList;
		gPtr->getAllMembers(memList);

		vector<playerDataPtr> memDataList;
		bool isFind = true;
		for (vector<int>::iterator it = memList.begin(); it != memList.end(); it++)
		{
			playerDataPtr pdata = player_mgr.getPlayerMain(*it);
			if (pdata == playerDataPtr())
			{
				isFind = false;
			}
			memDataList.push_back(pdata);
		}

		if (!isFind)
		{
			if (m._post_times > 1)
			{
				Return(r, -1);
			}
			player_mgr.postMessage(m);
			return;
		}

		sendPlayerEmailByGuildLeader(d, memDataList, leaverWordJson);
		d->Guild.alterEmailTimes(1);
		Return(r, 0);
	}

	//只能保存单个，不能保存多个了
	void email_system::saveMailReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		Json::Value mail_to_save = js_msg;

		if (!mail_to_save.isObject())
		{
			Return(r, -1);
		}
		r[msgStr][1] = mail_to_save[email::email_id].asInt();

		if (mail_to_save[email::attachment] != Json::Value::null && !mail_to_save[email::attachment][email::gift_has_get].asBool())
		{
			Return(r, 1);//有附件，不能保存
		}

		playerEmailItemPtr em_item;
		em_item = d->Email.json2mail_item(mail_to_save);
		d->Email.add_email_to_save_emaillist(em_item);
		Return(r, 0);
	}

	void email_system::getGiftReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int mail_id = js_msg[0u].asInt();

		vector<playerEmailItemPtr> ve = d->Email.player_email_data.vecSysAttachEmail;
		if (ve.size() == 0)
		{
			Return(r, 1);//没有该邮件
		}

		bool mail_in_list = false;
		vector<playerEmailItemPtr>::iterator it_gift = ve.begin();
		for (; it_gift != ve.end(); it_gift++)
		{
			if ((*it_gift)->email_id == mail_id)
			{
				mail_in_list = true;
				break;
			}
		}

		if (!mail_in_list)
		{
			Return(r, 1);//没有该邮件
		}

		if ((*it_gift)->attachments.size() == 0)
		{
			Return(r, 2);
		}

		for (unsigned i = 1; i < (*it_gift)->attachments.size(); i++)
		{
			for (unsigned j = 0; j < (*it_gift)->attachments[i].attachAction.size(); j++)
			{
				(*it_gift)->attachments[0].attachAction.append((*it_gift)->attachments[i].attachAction[j]);
			}
		}

		if (!(*it_gift)->attachments[0].isValid())
		{
			Return(r, 2);//该邮件没有附件
		}

		if ((*it_gift)->attachments[0].has_get)
		{
			Return(r, 3);//附件已经领取
		}

		bool ret = (*it_gift)->attachments[0].getGift(d);
		if (!ret)
		{
			Return(r, 4); //领取失败
		}
		
		vector<playerEmailItemPtr> veReal = d->Email.player_email_data.vecSysAttachEmail;
		vector<playerEmailItemPtr>::iterator it_gift_real = veReal.begin();
		for (; it_gift_real != veReal.end(); it_gift_real++)
		{
			if ((*it_gift_real)->email_id == mail_id)
			{
				(*it_gift_real)->attachments[0u].has_get = true;
				break;
			}
		}
		
		//玩家ID、时间、操作（领取附件）、附件内容。
		int cur_time = na::time_helper::get_current_time();
		StreamLog::Log(email::mysqlLogEmail, d, cur_time - na::time_helper::timeZone() * 3600, (*it_gift)->attachments[0].attachAction.toStyledString(), log_email_get_gift);

		(*it_gift_real)->type = email_type_system_ordinary;
		d->Email.add_email_to_db_emaillist(*it_gift);		
		ve.erase(it_gift);
		d->Email.save();
//		helper_mgr.insertSaveSet(&d->Email);
		Return(r, 0);
	}

	void email_system::delListReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		Json::Value mail_item = js_msg[0u];
		int ret = 0;

		if (!mail_item.isArray())
		{
			Return(r, -1);
		}
		for (unsigned i = 0; i < mail_item.size(); i++)
		{
			if (!mail_item[i].isArray())
			{
				Return(r, -1);
			}
			ret = d->Email.delEmail(mail_item[i][0u].asInt(), mail_item[i][1u].asInt());
		}
		
		Return(r, 0);
	}

	void email_system::delSavelistReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int mail_id = js_msg[0u].asInt();

		int ret = 0;
		ret = d->Email.delSaveEmail(mail_id);

		Return(r, ret);
	}

	void email_system::remove_json_member(Json::Value& jval, string mem)
	{
		if (jval.isArray())
		{
			Json::Value jret;
			unsigned idx = boost::lexical_cast<int, string>(mem);
			for (unsigned i = 0; i < jval.size(); i++)
			{
				if(i != idx)
				{
					jret.append(jval[i]);
				}
			}
			jval = jret;
		}
		else
		{
			jval.removeMember(mem);
		}
	}

	int email_system::getAttachPictureID( Json::Value attachJson)
	{
		int count = 0;
		int tempPicID;
		bool hasSecretary = false;
		for (unsigned i = 0; i < attachJson.size(); i++)
		{
			count++;
			int curActID = attachJson[i][email::actionID].asInt();
			if (curActID == action_add_item || curActID == action_rate_add_item)
			{
				if (count == 1)
				{
					tempPicID = attachJson[i][email::itemID].asInt();
				}
			}
			if (curActID == action_add_secretary)
			{
				hasSecretary = true;
			}
		}

		if (hasSecretary)
			return 15002;
		if (count == 1)
			return tempPicID;
		return 15001;
	}

	bool email_system::sendDoJumpFailEmail(playerDataPtr player, actionVector actionVect, vector<actionResult> &vecRetCode, emailTeam email_team, Json::Value &failSimpleJson)
	{
		bool ret = false;
		Json::Value complexJson = Json::arrayValue;
		Json::Value tempComplexJson = Json::arrayValue;
		failSimpleJson = Json::arrayValue;
		actionVector::iterator itAct = actionVect.begin();
		vector<actionResult>::iterator itCode = vecRetCode.begin();
		for (; itAct != actionVect.end(); itAct++,itCode++)
		{
			if (itCode->resultCode != 0)
			{
				complexJson.append((*itAct)->JsonData());
				ret = true;
			}
		}
		for (unsigned idx = 0; idx < complexJson.size(); idx++)
		{
			Json::Value tempAction = complexJson[idx];
			if (tempAction[ACTION::strActionID].asInt() == action_rate_add_item)
			{
				tempAction[ACTION::strActionID] = action_add_item;
			}
			tempComplexJson.append(tempAction);
		}
		if (tempComplexJson.size() > 0)
		{
			Json::Value simpleJson = gm_tools_mgr.complex2simple(tempComplexJson);
			email_sys.sendSystemEmailCommon(player, email_type_system_attachment, 
				email_team, Json::Value::null, simpleJson);
			failSimpleJson = simpleJson;
		}

		return ret;
	}

	bool email_system::sendDoJumpFailEmail(playerDataPtr player, Json::Value actionJson, vector<actionResult> &vecRetCode, emailTeam email_team, Json::Value &failSimpleJson)
	{
		if (!actionJson.isArray())
			return false;

		bool ret = false;
		Json::Value complexJson = Json::arrayValue;
		Json::Value tempComplexJson = Json::arrayValue;
		failSimpleJson = Json::arrayValue;
		Json::Value::iterator itAct = actionJson.begin();
		vector<actionResult>::iterator itCode = vecRetCode.begin();
		for (; itAct != actionJson.end(); itAct++,itCode++)
		{
			if (itCode->resultCode != 0)
			{
				complexJson.append(*itAct);
				ret = true;
			}
		}
		for (unsigned idx = 0; idx < complexJson.size(); idx++)
		{
			Json::Value tempAction = complexJson[idx];
			if (tempAction[ACTION::strActionID].asInt() == action_rate_add_item)
			{
				tempAction[ACTION::strActionID] = action_add_item;
			}
			tempComplexJson.append(tempAction);
		}
		if (tempComplexJson.size() > 0)
		{
			Json::Value simpleJson = gm_tools_mgr.complex2simple(tempComplexJson);
			email_sys.sendSystemEmailCommon(player, email_type_system_attachment, 
				email_team, Json::Value::null, simpleJson);
			failSimpleJson = simpleJson;
		}

		return ret;
	}

	void email_system::dealDoJumpFailLastDo(actionVector actionVec, vector<actionResult> &vecRetCode, Json::Value &lastDoJson)
	{
		Json::Value complexJson = Json::arrayValue;
		actionVector::iterator itAct = actionVec.begin();
		vector<actionResult>::iterator itCode = vecRetCode.begin();
		for (; itAct != actionVec.end(); itAct++, itCode++)
		{
			if (itCode->resultCode != 0)
			{
				complexJson.append((*itAct)->JsonData());
			}
		}
		for (unsigned idx = 0; idx < complexJson.size(); idx++)
		{
			Json::Value tempAction = complexJson[idx];
			if (tempAction[ACTION::strActionID].asInt() == action_rate_add_item)
			{
				tempAction[ACTION::strActionID] = action_add_item;
			}
			if (tempAction[ACTION::strActionID].asInt() == action_add_item)
			{
				Json::Value doJson;
				doJson.append(tempAction[ACTION::strActionID].asInt());
				doJson.append(tempAction[ACTION::addItemIDStr].asInt());
				doJson.append(tempAction[ACTION::addNumStr].asInt());
				lastDoJson.append(doJson);
			}
		}
	}

	void email_system::dealDoJumpFailLastDo(Json::Value actionJsonArr, vector<actionResult> &vecRetCode, Json::Value &lastDoJson)
	{
		Json::Value complexJson = Json::arrayValue;
		Json::Value::iterator itAct = actionJsonArr.begin();
		vector<actionResult>::iterator itCode = vecRetCode.begin();
		for (; itAct != actionJsonArr.end(); itAct++, itCode++)
		{
			if (itCode->resultCode != 0)
			{
				complexJson.append(*itAct);
			}
		}
		for (unsigned idx = 0; idx < complexJson.size(); idx++)
		{
			Json::Value tempAction = complexJson[idx];
			if (tempAction[ACTION::strActionID].asInt() == action_rate_add_item)
			{
				tempAction[ACTION::strActionID] = action_add_item;
			}
			if (tempAction[ACTION::strActionID].asInt() == action_add_item)
			{
				Json::Value doJson;
				doJson.append(tempAction[ACTION::strActionID].asInt());
				doJson.append(tempAction[ACTION::addItemIDStr].asInt());
				doJson.append(tempAction[ACTION::addNumStr].asInt());
				lastDoJson.append(doJson);
			}
		}
	}

}
