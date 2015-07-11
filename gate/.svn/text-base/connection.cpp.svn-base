//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "connection_manager.hpp"
#include "gate_server.h"
#include "core_helper.h"

namespace http {
	namespace server {

		connection::connection()
			: socket_(coreM.get_io(gt::http_io))
		{
			connection_id = 0;
			is_used_ = false;
		}

		boost::asio::ip::tcp::socket& connection::socket()
		{
			return socket_;
		}

		void connection::start()
		{
			socket_.async_read_some(boost::asio::buffer(buffer_),
				boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}

		void connection::show()
		{
			bool is_open = socket_.is_open();
			cout << "http connection--->";
			cout << "id:" << this->connection_id << " ";
			cout << "is_open:" << (is_open ? "true" : "no") << " ";
			cout << endl;
		}

		void connection::stop()
		{
			connection_id = 0;
			is_used_ = false;
			buffer_ = boost::array<char, 8192>();
			request_.reset();
			socket_.close();
			request_parser_.reset();
		}

		char* msg2ptr(na::msg::msg_json &m, char *pdata)
		{
			char *p = pdata;
			memmove(p, &m, json_str_offset);
			p += json_str_offset;
			memmove(p, m._json_str_utf8, m._total_len-json_str_offset);
			p += m._total_len-json_str_offset;
			p = '\0';

			return pdata;
		}

		bool connection::urls(string url)
		{
			return url.find("/service") == 0;			
		}

		void connection::handle_read(const boost::system::error_code& e,
			std::size_t bytes_transferred)
		{
			if (!e)
			{
				boost::tribool result;
				boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
					request_, buffer_.data(), buffer_.data() + bytes_transferred);

				if (request_.uri.size() >= MAX_MSG_LENGTH)
				{
					LogE << "http data size too large" << LogEnd;
					write_msg(reply::bad_request);
					return;
				}

				if (result)
				{
//					request_handler_.handle_request(request_, reply_);
					// 					boost::asio::async_write(socket_, reply_.to_buffers(),
					// 						boost::bind(&connection::handle_write, shared_from_this(),
					// 						boost::asio::placeholders::error));

					if (!urls(request_.uri))
					{
						write_msg(reply::bad_request);
						return;
					}

					Json::Value jreq;
					if (request_.method == "GET")
					{
						string s = en_de_.UrlDecode(request_.uri);
						jreq = request_parser_.parse_content(GET_METHOD, s);
					}
					else
					{
						string s = en_de_.UrlDecode(request_.content);
						jreq = request_parser_.parse_content(POST_METHOD, s);
					}
					jreq["net_id"] = this->connection_id;
					gate_svr.on_http_req(this, jreq);
				}
				else if (!result)
				{
					write_msg(reply::bad_request);
					return;
				}
				else
				{
					socket_.async_read_some(boost::asio::buffer(buffer_),
						boost::bind(&connection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				}
			}
			else if (e != boost::asio::error::operation_aborted)
			{
				http_conn_mgr->stop_connection(this->connection_id);
			}
		}

		void connection::handle_write(const boost::system::error_code& e)
		{
			if (!e)
			{
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
				http_conn_mgr->stop_connection(this->connection_id);
				return;
			}

			if (e == boost::asio::error::operation_aborted)
			{
//				http_conn_mgr->stop(this->connection_id);
			}
			else
			{
				http_conn_mgr->stop_connection(this->connection_id);
			}
		}

		connection::ptr connection::create()
		{
 			void* m = nedalloc::nedmalloc(sizeof(connection));
			return connection::ptr(new(m) connection(),connection::destroy);
		}

		void connection::destroy(connection* p)
		{
			p->~connection();
			nedalloc::nedfree(p);
		}

		void connection::write_msg(na::msg::msg_json& mj)
		{
			reply rep;
			rep.status = reply::ok;

			string stmp = string(mj._json_str_utf8, mj._total_len - json_str_offset);

			Json::Reader reader;
			Json::Value jval;
			reader.parse(stmp, jval);
			int code = 0;

			rep.content = stmp;
			rep.headers.resize(2);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = "application/json";


// 			LogD << __FUNCTION__ << "," << __LINE__ << "," << ",json_str_utf8:" << mj._json_str_utf8
// 				<< ",mj.net:" << mj._net_id << ",stmp:" << stmp << "mj._type:" << mj._type << LogEnd;
			reply_ = rep;
			boost::asio::async_write(socket_, reply_.to_buffers(),
				boost::bind(&connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error));
		}

		void connection::write_msg(int resp_status)
		{
			Json::Value jval;
			jval["msg"][0u] = resp_status;
			string s = jval.toStyledString();
			na::msg::msg_json mj(-1, s);
			write_msg(mj);
		}

		bool connection::is_valid()
		{
			if (is_used_)
				return true;
			
			if (socket_.is_open())
				return true;
			
			return false;
		}

	} // namespace server
} // namespace http
