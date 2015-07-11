#include "player_email.h"
#include "playerManager.h"
#include "helper.h"
#include "action_format.hpp"
#include "record_system.h"
#include "gm_tools.h"

using namespace mongo;

namespace gg
{
	playerEmail::playerEmail(playerData& own) : Block(own)
	{
		logoutTime_ = 0;
	}

	void playerEmail::autoUpdate()
	{
// 		Json::Value msg;
// 		package(msg);
// 		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::email_update_resp, msg);
		

		unsigned num = 0;
		Json::Value updateJson;
		updateJson[msgStr][0u] = 1;
		vector <playerEmailItemPtr> allEmails;
		vector <int> allIDs;
		getAllEmail(allEmails, allIDs);
		updateJson[msgStr][1u][email::strHasNewEmail] = newEmailCheck();
		Json::Value& dataJson = updateJson[msgStr][1u][email::email_list];
		dataJson = Json::arrayValue;
		for (unsigned i = 0; i < allEmails.size(); ++i)
		{
			playerEmailItemPtr itemEmail = allEmails[i];
			if(itemEmail == NULL)continue;
			Json::Value singleDataJson;
			singleItemPackage(singleDataJson, itemEmail);
			dataJson.append(singleDataJson);
			++num;
			if(num >= 20)
			{
				own.sendToClient(gate_client::email_update_resp, updateJson);
//				cout << __FUNCTION__ << ",updateJson:" << updateJson.toStyledString() <<endl;
				dataJson = Json::arrayValue;
				num = 0;
			}
		}
		if(num > 0 || allEmails.size() == 0)
		{
			own.sendToClient(gate_client::email_update_resp, updateJson);
//			cout << __FUNCTION__ << ",updateJson:" << updateJson.toStyledString() <<endl;
		}

		Json::Value updateBase;
		updateBase[msgStr][0u] = 0;
		own.sendToClient(gate_client::email_update_resp, updateBase);
//		cout << __FUNCTION__ << ",updateBase:" << updateBase.toStyledString() <<endl;
	}

	bool playerEmail::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(email::emailDBStr, key);

		if(!obj.isEmpty())
		{
			//玩家
			mongo::BSONElement player_obj = obj[email::player_email_list];
			singleGet(player_obj, player_email_data.vecPlayerEmail);

			//系统普通
			mongo::BSONElement ord_obj = obj[email::ordinary_email_list];
			singleGet(ord_obj, player_email_data.vecSysOrdEmail);

			//系统附件
			mongo::BSONElement attach_obj = obj[email::attach_email_list];
			singleGet(attach_obj, player_email_data.vecSysAttachEmail);

			//系统战报
			mongo::BSONElement report_obj = obj[email::report_email_list];
			singleGet(report_obj, player_email_data.vecSysReportEmail);

			//保存
			mongo::BSONElement save_obj = obj[email::save_email_list];
			singleGet(save_obj, player_email_data.vecSaveEmail);

			//其他
			checkNotEoo(obj[email::strLogoutTime])
				logoutTime_ = obj[email::strLogoutTime].Int();

			return true;
		}

		return false;
	}

	void playerEmail::singleGet( mongo::BSONElement& sobj, vector<playerEmailItemPtr> &eItem )
	{
		vector<BSONElement> sets = sobj.Array();
		eItem.clear();
		for (unsigned i = 0; i < sets.size(); i++)
		{
			playerEmailItemPtr em_item_ptr = createPlayerEmailItemPtr();
			em_item_ptr->email_id = sets[i][email::email_id].Int();
			em_item_ptr->type = emailType(sets[i][email::type].Int());
			em_item_ptr->team = emailTeam(sets[i][email::team].Int());
			em_item_ptr->sender_id = sets[i][email::sender_id].Int();
			em_item_ptr->sender_name = sets[i][email::sender_name].String();
			em_item_ptr->send_time = sets[i][email::send_time].Int();

			Json::Reader reader;
			Json::Value jattach;
			checkNotEoo(sets[i][email::attachment])
			{
				vector<BSONElement> attach_sets = sets[i][email::attachment].Array();
				attachManager attchTemp;
				for (unsigned j = 0; j < attach_sets.size(); j++)
				{
					reader.parse(attach_sets[j].Obj().jsonString(), jattach);
					attchTemp.has_get = jattach[email::gift_has_get].asBool();
					attchTemp.attachAction = jattach[email::gift_action];
					attchTemp.packageName = jattach[email::gift_name].asString();
					attchTemp.packagePictureId = jattach[email::gift_picture_id].asInt();
					attchTemp.packageType = jattach[email::gift_type].asInt();
					em_item_ptr->attachments.push_back(attchTemp);
				}
			}

			Json::Value jcontent;
			checkNotEoo(sets[i][email::content])
			{
				reader.parse(sets[i][email::content].Obj().jsonString(), jcontent);
			}
			em_item_ptr->content = jcontent;

			eItem.push_back(em_item_ptr);
		}
	}

