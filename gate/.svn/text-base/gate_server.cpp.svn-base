#include "gate_server.h"
#include "tcp_session.h"
#include "file_system.h"
#include "core_helper.h"
#include <boost/lexical_cast.hpp>
#include "common_protocol.h"
#include "service_config.h"
#include "xor.h"
#include "gate_account_protocol.h"
#include "gate_game_protocol.h"
#include "msg_base.h"
#include "file_system.h"
#include <boost/filesystem.hpp>  
#include "deal_protocol.hpp"
using namespace na::net;
#define t getDeadlineTimer()

namespace gt
{
//#define SCLOG
	inline void LogS2C(const char* data)
	{
#ifdef SCLOG
		na::msg::msg_base* b = (na::msg::msg_base*)data;
		LogW << "s2c (" << b->_player_id << ") (" << b->_type << "): "<< data + na::msg::json_str_offset << LogEnd;
#endif
	}

	inline void LogC2S(const char* data)
	{
#ifdef SCLOG
		na::msg::msg_base* b = (na::msg::msg_base*)data;
		LogW << "c2s (" << b->_player_id << ") (" << b->_type << "): "<< data + na::msg::json_str_offset << LogEnd;
#endif
	}

	inline void LogC2S(na::msg::msg_json& mj)
	{
#ifdef SCLOG
		LogW << "c2s ("<< mj._player_id << ") (" << mj._type << "): " << mj._json_str_utf8 << LogEnd;
#endif
	}


	boost::asio::deadline_timer& getDeadlineTimer()
	{
		static boost::asio::deadline_timer dt(coreM.get_io(base_io));
		return dt;
	}

	gate::gate()/* : _first_connect_game(true)*/
	{
	}

	gate::~gate()
	{

	}

	void gate::async_send_filter_svr(msg_json& mj)
	{
		_filter_server->write_json_msg(mj);
	}

	void gate::async_send_filter_svr( const char* data,int len )
	{
		_filter_server->write(data, len);
	}

	void gate::async_send_account_svr(msg_json& mj)
	{
		_account_server->write_json_msg(mj);
	}

	void gate::async_send_account_svr( const char* data,int len )
	{
		_account_server->write(data, len);
	}

	void gate::async_send_gamesvr(const char* data,int len)
	{
		LogC2S(data);
		_game_server->write(data, len);
	}

	void gate::set_session_id(const int net_id, const int player_id)
	{
		boost::mutex::scoped_lock l(_mutex);
		int rNetID = net_id / na::net::vNetOffset;
		client_map::iterator it = _map_client.find(rNetID);
		if(it == _map_client.end())
		{
			return;
		}
		tcp_session_ptr session = it->second;
		if(session->get_net_id() == rNetID && session->get_vN_net_id() == net_id)
		{
			session->_player_id = player_id;	
			session->set_session_status(gs_game);
			client_map::iterator playerClient = _player.find(player_id);
			if(playerClient != _player.end()){
				LogW << "kick same player ..." << player_id << LogEnd;
				tcp_session_ptr ptr = playerClient->second;
				kick(ptr, false);
			}
			_player.insert(client_map::value_type(session->_player_id,  session));
		}
	}

	void gate::async_send_gamesvr(na::msg::msg_json& mj)
	{
		LogC2S(mj);
		_game_server->write_json_msg(mj);
	}

	void gate::async_send_to_all(na::msg::msg_json& mj)
	{
		client_map::iterator it = _map_client.begin();
		while(it != _map_client.end())
		{
			tcp_session_ptr gs = it->second;

			if(_gdata.is_tx)
			{
				short tl = *((short*)&mj);
				tl += 2;
				short lb = tl >> 8 & 0x00FF;
				short hb = tl & 0x00FF;
				tl = (hb << 8) + lb;
				it->second->write((char*)&tl,2);
			}
			it->second->write_json_msg(mj);
			++it;
		}
	}

	void gate::async_send_to_all(const char* data,int len)
	{
		boost::mutex::scoped_lock lock(_mutex);
		client_map::iterator it = _map_client.begin();
		cc::convert_binary((char*)(data + na::msg::json_str_offset) ,len - na::msg::json_str_offset);
		while(it != _map_client.end())
		{
			tcp_session_ptr gs = it->second;

			if(_gdata.is_tx)
			{
				short tl = *((short*)data);
				tl += 2;
				short lb = tl >> 8 & 0x00FF;
				short hb = tl & 0x00FF;
				tl = (hb << 8) + lb;
				it->second->write((char*)&tl,2);
			}
			it->second->write(data,len);

			++it;
		}
	} 

