#include "kingdom_system.h"
#include <iostream>
#include "mongoDB.h"
#include "playerManager.h"
using namespace std;

namespace gg
{
	kingdom_system* const kingdom_system::kingdomSys = new kingdom_system();

	const static string kingdomKey = "kdk";
	const static string kingdomDBStr = "gl.kingdom_system";

	void kingdom_system::initData()
	{
		db_mgr.ensure_index(kingdomDBStr, BSON(kingdomKey << 1));
		memset(kM, 0x0, sizeof(kM));
		mongo::BSONObj obj = db_mgr.FindOne(kingdomDBStr , BSON(kingdomKey << kingdomKey));
		if(!obj.isEmpty())
		{
			checkNotEoo(obj["0"]){kM[0] = obj["0"].Int();}
			checkNotEoo(obj["1"]){kM[1] = obj["1"].Int();}
			checkNotEoo(obj["2"]){kM[2] = obj["2"].Int();}
		}
	}

	void kingdom_system::untickKingdom(const int kingdomID)
	{
		if(kingdomID < 0 || kingdomID > 2)return;
		--kM[kingdomID];
		kM[kingdomID] = kM[kingdomID] < 0 ? 0 : kM[kingdomID];
		save();
	}

	void kingdom_system::tickKingdom(const int kingdomID)
	{
		if(kingdomID < 0 || kingdomID > 2)return;
		++kM[kingdomID];
		save();
	}

	int kingdom_system::getMinKingdom()
	{
		int m = 0;
		for (int i = 1; i < 3; ++i)
		{
			if(kM[i] < kM[m])m = i;
		}
		return m;
	}

	void kingdom_system::save()
	{
		mongo::BSONObj obj = BSON(kingdomKey << kingdomKey << "0" << 
			kM[0] << "1" <<  kM[1] << "2" << kM[2]);
		mongo::BSONObj key = BSON(kingdomKey << kingdomKey);
		db_mgr.save_mongo(kingdomDBStr, key, obj);
	}
}