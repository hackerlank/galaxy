#pragma once

#include "core.h"

namespace gg
{
	namespace DangerIO
	{
		enum
		{
			mongo_io = na::net::base_count,
			battle_io,
			timer_io,
			gg_count,
		};
	}
}

const static int LogicThreadID = na::net::logic_io;
const static int BattleThreadID = gg::DangerIO::battle_io;
const static int TimerThreadID = gg::DangerIO::timer_io;

//logic io
#define _Logic_Post coreM.get_io(na::net::logic_io).post
//battle io
#define _Battle_Post coreM.get_io(::gg::DangerIO::battle_io).post
//timer io
#define _Timer_Post coreM.get_io(::gg::DangerIO::timer_io).post

#define  BBIND boost::bind
//«ÎŒÀÊ“‚ π”√
#define _Db_Post coreM.get_io(::gg::DangerIO::mongo_io).post 

