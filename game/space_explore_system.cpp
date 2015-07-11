#include "space_explore_system.h"
#include "response_def.h"
#include "player_data.h"
#include "playerManager.h"
#include "activity_game_param.h"
#include "mongoDB.h"
#include "playerManager.h"

namespace gg
{
#define TREASURE_HUNT_OPEN_LEVEL 30
#define FIRST_TEN_EXPLORE_FALL_EQUIP 14031
	space_explore_system* const space_explore_system::spaceExploreSys = new space_explore_system();

#define AsyncRecordUpdate(MSGJSON) \
	bool isfind = false;\
	for (unsigned i = 0; i < recordList_.size(); i++)\
		{\
		spaceRecord sr = recordList_[i];\
		playerDataPtr player = player_mgr.getPlayerMain(sr.playerID_);\
		if (player == playerDataPtr())\
			isfind = true;\
		}\
	if (isfind)\
		{\
		if (m._post_times < 2)\
				{\
			player_mgr.postMessage(m);\
			return;\
				}\
				else\
			{\
			Return(r, -1);\
			}\
		}\

	space_explore_system::space_explore_system()
	{

	}

	space_explore_system::~space_explore_system()
	{

	}

	void space_explore_system::initData()
	{
		db_mgr.ensure_index(space_explore_def::strDbPlayerSpaceExplore, BSON(playerIDStr << 1));
		db_mgr.ensure_index(space_explore_def::strDbSpaceExplore, BSON(space_explore_def::strKey << 1));
		Json::Value jconfig = na::file_system::load_jsonfile_val(space_explore_def::strFileConfig);

		config_.silverBase_ = jconfig["silverBase"].asInt();
		config_.oneHuntGoldCost_ = jconfig["oneHuntGoldCost"].asInt();
		config_.tenHuntsGoldCost_ = jconfig["tenHuntGoldCost"].asInt();
		config_.twoTimesCritical_ = int((jconfig["twoTimesCritical"].asDouble() + PRECISION)*RANGE);
		config_.fiveTimesCritical_ = int((jconfig["fiveTimesCirtical"].asDouble() + PRECISION)*RANGE);
		config_.silverProbability_ = int((jconfig["silverProbability"].asDouble() + PRECISION)*RANGE);

		for (unsigned i = 0; i < jconfig["equipIDList"].size(); i++)
		{
			config_.equipIDList_.push_back(jconfig["equipIDList"][i].asInt());
		}
		for (unsigned i = 0; i < jconfig["equipProbability"].size(); i++)
		{
			config_.equipProbability_.push_back(int((jconfig["equipProbability"][i].asDouble() + PRECISION)*RANGE));
		}
		if (config_.equipIDList_.size() != config_.equipProbability_.size())
		{
			cout << "equipID size not match probability size!!" << endl;
			throw exception();
		}

		int sumProb = 0;
		sumProb += config_.silverProbability_;
		for (unsigned i = 0; i < config_.equipProbability_.size(); i++)
		{
			sumProb += config_.equipProbability_[i];
		}
		if (sumProb != RANGE)
		{
			cout << "sum of probability  is not 1 !!" << endl;
			throw exception();
		}

		for (unsigned i = 0; i < config_.equipIDList_.size(); i++)
		{
			if (config_.equipIDList_[i] == FIRST_TEN_EXPLORE_FALL_EQUIP)
			{
				config_.firstTenExploreEquipIndex_ = i;
				break;
			}
		}
		if (config_.firstTenExploreEquipIndex_ < 0)
		{
			cout << "there must be equip " << FIRST_TEN_EXPLORE_FALL_EQUIP << " !!" << endl;
			throw exception();
		}

		config_.equipProbability_[0] += config_.silverProbability_;
		for (unsigned i = 1; i < config_.equipProbability_.size(); i++)
		{
			config_.equipProbability_[i] += config_.equipProbability_[i-1];
		}

		config_.isSystemOpen_ = (act_game_param_mgr.get_game_param_by_id(param_other_space_explore_open) == 0);

		get();
	}

	void space_explore_system::spaceExplorePlayerInfoReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		if (!config_.isSystemOpen_)
			Return(r, -1);

		if (d->Base.getLevel() < TREASURE_HUNT_OPEN_LEVEL)
			Return(r, -1);

		d->SpaceExplore.autoUpdate();
	}

	void space_explore_system::spaceExploreOneHuntReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		AsyncRecordUpdate(m);

		if (!config_.isSystemOpen_)
			Return(r, -1);

		if (d->Base.getLevel() < TREASURE_HUNT_OPEN_LEVEL)
			Return(r, -1);

		rewardResult rr;
		Json::Value failSimpleJson;
		int ret = d->SpaceExplore.exploreOne(rr, failSimpleJson);
		Json::Value jrest;
		jrest.append(rr.rewardIndex_);
		jrest.append(rr.rewardTimes_);
		r[msgStr][1u] = jrest;
		r[msgStr][2u] = failSimpleJson;

		Return(r, ret);
	}

	void space_explore_system::spaceExploreTenHuntReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		AsyncRecordUpdate(m);

		if (!config_.isSystemOpen_)
			Return(r, -1);

		if (d->Base.getLevel() < TREASURE_HUNT_OPEN_LEVEL)
			Return(r, -1);

		vector<rewardResult> rlist;
		Json::Value failSimpleJson;
		int ret = d->SpaceExplore.exploreTen(rlist, failSimpleJson);

		Json::Value jrest = Json::arrayValue;
		for (unsigned i = 0; i < rlist.size(); i++)
		{
			Json::Value ritem;
			ritem.append(rlist[i].rewardIndex_);
			ritem.append(rlist[i].rewardTimes_);
			jrest.append(ritem);
		}
		r[msgStr][1u] = jrest;
		r[msgStr][2u] = failSimpleJson;

		Return(r, ret);
	}

	void space_explore_system::spaceExploreUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		AsyncRecordUpdate(m);
		updateRecord(d);

