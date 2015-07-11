#pragma once

#include "server_base_direct.h"
#include "Glog.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
#include <json/json.h>
#include <iostream>
#include "gate_handle.h"
#include "http_server.hpp"

using namespace std;
using boost::asio::ip::tcp;

#define gate_svr boost::detail::thread::singleton<gt::gate>::instance()

namespace na
{
	namespace net
	{
		class tcp_session;
	}
}

namespace gt
{
	const static string allow_ip_login_file = "./server/allow_ip_login.json";
	struct GTData
	{
		int client_limit;
		size_t buffer;
		size_t client_buffer;
		int port;
		int net_id;
		bool is_tx;
		string _game_ip;
		string _game_port;
// 		string _fee_ip;
// 		string _fee_port;
		string _account_ip;
		string _account_port;
		string _filter_ip;
		string _filter_port;
		string _http_port;
		vector<string> _gm_tool_ip;
		vector<string> _limit_ip;
		int server_id;
	};

	class gate 
		//public na::net::serverdirect//,boost::enable_shared_from_this<gate>
	{
		typedef boost::shared_ptr<gt::gate> this_prt;
		typedef boost::shared_ptr<na::net::tcp_session> tcp_session_ptr;
		typedef boost::shared_ptr<tcp::acceptor>			acceptor_pointer;
		typedef boost::unordered_map<int,tcp_session_ptr>	client_map;					
		typedef boost::shared_ptr< net_handler>				net_handler_ptr;
		typedef boost::shared_ptr<http::server::http_server> http_svr_pointer;
	public:
		gate();
		~gate();

		void set_session_id(const int net_id, const int player_id);
		void async_send_to_all(na::msg::msg_json& mj);
		void async_send_to_all(const char* data,int len);
		bool async_send_client_without_internal_conver(na::msg::msg_json& mj, bool force = false);
		bool async_send_client(int net_id, int player_id, const char* data,int len, bool force = false);
		void async_send_gamesvr(const char* data,int len);
		void async_send_gamesvr(na::msg::msg_json& mj);
		void async_send_account_svr( const char* data,int len );
		void async_send_account_svr(msg_json& mj);
		void async_send_filter_svr( const char* data,int len );
		void async_send_filter_svr(msg_json& mj);
//		void async_send_fee_svr( const char* data,int len );
		void init(bool is_limit_ip);
		void client_connect_handler(tcp_session_ptr session,int error_value);
		void kick_async(tcp_session_ptr session, bool notify);
		void stop();
		void run();
		void on_http_req(http::server::connection* conn_ptr, Json::Value& http_req);
//		void sens_word_filter(string& sword);
		void printOnline();
	private:
		void connect_filter_svr();
		void connect_account_svr();
//		void connect_fee_svr();
		void connect_game_svr();
		NetInsPtr  create_net_id();
		void on_update();
		bool is_go_on_accept();
		bool is_allow_client_login(tcp_session_ptr session_ptr);
		void handle_accept(tcp_session_ptr new_connection,acceptor_pointer ap,const boost::system::error_code& error);
		void start_accept(acceptor_pointer ap);
		void sendheartbeat();
		void checkkick(boost::system_time& now);
		void kick(tcp_session_ptr session, bool notify /* = false */);		
		void connect(tcp_session_ptr connector,const char* ip_str, const char* port_str);
		void handle_connect(tcp_session_ptr connector,const boost::system::error_code& error);
		void create_http_svr(string svr_port, vector<string> gm_tool_ip);
		void initAllowIpList(bool is_limit_ip);
//		void init_chat_filter();
		void process_gm_req(http::server::connection* conn_ptr, Json::Value& http_req);
		GTData _gdata;
		boost::system_time _st, _console;
		tcp_session_ptr _game_server;
//		tcp_session_ptr _fee_server;
		tcp_session_ptr _account_server;
		tcp_session_ptr _filter_server;
 		boost::mutex		_mutex;
 		client_map			_map_client, _player;
		bool					_is_stop;
		acceptor_pointer		_ap;
		net_handler_ptr         _hanlder_point;
		http_svr_pointer _http_server;
// 		Json::Value _json_sens_words;
// 		vector <string> _harmony_words;
//		bool _first_connect_game;
		NetCreator* Creator;
	};
}
