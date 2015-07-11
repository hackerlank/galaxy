#pragma once

#include <boost/unordered_set.hpp>
#include "gate_game_protocol.h"

class DealMatch
{
public:
	inline static bool isMatchFTReq(const short protocol)
	{
		return FsReq_.find(protocol) != FsReq_.end();
	}

// 	inline static bool isMatchMSResp(const short protocol)
// 	{
// 		return FsResp_.find(protocol) != FsResp_.end();
// 	}

	inline static void initFilterProtocol()
	{

		//================gate filter ================//
		//name list
		insertFTReq(gate_client::gm_motify_player_name_req);
		insertFTReq(gate_client::guild_create_group_req);
		insertFTReq(gate_client::player_generate_role_req);
		insertFTReq(gate_client::player_change_name_req);
		insertFTReq(gate_client::guild_motify_words_req);
		insertFTReq(gate_client::world_area_motify_leave_message_req);
		insertFTReq(gate_client::email_write_to_player_req);
		insertFTReq(gate_client::gm_motify_guild_name_req);
		insertFTReq(gate_client::secretary_rename_req);
		//chat list
		insertFTReq(gate_client::chat_send_to_part_req);
		insertFTReq(gate_client::chat_voice_req);
		insertFTReq(gate_client::chat_danmu_req);
		insertFTReq(gate_client::paper_dispacth_paper_req);

		//===============gate multi send ==================//
		//chat list
// 		insertMSResp(gate_client::chat_send_to_part_resp);
// 		insertMSResp(gate_client::chat_voice_resp);
// 		insertMSResp(gate_client::chat_broadcast_resp);
// 		insertMSResp(gate_client::chat_roll_broadcast_resp);
// 		insertMSResp(gate_client::chat_show_resp); 
// 		insertMSResp(gate_client::chat_danmu_resp);
//		insertMSResp(gate_client::world_boss_remain_blood_resp);
	}
private:
	typedef boost::unordered_set<short> ForwardSet;
	inline static void insertFTReq(const short protocol)
	{
		FsReq_.insert(protocol);
	}
// 	inline static void insertMSResp(const short protocol)
// 	{
// 		FsResp_.insert(protocol);
// 	}
	static ForwardSet FsReq_;// , FsResp_;
};