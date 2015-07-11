#pragma once

#include "msg_server.h"
#include "Glog.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
#include <json/json.h>
#include <iostream>
#include "game_handle.h"

using namespace std;
using boost::asio::ip::tcp;

#define game_svr gg::game::getInstance()

namespace na
{
	namespace net
	{
		class tcp_session;
	}
}
using namespace na::net;
namespace gg
{
	struct GGData
	{
		int client_limit;
		size_t buffer;
		int port;
		bool is_tx;
		string _mongoDB;
		string _sql_ip;
		string _sql_port;
	};

	class game : 
		public na::net::serverbase
	{
		typedef boost::shared_ptr<gg::game> this_ptr;
		typedef boost::shared_ptr<na::net::tcp_session> tcp_session_pointer;
		typedef boost::shared_ptr<handler> game_handle_pointer;
	public:
		static this_ptr getInstance();
		game();
		~game();
		void async_send_to_db(const char* data, int len);
		void async_send_to_db(na::msg::msg_json& mj);
		void async_send_to_gate(const char* data, int len);
		void async_send_to_gate(na::msg::msg_json& mj);
		void async_send_to_gate(const short protocol, Json::Value& msg, const int netID = -1, const int playerID = -1);
		virtual bool init();
		virtual void on_update();
		virtual void on_recv_msg(const na::msg::msg_json::ptr recv_msg_ptr);
		virtual void client_connect_handler(tcp_session_ptr session,int error_value);
		void internalCallback(na::msg::msg_json::ptr msg_ptr);
		void logShowState(const bool state);
	protected:
		virtual NetInsPtr  create_net_id();
		virtual bool is_go_on_accept();
		virtual void start_accept(acceptor_pointer ap);
	private:
		void connect_log_svr();
		void sendheartbeat();
		void checkkick(boost::system_time& now);
		void kick(tcp_session_ptr session, bool notify = false);
		void kick_async(tcp_session_pointer session);
		boost::system_time tick;
		void connect(tcp_session_pointer connector,const char* ip_str, const char* port_str);
		void handle_connect(tcp_session_pointer connector,const boost::system::error_code& error);
		int _net_id;
		GGData _gdata;
		tcp_session_pointer _log_server;
		game_handle_pointer _game_handler;
		bool showLog;
	};
}
