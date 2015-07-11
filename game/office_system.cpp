#include "office_system.h"
#include "playerManager.h"
#include "file_system.h"
#include "response_def.h"
#include "helper.h"
#include "activity_system.h"

#define OFFICE_OPEN_LEVEL 16

namespace gg
{
	office_system* const office_system::officeSys = new office_system();

	office_system::office_system()
	{

	}

	office_system::~office_system()
	{

	}

	void office_system::initData()
	{
		db_mgr.ensure_index(office::officeDBStr, BSON(playerIDStr << 1));

		Json::Value confJson = na::file_system::load_jsonfile_val(office::strOfficeDataPath);
		for (unsigned idx = 0; idx < confJson.size(); idx++)
		{
			officeItemConfig temp;
			Json::Value tempJson = confJson[idx];
			temp.level = tempJson[office::strLevel].asInt();
			temp.requireWeiWang = tempJson[office::strRequireWeiWang].asInt();
			temp.salary = tempJson[office::strSalary].asInt();
			temp.canOccupyFarmlandNum = tempJson[office::strCanOccupyFarmlandNum].asInt();
			temp.canOccupySilverMineNum = tempJson[office::strCanOccupySilverMineNum].asInt();
			temp.canRecruitGeneralNum = tempJson[office::strCanRecruitGeneralNum].asInt();

			for (unsigned kidx = 0; kidx < tempJson[office::strAddCanRecruitGeneralRawId].size(); kidx++)
			{
				temp.addCanRecruitGeneralRawId.push_back(tempJson[office::strAddCanRecruitGeneralRawId][kidx].asInt());
			}

			_officeConfig[temp.level] = temp;
		}
	}

	void office_system::officeUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		if (d->Base.getLevel() < OFFICE_OPEN_LEVEL)
			Return(r, -1);

		d->Office.autoUpdate();
// 		r[msgStr][1] = d->Office.getOffcialLevel();
// 		unsigned nxtRecvSalaryTime = d->Office.getNextRcvSlrTime();
// 		unsigned now = na::time_helper::get_current_time();
// 		bool hasRecvSlr = (now > nxtRecvSalaryTime ? false : true);
// 		r[msgStr][2] = hasRecvSlr;
// 		r[msgStr][3] = d->Ruler.getTitle();
// 		Return(r, 0);
	}

	void office_system::officeReceiveSalaryReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		unsigned nxtRecvSalaryTime = d->Office.getNextRcvSlrTime();
		unsigned now = na::time_helper::get_current_time();
		bool hasNotRecvSlr = (now > nxtRecvSalaryTime ? true : false);
		if (d->Base.getLevel() < OFFICE_OPEN_LEVEL)
			Return(r, 2);

		if (now > nxtRecvSalaryTime)
		{
			int salary = _officeConfig[d->Office.getOffcialLevel()].salary;
			int title = d->Ruler.getTitle();

			if (title == _title_king)
			{
				salary = 4 * salary;
			}
			else if (title == _title_councilor || title == _title_consul)
			{
				salary = 3 * salary;
			}
			else if (_title_affairs_minister <= title && title <= _title_defense_minister)
			{
				salary = int(2.5 * salary);
			}
			else if (_title_building_envoy <= title && title <= _title_culture_envoy)
			{
				salary = int(2 * salary);
			}
			else
			{
			}
			d->Base.alterSilver(salary);
			unsigned standardTime = na::time_helper::get_time_zero(now);
			nxtRecvSalaryTime = (unsigned)na::time_helper::get_next_time(standardTime, 1, 5);
			d->Office.setNextRcvSlrTime(nxtRecvSalaryTime);
			activity_sys.updateActivity(d, 0, activity::_get_salary);
			Return(r, 0);
		}
		else
		{
			Return(r, 1);
		}
	}

	void office_system::officePromoteReq(msg_json& m, Json::Value& r)
	{
// 		AsyncGetPlayerData(m);
// 		officeBase temp = _officeConfig[d->Office.getOffcialLevel()];
// 		if (d->Office.getOffcialLevel() >= 50)
// 		{//¹ÙÂú
// 			Return(r, 1);
// 		}
// 		if (d->Base.getWeiWang() < temp.requireWeiWang)
// 		{//ÍþÍû²»¹»
// 			Return(r, 2);
// 		}
// 		int l = d->Office.getOffcialLevel();
// 		d->Office.setOffcialLevel(l + 1);
// 		d->Base.alterWeiWang(-temp.requireWeiWang);
// 		Return(r, 0);
	}

	static officeItemConfig nullItemConfig = officeItemConfig();
	officeItemConfig::officeItemConfig()
	{
		level = 0;
		requireWeiWang = 0;
		salary = 0;
		canOccupyFarmlandNum = 0;
		canOccupySilverMineNum = 0;
		canRecruitGeneralNum = 0;
	}

	const officeItemConfig office_system::getOfficeItem(int level)
	{
		if (_officeConfig.find(level) != _officeConfig.end())
		{
			return _officeConfig[level];
		}
		return nullItemConfig;
	}

	int office_system::getPilotCanEnlistNum(int level)
	{
		return _officeConfig[level].canRecruitGeneralNum;
	}
}
