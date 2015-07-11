#include "player_world_boss.h"
#include "world_boss_system.h"
#include "helper.h"
#include "chat_system.h"

namespace gg
{
	using namespace world_boss_def;
	myReportItem::myReportItem()
	{
		damage_ = 0;
		reportLink_ = 0;
	}

	playerWorldBoss::playerWorldBoss(playerData& own) : Block(own)
	{
		totalDamage_ = 0;
		battleCD_ = 0;
		clearAttackCDTimes_ = 0;
		normalInspireLayer_ = 0;
		guildInspireLayer_ = 0;
		curReportLink_ = 0;
		latestBossID_ = -1;
		latestBossBeginTime_ = 0;
		hasSeekBossHelp_ = false;
		storyCapacityTime_ = 0;
		nextStorageCapacityUpdateTime_ = 0;
		attackTimes_ = 0;
	}

	void playerWorldBoss::reset()
	{
		totalDamage_ = 0;
		battleCD_ = 0;
		clearAttackCDTimes_ = 0;
		normalInspireLayer_ = 0;
		guildInspireLayer_ = 0;
		curReportLink_ = 0;
		hasSeekBossHelp_ = false;
		attackTimes_ = 0;

		damageReward_ = bossRewardItem();
		myReportList_.clear();
	}

	bool playerWorldBoss::getHasSeekBossHelp()
	{
		refreshData();
		return hasSeekBossHelp_;
	}

	void playerWorldBoss::setHasSeekBossHelp(bool bVal)
	{
		hasSeekBossHelp_ = bVal;
		helper_mgr.insertSave(this);
	}

	void playerWorldBoss::refreshData()
	{
		bossDynamicData bossData = world_boss_sys.getLatestBoss();
		if (bossData.beginTime_ != latestBossBeginTime_)
		{
			latestBossBeginTime_ = bossData.beginTime_;
			reset();
			helper_mgr.insertSave(this);
		}
		int curTime = int(na::time_helper::get_current_time());
		if (curTime > nextStorageCapacityUpdateTime_)
		{
			storyCapacityTime_ = 0;
			nextStorageCapacityUpdateTime_ = na::time_helper::get_next_update_time(na::time_helper::get_current_time(), 12);
			helper_mgr.insertSave(this);
		}
		if (!world_boss_sys.isBossActValid()
			&& (normalInspireLayer_ != 0 || guildInspireLayer_ != 0 || attackTimes_ != 0))
		{
			normalInspireLayer_ = 0;
			guildInspireLayer_ = 0;
			attackTimes_ = 0;
			helper_mgr.insertSave(this);
		}
	}

	int playerWorldBoss::getBeHelpBossTimes()
	{
		refreshData();
		return guildInspireLayer_;
	}

	void playerWorldBoss::alterBeHelpBossTimes(int num)
	{
		refreshData();
		guildInspireLayer_ += num;
		if (guildInspireLayer_ < 0)
			guildInspireLayer_ = 0;
		helper_mgr.insertSave(this);
	}

	void gg::playerWorldBoss::autoUpdate()
	{
	}

	bool gg::playerWorldBoss::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(strDbPlayerWorldBoss, key);

		if (!obj.isEmpty())
		{
			checkNotEoo(obj[strTotalDamage])
				totalDamage_ = obj[strTotalDamage].Int();
			checkNotEoo(obj[strDamageReward])
				damageReward_.fromBson(obj[strDamageReward]);
			checkNotEoo(obj[strBattleCD])
				battleCD_ = obj[strBattleCD].Int();
			checkNotEoo(obj[strClearAttackCDTimes])
				clearAttackCDTimes_ = obj[strClearAttackCDTimes].Int();
			checkNotEoo(obj[strNormalInspire])
				normalInspireLayer_ = obj[strNormalInspire].Int();
			checkNotEoo(obj[strGuildInspire])
				guildInspireLayer_ = obj[strGuildInspire].Int();
			checkNotEoo(obj[strCurrentReport])
				curReportLink_ = obj[strCurrentReport].Int();
			checkNotEoo(obj[strLatestBossID])
				latestBossID_ = obj[strLatestBossID].Int();
			checkNotEoo(obj[strLatestBossBeginTime])
				latestBossBeginTime_ = obj[strLatestBossBeginTime].Int();
			checkNotEoo(obj[strHasSeekBossHelp])
				hasSeekBossHelp_ = obj[strHasSeekBossHelp].Bool();
			checkNotEoo(obj[strStoryCapacityTime])
				storyCapacityTime_ = obj[strStoryCapacityTime].Int();
			checkNotEoo(obj[strnextStorageCapacityUpdateTime])
				nextStorageCapacityUpdateTime_ = obj[strnextStorageCapacityUpdateTime].Int();

			checkNotEoo(obj[strMyReportList])
			{
				myReportList_.clear();
				for (unsigned i = 0; i < obj[strMyReportList].Array().size(); i++)
				{
					myReportItem mItem;
					mongo::BSONElement ele = obj[strMyReportList].Array()[i];
					mItem.damage_ = ele[strDamage].Int();
					mItem.reportLink_ = ele[strReport].Int();
					myReportList_.push_back(mItem);
				}
			}
			return true;
		}

		return false;
	}

	bool gg::playerWorldBoss::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;

		obj << playerIDStr << own.playerID;

		obj << strTotalDamage << totalDamage_;
		obj << strDamageReward << damageReward_.toBsonArr();
		obj << strBattleCD << battleCD_;
		obj << strClearAttackCDTimes << clearAttackCDTimes_;
		obj << strNormalInspire << normalInspireLayer_;
		obj << strGuildInspire << guildInspireLayer_;
		obj << strCurrentReport << curReportLink_;
		obj << strLatestBossID << latestBossID_;
		obj << strLatestBossBeginTime << latestBossBeginTime_;
		obj << strHasSeekBossHelp << hasSeekBossHelp_;
		obj << strStoryCapacityTime << storyCapacityTime_;
		obj << strnextStorageCapacityUpdateTime << nextStorageCapacityUpdateTime_;

		mongo::BSONArrayBuilder barr;
		for (unsigned i = 0; i < myReportList_.size(); i++)
		{
			mongo::BSONObjBuilder bobj;
			myReportItem mItem = myReportList_[i];
			bobj << strDamage << mItem.damage_;
			bobj << strReport << mItem.reportLink_;
			barr << bobj.obj();
		}
		obj << strMyReportList << barr.arr();

		return db_mgr.save_mongo(strDbPlayerWorldBoss, key, obj.obj());
	}

	void gg::playerWorldBoss::package(Json::Value& pack)
	{
	}

	bossRewardItem::bossRewardItem()
	{
		rewardSilver_ = 0;
		rewardCrystal_ = 0;
	}

	Json::Value bossRewardItem::toJson()
	{
		Json::Value retJson;
		retJson.append(rewardSilver_);
		retJson.append(rewardCrystal_);
		return retJson;
	}	

	void bossRewardItem::fromBson(mongo::BSONElement ele)
	{
		if (ele.Array().size() != 2)
			return;
		rewardSilver_ = ele.Array()[0u].Int();
		rewardCrystal_ = ele.Array()[1u].Int();
	}

	mongo::BSONArray bossRewardItem::toBsonArr()
	{
		mongo::BSONArrayBuilder barr;
		barr << rewardSilver_;
		barr << rewardCrystal_;
		return barr.arr();
	}
}


