#pragma once

#include "tcp_session.h"
#include "msg_base.h"
#include "filter.h"
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#ifndef WIN32
#include "DebugInfo.h"
#endif
using namespace na::net;
using namespace na::msg;

namespace ft
{
	class handler
	{		
	public:
		typedef boost::shared_ptr<tcp_session>	tcp_session_ptr;
		typedef	boost::shared_ptr<ft::handler>	pointer;
		handler();
		~handler();

		void recv_client_handler(tcp_session_ptr session, msg_json& m);
	private:
		typedef boost::function<void(Json::Value& json) > FilterFunc;
		typedef boost::unordered_map < short, FilterFunc > dispatchMap;
		dispatchMap ownMap_;
	};
}