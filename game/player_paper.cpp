#include "player_paper.h"
#include "mongoDB.h"
#include "player_data.h"
#include "helper.h"
#include "paper_system.h"
#include "playerManager.h"
#include "record_system.h"
#include "paper_system.h"

namespace gg
{
	PAPER::Record::Record(playerDataPtr player, const string fromPaper, const int appPaper)
	{
		playerID = player->playerID;
		name = player->Base.getName();
		level = player->Base.getLevel();
		kingdom = player->Base.getSphereID();
		time = na::time_helper::get_current_time();
		key = fromPaper;
		application = appPaper;
		faceID = player->Base.getFaceID();
	}

	playerPaper::playerPaper(playerData& own) : Block(own)
	{
		token = 0;
		totalReward = 0;
		totalSend = 0;
		_active.clear();
		_invalid.clear();
		_recv.clear();
		CD = na::time_helper::get_current_time();
		memset(dirty, 0x0, sizeof(dirty));
	}

	bool playerPaper::get()
	{
		objCollection objs = db_mgr.Query(PlayerPaper::strPlayerPaperDB, BSON(playerIDStr << own.playerID));
		for (unsigned i = 0; i < objs.size(); ++i)
		{
			mongo::BSONObj& obj = objs[i];
			int type = obj["tp"].Int();
			if (type == 0)
			{
				token = obj["tk"].Int();
				continue;
			}
			else if (type == 1)
			{
				totalReward = obj["rw"].Int();
				vector<mongo::BSONElement> vec = obj["dl"].Array();
				_recv.clear();
				_recv.resize(vec.size());
				for (unsigned i = 0; i < vec.size(); ++i)
				{
					PAPER::Record rc;
					mongo::BSONObj rcObj = vec[i].Obj();
					rc.fomat(rcObj);
					_recv[i] = rc;
				}
				continue;
			}
			else if (type == 2)
			{
				checkNotEoo(obj["sd"]){ totalSend = obj["sd"].Int(); }
				{
					vector<mongo::BSONElement> vec = obj["ac"].Array();
					_active.clear();
					_active.resize(vec.size());
					for (unsigned i = 0; i < vec.size(); ++i)
					{
						PlayerPaper::Data data;
						mongo::BSONObj dataObj = vec[i].Obj();
						data.fomat(dataObj);
						_active[i] = data;
					}
				}

				{
					vector<mongo::BSONElement> vec = obj["in"].Array();
					_invalid.clear();
					_invalid.resize(vec.size());
					for (unsigned i = 0; i < vec.size(); ++i)
					{
						PlayerPaper::Data data;
						mongo::BSONObj dataObj = vec[i].Obj();
						data.fomat(dataObj);
						_invalid[i] = data;
					}
				}
				continue;
			}
			else
			{
				db_mgr.remove_collection(PlayerPaper::strPlayerPaperDB, BSON(playerIDStr << own.playerID << "tp" << type));
			}

		}
		return true;
	}

	bool playerPaper::save()
	{
		if (dirty[0])
		{
			dirty[0] = false;
			mongo::BSONObj obj = BSON(playerIDStr << own.playerID << "tp" << 0 << "tk" << token);
			db_mgr.save_mongo(PlayerPaper::strPlayerPaperDB, BSON(playerIDStr << own.playerID << "tp" << 0), obj);
		}
		if (dirty[1])
		{
			dirty[1] = false;
			mongo::BSONArrayBuilder arr;
			for (unsigned i = 0; i < _recv.size(); ++i)
			{
				arr << _recv[i].toBson();
			}
			mongo::BSONObj obj = BSON(playerIDStr << own.playerID << "tp" << 1 << "rw" <<
				totalReward << "dl" << arr.arr());
			db_mgr.save_mongo(PlayerPaper::strPlayerPaperDB, BSON(playerIDStr << own.playerID << "tp" << 1), obj);
		}
		if (dirty[2])
		{
			dirty[2] = false;
			mongo::BSONArrayBuilder arr0, arr1;
			for (unsigned i = 0; i < _active.size(); ++i)
			{
				arr0 << _active[i].toBson();
			}
			for (unsigned i = 0; i < _invalid.size(); ++i)
			{
				arr1 << _invalid[i].toBson();
			}
			mongo::BSONObj obj = BSON(playerIDStr << own.playerID << "tp" << 2
				<< "ac" << arr0.arr() << "in" << arr1.arr() << "sd" << totalSend);
			db_mgr.save_mongo(PlayerPaper::strPlayerPaperDB, BSON(playerIDStr << own.playerID << "tp" << 2), obj);
		}
		return true;
	}

	void playerPaper::autoUpdate()
	{
		if (dirty[0])//主动更新红包币
		{
			updateToken();
		}
	}

	void playerPaper::robRecord(const string key, const int leave, const int token, const int reward)
	{
		for (unsigned i = 0; i < _active.size(); ++i)
		{
			PlayerPaper::Data& cData = _active[i];
			if (cData.key == key)
			{
				totalSend += reward;
				cData.leave = leave;
				cData.token = token;
				if (cData.isEmpty())
				{
					pushPaper(i);
				}
				dirty[2] = true;
				helper_mgr.insertSave(this);
				return;
			}
		}
	}

	void playerPaper::dispatchRecord(PlayerPaper::Data& record)
	{
		_active.insert(_active.begin(), record);
		dirty[2] = true;
		helper_mgr.insertSave(this);
	}

	void playerPaper::recvRecord(PAPER::Record& record)
	{
		_recv.insert(_recv.begin(), record);
		if (_recv.size() > 30)_recv.pop_back();
		totalReward += record.reward;
		dirty[1] = true;
		helper_mgr.insertSave(this);
	}

	bool playerPaper::canActivePaper()
	{
		return _active.size() < 10;
	}

	void playerPaper::deadType(const int type)
	{
		for (unsigned i = 0; i < _active.size(); ++i)
		{
			PlayerPaper::Data& cData = _active[i];
			if (type == cData.applicable)
			{
				cData.alive = false;
				dirty[2] = true;
				paper_sys.invalidPaper(cData.key);
			}
		}
		if (dirty[2])
		{
			helper_mgr.insertSave(this);
		}
	}

	void playerPaper::pushPaper(const unsigned idx)
	{
		_invalid.insert(_invalid.begin(), _active[idx]);
		if (_invalid.size() > 20)
		{
			paper_sys.delPaper(_invalid.back().key);
			_invalid.pop_back();
		}
		_active.erase(_active.begin() + idx);
		dirty[2] = true;
		helper_mgr.insertSave(this);
	}

	int playerPaper::recoveryPaper(const string key)
	{
		for (unsigned i = 0; i < _active.size(); ++i)
		{
			PlayerPaper::Data& cData = _active[i];
			if (cData.key == key)
			{
				unsigned now = na::time_helper::get_current_time();
				if (now > (cData.time + 1800) && cData.token > 0)
				{
					int num = cData.token;
					alterToken(num);
					cData.token = 0;
					pushPaper(i);
					paper_sys.invalidPaper(key);
					return num;
				}
				break;
			}
		}
		return 0;
	}

	const static string mysqlLogPaper = "log_paper";
	void playerPaper::alterToken(const int num)
	{
		int tmp = token;
		token += num;
		token = token < 0 ? 0 : token;
		StreamLog::Log(mysqlLogPaper, own, tmp, token);
		dirty[0] = true;
		helper_mgr.insertSaveAndUpdate(this);
	}

	bool playerPaper::checkKey(const string key)
	{
		for (unsigned i = 0; i < _active.size(); ++i)
		{
			if (key == _active[i].key)return true;
		}
		return false;
	}

	void playerPaper::updateToken()
	{
		Json::Value json;
		json[msgStr][0u] = 0;
		json[msgStr][1u]["tk"] = token;
		own.sendToClient(gate_client::paper_update_personal_resp, json);
	}

	void playerPaper::updateRecv()
	{
		Json::Value json;
		json[msgStr][0u] = 1;
		json[msgStr][1u]["rw"] = totalReward;
		Json::Value& jsonData = json[msgStr][1u]["dl"];
		jsonData = Json::arrayValue;
		for (unsigned i = 0; i < _recv.size(); ++i)
		{
			jsonData.append(_recv[i].toJson());
		}
		own.sendToClient(gate_client::paper_update_personal_resp, json);
	}

	void playerPaper::updateHistory()
	{
		Json::Value json;
		json[msgStr][0u] = 2;
		json[msgStr][1u]["rw"] = totalSend;
		Json::Value& jsonData = json[msgStr][1u]["dl"] = Json::arrayValue;
		for (unsigned i = 0; i < _active.size(); ++i)
		{
			jsonData.append(_active[i].toJson());
		}
		for (unsigned i = 0; i < _invalid.size(); ++i)
		{
			jsonData.append(_invalid[i].toJson());
		}
		own.sendToClient(gate_client::paper_update_personal_resp, json);
	}
}