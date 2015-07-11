#include "system_response.h"
#include "playerManager.h"
#include "helper.h"
#include "battle_system.h"
#include "pilotManager.h"
#include "war_story.h"
#include "guild_system.h"
#include "world_system.h"
#include "email_system.h"
#include "record_system.h"
#include "service_config.h"
#include "season_system.h"
#include "strategy_system.h"
#include "gate_game_protocol.h"
#include "task_system.h"
#include "item_system.h"
#include "action_instance.hpp"
#include "kingdom_system.h"
#include "chat_system.h"
#include "sign_system.h"
#include "gm_tools.h"
#include "workshop_system.h"
#include <stdlib.h>
#include "script.hpp"
#include "timmer.hpp"
#include "activity_system.h"

namespace gg
{
	const static string logWarStory = "log_war_story";
	enum
	{
		war_story_log_pve_single,
		war_story_log_pve_team
	};

	const static mongo::BSONObj dev_key = BSON("key" << 233);
	const static string devDB = "gl.system_dev";

	system_response* const system_response::respSys = new system_response();

// 	void system_response::chargeGold(msg_json& m, Json::Value& r)
// 	{
// 		const static string MysqlLogChargeGold = "log_charge";
// 		if (!gm_tools_mgr.is_from_gm_http(m._net_id))
// 			Return(r, -1);
// 		AsyncGetPlayerData(m);
// 		m._net_id = d->netID;
// 		ReadJsonArray;
// 		int player_id	= js_msg[0u].asInt();//player_mgr.get_player_id_by_uid(uid);
// 		string order_id	= js_msg[1u].asString();
// 		int gold		= js_msg[2u].asInt();
// 		int status		= js_msg[3u].asInt();
// 		string pay_type_str= js_msg[4u].asString();
// 		int pay_amount	= js_msg[5u].asInt();
// 		string error_str	= js_msg[6u].asString();
// 		int acc_gold	= js_msg[8u].asInt();
// 		int extra_gold	= js_msg[9u].asInt();
// 
// 		d->Base.alterGold(gold);
// 		d->Base.alterGoldTicket(extra_gold);
// 		//log
// 		
// 
// 		r[msgStr][0u] = 0;
// 		r[msgStr][1u] = order_id;
// 		r[msgStr][2u] = gold;
// 		r[msgStr][3u] = pay_type_str;
// 		r[msgStr][4u] = pay_amount;
// 		r[msgStr][5u] = error_str;
// 		r[msgStr][6u] = js_msg[msgStr][7u]; //unit
// 		r[msgStr][7u] = extra_gold;
// 
// 		string logData = r[msgStr].toStyledString();
// 		StreamLog::Log(MysqlLogChargeGold, d, order_id, logData);
// 	}

	//role system
	void system_response::roleUpdateClient(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Base.update();
	}

	void system_response::sendLoginUpdate(playerDataPtr player)
	{
		player->TickMgr.Tick();
		player->Item.update();
		player->Pilots.update(true);
//		player->Builds.update();
		player->Embattle.update();
		//player->active.noticeReward();
		player->Sign.playerLogin();
		player->Vip.notifyFirstGift();
		player->Vip.autoUpdate();
		player->Email.autoUpdate();
		player->Guild.update();
		player->Task.playerLogin();
		player->Science.update();
		player->Ally.playerLogin();
		player->onlineAward.playerLogin();
		player->Starwar.playerLogin();
		player->TradeSkill.update();
		//player->playerEvent.updateRedPoint();
		guild_sys.sendHelpList(player);
		activity_sys.noticeClientUpdateBonusPar(player);
		helper_mgr.insertSaveAndUpdate(&player->Levy);
		helper_mgr.insertSaveAndUpdate(&player->Campaign);
		Json::Value seasonJson = season_sys.serverOpenPackage();
		player_mgr.sendToPlayer(player->playerID, player->netID, gate_client::server_info_resp, seasonJson);
		Json::Value itemJson;
		item_sys.packageBaseItemSys(player, itemJson);
		gm_tools_mgr.customServiceNotity(player);
		player_mgr.sendToPlayer(player->playerID, player->netID, gate_client::item_system_info_update_resp, itemJson);
	}

