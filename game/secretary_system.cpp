#include "secretary_system.h"
#include "response_def.h"
#include "secretary_helper.h"

namespace gg
{
	secretary_system* const secretary_system::secretarySys = new secretary_system;

	void secretary_system::initData()
	{
		loadFile();
		loadIndividual();
		loadSelect();
		loadCost();
		loadExp();
		loadExpCrit();
		loadPosition();
	}

	void secretary_system::loadFile()
	{
		Json::Value val = na::file_system::load_jsonfile_val(secretary_config_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
		{
			const Json::Value& info = val[i];
			secConfigPtr ptr = creator<secretaryConfig>::run(info);
			_secConfigMap.insert(make_pair(ptr->getSid(), ptr));
		}
	}

	void secretary_system::loadIndividual()
	{
		Json::Value val1 = na::file_system::load_jsonfile_val(secretary_individual_1_file_dir_str);
		_attrConfig[secretary::_Quality_D - 1].load(val1);
		Json::Value val2 = na::file_system::load_jsonfile_val(secretary_individual_2_file_dir_str);
		_attrConfig[secretary::_Quality_C - 1].load(val2);
		Json::Value val3 = na::file_system::load_jsonfile_val(secretary_individual_3_file_dir_str);
		_attrConfig[secretary::_Quality_B - 1].load(val3);
		Json::Value val4 = na::file_system::load_jsonfile_val(secretary_individual_4_file_dir_str);
		_attrConfig[secretary::_Quality_A - 1].load(val4);
		Json::Value val5 = na::file_system::load_jsonfile_val(secretary_individual_5_file_dir_str);
		_attrConfig[secretary::_Quality_S - 1].load(val5);
	}

	void secretary_system::loadSelect()
	{
		Json::Value val = na::file_system::load_jsonfile_val(secretary_select_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
		{
			const Json::Value& temp = val[i];
			int type = temp["type"].asInt();
			_qualityRand[type - 1].load(temp);
		}
	}

	void secretary_system::loadCost()
	{
		Json::Value val = na::file_system::load_jsonfile_val(secretary_cost_file_dir_str);
		for (unsigned i = 0; i < val.size(); ++i)
			_upgradeCost.push_back(val[i].asInt());
	}

	void secretary_system::loadExp()
	{
		Json::Value val = na::file_system::load_jsonfile_val(secretary_exp_file_dir_str);
		int exp = 0;
		for (unsigned i = 0; i < val.size(); ++i)
		{
			exp += val[i].asInt();
			_upgradeExp.push_back(exp);
		}
	}

	void secretary_system::loadExpCrit()
	{
		Json::Value val = na::file_system::load_jsonfile_val(secretary_exp_crit_file_dir_str);
		_critRand.load(val);
	}

	void secretary_system::loadPosition()
	{
		Json::Value val = na::file_system::load_jsonfile_val(secretary_position_file_dir_str);
		_posLimit.load(val);
	}

	void secretary_system::infoReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		d->PlayerSecretaries.info();
	}

	void secretary_system::lotteryReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		int type = js_msg[0u].asInt();
		bool state = js_msg[1u].asBool();
		bool is_free = js_msg[2u].asBool();
		int result = d->PlayerSecretaries.lottery(type, state, is_free);
		SecretoryReturn(result);
	}

	void secretary_system::embattleReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		int sid = js_msg[0u].asInt();
		int pos = js_msg[1u].asInt();
		bool up = js_msg[2u].asBool();
		int result = d->PlayerSecretaries.embattle(sid, pos, up);
		SecretoryReturn(result);
	}

	void secretary_system::resolveReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		int sid = js_msg[0u].asInt();
		int result = d->PlayerSecretaries.resolve(sid);
		SecretoryReturn(result);
	}

	void secretary_system::combineReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		bool state = js_msg[0u].asBool();
		int result = d->PlayerSecretaries.combine(state);
		SecretoryReturn(result);
	}

	void secretary_system::swallowReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		int left_sid = js_msg[0u].asInt();
		int result = d->PlayerSecretaries.swallow(left_sid, js_msg[1u]);
		SecretoryReturn(result);
	}

	void secretary_system::upgradeReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		int sid = js_msg[0u].asInt();
		int type = js_msg[1u].asInt();
		int result = d->PlayerSecretaries.upgrade(sid, type);
		SecretoryReturn(result);
	}

	void secretary_system::swapExpReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		int left_sid = js_msg[0u].asInt();
		int right_sid = js_msg[1u].asInt();
		int result = d->PlayerSecretaries.swap_exp(left_sid, right_sid);
		SecretoryReturn(result);
	}

	void secretary_system::renameReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		if (js_msg[2u].asBool())
			Return(r, secretary::_invalid_name);

		int sid = js_msg[0u].asInt();
		string name = js_msg[1u].asString();
		int result = d->PlayerSecretaries.rename(sid, name);
		SecretoryReturn(result);
	}

	void secretary_system::lockReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		int sid = js_msg[0u].asInt();
		bool state = js_msg[1u].asBool();
		int result = d->PlayerSecretaries.lock(sid, state);
		SecretoryReturn(result);
	}

	void secretary_system::paramReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		d->PlayerSecretaries.param();
	}

	void secretary_system::showReq(msg_json& m, Json::Value& r)
	{
		SecretoryLoadPlayer(m);
		ReadJsonArray;
		int id = js_msg[0u].asInt();
		int channel = js_msg[1u].asInt();
		string name = "";
		if (js_msg.size() > 2)
			name = js_msg[2u].asString();
		int result = d->PlayerSecretaries.show(id, channel, name);
		SecretoryReturn(result);
	}

	void secretary_system::gmSecretaryListReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);

		r[msgStr][0u] = 0;
		d->PlayerSecretaries.packageGmList(r[msgStr][1u]);
	}

	void secretary_system::randQuality(int type, int& quality, int& chip_num)
	{
		_qualityRand[type].randQuality(quality, chip_num);
	}

	secConfigPtr secretary_system::randSecretory(int quality, bool lottery)
	{
		int sid = lottery ? _gainRand[quality - 1].run() : _combineRand[quality - 1].run();
		secConfigMap::iterator iter = _secConfigMap.find(sid);
		if (iter == _secConfigMap.end())
			return secConfigPtr();
		return iter->second;
	}

	int secretary_system::getUpgradeCost(int times)
	{
		if (times < 0 || times >= (int)_upgradeCost.size())
			times = _upgradeCost.size() - 1;
		return _upgradeCost[times];
	}

	int secretary_system::getUpgradeExpByLv(int lv)
	{
		if (lv == 1)
			return 0;
		lv = lv - 2;
		if (lv < 0 || lv >= (int)_upgradeExp.size())
			lv = _upgradeExp.size() - 1;
		return _upgradeExp[lv];
	}

	int secretary_system::getLvByExp(int exp, int& cExp)
	{
		for (unsigned i = 0; i < _upgradeExp.size(); ++i)
		{
			if (exp < _upgradeExp[i])
			{
				if (i == 0)
					cExp = exp;
				else
					cExp = exp - _upgradeExp[i - 1];
				return i + 1;
			}
		}
		cExp = 0;
		return _upgradeExp.size() + 1;
	}

	secConfigPtr secretary_system::getConfigById(int sid)
	{
		secConfigMap::iterator iter = _secConfigMap.find(sid);
		if (iter == _secConfigMap.end())
			return secConfigPtr();
		return iter->second;
	}

	int secretary_system::getUpgradeRate()
	{
		return _critRand.run();
	}
}