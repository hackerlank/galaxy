//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "http_server.hpp"
#include <boost/bind.hpp>
#include "msg_server.h"
#include "service_config.h"
#include "core_helper.h"

namespace http {
	namespace server {

		http_server::http_server(const std::string& port, vector<std::string> gm_tool_ip,
			const std::string& doc_root)
		{
			// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
			std::string svr_addr = "127.0.0.1";
			//	std::string svr_addr = "0.0.0.0";
			int iport = boost::lexical_cast<int, string>(port);
			http_acceptor_ = acceptor_pointer(new tcp::acceptor(coreM.get_io(gt::http_io), tcp::endpoint(tcp::v4(), iport)));
			http_acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			http_acceptor_->listen();
			gm_tool_ip_ = gm_tool_ip;
			http_conn_mgr->init();
			start_accept();
		}

		void http_server::stop()
		{
			// Post a call to the stop function so that server::stop() is safe to call
			// from any thread.
			handle_stop();
		}

		void http_server::start_accept()
		{
			connection_ptr new_connection = http_conn_mgr->get_new_connection();
			http_acceptor_->async_accept(new_connection->socket(),
				boost::bind(&http_server::handle_accept, this,new_connection,
				boost::asio::placeholders::error));
		}

		void http_server::handle_accept(connection_ptr new_connection,const boost::system::error_code& e)
		{
			if (!e)
			{
				std::string connect_ip = new_connection->socket().remote_endpoint().address().to_string();
				vector<std::string>::iterator it_ip = find(this->gm_tool_ip_.begin(), this->gm_tool_ip_.end(), connect_ip);
				if (it_ip != this->gm_tool_ip_.end())
				{
					http_conn_mgr->start_connection(new_connection, http::server::GM_CONNECITON_TYPE);
				}
				else
				{//目前先去掉
//					http_conn_mgr->start_connection(new_connection, http::server::OTHER_CONNECTIOPN_TYPE);
				}
				
				start_accept();
			}
		}

		http_server::ptr http_server::create( std::string svr_port, vector<std::string> gm_tool_ip)
		{
			string doc_root = "F:";
			void* m = nedalloc::nedmalloc(sizeof(http_server));
			return http_server::ptr(new(m) http_server(svr_port, gm_tool_ip, doc_root),destroy);
		}

		void http_server::destroy(http_server* p)
		{
			p->~http_server();
			nedalloc::nedfree(p);
		}

		void http_server::handle_stop()
		{
			// The server is stopped by cancelling all outstanding asynchronous
			// operations. Once all operations have finished the io_service::run() call
			// will exit.
			http_acceptor_->close();
			http_conn_mgr->stop_all();
		}

	} // namespace server
} // namespace http
