#include "gate_handle.h"
#include "common_protocol.h"
#include "gate_server.h"
#include "service_config.h"
#include "xor.h"
#include "gate_game_protocol.h"
#include "gate_account_protocol.h"
#include "deal_protocol.hpp"
#ifndef WIN32
#include "DebugInfo.h"
#endif

DealMatch::ForwardSet DealMatch::FsReq_;
//DealMatch::ForwardSet DealMatch::FsResp_;

namespace gt
{
	handler::handler()
	{

	}

	handler::~handler()
	{

	}

	bool handler::isCatch(const short protocol, const int fID)
	{
		_MP_ msg = getMsg(fID);
		if (!msg.isVaild())return false;
		if (msg._catchID != protocol)return false;
		return true;
	}

	handler::_MP_ handler::getMsg(const int ID)
	{
		MsgIt it = MsgMap.find(ID);
		if (it == MsgMap.end())return _MP_();
		return it->second;
	}

	void handler::recv_local_handler(tcp_session_ptr session, const char* data_ptr, int len)
	{
		if (len < (int)sizeof(na::msg::msg_base)) return;
		na::msg::msg_base* p = (na::msg::msg_base*)data_ptr;

		if (session->get_net_id() == service::process_id::GAME_NET_ID)
		{
			if (p->_type > gate_client::g2c_begin && p->_type < gate_client::g2c_end)
			{
				// 				if (DealMatch::isMatchResp(p->_type))
				// 				{
				// 					gate_svr.async_send_filter_svr(data_ptr, len);
				// 					return;
				// 				}
				if (gate_client::game_set_gate_buffer_resp == p->_type)
				{
					na::msg::msg_json::ptr mj_ptr = na::msg::msg_json::create(data_ptr, len);
					cc::convert_binary((char*)mj_ptr->_json_str_utf8, mj_ptr->_total_len - na::msg::json_str_offset);
					MsgMap[p->_net_id] = MsgPackage(p->_player_id, mj_ptr);
					return;
				}
				if (gate_client::game_clear_gate_buffer_resp == p->_type)
				{
					MsgMap.erase(p->_net_id);
					return;
				}

				//if (DealMatch::isMatchMSResp(p->_type))
				if (isCatch(p->_type, p->_net_id % 100))
				{
					_MP_ msg = getMsg(p->_net_id % 100);
					p->_net_id = p->_net_id / 100;
					if (!msg.isVaild())return;
					na::msg::msg_json player_mj(*p, *(msg._Msg));
					gate_svr.async_send_client_without_internal_conver(player_mj);
					return;
				}

				if (http_conn_mgr->is_from_gm_http(p->_net_id)
					|| http_conn_mgr->is_from_other_http(p->_net_id))
				{
					na::msg::msg_json::ptr pmsg = na::msg::msg_json::create(data_ptr, len);
					http_conn_mgr->write_msg(*pmsg);
				}
				else
					if (p->_net_id == service::process_id::ACCOUNT_NET_ID)
					{
						gate_svr.async_send_account_svr(data_ptr, len);
					}
					else
					{
						gate_svr.async_send_client(p->_net_id, p->_player_id, data_ptr, len);
					}

				return;
			}
		}
		else if (session->get_net_id() == service::process_id::ACCOUNT_NET_ID)
		{
			if (p->_type == protocol::l2c::login_resp){
				if (p->_net_id < 0 || p->_player_id <= 0)
				{
					p->_net_id /= na::net::vNetOffset;
					gate_svr.async_send_client(p->_net_id, p->_player_id, data_ptr, len, true);
					return;
				}
				gate_svr.set_session_id(p->_net_id, p->_player_id);
				p->_net_id /= na::net::vNetOffset;
				gate_svr.async_send_client(p->_net_id, p->_player_id, data_ptr, len, true);
			}
			else
				if (p->_type == protocol::l2c::update_session_key_resp)
				{
					gate_svr.async_send_client(p->_net_id, p->_player_id, data_ptr, len);
				}
			return;
		}
		else if (session->get_net_id() == service::process_id::FILTER_NET_ID)
		{
			// 			if (DealMatch::isMatchResp(p->_type))
			// 			{
			// 				gate_svr.async_send_client(p->_net_id, p->_player_id, data_ptr, len);
			// 				return;
			// 			}
			gate_svr.async_send_gamesvr(data_ptr, len);
		}
	}

	void handler::recv_client_handler(tcp_session_ptr session, const char* data_ptr, int len)
	{
		if (len < (int)sizeof(na::msg::msg_base)) return;
		na::msg::msg_base* p = (na::msg::msg_base*)data_ptr;

		if (session->get_net_id() < 0){
			recv_local_handler(session, data_ptr, len);
			return;
		}

		p->_net_id = session->get_net_id();
		p->_player_id = session->_player_id;
		char* msg_point = (char*)(data_ptr + na::msg::json_str_offset);
		cc::convert_binary(msg_point, p->_total_len - na::msg::json_str_offset);
		session->keep_alive();

		if (gate_client::gm_protocal_start_req < p->_type && p->_type < gate_client::gm_protocal_end_req)
		{
			return;
		}

		if (p->_type == protocol::c2l::login_req){
			na::msg::msg_json::ptr mjPtr = na::msg::msg_json::create(data_ptr, len);
			Json::Value val;
			Json::Reader r;
			r.parse(mjPtr->_json_str_utf8, val);
			if (!val.isMember("msg"))return;
			Json::Value& msgJson = val["msg"];
			std::string strIP = session->socket().remote_endpoint().address().to_string();
			msgJson.append(strIP);

			std::string s = val.toStyledString();
			na::msg::msg_json toAccount(protocol::c2l::login_req, s);
			//toAccount._net_id = mjPtr->_net_id;
			toAccount._net_id = session->get_vN_net_id();
			toAccount._player_id = mjPtr->_player_id;

			gate_svr.async_send_account_svr(toAccount);
			return;
		}

		if (DealMatch::isMatchFTReq(p->_type))
		{
			gate_svr.async_send_filter_svr(data_ptr, len);
			return;
		}

		if (p->_type == protocol::c2l::update_session_key_req)
		{
			gate_svr.async_send_account_svr(data_ptr, len);
			return;
		}

		if (p->_type == game_protocol::comomon::keep_alive_req)
		{
			string str;
			na::msg::msg_json mj(game_protocol::comomon::keep_alive_resp, str);
			session->write_json_msg(mj);
			return;
		}

		if (session->get_session_status() < gs_game)
			return;

		if (p->_type > gate_client::c2g_start && p->_type < gate_client::c2g_end)
		{
			gate_svr.async_send_gamesvr(data_ptr, len);
		}
	}

	void handler::client_connect_handler(tcp_session_ptr session, int error_value)
	{
		if (session->get_net_id() < 0)
			gate_svr.client_connect_handler(session, error_value);
		else
			gate_svr.kick_async(session, false);
	}
}