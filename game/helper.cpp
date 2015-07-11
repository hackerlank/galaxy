#include "helper.h"
#include "war_story.h"
#include "Glog.h"
#include "world_system.h"
#include "science_system.h"
#include "game_server.h"
#include "mineHeader.h"
#include "core_helper.h"
#include <boost/bind.hpp>
#include "file_system.h"

namespace gg
{
	const static unsigned bigBuffSize = 1024 * 1024 * 2;
	void writeToFile(string filePath, const char* data, const unsigned len)
	{
		static unsigned char* bigBuff = (unsigned char*)GGNew(bigBuffSize);
		//memset(bigBuff, 0x0, bigBuffSize);
		try
		{
			std::ofstream f(filePath.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
			unsigned long lsize = bigBuffSize;
			int res = compress(bigBuff, &lsize, (const unsigned char*)data, len);
			if (res != Z_OK){
				LogE << "zlib compress failed ... error code :" << res << LogEnd;
			}
			else{
				f.write((const char*)bigBuff, lsize);
				f.flush();
			}
			f.close();
		}
		catch (std::exception& e)
		{
			LogE << e.what() << LogEnd;
		}
	}


	void writeToFile(string filePath, Json::Value json)
	{
// 		string writeData = json.toStyledString();
// 		writeData = commom_sys.tighten(writeData);
		string writeData = commom_sys.json2string(json);
		writeToFile(filePath, writeData.c_str(), writeData.length());
	}

	void writeToFile(string filePath, string str)
	{
		//str = commom_sys.tighten(str);
		writeToFile(filePath, str.c_str(), str.length());
	}

	void zip_write(string path, string str)
	{
		_Battle_Post(boost::bind((void(*)(string,string))&writeToFile, path, str));
	}

	void zip_write(string path, Json::Value data)
	{
		_Battle_Post(boost::bind((void(*)(string, Json::Value))&writeToFile, path, data));
	}

	Json::Value zip_read_json(string path)
	{
		static unsigned char* bigBuff = (unsigned char*)GGNew(bigBuffSize);
		static unsigned char* source = (unsigned char*)GGNew(bigBuffSize);
		ifstream i(path.c_str(), std::ios::binary | std::ios::in);
		i.read((char*)source, bigBuffSize);
		unsigned long lsize = bigBuffSize;
		unsigned long size = (unsigned long)i.gcount();
		int res = uncompress(bigBuff, &lsize, source, size);
		if (res != Z_OK)
		{
			return Json::nullValue;
		}
		Json::Value json;
		Json::Reader reader;
		if (false == reader.parse((const char*)bigBuff, (const char*)bigBuff + lsize + 1, json))
		{
			LogE << "failed to parse file " << path << " " << reader.getFormatedErrorMessages() << LogEnd;
			return Json::Value::null;
		}
		return json;
	}

	sendMember::sendMember(const int playerID, const int netID) : _pID(playerID), _nID(netID)
	{

	}

	void MsgSignOnline(sendList& vec, Json::Value& msgJson, const short protocol, const int threadID)
	{
		if (vec.empty())return;
		string str = msgJson.toStyledString();
		na::msg::msg_json mj_begin(protocol, threadID, gate_client::game_set_gate_buffer_resp, str);
		game_svr->async_send_to_gate(mj_begin);

		for (unsigned i = 0; i < vec.size(); ++i)
		{
			const sendMember& player = vec[i];
			na::msg::msg_json mj_player(player._pID,
				player._nID * 100 + threadID, protocol);
			game_svr->async_send_to_gate(mj_player);
		}

		na::msg::msg_json mj_end(protocol, threadID, gate_client::game_clear_gate_buffer_resp);
		game_svr->async_send_to_gate(mj_end);
	}

	void MsgSignOnline(playerManager::playerDataVec& vec, Json::Value& msgJson, const short protocol, const int threadID /*= LogicThreadID*/)
	{
		if (vec.empty())return;
		string str = msgJson.toStyledString();
		na::msg::msg_json mj_begin(protocol, threadID, gate_client::game_set_gate_buffer_resp, str);
		game_svr->async_send_to_gate(mj_begin);

		for (unsigned i = 0; i < vec.size(); ++i)
		{
			playerDataPtr player = vec[i];
			na::msg::msg_json mj_player(player->playerID,
				player->netID * 100 + threadID, protocol);
			game_svr->async_send_to_gate(mj_player);
		}

		na::msg::msg_json mj_end(protocol, threadID, gate_client::game_clear_gate_buffer_resp);
		game_svr->async_send_to_gate(mj_end);
	}

	helper* const helper::helperMgr = new helper();

	helper::helper()
	{
	}

	void helper::insertUpdate(Block* pointer)
	{
		if (pointer == NULL)return;
		update.insert(pointer);
	}

	void helper::insertSave(Block* pointer)
	{
		if (pointer == NULL)return;
		save.insert(pointer);
	}

	void helper::insertSaveAndUpdate(Block* pointer)
	{
		if (pointer == NULL)return;
		save.insert(pointer);
		update.insert(pointer);
	}

	void helper::runSaveAndUpdate()
	{		
		for (SaveSet::iterator it = update.begin(); it != update.end();)
		{
			SaveSet::iterator cIt = it;
			++it;
			(*cIt)->autoUpdate();
		}

		for (SaveSet::iterator it = save.begin(); it != save.end();)
		{
			SaveSet::iterator cIt = it;
			++it;
			(*cIt)->save();
			(*cIt)->doEnd();
		}

		save.clear();
		update.clear();
	}

}