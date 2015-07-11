#pragma once

#include "tcp_session.h"
#include "msg_base.h"
#include "net_handler.h"
#include <string>

namespace db
{
	class handler : 
		public na::net::net_handler
	{		
	public:
		typedef boost::shared_ptr<na::net::tcp_session> tcp_session_ptr;
		handler();
		~handler();

		virtual void recv_client_handler(tcp_session_ptr session,const char* data_ptr,int len);
		virtual void client_connect_handler	(tcp_session_ptr session,int error_value);

	private:
		void sendMessage(tcp_session_ptr session, Json::Value& msg);
	};
}

