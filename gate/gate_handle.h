#pragma once

#include "tcp_session.h"
#include "msg_base.h"
#include "net_handler.h"
#include <boost/unordered_map.hpp>
#include "gate_game_protocol.h"
using namespace na::net;
using namespace na::msg;

namespace gt
{
	class handler :
		public na::net::net_handler
	{
	public:
		typedef boost::shared_ptr<tcp_session> tcp_session_ptr;
		handler();
		~handler();

		virtual void recv_client_handler(tcp_session_ptr session, const char* data_ptr, int len);
		virtual void recv_local_handler(tcp_session_ptr session, const char* data_ptr, int len);
		virtual void client_connect_handler(tcp_session_ptr session, int error_value);
	private:
		typedef struct MsgPackage
		{
			MsgPackage()
			{
				_catchID = gate_client::resp_game_gate_null_protocol;
			}
			MsgPackage(const short catchID, na::msg::msg_json::ptr msg)
			{
				_Msg = msg;
				_catchID = catchID;
			}
			inline bool isVaild()
			{
				return _catchID != gate_client::resp_game_gate_null_protocol;
			}
			na::msg::msg_json::ptr _Msg;
			short _catchID;
		}_MP_;
		typedef boost::unordered_map < int, MsgPackage> multiMsgMap;
		typedef multiMsgMap::iterator MsgIt;
		multiMsgMap MsgMap;
		_MP_ getMsg(const int ID);
		bool isCatch(const short protocol, const int fID);
	};
}

