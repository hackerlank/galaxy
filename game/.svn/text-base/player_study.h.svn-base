#pragma once
#include "block.h"
#include "playerCD.h"

namespace gg
{
	enum delegateQuality
	{
		QUALITY_GREEN = 2,
		QUALITY_YELLOW,
		QUALITY_RED,
		QUALITY_PURPLE
	};

	enum
	{
		study_log_delegate_artil,
		study_log_delegate_shield,
		study_log_psionic_refresh,
		study_log_psionic_reward,
	};

	struct delegateStudy
	{
		delegateStudy();
		void maintainDailyData();
		int delegateSet[4];
		int drawingId;
		playerCD delegateCD;
		int totalDelegateTimes;
		int convertDrawing[3];
		int todayDelegateTimes;
		bool todayHasPopupUI;
		unsigned nextDelegateDailyFreshTime;
	};

	struct psionicStudy
	{
		psionicStudy();
		void maintainDailyData();
		void maintainWeeklyData();
		void maintainSeasonData();
		int getMaxStepCanRefresh();
		int currentMaxStep;
		int currentStep;
		int studyTimes;
		int totalStudyTimes;
		unsigned nextDailyFreshTime;
		unsigned nextWeeklyFreshTime;
		unsigned nextSeasonFreshTime;
//		unsigned psionicCD;
		int studyStatus;
		int studyEvent;
	};

	namespace study
	{
		const static string strArtillery = "sa";
		const static string strShield = "ss";
		const static string strPsionic = "sp";
		
		const static string strCurrentMaxStep = "cms";
		const static string strCurrentStep = "cs";
		const static string strNextDailyFreshTime = "ndft";
		const static string strNextWeeklyFreshTime = "nwft";
		const static string strNextSeasonFreshTime = "nsft";
//		const static string strPsionicCD = "pcd";
		const static string strStudyEvent = "se";
		const static string strStudyStatus = "st";
		const static string strStudyTodayFinishTimes = "ft";
		const static string strStudyTotalFinishTimes = "tft";

		const static string strDelegateSet = "ds";
		const static string strDelegateCD = "dcd";
		const static string strDelegateCDLock = "dcdl";
		const static string strDrawingId = "did";
		const static string strTotalDelegateTimes = "dtt";
		const static string strConvertDrawing = "ctd";
		const static string strDelegateCounts = "dc";
		const static string strTodayDelegateTimes = "dtdt";
		const static string strNextDelegateDailyFreshTime = "ddft";
		const static string strHasPopupUI = "hpu";

		const static string mysqlLogStudy = "log_study";
	}

	class playerStudy : public Block
	{
	public:
		playerStudy(playerData& own);
		void autoUpdate();
		bool get();
		bool save();
		void package(Json::Value& pack, int packOpt = -1);
		void player_delegate(int deleType, int deleOpt, int &deleDrawingID, bool &isBagFull, int costSilver);
		int refreshPsionic(int &refresh_step);
		int reward();
		int getDrawingId(int deleType);
		void setDrawingId(int deleType, int val);
		int getPsionicStatus();
		void setPsionicStatus(int val);
		int getEventEqReward();
		int getEventJgReward();
		int getTotalDelegateTimes(int deleType);
		int getTotalPsionicStudyTimes();
		delegateStudy& getDeleStu(int deleType);
		psionicStudy& getPsionicStu();
		bool isInDeleSet(int deleType, int deleOpt);
	private:
		delegateStudy deleStu[2];
		psionicStudy psionicStu;
	};
}


