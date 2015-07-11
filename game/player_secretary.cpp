#include "player_secretary.h"
#include "secretary_system.h"
#include "helper.h"
#include "action_def.h"
#include "email_system.h"
#include "chat_system.h"
#include "starwar_base.h"
#include "activity_system.h"

namespace gg
{
	int playerSecretary::getAddExp(int exp, int max_lv)
	{
		int old_exp = _exp;
		int tmp_exp = _exp;
		int tmp_cExp;
		int tmp_lv;
		tmp_exp += exp;
		if (tmp_exp > secretary_sys.getMaxExp())
			tmp_exp = secretary_sys.getMaxExp();
		tmp_lv = secretary_sys.getLvByExp(tmp_exp, tmp_cExp);
		if (tmp_lv >= max_lv)
			tmp_exp = secretary_sys.getUpgradeExpByLv(max_lv);
		return tmp_exp - old_exp;
	}

	bool playerSecretary::alterExp(int& exp, int max_lv)
	{
		int tmp_lv = _level;
		int tmp_exp = _exp;
		_exp += exp;
		if (_exp > secretary_sys.getMaxExp())
			_exp = secretary_sys.getMaxExp();
		_level = secretary_sys.getLvByExp(_exp, _cExp);
		if (_level >= max_lv)
		{
			_level = max_lv;
			_cExp = 0;
			_exp = secretary_sys.getUpgradeExpByLv(_level);
		}
		exp = _exp - tmp_exp;
		return _level != tmp_lv;
	}

	bool playerSecretary::resetExp(int& exp, int max_lv)
	{
		int tmp_lv = _level;
		int tmp_exp = _exp;
		_exp = exp;
		if (_exp > secretary_sys.getMaxExp())
			_exp = secretary_sys.getMaxExp();
		_level = secretary_sys.getLvByExp(_exp, _cExp);
		if (_level >= max_lv)
		{
			_level = max_lv;
			_cExp = 0;
			_exp = secretary_sys.getUpgradeExpByLv(_level);
		}
		exp = _exp - tmp_exp;
		return _level != tmp_lv;
	}

	const secConfigPtr& playerSecretary::getConfigPtr()
	{
		if (_ptr == secConfigPtr())
			_ptr = secretary_sys.getConfigById(_sid);
		return _ptr;
	}

	void playerSecretary::package(Json::Value& info)
	{
		info = Json::arrayValue;
		info.append(_id);
		info.append(_sid);
		info.append(_name);
		info.append(_level);
		info.append(_cExp);
		info.append(_exp);
		info.append(_locked);
	}

	void playerSecretary::simplePackage(Json::Value& info)
	{
		info.append(_sid);
		info.append(_level);
	}

	void playerSecretary::init(secConfigPtr ptr)
	{
		_ptr = ptr;
		_sid = ptr->getSid();
		_level = ptr->getInitLevel();
		_exp = secretary_sys.getUpgradeExpByLv(_level);
		_cExp = 0;
	}

	void playerSecretary::init(const mongo::BSONElement& obj)
	{
		checkNotEoo(obj[secretary_id_field_str])
			_id = obj[secretary_id_field_str].Int();
		checkNotEoo(obj[secretary_sid_field_str])
			_sid = obj[secretary_sid_field_str].Int();
		checkNotEoo(obj[secretary_name_field_str])
			_name = obj[secretary_name_field_str].String();
		checkNotEoo(obj[secretary_level_field_str])
			_level = obj[secretary_level_field_str].Int();
		checkNotEoo(obj[secretary_exp_field_str])
			_exp = obj[secretary_exp_field_str].Int();
		checkNotEoo(obj[secretary_c_exp_field_str])
			_cExp = obj[secretary_c_exp_field_str].Int();
		checkNotEoo(obj[secretary_locked_field_str])
			_locked = obj[secretary_locked_field_str].Bool();
	}

	mongo::BSONObj playerSecretary::toBSON()
	{
		return BSON(secretary_id_field_str << _id << secretary_sid_field_str << _sid
			<< secretary_name_field_str << _name << secretary_level_field_str << _level
			<< secretary_exp_field_str << _exp << secretary_locked_field_str << _locked
			<< secretary_c_exp_field_str << _cExp); 
	}

	playerSecPtr playerSecretaries::_freePtr = creator<playerSecretary>::run((int)secretary::_free);
	playerSecPtr playerSecretaries::_closePtr = creator<playerSecretary>::run((int)secretary::_closed);

