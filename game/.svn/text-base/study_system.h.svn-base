#pragma once
#include "json/json.h"
#include "msg_base.h"
#include <boost/unordered_map.hpp>
#include "player_study.h"

#define study_sys (*gg::study_system::studySys)

using namespace na::msg;

namespace gg
{
#define TYPEOPT2ID (deleType*10+deleOpt)
#define IDTYPE2OPT(id) (id-deleType*10)
#define RANGE 10000
#define PRECISION 0.000001f

#define ARTIL 0
#define SHIELD 1
#define PSIONIC 2

#define RESEARCH 0
#define REWARD 1

#define MAX_STUDY_TIMES 8

	namespace study
	{
		const static string studyDBStr = "gl.player_study";
		const static string strDelegateDataDir = "./instance/travel/";
		const static string strPsionicDataDir = "./instance/psionic/";
	}

	struct deleConfig
	{
		int id;
		int cost;
		int canComeMerchantId;
		int canComeMerchantRate;
		vector<int> canGetEquipmentRawIds;
		vector<int> canGetEquipmentRates;
		bool isPermanent;
	};

	struct eventConfig
	{
		int reward_eq;
		int reward_jungong;
		int event_chance;
	};

	struct psionicConfig
	{
		int id;
		int probability;
		vector <eventConfig> eventRates;
	};

	class study_system
	{
		friend class playerStudy;
	public:
		static study_system* const studySys;
		study_system();
		~study_system();
		void initData();
		void initDelegate();
		void initPsionic();
		void studyUpdateReq(msg_json& m, Json::Value& r);
		void delegateReq(msg_json& m, Json::Value& r);
		void delegateStoreReq(msg_json& m, Json::Value& r);
		void delegateSellReq(msg_json& m, Json::Value& r);
		void delegateConvertReq(msg_json& m, Json::Value& r);
		void delegateClearCdReq(msg_json& m, Json::Value& r);
		void delegateActivateTheThirdGridReq(msg_json& m, Json::Value& r);
		void delegateFreeTimeEndNotifyReq(msg_json& m, Json::Value& r);
		void psionicResearchReq(msg_json& m, Json::Value& r);
		void psionicRewardReq(msg_json& m, Json::Value& r);
		void psionicRefreshReq(msg_json& m, Json::Value& r);
		void psionicFinishImmediateReq(msg_json& m, Json::Value& r);
		void psionic10RefreshReq(msg_json& m, Json::Value& r);
		void psionicCancelResearchReq(msg_json& m, Json::Value& r);
		void studyRewardBroadcast(playerDataPtr player, int itemID, int opt);

	private:
		typedef boost::unordered_map<int, deleConfig> deleMap;
		typedef map<int, psionicConfig> psionicMap;
		deleMap _deleMap;
		psionicMap _psionicMap;
		int drawingMap[2][3];
	};
}
