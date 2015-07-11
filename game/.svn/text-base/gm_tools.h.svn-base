#pragma once
#include "json/json.h"
#include "msg_base.h"
#include <set>
#include "block.h"

#define gm_tools_mgr (*gg::gm_tools::gmMgr)

using namespace na::msg;
//½¢³¤Êý
namespace gg
{
	namespace gm_tool_def
	{
		static const string strKey = "key";
		static const string strCustomList = "cls";
		static const string strDbGmTools = "gl.gm_base";
	}

	class gm_tools
	{
	public:
		static gm_tools* const gmMgr;
		gm_tools();
		~gm_tools();
		void playerBaseInfoUpdateReq(msg_json& m, Json::Value& r);
		void modifyPlayerBaseInfoReq(msg_json& m, Json::Value& r);
		void sendResourceToPlayerReq(msg_json& m, Json::Value& r);
		void gmSendGiftCardToPlayerReq(msg_json& m, Json::Value& r);
		void gmSendEmailToPlayerReq(msg_json& m, Json::Value& r);
		void gmCustomServiceReplyReq(msg_json& m, Json::Value& r);
		Json::Value complex2simple(Json::Value complexAction);
		Json::Value simple2complex(Json::Value simpleAction);
		Json::Value lastDo2simple(Json::Value lastDoJson);
		bool is_from_gm_http(int net_id);
		void initData();
		void customServiceNotity(playerDataPtr player);
		void actTimePretreat(int &timePre, int opt = 0);
		bool isGmProtocal(int msgType);
	private:
		bool get();
		bool save();
		set <int> customServiceList_;
	};
}