	void system_response::playerLogin(msg_json& m, Json::Value& r)
	{
		playerDataPtr player = player_mgr.getPlayerMain(m._player_id, m._net_id, false);
		if(m._post_times < 1 && playerDataPtr() == player)
		{
			player_mgr.postMessage(m);
			return;
		}
		if(m._post_times > 0 && playerDataPtr() == player)Return(r, -1);
		if(! player->isVaild())Return(r, 1);
		player->tickLogin();
		asyncSystemPlayerData(player);
		sendLoginUpdate(player);
		player->Base.update();
		r[msgStr][1u] = player->isFirstLogin();
		Return(r, 0);
	}

	void system_response::syncAccount(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m)
		r[msgStr][0u] = d->Base.getName();
	}

// 	void system_response::updateCDList(msg_json& m, Json::Value& r)
// 	{
// 		AsyncGetPlayerData(m);
// 		d->CDList.update();
// 	}

	void system_response::playerProcess(msg_json& m, Json::Value& r)
	{
		playerDataPtr player = player_mgr.getPlayerMain(m._player_id, m._net_id, false);
		if(m._post_times < 1 && playerDataPtr() == player)
		{
			player_mgr.postMessage(m);
			return;
		}
		if(m._post_times > 0 && playerDataPtr() == player)Return(r, -1);
		ReadJsonArray;
		if(!js_msg[0u].isUInt())Return(r, -1);
		player->Base.setPlayerProcess(js_msg[0u].asUInt());
		Return(r, 0);
	}

	void system_response::setGender(msg_json& m, Json::Value& r)
	{
		//AsyncGetPlayerData(m);
	}

	void system_response::joinKingdom(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		if(!js_msg[0u].isInt())Return(r, -1);
		int kingdom = js_msg[0u].asInt();
		int nowKD = d->Base.getSphereID();
		if(nowKD >= 0 && nowKD <= 2)Return(r, -1);
		if(!d->Warstory.isProcessMap(112))Return(r, 1);
		int res =  d->Base.setSphere(kingdom);
		Return(r, res);
	}

	unsigned getOrgTime()
	{
		static int ofs = na::time_helper::timeZone() * na::time_helper::ONEHOUR;
		return na::time_helper::get_current_time() - ofs;
	}

	void system_response::serverTimeGet(msg_json& m, Json::Value& r)
	{
		r[msgStr][2u] = na::time_helper::getDeviation();
		r[msgStr][1u] = getOrgTime();
		Return(r, 0);
	}


	void saveDev()
	{
		mongo::BSONObj val = BSON("key" << 233 << "val" << na::time_helper::getDeviation());
		db_mgr.save_mongo(devDB, dev_key, val);
	}

	void system_response::serverTimeSet(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		unsigned _dev = js_msg[0u].asUInt();
		if (_dev / na::time_helper::ONEDAY > 6)Return(r, -1);
		na::time_helper::setDeviation(_dev);
		saveDev();
		r[msgStr][2u] = na::time_helper::getDeviation();
		r[msgStr][1u] = getOrgTime();
		r[msgStr][0u] = 0;
		vector<playerDataPtr> allOnline = player_mgr.onlinePlayer();
		Json::Value seasonJson = season_sys.serverOpenPackage();
		for (unsigned i = 0; i < allOnline.size(); i++)
		{
			allOnline[i]->sendToClient(gate_client::server_info_resp, seasonJson);
		}
	}

	void system_response::joinMinKingdom(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int nowKD = d->Base.getSphereID();
		if(nowKD >= 0 && nowKD <= 2)Return(r, -1);
		if(!d->Warstory.isProcessMap(112))Return(r, 1);
		int kd = kingdom_sys.getMinKingdom();
		int res =  d->Base.setSphere(kd);
		if(res == 0)
		{
// 			int day = (int)(na::time_helper::get_current_time() - season_sys.getServerOpenTime()) / na::time_helper::ONEDAY;
// 			int silver = (int)(log10((double)(3 + day)) * 1000 + 23) * 400 ;
// 			if(silver > 0)d->Base.alterSilver(silver);
			d->Base.alterSilver(50000);
			d->Base.alterGoldTicket(100);
		}
		r[msgStr][1u] = kd;
		Return(r, res);
	}

	void system_response::asyncSystemPlayerData(playerDataPtr player)
	{
		world_sys.checkMoving(player);
		guild_sys.asyncPlayerData(player);
		world_sys.asyncPlayerData(player);
	}

	bool illegalLengthName(const string name)
	{
		if(name.length() < 1 || name.length() > 42)return false;
		size_t len = commom_sys.SimpleCnEg(name);
		if (len < 3 || len > 14)return false;
		return true;
	}

	void system_response::createRole(msg_json& m, Json::Value& r)
	{
		playerDataPtr d = player_mgr.getPlayerMain(m._player_id, m._net_id, false);
		if(m._post_times < 1 && playerDataPtr() == d){player_mgr.postMessage(m);return;}
		if(m._post_times > 0 && playerDataPtr() == d)Return(r, -1);
		ReadJsonArray;			
		string name = js_msg[0u].asString();
		int gender = js_msg[1u].asInt();
		if (js_msg[2u].isNull() || js_msg[2u].asBool())Return(r, 4);//有敏感词
		if(!(gender == playerDefine::male || gender == playerDefine::female))Return(r , 2);
		if(player_mgr.hasRegisterName(name))Return(r, 1);
		if(!illegalLengthName(name))Return(r, 3);
		if(d->Base.getName() != "")Return(r, -1);
		d->Base.setName(name);
		d->Base.setGender(gender);
		d->Base.setDefaultFace();
		d->Item.addItem(13002, 5);
		d->TickMgr.Tick();
		email_sys.sendSystemEmailCommon(d, email_type_system_ordinary, email_team_system_new_player);
		sendLoginUpdate(d);
		Return(r, 0);
	}

	void system_response::systemUpdate()
	{
		SafeCallLua(mLua::Root("sys")["ShowLuaMemInfo"]());
		Timer::AddEventSeconds(boost::bind(&system_response::systemUpdate, this), 3600);
	}

	void system_response::initData()
	{
		Json::Value val = na::file_system::load_jsonfile_val("./instance/world_extern/investPopulation.json");
		Params::ArrayValue arr(val);
		investPopulation = arr;

		mongo::BSONObj obj = db_mgr.FindOne(devDB, dev_key);
		if (!obj["val"].eoo())
		{
			na::time_helper::setDeviation(obj["val"].Int());
		}

		Timer::AddEventSeconds(boost::bind(&system_response::systemUpdate, this), 3600);
	}

	//response
	void system_response::teamPVERespon(BattlefieldMuilt field, Json::Value report)
	{
		State::setState(gate_client::player_npc_team_pve_req);
		NumberCounter::Step();
		int battleResult = report[BATTLE::strBattleResult].asInt();
		playerDataPtr leader;
		int mapID = field.ParamJson[BATTLE::strWarMapID].asInt();
		
// 		{
// 			battleResult = 1;
// 			report[BATTLE::strBattleResult] = 1;
// 		}

		int expMedicID = 14035;
		int sweepOrderID = 13009;
		int titaniumAlloyID = 13008;
		teamMapDataPtr mp = war_story.getTeamMapData(mapID);
		Json::Value actionTemp = Json::arrayValue;
		for (actionVector::iterator itAct = mp->actionVec.begin(); itAct != mp->actionVec.end(); itAct++)
		{
			actionTemp.append((*itAct)->JsonData());
		}
		int luckyGuy = commom_sys.randomBetween(0, field.atkSide.size()-1);
		for (unsigned i =0 ; i < field.atkSide.size(); ++i)
		{
			bool isPlayer = true;
			Json::Value actionJsonList = actionTemp;
			battleSidePtr side = field.atkSide[i];
			playerDataPtr player = player_mgr.getPlayerMain(side->getSideID());
			if (player == playerDataPtr())
			{
				isPlayer = false;
			}

			if (i == 0)
			{
				leader = player;
			}
//			side->netID = player->netID;
			//奖励
			do{
				if(battleResult  == 1)
				{
					if(mp == teamMapDataPtr())break;
					Json::Value Param;
					Param[ACTION::strFromWarStoryWin] = true;
					Param[ACTION::strCurrentWarStoryID] = mapID;
					armyVec av = side->getMember(MO::mem_null);
					Json::Value& PilotList = Param[ACTION::strPilotList];
					for (unsigned j = 0; j < av.size(); j++)	PilotList.append(av[j]->getID());

					double kejiAddRateWinNum = 0.0, kejiAddRateBase = 0.0;
					double addItemExtraRate = 0.0f;
					double d1, d2;

					if (isPlayer)
					{
						kejiAddRateBase = player->Science.getKeJiBuffer();
						d2 = activity_sys.getRate(activity::_jing_bing_qiang_zhen, player);
						d1 = activity_sys.getRate(activity::_fort_war, player);
						addItemExtraRate = activity_sys.getRate(activity::_fu_xing_gao_zhao, player);
					}
					else
					{//机器人按照队长信息填充，因此。。。
						d2 = activity_sys.getRate(activity::_jing_bing_qiang_zhen, leader);
						d1 = activity_sys.getRate(activity::_fort_war, leader);
					}

					kejiAddRateBase = kejiAddRateBase + d1 + d2;
										
					if (isPlayer && (!leader->Warstory.hasFirstBlood(mapID) || !player->Warstory.hasFirstBlood(mapID)))
					{
						Param[ACTION::strAddPlayerExpRate] = 0.0;
						kejiAddRateBase = kejiAddRateBase + 0.2;
					}
					else
					{
					}
					
					//对经验丹和装备奖励单独处理
					for (unsigned idxAct = 0; idxAct < actionJsonList.size(); idxAct++)
					{
						if (actionJsonList[idxAct][ACTION::strActionID].asInt() == action_rate_add_item)
						{
							int itemID = actionJsonList[idxAct][ACTION::addItemIDStr].asInt();
							actionJsonList[idxAct][ACTION::strAddItemRate] = actionJsonList[idxAct][ACTION::strAddItemRate].asDouble() * (1 + addItemExtraRate);
							if (itemID != expMedicID && itemID != titaniumAlloyID && itemID != sweepOrderID)//经验丹14035,这里对装备单独处理
							{
								if (luckyGuy != i)
								{
									actionJsonList[idxAct][ACTION::strAddItemRate] = 0.0f;
								}
							}
						}
					}

					if (side->getWinNum() >= 2)
						kejiAddRateWinNum = (0.2 + 0.05 * (side->getWinNum() - 2));

					Param[ACTION::strYaoSaiParamBase] = kejiAddRateBase;
					Param[ACTION::strYaoSaiParamWinNum] = kejiAddRateWinNum;
					vector<actionResult> vecRetCode;
					Json::Value npcLastDoJson = Json::arrayValue;
					if (isPlayer)
					{//玩家
						vecRetCode = actionFormat::actionDoJump(player, actionJsonList, Param);
						Json::Value failSimpleJson;
						email_sys.sendDoJumpFailEmail(player, mp->actionVec, vecRetCode, email_team_system_bag_full_war_story, failSimpleJson);

						{//仓库满发邮件的同时也要发公告
							vector<int> vecItem;
							for (unsigned j = 0; j < vecRetCode.size(); j++)
							{
								if (vecRetCode[j].resultCode != 0 
									&& (vecRetCode[j].breakID == action_rate_add_item || vecRetCode[j].breakID == action_add_item))
								{
									Json::Value singleAction = mp->actionVec[j]->JsonData();
									vecItem.push_back(singleAction[ACTION::addItemIDStr].asInt());
								}
							}

							for (unsigned j = 0; j < vecItem.size(); j++)
							{
								int itemID = vecItem[j];

								//照抄actionDo
								Params::ObjectValue& obj = item_sys.getConfig(itemID);
								int quality = obj[ItemDefine::itemQualityStr].asInt();
								Json::Value objJson = Params::ObjectValue::toJson(obj);
								int channelID = chat_area;
								if (quality > 2 && objJson[ItemDefine::chip].isNull())
								{
									if (quality == 5)channelID = chat_all;
									else if (quality == 4)channelID = chat_kingdom;

									Json::Value bCast;
									bCast[msgStr][0u] = 0;
									bCast[msgStr][1u][senderChannelStr] = channelID;
									bCast[msgStr][1u][chatBroadcastID] = BROADCAST::reward_good_item;
									const static string ParamListStr = "pl";
									bCast[msgStr][1u][ParamListStr].append(player->Base.getName());
									if (Param.isMember(ACTION::strCurrentWarStoryID))bCast[msgStr][1u][ParamListStr].append(0);
									else bCast[msgStr][1u][ParamListStr].append(1);
									bCast[msgStr][1u][ParamListStr].append(itemID);
									bCast[msgStr][1u][ParamListStr].append(1);
									if (quality == 3)player_mgr.sendToArea(gate_client::chat_broadcast_resp, bCast, player->Base.getPosition().first);
									else if (quality == 4)player_mgr.sendToSphere(gate_client::chat_broadcast_resp, bCast, player->Base.getSphereID());
									else player_mgr.sendToAll(gate_client::chat_broadcast_resp, bCast);
								}

								int localID = Param[ACTION::strCurrentWarStoryID].asInt();
								Json::Value annouceJson;
								annouceJson[msgStr][0u] = 0;
								annouceJson[msgStr][1] = player->Base.getName();
								annouceJson[msgStr][2] = itemID;
								annouceJson[msgStr][3] = false;
								string s = annouceJson.toStyledString();
								msg_json mj(gate_client::war_story_team_annouce_gain_item_resp, s);
								war_story.sendToTeamList(localID, mj);
							}
						}
					}
					else
					{//非玩家
						for (unsigned idxAct = 0; idxAct < actionJsonList.size(); idxAct++)
						{
							int actionID = actionJsonList[idxAct][ACTION::strActionID].asInt();
							if (actionID == action_rate_add_item)
							{
								int itemID = actionJsonList[idxAct][ACTION::addItemIDStr].asInt();
								double addRate = actionJsonList[idxAct][ACTION::strAddItemRate].asDouble();
								addRate = addRate * (1 + addItemExtraRate);
								if (itemID == expMedicID || itemID == titaniumAlloyID || itemID == sweepOrderID)//对经验丹和钛合金和扫荡令
								{
									if (commom_sys.randomOk(addRate))
									{
										Json::Value doJson;
										doJson.append(actionID);
										doJson.append(itemID);
										doJson.append(1);
										npcLastDoJson.append(doJson);
									}
								}
								else//这里对装备单独处理
								{
									if (luckyGuy == i && field.ParamJson[BATTLE::strWarStoryTeamMemberType][i].asInt() == 2
										&& commom_sys.randomOk(addRate))
									{
										Json::Value annouceJson;
										annouceJson[msgStr][0u] = 0;
										annouceJson[msgStr][1] = side->getName();
										annouceJson[msgStr][2] = itemID;
										annouceJson[msgStr][3] = true;
										string s = annouceJson.toStyledString();
										msg_json mj(gate_client::war_story_team_annouce_gain_item_resp, s);
										war_story.sendToTeamList(mapID, mj);

										Json::Value doJson;
										doJson.append(actionID);
										doJson.append(itemID);
										doJson.append(1);
										npcLastDoJson.append(doJson);
									}
								}
							}
							else if (actionID == action_add_keji_yaosai)
							{
								int fixed = actionJsonList[idxAct][ACTION::strValue].asInt();
								int tmpValueBase = int(fixed * (1 + Param[ACTION::strYaoSaiParamBase].asDouble()));
								int tmpValeWinNum = int(fixed * (Param[ACTION::strYaoSaiParamWinNum].asDouble()));
								Json::Value doJson;
								doJson.append(actionID);
								doJson.append(tmpValueBase);
								doJson.append(tmpValeWinNum);
								npcLastDoJson.append(doJson);
							}
							else
							{
							}
						}
					}
					//处理战报
					Json::Value SingleRW;
					SingleRW.append(side->getName());
					if (isPlayer)
					{
						SingleRW.append(player->Base.getSphereID());
					}
					else
					{
						SingleRW.append(side->sphereID);
					}
					SingleRW.append(side->getPlayerLevel());
					SingleRW.append(side->getWinNum());
					if (isPlayer)
					{//玩家
						Json::Value lastDoJson = actionFormat::getLastDoJson();
						email_sys.dealDoJumpFailLastDo(mp->actionVec, vecRetCode, lastDoJson);
						SingleRW.append(lastDoJson);
					}
					else
					{//非玩家						
						SingleRW.append(npcLastDoJson);
					}
					report[BATTLE::strBattleReward].append(SingleRW);

//					cout << __FUNCTION__ << ",reward:" << report[BATTLE::strBattleReward].toStyledString() << endl;
				}else{ //失败
					Json::Value SingleRW;
					SingleRW.append(side->getName());
					if (isPlayer)
					{
						SingleRW.append(player->Base.getSphereID());
					}
					else
					{
						SingleRW.append(side->sphereID);
					}
					SingleRW.append(side->getPlayerLevel());
					SingleRW.append(side->getWinNum());
					SingleRW.append(Json::arrayValue);
					report[BATTLE::strBattleReward].append(SingleRW);
				}

				if (isPlayer)
				{
					if (leader->Warstory.hasFirstBlood(mapID) && player->Warstory.hasFirstBlood(mapID))
					{
						if(battleResult == 1 || player->Base.getLevel() >= 80)
							player->Campaign.alterJunling(-1);
					}
					if (player->Warstory.isProcessMap(war_story.getCDCreateMap()))
					{
						player->Campaign.getCD().addCD(60);
						helper_mgr.insertSaveAndUpdate(&player->Campaign);
					}
					StreamLog::Log(logWarStory, player, mapID, battleResult, war_story_log_pve_team);
				}
			}while(false);
			helper_mgr.runSaveAndUpdate();
		}

		for (unsigned i = 0; i < field.atkSide.size(); ++i)
		{
			battleSidePtr side = field.atkSide[i];
			playerDataPtr player = player_mgr.getPlayerMain(side->getSideID());
			if (player == playerDataPtr())continue;
			if (battleResult == 1)
			{
				if (!player->Warstory.hasFirstBlood(mapID))
				{
					player->Warstory.setFirstBlood(mapID);
				}
			}
		}

		Json::Value annouceJson;
		annouceJson[msgStr][0u] = 0;
		annouceJson[msgStr][1] = mapID;
		annouceJson[msgStr][2] = leader->Base.getName();
		string s = annouceJson.toStyledString();
		msg_json mj(gate_client::war_story_team_annouce_attack_resp, s);
		war_story.sendToTeamList(mapID, mj);

		_Battle_Post(boost::bind(&battle_system::DealTeamPVEReport, battle_system::battleSys, 
			field, report));
	}

	void system_response::PVERespon(Battlefield field, Json::Value report)
	{
		State::setState(gate_client::player_war_pve_req);
		NumberCounter::Step();
		playerDataPtr player = player_mgr.getPlayerMain(field.atkSide->getSideID());
		if(player == playerDataPtr())return;

		int battleResult = report[BATTLE::strBattleResult].asInt();
		report[BATTLE::strBattleReward] = Json::arrayValue;
		int localID = field.defSide->getSideID();
		bool hasDefeat = player->Warstory.isProcessMap(localID);
		do{
			if(battleResult  == 1)
			{
				mapDataPtr mp = war_story.getMapData(field.defSide->getSideID());
				if(mp == mapDataPtr())break;
				Json::Value Param;
				Param[ACTION::strKejiBuffer] = player->Science.getKeJiBuffer();
				Param[ACTION::strFromFirstWarStoryWin] = (mp->firstBlood && !hasDefeat);
				Param[ACTION::strFromWarStoryWin] = true;
				armyVec av = field.atkSide->getMember(MO::mem_null);
				Json::Value& PilotList = Param[ACTION::strPilotList];
				Param[ACTION::strAddItemExtraRate] = activity_sys.getRate(activity::_fu_xing_gao_zhao, player);
				for (unsigned i = 0; i < av.size(); i++)	PilotList.append(av[i]->getID());
				vector<actionResult> vecRetCode = actionFormat::actionDoJump(player, mp->actionVec, Param);
				Json::Value failSimpleJson;
				email_sys.sendDoJumpFailEmail(player, mp->actionVec, vecRetCode, email_team_system_bag_full_war_story, failSimpleJson);
				Json::Value lastDoJson;
				lastDoJson = actionFormat::getLastDoJson();
				email_sys.dealDoJumpFailLastDo(mp->actionVec, vecRetCode, lastDoJson);
				report[BATTLE::strBattleReward] = lastDoJson;
				strategy_sys.updateInfo(player, field, report, mp->mapType);
				player->Warstory.processWarStory(player, field);
				war_story.pushBattleNpc(localID, field.atkSide);
			}
		}while(false);
		if(battleResult == 1 || player->Base.getLevel() >= 80)player->Campaign.alterJunling(-1);
		else helper_mgr.insertSaveAndUpdate(&player->Campaign);
		helper_mgr.runSaveAndUpdate();
		StreamLog::Log(logWarStory, player, localID, battleResult, war_story_log_pve_single);
		_Battle_Post(boost::bind(&battle_system::DealPVEReport, battle_system::battleSys, 
			field, report, hasDefeat));
	}

	const static string logWorldPVP = "log_world_pvp";
	const static string logTeamPVP = "log_pve_team";
	enum{
		world_tag_atk,
		world_tag_def,
	};

	void system_response::PVPRespon(const string file, Json::Value report, Battlefield field)
	{
		State::setState(gate_client::player_world_pvp_req);
		NumberCounter::Step();
		playerDataPtr player = player_mgr.getPlayerMain(field.atkSide->getPlayerID());
		playerDataPtr target = player_mgr.getPlayerMain(field.defSide->getPlayerID());
		if(player == NULL || target == NULL)return;
		int battleResult = report[BATTLE::strBattleResult].asInt();
		int old_popu = target->Base.getCalPopu();
		world_sys.wPVPDeal(player, target, battleResult);
		int lose_popu = std::abs(target->Base.getCalPopu() - old_popu);
		report["pvplose"] = lose_popu;
		Json::Value pvp_content;
		pvp_content["gw"]	= 0; //555str_gain_weiwang
		pvp_content["res"] = battleResult; //str_res
		pvp_content["rp"] = file; //str_report_link
		pvp_content["an"]	 = player->Base.getName(); //str_atk_name
		pvp_content["dn"] = target->Base.getName(); //str_def_name
		pvp_content["ai"] = player->playerID; //str_atk_name
		pvp_content["di"] = target->playerID; //str_def_name
		pvp_content["dl"] = lose_popu; //defend lost
		int destory_budling_id = -1;
		if (destory_budling_id != -1)
			pvp_content["did"] = destory_budling_id; //str_destory_budling_id
		//不发送攻击方的邮件
		pvp_content["cn"] = player->Base.getName();
		email_sys.sendSystemEmailCommon(player, email_type_system_battle_report, email_team_system_pvp, pvp_content);
		pvp_content["cn"] = target->Base.getName();
		email_sys.sendSystemEmailCommon(target, email_type_system_battle_report, email_team_system_pvp, pvp_content);

		helper_mgr.runSaveAndUpdate();
		StreamLog::Log(logWorldPVP, player, target->playerID, battleResult, world_tag_atk);
		StreamLog::Log(logWorldPVP, target, player->playerID, battleResult, world_tag_def);
		_Battle_Post(boost::bind(&battle_system::DealPVPReport, battle_system::battleSys,
			field, file, report));
	}

	void system_response::OverBattle(vector<int> IDList)
	{
		for (unsigned i = 0; i < IDList.size(); ++i)
		{
			playerDataPtr player = player_mgr.getPlayerMain(IDList[i]);
			if(player != NULL)player->SetFreeSate();
		}
	}

	void system_response::OverBattleBoss(vector<int> IDList)
	{
		for (unsigned i = 0; i < IDList.size(); ++i)
		{
			playerDataPtr player = player_mgr.getPlayerMain(IDList[i]);
			if (player != NULL)player->SetFreeSate();
		}
		world_boss_sys.worldBossOver();
	}

	void system_response::noticePlayerGuild(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int player_id = m._player_id;
		playerDataPtr player = player_mgr.getPlayerMain(player_id);
		if(player == playerDataPtr())return;
		player->Guild.setGuildID(-1, js_msg[0u].asInt());
	}

	void system_response::upgradeToLeaderNoticeReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		if(m._net_id >= 0)return; 
		int player_id = m._player_id;
		playerDataPtr player = player_mgr.getPlayerMain(player_id);
		if(player == playerDataPtr())return;

		Json::Value leaderReplaceEmail;
		string guildLeaderName = "gln";
		leaderReplaceEmail[guildLeaderName] = js_msg[0u].asInt();
		email_sys.sendSystemEmailCommon(player, email_type_system_ordinary, 
			email_team_system_replace_guild_leader, leaderReplaceEmail);
	}

	void system_response::levyCDClear(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int res = d->Levy.clearCD(true);
		Return(r, res);
	}

	void system_response::invest(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int num = d->Base.getInvest();
		if(num < 1)Return(r, 1);
		ReadJsonArray;
		int opt = js_msg[0u].asInt();
		int silver = 1000 + d->Base.getLevel() * 50;
		if(opt == 0);
		else if(opt == 1)silver *= 5;
		else silver *= 10;
		if(d->Base.getSilver() < silver)Return(r, 2);
		int pop = investPopulation[d->Base.getLevel()].asInt();
		if(opt == 0){
			pop /= 10;
			int ww = d->Base.getLevel() * 2;
			d->Base.alterWeiWang(ww);
		}else if(opt == 1){
			pop /= 2;
			int ww = d->Base.getLevel() * 10;
			d->Base.alterWeiWang(ww);
		}else {
			int ww = d->Base.getLevel() * 20;
			d->Base.alterWeiWang(ww);
		}
		d->Base.alterPopulation(pop);
		d->Base.alterSilver(-silver);
		d->Base.alterInvest(-1);
		task_sys.updateBranchTaskInfo(d, _task_investment);
		activity_sys.updateActivity(d, 0, activity::_invest);
		Return(r, 0);
	}

	void system_response::updateLevy(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Levy.update();
	}

	void system_response::levy(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if(d->Base.getLevel() < 26)Return(r, -1);
		ReadJsonArray;
		bool useGold = js_msg[0u].asBool();
		int silver = d->Base.getSilver();
		int gold = d->Base.getAllGold();
		int res = d->Levy.levy(useGold);
		int addSil = d->Base.getSilver() - silver;
		r[msgStr][1u] = addSil;
		r[msgStr][2u] = 0;
// 		if(res == 0 && commom_sys.randomOk(0.05)){
// 			d->Base.alterGoldTicket(10);
// 			r[msgStr][2u] = 10;
// 		}
		if(0 == res)
		{
			int rate = guild_sys.getScienceData(d->Guild.getGuildID(), playerGuild::guild_contribute).ADD;
			if(commom_sys.randomOk(rate))
			{
				addSil  = addSil < 0 ? 0 : addSil;
				//int donate = d->Base.getLevel() * 200;
				if(0 == guild_sys.donateDefault(d, addSil))
				{
					r[msgStr][2u] = addSil;
				}
			}
		}
		if(res == 0)
		{
			if(useGold)
				activity_sys.updateActivity(d, 0, activity::_force_levy);
			else
				activity_sys.updateActivity(d, 0, activity::_levy);
		}
		Return(r, res);
	}

	void system_response::setFaceIdReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int face_id = js_msg[0u].asInt();
		if(d->Base.getAllGold() < 10)Return(r, 100);
