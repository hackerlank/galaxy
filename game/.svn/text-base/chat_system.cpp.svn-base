#include "chat_system.h"
#include "gate_game_protocol.h"
#include "game_server.h"
#include "commom.h"
#include "playerManager.h"
#include "response_def.h"
#include "time_helper.h"
#include "world_system.h"
#include "guild_system.h"
#include "arena_system.h"
#include "service_config.h"
#include "war_story.h"
#include "guild_system.h"
#include "item_system.h"
#include "battle_def.h"
#include "gm_tools.h"
#include "core_helper.h"

using namespace mongo;
using namespace na::time_helper;

namespace gg
{
	const static unsigned playerChatTimeLimit = 3;
	const static string showPilotStr = "sp";
	const static string showItemStr = "si";
	const static string senderDataStr = "sd";
	const static string receiverDataStr = "rc";
	const static string senderWordsStr = "w";
	const static string attackerDataStr = "ad";
	const static string defenderDataStr = "dd";
	const static unsigned chatWordsSizeLimit = 200;

	//chat voice
	const static string senderVoiceStr = "svc";//语音key
	const static string senderVoiceRateStr = "vbr";//比特率


	chat_system* const chat_system::chatSys = new chat_system();

	static unsigned ChatCD[] = { 15, 15, 15, 15, 5 };

	timePeriod::timePeriod()
	{
		startFromZeroTime_ = 0;
		endFromZeroTime_ = 0;
	}

	rollBroacast::rollBroacast()
	{
		latestSendTime_ = 0;
		rollTimes_ = 1;
		broadCastID_ = ROLLBROADCAST::roll_gm_chat;
		channel_ = chat_all;
	}

	bool rollBroacast::isValid(unsigned tmp)
	{
		if (tmp < startZeroTime_ || tmp > endZeroTime_ + 86400)
		{
			return false;
		}
		for (unsigned i = 0; i < vecPeriod_.size(); i++)
		{
			timePeriod tp = vecPeriod_[i];
			for (unsigned j = startZeroTime_; j <= endZeroTime_; j = j + 86400)
			{
				if (j + tp.startFromZeroTime_ <= tmp && tmp <= j + tp.endFromZeroTime_)
					return true;
			}
		}
		return false;
	}

	void chat_system::initData()
	{
		getForbiddenPlayer();
		getRollBroacast();
	}

	void chat_system::packageSender(playerDataPtr sender, Json::Value& senderJson)
	{
		playerBase& pB = sender->Base;
		senderJson[playerIDStr] = sender->playerID;
		senderJson[playerNameStr] = pB.getName();
		senderJson[playerAreaIDStr] = pB.getPosition().first;
		senderJson[playerAreaPageStr] = pB.getPosition().second;
		senderJson[playerSphereIDStr] = pB.getSphereID();
		senderJson[playerWarStoryCurrentProcessStr] = sender->Warstory.currentProcess;
		senderJson[arenaRankStr] = sender->Arena.getRank();
		senderJson[office::playerOfficialLevelStr] = sender->Office.getOffcialLevel();
		senderJson[playerLevelStr] = sender->Base.getLevel();
		senderJson[ruler_title_field_str] = sender->Ruler.getTitle();
	}
// #ifndef _PUBLISH_
// 	bool chat_system::gmCommand(playerDataPtr player, string& words)
// 	{
// 		try
// 		{
// 			do
// 			{
// 				if(words.length() < 4)break;
// 				string gmC(words.begin(), words.begin() + 4);
// 				if(gmC == "gsil"){
// 					string data(words.begin() + 4, words.end());
// 					player->Base.alterSilver(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}
// 				else if(gmC == "gold"){
// 					string data(words.begin() + 4, words.end());
// 					player->Base.alterGold(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}
// 				else if(gmC == "gtic"){
// 					string data(words.begin() + 4, words.end());
// 					player->Base.alterGoldTicket(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}
// 				else if(gmC == "gitm"){
// 					string data(words.begin() + 4, words.end());
// 					player->Item.addItem(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}
// 				else if(gmC == "aexp"){
// 					string data(words.begin() + 4, words.end());
// 					player->Base.addExp(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}
// 				else if(gmC == "ggen"){
// 					string data(words.begin() + 4, words.end());
// 					player->Pilots.activePilot(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}
// 				else if(gmC == "gkjd"){
// 					string data(words.begin() + 4, words.end());
// 					player->Base.alterKeJi(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}
// 				else if(gmC == "pexp"){
// 					unsigned l = words.find(" ");
// 					if(l == string::npos)break;
// 					string data(words.begin() + 4, words.begin() + l);
// 					string exp(words.begin() + l + 1, words.end());
// 					player->Pilots.addExp(boost::lexical_cast<int,string>(data), 
// 						boost::lexical_cast<int,string>(exp));
// 					return true;
// 				}
// 				else if(gmC == "ckid"){
// 					string data(words.begin() + 4, words.end());
// 					player->Base.setSphere(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}
// 				else if(gmC == "gwwd"){
// 					string data(words.begin() + 4, words.end());
// 					player->Base.alterWeiWang(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}else if(gmC == "gsce"){
// 					unsigned l = words.find(" ");
// 					if(l == string::npos)break;
// 					string data(words.begin() + 4, words.begin() + l);
// 					string exp(words.begin() + l + 1, words.end());
// 					guild_sys.addGuildScienceExp(player, boost::lexical_cast<int,string>(data), 
// 						boost::lexical_cast<int,string>(exp));
// 					return true;
// 				}else if(gmC == "gpjl"){
// 					string data(words.begin() + 4, words.end());
// 					player->Campaign.alterJunling(boost::lexical_cast<int,string>(data));
// 					return true;
// 				}
// 			}while(false);
// 
// 		}catch(std::exception& e)
// 		{
// 			LogS << e.what() << LogEnd;
// 			LogS << words << " is not a gm command" << LogEnd;
// 		}
// 		return false;
// 	}
// #endif

