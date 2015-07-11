#include "gm_tools.h"
#include "response_def.h"
#include "playerManager.h"
#include "action_def.h"
#include "action_instance.hpp"
#include "action_format.hpp"
#include "block.h"
#include "player_campaign.h"
#include "guild_system.h"
#include "study_system.h"
#include "email_system.h"
#include "vip_system.h"
#include "record_system.h"
#include "service_config.h"
#include "explore_manager.h"

using namespace mongo;

namespace gg
{
	gm_tools* const gm_tools::gmMgr = new gm_tools();

	const static int exp_medic_id = 14035;
	gm_tools::gm_tools()
	{

	}

	gm_tools::~gm_tools()
	{

	}

	void gm_tools::playerBaseInfoUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		string str_player_base = "pb";
		string str_player_building = "pbd";
		string str_player_cd = "pcd";
		string str_player_science = "psc";

		Json::Value jresp;

		//基本信息
		Json::Value& jbase = jresp[str_player_base];
		jbase[playerIDStr] = d->playerID;
		jbase[playerNameStr] = d->Base.getName();
		jbase[playerLevelStr] = d->Base.getLevel();
		jbase[playerSphereIDStr] = d->Base.getSphereID();
		jbase[playerGoldStr] = d->Base.getGold();
		jbase[playerGoldTicketStr] = d->Base.getGoldTicket();
		jbase[playerSilverStr] = d->Base.getSilver();
		jbase[playerKejiStr] = d->Base.getKeJi();
		jbase[playerWeiWangStr] = d->Base.getWeiWang();
		jbase[office::playerOfficialLevelStr] = d->Office.getOffcialLevel();
		jbase[playerAreaIDStr] = d->Base.getPosition().first;
		jbase["jl"] = d->Campaign.getJunling();
		jbase[playerExpStr] = d->Base.getExp();
		jbase[playerPopulationStr] = d->Base.getCalPopu();
		jbase[point_def::strMyPoint] = d->Points.getMyPoint();
		jbase[vip::strRechargeGold] = d->Vip.getRechargeGold();
		jbase["tk"] = d->Paper.getToken();

		explore_sys.refreshPlayerData(d);
		jbase[ordinary_materials] = d->playerExplore.getOrdinaryMaterials();
		jbase[hq_materials] = d->playerExplore.getHQMaterials();

		Json::Value guild_info = guild_sys.getPersonal(d);
		string guild_name = "gna";
		jbase[guild_name] = guild_info[guildNameStr];
		jbase[memberContributeStr] = guild_info[memberContributeStr];

		string str_pilot_num = "pn";
		jbase[str_pilot_num] = 12;//舰长数,待定
		string str_exp_medicine = "em";
		jbase[str_exp_medicine] = d->Item.getItemStack(exp_medic_id);
// 		playerItemPtr exp_medic = d->Ware.getItem(5999);
// 		if (exp_medic == playerItemPtr())
// 		{
// 			jbase[str_exp_medicine] = 0;
// 		}
// 		else
// 		{
// 			jbase[str_exp_medicine] = exp_medic->getStack();
// 		}

		//建筑信息
// 		Json::Value& jbuilding = jresp[str_player_building];
// 		for (playerBuilds::buildsMap::iterator it = d->Builds.builds.begin();
// 			it != d->Builds.builds.end(); it++)
// 		{
// 			Json::Value temp;
// 			temp["id"] = it->second.buildID;
// 			temp["lv"] = it->second.buildLV;
// 
// 			jbuilding.append(temp);
// 		}

		//科技信息
		Json::Value& jscience = jresp[str_player_science];
		for(playerScience::scienceMaps::iterator it = d->Science.science_maps.begin(); it != d->Science.science_maps.end(); ++it)
		{
			Json::Value temp;
			temp["id"] = it->second.id;
			temp["lv"] = it->second.lv;

			jscience.append(temp);
		}

