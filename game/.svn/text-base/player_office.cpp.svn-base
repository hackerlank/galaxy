#include "player_office.h"
#include "playerManager.h"
#include "office_system.h"
#include "helper.h"
#include "task_system.h"

namespace gg
{
	playerOffice::playerOffice(playerData& own) : Block(own)
	{
		nextReceiveSalaryTime_ = 0;
		officialLevel_ = 1;
		isPromote_ = false;
	}

	void playerOffice::autoUpdate()
	{
		Json::Value msg;
		package(msg);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::office_update_resp, msg);

	}

	bool playerOffice::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(office::officeDBStr, key);

		if(!obj.isEmpty())
		{
			officialLevel_ = obj[office::playerOfficialLevelStr].Int();
			nextReceiveSalaryTime_ = obj[office::playerNxtRcvSalaryTimeStr].Int();
			return true;
		}

		return false;
	}

	bool playerOffice::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;

		obj << playerIDStr << own.playerID;
		obj << office::playerOfficialLevelStr << officialLevel_;
		obj << office::playerNxtRcvSalaryTimeStr << nextReceiveSalaryTime_;

		return db_mgr.save_mongo(office::officeDBStr, key, obj.obj());
	}

	void playerOffice::package(Json::Value& pack)
	{
		pack[msgStr][0u] = 0;
		pack[msgStr][1] = officialLevel_;
		unsigned now = na::time_helper::get_current_time();
		bool hasRecvSlr = (now > nextReceiveSalaryTime_ ? false : true);
		pack[msgStr][2] = hasRecvSlr;
		pack[msgStr][3] = own.Ruler.getTitle();
		pack[msgStr][4] = isPromote_;
		isPromote_ = false;
	}

	int playerOffice::setOffcialLevel(int offcialID)
	{
		officialLevel_ = offcialID;
		isPromote_ = true;
		helper_mgr.insertSaveAndUpdate(this);

		task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_nobility_lv);
		return 0;
	}

	int playerOffice::setNextRcvSlrTime(unsigned recvTime)
	{
		nextReceiveSalaryTime_ = recvTime;
		helper_mgr.insertSaveAndUpdate(this);
		return 0;
	}

	bool playerOffice::canPromote(int weiwang)
	{
		if (officialLevel_ >= 50 || officialLevel_ < 0)
		{
			return false;
		}

		officeItemConfig off = office_sys.getOfficeItem(officialLevel_+1);
		if (weiwang < off.requireWeiWang)
		{
			return false;
		}

		return true;
	}

	int playerOffice::getPilotCanEnlistNum()
	{
		return office_sys.getPilotCanEnlistNum(officialLevel_);
	}
}
