#include "danmu.hpp"
#include "response_def.h"
#include "chat_system.h"
#include "timmer.hpp"

namespace gg
{
	barrage* const barrage::barrageSys = new barrage();

	barrage::barrage()
	{

	}

	void transfer_channel(playerDataPtr player, int& ID)
	{

	}

	void barrage::initial()
	{
		_channel[boss_channel] = Channel(boss_channel);

		Timer::AddEventSeconds(boost::bind(&barrage::update, this), na::time_helper::ONEHOUR);
	}

	void barrage::update()
	{
		for (channelIt it = _channel.begin(); it != _channel.end();++it)
		{
			Channel& _c = it->second;
			_c.toList();
		}
		LogI << "clear barrage listener ..." << LogEnd;
		Timer::AddEventSeconds(boost::bind(&barrage::update, this), na::time_helper::ONEHOUR);
	}

	void barrage::DanmuReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		string str = js_msg[0u].asString();
		if (commom_sys.UTF8Len(str) > 15 || str.length() < 1)Return(r, 1);//¹ý³¤
		int cID = js_msg[1u].asInt();
		int orgID = cID;
		if (!(cID > -1 && cID < end_channel))Return(r, -1);
		unsigned now = na::time_helper::get_current_time();
		if (d->DanmuArray[cID] > now || now - d->DanmuArray[cID] < 20)Return(r, 2); //cd
		d->DanmuArray[cID] = now;
		transfer_channel(d, cID);
		Channel& cn = getChannel(cID);
		if (!cn.isVaild())Return(r, -1);
		cn.addListener(d->playerID, d->netID);
		sendList sL = cn.toList();
		Json::Value danmuJson;
		danmuJson[msgStr][0u] = 0;
		danmuJson[msgStr][1u]["dm"] = str;
		danmuJson[msgStr][1u]["sdr"] = d->Base.getName();
		danmuJson[msgStr][1u]["da"] = js_msg[2u];
		danmuJson[msgStr][1u]["cn"] = orgID;
		MsgSignOnline(sL, danmuJson, gate_client::chat_danmu_resp);
	}

	void barrage::ListenerTick(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int cID = js_msg[0u].asInt();
		if (!(cID > -1 && cID < end_channel))Return(r, -1);
		transfer_channel(d, cID);
		Channel& cn = getChannel(cID);
		if (!cn.isVaild())Return(r, -1);
		cn.addListener(m._player_id, m._net_id);
		Return(r, 0);
	}

	barrage::Channel barrage::nullChannel;

	barrage::Channel& barrage::getChannel(const int ID)
	{
		channelIt it = _channel.find(ID);
		if (it == _channel.end())return nullChannel;
		return it->second;
	}
}