// 		int res = 0;
// 		if (face_id < 0)
// 			d->Base.setDefaultFace();
// 		else
		int res = d->Base.setFaceID(face_id);
		if(res == 0)d->Base.alterBothGold(-10);
		Return(r, res);
	}

	void system_response::gmMotifyName(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		string name = js_msg[0u].asString();
		if (js_msg[1u].isNull() || js_msg[1u].asBool())Return(r, 1);//敏感字符
		if (player_mgr.hasRegisterName(name))Return(r, 2);//重复名字
		if (!illegalLengthName(name))Return(r, 3);//名字太长
		d->Base.setName(name, true);
		Return(r, 0);
	}

	void system_response::gmMotifyKingdom(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int kid = js_msg[0u].asInt();
		d->Base.setSphere(kid);
		Return(r, 0);
	}

	void system_response::gmAddWarStroy(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		if (!js_msg.isArray() || js_msg.size() > 300)Return(r, -1);
		for (unsigned i = 0; i < js_msg.size(); ++i)
		{
			Json::Value& val = js_msg[i];
			int localID = val[0].asInt();
			int star = val[1].asInt();
			d->Warstory.processWarStory(localID, star);
		}
		Return(r, 0);
	}

	void system_response::changeName(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		if(d->Base.getLevel() < 40)Return(r, -1);
		int cost = d->Base.getSetNameCost();
		if(cost > d->Base.getAllGold())Return(r, 2);
		string name = js_msg[0u].asString();
		if (js_msg[1u].isNull() || js_msg[1u].asBool())Return(r, 3);
		if(player_mgr.hasRegisterName(name))Return(r, 1);
		if(!illegalLengthName(name))Return(r, -1);
		string nameB = d->Base.getName();
		d->Base.setName(name);
		d->Base.alterBothGold(-cost);
		Json::Value bCast;
		bCast[msgStr][0u] = 0;
		bCast[msgStr][1u][senderChannelStr] = -1;
		bCast[msgStr][1u][chatBroadcastID] = BROADCAST::player_change_name;
		const static string ParamListStr = "pl";
		bCast[msgStr][1u][ParamListStr].append(nameB);
		bCast[msgStr][1u][ParamListStr].append(name);
		player_mgr.sendToAll(gate_client::chat_broadcast_resp, bCast);
		Return(r, 0);
	}

	void system_response::gateRestart(msg_json& m, Json::Value& r)
	{
		if(!gm_tools_mgr.is_from_gm_http(m._net_id))return;
		player_mgr.gateResetReq();
		LogS << "gate reset request ..." << LogEnd;
	}
}