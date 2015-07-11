#pragma once

namespace service
{
	namespace process_id
	{
		const static int GATE_NET_ID = -100;
		const static int GAME_NET_ID = -101;
		const static int DB_NET_ID = -102;

//		const static int FEE_NET_ID = -103;
		const static int ACCOUNT_NET_ID = -104;

		const static int FILTER_NET_ID = -106;

		const static int CLIENT_NET_ID_RANGE = 6000;
				
		const static int HTTP_GM_END_NET_ID = -200;
		const static int HTTP_GM_START_NET_ID = HTTP_GM_END_NET_ID - 100;

		const static int HTTP_OHTER_END_NET_ID = -400;
		const static int HTTP_OHTER_START_NET_ID = HTTP_OHTER_END_NET_ID - 100;

	}
}