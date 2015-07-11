#pragma once
#include "activity_base.h"
#include <map>
#include "msg_base.h"
#include <boost/shared_ptr.hpp>
//#include "action_format.hpp"
#include "block.h"

#define point_sys (*gg::act_point::pointSys)

using namespace na::msg;
using namespace std;

namespace gg
{
	namespace point_def
	{
//		const static string str_act_db_point = "gl.activity_point";
		const static string strReceiveDueTime = "rdt";
		const static string strPointDataList = "pdls";
		const static string strPackName = "pn";
		const static string strPictureID = "ptid";
		const static string strPackType = "tp";
		const static string strCondition = "co";
		const static string strCanExchangeTimes = "et";
		const static string strRewardAction = "ra";
	}

	struct pointData
	{
		pointData();
		string packName_;
		int pictureID_;
		int condition_;
		int canExchangeTimes_;
		int type_;
		Json::Value reward_;
	};

	class act_point
		:public activity_base
	{
	public:
		static act_point* const pointSys;
		act_point();
		~act_point();
		virtual void package(Json::Value& update_json);
		virtual int modify(Json::Value& modify_json);
		virtual void set_to_default();
		void initData();

		void gmActivityPointModifyReq(msg_json& m, Json::Value& r);
		void pointUpdateReq(msg_json& m, Json::Value& r);
		void pointExchangeReq(msg_json& m, Json::Value& r);
		void pointPlayerInfoUpdateReq(msg_json& m, Json::Value& r);
		pointData getPointData(int exIndex);
		int getPointDataSize(){return vecPointData_.size();};
		void pointRecharge(playerDataPtr pdata, int amount);
		void update2clients(vector<playerDataPtr> players);
	private:
		void update2client(playerDataPtr player);
		bool isActivityEnd();
		virtual bool get();
		virtual bool save();
		int receiveDueTime_;
		vector <pointData> vecPointData_;
	};
}
