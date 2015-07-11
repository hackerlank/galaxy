//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <boost/noncopyable.hpp>
#include "connection.hpp"
#include "connection_manager.hpp"
#include <boost/enable_shared_from_this.hpp>
#include "read_buffer.h"

namespace http {
namespace server {

/// The top-level class of the HTTP server.
class http_server
  : private boost::noncopyable
{
public:
	typedef boost::shared_ptr < http_server > ptr;
	typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_pointer;
	typedef boost::shared_ptr<connection> connection_ptr;

  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit http_server(const std::string& port, vector<std::string> gm_tool_ip,
      const std::string& doc_root);

  /// Run the server's io_service loop.
  void run();

  /// Stop the server.
  void stop();

  void start_accept();
  static ptr create( std::string svr_port, vector<std::string> gm_tool_ip);

  static void destroy(http_server* p);

private:
  /// Handle completion of an asynchronous accept operation.
  void handle_accept(connection_ptr new_connection,const boost::system::error_code& e);

  /// Handle a request to stop the server.
  void handle_stop();

  /// Acceptor used to listen for incoming connections.
//  boost::asio::ip::tcp::acceptor acceptor_;
  acceptor_pointer		http_acceptor_;
  vector<std::string> gm_tool_ip_;

  /// The next connection to be accepted.

};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP
