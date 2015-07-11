#pragma once
#include "json/json.h"
#include "msg_base.h"
#include "player_email.h"
#include "action_format.hpp"

#define email_sys (*gg::email_system::emailSys)

using namespace na::msg;

namespace gg
{
	class email_system
	{
	public:
		static email_system* const emailSys;
		email_system();
		~email_system();
		void initData();
		void mailUpdateReq(msg_json& m, Json::Value& r);
		void writeToPlayerReq(msg_json& m, Json::Value& r);
		void saveMailReq(msg_json& m, Json::Value& r);
		void getGiftReq(msg_json& m, Json::Value& r);
		void delListReq(msg_json& m, Json::Value& r);
		void delSavelistReq(msg_json& m, Json::Value& r);
		void writeToAllGuildMemberReq(msg_json& m, Json::Value& r);

		void sendPlayerEmailByPlayer(playerDataPtr sender, playerDataPtr receiver, Json::Value contentJson);
		void sendPlayerEmailByGuildLeader(playerDataPtr sender, vector<playerDataPtr> receiver, Json::Value contentJson);
		void sendSystemEmailCommon(playerDataPtr d, emailType email_type, emailTeam email_team, Json::Value contentJson = Json::Value::null, Json::Value attachJson = Json::Value::null);
		void sendSystemEmailByGm(playerDataPtr d, vector<attachManager>& attachs, Json::Value contentJson = Json::Value::null);
		bool sendDoJumpFailEmail(playerDataPtr player, actionVector actionVect, vector<actionResult> &vecRetCode, emailTeam email_team, Json::Value &failSimpleJson);
		bool sendDoJumpFailEmail(playerDataPtr player, Json::Value actionJson, vector<actionResult> &vecRetCode, emailTeam email_team, Json::Value &failSimpleJson);
		void dealDoJumpFailLastDo(actionVector actionVec, vector<actionResult> &vecRetCode, Json::Value &lastDoJson);
		void dealDoJumpFailLastDo(Json::Value actionJsonArr, vector<actionResult> &vecRetCode, Json::Value &lastDoJson);
	private:
		int getAttachPictureID(Json::Value attachJson);
		int sendToPlayer (playerDataPtr d, playerEmailItemPtr mail_ptr, bool is_check_size = true);
		void remove_json_member(Json::Value& jval, string mem);
	};
}
