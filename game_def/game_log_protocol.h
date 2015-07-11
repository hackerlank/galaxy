#pragma once

#include "protocol.h"
#include <string>
using namespace std;

namespace game_log
{
	enum{
		mysql_log_string_req = protocol::Game2MysqlBegin,
		mysql_log_system_req,
	};

	enum{
		mysql_log_string_resp = protocol::Mysql2GameBegin,
		mysql_log_system_resp,
	};

}