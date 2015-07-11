#include "filter_handler.h"
#include "Glog.h"
#include "gate_game_protocol.h"
#include "filter_server.h"
#include "common_protocol.h"
#include "mineHeader.h"
#include "deal_protocol.hpp"

DealMatch::ForwardSet DealMatch::FsReq_;
//DealMatch::ForwardSet DealMatch::FsResp_;

#define ReadJsonArray(CHARPOINTER) \
	Json::Value js;\
	Json::Reader reader;\
	std::string recv_str = CHARPOINTER;\
	if(! reader.parse(recv_str, js)){\
	LogW << "recv string :" << string(m._json_str_utf8, m._total_len - json_str_offset) << LogEnd;\
	LogW << "json parse failed " << __FUNCTION__ << \
	"	player_id: " << m._player_id << "	type " << m._type << LogEnd;\
	return;\
	}\
	if(! js.isObject()){\
	LogW << "recv string :" << js.toStyledString() << LogEnd;\
	LogW << "json is not a object " << __FUNCTION__ << \
	"	player_id: " << m._player_id << "	type " << m._type << LogEnd;\
	return;\
	}\
	Json::Value& js_msg = js[msgStr];\
	if(!js_msg.isArray()){\
	LogW << "recv string :" << js.toStyledString() << LogEnd;\
	LogW << "ReadJsonArray error " << __FUNCTION__ << \
	"	player_id: " << m._player_id << "	type " << m._type << LogEnd;\
	return;\
	}\

namespace ft
{
	const static string msgStr = "msg";
	const static string senderWordsStr = "w";
	const static string playerListStr = "ls";

#define RegisterFunction(PROTOCOL, FUNCTION) \
{\
	f = boost::bind(&FUNCTION, _1); \
	ownMap_[PROTOCOL] = f;\
}

	static void changeGuildName(Json::Value& json)
	{
		if (json[0u].isString())
		{
			string str = json[0u].asString();
			size_t filt_sz = 0;
			bool ret = FILTER.filtName(str, filt_sz);
			json[0u] = str;
			json[3u] = ret;
		}
		else
		{
			json[3u] = false;
		}
	}

	static void createRoleName(Json::Value& json)
	{
		if (json[0u].isString())
		{
			string str = json[0u].asString();
			size_t filt_sz = 0;
			bool ret = FILTER.filtName(str, filt_sz);
			json[0u] = str;
			json[2u] = ret;
		}
		else
		{
			json[2u] = false;
		}
	}

	static void changeRoleName(Json::Value& json)
	{
		if (json[0u].isString())
		{
			string str = json[0u].asString();
			size_t filt_sz = 0;
			bool ret = FILTER.filtName(str, filt_sz);
			json[0u] = str;
			json[1u] = ret;
		}
		else
		{
			json[1u] = false;
		}
	}

	static void filterChatWords(Json::Value& json)
	{
		if (!json[0u].isString() || !json[1u].isInt() || !json[2u].isString())
		{
			json.append(true);
			return;
		}
		string words = json[0u].asString();
		size_t filt_sz = 0;
		FILTER.filtChat(words, filt_sz);
		json[0u] = words;
		json.append(false);
	}

	static void filterChatVoice(Json::Value& json)
	{
		if (!json[0u].isString())
		{
			json.append(true);
			return;
		}
		string words = json[0u].asString();
		size_t filt_sz = 0;
		FILTER.filtChat(words, filt_sz);
		json[0u] = words;
		json.append(false);
	}

	static void filterLeaveWords(Json::Value& json)
	{
		string str = json[0u].asString();
		size_t filt_sz = 0;
		bool ret = FILTER.filtChat(str, filt_sz);
		json[0u] = str;
	}

	static void filterEmail(Json::Value& json)
	{
		string str = json[1u]["lw"].asString();
		size_t filt_sz = 0;
		bool ret = FILTER.filtChat(str, filt_sz);
		json[1u]["lw"] = str;
	}