		//CD信息 
 		Json::Value& jcd = jresp[str_player_cd];//玩家cd
 		jcd["b"] = Json::arrayValue;//建筑系统取消
// 		for (playerCDList::CDMap::iterator it = d->CDList.ownMap.begin(); it != d->CDList.ownMap.end(); ++it)
// 		{
// 			CDData  cCD = it->second.getCD();
// 			jcd["b"].append(cCD.CD);
// 		}

		string str_migrate = "mg";//迁移cd
		jcd[str_migrate] = 0;

		string str_equip_upgrade = "eu";//强化cd
		jcd[str_equip_upgrade] = d->Item.getCD().CD;

// 		string str_shop = "sp";//商店cd
// 		jcd[str_shop] = d->Shop.getBuyTimes(,);

		string str_levy = "lv";//征收cd
		jcd[str_levy] = d->Levy.getLevyCD().Cdd.CD;

		string str_pilot_upgrade = "pu";//突飞cd
		jcd[str_pilot_upgrade] = d->Pilots.getCD().CD;

		string str_campaign = "cp";//征战cd
		jcd[str_campaign] = d->Campaign.getCD().Cdd.CD;

		string str_artil = "at";//主炮研究cd
		delegateStudy& stu_artil = d->Study.getDeleStu(ARTIL);
		jcd[str_artil] = stu_artil.delegateCD.Cdd.CD;

		string str_shield = "sl";//护盾研究cd
		delegateStudy& stu_shield = d->Study.getDeleStu(SHIELD);
		jcd[str_shield] = stu_shield.delegateCD.Cdd.CD;

		r[msgStr][1u] = jresp;

