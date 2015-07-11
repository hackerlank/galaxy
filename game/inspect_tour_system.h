#pragma once 
#include "msg_base.h"
#include "email_system.h"
using namespace na::msg;
using namespace std;

#define inspect_tour_sys  (*gg::inspectTourSystem::inspectTourSys)

namespace gg
{
	namespace inspect_tour_def
	{
		enum
		{
			type_simple_start = -1,
			type_zhanchangfeixu, //战场废墟
			type_taikongxiulizhan, //太空修理站
			type_taikongchongnengzhan, //太空充能战
			type_qiujiuxinhao, //求救信号
			type_simple_end,

			type_complex_start = 9,
			type_xingjidadao, //星际大刀
			type_beikunshangchuan, //被困商船
			type_fukuangyunshi, //富矿陨石
			type_xijiehaidao, //洗劫海盗
			type_shenmishangdui, //神秘商队
			type_complex_end

		};
	}

	struct simpleTour 
	{
		int id;
		Json::Value simpleRewardJson;
	};

	typedef boost::shared_ptr<simpleTour> simpleTourPtr;

	class inspectTourSystem
	{
	public:
		static inspectTourSystem* const inspectTourSys;
		inspectTourSystem();
		~inspectTourSystem();
		void inspectTourUpdateReq(msg_json& m, Json::Value& r);
		void inspectTourSimpleFinishReq(msg_json& m, Json::Value& r);
		void inspectTourXingjidadaoReq(msg_json& m, Json::Value& r);
		void inspectTourBeikunshangchuanReq(msg_json& m, Json::Value& r);
		void inspectTourFukuangyunshiReq(msg_json& m, Json::Value& r);
		void inspectTourXijiehaidaoReq(msg_json& m, Json::Value& r);
		void inspectTourShenmishangduiReq(msg_json& m, Json::Value& r);
		void initData();
		simpleTourPtr getSimpleTourConfig(int eventID);
		emailTeam getEmailTeam(int eventID);
	private:
		map<int, simpleTourPtr> simpleTourInfo_;
		Json::Value rewardJsonXingjidadao[2];
		Json::Value rewardJsonBeikunshangchuan[3];
		Json::Value rewardJsonXijiehaidao[3];
		Json::Value rewardJsonShenmishangdui[2];
	};
}
