#pragma once
#include "json/json.h"
#include "msg_base.h"
#include "playerManager.h"
#include <boost/thread/detail/singleton.hpp>

#define office_sys (*gg::office_system::officeSys)

using namespace na::msg;

namespace gg
{
	namespace office
	{
		const static string strOfficeDataPath = "./instance/office/office.json";
		const static string strWeiWangCost = "weiwang_cost";
		const static string strLevel = "level";
		const static string strRequireWeiWang = "requireWeiWang";
		const static string strSalary = "salary";
		const static string strCanOccupyFarmlandNum = "canOccupyFarmlandNum";
		const static string strCanOccupySilverMineNum = "canOccupySilverMineNum";
		const static string strCanRecruitGeneralNum = "canRecruitGeneralNum";
		const static string strAddCanRecruitGeneralRawId = "addCanRecruitGeneralRawId";
		const static string strMysqlLogOffice = "log_office";

	}

	struct officeItemConfig 
	{
		officeItemConfig();
		int level;
		int requireWeiWang;
		int salary;
		int canOccupyFarmlandNum;
		int canOccupySilverMineNum;
		int canRecruitGeneralNum;
		vector<int> addCanRecruitGeneralRawId;
		static officeItemConfig nullItemConfig;
	};

	class office_system
	{
	public:
		static office_system* const officeSys;
		office_system();
		~office_system();
		void initData();
		void officeUpdateReq(msg_json& m, Json::Value& r);
		void officeReceiveSalaryReq(msg_json& m, Json::Value& r);
		void officePromoteReq(msg_json& m, Json::Value& r);
		int getPilotCanEnlistNum(int level);
		const officeItemConfig getOfficeItem(int level);
	private:
		map<int, officeItemConfig> _officeConfig;
	};
}
