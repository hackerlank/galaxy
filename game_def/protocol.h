#pragma once

namespace protocol
{
	enum
	{
		Gate2LoginBegin	=	10,			
		Login2GateBegin	=	110,

		CommonReqBegin = 300,
		CommonRespBegin = 500,		

		Gate2GameBegin	=	1000,
		Gate2GameEnd = 11000,
		Game2GateBegin	=	11000,
		Game2GateEnd = 21000,

		Game2MysqlBegin =	30000,
		Mysql2GameBegin =	31000,
	};
}