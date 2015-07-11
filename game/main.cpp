#include "game_server.h"
#include "core_helper.h"
#include "systemBase.h"
#include "time_helper.h"
#include "config.h"
#include "email_system.h"
#include "mineHeader.h"
#include "game_helper.hpp"
#include "playerManager.h"
#include "system_response.h"
#include "script.hpp"
#include "block.h"

using namespace gg;

int main(int argc, char* argv[])
{
	logger.readConfig("./server/game_cfg.json");
	logger.printConfig();
	mLua::initialLua();
#ifdef _WIN_UTF8_
	//system("chcp 65001");
	//system("chcp 936");
#endif
	coreM.init(DangerIO::gg_count);
	config_ins.load_config_json("./server/common_cfg.json");
	if(!game_svr->init()){
		LogE << "can not start game svr..." << LogEnd;
		na::time_helper::sleep(3800000);
		return -1;
	}
	game_svr->run();
	while(1)
	{
		std::string input_str;
		getline(std::cin,input_str);
		if(input_str=="q")
			break;
		else if(input_str == "clear")
		{
			LogS << "post clear message ... ..." << LogEnd;
			_Logic_Post(boost::bind(&playerManager::playerManagerClearRB, playerManager::playerMgr));
		}
		else if(input_str == "show")
		{
			LogS << "post show Player Manager Infomation ..." << LogEnd;
			_Logic_Post(boost::bind(&playerManager::showPlayerManagerInfo, playerManager::playerMgr));
		}
		else if (input_str == "add")
		{
			getline(std::cin, input_str);
			try
			{
				unsigned val = boost::lexical_cast<unsigned>(input_str);
				_Logic_Post(boost::bind(&na::time_helper::setDeviation, val));
				_Logic_Post(boost::bind(&gg::saveDev));
				LogI << "set dev sucess ..." << LogEnd;
			}
			catch (boost::bad_lexical_cast& e)
			{
				LogE << e.what() << LogEnd <<  input_str << " is not a number ..." << LogEnd;
			}
		}
		else if (input_str == "state")
		{
			LogS << State::getState() << LogEnd;
		}
		else if (input_str == "log")
		{
			static bool openType = false;
			openType = !openType;
			_Logic_Post(boost::bind(&game::logShowState, game_svr, openType));
		}
		else
		{
			LogW << color_red(input_str) << " is not a correct commands." << LogEnd;
		}
	}
	game_svr->stop();
	mLua::endLua();

	return 0;
}
