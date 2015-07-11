#include "server_base_direct.h"
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

		serverdirect::serverdirect(void):_is_stop(false)
		{
			_sdata.buffer = 1;
			_sdata.client_limit = 2000;
			_sdata.port = 2001;
			_sdata.net_id =  0;
			_sdata.is_tx = false;

		}

		serverdirect::~serverdirect(void)
		{

		}

		void serverdirect::init(std::string cfg)
		{
			_cfg_name = cfg;
			Json::Value configs = na::file_system::load_jsonfile_val(cfg);
			Json::Value common_json = na::file_system::load_jsonfile_val("./server/common_cfg.json");
			_sdata.client_limit = configs["client_limit"].asInt();
			_sdata.buffer = common_json["buffer_size"].asInt();
			_sdata.port = configs["port"].asInt();
			_sdata.is_tx = common_json["is_tx"].asBool();
			_ap = acceptor_pointer(new tcp::acceptor(coreM.get_io(base_io), 
				tcp::endpoint(tcp::v4(), _sdata.port)));
			start_accept(_ap);
			on_init();
			//			net_core.get_logic_io_service().post(boost::bind(&serverbase::update,this));
		}

		void serverdirect::run()
		{
			coreM.run();
		}

		void serverdirect::stop()
		{			
			_is_stop = true;
			on_stop();
			coreM.stop();			
		}

		void serverdirect::send_msg( int client_id,na::msg::msg_json& m )
		{
			boost::mutex::scoped_lock lock(_mutex);
			client_map::iterator it = _map_client.find(client_id);
			if(it!=_map_client.end())
				it->second->write_json_msg(m);
		}

		void serverdirect::send_msg( tcp_session_ptr conn_ptr,const na::msg::msg_json::ptr m )
		{
			conn_ptr->write_json_msg(*m);
		}

		void serverdirect::handle_accept(tcp_session_ptr new_connection,acceptor_pointer ap,const boost::system::error_code& error)
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
				new_connection->start(_hanlder_point);
				on_connect(new_connection->get_net_id(),new_connection);				
				start_accept(ap);
			}
			else
			{
				new_connection->stop();
				LogE <<  __FUNCTION__ << error.message() << LogEnd;
			}
		}
		void serverdirect::start_accept(acceptor_pointer ap)
		{	
			size_t n = _sdata.buffer; //byte 
			n *= 1024;//K
			n *= 1024;//M
			tcp_session_ptr new_connection = tcp_session::create( n , _sdata.is_tx ); // n(M)
			na::net::NetInsPtr nid = create_net_id();
			new_connection->set_net_id(nid);
			ap->async_accept(new_connection->socket(),
				boost::bind(&serverdirect::handle_accept,this, new_connection,ap,
				boost::asio::placeholders::error));
		}

		bool serverdirect::is_go_on_accept()
		{
			if((int)_map_client.size() >= _sdata.client_limit)
				return false;

			return true;
		}

	}
}

