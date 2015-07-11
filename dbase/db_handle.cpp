#include "db_handle.h"
#include "common_protocol.h"
#include "db_server.h"
#include "game_log_protocol.h"
#include "service_config.h"
#include "mysqlManager.h"

namespace db
{
	const static unsigned bigBuffSize = 1024 * 1024 * 10;


	handler::handler()
	{

	}

	handler::~handler()
	{

	}

	void handler::recv_client_handler(tcp_session_ptr session,const char* data_ptr,int len)
	{
		if(len < (int)sizeof(na::msg::msg_base)) return;
		na::msg::msg_json::ptr p = na::msg::msg_json::create(data_ptr,len);

		session->keep_alive();
		if(p->_type == game_protocol::comomon::keep_alive_req)
		{
			string str;
			na::msg::msg_json mj(game_protocol::comomon::keep_alive_resp,str);
			mj._net_id = session->get_net_id();
			session->write_json_msg(mj);
			return;
		}

		try{
			if(p->_type == game_log::mysql_log_string_req){
				logSave_sys.log_string(data_ptr, len);
				return;
			}
			if(p->_type == game_log::mysql_log_system_req){
				logSave_sys.log_system(data_ptr, len);
				return;
			}
		}catch(std::exception& e){
			LogS << "log something error ~	" << e.what() << LogEnd;
		}
	}

	void handler::sendMessage(tcp_session_ptr session, Json::Value& msg)
	{
// 		string pak = msg.toStyledString();	
// 		na::msg::msg_json mj(game_log::write_battle_report_resp, pak);
// 		mj._net_id = service::process_id::DB_NET_ID;
// 		session->write_json_msg(mj);
	}


	void handler::client_connect_handler(tcp_session_ptr session,int error_value)
	{
		db_svr->disconnect(session, error_value);	
	}
}