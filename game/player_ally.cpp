#include "player_ally.h"
#include "helper.h"
#include "time_helper.h"
#include "record_system.h"
#include "vip_system.h"
#include "ally_system.h"

namespace gg
{

#define UPDATE_OWN() \
	unsigned now = na::time_helper::get_current_time(); \
	own.Ally.checkAndUpdate(now); \

#define UPDATE_BOTH() \
	unsigned now = na::time_helper::get_current_time(); \
	own.Ally.checkAndUpdate(now); \
	targetP->Ally.checkAndUpdate(now); \

	allyBaseInfo::allyBaseInfo(playerDataPtr d)
	{
		_player_id = d->playerID;
		_nickname = d->Base.getName();
		_lv = d->Base.getLevel();
		_face_id = d->Base.getFaceID();
	}

	void allyBaseInfo::alter(playerDataPtr d)
	{
		_nickname = d->Base.getName();
		_lv = d->Base.getLevel();
		_face_id = d->Base.getFaceID();
	}

	applicant::applicant(playerDataPtr d, int state)
		: allyBaseInfo(d), _state(state)
	{
	}

	allyInfo::allyInfo(playerDataPtr d, int state)
		: applicant(d, state)
	{
		_charge_gold = d->Vip.getRechargeGold();
	}

	void allyInfo::package(Json::Value& info)
	{
		applicant::package(info);
		info.append(vip_sys.isVipByChargeGold(_charge_gold));
	}

	void allyInfo::alter(playerDataPtr targetP)
	{
		allyBaseInfo::alter(targetP);
		_charge_gold = targetP->Vip.getRechargeGold();
	}

	playerAlly::playerAlly(playerData& own)
		: Block(own), _had_allys(false), _times(_apply_times), _reward(false), _next_update_time(0), _rp_valid(false), _rp_state(false), _new_applied(false), _is_first(true)
	{
		_bLogins[_own] = false;
		_bLogins[_other] = false;
	}

	bool playerAlly::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(allyDbStr, key);
		if(!obj.isEmpty()){
			checkNotEoo(obj[ally_next_update_time_field_str])
				_next_update_time = obj[ally_next_update_time_field_str].Int();
			checkNotEoo(obj[ally_times_field_str])
				_times = obj[ally_times_field_str].Int();
			checkNotEoo(obj[ally_had_allys_field_str])
				_had_allys = obj[ally_had_allys_field_str].Bool();
			checkNotEoo(obj[ally_reward_field_str])
				_reward = obj[ally_reward_field_str].Bool();
			checkNotEoo(obj[ally_ally_info_field_str])
				_allyInfo.setFromBSON(obj[ally_ally_info_field_str]);
			checkNotEoo(obj[ally_bLogin_own_field_str])
				_bLogins[_own] = obj[ally_bLogin_own_field_str].Bool();
			checkNotEoo(obj[ally_bLogin_other_field_str])
				_bLogins[_other] = obj[ally_bLogin_other_field_str].Bool();
			checkNotEoo(obj[ally_active_field_str])
				_active = obj[ally_active_field_str].Bool();
			checkNotEoo(obj[ally_new_applied_field_str])
				_new_applied = obj[ally_new_applied_field_str].Bool();
			checkNotEoo(obj[ally_is_first_enter_field_str])
				_is_first = obj[ally_is_first_enter_field_str].Bool();
			checkNotEoo(obj[ally_apply_list_field_str])
			{
				_apply_list.clear();
				vector<mongo::BSONElement> ele = obj[ally_apply_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
				{
					applicant a;
					a.setFromBSON(ele[i]);
					addApplicant(a, 0);
				}
			}
			checkNotEoo(obj[ally_applied_list_field_str])
			{
				_applied_list.clear();
				vector<mongo::BSONElement> ele = obj[ally_applied_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
				{
					applicant a;
					a.setFromBSON(ele[i]);
					addApplicant(a, 1);
				}
			}
		}
		return true;
	}

	bool playerAlly::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID << ally_next_update_time_field_str << _next_update_time
			<< ally_times_field_str << _times << ally_had_allys_field_str << _had_allys
			<< ally_reward_field_str << _reward << ally_bLogin_own_field_str << _bLogins[_own]
			<< ally_bLogin_other_field_str << _bLogins[_other] << ally_active_field_str << _active
			<< ally_new_applied_field_str << _new_applied << ally_is_first_enter_field_str << _is_first;
		{
			mongo::BSONObjBuilder b;
			_allyInfo.toBSONBuilder(b);
			obj << ally_ally_info_field_str << b.obj();
		}
		{
			mongo::BSONArrayBuilder array_ally;
			for (applicants::iterator iter = _apply_list.begin(); iter != _apply_list.end(); ++iter)
			{
				mongo::BSONObjBuilder b;
				iter->toBSONBuilder(b);
				array_ally.append(b.obj());
			}
			obj << ally_apply_list_field_str << array_ally.arr();
		}
		{
			mongo::BSONArrayBuilder array_ally;
			for (applicants::iterator iter = _applied_list.begin(); iter != _applied_list.end(); ++iter)
			{
				mongo::BSONObjBuilder b;
				iter->toBSONBuilder(b);
				array_ally.append(b.obj());
			}
			obj << ally_applied_list_field_str << array_ally.arr();
		}
		return db_mgr.save_mongo(allyDbStr, key, obj.obj());
	}

