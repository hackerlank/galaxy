#pragma once

#define kingdom_sys (*gg::kingdom_system::kingdomSys)

namespace gg
{
	class kingdom_system
	{
	public:
		static kingdom_system* const kingdomSys;
		void initData();
		int getMinKingdom();
		void untickKingdom(const int kingdomID);
		void tickKingdom(const int kingdomID);
	private:
		void save();
		int kM[3];
	};
}