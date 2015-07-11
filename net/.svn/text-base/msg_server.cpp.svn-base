#include "msg_server.h"
#include <boost/bind.hpp>
#include "tcp_session.h"
#include <file_system.h>
#include "core.h"
#include <time_helper.h>
#include <boost/lexical_cast.hpp>
namespace na
{
	namespace net
	{

		serverbase::serverbase(void):_is_stop(false)
		{
		}


		serverbase::~serverbase(void)
		{

		}

		void serverbase::run()
		{
			coreM.run();			
		}

		void serverbase::stop()
		{			
			_is_stop = true;
			on_stop();
			coreM.stop();			
		}

		void serverbase::send_msg( int client_id,na::msg::msg_json& m )
		{
			boost::mutex::scoped_lock lock(_mutex);
			client_map::iterator it = _map_client.find(client_id);
			if(it!=_map_client.end())
				it->second->write_json_msg(m);
		}

		void serverbase::send_msg( tcp_session_ptr conn_ptr,const na::msg::msg_json::ptr m )
		{
			conn_ptr->write_json_msg(*m);
		}

		void serverbase::on_disconnect( int client_id,tcp_session_ptr conn_ptr )
		{
			boost::mutex::scoped_lock lock(_mutex);
			client_map::iterator it = _map_client.find(client_id);
			if(it != _map_client.end())
			{
				if(it->second!=conn_ptr) // a new client connection
					return;
				_map_client.erase(it);
			}
		}

		void serverbase::handle_accept(tcp_session_ptr new_connection,acceptor_pointer ap,const boost::system::error_code& error)
		{			
			if(_is_stop) return;
			
			boost::mutex::scoped_lock lock(_mutex);
			if(!is_go_on_accept()){
				new_connection->stop();
				start_accept(ap);
				return;
			}				

			if (!error)
			{
				_map_client[new_connection->get_net_id()] = new_connection;				
				new_connection->start(shared_from_this());
				on_connect(new_connection->get_net_id(),new_connection);				
				start_accept(ap);
			}
			else
			{
				new_connection->stop();
				LogE <<  __FUNCTION__ << error.message() << LogEnd;
			}
		}

		void serverbase::recv_client_handler( tcp_session_ptr session,const char* data_ptr,int len )
		{			
			if(len < (int)sizeof(na::msg::msg_base)) return;
			na::msg::msg_json::ptr p = na::msg::msg_json::create(data_ptr,len);

			boost::mutex::scoped_lock lock(_queue_mutex);
			_msg_queue.push(p);			
		}

		void serverbase::client_connect_handler( tcp_session_ptr session,int error_value )
		{
			on_disconnect(session->get_net_id(),session);
			session->stop();
		}

	}
}

