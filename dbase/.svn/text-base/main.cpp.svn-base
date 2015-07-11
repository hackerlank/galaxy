#include <ctime>
#include <iostream>
#include <string>
#include "db_server.h"
#include <mysql++.h>
#include <iomanip>
#include <ssqls.h>
#include "mysqlManager.h"
#include "core_helper.h"
#include <json/json.h>
#include <file_system.h>
#include "mineHeader.h"
//using namespace std;
#include "nedhelper.hpp"

int main()
{
	try
	{
		logger.readConfig("./server/db_cfg.json");
		logger.printConfig();
		logSave_sys;
		coreM.init(db::db_count);
		config_ins.load_config_json("./server/common_cfg.json");
		db_svr->init("./server/db_cfg.json");
		db_svr->run();
		while(1)
		{
			char c = getchar();
			if(c=='q')break;
		}
		db_svr->stop();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << LogEnd;
	}

	return 0;
}
