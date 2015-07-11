#pragma once
#include "block.h"

namespace gg
{
	namespace inspect_tour_def
	{
		const static string strPlayerInspectTour = "gl.player_inspect_tour";
		const static string strTourEventList = "ls";
	}
	class playerInspectTour : public Block
	{
	public:
		playerInspectTour(playerData& own);
		bool save();
		bool get();
		void autoUpdate();
		void package(Json::Value& pack);
		bool canOperate(int eventID);
		int operateFukuangyunshi(int &randomRes);
		void addEvent(int eventID);
		void removeEvent(int eventID);
	private:
		vector<int> tourEventList_;
		vector<int> fukuangyunshiRes_;
	};
}