	int chat_system::chatOption(const int chatID, const short protocol, Json::Value& msg, const int limitID /* = -1 */, const string targetNA /* = "" */)
	{
		switch (chatID)
		{
		case chat_all:
		{
			player_mgr.sendToAll(protocol, msg);
			break;
		}
		case chat_kingdom:
		{
			player_mgr.sendToSphere(protocol, msg, limitID);
			break;
		}
		case chat_area:
		{
			player_mgr.sendToArea(protocol, msg, limitID);
			break;
		}
		case chat_guild:
		{
			player_mgr.sendToGuild(protocol, msg, limitID);
			break;
		}
		case chat_player:
		{
			playerDataPtr target = player_mgr.getOnlinePlayer(targetNA);
			if (target == NULL)return 2;
			packageSender(target, msg[msgStr][1u][receiverDataStr]);
			target->sendToClient(protocol, msg);
			break;
		}
		case chat_team:
		{
			player_mgr.sendToAll(protocol, msg);
			break;
		}
		}
		return 0;
	}

	int chat_system::chatOption(const int chatID, const short protocol, Json::Value& msg, playerDataPtr player, const string targetNA /* = "" */)
	{
		switch (chatID)
		{
		case chat_all:
		{
			player_mgr.sendToAll(protocol, msg);
			break;
		}
		case chat_kingdom:
		{
			player_mgr.sendToSphere(protocol, msg, player->Base.getSphereID());
			break;
		}
		case chat_area:
		{
			player_mgr.sendToArea(protocol, msg, player->Base.getPosition().first);
			break;
		}
		case chat_guild:
		{
			player_mgr.sendToGuild(protocol, msg, player->Guild.getGuildID());
			break;
		}
		case chat_player:
		{
			playerDataPtr target = player_mgr.getOnlinePlayer(targetNA);
			if (target == NULL)return 2;
			packageSender(target, msg[msgStr][1u][receiverDataStr]);
			player->sendToClient(protocol, msg);
			target->sendToClient(protocol, msg);
			break;
		}
		case chat_team:
		{
			player_mgr.sendToAll(protocol, msg);
			break;
		}
		}
		return 0;
	}

	void chat_system::ChatShow(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		UnAcceptRetrun(js_msg[1u].isInt(), -1);
		UnAcceptRetrun(js_msg[2u].isInt(), -1);
		UnAcceptRetrun(js_msg[3u].isString(), -1);
		int option = js_msg[0u].asInt();
		int showID = js_msg[1u].asInt();
		if(showID <= SHOW::show_begin || showID >= SHOW::show_end)Return(r, -1);
		unsigned now = na::time_helper::get_current_time();
		if(d->Base.getLevel() < 30)Return(r, 100);
		if(now - d->showTime[showID - 1] < 60)Return(r, 101);
		int tID = js_msg[2u].asInt();
		string signNA = js_msg[3u].asString();
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = option;
		msg[msgStr][1u][chatShowID] = showID;
		packageSender(d, msg[msgStr][1u][senderDataStr]);
		if(showID == SHOW::show_pilot)
		{
			const playerPilot& cPilot = d->Pilots.getPlayerPilotExtert(tID);
			if(cPilot == playerPilots::NullPilotExtert)Return(r, -1);
			Json::Value cPJson = cPilot.package(true);	
			battleMemPtr mem = battleMember::Create(d, cPilot.pilotID, 0);
			if (mem != NULL)
			{
				for (unsigned i = 0; i < characterNum; ++i)
				{
					cPJson["attr"][i] = mem->getTotalChar(i);
				}
			}
			msg[msgStr][1u][showPilotStr] =cPJson;
		}else if(showID == SHOW::show_item)
		{
			playerItemPtr item = d->Item.getItem(tID);
			if(item == playerItemPtr())Return(r, -1);
			msg[msgStr][1u][showItemStr] = item->toJson();
		}
		int res = chatOption(option, gate_client::chat_show_resp, msg, d, signNA);
		if(res != 0)Return(r, res);
		d->showTime[showID - 1] = now;
	}

