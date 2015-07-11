#include "sign_system.h"
#include "response_def.h"
#include "file_system.h"
#include "action_def.h"
#include "record_system.h"


namespace gg
{
	const string sign_system::signDayRewardDir = "./instance/sign/login_gift_reward_normal.json";
	const string sign_system::signTotalRewardDir = "./instance/sign/login_gift_reward_special.json";
	sign_system* const sign_system::signSys = new sign_system();

	bool sign_reward::setValue(Json::Value& var)
	{
		if(!_reward.setValue(var))
			return false;
		if(!_is_first)
		{
			for(vector<rewardItem>::const_iterator iter = _reward.getItems().begin(); iter != _reward.getItems().end(); ++iter)
			{
				if(iter->getType() == action_add_silver || iter->getType() == action_add_keji)
					_have_rate = true;
			}
		}
		return true;
	}

	reward sign_reward::getSignReward(playerDataPtr d)
	{
		if(!_have_rate)
			return _reward;
		else
		{
			reward new_reward;
			for(vector<rewardItem>::const_iterator iter = _reward.getItems().begin(); iter != _reward.getItems().end(); ++iter)
			{
				if(iter->getType() == action_add_silver || iter->getType() == action_add_keji || iter->getType() == action_add_weiwang)
				{
					rewardItem item;
					item.setValue(iter->getType(), iter->getId(), iter->getNum() * d->Base.getLevel());
					new_reward.addRewardItem(item);
				}
				else
					new_reward.addRewardItem(*iter);
			}
			return new_reward;
		}
	}

	void sign_system::loadFile()
	{
		Json::Value dayReward = na::file_system::load_jsonfile_val(signDayRewardDir);

		Json::Value& reward1 = dayReward["rewards1"];

		for(unsigned i = 0; i < reward1.size(); ++i)
		{
			sign_reward one_reward(true);
			one_reward.setValue(reward1[i]);
			_rewards_day[_first_time].push_back(one_reward);
		}

		Json::Value& reward2 = dayReward["rewards2"];

		for(unsigned i = 0; i < reward2.size(); ++i)
		{
			sign_reward one_reward(false);
			one_reward.setValue(reward2[i]);
			_rewards_day[_cycle_time].push_back(one_reward);
		}

		Json::Value totalReward = na::file_system::load_jsonfile_val(signTotalRewardDir); 

		Json::Value& reward3 = totalReward["rewards1"];
		Json::Value& day3 = totalReward["login_number1"];

		for(unsigned i = 0; i < reward3.size(); ++i)
		{
			sign_reward one_reward(true, day3[i].asInt());
			one_reward.setValue(reward3[i]);
			_rewards_total[_first_time].push_back(one_reward);
		}

		Json::Value& reward4 = totalReward["rewards2"];
		Json::Value& day4 = totalReward["login_number2"];

		for(unsigned i = 0; i < reward4.size(); ++i)
		{
			sign_reward one_reward(false, day4[i].asInt());
			one_reward.setValue(reward4[i]);
			_rewards_total[_cycle_time].push_back(one_reward);
		}

		if(reward1.size() != playerSign::_cycle_days
			|| reward2.size() != playerSign::_cycle_days
			|| reward3.size() != playerSign::_total_rewards
			|| reward4.size() != playerSign::_total_rewards)
			rewardItem::throwSegFault();
	}

	void sign_system::initData()
	{
		db_mgr.ensure_index(player_sign_db_str, BSON(playerIDStr << 1));

		loadFile();
	}

	void sign_system::signUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Sign.update();
	}

	void sign_system::signRewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		UnAcceptRetrun(js_msg[0u].isInt(), -1);
		UnAcceptRetrun(js_msg[1u].isInt(), -1);

		int type = js_msg[0u].asInt();      // type = 0为每日奖励, type = 1为累积奖励
		int index = js_msg[1u].asInt();    // 第几份奖励(1-28)或(1-4)

		if((type == 0 && (index < 1 || index > playerSign::_cycle_days))
			|| (type == 1 && (index < 1 || index > playerSign::_total_rewards)))
			Return(r, 1);

		if((type == 0 && index > d->Sign.getTotalDays())
			|| (type == 1 && _rewards_total[!d->Sign.isFirst()][index - 1].getDay() > d->Sign.getTotalDays()))
			Return(r, 2);

		if(!d->Sign.checkRecord(type, index))
			Return(r, 3);
	
		vector<int> before;

		reward my_reward = type == 0? 
			_rewards_day[!d->Sign.isFirst()][index - 1].getSignReward(d) : _rewards_total[!d->Sign.isFirst()][index - 1].getSignReward(d);

		const vector<rewardItem>& rewards = my_reward.getItems();

		for(vector<rewardItem>::const_iterator iter = rewards.begin(); iter != rewards.end(); ++iter)
			before.push_back(rewardItem::getItemNum(d, iter->getType(), iter->getId()));

		if (my_reward.getRewardWithMail(d, email_team_system_bag_full_daily_login))
			r[msgStr][1u] = my_reward.getEmailValue();

		int i = 0;
		for(vector<rewardItem>::const_iterator iter = rewards.begin(); iter != rewards.end(); ++iter)
		{
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(index));
			fields.push_back(boost::lexical_cast<string, int>(d->Sign.getTotalDays()));
			int item_type = iter->getType();
			fields.push_back(boost::lexical_cast<string, int>(item_type));
			if(item_type == action_add_item)
			{
				fields.push_back(boost::lexical_cast<string, int>(iter->getId()));
				fields.push_back(boost::lexical_cast<string, int>(iter->getNum()));
			}
			else if(item_type == action_active_pilot)
				fields.push_back(boost::lexical_cast<string, int>(iter->getId()));
			else
				fields.push_back(boost::lexical_cast<string, int>(iter->getNum()));
			StreamLog::Log(signLogDBStr, d, boost::lexical_cast<string, int>(before[i]), boost::lexical_cast<string, int>(rewardItem::getItemNum(d, iter->getType(), iter->getId()))
					, fields, type);
			++i;
		}

		d->Sign.alterData(type, index);
		d->Sign.update();

		Return(r, 0);
	}
}