#include "record_system.h"
#include "game_server.h"
#include "config.h"
#include "game_log_protocol.h"
#include "commom.h"
using namespace gg;
namespace StreamLog
{
	static void sendLog(const short logReq, Json::Value& logData)
	{
		string logstr = logData.toStyledString();
		na::msg::msg_json mj(logReq, logstr);
		game_svr->async_send_to_db(mj);
	}

	void Log(const string logTable, const int playerID, const string playerName, const int playerLV, const string explain, const int NowValue, const int Tag)
	{
		Log(logTable, playerID, playerName, playerLV, explain, boost::lexical_cast<string ,int>(NowValue), Tag);
	}
	void Log(const string logTable, gg::playerDataPtr player, const string explain, const int NowValue, const int Tag)
	{
		Log(logTable, player->playerID, player->Base.getName(), player->Base.getLevel(), explain, NowValue, Tag);
	}
	void Log(const string logTable, gg::playerData& player, const string explain, const int NowValue, const int Tag)
	{
		Log(logTable, player.playerID, player.Base.getName(), player.Base.getLevel(), explain, NowValue, Tag);
	}
	void Log(const string logTable, const int playerID, const string playerName, const int playerLV, const int targetID, const string targetData, const int Tag)
	{
		Log(logTable, playerID, playerName, playerLV, boost::lexical_cast<string ,int>(targetID), targetData, Tag);
	}
	void Log(const string logTable, gg::playerDataPtr player, const int targetID, const string targetData, const int Tag)
	{
		Log(logTable, player->playerID, player->Base.getName(), player->Base.getLevel(), targetID, targetData, Tag);
	}
	void Log(const string logTable, gg::playerData& player, const int targetID, const string targetData, const int Tag)
	{
		Log(logTable, player.playerID, player.Base.getName(), player.Base.getLevel(), targetID, targetData, Tag);
	}
	void Log(const string logTable, const int playerID, const string playerName, const int playerLV, const int PreviousValue, const int NowValue, const int Tag)
	{
		Log(logTable, playerID, playerName, playerLV, boost::lexical_cast<string ,int>(PreviousValue), boost::lexical_cast<string ,int>(NowValue), Tag);
	}
	void Log(const string logTable, gg::playerDataPtr player, const int PreviousValue, const int NowValue, const int Tag)
	{
		Log(logTable, player->playerID, player->Base.getName(), player->Base.getLevel(), PreviousValue, NowValue, Tag);
	}
	void Log(const string logTable, gg::playerData& player, const int PreviousValue, const int NowValue, const int Tag)
	{
		Log(logTable, player.playerID, player.Base.getName(), player.Base.getLevel(), PreviousValue, NowValue, Tag);
	}
	void Log(const string logTable, gg::playerDataPtr player, const string PreviousValue, const string NowValue, const int Tag)
	{
		Log(logTable, player->playerID, player->Base.getName(), player->Base.getLevel(), PreviousValue, NowValue, Tag);
	}
	void Log(const string logTable, gg::playerData& player, const string PreviousValue, const string NowValue, const int Tag)
	{
		Log(logTable, player.playerID, player.Base.getName(), player.Base.getLevel(), PreviousValue, NowValue, Tag);
	}

	void Log(const string logTable, const int playerID, const string playerName,  const int playerLV, const string PreviousValue, const string NowValue, const int Tag)
	{
//		static const int serverID = config_ins.get_config_prame("server_id").asInt();
		Json::Value LogData;
		LogData.append(logTable);
		LogData.append(State::getState());
		LogData.append(playerID);
		LogData.append(playerName);
		LogData.append(playerLV);
		LogData.append(PreviousValue);
		LogData.append(NowValue);
		LogData.append(NumberCounter::getCounter());
		LogData.append(Tag);
//		LogData.append(commom_sys.serverID());
		if (playerID <= 0) LogData.append(commom_sys.serverID());
 		else LogData.append(playerID >> 20);
		sendLog(game_log::mysql_log_string_req, LogData);
	}


	//////////////////////////////////////////////////////////////////////////

