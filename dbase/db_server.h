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
#include "db_handle.h"

using boost::asio::ip::tcp;

//#define db_svr boost::detail::thread::singleton<db::dbMgr>::instance()

#define db_svr db::dbMgr::getInstance()

namespace na
{
	namespace net
	{
		class tcp_session;
	}
}
using namespace na::net;
namespace db
{
	class dbMgr : 
		public na::net::serverdirect
	{
		typedef boost::shared_ptr<db::dbMgr> this_ptr;
		typedef boost::shared_ptr<na::net::tcp_session> tcp_session_pointer;
	public:
		static this_ptr getInstance();
		dbMgr();
		~dbMgr();
		virtual void init(string cfg);
		virtual void disconnect(tcp_session_ptr session,int error_value);
	protected:
		virtual NetInsPtr create_net_id();
		virtual void on_update();		
//		virtual void on_recv_msg(const na::msg::msg_json::ptr recv_msg_ptr);
	private:
		boost::system_time _st;
		void checkkick(boost::system_time& tmp);
		void kick(tcp_session_ptr session, bool notify = false );
	};
}