	bool gate::async_send_client_without_internal_conver(na::msg::msg_json& mj, bool force /* = false */)
	{
		boost::mutex::scoped_lock lock(_mutex);
		client_map::iterator nit = _map_client.find(mj._net_id);
		if (nit == _map_client.end()){
			LogE << "cant find net_id:" << mj._net_id << "\tlost msg:" << mj._type << LogEnd;
			return false;
		}
		tcp_session_ptr findNetPtr = nit->second;
		if (!force)
		{
			if (findNetPtr->_player_id != mj._player_id){
				LogE << "find net id player is not match current messgae :" << mj._player_id << LogEnd;
				return false;
			}
			client_map::iterator pit = _player.find(mj._player_id);
			if (pit == _player.end()){
				LogE << "can not find player by net_id : " << mj._net_id << " and player_id : " << mj._player_id << LogEnd;
				kick(findNetPtr, false);
				return false;
			}
			tcp_session_ptr findPlayerPtr = pit->second;
			if (findPlayerPtr != findNetPtr)
			{
				LogE << "cant match player and message !! player_id : " << mj._player_id << " player net : " << findPlayerPtr->get_net_id()
					<< " net net : " << findNetPtr->get_net_id() << LogEnd;
				kick(findNetPtr, false);
				return false;
			}
		}
		if (_gdata.is_tx)
		{
			short tl = mj._total_len + 2;
			short lb = tl >> 8 & 0x00FF;
			short hb = tl & 0x00FF;
			tl = (hb << 8) + lb;
			findNetPtr->write((char*)&tl, 2);
		}

		findNetPtr->write_json_msg(mj);
		findNetPtr->set_session_status(na::net::gs_game);

		return true;
	}
	bool gate::async_send_client(int net_id, int player_id, const char* data,int len, bool force /* = false */)
	{
		boost::mutex::scoped_lock lock(_mutex);
		na::msg::msg_base* jm = (na::msg::msg_base*)data;
		client_map::iterator nit = _map_client.find(net_id);
		if(nit == _map_client.end()){
			LogE << "cant find net_id:" << net_id << "\tlost msg:" << jm->_type << LogEnd;
			return false;
		}
		tcp_session_ptr findNetPtr = nit->second;
		if(!force)
		{
			if(findNetPtr->_player_id != player_id){
				LogE << "find net id player is not match current messgae :" << player_id << LogEnd;
				return false;
			}
			client_map::iterator pit = _player.find(player_id);
			if(pit == _player.end()){
				LogE << "can not find player by net_id : " << net_id << " and player_id : " << player_id << LogEnd;
				kick(findNetPtr, false);
				return false;
			}
			tcp_session_ptr findPlayerPtr = pit->second;
			if(findPlayerPtr != findNetPtr)
			{
				LogE << "cant match player and message !! player_id : " << player_id << " player net : " << findPlayerPtr->get_net_id()
					<< " net net : " << findNetPtr->get_net_id() << LogEnd;
				kick(findNetPtr, false);
				return false;
			}
		}
		if(_gdata.is_tx)
		{
			short tl = jm->_total_len + 2;
			short lb = tl >> 8 & 0x00FF;
			short hb = tl & 0x00FF;
			tl = (hb << 8) + lb;
			findNetPtr->write((char*)&tl,2);
		}

		LogS2C(data);

		cc::convert_binary((char*)(data + na::msg::json_str_offset), len - na::msg::json_str_offset);
		findNetPtr->write(data,len);
		findNetPtr->set_session_status(na::net::gs_game);

// 			na::msg::msg_base* b = (na::msg::msg_base*)data;
// 			cout << "protocol : " << b->_type << 
// 				"\ttotal len:" << b->_total_len << endl;
// 			for (int i = 0; i < len; ++i)
// 			{
// 				char c = *(data + i);
// 				int ci = (int)c;
// 				ci = ci & 0xff;
// 				cout << setw(2) << setfill('0') << std::uppercase <<  std::hex << ci << " ";
// 			}

		return true;
	}