	void playerAlly::update()
	{
		UPDATE_OWN();

		Json::Value msg;
		msg[msgStr][0u] = 0;
		package(msg[msgStr][1u]);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::ally_update_resp, msg); 
	}

	void playerAlly::autoUpdate()
	{
		//update();
	}

	bool playerAlly::getRewardState()
	{
		if(_had_allys && _bLogins[_other] && !_reward)
			return true;
		return false;
	}

	void playerAlly::package(Json::Value& info)
	{
		info[ally_times_field_str] = _times;
		info[ally_had_allys_field_str] = _had_allys;
		info[ally_reward_field_str] = (_had_allys && _bLogins[_other]) ? (_reward ? 1 : 2) : 0;
		info[ally_new_applied_field_str] = _new_applied;
		info[ally_reward_info_field_str] = Json::arrayValue;
		info[ally_reward_info_field_str].append(ally_sys.getGold());
		unsigned extra_gold = 0;
		if(vip_sys.isVipByChargeGold(own.Vip.getRechargeGold()))
			extra_gold += ally_sys.getExtraGold();
		if(vip_sys.isVipByChargeGold(_allyInfo.getChargeGold()))
			extra_gold += ally_sys.getExtraGold();
		info[ally_reward_info_field_str].append(extra_gold);

		if(_allyInfo.getPlayerId() != -1)
		{
			_allyInfo.package(info[ally_ally_info_field_str]);
			int player_id = _allyInfo.getPlayerId();
			playerDataPtr targetP = player_mgr.getPlayerMain(player_id);
			if(targetP == playerDataPtr())
				info[ally_ally_info_field_str].append(false);
			else
				info[ally_ally_info_field_str].append(targetP->netID >= 0);			
		}
		if(_allyInfo.empty())
		{
			info[ally_apply_list_field_str] = Json::arrayValue;
			Json::Value& ref = info[ally_apply_list_field_str];
			for(applicants::iterator iter = _apply_list.begin(); iter != _apply_list.end(); ++iter)
			{
				Json::Value a;
				iter->package(a);
				ref.append(a);
			}
			info[ally_applied_list_field_str] = Json::arrayValue;
			Json::Value& ref2 = info[ally_applied_list_field_str];
			for(applicants::iterator iter = _applied_list.begin(); iter != _applied_list.end(); ++iter)
			{
				Json::Value a;
				iter->package(a);
				ref2.append(a);
			}
		}
	}

	void playerAlly::postToActiveReward()
	{
		if(_active || _allyInfo.empty())
			return;

		int target_id = _allyInfo.getPlayerId();
		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(own.playerID);
		msg[msgStr].append(target_id);
		string str = msg.toStyledString();
		na::msg::msg_json m(gate_client::ally_active_op_reward_inner_req, str);
		player_mgr.postMessage(m);
	}

	void playerAlly::setFlag(bool flag)
	{
		if (flag && _is_first)
		{
			_is_first = false;
			redPoint();
			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	void playerAlly::active(bool flag)
	{
		_active = true;
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerAlly::setbLogin(int side, bool flag)
	{
		if(_bLogins[side] != flag)
		{
			_bLogins[side] = flag;
			if(_bLogins[side] && side == _own)
				postToActiveReward();
			helper_mgr.insertSaveAndUpdate(this);
		}
		if(side == _other)
			redPoint();
	}

	void playerAlly::redPoint(bool flag)
	{
		if(!_rp_valid || (_rp_valid && _rp_state != flag))
		{
			_rp_valid = true;
			_rp_state = flag;

			Json::Value msg;
			msg[msgStr][0u] = 0;
			msg[msgStr][1u] = flag;
			player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::ally_red_point_resp, msg); 
		}
	}

	void playerAlly::redPoint()
	{
		if(own.netID < 0)
			return;
		redPoint((_allyInfo.empty() && _new_applied) || (_allyInfo.empty() && _apply_list.empty() && _is_first) || (!_reward && _bLogins[_other]));
	}

	void playerAlly::playerLogin()
	{
		UPDATE_OWN();
		setbLogin(_own, true);
		redPoint();
	}

	void playerAlly::playerLogout()
	{
		UPDATE_OWN();
		setbLogin(_own, true);
		_rp_valid = false;
	}

	void playerAlly::checkAndUpdate(unsigned now)
	{
		if(now >= _next_update_time)
		{
			_is_first = true;
			_times = _apply_times;
			_had_allys = !getAllyInfo().empty();
			_reward = false;
			_next_update_time = na::time_helper::get_next_update_time(now);
			_active = false;
			_bLogins[_own] = own.netID >= 0;
			_bLogins[_other] = false;
			if(_bLogins[_own])
				postToActiveReward();
			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	void playerAlly::setAllyInfo(const allyInfo& info)
	{
		_allyInfo = info;
		_had_allys = true;
		postToUpdateState();
		clearApplicant();
	}

	void playerAlly::clearApplicant()
	{
		_apply_list.clear();
		_applied_list.clear();
		_new_applied = false;
	}

	bool playerAlly::checkInApplyList(int target_id)
	{
		for(applicants::iterator iter = _apply_list.begin(); iter != _apply_list.end(); ++iter)
		{
			if(iter->getPlayerId() == target_id)
				return true;
		}
		return false;
	}

	int playerAlly::send(playerDataPtr targetP)
	{
		UPDATE_BOTH();

		if(_times == 0)
			return _times_limit;
		if(_had_allys)
			return _had_ally;
		if(checkInApplyList(targetP->playerID))
			return _to_same_person;
		if(!targetP->Ally.getAllyInfo().empty())
			return _op_had_ally;

		applicant a(targetP, apply_state::_wait_for_verify);
		addApplicant(a, _apply);
		applicant b(own.getOwnDataPtr(), apply_state::_wait_for_deal);
		targetP->Ally.addApplicant(b, _applied);
		--_times;

		redPoint();
		helper_mgr.insertSaveAndUpdate(this);
		return _success;
	}

	int playerAlly::cancel(playerDataPtr targetP)
	{
		UPDATE_BOTH();

		for(applicants::iterator iter = _apply_list.begin(); iter != _apply_list.end(); ++iter)
		{
			if(iter->getPlayerId() == targetP->playerID)
			{
				_apply_list.erase(iter);
				targetP->Ally.handleCancelled(own.playerID);
				redPoint();
				helper_mgr.insertSaveAndUpdate(this);
				return _success;
			}
		}
		return _not_in_list;
	}

	int playerAlly::agree(playerDataPtr targetP)
	{
		UPDATE_BOTH();

		if(_had_allys)
			return _had_ally;
		if(targetP->Ally.hadAlly())
			return _op_had_ally;

		for(applicants::iterator iter = _applied_list.begin(); iter != _applied_list.end(); ++iter)
		{
			if(iter->getPlayerId() == targetP->playerID)
			{
				if(iter->getState() != apply_state::_wait_for_deal)
					return _not_in_list;

				allyInfo a(targetP, apply_state::_normal);
				setAllyInfo(a);

				allyInfo b(own.getOwnDataPtr(), apply_state::_agreed);
				targetP->Ally.setAllyInfo(b);

				setbLogin(_other, targetP->Ally.getbLogin(_own));
				targetP->Ally.setbLogin(_other, _bLogins[_own]);

				redPoint();
				log(_log_agree);
				helper_mgr.insertSaveAndUpdate(this);
				return _success;
			}
		}
		return _not_in_list;
	}

	int playerAlly::refuse(playerDataPtr targetP)
	{
		UPDATE_BOTH();

		for(applicants::iterator iter = _applied_list.begin(); iter != _applied_list.end(); ++iter)
		{
			if(iter->getPlayerId() == targetP->playerID && iter->getState() == apply_state::_wait_for_deal)
			{
				if(iter->getState() != apply_state::_wait_for_deal)
					return _not_in_list;

				_applied_list.erase(iter);
				targetP->Ally.handleRefused(own.playerID);
				redPoint();
				helper_mgr.insertSaveAndUpdate(this);
				return _success;
			}
		}
		return _not_in_list;
	}

	int playerAlly::dissolve(playerDataPtr targetP)
	{
		UPDATE_BOTH();

		if(_allyInfo.empty())
			return _not_had_ally;
		if(_allyInfo.getPlayerId() != targetP->playerID)
			return _not_in_list;

		_allyInfo.setState(apply_state::_backup);
		targetP->Ally.handleDissolved();
		
		redPoint();
		log(_log_dissolve);
		helper_mgr.insertSaveAndUpdate(this);
		return _success;
	}

	int playerAlly::del(int type, int target_id)
	{
		UPDATE_OWN();

		if(type == 3)
		{
			if(_new_applied)
			{
				_new_applied = false;
				redPoint();
				helper_mgr.insertSaveAndUpdate(this);
			}
			return _success;
		}

		if(type == 2)
		{
			if(_allyInfo.getState() == apply_state::_agreed)
			{
				_allyInfo.setState(apply_state::_normal);
				helper_mgr.insertSaveAndUpdate(this);
			}
			else if(_allyInfo.getState() == apply_state::_dissolved)
			{
				_allyInfo.setState(apply_state::_backup);
				helper_mgr.insertSaveAndUpdate(this);
			}
			else
				return _not_in_list;
			
			return _success;
		}

		applicants& l = type == 0? _apply_list : _applied_list;
		for(applicants::iterator iter = l.begin(); iter != l.end(); ++iter)
		{
			if(iter->getPlayerId() == target_id)
			{
				l.erase(iter);
				helper_mgr.insertSaveAndUpdate(this);
				return _success;
			}
		}
		return _not_in_list;
	}

	void playerAlly::addApplicant(const applicant& a, int type)
	{
		applicants& l = type == _apply? _apply_list : _applied_list;
		for(applicants::iterator iter = l.begin(); iter != l.end(); ++iter) // 删除同个人的申请
		{
			if(iter->getPlayerId() == a.getPlayerId())
			{
				l.erase(iter);
				break;
			}
		}

		l.push_front(a);
		while(l.size() > 10)
		{
			applicant b = l.back();
			if(type == _apply)
			{
				if(b.getState() == apply_state::_wait_for_verify)
					postToUpdateState(_applied, b.getPlayerId(), apply_state::_cancelled);
			}
			else
			{
				if(b.getState() == apply_state::_wait_for_deal)
					postToUpdateState(_apply, b.getPlayerId(), apply_state::_invalid);
			}
			l.pop_back();
		}

		if(type == _applied)
		{
			_new_applied = true;
			redPoint();
		}

		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerAlly::postToUpdateState()
	{
		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(_applied);
		msg[msgStr].append(apply_state::_cancelled);
		msg[msgStr].append(own.playerID);
		for(applicants::iterator iter = _apply_list.begin(); iter != _apply_list.end(); ++iter)
		{
			if(iter->getPlayerId() == _allyInfo.getPlayerId())
				continue;
			msg[msgStr].append(iter->getPlayerId());
		}

		if(msg[msgStr].size() > 3)
		{
			string str = msg.toStyledString();
			na::msg::msg_json m(gate_client::ally_op_update_inner_req, str);
			player_mgr.postMessage(m);
		}

		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(_apply);
		msg[msgStr].append(apply_state::_have_ally);
		msg[msgStr].append(own.playerID);
		for(applicants::iterator iter = _applied_list.begin(); iter != _applied_list.end(); ++iter)
		{
			if(iter->getPlayerId() == _allyInfo.getPlayerId())
				continue;
			msg[msgStr].append(iter->getPlayerId());
		}

		if(msg[msgStr].size() > 3)
		{
			string str = msg.toStyledString();
			na::msg::msg_json m(gate_client::ally_op_update_inner_req, str);
			player_mgr.postMessage(m);
		}
	}

	void playerAlly::postToUpdateState(int type, int target_id, int state)
	{
		Json::Value msg;
		msg[msgStr] = Json::arrayValue;
		msg[msgStr].append(type);
		msg[msgStr].append(state);
		msg[msgStr].append(own.playerID);
		msg[msgStr].append(target_id);

		string str = msg.toStyledString();
		na::msg::msg_json m(gate_client::ally_op_update_inner_req, str);
		player_mgr.postMessage(m);
	}

	void playerAlly::handleCancelled(int target_id)
	{
		UPDATE_OWN();

		for(applicants::iterator iter = _applied_list.begin(); iter != _applied_list.end(); ++iter)
		{
			if(iter->getPlayerId() == target_id)
			{
				_applied_list.erase(iter);
				redPoint();
				helper_mgr.insertSaveAndUpdate(this);
				return;
			}
		}
	}

	void playerAlly::handleHadAlly(int target_id)
	{
		UPDATE_OWN();

		for(applicants::iterator iter = _apply_list.begin(); iter != _apply_list.end(); ++iter)
		{
			if(iter->getPlayerId() == target_id)
			{
				iter->setState(apply_state::_have_ally);
				redPoint();
				helper_mgr.insertSaveAndUpdate(this);
				return;
			}
		}
	}

	void playerAlly::handleRefused(int target_id)
	{
		UPDATE_OWN();

		for(applicants::iterator iter = _apply_list.begin(); iter != _apply_list.end(); ++iter)
		{
			if(iter->getPlayerId() == target_id && iter->getState() == apply_state::_wait_for_verify)
			{
				iter->setState(apply_state::_refused);
				redPoint();
				helper_mgr.insertSaveAndUpdate(this);
				return;
			}
		}
	}

	void playerAlly::handleInvalid(int target_id)
	{
		UPDATE_OWN();

		for(applicants::iterator iter = _apply_list.begin(); iter != _apply_list.end(); ++iter)
		{
			if(iter->getPlayerId() == target_id)
			{
				iter->setState(apply_state::_invalid);
				redPoint();
				helper_mgr.insertSaveAndUpdate(this);
				return;
			}
		}
	}

	void playerAlly::handleDissolved()
	{
		UPDATE_OWN();

		_allyInfo.setState(apply_state::_dissolved);
		redPoint();
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerAlly::handleApplicant(int type, int target_id, int state)
	{
		switch(state)
		{
		case apply_state::_cancelled:
			handleCancelled(target_id);
			break;
		case apply_state::_have_ally:
			handleHadAlly(target_id);
			break;
		case apply_state::_refused:
			handleRefused(target_id);
			break;
		case apply_state::_invalid:
			handleInvalid(target_id);
			break;
		default:
			break;
		}
	}

	void playerAlly::alterAllyInfo(playerDataPtr targetP)
	{
		if(targetP->playerID != _allyInfo.getPlayerId())
			return;

		_allyInfo.alter(targetP);
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerAlly::getReward(int& get_gold, int& extra_gold)
	{
		UPDATE_OWN();

		if(_reward)
			return _had_reward;

		if(!_bLogins[_other])
			return _op_not_logined;

		_reward = true;
		int all_gold = 0;
		get_gold = ally_sys.getGold();
		extra_gold = 0;
		if(vip_sys.isVipByChargeGold(own.Vip.getRechargeGold()))
			extra_gold += ally_sys.getExtraGold();
		if(vip_sys.isVipByChargeGold(_allyInfo.getChargeGold()))
			extra_gold += ally_sys.getExtraGold();
		all_gold = get_gold + extra_gold;

		own.Base.alterBothGold(all_gold);

		redPoint();
		log(_log_reward, all_gold);
		helper_mgr.insertSaveAndUpdate(this);
		return _success;
	}

	void playerAlly::log(int type, int get_gold)
	{
		vector<string> fields;
		fields.push_back(boost::lexical_cast<string, int>(_allyInfo.getPlayerId()));
		fields.push_back(_allyInfo.getName());
		fields.push_back(boost::lexical_cast<string, int>(_allyInfo.getLv()));

		if(type == _log_reward)
		{
			int lv = vip_sys.getVipLevelByChargeGold(_allyInfo.getChargeGold());
			fields.push_back(boost::lexical_cast<string, int>(lv));
			fields.push_back(boost::lexical_cast<string, int>(get_gold));
		}
		
		StreamLog::Log(allyLogDBStr, own.getOwnDataPtr(), "", "", fields, type);
	}
}

