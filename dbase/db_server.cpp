#include "db_server.h"
#include "tcp_session.h"
#include "file_system.h"
#include "core_helper.h"
#include "service_config.h"
#include "mysqlManager.h"
using namespace na::net;
namespace db
{
	dbMgr::this_ptr dbMgr::getInstance()
	{
		static this_ptr p(new dbMgr());
		return p;
	}

	dbMgr::dbMgr()
	{

	}

	dbMgr::~dbMgr()
	{

	}

	void dbMgr::init(string cfg)
	{
		serverdirect::init(cfg);
		_hanlder_point = net_handler_ptr(new handler());
		coreM.get_io(na::net::logic_io).post(boost::bind(&dbMgr::on_update,this));
	}

	NetInsPtr dbMgr::create_net_id()
	{
		return NetCreator::Create2(service::process_id::GAME_NET_ID);
	}

	void dbMgr::disconnect(tcp_session_ptr session,int error_value)
	{
		boost::mutex::scoped_lock(_mutex);
		_map_client.clear();
		session->stop();
	}

	void dbMgr::on_update()
	{
		if(_is_stop) return;
		boost::system_time tmp = boost::get_system_time();

		if((tmp - _st).total_milliseconds() > 600000)//1 min
		{
			_st = tmp;
			checkkick(tmp);
		}

		na::time_helper::sleep(1000);
		coreM.get_io(na::net::logic_io).post(boost::bind(&dbMgr::on_update,this));
	}


	void dbMgr::checkkick(boost::system_time& tmp)
	{
		boost::mutex::scoped_lock lock(_mutex);
		client_map::iterator ite = _map_client.begin();
		for (;ite!=_map_client.end();)
		{
			tcp_session_pointer session = ite->second;
			++ite;//!!!cause the ite may earse in the follow function -> kick_client() -> _client.erase() so it must plus plus first!!!
			if(!session->is_alive(tmp, 600000))
			{
				LogW <<  "************* Dead client IP:" << session->socket().remote_endpoint().address().to_string() << " *******************" << LogEnd;
				kick(session,true);			
				continue;
			}
		}
	}

	void dbMgr::kick(tcp_session_ptr session, bool notify )
	{
		client_map::iterator it = _map_client.find(session->get_net_id());
		if(it==_map_client.end())		
			return;
		_map_client.erase(it);	
		if(notify)
		{	
			LogS <<  "Online:\t" << color_green(_map_client.size()) << LogEnd;			
		}
		session->stop();
	}
	
}
