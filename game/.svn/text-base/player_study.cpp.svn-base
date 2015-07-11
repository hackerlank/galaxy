#include "player_study.h"
#include "study_system.h"
#include "playerManager.h"
#include "helper.h"
#include "task_system.h"
#include "email_system.h"
#include "action_def.h"
#include "record_system.h"
#include "item_system.h"
#include "season_system.h"

using namespace mongo;

namespace gg
{
	delegateStudy::delegateStudy()
	{
		drawingId = 0;
		totalDelegateTimes = 0;
		todayDelegateTimes = 0;
		todayHasPopupUI = false;
		nextDelegateDailyFreshTime = 0;
	}

	void delegateStudy::maintainDailyData()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > nextDelegateDailyFreshTime)
		{
			todayDelegateTimes = 0;
			todayHasPopupUI = false;

			boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
			tm t = boost::posix_time::to_tm(p);
			unsigned standard_time = cur_time - 3600*t.tm_hour - 60*t.tm_min - t.tm_sec;
			if (t.tm_hour >= 5)
			{
				nextDelegateDailyFreshTime = unsigned(na::time_helper::get_next_time(standard_time, 1, 5));
			}
			else
			{
				nextDelegateDailyFreshTime = unsigned(na::time_helper::get_next_time(standard_time, 0, 5));
			}
		}
	}

	playerStudy::playerStudy(playerData& own) : Block(own)
	{
		deleStu[ARTIL].delegateSet[0] = 1;
		deleStu[SHIELD].delegateSet[0] = 1;
		for (unsigned i = 1; i < 4; i++)
		{
			deleStu[ARTIL].delegateSet[i] = 0;
			deleStu[SHIELD].delegateSet[i] = 0;
		}
		playerCD cd(-1, 60*60);
		deleStu[ARTIL].delegateCD = cd;
		deleStu[SHIELD].delegateCD = cd;

		memset(deleStu[ARTIL].convertDrawing, 0, sizeof(deleStu[ARTIL].convertDrawing));
		memset(deleStu[SHIELD].convertDrawing, 0, sizeof(deleStu[SHIELD].convertDrawing));
	}

	void playerStudy::autoUpdate()
	{
		Json::Value msg;
		package(msg);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::study_update_resp, msg);
	}

	bool playerStudy::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(study::studyDBStr, key);

		if(!obj.isEmpty())
		{
			//主炮研究
			checkNotEoo(obj[study::strArtillery])
			{
				mongo::BSONObj obj_temp = obj[study::strArtillery].Obj();
				checkNotEoo(obj_temp[study::strDrawingId])
					deleStu[ARTIL].drawingId = obj_temp[study::strDrawingId].Int();
				checkNotEoo(obj_temp[study::strTotalDelegateTimes])
					deleStu[ARTIL].totalDelegateTimes = obj_temp[study::strTotalDelegateTimes].Int();
				checkNotEoo(obj_temp[study::strDelegateCDLock])
					deleStu[ARTIL].delegateCD.Cdd.Lock = obj_temp[study::strDelegateCDLock].boolean();
				checkNotEoo(obj_temp[study::strTodayDelegateTimes])
					deleStu[ARTIL].todayDelegateTimes = obj_temp[study::strTodayDelegateTimes].Int();
				checkNotEoo(obj_temp[study::strNextDelegateDailyFreshTime])
					deleStu[ARTIL].nextDelegateDailyFreshTime = obj_temp[study::strNextDelegateDailyFreshTime].Int();
				checkNotEoo(obj_temp[study::strHasPopupUI])
					deleStu[ARTIL].todayHasPopupUI = obj_temp[study::strHasPopupUI].Bool();

				checkNotEoo(obj_temp[study::strDelegateCD])
				{
					deleStu[ARTIL].delegateCD.Cdd.CD = obj_temp[study::strDelegateCD].Int();				
					CDData& cd = deleStu[ARTIL].delegateCD.getCD();
				}

				checkNotEoo(obj_temp[study::strDelegateSet]) 
				{
					vector<BSONElement> sets = obj_temp[study::strDelegateSet].Array();
					for (unsigned i = 0; i < sets.size(); ++i)
					{
						BSONElement dset = sets[i];
						deleStu[ARTIL].delegateSet[i] = dset.Int();
					}
				}

				checkNotEoo(obj_temp[study::strConvertDrawing])
				{
					vector<BSONElement> cvts = obj_temp[study::strConvertDrawing].Array();
					for (unsigned i = 0; i < cvts.size(); ++i)
					{
						BSONElement dcvt = cvts[i];
						deleStu[ARTIL].convertDrawing[i] = dcvt.Int();
					}
				}
			}

			//护盾研究
			checkNotEoo(obj[study::strShield])
			{
				mongo::BSONObj obj_temp = obj[study::strShield].Obj();
				checkNotEoo(obj_temp[study::strDrawingId])
					deleStu[SHIELD].drawingId = obj_temp[study::strDrawingId].Int();
				checkNotEoo(obj_temp[study::strTotalDelegateTimes])
					deleStu[SHIELD].totalDelegateTimes = obj_temp[study::strTotalDelegateTimes].Int();
				checkNotEoo(obj_temp[study::strDelegateCDLock])
					deleStu[SHIELD].delegateCD.Cdd.Lock = obj_temp[study::strDelegateCDLock].boolean();
				checkNotEoo(obj_temp[study::strTodayDelegateTimes])
					deleStu[SHIELD].todayDelegateTimes = obj_temp[study::strTodayDelegateTimes].Int();
				checkNotEoo(obj_temp[study::strNextDelegateDailyFreshTime])
					deleStu[SHIELD].nextDelegateDailyFreshTime = obj_temp[study::strNextDelegateDailyFreshTime].Int();
				checkNotEoo(obj_temp[study::strHasPopupUI])
					deleStu[SHIELD].todayHasPopupUI = obj_temp[study::strHasPopupUI].Bool();

				checkNotEoo(obj_temp[study::strDelegateCD])
				{
					deleStu[SHIELD].delegateCD.Cdd.CD = obj_temp[study::strDelegateCD].Int();				
					CDData& cd = deleStu[SHIELD].delegateCD.getCD();
				}

				checkNotEoo(obj_temp[study::strDelegateSet]) 
				{
					vector<BSONElement> sets = obj_temp[study::strDelegateSet].Array();
					for (unsigned i = 0; i < sets.size(); ++i)
					{
						BSONElement dset = sets[i];
						deleStu[SHIELD].delegateSet[i] = dset.Int();
					}
				}

				checkNotEoo(obj_temp[study::strConvertDrawing])
				{
					vector<BSONElement> cvts = obj_temp[study::strConvertDrawing].Array();
					for (unsigned i = 0; i < cvts.size(); ++i)
					{
						BSONElement dcvt = cvts[i];
						deleStu[SHIELD].convertDrawing[i] = dcvt.Int();
					}
				}

				deleStu[ARTIL].maintainDailyData();
				deleStu[SHIELD].maintainDailyData();
			}

			//灵能研究
			checkNotEoo(obj[study::strPsionic])
			{
				mongo::BSONObj obj_temp = obj[study::strPsionic].Obj();
				checkNotEoo(obj_temp[study::strCurrentMaxStep]) psionicStu.currentMaxStep = obj_temp[study::strCurrentMaxStep].Int();
				checkNotEoo(obj_temp[study::strCurrentStep]) psionicStu.currentStep = obj_temp[study::strCurrentStep].Int();
//				checkNotEoo(obj_temp[study::strPsionicCD]) psionicStu.psionicCD = obj_temp[study::strPsionicCD].Int();
				checkNotEoo(obj_temp[study::strStudyStatus]) psionicStu.studyStatus = obj_temp[study::strStudyStatus].Int();
				checkNotEoo(obj_temp[study::strStudyEvent]) psionicStu.studyEvent = obj_temp[study::strStudyEvent].Int();
				checkNotEoo(obj_temp[study::strStudyTodayFinishTimes]) psionicStu.studyTimes = obj_temp[study::strStudyTodayFinishTimes].Int();
				checkNotEoo(obj_temp[study::strStudyTotalFinishTimes]) psionicStu.totalStudyTimes = obj_temp[study::strStudyTotalFinishTimes].Int();
				checkNotEoo(obj_temp[study::strNextDailyFreshTime]) psionicStu.nextDailyFreshTime = obj_temp[study::strNextDailyFreshTime].Int();
				checkNotEoo(obj_temp[study::strNextWeeklyFreshTime]) psionicStu.nextWeeklyFreshTime = obj_temp[study::strNextWeeklyFreshTime].Int();
				checkNotEoo(obj_temp[study::strNextSeasonFreshTime]) psionicStu.nextSeasonFreshTime = obj_temp[study::strNextSeasonFreshTime].Int();
				psionicStu.maintainDailyData();
				psionicStu.maintainSeasonData();
			}

			return true;
		}
		return false;
	}

	bool playerStudy::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj, objArtDs, objArtCvd, objShieldDs, objShieldCvd, objPsionic;
		mongo::BSONArrayBuilder arrArtDs, arrArtCvd, arrShield, arrShieldCvd, arrPsionic;

		obj << playerIDStr << own.playerID;

		//主炮研究
		objArtDs << study::strDrawingId << deleStu[ARTIL].drawingId;
		objArtDs << study::strTotalDelegateTimes << deleStu[ARTIL].totalDelegateTimes;
		objArtDs << study::strDelegateCD << deleStu[ARTIL].delegateCD.Cdd.CD;
		objArtDs << study::strDelegateCDLock << deleStu[ARTIL].delegateCD.Cdd.Lock;
		objArtDs << study::strTodayDelegateTimes << deleStu[ARTIL].todayDelegateTimes;
		objArtDs << study::strNextDelegateDailyFreshTime << deleStu[ARTIL].nextDelegateDailyFreshTime;
		objArtDs << study::strHasPopupUI << deleStu[ARTIL].todayHasPopupUI;

		for (unsigned i = 0; i < 4; i++)
		{
			arrArtDs << deleStu[ARTIL].delegateSet[i];
		}
		objArtDs << study::strDelegateSet << arrArtDs.arr();

		for (unsigned i = 0; i < 3; i++)
		{
			arrArtCvd << deleStu[ARTIL].convertDrawing[i];
		}
		objArtDs << study::strConvertDrawing << arrArtCvd.arr();
		obj << study::strArtillery << objArtDs.obj();

		//护盾研究
		objShieldDs << study::strDrawingId << deleStu[SHIELD].drawingId;
		objShieldDs << study::strTotalDelegateTimes << deleStu[SHIELD].totalDelegateTimes;
		objShieldDs << study::strDelegateCD << deleStu[SHIELD].delegateCD.Cdd.CD;
		objShieldDs << study::strDelegateCDLock << deleStu[SHIELD].delegateCD.Cdd.Lock;
		objShieldDs << study::strTodayDelegateTimes << deleStu[SHIELD].todayDelegateTimes;
		objShieldDs << study::strNextDelegateDailyFreshTime << deleStu[SHIELD].nextDelegateDailyFreshTime;
		objShieldDs << study::strHasPopupUI << deleStu[SHIELD].todayHasPopupUI;

		for (unsigned i = 0; i < 4; i++)
		{
			arrShield << deleStu[SHIELD].delegateSet[i];
		}
		objShieldDs << study::strDelegateSet << arrShield.arr();