	playerSecretaries::playerSecretaries(playerData& own)
		: Block(own), _max_id(0), _low_free_times(0), _chip_num(0), _next_update_time(0), _inited(false), _updateInfo(Json::nullValue), _lottery_high_times(0), _first_lottery(true), _show_cd(0)
	{
		memset(_cd, 0x0, sizeof(_cd));
		unsigned i = 0;
		for (; i < secretary::_init_free_num; ++i)
			_formation[i] = _freePtr;
		for (; i < secretary::_max_upload_num; ++i)
			_formation[i] = _closePtr;

		_upgrade_times[secretary::_use_silver] = 5;
		_upgrade_times[secretary::_use_gold] = 30;
	}

	void playerSecretaries::checkAndUpdate()
	{
		unsigned now = na::time_helper::get_current_time();
		if (now >= _next_update_time)
		{
			_low_free_times = secretary::_lottery_low_free_times;
			_lottery_low_times = 0;
			_upgrade_times[secretary::_use_silver] += (((now - _next_update_time) / na::time_helper::DAY + 1) * 5);
			if (_upgrade_times[secretary::_use_silver] > 20)
				_upgrade_times[secretary::_use_silver] = 20;
			_upgrade_times[secretary::_use_gold] = 30;
			_next_update_time = na::time_helper::get_next_update_time(now);
			helper_mgr.insertSaveAndUpdate(this);
		}
		if (!_inited)
		{
			openFm();
			resetAttrs();
			_inited = true;
		}
	}

	bool playerSecretaries::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(secretary_db_str, key);
		if (!obj.isEmpty()){
			checkNotEoo(obj[secretary_chip_num_field_str])
				_chip_num = obj[secretary_chip_num_field_str].Int();
			checkNotEoo(obj[secretary_lottery_low_free_times])
				_low_free_times = obj[secretary_lottery_low_free_times].Int();
			checkNotEoo(obj[secretary_lottery_low_times])
				_lottery_low_times = obj[secretary_lottery_low_times].Int();
			checkNotEoo(obj[secretary_lottery_high_times])
				_lottery_high_times = obj[secretary_lottery_high_times].Int();
			checkNotEoo(obj[secretary_next_update_time_field_str])
				_next_update_time = obj[secretary_next_update_time_field_str].Int();
			checkNotEoo(obj[secretary_max_id_field_str])
				_max_id = obj[secretary_max_id_field_str].Int();
			checkNotEoo(obj[secretary_first_lottery_field_str])
				_first_lottery = obj[secretary_first_lottery_field_str].Bool();
			checkNotEoo(obj[secretary_show_cd_field_str])
				_show_cd = obj[secretary_show_cd_field_str].Int();
			
			checkNotEoo(obj[secretary_cd_field_str])
			{
				vector<mongo::BSONElement> ele = obj[secretary_cd_field_str].Array();
				for (unsigned i = 0; i < 3; ++i)
					_cd[i] = ele[i].Int();
			}
			checkNotEoo(obj[secretary_upgrade_times])
			{
				vector<mongo::BSONElement> ele = obj[secretary_upgrade_times].Array();
				for (unsigned i = 0; i < 2; ++i)
					_upgrade_times[i] = ele[i].Int();
			}
			checkNotEoo(obj[secretary_list_field_str])
			{
				vector<mongo::BSONElement> ele = obj[secretary_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
				{
					int id = ele[i][secretary_id_field_str].Int();
					playerSecPtr sec = creator<playerSecretary>::run(id);
					sec->init(ele[i]);
					_secMap.insert(make_pair(sec->getId(), sec));
				}
			}
			checkNotEoo(obj[secretary_formation_field_str])
			{
				vector<mongo::BSONElement> ele = obj[secretary_formation_field_str].Array();
				unsigned i = 0;
				for (; i < ele.size(); ++i)
				{
					int id = ele[i].Int();
					if (id == secretary::_closed)
					{
						_formation[i] = _closePtr;
						continue;
					}
					if (id == secretary::_free)
					{
						_formation[i] = _freePtr;
						continue;
					}
					playerSecMap::iterator iter = _secMap.find(id);
					if (iter == _secMap.end())
						_formation[i] = _freePtr;
					else
						_formation[i] = iter->second;
				}
			}
			checkNotEoo(obj[secretary_owned_sid_list_field_str])
			{
				vector<mongo::BSONElement> ele = obj[secretary_owned_sid_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
					_ownedSidList.insert(ele[i].Int());
			}
		}
		return true;
	}

	bool playerSecretaries::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr<< own.playerID 
			<< secretary_chip_num_field_str << _chip_num << secretary_lottery_low_free_times << _low_free_times
			<< secretary_lottery_low_times << _lottery_low_times << secretary_lottery_high_times << _lottery_high_times
			<< secretary_next_update_time_field_str << _next_update_time << secretary_max_id_field_str << _max_id
			<< secretary_first_lottery_field_str << _first_lottery << secretary_show_cd_field_str << _show_cd;
		{
			mongo::BSONArrayBuilder array_sec;
			for (playerSecMap::iterator iter = _secMap.begin(); iter != _secMap.end(); ++iter)
				array_sec.append(iter->second->toBSON());
			obj << secretary_list_field_str << array_sec.arr();
		}
		{
			mongo::BSONArrayBuilder array_sec;
			for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
				array_sec.append(_formation[i]->getId());
			obj << secretary_formation_field_str << array_sec.arr();
		}
		{
			mongo::BSONArrayBuilder array_sec;
			for (unsigned i = 0; i < 3; ++i)
				array_sec.append(_cd[i]);
			obj << secretary_cd_field_str << array_sec.arr();
		}
		{
			mongo::BSONArrayBuilder array_sec;
			for (unsigned i = 0; i < 2; ++i)
				array_sec.append(_upgrade_times[i]);
			obj << secretary_upgrade_times << array_sec.arr();
		}
		{
			mongo::BSONArrayBuilder array_sec;
			for (set<int>::iterator iter = _ownedSidList.begin(); iter != _ownedSidList.end(); ++iter)
				array_sec.append(*iter);
			obj << secretary_owned_sid_list_field_str << array_sec.arr();
		}
		return db_mgr.save_mongo(secretary_db_str, key, obj.obj());
	}