// 	bool playerEmail::saveJson()
// 	{
// 		Json::Value key;
// 		key[playerIDStr] = own.playerID;
// 		Json::Value obj;
// 
// 		obj[playerIDStr] = own.playerID;
// 		//玩家
// 		Json::Value player_obj;
// 		singleSaveJson(player_obj, player_email_data.vecPlayerEmail);
// 		obj[email::player_email_list] = player_obj;
// 
// 		//系统普通
// 		Json::Value ord_obj;
// 		singleSaveJson(ord_obj, player_email_data.vecSysOrdEmail);
// 		obj[email::ordinary_email_list] = ord_obj;
// 
// 		//系统附件
// 		Json::Value attach_obj;
// 		singleSaveJson(attach_obj, player_email_data.vecSysAttachEmail);
// 		obj[email::attach_email_list] = attach_obj;
// 
// 		//系统战报
// 		Json::Value report_obj;
// 		singleSaveJson(report_obj, player_email_data.vecSysReportEmail);
// 		obj[email::report_email_list] = report_obj;
// 
// 		//保存
// 		Json::Value save_obj;
// 		singleSaveJson(save_obj, player_email_data.vecSaveEmail);
// 		obj[email::save_email_list] = save_obj;
// 
// 		return db_mgr.save_mongo(email::emailDBStr, key, obj.obj());
// 	}

	bool playerEmail::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;

		obj << playerIDStr << own.playerID;

		//玩家
		mongo::BSONArrayBuilder player_obj;
		FalseReturn(singleSave(player_obj, player_email_data.vecPlayerEmail) == 0, 1);
		obj << email::player_email_list << player_obj.arr();

		//系统普通
		mongo::BSONArrayBuilder ord_obj;
		FalseReturn(singleSave(ord_obj, player_email_data.vecSysOrdEmail) == 0, 1);
		obj << email::ordinary_email_list << ord_obj.arr();

		//系统附件
		mongo::BSONArrayBuilder attach_obj;
		FalseReturn(singleSave(attach_obj, player_email_data.vecSysAttachEmail) == 0, 1);
		obj << email::attach_email_list << attach_obj.arr();

		//系统战报
		mongo::BSONArrayBuilder report_obj;
		FalseReturn(singleSave(report_obj, player_email_data.vecSysReportEmail) == 0, 1);
		obj << email::report_email_list << report_obj.arr();

		//保存
		mongo::BSONArrayBuilder save_obj;
		FalseReturn(singleSave(save_obj, player_email_data.vecSaveEmail) == 0, 1);
		obj << email::save_email_list << save_obj.arr();
		
		//其他
		obj << email::strLogoutTime << logoutTime_;

		return db_mgr.save_mongo(email::emailDBStr, key, obj.obj());
	}

	int playerEmail::singleSave( mongo::BSONArrayBuilder& em_arr, vector<playerEmailItemPtr> eItem )
	{
		try
		{
			BSONObjBuilder bsobj;
			for (unsigned i = 0; i < eItem.size(); i++)
			{
				playerEmailItemPtr email_item_ptr = eItem[i];
				mongo::BSONObjBuilder obj_item;
				obj_item << email::email_id << email_item_ptr->email_id;
				obj_item << email::type << email_item_ptr->type;
				obj_item << email::team << email_item_ptr->team;
				obj_item << email::sender_id << email_item_ptr->sender_id;
				obj_item << email::sender_name << email_item_ptr->sender_name;
				obj_item << email::send_time << email_item_ptr->send_time;

				if (email_item_ptr->content != Json::Value::null)
				{
// 					string tmp = email_item_ptr->content.toStyledString();
// 					tmp = commom_sys.tighten(tmp);
					string tmp = commom_sys.json2string(email_item_ptr->content);
					mongo::BSONObj obj_ct = mongo::fromjson(tmp);
					obj_item << email::content << obj_ct;
				}

				mongo::BSONArrayBuilder arr_at;
				for (unsigned j = 0; j < email_item_ptr->attachments.size(); j++)
				{
					attachManager& attachTemp = email_item_ptr->attachments[j];
					mongo::BSONObjBuilder obj_att;
					obj_att << email::gift_has_get << attachTemp.has_get;
					obj_att << email::gift_name << attachTemp.packageName;
					obj_att << email::gift_picture_id << attachTemp.packagePictureId;
					obj_att << email::gift_type << attachTemp.packageType;
					if (attachTemp.attachAction != Json::Value::null)
					{
						mongo::BSONArrayBuilder act_arr;
						for (unsigned idx_at = 0; idx_at < attachTemp.attachAction.size(); idx_at++)
						{
// 							string tmp = attachTemp.attachAction[idx_at].toStyledString();
// 							tmp = commom_sys.tighten(tmp);
							string tmp = commom_sys.json2string(attachTemp.attachAction[idx_at]);
							mongo::BSONObj obj_at = mongo::fromjson(tmp);
							act_arr << obj_at;
						}
						obj_att << email::gift_action << act_arr.arr();
					}
					arr_at << obj_att.obj();
				}

				obj_item << email::attachment << arr_at.arr();

				em_arr << obj_item.obj();
			}
		}
		catch (std::exception& e)
		{
			cout << e.what();
			return -1;
		}

		return 0;
	}

