//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_manager.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include "service_config.h"
#include "core_helper.h"

namespace http {
	namespace server {

		connection_manager::this_prt connection_manager::getInstance()
		{
			static this_prt p(new connection_manager());
			return p;
		}

		connection_manager::connection_manager()
		{
			latest_gm_conn_id_ = service::process_id::HTTP_GM_START_NET_ID - 1;
			latest_other_conn_id_ = service::process_id::HTTP_OHTER_START_NET_ID - 1;
		}

		connection_manager::~connection_manager()
		{
			latest_gm_conn_id_ = service::process_id::HTTP_GM_START_NET_ID;
			latest_other_conn_id_ = service::process_id::HTTP_OHTER_START_NET_ID;
			stop_all();
		}

		void connection_manager::stop_connection(int conn_id)
		{
			if (is_from_gm_http(conn_id))
			{
				map<int, connection_ptr>::iterator it = gm_connections_.find(conn_id);
				if (it == gm_connections_.end())
					return;
				it->second->stop();
				unused_connections_.push_back(it->second);
				gm_connections_.erase(it);
			}
			else if (is_from_other_http(conn_id))
			{
				map<int, connection_ptr>::iterator it = other_connections_.find(conn_id);
				if (it == other_connections_.end())
					return;
				it->second->stop();
				unused_connections_.push_back(it->second);
				other_connections_.erase(it);
			}
			else
			{
			}
		}

		void connection_manager::show_all()
		{
			Json::Value infoJson = Json::arrayValue;
			string s;

			cout << "gm_conn size:" << gm_connections_.size() << endl;
			for (std::map<int, connection_ptr>::iterator it = gm_connections_.begin();
				it != gm_connections_.end(); it++)
			{
				infoJson.append(it->second->connection_id);
			}
// 			s = infoJson.toStyledString();
// 			s = commom_sys.tighten(s);
			s = commom_sys.json2string(infoJson);
			cout << "gm_conn info:" << s << endl;
			
			infoJson = Json::arrayValue;
			cout << "other_conn size:" << other_connections_.size() << endl;
			for (std::map<int, connection_ptr>::iterator it = other_connections_.begin();
				it != other_connections_.end(); it++)
			{
				infoJson.append(it->second->connection_id);
			}
// 			s = infoJson.toStyledString();
// 			s = commom_sys.tighten(s);
			s = commom_sys.json2string(infoJson);
			cout << "other_conn info:" << s << endl;

			infoJson = Json::arrayValue;
			cout << "unused_conn size:" << unused_connections_.size() << endl;
			for (std::vector<connection_ptr>::iterator it = unused_connections_.begin();
				it != unused_connections_.end(); it++)
			{
				infoJson.append((*it)->connection_id);
			}
// 			s = infoJson.toStyledString();
// 			s = commom_sys.tighten(s);
			s = commom_sys.json2string(infoJson);
			cout << "unused_conn info:" << s << endl;
		}

		void connection_manager::stop_all()
		{
			for (std::map<int, connection_ptr>::iterator it = gm_connections_.begin(); 
				it != gm_connections_.end(); it++)
			{
				if (it->second != connection_ptr())
				{
					it->second->stop();
				}				
			}
			gm_connections_.clear();

			for (std::map<int, connection_ptr>::iterator it = other_connections_.begin(); 
				it != other_connections_.end(); it++)
			{
				if (it->second != connection_ptr())
				{
					it->second->stop();
				}				
			}
			other_connections_.clear();

			unused_connections_.clear();
		}

		bool connection_manager::is_from_other_http(int net_id)
		{
			if (is_from_gm_http(net_id))
				return true;
			
			return service::process_id::HTTP_OHTER_END_NET_ID <= net_id && net_id <= service::process_id::HTTP_OHTER_END_NET_ID;
		}

		bool connection_manager::is_from_gm_http(int net_id)
		{
			return (service::process_id::HTTP_GM_START_NET_ID <=net_id && net_id <= service::process_id::HTTP_GM_END_NET_ID);
		}

		void connection_manager::write_msg(na::msg::msg_json& mj)
		{
			int net_id = mj._net_id;
			if (is_from_gm_http(net_id))
			{
				if (gm_connections_.find(net_id) == gm_connections_.end())
					return;
				gm_connections_[net_id]->write_msg(mj);
			}
			else if (is_from_other_http(net_id))
			{
				if (other_connections_.find(net_id) == other_connections_.end())
					return;
				other_connections_[net_id]->write_msg(mj);
			}
			else
			{
			}
		}

		void connection_manager::init()
		{
			static int init_num = 100;
			
			for (int i = 0; i < init_num; i++)
			{
				connection_ptr new_connection = connection::create();
				if (new_connection == connection_ptr())
				{
					LogE << "init connection pool error!" << LogEnd;
					return;
				}
				
				unused_connections_.push_back(new_connection);
			}
		}

		connection_manager::connection_ptr connection_manager::get_new_connection()
		{
			static int again_num = 100;

			connection_ptr new_connection;
			int connection_size = int(unused_connections_.size());
			if (connection_size > 0)
			{
			}
			else
			{
				for (int i = 0; i < again_num; i++)
				{
					connection_ptr new_connection = connection::create();
					if (new_connection == connection_ptr())
					{
						LogE << "get new http connection error!" << LogEnd;
						return new_connection;
					}
					unused_connections_.push_back(new_connection);
				}
			}

			connection_size = int(unused_connections_.size());
			new_connection = unused_connections_[connection_size-1];
			unused_connections_.pop_back();
			return new_connection;
		}

		void connection_manager::start_connection( connection_ptr conn, connection_type conn_type)
		{
			int new_conn_id = get_new_connection_id(conn_type);
			if (conn_type == GM_CONNECITON_TYPE)
			{
				gm_connections_[new_conn_id] = conn;
			}
			else
			{
				other_connections_[new_conn_id] = conn;
			}
			conn->connection_id = new_conn_id;
			conn->is_used_ = true;
			boost::system::error_code ec;
			if (!conn->socket().is_open())
			{
				conn->socket().open(boost::asio::ip::tcp::v4(), ec);
			}
			
			conn->start();
		}

		int connection_manager::get_new_connection_id(std::map<int, connection_ptr> &connection_map, int &latest_id, int start_id, int end_id)
		{
			int count = 0;
			do 
			{
				count++;
				latest_id++;
				if (latest_id > end_id)
					latest_id = start_id;					

				std::map<int, connection_ptr>::iterator it = connection_map.find(latest_id);
				if (it != connection_map.end())
				{
					if (it->second->is_valid())
					{
						continue;
					}
					else
					{
						stop_connection(latest_id);
						return latest_id;
					}
				}
				else
				{
					return latest_id;
				}
			} while (count < 100);

			if (count >= 100)
			{
				LogE << __FUNCTION__ << ", get new http id error!" << LogEnd;
			}

			return 0;
		}

		int connection_manager::get_new_connection_id( connection_type conn_type )
		{
			if (conn_type == GM_CONNECITON_TYPE)
			{
				return get_new_connection_id(gm_connections_, latest_gm_conn_id_, 
					service::process_id::HTTP_GM_START_NET_ID, service::process_id::HTTP_GM_END_NET_ID);
			}
			else
			{
				return get_new_connection_id(other_connections_, latest_other_conn_id_, 
					service::process_id::HTTP_OHTER_START_NET_ID, service::process_id::HTTP_OHTER_END_NET_ID);
			}
		}


	} // namespace server
} // namespace http
