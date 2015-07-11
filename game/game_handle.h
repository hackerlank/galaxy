#pragma once

#include "tcp_session.h"
#include "msg_base.h"
#include "helper.h"
#ifndef WIN32
#include "DebugInfo.h"
#endif
using namespace na::net;
using namespace na::msg;


namespace gg
{
	class helper;
	class handler
	{		
	public:
		typedef boost::shared_ptr<tcp_session>	tcp_session_ptr;
		typedef boost::function< void ( na::msg::msg_json&, Json::Value&) >	handler_function;	
		typedef boost::function< void ( boost::system_time& ) > update_function;
		typedef	boost::shared_ptr<gg::handler>	pointer;
		struct CallFunction{
			CallFunction();
			handler_function f_;
			short resp_;
		};
		struct updateFunction
		{
			updateFunction();
			update_function f_;
			short state_;
		};
		typedef std::map< short, CallFunction >	function_keeper;
		typedef std::vector< updateFunction > function_vector;

		handler();
		~handler();

		void internalCallback(msg_json::ptr ptr);
		void recv_client_handler(tcp_session_ptr session, msg_json& m);
		void recv_local_handler(tcp_session_ptr session, msg_json& m);
		void on_system_operating();
		void logShowState(const bool state);
	protected:
		void reg_func(const short command_id, const short response_id, handler_function func);
		void despatch(na::msg::msg_json& m);
		
		function_keeper func_keeper;
		function_vector  func_vector;
		bool showLog;
	};
}

