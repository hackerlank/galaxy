#pragma once

#include "protocol.h"

namespace protocol
{
	namespace c2l
	{
		enum 
		{
			c2l_begin		= protocol::Gate2LoginBegin,
			register_req,	// json: { "msg":["account","passwd" ]}
			login_req,		// json: { "msg":[user_type,openid,timestamp,sign_str,qd_key,ver,imei]}	
			logout_req,
			changePassword_req,//[oldPW,newPW]

			charge_gold_req,	// ["account",order_id,gold,status]
			system_keep_alive, // json []
			log_out_all_player_req,
			update_session_key_req,
			c2l_end
		};
	}

	namespace l2c
	{
		enum
		{
			l2c_begin		= protocol::Login2GateBegin,
			register_resp,	// json: { "msg":["account",0] }	(0:failed,1:success)  
			login_resp,		// json: { "msg":[0,player_id,"提示字符"] }	(0:failed,1:success,2:already login,3:ban user)  
			logout_resp,
			changePassword_resp,// [(-1:非法操作,0:成功,1:旧密码错误)]
			charge_gold_resp,	// [(-1:非法操作,0:成功,1:失败),OrderId,Gold]

			update_session_key_resp = 118,
			l2c_end
		};
	}
}