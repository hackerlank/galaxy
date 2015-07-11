#include "reward.h"
#include "action_def.h"
#include "player_data.h"
#include "email_system.h"
#include "item_system.h"
#include "pilotManager.h"

namespace gg
{
	bool rewardItem::throwSegFault()
	{
		LogE << "rewardItem setValue error" << LogEnd;
		char* ptr = NULL;
		*ptr = 0;
		return false;
	}

	int rewardItem::getItemNum(playerDataPtr d, int type, int id)
	{
		switch(type)
		{
		case action_add_silver:
		case action_add_silver_vip_resource:
			return d->Base.getSilver();
		case action_add_gold_ticket:
			return d->Base.getGoldTicket();
		case action_add_gold:
			return d->Base.getGold();
		case action_add_keji:
		case action_add_keji_vip_resource:
			return d->Base.getKeJi();
		case action_add_role_exp:
			return d->Base.getExp();
		case action_add_item:
			return d->Item.getItemStack(id);
		case action_add_work_times:
			return d->Workshop.getPresentationCount();
		case action_add_weiwang:
		case action_add_weiwang_vip_resource:
			return d->Base.getWeiWang();
		case action_active_pilot:
			return d->Pilots.checkPilotActive(id)? 1 : 0;
		case action_add_junling:
			return d->Campaign.getJunling();
		case action_add_gold_vip_resource:
			return d->Base.getAllGold();
		case action_add_paper:
			return d->Paper.getToken();
		default:
			return -1;
		}
	}

	Json::Value rewardItem::getValue() const
	{
		Json::Value ret;
		ret[reward_action_id_field_str] = _type;
		if(_type == action_add_item)
		{
			ret[reward_item_id_field_str] = _id;
			ret[reward_add_num_field_str] = _num;
		}
		else if(_type == action_active_pilot)
			ret[reward_value_field_str] = _id;
		else
			ret[reward_value_field_str] = _num;
		return ret;
	}

	bool rewardItem::setValue(int type, int id, int num)
	{
		_type = type;
		_id = id;
		_num = num;
		return checkValid() || throwSegFault();
	}

	void rewardItem::setFromArray(const Json::Value& var)
	{
		_type = var[0u].asInt();
		if(_type == action_add_item)
		{
			_id = var[1u].asInt();
			_num = var[2u].asInt();
		}
		else if(_type == action_active_pilot)
		{
			_id = var[1u].asInt();
			_num = 1;
		}
		else
			_num = var[1u].asInt();
	}

	void rewardItem::setFromObject(const Json::Value& var)
	{
		_type = var[reward_action_id_field_str].asInt();
		if(_type == action_add_item)
		{
			_id = var[reward_item_id_field_str].asInt();
			_num = var[reward_add_num_field_str].asInt();
		}
		else if(_type == action_active_pilot)
		{
			_id = var[reward_value_field_str].asInt();
			_num = 1;
		}
		else
			_num = var[reward_value_field_str].asInt();
	}

	bool rewardItem::setValue(const Json::Value& var)
	{
		if(var.isArray())
			setFromArray(var);
		else
			setFromObject(var);
		return checkValid() || throwSegFault();
	}

	bool rewardItem::checkValid()
	{
		switch(_type)
		{
		case action_add_silver:
		case action_add_gold_ticket:
		case action_add_gold:
		case action_add_keji:
		case action_add_role_exp:
		case action_add_work_times:
		case action_add_weiwang:
		case action_add_junling:
		case action_add_silver_vip_resource:
		case action_add_gold_vip_resource:
		case action_add_keji_vip_resource:
		case action_add_weiwang_vip_resource:
		case action_add_paper:
			return _num > 0;
		case action_add_item:
			return _num > 0 && !item_sys.getConfig(_id).isEmpty();
		case action_active_pilot:
			return pilot_sys.getPilotConfig(_id).isVaild();
		default:
			return false;
		}
	}

	int rewardItem::getReward(playerDataPtr d)
	{
		switch(_type)
		{
		case action_add_silver:
			d->Base.alterSilver(_num); break;
		case action_add_gold_ticket:
			d->Base.alterGoldTicket(_num); break;
		case action_add_gold:
			d->Base.alterGold(_num); break;
		case action_add_keji:
			d->Base.alterKeJi(_num); break;
		case action_add_role_exp:
			d->Base.addExp(_num); break;
		case action_add_work_times:
			d->Workshop.addPresentation(_num); break;
		case action_add_weiwang:
			d->Base.alterWeiWang(_num); break;
		case action_add_item:
			return !d->Item.addItem(_id, _num).empty()? _success : _error_package_full;
		case action_active_pilot:
			return d->Pilots.activePilot(_id) == 0? _success : _error_pilot;
		case action_add_junling:
			d->Campaign.alterJunling(_num);break;
		case action_add_silver_vip_resource:
			d->Base.alterSilver(_num * d->Base.getLevel()); break;
		case action_add_gold_vip_resource:
			d->Base.alterBothGold(_num * d->Base.getLevel()); break;
		case action_add_keji_vip_resource:
			d->Base.alterKeJi(_num * d->Base.getLevel()); break;
		case action_add_weiwang_vip_resource:
			d->Base.alterWeiWang(_num * d->Base.getLevel()); break;
		case action_add_paper:
			d->Paper.alterToken(_num); break;
		default:
			return _error_max;
		}
		return _success;
	}

	Json::Value reward::_email_value = Json::nullValue;

	Json::Value reward::getValue() const
	{
		Json::Value ret = Json::arrayValue;
		for(vector<rewardItem>::const_iterator iter = _items.begin(); iter != _items.end(); ++iter)
		{
			ret.append(iter->getValue());
		}
		return ret;
	}

	bool reward::getRewardWithMail(playerDataPtr d, int type)
	{
		vector<rewardItem> unget_items;
		for(vector<rewardItem>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
		{
			if(iter->getReward(d) == rewardItem::_error_package_full)
				unget_items.push_back(*iter);
		}
		_email_value = Json::arrayValue;
		for(size_t i = 0; i < unget_items.size(); ++i)
		{
			Json::Value item;
			item[email::actionID] = action_add_item;
			item[email::itemID] = unget_items[i].getId();
			item[email::addNum] = unget_items[i].getNum();
			_email_value.append(item);
		}
		if (unget_items.size() > 0)
		{
			email_sys.sendSystemEmailCommon(d, email_type_system_attachment, (emailTeam)type, Json::Value::null, _email_value);
			return true;
		}
		return false;
	}

	int reward::getItemNum(int type, int id /* = -1 */)
	{
		int num = 0;
		for(vector<rewardItem>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
		{
			if(iter->getType() == type)
				num += iter->getNum();
		}
		return num;
	}

	bool reward::setValue(const Json::Value& var)
	{
		if(!var.isArray() || var.size() < 1)
			return rewardItem::throwSegFault();

		for(unsigned i = 0; i < var.size(); ++i)
		{
			rewardItem it;
			it.setValue(var[i]);
			_items.push_back(it);
		}
		return true;
	}

	void reward::addRewardItem(const rewardItem& item)
	{
		_items.push_back(item);
	}
}