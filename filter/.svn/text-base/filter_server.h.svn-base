#pragma once

#include "msg_server.h"
#include "filter_handler.h"
#include "Glog.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
#include <json/json.h>
#include <iostream>

#define filter_svr ft::filter::getInstance()

namespace ft
{
	struct FTData
	{
		size_t buffer;
		int port;
		bool is_tx;
	};

	class filter :
		public na::net::serverbase
	{
		typedef boost::shared_ptr<ft::filter> this_ptr;
		typedef boost::shared_ptr<na::net::tcp_session> tcp_session_pointer;
		typedef boost::shared_ptr<handler> filter_handle_pointer;
	public:
		static this_ptr getInstance();
		filter();
		~filter();
		void async_send_to_gate(const char* data, int len);
		void async_send_to_gate(na::msg::msg_json& mj);
		virtual void init();
		virtual void on_update(const int IOID);
		virtual void system_run();
		virtual void on_recv_msg(const na::msg::msg_json::ptr recv_msg_ptr);
	protected:
		virtual NetInsPtr  create_net_id();
		virtual bool is_go_on_accept();
		virtual void start_accept(acceptor_pointer ap);
	private:
		void checkkick(boost::system_time& now);
		void kick(tcp_session_ptr session, bool notify = false);
		void kick_async(tcp_session_pointer session);
		boost::system_time tick;
		FTData _fdata;
		filter_handle_pointer filter_handler;
	};
}