	void chat_system::playerChatVoice(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		if (js_msg.size() > 6)Return(r, -1);
		if (js_msg[5u].isNull() || js_msg[5u].asBool())Return(r, -1);
		UnAcceptRetrun(js_msg[0u].isString(), -1);
		string words = js_msg[0u].asString();
		if (words.length() < 1 || words.length() > 180)Return(r, -1);
		UnAcceptRetrun(js_msg[1u].isString(), -1);
		string keys = js_msg[1u].asString();
		if (keys.length() > 60)Return(r, -1);
		UnAcceptRetrun(js_msg[2u].isDouble(), -1);
		double bitRate = js_msg[2u].asDouble();
		UnAcceptRetrun(js_msg[3u].isInt(), -1);
		UnAcceptRetrun(js_msg[4u].isString(), -1);
		int option = js_msg[3u].asInt();
		string signNa = js_msg[4u].asString();

		if (gm_tools_mgr.is_from_gm_http(m._net_id))
		{
			int ret = 0;
			if (chat_all == option)
			{
				sendToAllByGM(words);
			}
			else if (chat_player == option)
			{
				ret = sendToPlayerByGm(signNa, words);
			}

			Return(r, ret);
		}

		AsyncGetPlayerData(m);
		if (d->Vip.getVipLevel() < 1 && d->Base.getLevel() < 15 && option == chat_all)Return(r, -1);
		if (option < chat_all || option > chat_player) Return(r, -1);
		unsigned now = get_current_time();
		if (now < d->ChatArray[option + 1] || now - d->ChatArray[option + 1] < ChatCD[option + 1]) Return(r, 1);
		if (isForbidden(d->playerID))	Return(r, 4);
		d->ChatArray[option + 1] = now;
		int res = 0;
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = option;
		packageSender(d, msg[msgStr][1u][senderDataStr]);
		msg[msgStr][1u][senderWordsStr] = words;
		msg[msgStr][1u][senderVoiceStr] = keys;
		msg[msgStr][1u][senderVoiceRateStr] = bitRate;
		
		playerManager::playerDataVec vec;
		switch (option)
		{
		case chat_all:
		{
			vec = player_mgr.onlinePlayer();
			break;
		}
		case chat_kingdom:
		{
			vec = player_mgr.onlinePlayerSphereID(d->Base.getSphereID());
			break;
		}
		case chat_area:
		{
			vec = player_mgr.onlinePlayerAreaID(d->Base.getPosition().first);
			break;
		}
		case chat_guild:
		{
			int GuildID = d->Guild.getGuildID();
			vec = guild_sys.getOnlineMember(GuildID);
			if (vec.empty())Return(r, 2);
			break;
		}
		case chat_player:
		{
			playerDataPtr target = player_mgr.getOnlinePlayer(signNa);
			if (target == NULL)Return(r, 3);
			packageSender(target, msg[msgStr][1u][receiverDataStr]);
			vec.push_back(d);
			vec.push_back(target);
			break;
		}
		}
		MsgSignOnline(vec, msg, gate_client::chat_voice_resp);
	}

	void chat_system::playerChat(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		if (js_msg.size() > 4)Return(r, -1);
		if (js_msg[3u].isNull() || js_msg[3u].asBool())Return(r, -1);
		UnAcceptRetrun(js_msg[0u].isString(), -1);
		string words = js_msg[0u].asString();
		if(words.length() < 1 || words.length() > 180)Return(r, -1);
		UnAcceptRetrun(js_msg[1u].isInt(), -1);
		UnAcceptRetrun(js_msg[2u].isString(), -1);
		int option = js_msg[1u].asInt();
		string signNa = js_msg[2u].asString();

		if (gm_tools_mgr.is_from_gm_http(m._net_id))
		{
			int ret = 0;
			if(chat_all == option)
			{
				sendToAllByGM(words);
			}
			else if (chat_player == option)
			{
				ret = sendToPlayerByGm(signNa, words);
			}

			Return(r, ret);
		}

		AsyncGetPlayerData(m);
		if(d->Vip.getVipLevel() < 1 && d->Base.getLevel() < 15 && option == chat_all)Return(r, -1);
		if(option < chat_all || option > chat_player) Return(r, -1);
		unsigned now = get_current_time();
		if(now < d->ChatArray[option + 1] || now - d->ChatArray[option + 1] < ChatCD[option + 1]) Return(r, 1);
		if (isForbidden(d->playerID))	Return(r, 4);
		d->ChatArray[option + 1] = now;
		int res = 0;
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = option;
		packageSender(d, msg[msgStr][1u][senderDataStr]);
		msg[msgStr][1u][senderWordsStr] = words; 
		playerManager::playerDataVec vec;
		switch(option)
		{
		case chat_all:
			{
				vec = player_mgr.onlinePlayer();
				break;
			}
		case chat_kingdom:
			{
				vec = player_mgr.onlinePlayerSphereID(d->Base.getSphereID());
				break;
			}
		case chat_area:
			{
				vec = player_mgr.onlinePlayerAreaID(d->Base.getPosition().first);
				break;
			}
		case chat_guild:
			{
				int GuildID = d->Guild.getGuildID();
				vec = guild_sys.getOnlineMember(GuildID);
				if(vec.empty())Return(r, 2);
				break;
			}
		case chat_player:
			{
				playerDataPtr target = player_mgr.getOnlinePlayer(signNa);
				if(target == NULL)Return(r, 3);
				packageSender(target, msg[msgStr][1u][receiverDataStr]);
				vec.push_back(d);
				vec.push_back(target);
				break;
			}
		}
		MsgSignOnline(vec, msg, gate_client::chat_send_to_part_resp);
	}

