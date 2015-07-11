#pragma once

#include "json/json.h"
#include "msg_base.h"
#include "gate_server.h"
#include "common_protocol.h"
#include <iostream>
#include "service_config.h"
using namespace std;

namespace cmd
{
	void crack()
	{
		static string str = "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
		for (unsigned i = 0; i < 10000; ++i)
		{
			na::msg::msg_json mj(-1, service::process_id::GATE_NET_ID, game_protocol::comomon::keep_alive_req, str);
			gate_svr.async_send_gamesvr(mj);
		}
	}
}