// 	void playerEmail::singleSaveJson(Json::Value& sjval, vector<playerEmailItem> eItem)
// 	{
// 		Json::Value em_arr;
// 		for (unsigned i = 0; i < eItem.size(); i++)
// 		{
// 			playerEmailItem& email_item = eItem[i];
// 			Json::Value obj_item;
// 			obj_item[email::email_id] = email_item.email_id;
// 			obj_item[email::type] = email_item.type;
// 			obj_item[email::team] = email_item.team;
// 			obj_item[email::sender_id] = email_item.sender_id;
// 			obj_item[email::sender_name] = email_item.sender_name;
// 			obj_item[email::send_time] = email_item.send_time;
// 
// 			if (email_item.content != Json::Value::null)
// 			{
// 				obj_item[email::content] = email_item.content;
// 			}
// 
// 			Json::Value obj_att;
// 			obj_att[email::gift_has_get] = email_item.attachment.has_get;
// 			if (email_item.attachment.attachAction != Json::Value::null)
// 			{
// 				Json::Value arr_at;
// 				for (unsigned idx_at = 0; idx_at < email_item.attachment.attachAction.size(); idx_at++)
// 				{
// 					arr_at.append(email_item.attachment.attachAction[idx_at]);
// 				}
// 				obj_att[email::gift_action] = arr_at;
// 			}
// 			obj_item[email::attachment] = obj_att;
// 
// 			em_arr.append(obj_item);
// 		}
// 
// 		sjval = em_arr;
// 	}

	void playerEmail::package(Json::Value& pack)
	{
		pack[msgStr][0u] = 0;

		//玩家
		singlePackage(pack[msgStr][1u][email::player_email_list], player_email_data.vecPlayerEmail);

		//系统普通
		singlePackage(pack[msgStr][1u][email::ordinary_email_list], player_email_data.vecSysOrdEmail);

		//系统附件
		singlePackage(pack[msgStr][1u][email::attach_email_list], player_email_data.vecSysAttachEmail);

		//系统战报
		singlePackage(pack[msgStr][1u][email::report_email_list], player_email_data.vecSysReportEmail);

		//保存
		singlePackage(pack[msgStr][1u][email::save_email_list], player_email_data.vecSaveEmail);

		pack[msgStr][1u][email::strHasNewEmail] = newEmailCheck();
	}

	void playerEmail::singlePackage( Json::Value& pack, vector<playerEmailItemPtr> eItem )
	{
		pack = Json::arrayValue;
		for (unsigned i = 0; i < eItem.size(); i++)
		{
			Json::Value& email_item = pack[i];
			singleItemPackage(pack[i], eItem[i]);
		}
	}

	void playerEmail::singleItemPackage(Json::Value& itemPack, playerEmailItemPtr eItem)
	{
		playerEmailItemPtr tempItem = eItem;
		itemPack[email::email_id] = tempItem->email_id;
		itemPack[email::type] = tempItem->type;
		itemPack[email::team] = tempItem->team;
		itemPack[email::sender_id] = tempItem->sender_id;
		itemPack[email::sender_name] = tempItem->sender_name;
		itemPack[email::send_time] = tempItem->send_time;
		if (tempItem->content != Json::Value::null)
		{
			itemPack[email::content] = tempItem->content;
		}
		itemPack[email::attachment] = Json::arrayValue;
		for (unsigned j = 0; j < tempItem->attachments.size(); j++)
		{
			attachManager& attachTemp = tempItem->attachments[j];
			Json::Value jtemp;
			jtemp[email::gift_action] = attachTemp.attachAction;
			jtemp[email::gift_has_get] = attachTemp.has_get;
			jtemp[email::gift_name] = attachTemp.packageName;
			jtemp[email::gift_picture_id] = attachTemp.packagePictureId;
			jtemp[email::gift_type] = attachTemp.packageType;
			itemPack[email::attachment].append(jtemp);
		}

	}

	int playerEmail::delEmail( int email_type, int email_id )
	{
		if (email_type <= email_type_begin || email_type >= email_type_end)
		{
			return -1;
		}

		if (email_type == email_type_player)
		{
			//玩家
			for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecPlayerEmail.begin();
				it != player_email_data.vecPlayerEmail.end(); it++)
			{
				if ((*it)->email_id == email_id)
				{
					player_email_data.vecPlayerEmail.erase(it);
					save();
					return 0;
				}
			}
		}
		else if (email_type == email_type_system_ordinary)
		{
			//普通系统邮件
			for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecSysOrdEmail.begin();
				it != player_email_data.vecSysOrdEmail.end(); it++)
			{
				if ((*it)->email_id == email_id)
				{
					player_email_data.vecSysOrdEmail.erase(it);
					save();
					return 0;
				}
			}
		}
		else if (email_type == email_type_system_attachment)
		{
			//附件系统邮件
			for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecSysAttachEmail.begin();
				it != player_email_data.vecSysAttachEmail.end(); it++)
			{
				if ((*it)->email_id == email_id)
				{
					player_email_data.vecSysAttachEmail.erase(it);
					save();
					return 0;
				}
			}
		}
		else if (email_type == email_type_system_battle_report)
		{
			//战报系统邮件
			for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecSysReportEmail.begin();
				it != player_email_data.vecSysReportEmail.end(); it++)
			{
				if ((*it)->email_id == email_id)
				{
					player_email_data.vecSysReportEmail.erase(it);
					save();
					return 0;
				}
			}
		}
		else
		{
		}
		
		return 1;
	}

	int playerEmail::delSaveEmail(int email_id)
	{
		//战报系统邮件
		for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecSaveEmail.begin();
			it != player_email_data.vecSaveEmail.end(); it++)
		{
			if ((*it)->email_id == email_id)
			{
				player_email_data.vecSaveEmail.erase(it);
				save();

				int cur_time = na::time_helper::get_current_time();
				Json::Value attachJson = Json::arrayValue;
				for (unsigned i = 0; i < (*it)->attachments.size(); i++)
				{
					attachJson.append((*it)->attachments[i].attachAction);
				}
				StreamLog::Log(email::mysqlLogEmail, own, cur_time - na::time_helper::timeZone() * 3600, attachJson.toStyledString(), log_email_del);

				return 0;
			}
		}
		return 1;
	}

	bool playerEmail::add_email_to_db_emaillist( playerEmailItemPtr email_item_ptr )
	{
		int etype = email_item_ptr->type;

		if (etype <= email_type_begin || etype >= email_type_end || email_item_ptr->email_id != 0)
		{
			return false;
		}
		
		email_item_ptr->email_id = genNewMailID();

		if (etype == email_type_player)
		{
			player_email_data.vecPlayerEmail.insert(player_email_data.vecPlayerEmail.begin(), email_item_ptr);
			if (player_email_data.vecPlayerEmail.size() > 25)
			{
				player_email_data.vecPlayerEmail.pop_back();
			}
		}
		else if (etype == email_type_system_ordinary)
		{
			player_email_data.vecSysOrdEmail.insert(player_email_data.vecSysOrdEmail.begin(), email_item_ptr);
//			player_email_data.vecSysOrdEmail.push_back(email_item);
			if (player_email_data.vecSysOrdEmail.size() > 40)
			{
				player_email_data.vecSysOrdEmail.pop_back();
			}
		}
		else if (etype == email_type_system_attachment)
		{
			player_email_data.vecSysAttachEmail.insert(player_email_data.vecSysAttachEmail.begin(), email_item_ptr);
			if (player_email_data.vecSysAttachEmail.size() > 50)
			{
				player_email_data.vecSysAttachEmail.pop_back();
			}
		}
		else if (etype == email_type_system_battle_report)
		{
			player_email_data.vecSysReportEmail.insert(player_email_data.vecSysReportEmail.begin(), email_item_ptr);
			if (player_email_data.vecSysReportEmail.size() > 30)
			{
				player_email_data.vecSysReportEmail.pop_back();
			}
		}
		else
		{
		}

		return save();
	}

	bool playerEmail::add_email_to_save_emaillist( playerEmailItemPtr email_item_ptr )
	{
		email_item_ptr->email_id = genNewMailID();
		player_email_data.vecSaveEmail.insert(player_email_data.vecSaveEmail.begin(), email_item_ptr);

		int mail_list_size = player_email_data.vecSaveEmail.size();
		if(mail_list_size > 30)
		{
			//delete the oldest mail
			player_email_data.vecSaveEmail.pop_back();
		}

		return save();
	}

	playerEmailItemPtr playerEmail::json2mail_item(Json::Value& jval)
	{
		playerEmailItemPtr em_item_ptr = createPlayerEmailItemPtr();
		em_item_ptr->email_id = jval[email::email_id].asInt();
		em_item_ptr->type = emailType(jval[email::type].asInt());
		em_item_ptr->team = emailTeam(jval[email::team].asInt());
		em_item_ptr->sender_id = jval[email::sender_id].asInt();
		em_item_ptr->sender_name = jval[email::sender_name].asString();
		em_item_ptr->send_time = jval[email::send_time].asInt();

		em_item_ptr->content = jval[email::content];

		return em_item_ptr;
	}

	void playerEmail::playerLogout()
	{
		logoutTime_ = na::time_helper::get_current_time();
		save();
	}

	bool playerEmail::newEmailCheck()
	{
		if (newEmailSingleCheck(player_email_data.vecPlayerEmail))
			return true;

		if (newEmailSingleCheck(player_email_data.vecSysOrdEmail))
			return true;

		if (newEmailSingleCheck(player_email_data.vecSysAttachEmail))
			return true;

		if (newEmailSingleCheck(player_email_data.vecSysReportEmail))
			return true;

		if (newEmailSingleCheck(player_email_data.vecSaveEmail))
			return true;

		return false;
	}

	bool playerEmail::newEmailSingleCheck( vector<playerEmailItemPtr> &eItem )
	{
		for (unsigned i = 0; i < eItem.size(); i++)
		{
			playerEmailItemPtr email_item_ptr = eItem[i];
			if (email_item_ptr->send_time > logoutTime_)
				return true;
		}
		
		return  false;
	}

	void playerEmail::getAllEmail( vector<playerEmailItemPtr> &allEmails, vector<int> &allIDs )
	{
		for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecPlayerEmail.begin();
			it != player_email_data.vecPlayerEmail.end(); it++)
		{
			allEmails.push_back(*it);
			allIDs.push_back((*it)->email_id);
		}

		for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecSysOrdEmail.begin();
			it != player_email_data.vecSysOrdEmail.end(); it++)
		{
			allEmails.push_back(*it);
			allIDs.push_back((*it)->email_id);
		}

		for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecSysAttachEmail.begin();
			it != player_email_data.vecSysAttachEmail.end(); it++)
		{
			allEmails.push_back(*it);
			allIDs.push_back((*it)->email_id);
		}

		for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecSysReportEmail.begin();
			it != player_email_data.vecSysReportEmail.end(); it++)
		{
			allEmails.push_back(*it);
			allIDs.push_back((*it)->email_id);
		}

		for (vector<playerEmailItemPtr>::iterator it = player_email_data.vecSaveEmail.begin();
			it != player_email_data.vecSaveEmail.end(); it++)
		{
			allEmails.push_back(*it);
			allIDs.push_back((*it)->email_id);
		}
	}

	int playerEmail::genNewMailID()
	{
		vector<playerEmailItemPtr> allEmails;
		vector <int> allIDs;
		getAllEmail(allEmails, allIDs);
		int count = 0;
		int ret = 0;

		int i = this->player_email_data.cur_email_id + 1;
		do 
		{
			if (i >= MAX_MAIL_ID)
				i = 1;

			vector<int>::iterator it = find(allIDs.begin(), allIDs.end(), i);
			if (it == allIDs.end())
			{
				ret = i;
				break;
			}

			count++;
			i++;
		} while (count < 500);

		if (count >= 500)
		{
			LogE << "get new mail id error!!" << LogEnd;
		}

		this->player_email_data.cur_email_id = ret;
		return ret;
	}

	attachManager::attachManager()
	{
		attachAction = Json::arrayValue;
		has_get = false;
		packageName = "";
		packagePictureId = 0;
		packageType = 0;
	}

	bool attachManager::isValid()
	{
		for (unsigned idx = 0; idx < attachAction.size(); idx++)
		{
			if (attachAction[idx][email::actionID].asInt() > 0)
			{
				return true;
			}
		}

		return false;
	}

	bool attachManager::getGift(playerDataPtr d)
	{
		Json::Value complexAction = gm_tools_mgr.simple2complex(attachAction);
		Json::Value Param;
		actionResult resAction = actionFormat::actionDo(d, complexAction, Param);
		if (resAction.resultCode != 0)
		{
			return false;
		}
		has_get = true;
		return true;
	}

	playerEmailData::playerEmailData()
	{
		cur_email_id = 0;
	}

	playerEmailItem::playerEmailItem()
	{
		this->email_id = 0;
		this->type = email_type_begin;
		this->team = email_team_null;
		this->sender_name = "";
		this->sender_id = 0;
		this->send_time = 0;
		attachManager attachment;
	}

// 	Json::Value attachManager::complex2simple(Json::Value& complexAction)
// 	{
// 		Json::Value simpleAction;
// 		return simpleAction;
// 	}
// 
// 	Json::Value attachManager::simple2complex(Json::Value& simpleAction)
// 	{
// 		Json::Value complexAction;
// 
// 		complexAction = Json::arrayValue;
// 		for (unsigned idx = 0; idx < simpleAction.size(); idx++)
// 		{
// 			if (!simpleAction[idx][email::actionID].isNull())
// 			{
// 				complexAction[idx][ACTION::strActionID] = simpleAction[idx][email::actionID].asInt();
// 			}
// 			if (!simpleAction[idx][email::value].isNull())
// 			{
// 				complexAction[idx][ACTION::strValue] = simpleAction[idx][email::value].asInt();
// 			}
// 			if (!simpleAction[idx][email::addNum].isNull())
// 			{
// 				complexAction[idx]["addNum"] = simpleAction[idx][email::addNum].asInt();
// 			}
// 			if (!simpleAction[idx][email::itemID].isNull())
// 			{
// 				complexAction[idx]["itemID"] = simpleAction[idx][email::itemID].asInt();
// 			}
// 		}
// 		return complexAction;
// 	}

}