	void gate::init(bool is_limit_ip)
	{
		commom_sys;
		DealMatch::initFilterProtocol();
		Creator = new NetCreator();
		Creator->initData(0, service::process_id::CLIENT_NET_ID_RANGE, Creator);
		_hanlder_point = net_handler_ptr(new handler());
		Json::Value gate_json = na::file_system::load_jsonfile_val("./server/gate_cfg.json");
		Json::Value conmon_json = na::file_system::load_jsonfile_val("./server/common_cfg.json");
		_gdata.client_limit = gate_json["client_limit"].asInt();
		_gdata.buffer = conmon_json["buffer_size"].asUInt();
		_gdata.client_buffer = gate_json["client_size"].asUInt();
		_gdata.port = gate_json["port"].asInt();
		_gdata.is_tx = conmon_json["is_tx"].asBool();
		_gdata._game_ip = gate_json["game_ip"].asString();
		_gdata._game_port = gate_json["game_port"].asString();
		_gdata._account_ip = gate_json["account_ip"].asString();
		_gdata._account_port = gate_json["account_port"].asString();
		_gdata._filter_ip = gate_json["filter_ip"].asString();
		_gdata._filter_port = gate_json["filter_port"].asString();
// 		_gdata._fee_ip = gate_json["fee_ip"].asString();
// 		_gdata._fee_port = gate_json["fee_port"].asString();
		_gdata._http_port = gate_json["http_port"].asString();
		_gdata.server_id = conmon_json["server_id"].asInt();
		Json::Value tempIps = gate_json["gm_tool_ip"];
		for (unsigned ip_idx = 0; ip_idx < tempIps.size(); ip_idx++)
		{
			_gdata._gm_tool_ip.push_back(tempIps[ip_idx].asString());
		}

		_ap = acceptor_pointer(new tcp::acceptor(coreM.get_io(na::net::base_io), 
			tcp::endpoint(tcp::v4(), _gdata.port)));
		start_accept(_ap);
		LogS << color_yellow("gate server start accept ...") << LogEnd;
		
		size_t n = _gdata.buffer;
		n *= 1024;
		n *= 1024;
		LogS << "try to connect game server..." << LogEnd;
		_game_server = tcp_session::create(n);
 		connect_game_svr();//游戏服务器
		LogS << "try to connect account server..." << LogEnd;
		_account_server = tcp_session::create(n);
 		connect_account_svr();//账号服务器
		LogS << "try to connect filter server..." << LogEnd;
		_filter_server = tcp_session::create(n);
		connect_filter_svr();//过滤服务器

		create_http_svr(_gdata._http_port, _gdata._gm_tool_ip);
		initAllowIpList(is_limit_ip);

		coreM.get_io(logic_io).post(boost::bind(&gate::on_update,this));

		LogS << "gate server initial success ... " << LogEnd;
	}

	void gate::connect_filter_svr()
	{
		NetInsPtr ptr = NetCreator::Create2(service::process_id::FILTER_NET_ID);
		_filter_server->set_net_id(ptr);
		connect(_filter_server,_gdata._filter_ip.c_str(), _gdata._filter_port.c_str());
	}

	void gate::connect_account_svr()
	{
		NetInsPtr ptr = NetCreator::Create2(service::process_id::ACCOUNT_NET_ID);
		_account_server->set_net_id(ptr);
		connect(_account_server,_gdata._account_ip.c_str(), _gdata._account_port.c_str());
	}

	void gate::connect_game_svr()
	{
		NetInsPtr ptr = NetCreator::Create2(service::process_id::GAME_NET_ID);
		_game_server->set_net_id(ptr);
		connect(_game_server,_gdata._game_ip.c_str(), _gdata._game_port.c_str());
	}

	void gate::stop()
	{			
		_is_stop = true;
		Creator->stop();
		coreM.stop();	
	}

	void gate::run()
	{
		coreM.run();
	}

	void gate::start_accept(acceptor_pointer ap)
	{	
		size_t n = _gdata.client_buffer; //byte 
		n *= 1024;//K
		tcp_session_ptr new_connection = tcp_session::create( n , _gdata.is_tx ); // n(M)
		if(new_connection == NULL){
			LogE << "tcp session malloc memory failed ..." << LogEnd;
			t.expires_from_now(boost::posix_time::seconds(10));
			t.async_wait(boost::bind(&gate::start_accept,this, ap));
			return;
		}
		NetInsPtr nid = create_net_id();
		if(nid == NULL){
			LogE << "net id malloc memory failed ..." << LogEnd;
			t.expires_from_now(boost::posix_time::seconds(10));
			t.async_wait(boost::bind(&gate::start_accept,this, ap));
			return;
		}
		new_connection->set_net_id(nid);
		ap->async_accept(new_connection->socket(),
			boost::bind(&gate::handle_accept,this, new_connection,ap,
			boost::asio::placeholders::error));
	}

	bool gate::is_go_on_accept()
	{
		if(_map_client.size() >= (unsigned)_gdata.client_limit)
			return false;

		return true;
	}

	bool gate::is_allow_client_login(tcp_session_ptr session_ptr)
	{
		try
		{
			std::string remote_adress = session_ptr->socket().remote_endpoint().address().to_string();
			cout << "remote_adress:" << remote_adress << endl;
			if (_gdata._limit_ip.size() < 1)
				return true;

			if (remote_adress == _gdata._game_ip
				|| remote_adress == _gdata._account_ip
				|| remote_adress == _gdata._filter_ip
				|| remote_adress == "127.0.0.1")
				return true;

			vector<string>::iterator it_limit = find(_gdata._limit_ip.begin(), _gdata._limit_ip.end(), remote_adress);
			if (it_limit != _gdata._limit_ip.end())
				return true;

			vector<string>::iterator it_gm = find(_gdata._gm_tool_ip.begin(), _gdata._gm_tool_ip.end(), remote_adress);
			if (it_gm != _gdata._gm_tool_ip.end())
				return true;
		}
		catch (boost::system::system_error& e)
		{
			LogW << "something error ...msg:" << e.what() << "\terror_code " << e.code() << LogEnd;
		}
		catch (std::exception& e)
		{
			LogW << "something error ...msg:" << e.what() << LogEnd;
		}
		return false;
	}

	void gate::handle_accept(tcp_session_ptr new_connection,acceptor_pointer ap,const boost::system::error_code& error)
	{			
		if(_is_stop) return;

		boost::mutex::scoped_lock lock(_mutex);
		if (new_connection == tcp_session_ptr())
		{
			start_accept(ap);
			return;
		}

		if(!is_go_on_accept()){
			new_connection->stop();
			start_accept(ap);
			return;
		}				

		if(!is_allow_client_login(new_connection)){
			new_connection->stop();
			start_accept(ap);
			return;
		}				

		if (!error)
		{
			_map_client[new_connection->get_net_id()] = new_connection;				
			new_connection->start(_hanlder_point);			
			start_accept(ap);
		}
		else
		{
			new_connection->stop();
			LogE <<  __FUNCTION__ << error.message() << LogEnd;
		}
	}

	void gate::connect(tcp_session_ptr connector,const char* ip_str, const char* port_str)
	{
		if(connector->get_session_status()>gs_null)
			return;
		connector->set_session_status(gs_connecting);
		tcp::resolver resolver(connector->socket().get_io_service());
		tcp::resolver::query query(ip_str, port_str);
		tcp::resolver::iterator iterator = resolver.resolve(query);	

		boost::asio::async_connect(connector->socket(), iterator,
			boost::bind(&gate::handle_connect, this,connector,
			boost::asio::placeholders::error));
	}

	void gate::handle_connect(tcp_session_ptr connector,const boost::system::error_code& error)
	{
		if(_is_stop)
			return;
//		boost::mutex::scoped_lock lock(_mutex);
		if (!error)
		{	
			connector->socket().non_blocking(true);
			connector->start(_hanlder_point);
			if(connector == _game_server)
			{
				LogI<<  "game server connected ..." <<LogEnd;
// 				if(_first_connect_game)
// 				{
					boost::system_time tmp = boost::get_system_time();
					checkkick(tmp);
					boost::mutex::scoped_lock lock(_mutex);
					if(_map_client.empty())
					{
						static string str = "";
						na::msg::msg_json mj(-1, service::process_id::HTTP_GM_START_NET_ID, gate_client::gate_restart_req, str);
						connector->write_json_msg(mj);
					}
//					_first_connect_game = false;
//				}
			}
			else if(connector == _account_server)
			{
				LogI<<  "account server connected ..." <<LogEnd;
				int server_id = _gdata.server_id;
				LogS << color_pink("server_id:") << server_id  << color_pink("has been connect account...")<< LogEnd;
				connector->write((const char*)&server_id,4);
			}
			else if(connector == _filter_server)
			{
				LogI<<  "filter server connected ..." <<LogEnd;
			}
			connector->set_session_status(gs_game);
		}
		else
		{
			if(error == boost::asio::error::would_block)
			{
				LogE <<  "************* server is blocking, id:\t" << connector->get_net_id() << "  *********" << LogEnd;
				return;
			}

			connector->stop();

			if(connector->get_net_id()== service::process_id::GAME_NET_ID)
			{
				LogE <<  "************* can not connect to game server *********" <<LogEnd;
 				na::time_helper::sleep(5000000);
				connect_game_svr();
			}
			else if(connector->get_net_id() == service::process_id::ACCOUNT_NET_ID)
			{
				LogE <<  "************* can not connect to account server *********" <<LogEnd;
 				na::time_helper::sleep(1000000);
				connect_account_svr();
			}
			else if(connector->get_net_id() == service::process_id::FILTER_NET_ID)
			{
				LogE <<  "************* can not connect to filter server *********" <<LogEnd;
				na::time_helper::sleep(5000000);
				connect_filter_svr();
			}
		}
	}

	NetInsPtr gate::create_net_id()
	{
		return Creator->getNewNetPrt();
	}

	void gate::on_update()
	{
		if(_is_stop) return;
		boost::system_time tmp = boost::get_system_time();
		if((tmp - _st).total_milliseconds() > 160000)//3 min
		{
			_st = tmp;
			checkkick(tmp);
			sendheartbeat();
		}

		if((tmp - _console).total_seconds() > 600)//10 min
		{
			_console = tmp;
			printOnline();
		}

		na::time_helper::sleep(1);
		coreM.get_io(na::net::logic_io).post(boost::bind(&gate::on_update,this));
	}

	void gate::sendheartbeat()
	{
		string str;
		na::msg::msg_json mj(game_protocol::comomon::keep_alive_req, str);
		mj._net_id = service::process_id::HTTP_GM_START_NET_ID;
		if(_game_server->get_session_status() == gs_game)
			_game_server->write_json_msg(mj);

		mj._net_id = service::process_id::FILTER_NET_ID;
		if(_filter_server->get_session_status() == gs_game)
			_filter_server->write_json_msg(mj);

		mj._type = protocol::c2l::system_keep_alive;
		mj._net_id = service::process_id::ACCOUNT_NET_ID;
		if(_account_server->get_session_status() == gs_game)
			_account_server->write_json_msg(mj);
	}

	void gate::checkkick(boost::system_time& now)
	{
		boost::mutex::scoped_lock lock(_mutex);
		client_map::iterator ite = _map_client.begin();
		for (;ite!=_map_client.end();)
		{
			tcp_session_ptr session = ite->second;
			++ite;//!!!cause the ite may earse in the follow function -> kick_client() -> _client.erase() so it must plus plus first!!!
			if(!session->is_alive(now))
			{
				try
				{
					LogW << "************* Dead client IP:" << session->socket().remote_endpoint().address().to_string() << " *******************" << LogEnd;
				}
				catch (boost::system::system_error& e)
				{
					LogW << "something error ...msg:" << e.what() << "\terror_code " << e.code() << LogEnd;
				}
				catch (std::exception& e)
				{
					LogW << "something error ...msg:" << e.what()  << LogEnd;
				}
				kick(session,false);			
				continue;
			}
		}
	}

	void gate::kick_async(tcp_session_ptr session, bool notify)
	{
		boost::mutex::scoped_lock l(_mutex);
		kick(session, notify);
	}

	void gate::kick(tcp_session_ptr session, bool notify)
	{		
		client_map::iterator nit = _map_client.find(session->get_net_id());
		if(nit !=_map_client.end())
		{
			//通知gg删除该玩家信息
			if(session->get_session_status() == na::net::gs_game)
			{
				string str;
				session->set_session_status(na::net::gs_null);
				na::msg::msg_json mj(game_protocol::comomon::del_playerInfo_req, str);
				mj._player_id  = session->_player_id;
				mj._net_id = service::process_id::HTTP_GM_START_NET_ID;
				_game_server->write_json_msg(mj);
			}
			session->stop();

			tcp_session_ptr nPtr = nit->second;
			_map_client.erase(nit);

			if(notify)
			{	
				LogS <<  "Online:\t" << color_green(_map_client.size()) << LogEnd;			
			}

			client_map::iterator pit = _player.find(session->_player_id);
			if(pit != _player.end())
			{
				tcp_session_ptr pPtr = pit->second;
				if(nPtr == pPtr || 
					_map_client.find(pPtr->get_net_id()) == _map_client.end())_player.erase(pit);
			}
		}else
		{
			client_map::iterator pit = _player.find(session->_player_id);
			if(pit != _player.end())
			{
				tcp_session_ptr pPtr = pit->second;
				if(_map_client.find(pPtr->get_net_id()) == _map_client.end())_player.erase(pit);
			}
		}
	}

	void gate::client_connect_handler(tcp_session_ptr session,int error_value)
	{
			if(session == _game_server)
			{
				if(session->get_session_status()!=gs_connecting)
					_game_server->stop();
				else
					return;
				LogS << "reconnect to game service ..." << LogEnd;
				connect_game_svr();
			}
			else if(session == _account_server)
			{
				if(session->get_session_status()!=gs_connecting)
					_account_server->stop();
				else
					return;
				LogS << "reconnect to account service ..." << LogEnd;
				connect_account_svr();
			}
			else if(session == _filter_server)
			{
				if(session->get_session_status()!=gs_connecting)
					_filter_server->stop();
				else
					return;
				LogS << "reconnect to account service ..." << LogEnd;
				connect_filter_svr();
			}
			else
			{
				kick_async(session, false);
				LogS << "client session disconnect ..." << LogEnd;
			}
	}

	void gate::create_http_svr(string svr_port, vector<string> gm_tool_ip)
	{
		_http_server = http::server::http_server::create(svr_port, gm_tool_ip);
	}

	void gate::initAllowIpList(bool is_limit_ip)
	{
		string fileName = allow_ip_login_file;
		Json::Value allowIpJson = Json::arrayValue;
		boost::filesystem::path boostPath(fileName);
		if (boost::filesystem::is_regular_file(boostPath))
		{
			allowIpJson = na::file_system::load_jsonfile_val(fileName);
		}

		if (is_limit_ip)
		{
			string local_ip = "127.0.0.1";
			_gdata._limit_ip.push_back(local_ip);
		}

		for (unsigned i = 0; i < allowIpJson.size(); i++)
		{
			_gdata._limit_ip.push_back(allowIpJson[i].asString());
		}
	}

	void gate::on_http_req(http::server::connection* conn_ptr, Json::Value& http_req)
	{
		if (http_req["req_type"] != Json::Value::null)
		{
			process_gm_req(conn_ptr, http_req);
		}
		else if (0)
		{
		}
		else
		{
			conn_ptr->write_msg(http::server::reply::bad_request);
		}

	}

	void gate::process_gm_req( http::server::connection* conn_ptr, Json::Value& http_req )
	{
		if (http_req["req_type"] == Json::Value::null)
			return;

		int player_id = 0;
		short req_type = boost::lexical_cast<short, string>(http_req["req_type"].asString());

		if (http_req["player_id"] != Json::Value::null)
		{
// 			try
// 			{
				player_id = boost::lexical_cast<int, string>(http_req["player_id"].asString());
/*			}
			catch (boost::bad_lexical_cast& e)
			{
				LogE << "bad_lexical_cast player_id=" << http_req["player_id"].asString() << LogEnd;
				return;
			}*/
		}

		string scontent = "";
		if (http_req["content"] != Json::Value::null)
		{
			scontent = http_req["content"].asString();
		}

		if (gate_client::gm_protocal_start_req < req_type && req_type < gate_client::gm_protocal_end_req)
		{
			if (!http_conn_mgr->is_from_gm_http(conn_ptr->get_connection_id()))
			{
				return;
			}
		}

		///////
		if (!http_conn_mgr->is_from_gm_http(conn_ptr->get_connection_id()))
		{
			return;
		}

		string msgStr = "msg";
		if (req_type == gate_client::gm_kick_player_req)
		{//后台踢人下线
			short resp_type = gate_client::gm_kick_player_resp;
			client_map::iterator it_player = _player.find(player_id);
			if (it_player == _player.end())
			{//玩家不在线
				Json::Value respJson;
				respJson[msgStr][0] = 1;
				string s = respJson.toStyledString();
				na::msg::msg_json mj(resp_type, s);
				conn_ptr->write_msg(mj);
				return;
			}
			
			client_map::iterator it_client = _map_client.find(it_player->second->get_net_id());
			if(it_client == _map_client.end())	
			{//玩家不在线
				Json::Value respJson;
				respJson[msgStr][0] = 1;
				string s = respJson.toStyledString();
				na::msg::msg_json mj(resp_type, s);
				conn_ptr->write_msg(mj);
				return;
			}

			kick_async(_player[player_id], true);

			Json::Value respJson;
			respJson[msgStr][0] = 0;
			string s = respJson.toStyledString();
			na::msg::msg_json mj(resp_type, s);
			conn_ptr->write_msg(mj);
		}
		else if (req_type == gate_client::gm_allow_ip_info_update_req)
		{//白名单信息查询
			short resp_type = gate_client::gm_allow_ip_info_update_resp;
			Json::Reader reader;
			Json::Value jval;
			int ret = 0;

			Json::Value ipJson = Json::arrayValue;

			if (ret == 0)
			{
				for (unsigned i = 0; i < _gdata._limit_ip.size(); i++)
				{
					ipJson.append(_gdata._limit_ip[i]);
				}
			}

			Json::Value respJson;
			respJson[msgStr][0u] = ret;
			respJson[msgStr][1] = ipJson;
			string s = respJson.toStyledString();
			na::msg::msg_json mj(resp_type, s);
			conn_ptr->write_msg(mj);
		}
		else if (req_type == gate_client::gm_allow_ip_set_login_req)
		{//白名单设置
			short resp_type = gate_client::gm_allow_ip_set_login_resp;
			Json::Reader reader;
			Json::Value jval;
			int ret = 0;
			if (!reader.parse(scontent, jval))
			{
				LogW << "json is not a object " << LogEnd;
				ret = -1;
			}
			
			Json::Value js_msg = jval[msgStr];
			if (!js_msg.isArray() || !js_msg[0].isArray())
			{
				LogW << "json is not a array " << LogEnd;
				ret = -1;
			}

			if (ret == 0)
			{
				_gdata._limit_ip.clear();
				for (unsigned i = 0; i < js_msg[0].size(); i++)
				{
					_gdata._limit_ip.push_back(js_msg[0][i].asString());
				}
				string sallow = js_msg[0].toStyledString();
				na::file_system::write_file(allow_ip_login_file, sallow);
			}

			Json::Value respJson;
			respJson[msgStr][0u] = ret;
			string s = respJson.toStyledString();
			na::msg::msg_json mj(resp_type, s);
			conn_ptr->write_msg(mj);
		}
		else
		{
			na::msg::msg_json mj(req_type, scontent);
			mj._player_id = player_id;
			mj._net_id = http_req["net_id"].asInt();
			if (DealMatch::isMatchFTReq(mj._type))
			{
				gate_svr.async_send_filter_svr(mj);
				return;
			}
			gate_svr.async_send_gamesvr(mj);
		}

	}

	void gate::printOnline()
	{
		boost::mutex::scoped_lock l(_mutex);
		LogS << ("Online:\t") << color_green(_map_client.size()) << LogEnd;
	}

// 	void gate::sens_word_filter(string& sword)
// 	{
// 		unsigned sz = _json_sens_words.size();
// 		for (unsigned i = 0; i < sz; i++)
// 		{
// 			int cur_pos = 0;
// 			int find_pos;
// 			string tmpword = _json_sens_words[i].asString();
// 			while((find_pos=sword.find(tmpword, cur_pos)) 
// 				!= std::string::npos)
// 			{
// 				unsigned star_size = 0;
// 				if (_json_sens_words[i].asString()[0] < 255 && tmpword[0] > 0)
// 				{
// 					star_size = tmpword.size();
// 				}
// 				else
// 				{
// 					star_size = tmpword.size() / 2;
// 				}
// 				sword.replace(find_pos, tmpword.length(), _harmony_words[star_size]);
// 				cur_pos = find_pos + tmpword.length();
// 			}
// 		}
// 	}

}


