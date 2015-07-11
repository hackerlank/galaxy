#pragma once

#include "json/json.h"
#include "block.h"
#include <string>
#include "player_email.h"

namespace gg
{
	const static string reward_action_id_field_str = "aid";
	const static string reward_item_id_field_str = "iid";
	const static string reward_add_num_field_str = "an";
	const static string reward_value_field_str = "v";

	class rewardItem
	{
	public:
		enum{
			_success = 0,
			_error_pilot,
			_error_package_full,

			_error_max
		};

		rewardItem() : _type(-1), _id(-1), _num(-1){}

		bool setValue(int type, int id, int num);
		bool setValue(const Json::Value& var);
		Json::Value getValue() const;

		int getReward(playerDataPtr d);

		int getType() const { return _type; }
		int getNum() const { return _num; }
		int getId() const { return _id; }

		static int getItemNum(playerDataPtr d, int type, int id = -1);

		static bool throwSegFault();

	private:
		bool checkValid();
		void setFromObject(const Json::Value& var);
		void setFromArray(const Json::Value& var);

		int _type;
		int _num;
		int _id;
	};

	class reward
	{
	public:
		bool getRewardWithMail(playerDataPtr d, int type = email_team_system_bag_full);

		int getItemNum(int type, int id = -1);

		bool setValue(const Json::Value& var);
		const vector<rewardItem>& getItems() const { return _items; }
		Json::Value getValue() const;

		void addRewardItem(const rewardItem& item);
		static Json::Value getEmailValue(){ return _email_value; }

	private:
		vector<rewardItem> _items;
		static Json::Value _email_value;
	};
}