	void playerSecretaries::autoUpdate()
	{
		if (_updateInfo != Json::nullValue)
		{
			Json::Value msg;
			msg[msgStr][0u] = 0;
			msg[msgStr][1u] = _updateInfo;
			player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::secretary_update_resp, msg);
			_updateInfo = Json::nullValue;
		}
	}

	void playerSecretaries::packageInfo(Json::Value& info)
	{
		info[secretary_formation_field_str] = Json::arrayValue;
		Json::Value& ref_fm = info[secretary_formation_field_str];
		for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
			ref_fm.append(_formation[i]->getId());

		info[secretary_list_field_str] = Json::arrayValue;
		Json::Value& ref_sec = info[secretary_list_field_str];
		for (playerSecMap::iterator iter = _secMap.begin(); iter != _secMap.end(); ++iter)
		{
			Json::Value temp;
			iter->second->package(temp);
			ref_sec.append(temp);
		}
	}

	void playerSecretaries::packageSidList(Json::Value& info)
	{
		info[secretary_owned_sid_list_field_str] = Json::arrayValue;
		Json::Value& ref_osl = info[secretary_owned_sid_list_field_str];
		for (set<int>::iterator iter = _ownedSidList.begin(); iter != _ownedSidList.end(); ++iter)
			ref_osl.append(*iter);
	}

	void playerSecretaries::info()
	{
		if (_cd[1] == 0)
		{
			_cd[1] = na::time_helper::get_current_time() + 30 * na::time_helper::MINUTE;
			helper_mgr.insertSave(this);
		}

		Json::Value msg;
		msg[msgStr][0u] = 0;
		packageInfo(msg[msgStr][1u]);
		packageSidList(msg[msgStr][1u]);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::secretary_info_resp, msg);
	}

	void playerSecretaries::packageParam(Json::Value& info)
	{
		info[secretary_chip_num_field_str] = _chip_num;
		info[secretary_cd_field_str] = Json::arrayValue;
		Json::Value& ref_cd = info[secretary_cd_field_str];
		for (unsigned i = 0; i < 3; ++i)
			ref_cd.append(_cd[i]);
		info[secretary_lottery_low_free_times] = _low_free_times;
		info[secretary_lottery_low_times] = _lottery_low_times;
		info[secretary_upgrade_times] = Json::arrayValue;
		Json::Value& ref_ut = info[secretary_upgrade_times];
		for (unsigned i = 0; i < 2; ++i)
			ref_ut.append(_upgrade_times[i]);
	}

	void playerSecretaries::packageGmList(Json::Value& info)
	{
		info = Json::arrayValue;
		for (playerSecMap::iterator iter = _secMap.begin(); iter != _secMap.end(); ++iter)
		{
			Json::Value temp;
			iter->second->simplePackage(temp);
			int up = 0;
			for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
			{
				if (_formation[i] == iter->second)
				{
					up = 1;
					break;
				}
			}
			temp.append(up);
			info.append(temp);
		}
	}

	void playerSecretaries::param()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		packageParam(msg[msgStr][1u]);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::secretary_param_resp, msg);
	}

	int playerSecretaries::lottery(int type, bool state, bool is_free)
	{
		if (type < 0 || type > 2)
			return secretary::_client_error;

		if (_secMap.size() >= secretary::_max_secretary_num)
			return secretary::_total_limit;

		int cost = 0;
		if (is_free)
		{
			if (type == 2 || state)
				return secretary::_client_error;

			unsigned now = na::time_helper::get_current_time();
			if (now < _cd[type])
				return secretary::_in_cd;

			if (type == 0 && _low_free_times <= 0)
				return secretary::_times_limit;

			_cd[type] = now + getCdInterval(type);
			if (type == 0)
				--_low_free_times;
		}
		else
		{
			if (type == 0 && state)
				return secretary::_client_error;

			if (type == 0 && _lottery_low_times >= secretary::_lottery_max_low_times)
				return secretary::_times_limit;

			cost = getLotteryCost(type, state);
			if (type == 0)
			{
				if (own.Base.getSilver() < cost)
					return secretary::_silver_limit;
				else
					own.Base.alterSilver(0 - cost);
			}
			else
			{
				if (own.Base.getAllGold() < cost)
					return secretary::_gold_limit;
				else
					own.Base.alterBothGold(0 - cost);
			}
		}

		unsigned times = state ? 10 : 1;
		//Json::Value sid_array = Json::arrayValue;
		Json::Value& ref_param = secretary_sys.getParam();
		ref_param = Json::arrayValue;
		string str_sid;
		string str_param;
		for (unsigned i = 0; i < times; ++i)
		{
			if (type == 2)
				++_lottery_high_times;
			if (type == 0 && !is_free)
				++_lottery_low_times;

			if (i != 0)
			{
				str_sid += ",";
				str_param += ",";
			}
			int quality = -1;
			int chip_num = 0;
			secretary_sys.randQuality(type, quality, chip_num);
			if (quality == secretary::_Quality_S && _lottery_high_times <= secretary::_lottery_getS_times_limit)
				quality = secretary::_Quality_A;
			if (_first_lottery)
			{
				_first_lottery = false;
				quality = secretary::_Quality_B;
			}
			if (quality > secretary::_Quality_Max)
			{
				ref_param.append(chip_num * -1);
				_chip_num += chip_num;
				str_sid += "0";
				str_param += toString(chip_num);
				continue;
			}
			secConfigPtr ptr = secretary_sys.randSecretory(quality);
			if (ptr == secConfigPtr())
				return secretary::_server_error;
			ref_param.append(ptr->getSid());
			playerSecPtr sec = creator<playerSecretary>::run(_max_id++);
			sec->init(ptr);
			_secMap.insert(make_pair(sec->getId(), sec));
			updateInfo(sec);

			str_sid += toString(ptr->getSid());
			str_param += toString(ptr->getInitLevel());

			if (quality == secretary::_Quality_S)
				getSecretaryBroadcast(0, ptr->getSid());
		}
		//if (sid_array.size() != 0)
		//{
		//	Json::Value temp;
		//	Json::Value mail;
		//	mail[email::actionID] = action_add_secretary;
		//	mail[email::secretaryList] = sid_array;
		//	temp.append(mail);
		//	email_sys.sendSystemEmailCommon(own.getOwnDataPtr(), email_type_system_attachment, email_team_system_secretary_full, Json::Value::null, temp);
		//}
		
		if (type == 1)
			_chip_num += times;
		if (type == 2)
			_chip_num += 3 * times;

		if (type == 0 || is_free)
			param();
		SecretoryLog((int)secretary::_log_lottery, "", "", cost, type, times, str_sid, str_param);
		helper_mgr.insertSaveAndUpdate(this);
		return secretary::_success;
	}

	int playerSecretaries::swap_exp(int left_id, int right_id)
	{
		playerSecMap::iterator left_iter = _secMap.find(left_id);
		playerSecMap::iterator right_iter = _secMap.find(right_id);
		if (left_iter == _secMap.end() || right_iter == _secMap.end())
			return secretary::_id_error;

		if (left_iter->second->isLocked() || right_iter->second->isLocked())
			return secretary::_locked;

		if (own.Base.getAllGold() < secretary::_swap_exp_cost)
			return secretary::_gold_limit;
		own.Base.alterBothGold(0 - (int)secretary::_swap_exp_cost);

		int temp = left_iter->second->getExp();
		int add_exp = right_iter->second->getExp();
		int left_lv = left_iter->second->getLv();
		int right_lv = right_iter->second->getLv();
		int max_lv = getCurrentMaxLv();
		bool flag = false;
		if (resetExp(left_iter->second, add_exp, max_lv))
			flag = true;
		if (resetExp(right_iter->second, temp, max_lv))
			flag = true;
		if (flag)
			resetAttrs();
		updateInfo(left_iter->second);
		updateInfo(right_iter->second);
		helper_mgr.insertSaveAndUpdate(this);
		SecretoryLog((int)secretary::_log_swap, "", "", left_iter->second->getSid(), left_lv, right_iter->second->getSid(), right_lv, (int)secretary::_swap_exp_cost);
		return secretary::_success;
	}

	int playerSecretaries::rename(int id, const string& name)
	{
		playerSecMap::iterator iter = _secMap.find(id);
		if (iter == _secMap.end())
			return secretary::_id_error;

		if (own.Base.getAllGold() < secretary::_rename_cost)
			return secretary::_gold_limit;
		own.Base.alterBothGold(0 - (int)secretary::_rename_cost);

		iter->second->setName(name);
		updateInfo(iter->second);
		helper_mgr.insertSaveAndUpdate(this);
		return secretary::_success;
	}

	int playerSecretaries::lock(int id, bool state)
	{
		playerSecMap::iterator iter = _secMap.find(id);
		if (iter == _secMap.end())
			return secretary::_id_error;

		iter->second->setLocked(state);
		updateInfo(iter->second);
		helper_mgr.insertSaveAndUpdate(this);
		secretary_sys.getParam() = state;
		return secretary::_success;
	}

	int playerSecretaries::upgrade(int id, int type)
	{
		playerSecMap::iterator iter = _secMap.find(id);
		if (iter == _secMap.end())
			return secretary::_id_error;

		int result = upgradeLimit(iter->second);
		if (result != secretary::_success)
			return result;
		
		int cost = 0;

		if (type == secretary::_use_silver)
		{
			if (own.Base.getLevel() < 50)
				cost = own.Base.getLevel() * 300;
			else if (own.Base.getLevel() < 70)
				cost = own.Base.getLevel() * 500;
			else
				cost = own.Base.getLevel() * 800;
			if (own.Base.getSilver() < cost)
				return secretary::_silver_limit;
			if (_upgrade_times[secretary::_use_silver] <= 0)
				return secretary::_times_limit;
			own.Base.alterSilver(0 - cost);
		}
		else if (type == secretary::_use_gold)
		{
			cost = secretary_sys.getUpgradeCost(30 - _upgrade_times[secretary::_use_gold]);
			if (own.Base.getAllGold() < cost)
				return secretary::_gold_limit;
			if (_upgrade_times[secretary::_use_gold] <= 0)
				return secretary::_times_limit;
			own.Base.alterBothGold(0 - cost);
		}
		else
			return secretary::_client_error;

		int pre_exp = iter->second->getExp();
		int max_lv = own.Base.getLevel() < secretary::_role_lv_limit ? own.Base.getLevel() : secretary_sys.getMaxLv();
		int rate = secretary_sys.getUpgradeRate();
		int add_exp = own.Base.getLevel() * rate * 100;
		if (type == secretary::_use_gold)
			add_exp *= 2;
		int bv_temp = own.Embattle.getCurrentBV();
		if (alterExp(iter->second, add_exp, max_lv))
			resetAttrs();
		updateInfo(iter->second);
		--_upgrade_times[type];
		param();
		secretary_sys.getParam()[0u] = add_exp;
		secretary_sys.getParam()[1u] = rate;
		secretary_sys.getParam()[2u] = own.Embattle.getCurrentBV() - bv_temp;
		int now_exp = iter->second->getExp();
		SecretoryLog((int)secretary::_log_upgrade, pre_exp, now_exp, iter->second->getSid(), type, cost);
		helper_mgr.insertSaveAndUpdate(this);
		activity_sys.updateActivity(own.getOwnDataPtr(), 0, activity::_upgrade_secretary);
		return secretary::_success;
	}

	int playerSecretaries::swallow(int left_id, const Json::Value& right_list)
	{
		playerSecMap::iterator left_iter = _secMap.find(left_id);
		if (left_iter == _secMap.end())
			return secretary::_id_error;

		int result = upgradeLimit(left_iter->second);
		if (result != secretary::_success)
			return result;

		vector<playerSecPtr> playerSecVec;
		for (unsigned i = 0; i < right_list.size(); ++i)
		{
			playerSecMap::iterator right_iter = _secMap.find(right_list[i].asInt());
			if (right_iter == _secMap.end())
				return secretary::_id_error;
			if (right_iter->second->isLocked() || inFormat(right_iter->second->getId()) != -1)
				return secretary::_locked;

			playerSecVec.push_back(right_iter->second);
		}

		int exp = 0;
		for (unsigned i = 0; i < playerSecVec.size(); ++i)
		{
			exp += playerSecVec[i]->getExp();
			if (playerSecVec[i]->getConfigPtr()->getQuality() == secretary::_Quality_S)
				exp += 50000;
		}

		int max_lv = getCurrentMaxLv();
		int pre_exp = left_iter->second->getExp();
		string str_sid;
		exp = left_iter->second->getAddExp(exp, max_lv);
		int cost = exp;
		if (own.Base.getSilver() < cost)
			return secretary::_silver_limit;

		own.Base.alterSilver(0 - cost);

		bool flag = alterExp(left_iter->second, exp, max_lv);
		for (unsigned i = 0; i < playerSecVec.size(); ++i)
		{
			int idx = -1;
			if ((idx = inFormat(playerSecVec[i]->getId())) != -1)
			{
				_formation[i] = _freePtr;
				flag = true;
			}
			delInfo(playerSecVec[i]->getId());
		}
		updateInfo(left_iter->second);
		for (unsigned i = 0; i < playerSecVec.size(); ++i)
		{
			int id = playerSecVec[i]->getId();
			if (i != 0)
				str_sid += ",";
			str_sid += toString(playerSecVec[i]->getSid());
			_secMap.erase(id);
		}
		int bv_temp = own.Embattle.getCurrentBV();
		if (flag)
		{
			updateFm();
			resetAttrs();
		}
		secretary_sys.getParam()[0u] = exp;
		secretary_sys.getParam()[1u] = own.Embattle.getCurrentBV() - bv_temp;
		helper_mgr.insertSaveAndUpdate(this);
		int now_exp = left_iter->second->getExp();
		SecretoryLog((int)secretary::_log_swallow, pre_exp, now_exp, left_iter->second->getSid(), str_sid, cost);
		return secretary::_success;
	}

	int playerSecretaries::resolve(int id)
	{
		playerSecMap::iterator iter = _secMap.find(id);
		if (iter == _secMap.end())
			return secretary::_id_error;

		int sid = iter->second->getSid();

		secConfigPtr ptr = secretary_sys.getConfigById(sid);
		if (ptr == secConfigPtr())
			return secretary::_server_error;

		int quality = ptr->getQuality();
		if (quality < secretary::_Quality_A)
			return secretary::_quality_limit;

		if (inFormat(id) != -1)
			return secretary::_in_format;

		int get_chip = quality == secretary::_Quality_S ? secretary::_resolveS_chip_num : secretary::_resolveA_chip_num;
		_chip_num += get_chip;
		delInfo(id);
		_secMap.erase(iter);
		secretary_sys.getParam() = get_chip;
		helper_mgr.insertSaveAndUpdate(this);
		return secretary::_success;
	}

	int playerSecretaries::combine(bool state)
	{
		int need_num = state ? secretary::_combineS_chip_num : secretary::_combineA_chip_num;
		if (_chip_num < need_num)
			return secretary::_num_limit;

		if (_secMap.size() >= secretary::_max_secretary_num)
			return secretary::_total_limit;

		int quality = state ? secretary::_Quality_S : secretary::_Quality_A;
		secConfigPtr ptr = secretary_sys.randSecretory(quality, false);
		if (ptr == secConfigPtr())
			return secretary::_server_error;
		secretary_sys.getParam() = ptr->getSid();
		int preV = _chip_num;
		_chip_num -= need_num;
		addSecretary(ptr->getSid());
		if (ptr->getQuality() == secretary::_Quality_S)
			getSecretaryBroadcast(1, ptr->getSid());
		SecretoryLog((int)secretary::_log_combine, preV, _chip_num, state ? 1 : 0, ptr->getSid(), ptr->getInitLevel(), need_num);
		helper_mgr.insertSaveAndUpdate(this);
		return secretary::_success;
	}

	int playerSecretaries::embattle(int id, int pos, bool up)
	{
		return up ? upLoad(id, pos) : unLoad(id, pos);
	}

	int playerSecretaries::upLoad(int id, int pos)
	{
		if (pos < 1 || pos > secretary::_max_upload_num)
			return secretary::_client_error;

		if (_formation[pos - 1] == _closePtr)
			return secretary::_not_opened;

		playerSecMap::iterator iter = _secMap.find(id);
		if (iter == _secMap.end())
			return secretary::_id_error;

		if (iter->second->getConfigPtr()->getQuality() < secretary_sys.getQualityLimit(pos))
			return secretary::_quality_limit;

		for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
		{
			if (i == pos - 1)
				continue;
			if (_formation[i] != _closePtr && _formation[i] != _freePtr)
			{
				if (_formation[i]->getSid() == iter->second->getSid())
					return secretary::_same_sid;
			}
		}

		_formation[pos - 1] = iter->second;
		resetAttrs();
		updateFm();
		helper_mgr.insertSaveAndUpdate(this);
		return secretary::_success;
	}

	int playerSecretaries::unLoad(int id, int pos)
	{
		if (pos < 1 || pos > secretary::_max_upload_num)
			return secretary::_client_error;

		if (_formation[pos - 1]->getId() != id)
			return secretary::_id_error;

		_formation[pos - 1] = _freePtr;
		resetAttrs();
		updateFm();
		helper_mgr.insertSaveAndUpdate(this);
		return secretary::_success;
	}

	int playerSecretaries::show(int id, int channel, const string& target_name)
	{
		unsigned now = na::time_helper::get_current_time();
		if (now < _show_cd)
			return secretary::_in_cd;

		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][senderChannelStr] = channel;
		msg[msgStr][1u]["sd"].append(own.playerID);
		msg[msgStr][1u]["sd"].append(own.Base.getName());
		if (id == -1)
		{
			Json::Value content;
			for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
			{
				Json::Value temp;
				if (_formation[i] == _closePtr)
					temp.append(secretary::_closed);
				else if (_formation[i] == _freePtr)
					temp.append(secretary::_free);
				else
				{
					temp.append(_formation[i]->getSid());
					temp.append(_formation[i]->getLv());
				}
				content.append(temp);
			}
			msg[msgStr][1u]["sd"].append(content);
			msg[msgStr][1u][chatBroadcastID] = BROADCAST::secretary_room_show;
			chat_sys.chatOption(channel, gate_client::chat_broadcast_resp, msg, own.getOwnDataPtr(), target_name);
		}
		else
		{
			playerSecMap::iterator iter = _secMap.find(id);
			if (iter == _secMap.end())
				return secretary::_id_error;
			msg[msgStr][1u]["sd"].append(iter->second->getSid());
			Json::Value content;
			content.append(iter->second->getSid());
			content.append(iter->second->getLv());
			content.append(iter->second->getCExp());
			msg[msgStr][1u]["sd"].append(content);
			msg[msgStr][1u][chatBroadcastID] = BROADCAST::secretary_show;
			chat_sys.chatOption(channel, gate_client::chat_broadcast_resp, msg, own.getOwnDataPtr(), target_name);
		}
		
		_show_cd = now + na::time_helper::MINUTE * 1;
		return secretary::_success;
	}

	int playerSecretaries::addSecretary(int sid)
	{
		secConfigPtr ptr = secretary_sys.getConfigById(sid);
		if (ptr == secConfigPtr())
			return secretary::_id_error;

		if ((int)_secMap.size() >= secretary::_max_secretary_num)
			return secretary::_total_limit;

		playerSecPtr sec = creator<playerSecretary>::run(_max_id++);
		sec->init(ptr);
		_secMap.insert(make_pair(sec->getId(), sec));
		updateInfo(sec);
		helper_mgr.insertSaveAndUpdate(this);
		return secretary::_success;
	}

	int playerSecretaries::addSecretatyWithoutCheck(int sid)
	{
		secConfigPtr ptr = secretary_sys.getConfigById(sid);
		if (ptr == secConfigPtr())
			return secretary::_id_error;

		/*if ((int)_secMap.size() >= secretary::_max_secretary_num)
			return secretary::_total_limit;*/

		playerSecPtr sec = creator<playerSecretary>::run(_max_id++);
		sec->init(ptr);
		_secMap.insert(make_pair(sec->getId(), sec));
		updateInfo(sec);
		helper_mgr.insertSaveAndUpdate(this);
		return secretary::_success;
	}

	int playerSecretaries::addSecretaryWithMail(secConfigPtr ptr)
	{
		Json::Value sid_array = Json::arrayValue;
		if ((int)_secMap.size() >= secretary::_max_secretary_num)
			sid_array.append(ptr->getSid());
		else
		{
			playerSecPtr sec = creator<playerSecretary>::run(_max_id++);
			sec->init(ptr);
			_secMap.insert(make_pair(sec->getId(), sec));
			updateInfo(sec);
		}
		if (sid_array.size() != 0)
		{
			Json::Value mail;
			Json::Value temp;
			mail[email::actionID] = action_add_secretary;
			mail[email::secretaryList] = sid_array;
			temp.append(mail);
			email_sys.sendSystemEmailCommon(own.getOwnDataPtr(), email_type_system_attachment, email_team_system_secretary_full, Json::Value::null, temp);
		}
		helper_mgr.insertSaveAndUpdate(this);
		return secretary::_success;
	}

	void playerSecretaries::getAttrs(void* point)
	{
		if (!_inited)
		{
			openFm();
			resetAttrs();
			_inited = true;
		}
		memcpy(point, _attrs, sizeof(_attrs));
	}

	void playerSecretaries::resetAttrs()
	{
		memset(_attrs, 0x0, sizeof(_attrs));

		int format[secretary::_max_upload_num];
		int num = 0;
		for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
		{
			if (_formation[i] != _freePtr && _formation[i] != _closePtr)
			{
				format[num] = _formation[i]->getSid();
				++num;
			}
		}

		for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
		{
			if (_formation[i] == _freePtr || _formation[i] == _closePtr)
				continue;

			int id = _formation[i]->getId();

			playerSecMap::iterator iter = _secMap.find(id);
			if (iter != _secMap.end())
			{
				const secConfigPtr& ptr = iter->second->getConfigPtr();
				if (ptr != secConfigPtr())
				{
					double added[characterNum];
					ptr->getAttrs(added, iter->second->getLv());
					double c_added[characterNum];
					ptr->getCombineAttrs(c_added, format, num);
					for (unsigned j = 0; j < characterNum; ++j)
					{
						_attrs[j] += added[j];
						_attrs[j] += c_added[j];
					}
				}
			}
		}

		if(_inited)
			own.Pilots.recalBattleValue();
	}

	bool playerSecretaries::canAddSecretary(int num) const
	{
		return (_secMap.size() < secretary::_max_secretary_num) && ((int)_secMap.size() + num < secretary::_max_secretary_num + 10);
	}

	int playerSecretaries::inFormat(int id)
	{
		for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
		{
			if (_formation[i]->getId() == id)
				return i;
		}
		return -1;
	}

	bool playerSecretaries::resetExp(playerSecPtr ptr, int& exp, int max_lv)
	{
		return ptr->resetExp(exp, max_lv) && inFormat(ptr->getId()) != -1;
	}

	bool playerSecretaries::alterExp(playerSecPtr ptr, int& exp, int max_lv)
	{
		return ptr->alterExp(exp, max_lv) && inFormat(ptr->getId()) != -1;
	}

	void playerSecretaries::updateFm()
	{
		_updateInfo["fm"] = Json::arrayValue;
		Json::Value& ref_fm = _updateInfo["fm"];
		for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
			ref_fm.append(_formation[i]->getId());
	}

	void playerSecretaries::updateInfo(playerSecPtr ptr)
	{
		Json::Value temp;
		ptr->package(temp);
		_updateInfo["sec_up"].append(temp);
		if (!inSidList(ptr->getSid()))
		{
			_ownedSidList.insert(ptr->getSid());
			_updateInfo["os"].append(ptr->getSid());
		}
	}

	void playerSecretaries::delInfo(int id)
	{
		_updateInfo["sec_del"].append(id);
	}

	int playerSecretaries::getLotteryCost(int type, bool state)
	{
		if (type == 0)
			return ((_lottery_low_times + 1) * 20 + 100) * own.Base.getLevel();
		if (type == 1)
			return state ? secretary::_ten_lottery_min_cost : secretary::_lottery_min_cost;
		
		return state ? secretary::_ten_lottery_high_cost : secretary::_lottery_high_cost;
	}

	int playerSecretaries::upgradeLimit(playerSecPtr ptr)
	{
		if (ptr->getExp() >= secretary_sys.getMaxExp())
			return secretary::_exp_limit;
		if (ptr->getLv() >= own.Base.getLevel() && own.Base.getLevel() < secretary::_role_lv_limit)
			return secretary::_lv_limit;
		return secretary::_success;
	}

	int playerSecretaries::getCurrentMaxLv()
	{
		return own.Base.getLevel() < secretary::_role_lv_limit ? own.Base.getLevel() : secretary_sys.getMaxLv();
	}

	unsigned playerSecretaries::getCdInterval(int type)
	{
		switch (type)
		{
		case 0:
			return 5 * na::time_helper::MINUTE;
		case 1:
			return 24 * na::time_helper::HOUR;
		default:
			return 72 * na::time_helper::HOUR;
		}
	}

	void playerSecretaries::openFm()
	{
		for (unsigned i = 0; i < secretary::_max_upload_num; ++i)
		{
			if (_formation[i] == _closePtr && own.Base.getLevel() >= secretary_sys.getOpenLv(i + 1))
			{
				_formation[i] = _freePtr;
				updateFm();
				helper_mgr.insertUpdate(this);
			}
		}
	}

	void playerSecretaries::getSecretaryBroadcast(int type, int sid)
	{
		Json::Value msg;
		msg.append(type);
		msg.append(own.playerID);
		msg.append(own.Base.getName());
		msg.append(sid);
		chat_sys.sendToAllBroadCastCommon(BROADCAST::get_secretary_broadcast, msg);
	}
}
