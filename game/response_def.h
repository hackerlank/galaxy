#pragma once

#define PARSEJSON \
	Json::Value js;\
	Json::Reader reader;\
	std::string recv_str = m._json_str_utf8;\
	reader.parse(recv_str, js);\

#define ReadJsonArray \
	Json::Value js;\
	Json::Reader reader;\
	std::string recv_str = m._json_str_utf8;\
	if(! reader.parse(recv_str, js)){\
		LogW << "recv string :" << string(m._json_str_utf8, m._total_len - json_str_offset) << LogEnd;\
		LogW << "json parse failed " << __FUNCTION__ << \
		"	player_id: " << m._player_id << "	type " << m._type << LogEnd;\
		return;\
	}\
	if(! js.isObject()){\
	LogW << "recv string :" << js.toStyledString() << LogEnd;\
		LogW << "json is not a object " << __FUNCTION__ << \
			"	player_id: " << m._player_id << "	type " << m._type << LogEnd;\
		return;\
	}\
	Json::Value& js_msg = js[msgStr];\
	if(!js_msg.isArray()){\
		LogW << "recv string :" << js.toStyledString() << LogEnd;\
		LogW << "ReadJsonArray error " << __FUNCTION__ << \
		"	player_id: " << m._player_id << "	type " << m._type << LogEnd;\
		return;\
	}\


#define AsyncGetPlayerData(MSGJSON) \
	playerDataPtr d = player_mgr.getPlayerMain(MSGJSON._player_id, MSGJSON._net_id);\
	if(MSGJSON._post_times < 1 && playerDataPtr() == d){\
		player_mgr.postMessage(MSGJSON);\
		return;\
	}\
	if(MSGJSON._post_times > 0 && playerDataPtr() == d)Return(r, -1);\


#define GGSafeReadObject(NAME,JSON)\
	if(! JSON.isObject())\
	return;\
	Json::Value& NAME  = JSON;

#define GGSafeReadArray(NAME,JSON)\
	if(! JSON.isArray())\
	return;\
	Json::Value& NAME  = JSON;

#define GGSafeReadInt(NAME,JSON) \
	if(JSON.isObject() || JSON.isArray())\
	return;\
	int NAME = JSON.asInt();

#define GG_Safe_Read_String(NAME,JSON)\
	if(JSON.isObject() || JSON.isArray())\
	return;\
	string NAME = JSON.asString();

#define UnAcceptRetrun(CON, CODE)\
	if( !( CON ) ){\
	r[msgStr][0u] = CODE;\
	return;\
	}

#define FormatString(JSON, NAME)\
	if(JSON.isString()){\
		NAME = JSON.asString();\
	}

#define FormatInt(JSON, NAME)\
	if(JSON.isInt()){\
		NAME = JSON.asInt();\
	}

#define CheckIsObj(JSON)\
	if(JSON.isObject())

#define CheckIsArray(JSON)\
	if(JSON.isArray())

#define Return(JSON, CODE)\
	{\
		JSON[msgStr][0u] = CODE;\
		return;\
	}
