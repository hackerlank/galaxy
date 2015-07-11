#include "reportShare.h"
#include "response_def.h"
#include "battle_system.h"
#include <boost/lexical_cast.hpp>
#include "file_system.h"
#include "chat_system.h"
#include "guild_system.h"
#include <stdlib.h>
#include "core_helper.h"
#include <boost/bind.hpp>
#include "mineHeader.h"
#include "battle_system.h"
#include <boost/thread/mutex.hpp>

namespace gg
{
#define ParseJson(json)\
	int chatID = json[0u].asInt();\
	string signName = json[1u].asString();\
	string atkName = json[2u].asString();\
	string defName = json[3u].asString();\
	string httpLink = json[4u].asString();\
	string realatk = json[5u].asString();\
	string realdef = json[6u].asString();


	static unsigned char* BufferFile = new unsigned char[30720];//30k
	static unsigned char* BufferMem = new unsigned char[102400];//100K

	ReportShare* const ReportShare::RShare = new ReportShare();

	ReportShare::ReportShare():SharedIdx(0)
	{

	}


	void ReplaceCMDChar(string& str)
	{
#ifdef WIN32
		str = boost::replace_all_copy(str, "/", "\\");
#else
		str = boost::replace_all_copy(str, "\\", "/");
#endif
	}

	void ReportShare::sharedDeal(int res, const string sIdx, Json::Value json, const int playerID)
	{
		State::setState(gate_client::share_link_report_req);
		NumberCounter::Step();

		playerDataPtr player = player_mgr.getOnlinePlayer(playerID);
		if(player == NULL)return;

		if(res == 0)
		{
			ParseJson(json);
			Json::Value bCast;
			bCast[msgStr][0u] = 0;
			bCast[msgStr][1u][senderChannelStr] =chatID;
			bCast[msgStr][1u][chatBroadcastID] = BROADCAST::shared_report;
			const static string ParamListStr = "pl";
			bCast[msgStr][1u][ParamListStr].append(player->Base.getName());
			bCast[msgStr][1u][ParamListStr].append(realatk);
			bCast[msgStr][1u][ParamListStr].append(realdef);
			bCast[msgStr][1u][ParamListStr].append(sIdx);
			res = chat_sys.chatOption(chatID, gate_client::chat_broadcast_resp, bCast, player, signName);
			if(res == 0)
			{
				player->lastSharedTime = na::time_helper::get_current_time();
				player->Base.alterShared(-1);
			}
		}

		helper_mgr.runSaveAndUpdate();

		Json::Value resJson;
		resJson[msgStr][0u] = res;
		resJson[msgStr][1u] = sIdx;
		player->sendToClient(gate_client::share_link_report_resp, resJson);
	}

	void ReportShare::sharedCheck(Json::Value json, const int playerID)
	{
		int res = -1;
		string link = "";
		ParseJson(json);
		string rP = "./report" + httpLink;
		ReplaceCMDChar(rP);
		boost::filesystem::path path(rP);
		do 
		{
			if(!boost::filesystem::is_regular_file(path)){res = 100;break;}
			memset(BufferFile, 0x0, 30720);
			memset(BufferMem, 0x0, 102400);
			try
			{		
				ifstream i(rP.c_str(), std::ios::binary | std::ios::in);
				i.read((char*)BufferFile, 30720);
				unsigned long size = (unsigned long)i.gcount();
				unsigned long lsize = 102400;
				res = uncompress(BufferMem, &lsize, BufferFile, size);
				if(res != Z_OK){res = 101;break;}//文件解压失败
				Json::Value report;
				Json::Reader reader;
				std::string json_str((char*)BufferMem, lsize);
				if(! reader.parse(json_str, report)){res = 102; break;}//战报分享超时
				if(!report[BATTLE::strInfoAtk].isArray() || !report[BATTLE::strInfoDef].isArray()){res = 102; break;}//战报分享超时
				if(!report[BATTLE::strInfoAtk][2u].isString() || !report[BATTLE::strInfoDef][2u].isString()){res = 102; break;}//战报分享超时
				if(report[BATTLE::strInfoAtk][2u].asString() != atkName ||  defName != report[BATTLE::strInfoDef][2u].asString()){res = 102; break;}//战报分享超时
				link = getIdx();
				string fileLink = ShareReportPublic + link;
				ReplaceCMDChar(fileLink);
				std::ofstream write(fileLink.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
				write.write((const char*)BufferFile, size) ;
				write.flush();
				write.close();
				res = 0;
			}catch (std::exception& e)
			{
				LogE << e.what() << LogEnd;
				res = 103;//文件解析错误
				return;
			}
		} while (false);
		_Logic_Post(boost::bind(&ReportShare::sharedDeal, this, res, link, json, playerID));
	}

	string ReportShare::shareLink(string reportPath)
	{
		string link = getIdx();
		string wP = ShareReportPublic + link;
#ifdef WIN32
		ReplaceCMDChar(reportPath);
		ReplaceCMDChar(wP);
		string command = "copy /y " + reportPath + " " + wP;
#else
		string command = "cp -f " + reportPath + " " + wP;
#endif
		int res = std::system(command.c_str());
		if(res != 0)return "";
		return link;
	}

	void ReportShare::shareLinkReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if(d->Base.getShared() < 1)Return(r, 201);//分享次数
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		UnAcceptRetrun(js_msg[1u].isString(), -1);
		UnAcceptRetrun(js_msg[2u].isString(), -1);
		UnAcceptRetrun(js_msg[3u].isString(), -1);
		UnAcceptRetrun(js_msg[4u].isString(), -1);
		UnAcceptRetrun(js_msg[5u].isString(), -1);
		UnAcceptRetrun(js_msg[6u].isString(), -1);

		//ParseJson(js_msg);

		unsigned now = na::time_helper::get_current_time();
		if(now < d->lastSharedTime || now - d->lastSharedTime < 300)Return(r, 200);
		_Battle_Post(boost::bind(&ReportShare::sharedCheck, this, js_msg, d->playerID));
	}

	string ReportShare::getIdx()
	{
		static boost::mutex m;
		boost::mutex::scoped_lock lock(m);
		SharedIdx = ++SharedIdx % 100; 
		return boost::lexical_cast<string, unsigned>(SharedIdx);
	}

}