	void chat_system::gmForbidSpeakerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int forbidden_player = js_msg[0u].asInt();
		unsigned forbid_time = js_msg[1].asUInt();

		unsigned now = na::time_helper::get_current_time();

		for (vector<forbiddenPlayer>::iterator it = forbidPlayers.begin(); it != forbidPlayers.end(); it++)
		{
			if (it->playerID == forbidden_player)
			{
				if (it->startTime < now && now < it->endTime)
				{
					Return(r, 1);
				}
			}
		}

		forbiddenPlayer player_item;
		player_item.playerID = forbidden_player;
		player_item.startTime = now;
		player_item.endTime = now + forbid_time;
		forbidPlayers.push_back(player_item);
		saveForbiddenPlayer();
		Return(r, 0);
	}

	void chat_system::gmAllowSpeakerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int allow_player = js_msg[0u].asInt();

		bool need_modify = false;
		for (vector<forbiddenPlayer>::iterator it = forbidPlayers.begin(); it != forbidPlayers.end(); it++)
		{
			if (it->playerID == allow_player)
			{
				forbidPlayers.erase(it);
				need_modify = true;
				break;
			}
		}

		if (need_modify)
		{
			saveForbiddenPlayer();
			Return(r, 0);
		}
		else
		{
			Return(r, 1);
		}
	}

	void chat_system::gmForbidSpeakerUpdateReq(msg_json& m, Json::Value& r)
	{
		packageForbiddenPlayer(r[msgStr][1]);
		Return(r, 0);
	}

	void chat_system::gmRollBroadcastUpdateReq(msg_json& m, Json::Value& r)
	{
		packageRollBroacast(r[msgStr][1]);
		Return(r, 0);
	}

	void chat_system::gmRollBroadcastSetReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		Json::Value rollVal = js_msg[0u];

		if (rollVal.isMember(chatRollBroadcastId) && rollVal[chatRollBroadcastId].asInt() < -1)
		{
			Return(r, -1);
		}
		if (!rollVal.isMember(chatRollBroadcastId) || rollVal[chatRollBroadcastId].asInt() == -1)
		{
			rollVal[chatRollBroadcastId] = getNewRollId(vecRollBroadcast);
		}

		rollBroacast rb;
		rb.id_ = rollVal[chatRollBroadcastId].asInt();
		int start_time = rollVal[chatStartTime].asInt();
		int end_time = rollVal[chatEndTime].asInt();
		gm_tools_mgr.actTimePretreat(start_time);
		gm_tools_mgr.actTimePretreat(end_time);

		boost::posix_time::ptime pstart = boost::posix_time::from_time_t(start_time);
		tm tstart = boost::posix_time::to_tm(pstart);
		boost::posix_time::ptime pend = boost::posix_time::from_time_t(end_time);
		tm tend = boost::posix_time::to_tm(pend);
		if (tstart.tm_hour != 0 || tstart.tm_min != 0 || tstart.tm_sec != 0
			|| tend.tm_hour != 0 || tend.tm_min != 0 || tend.tm_sec != 0)
		{
			Return(r, 1);
		}

		rb.startZeroTime_ = start_time;
		rb.endZeroTime_ = end_time;
		string s = rollVal[chatRollBroadCastContent].asString();
		rb.contentJson_[senderWordsStr] = s;
		rb.interval_ = rollVal[chatRollBroadCastInterVal].asInt();
		rb.rollTimes_ = rollVal[chatRollTimes].asInt();
		for (unsigned i = 0; i < rollVal[chatRollTimePeriodList].size(); i++)
		{
			timePeriod tp;
			Json::Value temp = rollVal[chatRollTimePeriodList][i];
			tp.startFromZeroTime_ = temp[0u].asInt();
			tp.endFromZeroTime_ = temp[1].asInt();
			rb.vecPeriod_.push_back(tp);
		}

		bool isFind = false;
		for (unsigned i = 0; i < vecRollBroadcast.size(); i++)
		{
			if (vecRollBroadcast[i].id_ == rb.id_)
			{
				isFind = true;
				vecRollBroadcast[i] = rb;
				break;
			}
		}

		if (!isFind)
		{
			vecRollBroadcast.push_back(rb);
		}

		saveRollBroacast();
		Return(r, 0);
	}

	void chat_system::gmRollBroadcastDelReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int rollId = js_msg[0u].asInt();

		if (rollId == -1)
		{
			vecRollBroadcast.clear();
			saveRollBroacast();
		}
		else
		{
			bool isFind = false;
			for (vector<rollBroacast>::iterator it = vecRollBroadcast.begin(); it != vecRollBroadcast.end(); it++)
			{
				if (it->id_ == rollId)
				{
					vecRollBroadcast.erase(it);
					isFind = true;
					saveRollBroacast();
					break;
				}
			}

			if (!isFind)
			{//没有该广播
				Return(r, 1);
			}
		}
		Return(r, 0);
	}

	void chat_system::chatTableUpdate(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		playerDataPtr table;
		if(js_msg[0u].isString())
		{
			string playerNa =js_msg[0u].asString();
			table = player_mgr.getPlayerMainByName(playerNa);
		}
		else if(js_msg[0u].isInt())
		{
			int playerID =js_msg[0u].asInt();
			table = player_mgr.getPlayerMain(playerID);
		}
		else
		{
			Return(r, -1);
		}

		if(table == playerDataPtr())
		{
			if(m._post_times < 1)
			{
				player_mgr.postMessage(m);
				return;
			}
			else
			{
				Return(r, -1);
			}
		}

		r[msgStr][0u] = 0;
		r[msgStr][1u]["ol"] = table->netID < 0 ? false : true;
		r[msgStr][1u][playerWarStoryCurrentProcessStr] = table->Warstory.currentProcess;
		r[msgStr][1u][playerNameStr] = table->Base.getName();
		r[msgStr][1u][playerIDStr] = table->playerID;
		r[msgStr][1u][arenaRankStr] = table->Arena.getRank();
		r[msgStr][1u][playerSphereIDStr] = table->Base.getSphereID();
		r[msgStr][1u][office::playerOfficialLevelStr] = table->Office.getOffcialLevel();
		r[msgStr][1u][playerLevelStr] = table->Base.getLevel();
		r[msgStr][1u][playerFaceIDStr] = table->Base.getFaceID();
		r[msgStr][1u][playerAreaIDStr] = table->Base.getPosition().first;
		r[msgStr][1u][playerAreaPageStr] = table->Base.getPosition().second;
		static const string senderGuildName = "gna";
		r[msgStr][1u]["gna"] = guild_sys.getGuildName(table->Guild.getGuildID());
		r[msgStr][1u][playerPlanetIDStr] = table->Base.planetID;
		r[msgStr][1u]["ais"] = !(table->Ally.getAllyInfo().empty());
		r[msgStr][1u]["bv"] = table->Embattle.getCurrentBV();
	}

	void chat_system::chatStrategyBroadcastResp(playerDataPtr d, int local_id, string& report)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		packageSender(d, msg[msgStr][1u][attackerDataStr]);
		msg[msgStr][1u][arenaReportIdStr] = report;
		msg[msgStr][1u][senderChannelStr] = -1;
		msg[msgStr][1u][playerWarStoryIDStr] = local_id;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::first_pass_war_story;
		player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);
	}

	void chat_system::sendToAllArenaSummary(playerDataPtr d, int rank, int silver, int weiwang, int gold)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::arena_summary;
		msg[msgStr][1u][senderChannelStr] = -1;
		msg[msgStr][1u][arenaRankStr] = rank;
		if(rank > 0)
		{
			packageSender(d, msg[msgStr][1u][attackerDataStr]);
			msg[msgStr][1u][playerSilverStr] = silver;
			msg[msgStr][1u][playerWeiWangStr] = weiwang;
			msg[msgStr][1u][playerGoldStr] = gold;
		}
		player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);
	}

	void chat_system::chatRulerBroadcastResp(Json::Value& data, int channel, int arg)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u] = data;
		msg[msgStr][1u][senderChannelStr] = channel;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::ruler_broadcast;
		if(channel == -1)
			player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);
		else
		{
			playerManager::playerDataVec vec= player_mgr.onlinePlayerSphereID(arg);
			MsgSignOnline(vec, msg, gate_client::chat_broadcast_resp);
// 			for (unsigned i = 0; i < vec.size(); ++i)
// 			{
// 				playerDataPtr receiver = vec[i];
// 				player_mgr.sendToPlayer(receiver->playerID, receiver->netID, gate_client::chat_broadcast_resp, msg);
// 			}
		}

	}

	void chat_system::sendReportToAll(playerDataPtr d, playerDataPtr target, string filename)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		packageSender(d, msg[msgStr][1u][attackerDataStr]);
		packageSender(target, msg[msgStr][1u][defenderDataStr]);
		msg[msgStr][1u][arenaRankStr] = d->Arena.getRank();
		msg[msgStr][1u][arenaReportIdStr] = filename;
		msg[msgStr][1u][senderChannelStr] = -1;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::arean_report;
		player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);
	}

	void chat_system::sendToAll(playerDataPtr sender, string& str)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = -1;
		packageSender(sender, msg[msgStr][1u][senderDataStr]);
		msg[msgStr][1u][senderWordsStr] = str; 
		player_mgr.sendToAll(gate_client::chat_send_to_part_resp, msg);
	}

	void chat_system::sendToAllVipGetGift(playerDataPtr pdata, int vipLevel)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = chat_all;
		msg[msgStr][1u][playerNameStr] = pdata->Base.getName();
		msg[msgStr][1u]["vlv"] = vipLevel;
		msg[msgStr][1u][chatBroadcastID] = BROADCAST::vip_get_gift;
		player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);
	}

	void chat_system::sendToAllBroadCastCommon( int broadcastID, Json::Value chatContent )
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = chat_all;
		msg[msgStr][1u][chatBroadcastID] = broadcastID;
		if (chatContent != Json::Value::null)
			msg[msgStr][1u][senderDataStr] = chatContent;

		player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);
	}

	void chat_system::sendToTeamBroadCastCommon( int broadcastID, Json::Value chatContent )
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = chat_team;
		msg[msgStr][1u][chatBroadcastID] = broadcastID;
		if (chatContent != Json::Value::null)
			msg[msgStr][1u][senderDataStr] = chatContent;

		player_mgr.sendToAll(gate_client::chat_broadcast_resp, msg);
	}

	void chat_system::sendToAreaBroadCastCommon(int areaID, int broadcastID, Json::Value chatContent)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = chat_area;
		msg[msgStr][1u][chatBroadcastID] = broadcastID;
		if (chatContent != Json::Value::null)
			msg[msgStr][1u][senderDataStr] = chatContent;

		player_mgr.sendToArea(gate_client::chat_broadcast_resp, msg, areaID);
	}

	void chat_system::sendToGuildBroadCastCommon(int guildID, int broadcastID, Json::Value chatContent)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = chat_guild;
		msg[msgStr][1u][chatBroadcastID] = broadcastID;
		if (chatContent != Json::Value::null)
			msg[msgStr][1u][senderDataStr] = chatContent;

		player_mgr.sendToGuild(gate_client::chat_broadcast_resp, msg, guildID);
	}

	void chat_system::sendToSpereBroadCastCommon(int spereID, int broadcastID, Json::Value chatContent)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = chat_kingdom;
		msg[msgStr][1u][chatBroadcastID] = broadcastID;
		if (chatContent != Json::Value::null)
			msg[msgStr][1u][senderDataStr] = chatContent;

		player_mgr.sendToSphere(gate_client::chat_broadcast_resp, msg, spereID);
	}

	void chat_system::sendToAllRollBroadCastCommon(int broadcastID, int times, Json::Value chatContent)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatRollBroadcastID] = broadcastID;
		msg[msgStr][1u][chatRollTimes] = times;
		if (chatContent != Json::Value::null)
			msg[msgStr][1u][senderDataStr] = chatContent;

		player_mgr.sendToAll(gate_client::chat_roll_broadcast_resp, msg);
	}

	void chat_system::sendToAreaRollBroadCastCommon(int areaID, int broadcastID, int times, Json::Value chatContent)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatRollBroadcastID] = broadcastID;
		msg[msgStr][1u][chatRollTimes] = times;
		if (chatContent != Json::Value::null)
			msg[msgStr][1u][senderDataStr] = chatContent;

		player_mgr.sendToArea(gate_client::chat_roll_broadcast_resp, msg, areaID);
	}

	void chat_system::sendToGuildRollBroadCastCommon(int guildID, int broadcastID, int times, Json::Value chatContent)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatRollBroadcastID] = broadcastID;
		msg[msgStr][1u][chatRollTimes] = times;
		if (chatContent != Json::Value::null)
			msg[msgStr][1u][senderDataStr] = chatContent;

		player_mgr.sendToGuild(gate_client::chat_roll_broadcast_resp, msg, guildID);
	}

	void chat_system::sendToSpereRollBroadCastCommon(int spereID, int broadcastID, int times, Json::Value chatContent)
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][chatRollBroadcastID] = broadcastID;
		msg[msgStr][1u][chatRollTimes] = times;
		if (chatContent != Json::Value::null)
			msg[msgStr][1u][senderDataStr] = chatContent;

		player_mgr.sendToSphere(gate_client::chat_roll_broadcast_resp, msg, spereID);
	}

	void chat_system::sendToAllByGM(string& str)
	{
		Json::Value msg;
		msg[senderWordsStr] = str;
		sendToAllBroadCastCommon(BROADCAST::gm_chat_all, msg);
	}

	int chat_system::sendToPlayerByGm(const string player_name, string& str)
	{
		playerDataPtr target = player_mgr.getOnlinePlayer(player_name);
		if(target == NULL)return 3;//私聊对方不在线
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = chat_player;

		msg[msgStr][1u][chatBroadcastID] = BROADCAST::gm_chat_single;
		//		packageSender(target, msg[msgStr][1u][receiverDataStr]);	
		msg[msgStr][1u][senderDataStr][senderWordsStr] = str;
		player_mgr.sendToPlayer(target->playerID, target->netID, gate_client::chat_broadcast_resp, msg);
		//		cout << __FUNCTION__ << ":msg:" << msg.toStyledString();
		return 0;
	}

	int chat_system::sendToPlayer(const int player_id, playerDataPtr sender,string& str)
	{
		if(! player_mgr.IsOnline(player_id)) return 1;//私聊对方不在线
		playerDataPtr target = player_mgr.getOnlinePlayer(player_id);
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = chat_player;
		packageSender(sender, msg[msgStr][1u][senderDataStr]);
		packageSender(target, msg[msgStr][1u][receiverDataStr]);	
		msg[msgStr][1u][senderWordsStr] = str; 
		player_mgr.sendToPlayer(sender->playerID, sender->netID, gate_client::chat_send_to_part_resp, msg);
		player_mgr.sendToPlayer(target->playerID, target->netID, gate_client::chat_send_to_part_resp, msg);
		return 0;
	}

	bool chat_system::isForbidden( int player_id)
	{
		bool ret = false;
		unsigned cur_time = na::time_helper::get_current_time();
		for (vector<forbiddenPlayer>::iterator it = forbidPlayers.begin(); it != forbidPlayers.end(); it++)
		{
			if (it->playerID == player_id)
			{
				if (it->startTime < cur_time && cur_time < it->endTime)
				{
					ret = true;
					break;
				}
			}
		}

		return ret;
	}

	bool chat_system::saveForbiddenPlayer()
	{
		mongo::BSONObj key = BSON(chatKey << KEY_FORBIDDEN);
		mongo::BSONObjBuilder obj;

		obj << chatKey << KEY_FORBIDDEN;

		mongo::BSONArrayBuilder arrItem;
		for (unsigned i = 0; i < forbidPlayers.size(); i++)
		{
			mongo::BSONObjBuilder objItem;
			objItem << playerIDStr << forbidPlayers[i].playerID;
			objItem << chatStartTime << forbidPlayers[i].startTime;
			objItem << chatEndTime << forbidPlayers[i].endTime;
			arrItem << objItem.obj();
		}
		obj << chatForbiddenList << arrItem.arr();

		return db_mgr.save_mongo(chatDbName, key, obj.obj());
	}

	bool chat_system::getForbiddenPlayer()
	{
		mongo::BSONObj key = BSON(chatKey << KEY_FORBIDDEN);
		mongo::BSONObj obj = db_mgr.FindOne(chatDbName, key);

		if (!obj.isEmpty())
		{
			vector<BSONElement> sets = obj[chatForbiddenList].Array();
			forbidPlayers.clear();
			unsigned cur_time = na::time_helper::get_current_time();
			bool need_modify = false;
			for (unsigned i = 0; i < sets.size(); i++)
			{
				forbiddenPlayer player;
				player.playerID = sets[i][playerIDStr].Int();
				player.startTime = sets[i][chatStartTime].Int();
				player.endTime = sets[i][chatEndTime].Int();

				if (cur_time > player.endTime)
				{
					need_modify = true;
					continue;
				}

				forbidPlayers.push_back(player);
			}

			if (need_modify)
			{
				saveForbiddenPlayer();
			}

			return true;
		}
		return false;
	}

	bool chat_system::saveRollBroacast()
	{
		mongo::BSONObj key = BSON(chatKey << KEY_ROLLING_BROADCAST);
		mongo::BSONObjBuilder obj;

		obj << chatKey << KEY_ROLLING_BROADCAST;

		mongo::BSONArrayBuilder arrItem;
		for (unsigned i = 0; i < vecRollBroadcast.size(); i++)
		{
			mongo::BSONObjBuilder objItem;
			objItem << chatRollBroadcastId << vecRollBroadcast[i].id_;
			objItem << chatStartTime << vecRollBroadcast[i].startZeroTime_;
			objItem << chatEndTime << vecRollBroadcast[i].endZeroTime_;
			objItem << chatRollBroadCastInterVal << vecRollBroadcast[i].interval_;
			objItem << chatRollTimes << vecRollBroadcast[i].rollTimes_;
			objItem << chatRollBroadCastContent << vecRollBroadcast[i].contentJson_[senderWordsStr].asString();
			mongo::BSONArrayBuilder parr;
			for (unsigned j = 0; j < vecRollBroadcast[i].vecPeriod_.size(); j++)
			{
				mongo::BSONArrayBuilder tarr;
				timePeriod tp = vecRollBroadcast[i].vecPeriod_[j];
				tarr << tp.startFromZeroTime_;
				tarr << tp.endFromZeroTime_;
				parr << tarr.arr();
			}
			objItem << chatRollTimePeriodList << parr.arr();
			arrItem << objItem.obj();
		}
		obj << chatRollBroadCastList << arrItem.arr();

		return db_mgr.save_mongo(chatDbName, key, obj.obj());
	}

	bool chat_system::getRollBroacast()
	{
		mongo::BSONObj key = BSON(chatKey << KEY_ROLLING_BROADCAST);
		mongo::BSONObj obj = db_mgr.FindOne(chatDbName, key);

		if (!obj.isEmpty())
		{
			vector<BSONElement> sets = obj[chatRollBroadCastList].Array();
			vecRollBroadcast.clear();
			unsigned cur_time = na::time_helper::get_current_time();
			bool need_modify = false;
			for (unsigned i = 0; i < sets.size(); i++)
			{
				rollBroacast rollBr;
				rollBr.id_ = sets[i][chatRollBroadcastId].Int();
				rollBr.startZeroTime_ = sets[i][chatStartTime].Int();
				rollBr.endZeroTime_ = sets[i][chatEndTime].Int();
				rollBr.interval_ = sets[i][chatRollBroadCastInterVal].Int();
				checkNotEoo(sets[i][chatRollTimes])
					rollBr.rollTimes_ = sets[i][chatRollTimes].Int();
				string tmp = sets[i][chatRollBroadCastContent].String();
				rollBr.contentJson_[senderWordsStr] = tmp;
				if(!sets[i][chatRollTimePeriodList].eoo())
				{
					vector<mongo::BSONElement> vecPeriod = sets[i][chatRollTimePeriodList].Array();
					for (unsigned j = 0; j < vecPeriod.size(); j++)
					{
						timePeriod tp;
						tp.startFromZeroTime_ = vecPeriod[j].Array()[0u].Int();
						tp.endFromZeroTime_ = vecPeriod[j].Array()[1u].Int();
						rollBr.vecPeriod_.push_back(tp);
					}
				}
				else
				{//删除旧数据
					db_mgr.remove_collection(chatDbName, key);
					cout << __FUNCTION__ << " remove old roll broadcast" << endl;
					return false;
				}

				if (cur_time > rollBr.endZeroTime_ + 86400)
				{
					need_modify = true;
					continue;
				}

				vecRollBroadcast.push_back(rollBr);
			}

			if (need_modify)
			{
				saveRollBroacast();
			}

			return true;
		}
		return false;
	}

	void chat_system::packageRollBroacast(Json::Value& pack)
	{
		pack = Json::arrayValue;
		for (unsigned i = 0; i < vecRollBroadcast.size(); i++)
		{
			Json::Value temp;
			mongo::BSONObjBuilder objItem;
			temp[chatRollBroadcastId] = vecRollBroadcast[i].id_;
			int start_time = vecRollBroadcast[i].startZeroTime_;
			int end_time = vecRollBroadcast[i].endZeroTime_;
			gm_tools_mgr.actTimePretreat(start_time, -1);
			gm_tools_mgr.actTimePretreat(end_time, -1);
			temp[chatStartTime] = start_time;
			temp[chatEndTime] = end_time;
			temp[chatRollBroadCastInterVal] = vecRollBroadcast[i].interval_;
			temp[chatRollTimes] = vecRollBroadcast[i].rollTimes_;
			temp[chatRollBroadCastContent] = vecRollBroadcast[i].contentJson_[senderWordsStr].asString();
			temp[chatRollTimePeriodList] = Json::arrayValue;
			for (unsigned j = 0; j < vecRollBroadcast[i].vecPeriod_.size(); j++)
			{
				timePeriod tp = vecRollBroadcast[i].vecPeriod_[j];
				Json::Value tmpPrd;
				tmpPrd.append(tp.startFromZeroTime_);
				tmpPrd.append(tp.endFromZeroTime_);
				temp[chatRollTimePeriodList].append(tmpPrd);
			}
			pack.append(temp);
		}
	}

	int chat_system::getNewRollId(vector<rollBroacast> &vecRoll)
	{
		unsigned cur_time = na::time_helper::get_current_time();
		bool need_modify = false;
		int maxId = 0;
		for (vector<rollBroacast>::iterator it = vecRoll.begin(); it != vecRoll.end();)
		{
			if (cur_time > it->endZeroTime_ + 86400)
			{
				need_modify = true;
				it = vecRoll.erase(it);
			}
			else
			{
				if (maxId < it->id_)
				{
					maxId = it->id_;
				}
				it++;
			}
		}

		if (need_modify)
		{
			saveRollBroacast();
		}
		return maxId + 1;
	}

	void chat_system::packageForbiddenPlayer(Json::Value& pack)
	{
		pack = Json::arrayValue;

		for (unsigned i = 0; i < forbidPlayers.size(); i++)
		{
			Json::Value temp;
			temp[playerIDStr] = forbidPlayers[i].playerID;
			int start_time = forbidPlayers[i].startTime;
			int end_time = forbidPlayers[i].endTime;
			gm_tools_mgr.actTimePretreat(start_time, -1);
			gm_tools_mgr.actTimePretreat(end_time, -1);
			temp[chatStartTime] = start_time;
			temp[chatEndTime] = end_time;

			pack.append(temp);
		}
	}

	void chat_system::chatUpdate(boost::system_time& tmp)
	{//调小时间间隔，使得滚动公告能及时显示
		if((tmp - tick).total_seconds() < 2)return;

		tick = tmp;

		chatUpdateSingle(vecRollBroadcast);
		chatUpdateSingle(vecSystemRollBroadcast_);
	}

	void chat_system::chatUpdateSingle(vector <rollBroacast> &singleVec)
	{
		if (singleVec.size() == 0)
			return;

		unsigned now = na::time_helper::get_current_time();
		for (unsigned i = 0; i < singleVec.size(); i++)
		{
			rollBroacast &rb = singleVec[i];
			if (now - rb.latestSendTime_ >= rb.interval_ && rb.isValid(now))
			{
				sendToAllRollBroadCastCommon(rb.broadCastID_, rb.rollTimes_, rb.contentJson_);
				rb.latestSendTime_ = now;
			}
		}

		bool isModify = false;
		for (vector <rollBroacast>::iterator it = singleVec.begin(); it != singleVec.end();)
		{
			if (now > it->endZeroTime_ + 86400)
			{
				it = singleVec.erase(it);
				isModify = true;
			}
			else
			{
				it++;
			}
		}
	}

	void chat_system::addSystemRollBroadCast(int broadcastID, int interval, vector<timePeriod> vecPeriod, int channel/* = chat_all*/, int rollTimes /*= 3*/, Json::Value chatContent /*= Json::Value::null*/)
	{
		int curTime = na::time_helper::get_current_time();
		unsigned zeroTime = na::time_helper::get_time_zero(curTime);
		rollBroacast rb;
		rb.broadCastID_ = broadcastID;
		rb.interval_ = interval;
		rb.vecPeriod_ = vecPeriod;
		rb.contentJson_ = chatContent;
		rb.rollTimes_ = rollTimes;
		rb.startZeroTime_ = zeroTime;
		rb.endZeroTime_ = zeroTime;
		rb.channel_ = channel;
		rb.id_ = getNewRollId(vecSystemRollBroadcast_);
		vecSystemRollBroadcast_.push_back(rb);
	}

}

