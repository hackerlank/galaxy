#include "filter_server.h"
#include "tcp_session.h"
#include "file_system.h"
#include "core_helper.h"
#include <boost/lexical_cast.hpp>
#include "common_protocol.h"
#include "time_helper.h"
#include "service_config.h"
#include "commom.h"
#include "nedhelper.hpp"
#include "filter.h"
#include "deal_protocol.hpp"
using namespace na::net;
#define t getDeadlineTimer()
namespace ft
{
	boost::asio::deadline_timer& getDeadlineTimer()
	{
		static boost::asio::deadline_timer dt(coreM.get_io(logic_io));
		return dt;
	}

	filter::this_ptr filter::getInstance()
	{
		static this_ptr p(new filter());
		return p;
	}

	filter::filter()
	{
		tick = boost::get_system_time();
	}

	filter::~filter()
	{

	}

	void filter::async_send_to_gate(const char* data, int len)
	{
		client_map::iterator it = _map_client.find(service::process_id::GATE_NET_ID);
		if(it!=_map_client.end())
			it->second->write(data, len);
	}

	void filter::async_send_to_gate(na::msg::msg_json& mj)
	{
		client_map::iterator it = _map_client.find(service::process_id::GATE_NET_ID);
		if(it!=_map_client.end())
			it->second->write_json_msg(mj);
	}

	void filter::start_accept(acceptor_pointer ap)
	{	
		size_t n = _fdata.buffer * 2; //byte 
		n *= 1024;//K
		n *= 1024;//M
		tcp_session_ptr new_connection = tcp_session::create( n , _fdata.is_tx ); // n(M)
		na::net::NetInsPtr nid = create_net_id();
		new_connection->set_net_id(nid);
		ap->async_accept(new_connection->socket(),
			boost::bind(&filter::handle_accept,this, new_connection,ap,
			boost::asio::placeholders::error));
	}

	bool filter::is_go_on_accept()
	{
		return true;
	}

	void filter::init()
	{
		DealMatch::initFilterProtocol();
		Json::Value game_json = na::file_system::load_jsonfile_val("./server/filter_cfg.json");
		Json::Value common_json = na::file_system::load_jsonfile_val("./server/common_cfg.json");
		_fdata.buffer = common_json["buffer_size"].asInt();
		_fdata.port = game_json["port"].asInt();
		_fdata.is_tx = common_json["is_tx"].asBool();

		_ap = acceptor_pointer(new tcp::acceptor(coreM.get_io(na::net::base_io), 
			tcp::endpoint(tcp::v4(), _fdata.port)));
		start_accept(_ap);

		filter_handler = handler::pointer(new handler());
		FILTER.initData();

		coreM.get_io(logic_io).post(boost::bind(&filter::system_run,this));

		coreM.get_io(filter_1).post(boost::bind(&filter::on_update,this, (int)filter_1));
		coreM.get_io(filter_2).post(boost::bind(&filter::on_update,this, (int)filter_2));
		coreM.get_io(filter_3).post(boost::bind(&filter::on_update,this, (int)filter_3));
		coreM.get_io(filter_4).post(boost::bind(&filter::on_update,this, (int)filter_4));
		coreM.get_io(filter_5).post(boost::bind(&filter::on_update,this, (int)filter_5));
// 		coreM.get_io(filter_6).post(boost::bind(&filter::on_update,this, (int)filter_6));
// 		coreM.get_io(filter_7).post(boost::bind(&filter::on_update,this, (int)filter_7));
// 		coreM.get_io(filter_8).post(boost::bind(&filter::on_update,this, (int)filter_8));

		LogI << color_green("filter server start..") << LogEnd;
	}

	NetInsPtr filter::create_net_id()
	{
		return NetCreator::Create2(service::process_id::GATE_NET_ID);
	}

	void filter::system_run()
	{
		boost::system_time tmp = boost::get_system_time();
		if((tmp - tick).total_milliseconds() > 300000)
		{
			tick = tmp;
			checkkick(tmp);
		}
	}

	void filter::on_update(const int IOID)
	{
		if(_is_stop)	return;
		boost::mutex::scoped_lock lock(_queue_mutex);
		int sl_t = 666666;
		if(!_msg_queue.empty()){
			na::msg::msg_json::ptr p = _msg_queue.front();
			_msg_queue.pop();			
			// unlock
			lock.unlock();
			sl_t = 1;
			on_recv_msg(p);
		}else
		{
			lock.unlock();
		}

		na::time_helper::sleep(sl_t);
		coreM.get_io(IOID).post(boost::bind(&filter::on_update, this, IOID));
	}

	void filter::on_recv_msg(const na::msg::msg_json::ptr recv_msg_ptr)
	{
// 		int netID = recv_msg_ptr->_net_id;
// 		if(netID != service::process_id::GATE_NET_ID)return;
		boost::mutex::scoped_lock l(_mutex);
		client_map::iterator it = _map_client.find(service::process_id::GATE_NET_ID);
		if(it == _map_client.end())return;
		filter_handler->recv_client_handler(it->second, *recv_msg_ptr);
	}

	void filter::checkkick(boost::system_time& now)
	{
		boost::mutex::scoped_lock lock(_mutex);
		client_map::iterator ite = _map_client.begin();
		for (;ite!=_map_client.end();)
		{
			tcp_session_pointer session = ite->second;
			++ite;//!!!cause the ite may earse in the follow function -> kick_client() -> _client.erase() so it must plus plus first!!!
			if(!session->is_alive(now, 300000)){
				kick(session, false);			
				continue;
			}
		}
	}

	void filter::kick(tcp_session_ptr session, bool notify /* = false */)
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

	void filter::kick_async(tcp_session_pointer session)
	{
		boost::mutex::scoped_lock l(_mutex);
		kick(session);
	}
}