// 		int exploreTimes = 100*10000;
// 		vector<int> vecCount;
// 		vecCount.resize(config_.equipIDList_.size() + 1);
// 		for (unsigned i = 0; i < vecCount.size(); i++)
// 		{
// 			vecCount[i] = 0;
// 		}
// 		for (int i = 0; i < exploreTimes; i++)
// 		{
// 			rewardResult rr;
// 			d->SpaceExplore.exploreOne(rr);
// 			if (rr.rewardIndex_ >= 11)
// 			{
// 				cout << "error program" << endl;
// 			}
// 			if (i % 5000 == 0)
// 			{
// 				cout << "the " << i << " times"<< endl;
// 			}
// 			vecCount[rr.rewardIndex_]++;
// 		}
// 		for (unsigned i = 0; i < vecCount.size(); i++)
// 		{
// 			cout << i << "-" << vecCount[i] << endl;
// 		}
// 		cout << endl;
	}

	void space_explore_system::setSystemState()
	{
		config_.isSystemOpen_ = (act_game_param_mgr.get_game_param_by_id(param_other_space_explore_open) == 0);
	}

	void space_explore_system::insertRecord(spaceRecord sr)
	{
		recordList_.insert(recordList_.begin(), sr);
		if (recordList_.size() > 30)
		{
			recordList_.pop_back();
		}
	}

	bool space_explore_system::save()
	{
		mongo::BSONObj key = BSON(space_explore_def::strKey << space_explore_def::strKey);
		mongo::BSONObjBuilder obj;

		obj << space_explore_def::strKey << space_explore_def::strKey;

		mongo::BSONArrayBuilder arrRecord;
		for (unsigned i = 0; i < recordList_.size(); i++)
		{
			spaceRecord sr = recordList_[i];
			mongo::BSONArrayBuilder arrSingle;
			arrSingle << sr.playerID_;
			arrSingle << sr.reward_.rewardIndex_;
			arrSingle << sr.reward_.rewardTimes_;
			arrRecord << arrSingle.arr();
		}
		obj << space_explore_def::strRecordList << arrRecord.arr();

		return db_mgr.save_mongo(space_explore_def::strDbSpaceExplore, key, obj.obj());
	}

	bool space_explore_system::get()
	{
		mongo::BSONObj key = BSON(space_explore_def::strKey << space_explore_def::strKey);
		mongo::BSONObj obj = db_mgr.FindOne(space_explore_def::strDbSpaceExplore, key);

		if (!obj.isEmpty())
		{
			checkNotEoo(obj[space_explore_def::strRecordList])
			{
				recordList_.clear();
				vector<mongo::BSONElement> sets = obj[space_explore_def::strRecordList].Array();
				for (unsigned i = 0; i < sets.size(); i++)
				{
					vector<mongo::BSONElement> singleSet = sets[i].Array();
					spaceRecord sr;
					sr.playerID_ = singleSet[0].Int();
					sr.reward_.rewardIndex_ = singleSet[1].Int();
					sr.reward_.rewardTimes_ = singleSet[2].Int();
					recordList_.push_back(sr);
				}
				return true;
			}

			return false;
		}

		return false;
	}

	void space_explore_system::updateRecord(playerDataPtr pdata)
	{
		Json::Value recordJson = Json::arrayValue;
		for (unsigned i = 0; i < recordList_.size(); i++)
		{
			spaceRecord sr = recordList_[i];
			Json::Value singleRecord;
			singleRecord.append(sr.playerID_);
			playerDataPtr player = player_mgr.getPlayerMain(sr.playerID_);
			string playerName = "";
			if (player != playerDataPtr())
			{
				playerName = player->Base.getName();
			}
			else
			{
				LogE << __FUNCTION__ << ", cannot find name of " << sr.playerID_ << LogEnd;
			}
			singleRecord.append(playerName);
			singleRecord.append(sr.reward_.rewardIndex_);
			singleRecord.append(sr.reward_.rewardTimes_);
			recordJson.append(singleRecord);
		}

		Json::Value r;
		r[msgStr][0u] = 0;
		r[msgStr][1] = recordJson;

// 		string s = r.toStyledString();
// 		s = commom_sys.tighten(s);
//		na::msg::msg_json mj(gate_client::space_explore_update_resp, s);
//		cout << __FUNCTION__ << ",s:" << s << endl;
		player_mgr.sendToPlayer(pdata->playerID, pdata->netID, gate_client::space_explore_update_resp, r);
	}

	rewardResult::rewardResult()
	{
		rewardIndex_ = -1;
		rewardTimes_ = 1;
	}
	
	spaceExploreConfig::spaceExploreConfig()
	{
		silverBase_ = 0;
		oneHuntGoldCost_ = 0;
		tenHuntsGoldCost_ = 0;
		twoTimesCritical_ = 0;
		fiveTimesCritical_ = 0;
		silverProbability_ = 0;
		firstTenExploreEquipIndex_ = -1;
		isSystemOpen_ = true;
	}
}
