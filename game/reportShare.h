#pragma once

#include "playerManager.h"
#include "json/json.h"

using namespace na::msg;

#define shareSys (*gg::ReportShare::RShare)

namespace gg
{
	class ReportShare
	{
	public:
		static ReportShare* const RShare;
		ReportShare();
		~ReportShare(){}
		void shareLinkReq(msg_json& m, Json::Value& r);

		string shareLink(string reportPath);
	private:
		void sharedCheck(Json::Value json, const int playerID);
		void sharedDeal(int res, const string sIdx, Json::Value json, const int playerID);
		string getIdx();
		unsigned SharedIdx;
	};

}