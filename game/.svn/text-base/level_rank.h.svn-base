#pragma once

#include <iostream>
#include <vector>
using namespace std;

#define levelRank_sys (*gg::levelRank::LevelRankSys)

namespace gg
{
	class levelRank
	{
		struct Rank
		{
			Rank(const int id, const int lv)
			{
				playerID = id;
				level = lv;
			}
			int playerID;
			int level;
		};
	public:
		levelRank();
		~levelRank(){}
		static levelRank* const LevelRankSys;
		int getLastLevel();
		int serverLevel();
		void initData();
		void insertData(const int playerID, const int level);
		void insertData(Rank rank);
	private:
		static bool RankMethod(const Rank& left, const Rank& right)
		{
			if (left.level != right.level)return left.level > right.level;
			return left.playerID < right.playerID;
		}
		void save();
		vector<Rank> RankList;
		unsigned tickTime;
		int lastLevel;
	};

}