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
#include "net_handler.h"
#include "net_creator.h"

using boost::asio::ip::tcp;

namespace na
{
	namespace net
	{
		typedef struct serverdata
		{
			int client_limit;
			unsigned int buffer;
			int port;
			int net_id;
			bool is_tx;
		}SData;

		class net_handler;
		class tcp_session;
		class serverdirect
		{
		public:
			typedef boost::shared_ptr<na::net::tcp_session>		tcp_session_ptr;
			typedef boost::unordered_map<int,tcp_session_ptr>	client_map;					
			typedef	std::queue<na::msg::msg_json::ptr>			msg_queue;
			typedef boost::shared_ptr< net_handler>				net_handler_ptr;
			typedef boost::shared_ptr<tcp::acceptor>			acceptor_pointer;
			serverdirect(void);
			virtual ~serverdirect(void);

			void init(std::string cfg);
			void run();
			void stop();
			const std::string& get_cfg_name() const { return _cfg_name;}

			virtual void on_init(){}
			virtual void on_stop(){}

			inline bool is_stop() { return _is_stop;}

			virtual	void on_connect(int client_id,tcp_session_ptr conn_ptr){}

			void send_msg(int client_id,na::msg::msg_json& m);
			void send_msg(tcp_session_ptr conn_ptr,const na::msg::msg_json::ptr m);
		protected:
			virtual NetInsPtr create_net_id() = 0;
			virtual bool is_go_on_accept();
		private:
			void start_accept(acceptor_pointer ap);
			void handle_accept(tcp_session_ptr new_connection,acceptor_pointer ap,const boost::system::error_code& error);


		protected:
			acceptor_pointer		_ap;
			boost::mutex			_mutex;
			SData					_sdata;
			client_map				_map_client;
			bool					_is_stop;
			std::string				_cfg_name;
			net_handler_ptr         _hanlder_point;
		};
	}
}