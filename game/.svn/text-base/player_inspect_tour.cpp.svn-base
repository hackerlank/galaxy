#include "player_inspect_tour.h"
#include "playerManager.h"
#include "helper.h"

namespace gg
{
#define FUKUANGYUANSHI_TIMES 5

	playerInspectTour::playerInspectTour(playerData& own) : Block(own)
	{
	}

	bool playerInspectTour::save()
	{
		return false;
	}

	bool playerInspectTour::get()
	{
		return false;
	}

	void playerInspectTour::autoUpdate()
	{
		Json::Value msg;
		package(msg);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::inspect_tour_update_resp, msg);
	}

	void playerInspectTour::package(Json::Value& pack)
	{
		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();
		pack[msgStr][1u][inspect_tour_def::strTourEventList] = Json::arrayValue;
		for (unsigned i = 0; i < tourEventList_.size(); i++)
		{
			pack[msgStr][1u][inspect_tour_def::strTourEventList].append(tourEventList_[i]);
		}
	}

	bool playerInspectTour::canOperate(int eventID)
	{
		return find(tourEventList_.begin(), tourEventList_.end(), eventID) != tourEventList_.end();
	}

	int playerInspectTour::operateFukuangyunshi(int &randomRes)
	{
		randomRes = 0;
		if (fukuangyunshiRes_.size() >= FUKUANGYUANSHI_TIMES)
			return -1;

		randomRes = commom_sys.randomBetween(1, 100);
		fukuangyunshiRes_.push_back(randomRes);
		if (fukuangyunshiRes_.size() >= FUKUANGYUANSHI_TIMES)
		{
			int totalSilver = 0;
			for (unsigned i = 0; i < fukuangyunshiRes_.size(); i++)
				totalSilver += fukuangyunshiRes_[i];

			own.Base.alterSilver(totalSilver);
			fukuangyunshiRes_.clear();
		}
		return 0;
	}

	void playerInspectTour::addEvent(int eventID)
	{
		tourEventList_.push_back(eventID);
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerInspectTour::removeEvent(int eventID)
	{
		vector<int>::iterator it = find(tourEventList_.begin(), tourEventList_.end(), eventID);
		if (it != tourEventList_.end())
		{
			tourEventList_.erase(it);
			helper_mgr.insertSaveAndUpdate(this);
		}
	}

}
