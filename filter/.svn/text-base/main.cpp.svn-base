#include "filter_server.h"
#include "nedhelper.hpp"
#include "core_helper.h"

int main(int argc, char* argv[])
{
	logger.readConfig("./server/filter_cfg.json");
	logger.printConfig();
	coreM.init(ft::ft_count);
	config_ins.load_config_json("./server/common_cfg.json");
	filter_svr->init();
	filter_svr->run();
	while(1)
	{
		std::string input_str;
		getline(std::cin,input_str);
		if(input_str=="q")
			break;
	}
	filter_svr->stop();
	return 0;
}