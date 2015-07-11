#include "ally_system.h"
#include "response_def.h"

namespace gg
{
	ally_system* const ally_system::allySys = new ally_system;

#define loadPlayer(m) \
	ReadJsonArray; \
	int player_id = m._player_id; \
	int target_id = js_msg[0u].asInt(); \
	if(player_id == target_id) Return(r, -1); \
	playerDataPtr d = player_mgr.getPlayerMain(player_id); \
	playerDataPtr targetP = player_mgr.getPlayerMain(target_id); \
	if(d == playerDataPtr() || targetP == playerDataPtr()) \
	{ \
		if(m._post_times < 1) \
		{ \
			player_mgr.postMessage(m); \
			return; \
		} \
		else \
			Return(r, -1); \
	} \

	void ally_system::initData()
	{
		loadFile();
	}

	void ally_system::loadFile()
	{
		Json::Value info = na::file_system::load_jsonfile_val(ally_reward_file_dir_str);
		_gold = info[0u].asUInt();
		_extra_gold = info[1u].asUInt();
	}

	void ally_system::searchReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int player_id = m._player_id;
		string name = js_msg[0u].asString();

		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		playerDataPtr targetP = player_mgr.getPlayerMainByName(name);
		if(d == playerDataPtr() || targetP == playerDataPtr())
		{
			if(m._post_times < 1)
			{
				player_mgr.postMessage(m);
				return;
			}
			else
				Return(r, -1);
		}

		Json::Value msg;
		allyBaseInfo info(targetP);
		info.package(msg[msgStr][1u]);
		msg[msgStr][0u] = 0;
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::ally_search_resp, msg); 
	}

	void ally_system::updateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		bool is_first = js_msg[0u].asBool();
		
		d->Ally.setFlag(is_first);
		d->Ally.update();
	}

	void ally_system::sendReq(msg_json& m, Json::Value& r)
	{
		loadPlayer(m);

		int result = d->Ally.send(targetP);
		if(result == 0)
			d->Ally.update();
		Return(r, result);
	}

	void ally_system::cancelReq(msg_json& m, Json::Value& r)
	{
		loadPlayer(m);

		int result = d->Ally.cancel(targetP);
		if(result == 0)
			d->Ally.update();
		Return(r, result);
	}

	void ally_system::agreeReq(msg_json& m, Json::Value& r)
	{
		loadPlayer(m);

		int result = d->Ally.agree(targetP);
		if(result == 0)
			d->Ally.update();
		Return(r, result);
	}

	void ally_system::refuseReq(msg_json& m, Json::Value& r)
	{
		loadPlayer(m);

		int result = d->Ally.refuse(targetP);
		if(result == 0)
			d->Ally.update();
		Return(r, result);
	}

	void ally_system::dissolveReq(msg_json& m, Json::Value& r)
	{
		loadPlayer(m);

		int result = d->Ally.dissolve(targetP);
		if(result == 0)
			d->Ally.update();
		Return(r, result);
	}

	void ally_system::delReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int type = js_msg[0u].asInt();
		int target_id = js_msg[1u].asInt();

		int result = d->Ally.del(type, target_id);
		if(result == 0)
			d->Ally.update();
		Return(r, result);
	}

	void ally_system::rewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		int get_gold = 0;
		int extra_gold = 0;
		int result = d->Ally.getReward(get_gold, extra_gold);
		if(result == 0)
		{
			d->Ally.update();
			r[msgStr][1u] = get_gold;
			r[msgStr][2u] = extra_gold;
		}
		Return(r, result);
	}

	void ally_system::opUpdateInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int type = js_msg[0u].asInt();
		int state = js_msg[1u].asInt();
		int target_id = js_msg[2u].asInt();

		vector<playerDataPtr> vec;
		for(unsigned i = 3; i < js_msg.size(); ++i)
		{
			playerDataPtr d = player_mgr.getPlayerMain(js_msg[i].asInt());
			if(d != playerDataPtr())
				vec.push_back(d);
		}

		if(vec.size() != js_msg.size() - 3)
		{
			if(m._post_times < 2)
				player_mgr.postMessage(m);
			return;
		}

		for(vector<playerDataPtr>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			(*iter)->Ally.handleApplicant(type, target_id, state);
		}
	}

	void ally_system::postToUpdateInfo(playerDataPtr d)
	{
		if(d->Ally.getAllyInfo().empty())
			return;

		int target_id = d->Ally.getAllyInfo().getPlayerId();
		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(target_id);
		msg[msgStr].append(d->playerID);

		string str = msg.toStyledString();
		na::msg::msg_json m(gate_client::ally_info_update_inner_req, str);
		player_mgr.postMessage(m);
	}

	void ally_system::infoUpdateInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int player_id = js_msg[0u].asInt();
		int target_id = js_msg[1u].asInt();

		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		playerDataPtr targetP = player_mgr.getPlayerMain(target_id);
		if(d == playerDataPtr() || targetP == playerDataPtr())
		{
			if(m._post_times < 2)
				player_mgr.postMessage(m);
			return;
		}

		d->Ally.alterAllyInfo(targetP);
	}

	void ally_system::activeOpRewardInnerReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		int player_id = js_msg[0u].asInt();
		int target_id = js_msg[1u].asInt();
		playerDataPtr d = player_mgr.getPlayerMain(player_id);
		playerDataPtr targetP = player_mgr.getPlayerMain(target_id);
		if(d == playerDataPtr() || targetP == playerDataPtr())
		{
			if(m._post_times < 2)
				player_mgr.postMessage(m);
			return;
		}

		if(d->Ally.actived())
			return;

		unsigned now = na::time_helper::get_current_time();
		targetP->Ally.checkAndUpdate(now);
		targetP->Ally.setbLogin(playerAlly::_other, true);
		d->Ally.active(true);
	}
}