		Return(r, 0);
	}

	void gm_tools::modifyPlayerBaseInfoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		Json::Value req = js_msg[0u];

		string str_player_base = "pb";
		string str_player_building = "pbd";
		string str_player_cd = "pcd";
		string str_player_science = "psc";
		if (!req[str_player_base].isNull())
		{
			Json::Value temp_req = req[str_player_base];
			if (!temp_req[playerGoldStr].isNull())
			{
				d->Base.alterGold(temp_req[playerGoldStr].asInt());
			}
			if (!temp_req[playerGoldTicketStr].isNull())
			{
				d->Base.alterGoldTicket(temp_req[playerGoldTicketStr].asInt());
			}
			if (!temp_req[playerSilverStr].isNull())
			{
				d->Base.alterSilver(temp_req[playerSilverStr].asInt());
			}
			if (!temp_req[playerKejiStr].isNull())
			{
				d->Base.alterKeJi(temp_req[playerKejiStr].asInt());
			}
			if (!temp_req[playerWeiWangStr].isNull())
			{
				d->Base.alterWeiWang(temp_req[playerWeiWangStr].asInt());
			}
			if (!temp_req[playerExpStr].isNull())
			{
				d->Base.addExp(temp_req[playerExpStr].asInt());
			}
			if (!temp_req[playerPopulationStr].isNull())
			{
				d->Base.alterPopulation(temp_req[playerPopulationStr].asInt());
			}
			if (!temp_req[point_def::strMyPoint].isNull())
			{
				d->Points.alterMyPoint(temp_req[point_def::strMyPoint].asInt());
			}
			if (!temp_req["tk"].isNull())
			{
				d->Paper.alterToken(temp_req["tk"].asInt());
			}
			explore_sys.refreshPlayerData(d);
			if (!temp_req[ordinary_materials].isNull())
			{
				d->playerExplore.alterOrdinaryMaterials(temp_req[ordinary_materials].asInt());
			}
			if (!temp_req[hq_materials].isNull())
			{
				d->playerExplore.alterHQMaterials(temp_req[hq_materials].asInt());
			}

			if (!temp_req[vip::strRechargeGold].isNull())
			{
				vector<string> fds;
				int previousRechargeGold = d->Vip.getRechargeGold();
				d->Vip.alterRechargeGold(temp_req[vip::strRechargeGold].asInt());
				StreamLog::Log(vip::MysqlLogChargeGold, d, boost::lexical_cast<string, int>(previousRechargeGold), 
					boost::lexical_cast<string, int>(d->Vip.getRechargeGold()), fds, vip_log_gm_modify);
			}

			string str_exp_medicine = "em";
			if (!temp_req[str_exp_medicine].isNull())
			{
				d->Item.addItem(exp_medic_id, temp_req[str_exp_medicine].asInt());
			}

			//舰长数
			//

			string str_junling = "jl";
			if (!temp_req[str_junling].isNull())
			{
				d->Campaign.alterJunling(temp_req[str_junling].asInt());
			}

			//添加一个修改团贡的接口
			if (!temp_req[memberContributeStr].isNull())
			{//修改的是星梦等级
				guild_sys.donate(d, 2, temp_req[memberContributeStr].asInt());
			}
		}

		if (!req[str_player_cd].isNull())
		{
// 			Json::Value temp_req = req[str_player_cd];
// 
// 			string str_migrate = "mg";
// 			if (!temp_req[str_migrate].isNull())
// 			{
// 			}
// 
// 			string str_equip_upgrade = "eu";
// 			if (!temp_req[str_equip_upgrade].isNull())
// 			{
// 
// 			}
		}

		if (!req[str_player_science].isNull())
		{
			Json::Value temp_req = req[str_player_science];

			for (unsigned idx = 0; idx < temp_req.size(); idx++)
			{
				int scid = temp_req[idx]["id"].asInt();
				int sclv = temp_req[idx]["lv"].asInt();

				d->Science.set_level(scid, sclv);
			}

			helper_mgr.insertSaveAndUpdate(&d->Science);
		}

		Return(r, 0);
	}

	void gm_tools::sendResourceToPlayerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		Json::Value req_players = js_msg[0u];
		Json::Value req_resource = js_msg[1];

		vector<playerDataPtr> vecPlayer;

		Json::Value retErr;
		bool is_find_player = true;

		if (req_players[0].asInt() == -999)
		{
			vecPlayer = player_mgr.onlinePlayer();
		}
		else
		{
			for (unsigned i = 0; i < req_players.size(); i++)
			{
				playerDataPtr d = player_mgr.getPlayerMain(req_players[i].asInt());

				if (d == playerDataPtr())
				{
					is_find_player = false;
					retErr.append(req_players[i].asInt());
				}
				else
				{
					vecPlayer.push_back(d);
				}
			}

			if (!is_find_player)
			{
				if(m._post_times > 0)
				{
					r[msgStr][1] = retErr;
					Return(r, 1);
				}
				player_mgr.postMessage(m);
				return;
			}
		}

		Json::Value complexAction = simple2complex(req_resource);
		for (unsigned i = 0; i < req_players.size(); i++)
		{
			actionResult resAction = actionFormat::actionDo(vecPlayer[i], complexAction);

			if (resAction.resultCode != 0)
			{
				Json::Value mail;
				for (unsigned j = 0; j < req_resource.size(); j++)
				{
					if (req_resource[j][email::actionID].asInt() >= resAction.breakID)
					{
						mail.append(req_resource[j]);
					}
				}
				email_sys.sendSystemEmailCommon(vecPlayer[i], email_type_system_attachment, email_team_system_bag_full, Json::Value::null, mail);
			}
		}

		Return(r, 0);
	}

	void gm_tools::gmSendGiftCardToPlayerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		Json::Value req_players = js_msg[0u];
		Json::Value req_resource = js_msg[1];
		string strCard = js_msg[2].asString();

		vector<playerDataPtr> vecPlayer;

		bool is_find_player = true;

		r[msgStr][1] = strCard;
		r[msgStr][2] = req_resource;
		r[msgStr][3][0u] = 0;
		r[msgStr][3][1] = 0;
		if (req_players.size() > 1)
		{
			Return(r, -1);
		}

		for (unsigned i = 0; i < req_players.size(); i++)
		{
			playerDataPtr d = player_mgr.getPlayerMain(req_players[i].asInt());

			if (d == playerDataPtr())
			{
				is_find_player = false;
			}
			else
			{
				vecPlayer.push_back(d);
			}
		}

		if (!is_find_player)
		{
			if(m._post_times > 0)
			{
				Return(r, -1);
			}
			player_mgr.postMessage(m);
			return;
		}

		int ret = 0;
		Json::Value complexAction = simple2complex(req_resource);
		for (unsigned i = 0; i < req_players.size(); i++)
		{
			actionResult resAction = actionFormat::actionDo(vecPlayer[i], complexAction);

			if (resAction.resultCode != 0)
			{
				r[msgStr][1][0u] = resAction.breakID;
				r[msgStr][1][1] = resAction.resultCode;
				Json::Value mail;
				for (unsigned j = 0; j < req_resource.size(); j++)
				{
					if (req_resource[j][email::actionID].asInt() >= resAction.breakID)
					{
						mail.append(req_resource[j]);
					}
				}
				ret = 1;
				break;
			}
		}

		r[msgStr][0] = ret;
		string s = r.toStyledString();
		na::msg::msg_json mj(gate_client::gm_send_gift_card_to_player_resp, s);
		mj._player_id = req_players[0u].asInt();
		mj._net_id = vecPlayer[0u]->netID;
		player_mgr.sendToPlayer(mj);
	}

	void gm_tools::gmSendEmailToPlayerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		Json::Value req_players = js_msg[0u];
		Json::Value req_resource = js_msg[1];
		string req_words = js_msg[2].asString();
		string packName = js_msg[3].asString();
		int packPicId = js_msg[4].asInt();

		vector<playerDataPtr> vecPlayer;

		Json::Value retErr;
		bool is_find_player = true;

		if (req_players[0u].asInt() == -998)
		{
			string guildName = req_players[1].asString();
			Guild::guildPtr gPtr = guild_sys.getGuild(guildName);
			if (gPtr == Guild::guildPtr())
				Return(r, 2);
			vector<int> memList;
			gPtr->getAllMembers(memList);
			unsigned i = 0;
			req_players = Json::arrayValue;
			for (vector<int>::iterator it = memList.begin(); it != memList.end(); it++, i++)
			{
				req_players[i] = *it;
			}
		}

		if (req_players[0].asInt() == -999)
		{
			vecPlayer = player_mgr.onlinePlayer();
		}
		else 
		{
			for (unsigned i = 0; i < req_players.size(); i++)
			{
				playerDataPtr d = player_mgr.getPlayerMain(req_players[i].asInt());
				if (d == playerDataPtr())
				{
					is_find_player = false;
					retErr.append(req_players[i].asInt());
				}
				else
				{
					vecPlayer.push_back(d);
				}
			}

			if (!is_find_player)
			{
				if(m._post_times > 0)
				{
					r[msgStr][1] = retErr;
					Return(r, 1);
				}
				player_mgr.postMessage(m);
				return;
			}
		}
		vector<attachManager> attachs;
		for (unsigned i = 0; i < req_resource.size(); i++)
		{
			attachManager am;
			Json::Value& tempJson = req_resource[i];
			if (tempJson[email::gift_action].isNull() || tempJson[email::gift_action].size() == 0)
				continue;
			am.packageType = tempJson[email::gift_type].asInt();
			am.packagePictureId = tempJson[email::gift_picture_id].asInt();
			am.packageName = tempJson[email::gift_name].asString();
			am.attachAction = tempJson[email::gift_action];
			attachs.push_back(am);
		}

		Json::Value wordJson;
		wordJson[email::leave_word] = req_words;
		for (unsigned i = 0; i < vecPlayer.size(); i++)
		{
			email_sys.sendSystemEmailByGm(vecPlayer[i], attachs, wordJson);
		}

		Return(r, 0);
	}

	void gm_tools::gmCustomServiceReplyReq( msg_json& m, Json::Value& r )
	{
		AsyncGetPlayerData(m);
		
		int player_id = m._player_id;
		r[msgStr][0u] = 0;
		string s = r.toStyledString();
		msg_json mj(gate_client::gm_custom_service_reply_resp, s);
		if (player_mgr.IsOnline(player_id))
		{
			mj._net_id = d->netID;
			mj._player_id = player_id;
			player_mgr.sendToPlayer(mj);
		}
		else
		{
			customServiceList_.insert(player_id);
			save();
		}
	}

	Json::Value gm_tools::complex2simple(Json::Value complexAction)
	{
		Json::Value simpleAction;

		simpleAction = Json::arrayValue;
		for (unsigned idx = 0; idx < complexAction.size(); idx++)
		{
			if (!complexAction[idx][ACTION::strActionID].isNull())
			{
				simpleAction[idx][email::actionID] = complexAction[idx][ACTION::strActionID].asInt();
			}
			if (!complexAction[idx][ACTION::strValue].isNull())
			{
				simpleAction[idx][email::value] = complexAction[idx][ACTION::strValue].asInt();
			}
			if (!complexAction[idx][ACTION::addNumStr].isNull())
			{
				simpleAction[idx][email::addNum] = complexAction[idx][ACTION::addNumStr].asInt();
			}
			if (!complexAction[idx][ACTION::addItemIDStr].isNull())
			{
				simpleAction[idx][email::itemID] = complexAction[idx][ACTION::addItemIDStr].asInt();
			}
			if (!complexAction[idx][ACTION::strPilotActiveList].isNull())
			{
				simpleAction[idx][email::pilotActiveList] = complexAction[idx][ACTION::strPilotActiveList];
			}
			if (!complexAction[idx][ACTION::strAddItemRate].isNull())
			{
				simpleAction[idx][email::addItemRate] = complexAction[idx][ACTION::strAddItemRate].asDouble();
			}
			if (!complexAction[idx][ACTION::strSecretaryList].isNull())
			{
				simpleAction[idx][email::secretaryList] = complexAction[idx][ACTION::strSecretaryList];
			}
		}
		return simpleAction;
	}

	Json::Value gm_tools::simple2complex(Json::Value simpleAction)
	{
		Json::Value complexAction;

		complexAction = Json::arrayValue;
		for (unsigned idx = 0; idx < simpleAction.size(); idx++)
		{
			if (!simpleAction[idx][email::actionID].isNull())
			{
				complexAction[idx][ACTION::strActionID] = simpleAction[idx][email::actionID].asInt();
			}
			if (!simpleAction[idx][email::value].isNull())
			{
				complexAction[idx][ACTION::strValue] = simpleAction[idx][email::value].asInt();
			}
			if (!simpleAction[idx][email::addNum].isNull())
			{
				complexAction[idx][ACTION::addNumStr] = simpleAction[idx][email::addNum].asInt();
			}
			if (!simpleAction[idx][email::itemID].isNull())
			{
				complexAction[idx][ACTION::addItemIDStr] = simpleAction[idx][email::itemID].asInt();
			}
			if (!simpleAction[idx][email::pilotActiveList].isNull())
			{
				complexAction[idx][ACTION::strPilotActiveList] = simpleAction[idx][email::pilotActiveList];
			}
			if (!simpleAction[idx][email::addItemRate].isNull())
			{
				complexAction[idx][ACTION::strAddItemRate] = simpleAction[idx][email::addItemRate].asDouble();
			}
			if (!simpleAction[idx][email::actionItemType].isNull())
			{//特殊处理
				complexAction[idx][email::actionItemType] = simpleAction[idx][email::actionItemType].asDouble();
			}
			if (!simpleAction[idx][email::secretaryList].isNull())
			{//特殊处理
				complexAction[idx][ACTION::strSecretaryList] = simpleAction[idx][email::secretaryList];
			}
		}
		return complexAction;
	}

	Json::Value gm_tools::lastDo2simple(Json::Value lastDoJson)
	{
		Json::Value simpleAction;
		simpleAction = Json::arrayValue;
		for (unsigned idx = 0; idx < lastDoJson.size(); idx++)
		{
			int aid = lastDoJson[idx][0u].asInt();
			Json::Value temp;
			temp[email::actionID] = aid;
			switch (aid)
			{
				case action_add_silver:
				case action_add_gold_ticket:
				case action_add_gold:
				case action_add_keji:
				case action_add_work_times:
				case action_add_weiwang:
				case action_add_junling:
				case action_set_vip:
				case action_add_silver_vip_resource:
				case action_add_gold_vip_resource:
				case action_add_keji_vip_resource:
				case action_add_weiwang_vip_resource:
				case action_add_paper:
					temp[email::value] = lastDoJson[idx][1u].asInt();
					simpleAction.append(temp);
					break;
				case action_add_item:
				case action_rate_add_item:
					temp[email::itemID] = lastDoJson[idx][1u].asInt();
					temp[email::addNum] = lastDoJson[idx][2u].asInt();
					simpleAction.append(temp);
					break;
				case action_active_pilot:
				case action_add_pilot:
					temp[email::pilotActiveList] = lastDoJson[idx][1u];
					simpleAction.append(temp);
					break;
				case action_add_secretary:
					temp[email::secretaryList] = lastDoJson[idx][1u];
					simpleAction.append(temp);
					break;
				case action_add_role_exp:
				case action_add_pilot_exp:
				case action_add_keji_yaosai:
				case action_add_mutil_pilot_exp:
				case action_deal_war:
				default:
					break;
			}
		}
		return simpleAction;
	}

	bool gm_tools::is_from_gm_http(int net_id)
	{
		return service::process_id::HTTP_GM_START_NET_ID <= net_id && net_id <= service::process_id::HTTP_GM_END_NET_ID;
	}

	void gm_tools::initData()
	{
		get();
	}

	bool gm_tools::get()
	{
		mongo::BSONObj key = BSON(gm_tool_def::strKey << gm_tool_def::strKey);
		mongo::BSONObj obj = db_mgr.FindOne(gm_tool_def::strDbGmTools, key);

		if (!obj.isEmpty())
		{
			customServiceList_.clear();
			checkNotEoo(obj[gm_tool_def::strCustomList])
			{
				vector<BSONElement> sets = obj[gm_tool_def::strCustomList].Array();
				for (unsigned i = 0; i < sets.size(); i++)
				{
					customServiceList_.insert(sets[i].Int());
				}
			}
			return true;
		}
		return false;
	}

	bool gm_tools::save()
	{
		mongo::BSONObj key = BSON(gm_tool_def::strKey << gm_tool_def::strKey);
		mongo::BSONObjBuilder obj;

		obj << gm_tool_def::strKey << gm_tool_def::strKey;

		mongo::BSONArrayBuilder carr;
		for (set<int>::iterator it = customServiceList_.begin(); it != customServiceList_.end(); it++)
		{
			carr << *it;
		}
		obj << gm_tool_def::strCustomList << carr.arr();

		return db_mgr.save_mongo(gm_tool_def::strDbGmTools, key, obj.obj());
	}

	void gm_tools::customServiceNotity(playerDataPtr player)
	{
		int player_id = player->playerID;
		set<int>::iterator it = find(customServiceList_.begin(), customServiceList_.end(), player_id);
		if (it != customServiceList_.end())
		{
			if (player_mgr.IsOnline(player_id))
			{
				Json::Value r;
				r[msgStr][0u] = 0;
				string s = r.toStyledString();
				msg_json mj(gate_client::gm_custom_service_reply_resp, s);
				mj._net_id = player->netID;
				mj._player_id = player_id;
				player_mgr.sendToPlayer(mj);
				customServiceList_.erase(it);
				save();
			}
		}
	}

	void gm_tools::actTimePretreat( int &timePre, int opt /*= 0*/ )
	{
		na::time_helper::get_current_time();
		if (timePre != 0)
		{
			if (opt == 0)
				timePre = timePre + 3600 * na::time_helper::timeZone();
			else
				timePre = timePre - 3600 * na::time_helper::timeZone();
		}
	}

	bool gm_tools::isGmProtocal( int msgType )
	{
		if (gate_client::gm_protocal_start_req <=msgType && msgType <= gate_client::gm_protocal_end_req)
			return true;

		return false;
	}

}

