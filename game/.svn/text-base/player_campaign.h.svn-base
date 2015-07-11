#pragma once

#include "block.h"
#include "playerCD.h"

namespace gg
{
	const static string playerCampaignDBStr = "gl.player_campaign";
	class TTManager;
	class playerCampaign : public Block
	{
		friend class TTManager;
	public:
		playerCampaign(playerData& own);
		virtual void update();
		virtual void autoUpdate();
		virtual bool get();
		virtual bool save();
		playerCD& getCD();
		inline bool getHelpTick()
		{
			return helpTick;
		}
		void setHelpTick(const bool ret);
		void alterBuyToday(const int num);
		int getBuyToday();
		int getJunling();
		void alterJunling(const int nums);
	private:
		void refresh();
		playerCD CD;
		int junling;
		int buyToday;
		unsigned lastUpdateTime;
		bool helpTick;
	};
}