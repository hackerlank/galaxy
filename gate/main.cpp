#include "gate_server.h"
#include "core_helper.h"
#include "cmd_helper.h"

bool is_limit_ip = false;
bool active_argv(int argc, char**argv)
{
	if (argc == 1)
		return true;

	for (int i = 1; i < argc; ++i)
	{
		string comm = argv[i];
		if (comm == "-l")
			is_limit_ip = true;
		else
		{
			LogI << "Start Failed. ---- Error commond." << LogEnd;
			return false;
		}
	}
	return true;
}

int main(int argc, char* argv[])
{
	if (!active_argv(argc, argv))
		return -1;

	logger.readConfig("./server/gate_cfg.json");
	logger.printConfig();
	coreM.init(gt::gt_count);
	gate_svr.init(is_limit_ip);
	gate_svr.run();
	while(1)
	{
		std::string input_str;
		getline(std::cin,input_str);
		if(input_str=="q")
			break;
		else if(input_str == "crack")
		{
			cmd::crack();
		}
		else if(input_str == "online")
		{
			gate_svr.printOnline();
		}
		else if (input_str == "httpshow")
		{
			http_conn_mgr->show_all();
		}
		else
		{
			LogW << color_red(input_str) << " is not a correct commands." << LogEnd;
		}
	}
	gate_svr.stop();
	return 0;
}