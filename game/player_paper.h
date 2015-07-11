#pragma once

#include "block.h"
#include <vector>
using namespace std;

namespace gg
{
	namespace PAPER
	{
		struct Record
		{
			Record()
			{
				faceID = -1;
				playerID = -1;
				name = "";
				kingdom = -1;
				level = 0;
				time = 0;
				reward = 0;
				key = "";
				application = -1;
			}
			Record(playerDataPtr player, const string fromPaper, const int appPaper);
			string key;
			int playerID;
			int faceID;
			string name;
			int kingdom;
			int  level;
			unsigned time;
			int reward;
			int application;
			Json::Value toJson()
			{
				Json::Value json;
				json["k"] = key;
				json["na"] = name;
				json["kid"] = kingdom;
				json["lv"] = level;
				json["tm"] = time;
				json["rw"] = reward;
				json["fid"] = faceID;
				json["ap"] = application;
				return json;
			}

			mongo::BSONObj toBson()
			{
				return BSON("pi" << playerID << "na" << name << "kid" << kingdom << "lv" << level <<
					"tm" << time << "rw" << reward << "k" << key << "fid" << faceID << "ap" << application);
			}

			void fomat(mongo::BSONObj& obj)
			{
				playerID = obj["pi"].Int();
				name = obj["na"].String();
				kingdom = obj["kid"].Int();
				level = obj["lv"].Int();
				time = unsigned(obj["tm"].Int());
				reward = obj["rw"].Int();
				key = obj["k"].String();
				checkNotEoo(obj["fid"]){ faceID = obj["fid"].Int(); }
				checkNotEoo(obj["ap"]){ application = obj["ap"].Int(); }
			}
		};

		typedef vector<Record> RcList;
	}

	namespace PlayerPaper
	{
		const static string strPlayerPaperDB = "gl.player_paper";

		struct Data
		{
			Data()
			{
				key = "";
				applicable = -1;
				leave = 0;
				total = 0;
				token = 0;
				totalToken = 0;
				time = 0;
				alive = true;
			}
			string key;
			bool alive;
			int applicable;//-1所有人 0国家 1地区 2星盟 3私人
			int leave;
			int total;
			int token;
			int totalToken;
			unsigned time;
			bool isEmpty()
			{
				return (leave >= total || token <= 0);
			}
			Json::Value toJson()
			{
				Json::Value json;
				json["k"] = key;
				json["ap"] = applicable;
				json["tal"] = total;
				json["la"] = leave;
				json["tm"] = time;
				json["tk"] = token;
				json["ttk"] = totalToken;
				json["al"] = alive;
				return json;
			}

			mongo::BSONObj toBson()
			{
				return BSON("k" << key << "ap" << applicable << "tal" << total << "la" << leave << "tm" << time <<
					"tk" << token << "ttk" << totalToken << "al" << alive);
			}

			void fomat(mongo::BSONObj& obj)
			{
				key = obj["k"].String();
				applicable = obj["ap"].Int();
				total = obj["tal"].Int();
				leave = obj["la"].Int();
				token = obj["tk"].Int();
				totalToken = obj["ttk"].Int();
				time = (unsigned)obj["tm"].Int();
				alive = obj["al"].Bool();
			}
		};
	}

	class playerPaper :
		public Block
	{
	public:
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate();

		void deadType(const int type);
		void updateToken();
		void updateRecv();
		void updateHistory();
		playerPaper(playerData& own);
		bool checkKey(const string key);
		inline int getToken(){ return token; }
		void alterToken(const int num);
		void robRecord(const string key, const int leave, const int token, const int reward);//抢红包记录
		void recvRecord(PAPER::Record& record);//收到金钱的时候记录
		void dispatchRecord(PlayerPaper::Data& record);//_active红包记录
		bool canActivePaper();//最多激活红包数量 10个
		int recoveryPaper(const string key);//红包回收
		unsigned CD;
	private:
		void pushPaper(const unsigned idx);
		int token;//0
		int totalReward;//1
		int totalSend;
		typedef vector< PlayerPaper::Data > HistoryVector;
		HistoryVector _active, _invalid;// 10, 20  total 30 //2
		PAPER::RcList _recv;// 30 //1
		bool dirty[3];
	};
}