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
			string PaperKey;//���
			int subjectID;
			int faceID;
			string subjectName;
			int Applicable;//-1������ 0���� 1���� 2���� 3˽��
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
		int chanelID_;//Ƶ��ID
		int dispatchNum_;//��������
		int totalToken_;//token����
		string leaveMessage_;//����
		int limitID_;//����ID
		string name_;//˽��������
		string broadcast_;//����
		int actStartTime_;//����ʱ��
		int actEndTime_;//����ʱ��
		string preBroadcast_;//Ԥ�������������칫��
		string paperKey_;
		bool hasAddPaper_;
		bool sendFirstPreNotice_;
		bool sendSecondPreNotice_;
	};

	class PaperManager//���
	{
	public:
		static PaperManager* const paperSys;
		PaperManager();
		void update(msg_json& m, Json::Value& r);//���¸�����Ϣ
		void updatePaperTable(msg_json& m, Json::Value& r);//���������Ϣ��ȡ
		void dispatchPaper(msg_json& m, Json::Value& r);//�����
		void robPaper(msg_json& m, Json::Value& r);//��ȡ���
		void recoveryPaper(msg_json& m, Json::Value& r);//�������
		void paperSendagain(msg_json& m, Json::Value& r);//���·��ͺ������
		void paperExchange(msg_json& m, Json::Value& r);//����Ҷһ�
		void gmDispatchPaper(msg_json& m, Json::Value& r);//gm �����
		void gmPaperUpdateReq(msg_json& m, Json::Value& r);//gm ��ѯ���
		void updateRewardList(msg_json& m, Json::Value& r);//��ȡ�����ȡ��¼
		void innerSendGmPaperReq(msg_json& m, Json::Value& r);//��ʱ������gm���
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