#pragma once

#include "playerManager.h"
#include "secretary_helper.h"
#include "recorder.hpp"

#define secretary_sys (*gg::secretary_system::secretarySys) 

namespace gg
{

	const static string secretary_config_file_dir_str = "./instance/secretary/secretary.json";
	const static string secretary_individual_1_file_dir_str = "./instance/secretary/individual_1.json";
	const static string secretary_individual_2_file_dir_str = "./instance/secretary/individual_2.json";
	const static string secretary_individual_3_file_dir_str = "./instance/secretary/individual_3.json";
	const static string secretary_individual_4_file_dir_str = "./instance/secretary/individual_4.json";
	const static string secretary_individual_5_file_dir_str = "./instance/secretary/individual_5.json";
	const static string secretary_select_file_dir_str = "./instance/secretary/select.json";
	const static string secretary_cost_file_dir_str = "./instance/secretary/secretary_cost.json";
	const static string secretary_exp_file_dir_str = "./instance/secretary/secretary_exp.json";
	const static string secretary_exp_crit_file_dir_str = "./instance/secretary/exp_crit.json";
	const static string secretary_position_file_dir_str = "./instance/secretary/position.json";

	const static string secretaryLogDBStr = "log_secretary";

#define SecretoryLoadPlayer(m) \
	AsyncGetPlayerData(m); \
	if (d->Base.getLevel() < secretary::_open_level) \
		Return(r, secretary::_lv_limit); \
	d->PlayerSecretaries.checkAndUpdate(); \

#define SecretoryReturn(result) \
	if(result == secretary::_success && _rParam != Json::nullValue) \
	{ \
		r[msgStr][1u] = _rParam; \
		_rParam = Json::nullValue; \
	} \
	Return(r, result); \

#define SecretoryLog(tag, ...)\
	LogTemplate(secretaryLogDBStr, own.getOwnDataPtr(), tag, __VA_ARGS__)       // __VA_ARGS__ = preV, nowV, f1, f2, ... 

	class secretary_system
	{
		public:
			static secretary_system* const secretarySys;

			void initData();
			void loadFile();
			void loadIndividual();
			void loadSelect();
			void loadCost();
			void loadExp();
			void loadExpCrit();
			void loadPosition();

			void infoReq(msg_json& m, Json::Value& r);
			void lotteryReq(msg_json& m, Json::Value& r);
			void embattleReq(msg_json& m, Json::Value& r);
			void resolveReq(msg_json& m, Json::Value& r);
			void combineReq(msg_json& m, Json::Value& r);
			void swallowReq(msg_json& m, Json::Value& r);
			void upgradeReq(msg_json& m, Json::Value& r);
			void swapExpReq(msg_json& m, Json::Value& r);
			void renameReq(msg_json& m, Json::Value& r);
			void lockReq(msg_json& m, Json::Value& r);
			void paramReq(msg_json& m, Json::Value& r);
			void showReq(msg_json& m, Json::Value& r);

			void gmSecretaryListReq(msg_json& m, Json::Value& r);

			secConfigPtr getConfigById(int sid);

			secRandom* getGainRand(){ return _gainRand; }
			secRandom* getCombineRand(){ return _combineRand; }

			void randQuality(int type, int& quality, int& chip_num);
			secConfigPtr randSecretory(int quality, bool lottery = true);

			int getUpgradeCost(int times);
			int getUpgradeRate();
			int getUpgradeExpByLv(int lv);
			int getLvByExp(int exp, int& cExp);
			int getMaxExp() const { return _upgradeExp.back(); }
			int getMaxLv() const { return _upgradeExp.size() + 1; }

			int getQualityLimit(int pos){ return _posLimit.getQualityLimit(pos); }
			int getOpenLv(int pos){ return _posLimit.getOpenLv(pos); }

			Json::Value& getParam(){ return _rParam; }

			void getAttrByLv(void* point, int lv, int quality){ return _attrConfig[quality - 1].getAttr(point, lv); }

		private:
			secConfigPtr getConfig(int type);
			int randQuality(int type);

			typedef boost::unordered_map<int, secConfigPtr> secConfigMap;
			secConfigMap _secConfigMap;

			secRandom _gainRand[secretary::_Quality_Max];
			secRandom _combineRand[secretary::_Quality_Max];
			
			attrConfig _attrConfig[secretary::_Quality_Max];
			qualityRandom _qualityRand[3];
			critRandom _critRand;
			vector<int> _upgradeCost;
			vector<int> _upgradeExp;	
			positionLimit _posLimit;

			Json::Value _rParam;
	};
}

