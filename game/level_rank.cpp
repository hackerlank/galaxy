#include "level_rank.h"
#include "mongoDB.h"
#include "player_base.h"
#include "Glog.h"
#include "season_system.h"

namespace gg
{
	levelRank* const levelRank::LevelRankSys = new levelRank();

	levelRank::levelRank()
	{
		RankList.clear();
		tickTime = 0;
		lastLevel = 0;
	}

	int levelRank::getLastLevel()
	{
		unsigned now = na::time_helper::get_current_time();
		if (now > tickTime)
		{
			do
			{
				tickTime += na::time_helper::ONEDAY;
			} while (now > tickTime);
			if (!RankList.empty())lastLevel = RankList.back().level;
			save();
		}
		return lastLevel;
	}

	int levelRank::serverLevel()
	{
		int days = season_sys.getServerOpenDay();
		if (days <= 1)return 0;
		int lastLevel = levelRank_sys.getLastLevel();
		int limit = lastLevel - 30;
		return limit < 10 ? 10 : limit;
	}

	const static string strDBLevelRank = "gl.player_level_rank";
	void levelRank::initData()
	{
		mongo::BSONObj obj = db_mgr.FindOne(strDBLevelRank, BSON("key" << "key"));
		if (obj.isEmpty())
		{
			LogS << "init player level rank ..." << LogEnd;
			objCollection objs = db_mgr.Query(playerBaseDBStr);
			vector<Rank> vRank;
			for (unsigned i = 0; i < objs.size(); i++)
			{
				mongo::BSONObj& obj = objs[i];
				vRank.push_back(Rank(obj[playerIDStr].Int(), obj[playerLevelStr].Int()));
			}
			std::sort(vRank.begin(), vRank.end(), RankMethod);
			if (vRank.size() > 50)vRank.erase(vRank.begin() + 50, vRank.end());
			RankList = vRank;
			tickTime = season_sys.getNextSeasonTime();
			if (!RankList.empty())
			{
				lastLevel = RankList.back().level;
			}
			save();
		}
		else
		{
			LogS << "load player db data ..." << LogEnd;
			tickTime = (unsigned)obj["tt"].Int();
			lastLevel = obj["ll"].Int();
			RankList.clear();
			vector<mongo::BSONElement> elems = obj["rk"].Array();
			for (unsigned i = 0; i < elems.size(); ++i)
			{
				mongo::BSONElement& elem = elems[i];
				RankList.push_back(Rank(elem[playerIDStr].Int(), elem[playerLevelStr].Int()));
			}
		}
	}

	void levelRank::insertData(const int playerID, const int level)
	{
		insertData(Rank(playerID, level));
	}

	void levelRank::insertData(Rank rank)
	{
		unsigned now = na::time_helper::get_current_time();
		if (now > tickTime)
		{
			do 
			{
				tickTime += na::time_helper::ONEDAY;
			} while (now > tickTime);
			if (!RankList.empty())lastLevel = RankList.back().level;
			save();
		}

		if (RankList.size() >= 50 && rank.level < RankList.back().level)
		{
			return;
		}

		for (unsigned i = 0; i < RankList.size(); ++i)
		{
			if (RankList[i].playerID == rank.playerID)
			{
				RankList.erase(RankList.begin() + i);
			}
		}

		RankList.push_back(rank);
		std::sort(RankList.begin(), RankList.end(), RankMethod);

		if (RankList.size() > 50)RankList.pop_back();
		save();
	}

	void levelRank::save()
	{
		mongo::BSONObj key = BSON("key" << "key");
		mongo::BSONObjBuilder builder;
		builder << "key" << "key" << "tt" << tickTime << "ll" << lastLevel;
		mongo::BSONArrayBuilder arrBuilder;
		for (unsigned i = 0; i < RankList.size(); ++i)
		{
			const Rank& cR = RankList[i];
			arrBuilder << BSON(playerIDStr << cR.playerID << playerLevelStr << cR.level);
		}
		builder << "rk" << arrBuilder.arr();
		db_mgr.save_mongo(strDBLevelRank, key, builder.obj());
	}
}
