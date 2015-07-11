#pragma once

#include "playerManager.h"
#include <set>
#include <boost/unordered_map.hpp>
#include <vector>


using namespace na::msg;
using namespace std;

#define paper_sys  (*gg::PaperManager::paperSys)

namespace gg
{
	namespace PAPER
	{
//	 		chat_all = -1,
// 			chat_kingdom = 0,
// 			chat_area = 1,
// 			chat_guild = 2,
// 			chat_player = 3,
		struct DATA
		{
			DATA()
			{
				PaperKey = "";
				subjectID = -1;
				faceID = -1;
				subjectName = "";
				Applicable = -1;
				Leave = 0;
				Param = Json::nullValue;
				Deadline = 0;
				Total = 0;
				Token = 0;
				totalToken = 0;
				Message = "";
				Alive = true;
			}
			bool LastOne()
			{
				return Leave >= Total;
			}
			bool Invalid()
			{
				return (!Alive || Leave >= Total || na::time_helper::get_current_time() >= Deadline);
			}
			bool isSystem()
			{
				return subjectID < 0;
			}
			int LeavePaperNum()
			{
				int num = Total - Leave;
				return num < 0 ? 0 : num;
			}
			string PaperKey;//红包
			int subjectID;
			int faceID;
			string subjectName;
			int Applicable;//-1所有人 0国家 1地区 2星盟 3私人
			int Leave;
			int Total;
			int Token;
			int totalToken;
			Json::Value Param;
			unsigned Deadline;
			bool Alive;
			string Message;
			RcList RCList;
			bool canRob(playerDataPtr player)
			{
				if (Applicable == chat_all)
				{}
				else if (Applicable == chat_kingdom)
				{
					if (Param[0u].asInt() != player->Base.getSphereID())return false;
				}
				else if (Applicable == chat_area)
				{
					if (Param[0u].asInt() != player->Base.getPosition().first)return false;
				}
				else if (Applicable == chat_guild)
				{
					if (Param[0u].asInt() != player->Guild.getGuildID())return false;
				}
				else if (Applicable == chat_player)
				{
					if (Param[0u].asInt() != player->playerID)return false;
				}
				else
				{
					return false;
				}

				for (unsigned i = 0; i < RCList.size(); ++i)
				{
					if (RCList[i].playerID == player->playerID)return false;
				}
				return true;
			}

			Json::Value toJson()
			{
				Json::Value json;
				json["k"] = PaperKey;
				json["n"] = subjectName;
				json["ap"] = Applicable;
				json["la"] = Leave;
				json["tal"] = Total;
				json["ttk"] = totalToken;
				json["msg"] = Message;
				json["fid"] = faceID;
				return json;
			}
			mongo::BSONObj toBson()
			{
				mongo::BSONArrayBuilder arr;
				for (unsigned i = 0; i < RCList.size(); ++i)
				{
					arr << RCList[i].toBson();
				}
				return BSON("k" << PaperKey << "id" << subjectID << "n" << subjectName <<
					"ap" << Applicable << "la" << Leave << "p" << commom_sys.json2string(Param) <<
					"dl" << Deadline << "tal" << Total << "tk" << Token << "ttk" << totalToken << 
					"msg" << Message << "fid" << faceID << "rl" << arr.arr() << "al" << Alive);
			}

			Json::Value list_toJson()
			{
				Json::Value json = Json::arrayValue;
				for (unsigned i = 0; i < RCList.size(); ++i)
				{
					json.append(RCList[i].toJson());
				}
				return json;
			}

			void fomat(mongo::BSONObj& obj)
			{
				PaperKey = obj["k"].String();
				subjectID = obj["id"].Int();
				subjectName = obj["n"].String();
				Applicable = obj["ap"].Int();
				Leave = obj["la"].Int();
				Deadline = (unsigned)obj["dl"].Int();
				Total = obj["tal"].Int();
				Token = obj["tk"].Int();
				totalToken = obj["ttk"].Int();
				Param = commom_sys.string2json(obj["p"].String());
				Message = obj["msg"].String();
				faceID = obj["fid"].Int();
				Alive = obj["al"].Bool();
				vector<mongo::BSONElement> vec = obj["rl"].Array();
				RCList.clear();
				RCList.resize(vec.size());
				for (unsigned i = 0; i < vec.size(); ++i)
				{
					Record rc;
					mongo::BSONObj rcBson = vec[i].Obj();
					rc.fomat(rcBson);
					RCList[i] = rc;
				}
			}
		};

		typedef boost::shared_ptr < DATA > DataPtr;

		static void Destory(DATA* pointer)
		{
			GGDelete(pointer);
		}

		static DataPtr Create()
		{
			return DataPtr(new(GGNew(sizeof(DATA))) DATA(), Destory);
		}
	}

	struct actPaperData 
	{
		actPaperData();
		~actPaperData();
		Json::Value toJson();
		int actID_;
		string actName_;
		int chanelID_;//频道ID
		int dispatchNum_;//分派数量
		int totalToken_;//token总数
		string leaveMessage_;//留言
		int limitID_;//限制ID
		string name_;//私聊人名字
		string broadcast_;//布告
		int actStartTime_;//开启时间
		int actEndTime_;//开启时间
		string preBroadcast_;//预告滚动公告和聊天公告
		string paperKey_;
		bool hasAddPaper_;
		bool sendFirstPreNotice_;
		bool sendSecondPreNotice_;
	};

	class PaperManager//红包
	{
	public:
		static PaperManager* const paperSys;
		PaperManager();
		void update(msg_json& m, Json::Value& r);//更新个人信息
		void updatePaperTable(msg_json& m, Json::Value& r);//红包界面信息获取
		void dispatchPaper(msg_json& m, Json::Value& r);//发红包
		void robPaper(msg_json& m, Json::Value& r);//领取红包
		void recoveryPaper(msg_json& m, Json::Value& r);//红包回收
		void paperSendagain(msg_json& m, Json::Value& r);//重新发送红包连接
		void paperExchange(msg_json& m, Json::Value& r);//红包币兑换
		void gmDispatchPaper(msg_json& m, Json::Value& r);//gm 发红包
		void gmPaperUpdateReq(msg_json& m, Json::Value& r);//gm 查询红包
		void updateRewardList(msg_json& m, Json::Value& r);//获取红包领取记录
		void innerSendGmPaperReq(msg_json& m, Json::Value& r);//定时器发送gm红包
		void delPaper(const string key);
		void invalidPaper(const string key);
		void initData();
		void scheduleUpdate(boost::system_time& sys_time);
	private:
		void DeleteOldPaper();
		typedef vector<PAPER::DataPtr>  Rank;
		typedef boost::unordered_map < string, PAPER::DataPtr > PaperMap;
		Rank rank;
		PaperMap Papers;
		boost::system_time tick_;
		int duration_;
		PAPER::DataPtr getPaper(const string key);
		string createKey();
		void addPaper(PAPER::DataPtr paper);
		void RankList();
		void savePaper(PAPER::DataPtr paper);
		vector<actPaperData> vecActPaperList_;
		void initActPaper();
		void readActivityPaper();
		void saveActivityPaper();
		void refreshActivityPaper();
		void sendGmPaper(actPaperData &actData);
		void sendPreGmPaperBroadCast(actPaperData actData);
	};
}