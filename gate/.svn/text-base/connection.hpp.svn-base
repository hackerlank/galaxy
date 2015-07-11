//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "http_common.hpp"
#include "request_parser.hpp"

namespace http {
namespace server {

class connection_manager;

/// Represents a single connection from a client.
class connection
  : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
{
	friend class connection_manager;
public:
	typedef boost::shared_ptr < connection > ptr;
  /// Construct a connection with the given io_service.
  explicit connection();

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();

  void show();
  /// Stop all asynchronous operations associated with the connection.
  void stop();

  void write_msg(na::msg::msg_json& mj);
  void write_msg(int resp_status);

  static void destroy(connection* p);
  static ptr create();
  int get_connection_id(){return connection_id;}
  bool is_valid();
private:
  /// Handle completion of a read operation.
  void handle_read(const boost::system::error_code& e,
      std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handle_write(const boost::system::error_code& e);

  bool urls(string url);
  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// Buffer for incoming data.
  boost::array<char, 8192> buffer_;

  /// The incoming request.
  request request_;

  /// The parser for the incoming request.
  request_parser request_parser_;

  /// The reply to be sent back to the client.
  reply reply_;

  ulr_en_de_code en_de_;

  int connection_id;
  bool is_used_;
};

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP
