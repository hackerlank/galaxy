#include "game_server.h"
#include "tcp_session.h"
#include "file_system.h"
#include "core_helper.h"
#include <boost/lexical_cast.hpp>
#include "common_protocol.h"
#include "time_helper.h"
#include "service_config.h"
#include "playerManager.h"
#include "mongoDB.h"
#include "away_skill.h"
#include "pilotManager.h"
#include "war_story.h"
#include "embattle_sys.h"
#include "world_system.h"
#include "item_system.h"
#include "season_system.h"
#include "workshop_system.h"
#include "science_system.h"
#include "action_format.hpp"
#include "guild_system.h"
#include "vip_system.h"
#include "study_system.h"
#include "battle_system.h"
#include "email_system.h"
#include "OnlineAwardSys.h"
#include "sign_system.h"
#include "arena_system.h"
#include "office_system.h"
#include "shop_system.h"
#include "chat_system.h"
#include "ruler_system.h"
#include "strategy_system.h"
#include "task_system.h"
#include "translate.h"
#include "commom.h"
#include "mine_resource_system.h"
#include "game_helper.hpp"
#include "junling_system.h"
#include "explore_manager.h"
#include "activity_game_param.h"
#include "system_response.h"
#include "activity_gift_defined.h"
#include "gm_tools.h"
#include "starwar_system.h"
#include "inspect_tour_system.h"
#include "InviteSystem.h"
#include "ally_system.h"
#include "space_explore_system.h"
#include "timmer.hpp"
#include "danmu.hpp"
#include "trade_system.h"
#include "level_rank.h"
#include "market_system.h"
#include "secretary_system.h"
#include "activity_system.h"
#include "paper_system.h"

#define t getDeadlineTimer()
namespace gg
{
	boost::asio::deadline_timer& getDeadlineTimer()
	{
		static boost::asio::deadline_timer dt(coreM.get_io(logic_io));
		return dt;
	}

	game::this_ptr game::getInstance()
	{
		static this_ptr p(new game());
		return p;
	}

	game::game() : _net_id(0)
	{
		tick = boost::get_system_time();
		showLog = false;
	}

	game::~game()
	{

	}

	void game::async_send_to_db(const char* data, int len)
	{
		if(_log_server->isVaild())_log_server->write(data, len);
	}

	void game::async_send_to_gate(const char* data, int len)
	{
		if (showLog)
		{
			na::msg::msg_base* mj = (na::msg::msg_base*)data;
			LogS << color_yellow("==============send================") << LogEnd;
			LogS << "type		id" << LogEnd;
			LogS << mj->_type << "		" << mj->_player_id << LogEnd;
		}
		client_map::iterator it = _map_client.find(service::process_id::GATE_NET_ID);
		if(it!=_map_client.end())
			it->second->write(data, len);
	}

	void game::async_send_to_gate(const short protocol, Json::Value& msg, const int netID, const int playerID)
	{
		string str = msg.toStyledString();
		na::msg::msg_json mj(protocol, str);
		mj._player_id = playerID;
		mj._net_id = netID;
		async_send_to_gate(mj);
	}

	void game::async_send_to_db(na::msg::msg_json& mj)
	{
		if(_log_server->isVaild())_log_server->write_json_msg(mj);
	}

	void game::async_send_to_gate(na::msg::msg_json& mj)
	{
		if (showLog)
		{
			LogS << color_yellow("==============send================") << LogEnd;
			LogS << "type		id" << LogEnd;
			LogS << mj._type << "		" << mj._player_id << LogEnd;
		}
		client_map::iterator it = _map_client.find(service::process_id::GATE_NET_ID);
		if(it!=_map_client.end())
			it->second->write_json_msg(mj);
	}

	void game::start_accept(acceptor_pointer ap)
	{	
		size_t n = _gdata.buffer; //byte 
		n *= 1024;//K
		n *= 1024;//M
		tcp_session_ptr new_connection = tcp_session::create( n , _gdata.is_tx ); // n(M)
		NetInsPtr nid = create_net_id();
		new_connection->set_net_id(nid);
		ap->async_accept(new_connection->socket(),
			boost::bind(&game::handle_accept,this, new_connection,ap,
			boost::asio::placeholders::error));
	}

	bool game::is_go_on_accept()
	{
		if((int)_map_client.size() >= _gdata.client_limit)
			return false;

		return true;
	}

	bool game::init()
	{
		Json::Value game_json = na::file_system::load_jsonfile_val("./server/game_cfg.json");
		Json::Value common_json = na::file_system::load_jsonfile_val("./server/common_cfg.json");
		_gdata.client_limit = game_json["client_limit"].asInt();
		_gdata.buffer = common_json["buffer_size"].asInt();
		_gdata.port = game_json["port"].asInt();
		_gdata.is_tx = common_json["is_tx"].asBool();
		_gdata._mongoDB = game_json["mongodb"].asString();
		_gdata._sql_ip = game_json["sql_ip"].asString();
		_gdata._sql_port = game_json["sql_port"].asString();

		if(!db_mgr.connect_db(_gdata._mongoDB.c_str()))
			return false;

		_ap = acceptor_pointer(new tcp::acceptor(coreM.get_io(na::net::base_io), 
			tcp::endpoint(tcp::v4(), _gdata.port)));
		start_accept(_ap);

		//TODO: init here
		int step = commom_sys.random() % 1000;
		for(int i = 0; i < step; ++i)NumberCounter::Step();
//		TransLate::initTranslateFile();
		resp_sys.initData();
		battle_sys.initBattleSystem();
		actionFormat::initialActionMap();
		player_mgr.initPlayerManager();
		playerBase::initPublicData();
		skill_sys.initSkillData();
		pilot_sys.initPilot();
		playerPilots::initData();
//		build_sys.initBuildData();
		war_story.initData();
		embattle_sys.initData();
		world_sys.initData();
		item_sys.initData();
		season_sys.initData();
		jl_sys.initData();
		BUFFTRIGGER::initBUFFTRIGGER();

		workshop_sys.initData();
		science_sys.initData();
		playerScience::initData();
		guild_sys.initData();
		vip_sys.initData();
		study_sys.initData();
		sign_sys.initData();
		arena_sys.initData();
		email_sys.initData();
		online_award_sys.initOnlineAward();
		office_sys.initData();
		shop_sys.initData();
//		BuffCheck::initialBuffMap();
//		buff_sys.initialData();
//		event_sys.initData();
		chat_sys.initData();
		ruler_sys.initData();
		strategy_sys.initData();
		task_sys.initData();
		mine_res_sys.initData();
		
		rebate_sys.initData();
		point_sys.initData();
		explore_sys.initData();
		guild_battle_sys.initData();
		act_game_param_mgr.initData();
		gift_defined_sys.initData();
		gm_tools_mgr.initData();
		starwar_sys.initData();
		inspect_tour_sys.initData();
		invite_sys.inintData();
		ally_sys.initData();
		space_explore_sys.initData();
		danmu_sys.initial();
		world_boss_sys.initData();
		trade_sys.initData();
		playerTradeSkill::initSkill();
		levelRank_sys.initData();
		market_sys.initData();
		secretary_sys.initData();
		activity_sys.initData();
		paper_sys.initData();
		
		//action_show_sys.initData();     //这个要在所有的系统后面
		_Logic_Post(boost::bind(&game::on_update,this));
		_Timer_Post(boost::bind(&Timer::EventUpdate));
		_game_handler = handler::pointer(new handler());
		LogS << "try to connect log svr..." << LogEnd;
		size_t n = _gdata.buffer;
		n = 1024 * 1024 * n;
		_log_server = tcp_session::create(n);
		connect_log_svr();
//		GlobalState::GlobalInit = true;
		LogI << color_green("game server start..") << LogEnd;

		return true;
	}

	void game::connect_log_svr()
	{
		NetInsPtr ptr = NetCreator::Create2(service::process_id::DB_NET_ID);
		_log_server->set_net_id(ptr);
		connect(_log_server,_gdata._sql_ip.c_str(), _gdata._sql_port.c_str());
	}

	void game::connect(tcp_session_pointer connector,const char* ip_str, const char* port_str)
	{
		if(connector->get_session_status()>gs_null)
			return;
		connector->set_session_status(gs_connecting);
		tcp::resolver resolver(connector->socket().get_io_service());
		tcp::resolver::query query(ip_str, port_str);
		tcp::resolver::iterator iterator = resolver.resolve(query);	

		boost::asio::async_connect(connector->socket(), iterator,
			boost::bind(&game::handle_connect,this,connector,
			boost::asio::placeholders::error));
	}

	void game::handle_connect(tcp_session_pointer connector,const boost::system::error_code& error)
	{
		if(_is_stop)
			return;
		boost::mutex::scoped_lock lock(_mutex);
		if (!error)
		{	
				connector->socket().non_blocking(true);
				connector->start(shared_from_this());
				connector->set_session_status(gs_game);
				if(connector == _log_server)
				{
					LogI<<  "log / db  server connected ..." <<LogEnd;
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
			connector->set_session_status(gs_null);

			if(connector->get_net_id()== service::process_id::DB_NET_ID)
			{
				LogE << error << LogEnd;
				LogE <<  "************* can not connect to db server *********" <<LogEnd;
				t.expires_from_now(boost::posix_time::seconds(5));
				t.async_wait(boost::bind(&game::connect_log_svr, this));
			}
		}
	}

	NetInsPtr game::create_net_id()
	{
		return NetCreator::Create2(service::process_id::GATE_NET_ID);
	}

	void game::on_update()
	{
		if(_is_stop) 			
			return;
		boost::system_time tmp = boost::get_system_time();
		while(true)
		{
			// lock lock lock
			boost::mutex::scoped_lock lock(_queue_mutex);
			if(!_msg_queue.empty()){
				na::msg::msg_json::ptr p = _msg_queue.front();
				_msg_queue.pop();			
				// unlock
				lock.unlock();
				std::string logstr = "[NID:";
				logstr += boost::lexical_cast<string,int>(p->_net_id);
				logstr += "] [PID:";
				logstr += boost::lexical_cast<string,int>(p->_player_id);
				logstr += "] [TYPE:";
				logstr += boost::lexical_cast<string,short>(p->_type);
				logstr += "]";
				time_logger l(logstr.c_str());
				on_recv_msg(p);
			}else{
				break;
			}

			boost::system_time now = boost::get_system_time();
			if ((now - tmp).total_milliseconds() >= 100){
				break;
			}
		}

		_game_handler->on_system_operating();

		if((tmp - tick).total_milliseconds() > 300000)
		{
			tick = tmp;
			checkkick(tmp);
			sendheartbeat();
		}

		na::time_helper::sleep(1);
		_Logic_Post(boost::bind(&game::on_update, this));
	}

	void game::sendheartbeat()
	{
		string str;
		na::msg::msg_json mj(game_protocol::comomon::keep_alive_req, str);
		if(_log_server->get_session_status() == gs_game)
			_log_server->write_json_msg(mj);
	}

	void game::client_connect_handler(tcp_session_ptr session,int error_value)
	{
		if(session == _log_server)
		{
			if(session->get_session_status()!=gs_connecting)
				_log_server->stop();
			else
				return;
			t.expires_from_now(boost::posix_time::seconds(5));
			t.async_wait(boost::bind(&game::connect_log_svr,this));
		}
		else
		{
			serverbase::client_connect_handler(session, error_value);
		}
	}

	void game::logShowState(const bool state)
	{
		showLog = state;
		_game_handler->logShowState(state);
		if (showLog)
		{
			LogS << "now message log is open ..." << LogEnd;
		}
		else
		{
			LogS << "now message log is close ..." << LogEnd;
		}
	}

	void game::internalCallback(na::msg::msg_json::ptr msg_ptr)
	{
		_game_handler->internalCallback(msg_ptr);
	}

	void game::on_recv_msg(const na::msg::msg_json::ptr recv_msg_ptr)
	{
		int netID = recv_msg_ptr->_net_id;
		if(netID == service::process_id::DB_NET_ID){
			_game_handler->recv_client_handler(_log_server, *recv_msg_ptr);
		}else
		if(gm_tools_mgr.is_from_gm_http(netID))
		{
			boost::mutex::scoped_lock l(_mutex);
			client_map::iterator it = _map_client.find(service::process_id::GATE_NET_ID);
			if(it == _map_client.end())return;
			_game_handler->recv_local_handler(it->second, *recv_msg_ptr);
		}else
		if(netID >= 0 && recv_msg_ptr->_net_id <= service::process_id::CLIENT_NET_ID_RANGE)
		{
			if(recv_msg_ptr->_player_id <= 0)
			{
				LogE << "vaild netID with a invaild playerID : " << recv_msg_ptr->_player_id <<
					"\tmsg type : " << recv_msg_ptr->_type << LogEnd;
				return;
			}
			boost::mutex::scoped_lock l(_mutex);
			client_map::iterator it = _map_client.find(service::process_id::GATE_NET_ID);
			if(it == _map_client.end())return;
			_game_handler->recv_client_handler(it->second, *recv_msg_ptr);
		}else
		if(netID == service::process_id::ACCOUNT_NET_ID)
		{
			boost::mutex::scoped_lock l(_mutex);
			client_map::iterator it = _map_client.find(service::process_id::GATE_NET_ID);
			if(it == _map_client.end())return;
			_game_handler->recv_local_handler(it->second, *recv_msg_ptr);
		}else
		if (service::process_id::HTTP_OHTER_START_NET_ID <= netID && netID <= service::process_id::HTTP_OHTER_END_NET_ID)
		{
		}
		else
		{
		}
	}

	void game::checkkick(boost::system_time& now)
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

	void game::kick(tcp_session_ptr session, bool notify /* = false */)
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

	void game::kick_async(tcp_session_pointer session)
	{
		boost::mutex::scoped_lock l(_mutex);
		kick(session);
	}
}