//		obj << study::strShield << objShieldDs.obj();

		for (unsigned i = 0; i < 3; i++)
		{
			arrShieldCvd << deleStu[SHIELD].convertDrawing[i];
		}
		objShieldDs << study::strConvertDrawing << arrShieldCvd.arr();
		obj << study::strShield << objShieldDs.obj();

		//灵能研究
		objPsionic << study::strCurrentMaxStep << psionicStu.currentMaxStep;
		objPsionic << study::strCurrentStep << psionicStu.currentStep;
//		objPsionic << study::strPsionicCD << psionicStu.psionicCD;
		objPsionic << study::strStudyStatus << psionicStu.studyStatus;
		objPsionic << study::strStudyEvent << psionicStu.studyEvent;
		objPsionic << study::strStudyTodayFinishTimes << psionicStu.studyTimes;
		objPsionic << study::strStudyTotalFinishTimes << psionicStu.totalStudyTimes;
		objPsionic << study::strNextDailyFreshTime << psionicStu.nextDailyFreshTime;
		objPsionic << study::strNextWeeklyFreshTime << psionicStu.nextWeeklyFreshTime;
		objPsionic << study::strNextSeasonFreshTime << psionicStu.nextSeasonFreshTime;
		obj << study::strPsionic << objPsionic.obj();

		return db_mgr.save_mongo(study::studyDBStr, key, obj.obj());
	}

	void playerStudy::package(Json::Value& pack, int packOpt /* = -1 */)
	{

		pack[msgStr][0u] = 0;
		pack[msgStr][1u][updateFromStr] = State::getState();

		//主炮研究
		if (packOpt == -1 || packOpt == 0)
		{
			delegateStudy &artilStu = getDeleStu(ARTIL);
			for (unsigned i = 0; i < 4; i++)
			{
				pack[msgStr][1u][study::strArtillery][study::strDelegateSet][i] = artilStu.delegateSet[i];
			}
//			pack[msgStr][1u][study::strArtillery][study::strDrawingId] = artilStu.drawingId;
			pack[msgStr][1u][study::strArtillery][study::strTodayDelegateTimes] = artilStu.todayDelegateTimes;
			CDData& cd = artilStu.delegateCD.getCD();
			pack[msgStr][1u][study::strArtillery][study::strDelegateCD] = artilStu.delegateCD.Cdd.CD;
			pack[msgStr][1u][study::strArtillery][study::strDelegateCDLock] = artilStu.delegateCD.Cdd.Lock;
			pack[msgStr][1u][study::strArtillery][study::strHasPopupUI] = artilStu.todayHasPopupUI;
			for (unsigned i = 0; i < 3; i++)
			{//从装备系统里面获取图纸的数量，待定
				pack[msgStr][1u][study::strArtillery][study::strConvertDrawing][i] = artilStu.convertDrawing[i];
			}
		}

		//护盾研究
		if (packOpt == -1 || packOpt == 1)
		{
			delegateStudy &shieldStu = getDeleStu(SHIELD);
			for (unsigned i = 0; i < 4; i++)
			{
				pack[msgStr][1u][study::strShield][study::strDelegateSet][i] = shieldStu.delegateSet[i];
			}
//			pack[msgStr][1u][study::strShield][study::strDrawingId] = shieldStu.drawingId;
			pack[msgStr][1u][study::strShield][study::strTodayDelegateTimes] = shieldStu.todayDelegateTimes;
			CDData& cd = shieldStu.delegateCD.getCD();
			pack[msgStr][1u][study::strShield][study::strDelegateCD] = shieldStu.delegateCD.Cdd.CD;
			pack[msgStr][1u][study::strShield][study::strDelegateCDLock] = shieldStu.delegateCD.Cdd.Lock;		
			pack[msgStr][1u][study::strShield][study::strHasPopupUI] = shieldStu.todayHasPopupUI;
			for (unsigned i = 0; i < 3; i++)
			{//从装备系统里面获取图纸的数量，待定
				pack[msgStr][1u][study::strShield][study::strConvertDrawing][i] = shieldStu.convertDrawing[i];
			}
		}

		//灵能研究
		if (packOpt == -1 || packOpt == 2)
		{
			psionicStudy &psStu = getPsionicStu();
			pack[msgStr][1u][study::strPsionic][study::strCurrentMaxStep] = psStu.currentMaxStep;
			pack[msgStr][1u][study::strPsionic][study::strCurrentStep] = psStu.currentStep;
//			pack[msgStr][1u][study::strPsionic][study::strPsionicCD] = psionicStu.psionicCD;
			pack[msgStr][1u][study::strPsionic][study::strStudyEvent] = psStu.studyEvent;
			pack[msgStr][1u][study::strPsionic][study::strStudyStatus] = psStu.studyStatus;
			pack[msgStr][1u][study::strPsionic][study::strStudyTodayFinishTimes] = psStu.studyTimes;
		}
	}

	void playerStudy::player_delegate(int deleType, int deleOpt, int &deleDrawingID, bool &isBagFull, int costSilver)
	{
		isBagFull = false;
		deleDrawingID = 0;
		if (!study_sys._deleMap[TYPEOPT2ID].isPermanent)
		{
			deleStu[deleType].delegateSet[deleOpt] = 0;
		}
		if (study_sys._deleMap[TYPEOPT2ID].canComeMerchantId != -1)
		{
			int random_merchant = commom_sys.randomBetween(0, RANGE-1);
			int config_rate = study_sys._deleMap[TYPEOPT2ID].canComeMerchantRate;
			if (random_merchant < config_rate)
			{
				deleStu[deleType].delegateSet[IDTYPE2OPT(study_sys._deleMap[TYPEOPT2ID].canComeMerchantId)] = 1;
			}
		}

		unsigned rate_size = study_sys._deleMap[TYPEOPT2ID].canGetEquipmentRates.size();
		int eq_rate = commom_sys.randomBetween(0, RANGE-1);
		unsigned chose_idx;
		for (unsigned idx = 0; idx < rate_size; idx++)
		{
			if (eq_rate < study_sys._deleMap[TYPEOPT2ID].canGetEquipmentRates[idx])
			{
				chose_idx = idx;
				break;
			}
		}

		int item_id = study_sys._deleMap[TYPEOPT2ID].canGetEquipmentRawIds[chose_idx];
		if (deleStu[deleType].convertDrawing[2] == 200 && deleOpt == 3)
		{
			item_id = study_sys.drawingMap[deleType][3];
			deleStu[deleType].convertDrawing[2] = -1;
		}

		if (deleType == ARTIL && deleStu[deleType].totalDelegateTimes == 0) //首次委派固定掉落
			item_id = study_sys._deleMap[TYPEOPT2ID].canGetEquipmentRawIds[0];

		deleDrawingID = item_id;
		if (item_id > 0)
		{
			int res = 1;
			playerItemPtr itemPtr = own.Item.addItem(item_id);
			if(itemPtr != playerItemPtr())
			{
				deleStu[deleType].drawingId = itemPtr->getLocalID();
			}
			else
			{
				Json::Value attachJson;
				Json::Value attachItem;
				attachItem[email::actionID] = action_add_item;
				attachItem[email::addNum] = 1;
				attachItem[email::itemID] = item_id;
				attachJson.append(attachItem);

				isBagFull = true;
				email_sys.sendSystemEmailCommon(own.shared_from_this(), email_type_system_attachment, email_team_system_bag_full_study, Json::Value::null, attachJson);
				deleStu[deleType].drawingId = 0;
			}
		}

		{//1.	主炮研究log记录：玩家ID、时间、操作（主炮研究）、获得图纸ID、研究前星币数量、研究后星币数量、研究前信用点数量、研究后信用点数量。
			vector<string> fields;
			int cur_time = na::time_helper::get_current_time();
			fields.push_back(boost::lexical_cast<string, int>(cur_time - na::time_helper::timeZone() * 3600));
			fields.push_back(boost::lexical_cast<string, int>(item_id));
			fields.push_back(boost::lexical_cast<string, int>(own.Base.getAllGold()));
			fields.push_back(boost::lexical_cast<string, int>(own.Base.getAllGold()));//伯乐
			fields.push_back(boost::lexical_cast<string, int>(deleOpt));

			int log_type;
			if (deleType == ARTIL)
			{
				log_type = study_log_delegate_artil;
			}
			else
			{
				log_type = study_log_delegate_shield;
			}
			StreamLog::Log(study::mysqlLogStudy, own.shared_from_this(), 
				boost::lexical_cast<string, int>(own.Base.getSilver() + costSilver),
				boost::lexical_cast<string, int>(own.Base.getSilver()), fields, log_type);

			study_sys.studyRewardBroadcast(own.shared_from_this(), item_id, deleType);
		}

		if (deleOpt == 3)
		{
			deleStu[deleType].convertDrawing[2]++;
		}
		deleStu[deleType].totalDelegateTimes++;
		deleStu[deleType].todayDelegateTimes++;
		if(deleType == ARTIL)
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_cannon_study_times);
		else
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_shield_study_times);
		deleStu[deleType].delegateCD.addCD(10*60);
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerStudy::refreshPsionic( int &refresh_step )
	{//刷新规则，判断产生事件，次数增加，产生CD，是否更新最高纪录
		unsigned rate_size = 11;
		int eq_rate = commom_sys.randomBetween(0, RANGE-1);
		int star_idx;

		//进度从1开始
		for (unsigned idx = 1; idx < rate_size; idx++)
		{
			if (eq_rate < study_sys._psionicMap[idx].probability)
			{
				star_idx = idx;
				break;
			}
		}

		if (star_idx > psionicStu.getMaxStepCanRefresh())
		{
			star_idx = psionicStu.getMaxStepCanRefresh();
		}
		if (star_idx < psionicStu.currentMaxStep - 2)
		{
			star_idx = psionicStu.currentMaxStep - 2;
		}

		if (psionicStu.currentMaxStep >= 6)
		{
			if (star_idx < 6)
			{
				star_idx = 6;
			}
		}
		else
		{
			star_idx = psionicStu.getMaxStepCanRefresh();
		}

		if (star_idx < 1)
		{
			star_idx = 1;
		}
		if (star_idx > 10)
		{
			star_idx = 10;
		}

		//判断产生事件
		rate_size = 2;
		eq_rate = commom_sys.randomBetween(0, RANGE-1);
		unsigned event_idx;
		for (unsigned idx = 0; idx < rate_size; idx++)
		{
			if (eq_rate < study_sys._psionicMap[star_idx].eventRates[idx].event_chance)
			{
				event_idx = idx;
				break;
			}
		}

		psionicStu.studyEvent = event_idx;
		psionicStu.currentStep = star_idx;

		refresh_step = star_idx;

		return 0;
	}

	int playerStudy::reward()
	{
		if (psionicStu.studyStatus != REWARD)
		{
			return -1;
		}

		int reward_eq = study_sys._psionicMap[psionicStu.currentStep].eventRates[psionicStu.studyEvent].reward_eq;
		int reward_jungong = study_sys._psionicMap[psionicStu.currentStep].eventRates[psionicStu.studyEvent].reward_jungong;

		if (reward_eq != -1)
		{
			
		}

		psionicStu.studyStatus = RESEARCH;
		return 0;
	}

	int playerStudy::getDrawingId(int deleType)
	{
		return deleStu[deleType].drawingId;
	}

	void playerStudy::setDrawingId(int deleType, int val)
	{
		deleStu[deleType].drawingId = val;
	}

	int playerStudy::getPsionicStatus()
	{
		return psionicStu.studyStatus;
	}

	void playerStudy::setPsionicStatus(int val)
	{
		psionicStu.studyStatus = val;
	}

	int playerStudy::getEventEqReward()
	{
		return study_sys._psionicMap[psionicStu.currentStep].eventRates[psionicStu.studyEvent].reward_eq;
	}

	int playerStudy::getEventJgReward()
	{
		return study_sys._psionicMap[psionicStu.currentStep].eventRates[psionicStu.studyEvent].reward_jungong;
	}

	int playerStudy::getTotalPsionicStudyTimes()
	{
		return psionicStu.totalStudyTimes;
	}

	int playerStudy::getTotalDelegateTimes(int deleType)
	{
		return deleStu[deleType].totalDelegateTimes;
	}

	delegateStudy& playerStudy::getDeleStu(int deleType)
	{
		deleStu[deleType].maintainDailyData();
		return deleStu[deleType];
	}

	psionicStudy& playerStudy::getPsionicStu()
	{
		psionicStu.maintainDailyData();
		psionicStu.maintainSeasonData();
		return psionicStu;
	}

	bool playerStudy::isInDeleSet(int deleType, int deleOpt)
	{
		return deleStu[deleType].delegateSet[deleOpt] == 1;
	}

	void psionicStudy::maintainDailyData()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > nextDailyFreshTime)
		{
			studyTimes = 0;

			boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
			tm t = boost::posix_time::to_tm(p);
			unsigned standard_time = cur_time - 3600*t.tm_hour - 60*t.tm_min - t.tm_sec;
			if (t.tm_hour >= 5)
			{
				nextDailyFreshTime = unsigned(na::time_helper::get_next_time(standard_time, 1, 5));
			}
			else
			{
				nextDailyFreshTime = unsigned(na::time_helper::get_next_time(standard_time, 0, 5));
			}
		}
	}

	void psionicStudy::maintainWeeklyData()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > nextWeeklyFreshTime)
		{
			currentMaxStep = 0;
			currentStep = 1;
			studyEvent = 0;
			studyStatus = RESEARCH;
//			psionicCD = 0;

			boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
			tm t = boost::posix_time::to_tm(p);
			unsigned standard_time = cur_time - 86400*t.tm_wday - 3600*t.tm_hour - 60*t.tm_min - t.tm_sec;

			if ((t.tm_wday == 0	|| t.tm_wday == 1 && t.tm_hour < 5))
			{
				nextWeeklyFreshTime = (unsigned)na::time_helper::get_next_time(standard_time, 1, 5);
			}
			else
			{
				nextWeeklyFreshTime = (unsigned)na::time_helper::get_next_time(standard_time, 8, 5);
			}
		}
	}

	void psionicStudy::maintainSeasonData()
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (cur_time > nextWeeklyFreshTime)
		{
			currentMaxStep = 0;
			currentStep = 1;
			studyEvent = 0;
			studyStatus = RESEARCH;
//			psionicCD = 0;

			nextWeeklyFreshTime = season_sys.getNextSeasonTime(season_quarter_one, 5);
		}
	}
	
	psionicStudy::psionicStudy()
	{
		studyStatus = RESEARCH;
		currentMaxStep = 0;
		currentStep = 1;
		studyTimes = 0;
		totalStudyTimes = 0;
//		psionicCD = 0;
		studyEvent = 0;
		nextDailyFreshTime = 0;
		nextWeeklyFreshTime = 0;
		nextSeasonFreshTime = 0;
	}

	int psionicStudy::getMaxStepCanRefresh()
	{
		int maxCanRefresh = currentMaxStep + 1;
		if (maxCanRefresh > 10)
			maxCanRefresh = 10;

		return maxCanRefresh;
	}

}