	static void filterGuildNameGM(Json::Value& json)
	{
		string str = json[1u].asString();
		size_t filt_sz = 0;
		bool ret = FILTER.filtName(str, filt_sz);
		json[1u] = str;
		json[2u] = ret;
	}

	static void filterPaper(Json::Value& json)
	{
		string str = json[3u].asString();
		size_t filt_sz = 0;
		bool ret = FILTER.filtChat(str, filt_sz);
		json[3u] = str;
		json.append(ret);
	}

	static void filterDanMu(Json::Value& json)
	{
		string str = json[0u].asString();
		size_t filt_sz = 0;
		bool ret = FILTER.filtChat(str, filt_sz);
		json[0u] = str;
	}

	static void filterSecretaryName(Json::Value& json)
	{
		string str = json[1u].asString();
		size_t filt_sz = 0;
		bool ret = FILTER.filtName(str, filt_sz);
		json[1u] = str;
		json[2u] = ret;
	}

	handler::handler()
	{
		FilterFunc f;
		RegisterFunction(gate_client::guild_create_group_req, changeGuildName);
		RegisterFunction(gate_client::player_generate_role_req, createRoleName);
		RegisterFunction(gate_client::player_change_name_req, changeRoleName);
		RegisterFunction(gate_client::gm_motify_player_name_req, changeRoleName);
		RegisterFunction(gate_client::chat_send_to_part_req, filterChatWords);
		RegisterFunction(gate_client::chat_voice_req, filterChatVoice);
		RegisterFunction(gate_client::guild_motify_words_req, filterLeaveWords);
		RegisterFunction(gate_client::world_area_motify_leave_message_req, filterLeaveWords);
		RegisterFunction(gate_client::email_write_to_player_req, filterEmail);
		RegisterFunction(gate_client::gm_motify_guild_name_req, filterGuildNameGM);
		RegisterFunction(gate_client::chat_danmu_req, filterDanMu);
		RegisterFunction(gate_client::secretary_rename_req, filterSecretaryName);
		RegisterFunction(gate_client::paper_dispacth_paper_req, filterPaper);
	}

	handler::~handler()
	{

	}

	void handler::recv_client_handler(tcp_session_ptr session, msg_json& m)
	{
		session->keep_alive();
		if(m._type == game_protocol::comomon::keep_alive_req)
		{
			string str;
			na::msg::msg_json mj(game_protocol::comomon::keep_alive_resp,str);
			session->write_json_msg(mj);
			return;
		}
		try
		{
			ReadJsonArray(m._json_str_utf8);
			dispatchMap::iterator it = ownMap_.find(m._type);
			if (it != ownMap_.end())
			{
				it->second(js_msg);
			}
// 			if (m._type == gate_client::chat_send_to_part_resp ||
// 				m._type == gate_client::chat_voice_resp)
// 			{
// 				Json::Value playerJson = js_msg[1u][playerListStr];
// 				js_msg[1u].removeMember(playerListStr);
// 				string str = js.toStyledString();
// 				na::msg::msg_json mj(m._player_id, m._net_id, m._type, str);
// 				for (unsigned i = 0; i < playerJson.size(); ++i)
// 				{
// 					Json::Value& PackJson = playerJson[i];
// 					mj._player_id = PackJson[0u].asInt();
// 					mj._net_id = PackJson[1u].asInt();
// 					filter_svr->async_send_to_gate(mj);
// 				}
// 			}
			if (DealMatch::isMatchFTReq(m._type))
			{
				string str = js.toStyledString();
				na::msg::msg_json mj(m._player_id, m._net_id, m._type, str);
				filter_svr->async_send_to_gate(mj);
			}
			else
			{
				filter_svr->async_send_to_gate(m);
			}
		}
		catch(std::exception& e)
		{
			LogE << e.what() << LogEnd;
		}
	}
}