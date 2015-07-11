//
// connection_manager.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>
#include <boost/noncopyable.hpp>
#include "connection.hpp"

#define http_conn_mgr http::server::connection_manager::getInstance()

namespace http {
	namespace server {

		/// Manages open connections so that they may be cleanly stopped when the server
		/// needs to shut down.
		class connection_manager
			: private boost::noncopyable
		{
			typedef boost::shared_ptr<http::server::connection_manager> this_prt;
			typedef http::server::connection::ptr connection_ptr;
		public:
			connection_manager();
			~connection_manager();
			/// Add the specified connection to the manager and start it.
			static this_prt getInstance();

			/// Stop the specified connection.
			void stop_connection(int conn_id);

			void show_all();
			/// Stop all connections.
			void stop_all();

			bool is_from_other_http(int net_id);
			bool is_from_gm_http(int net_id);
			void write_msg(na::msg::msg_json& mj);

			void init();
			connection_ptr get_new_connection();
			void start_connection(connection_ptr conn, connection_type conn_type);
		private:
			/// The managed connections.
			int get_new_connection_id(connection_type conn_type);
			int get_new_connection_id(std::map<int, connection_ptr> &connection_map, int &latest_id, int start_id, int end_id);
			vector <connection_ptr> unused_connections_;
			std::map<int, connection_ptr> gm_connections_;
			std::map<int, connection_ptr> other_connections_;
			int latest_gm_conn_id_;
			int latest_other_conn_id_;
		};

	} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_MANAGER_HPP
