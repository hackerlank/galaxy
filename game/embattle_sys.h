#pragma once

#include <boost/unordered_map.hpp>
#include <vector>
#include "pilotManager.h"
#include "msg_base.h"

using namespace na::msg;
using namespace std;

#define embattle_sys (*gg::embattle_system::embattleSys)

namespace gg
{
	struct embattleHole
	{
		int holeIndex;
		int holeOpen;//-1 没有前提条件 >0 对应mapID
		int levelLimit;
	};

	struct embattleAddAttri
	{
		int idxWhat;
		double addBase;
	};

	static const int beginEMID = 1;

	struct embattleData
	{
		embattleData(){
			emID = -1;
			levelLimit = 0;
			holeConfig.clear();
			embattleAdd.clear();
		}

		bool operator !=(const embattleData& emb) const{
			return emID != emb.emID;
		}

		bool operator ==(const embattleData& emb) const{
			return emID == emb.emID;
		}

		bool canUseHole(const int hole, playerDataPtr player)const;

		int emID;//阵型id
		int levelLimit;
		vector<embattleHole> holeConfig;
		vector<embattleAddAttri> embattleAdd;
	};

	const static embattleData NullEmbattleData;

	class embattle_system
	{
	public:
		static embattle_system* const embattleSys;
		void initData();
		const embattleData& getEMbattle(const int rawID);
		void activeEMbattle(playerDataPtr player);
		void checkNullEMbattle(playerDataPtr player);

		void setCurrentFormat(msg_json& m, Json::Value& r);
		void embattleUpdate(msg_json& m, Json::Value& r);
		void embattleInFormat(msg_json& m, Json::Value& r);
	private:
		typedef boost::unordered_map<int, embattleData> embattleMap;
		embattleMap map_;
	};
}