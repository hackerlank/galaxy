#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <queue>
#include "msg_base.h"
#include <json/json.h>
#include "net_handler.h"
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>
#include "net_creator.h"

using boost::asio::ip::tcp;

namespace na
{
	namespace net
	{
		class tcp_session;
		class serverbase : 
			public net_handler
			,public boost::enable_shared_from_this<serverbase>
		{
		public:
			typedef boost::shared_ptr<na::net::tcp_session>		tcp_session_ptr;
			typedef boost::unordered_map<int,tcp_session_ptr>	client_map;					
			typedef	std::queue<na::msg::msg_json::ptr>			msg_queue;
			typedef boost::shared_ptr< net_handler>				net_handler_ptr;
			typedef boost::shared_ptr<tcp::acceptor>			acceptor_pointer;
			serverbase(void);
			virtual ~serverbase(void);

			void run();
			void stop();

			inline bool is_stop() { return _is_stop;}

			virtual	void on_connect(int client_id,tcp_session_ptr conn_ptr){}
			virtual	void on_disconnect(int client_id,tcp_session_ptr conn_ptr);
			virtual	void on_recv_msg(const na::msg::msg_json::ptr recv_msg_ptr){}

			virtual	void on_update(){}
			virtual void on_stop(){}

			void send_msg(int client_id,na::msg::msg_json& m);
			void send_msg(tcp_session_ptr conn_ptr,const na::msg::msg_json::ptr m);

			void recv_client_handler	(tcp_session_ptr session,const char* data_ptr,int len);
			virtual void client_connect_handler	(tcp_session_ptr session,int error_value);

		protected:
			virtual NetInsPtr create_net_id() = 0;
			virtual bool is_go_on_accept() = 0;
			void handle_accept(tcp_session_ptr new_connection,acceptor_pointer ap,const boost::system::error_code& error);
			virtual void start_accept(acceptor_pointer ap) = 0;

			acceptor_pointer		_ap;
			boost::mutex			_mutex;
			client_map				_map_client;
			msg_queue				_msg_queue;
			boost::mutex			_queue_mutex;
			bool					_is_stop;
		};
	}
}