	void Log(const string logTable, gg::playerDataPtr player, const string PreviousValue, const string NowValue, vector<string> fields, const int Tag)
	{
		Log(logTable, player->playerID, player->Base.getName(), player->Base.getLevel(), PreviousValue, NowValue, fields, Tag);
	}

	void Log(const string logTable, gg::playerData& player, const string PreviousValue, const string NowValue, vector<string> fields, const int Tag)
	{
		Log(logTable, player.playerID, player.Base.getName(), player.Base.getLevel(), PreviousValue, NowValue, fields, Tag);
	}

	void Log(const string logTable, const int playerID, const string playerName, const int playerLV, const string PreviousValue, const string NowValue, vector<string> fields, const int Tag)
	{
//		static const int serverID = config_ins.get_config_prame("server_id").asInt();
		Json::Value LogData;
		LogData.append(logTable);
		LogData.append(State::getState());
		LogData.append(playerID);
		LogData.append(playerName);
		LogData.append(playerLV);
		LogData.append(PreviousValue);
		LogData.append(NowValue);
		LogData.append(NumberCounter::getCounter());
		LogData.append(Tag);
//		LogData.append(commom_sys.serverID());
		if (playerID <= 0) LogData.append(commom_sys.serverID());
 		else LogData.append(playerID >> 20);
		for(vector<string>::iterator iter = fields.begin(); iter != fields.end(); ++iter)LogData.append(*iter);
		sendLog(game_log::mysql_log_system_req, LogData);
	}

	void LogV(const string logTable, const string PreviousValue, const string NowValue, const int Tag /* = -1 */, const string f1 /* = "" */, const string f2 /* = "" */, const string f3 /* = "" */, const string f4 /* = "" */, const string f5 /* = "" */, const string f6 /* = "" */ )
	{
		static vector<string> fields;
		fields.clear();
		fields.push_back(f1);
		fields.push_back(f2);
		fields.push_back(f3);
		fields.push_back(f4);
		fields.push_back(f5);
		fields.push_back(f6);
		Log(logTable, -1, "system", -1, PreviousValue, NowValue, fields, Tag);
	}

	void LogV(const string logTable, gg::playerDataPtr player, const string PreviousValue, const string NowValue, const int Tag /* = -1 */, const string f1 /* = "" */, const string f2 /* = "" */, const string f3 /* = "" */, const string f4 /* = "" */, const string f5 /* = "" */, const string f6 /* = "" */ )
	{
		static vector<string> fields;
		fields.clear();
		fields.push_back(f1);
		fields.push_back(f2);
		fields.push_back(f3);
		fields.push_back(f4);
		fields.push_back(f5);
		fields.push_back(f6);
		Log(logTable, player->playerID, player->Base.getName(), player->Base.getLevel(), PreviousValue, NowValue, fields, Tag);
	}

	void LogV(const string logTable, gg::playerDataPtr player, const int PreviousValue, const int NowValue, const int Tag /* = -1 */, const string f1 /* = "" */, const string f2 /* = "" */, const string f3 /* = "" */, const string f4 /* = "" */, const string f5 /* = "" */, const string f6 /* = "" */ )
	{
		LogV(logTable, player, boost::lexical_cast<string, int>(PreviousValue), boost::lexical_cast<string, int>(NowValue), Tag, f1, f2, f3, f4, f5, f6);
	}

	void LogV(const string logTable, gg::playerDataPtr player, const string PreviousValue, const int NowValue, const int Tag /* = -1 */, const string f1 /* = "" */, const string f2 /* = "" */, const string f3 /* = "" */, const string f4 /* = "" */, const string f5 /* = "" */, const string f6 /* = "" */ )
	{
		LogV(logTable, player, (PreviousValue), boost::lexical_cast<string, int>(NowValue), Tag, f1, f2, f3, f4, f5, f6);
	}

	void LogV(const string logTable, gg::playerDataPtr player, const int PreviousValue, const string NowValue, const int Tag /* = -1 */, const string f1 /* = "" */, const string f2 /* = "" */, const string f3 /* = "" */, const string f4 /* = "" */, const string f5 /* = "" */, const string f6 /* = "" */ )
	{
		LogV(logTable, player, boost::lexical_cast<string, int>(PreviousValue), NowValue, Tag, f1, f2, f3, f4, f5, f6);
	}

}