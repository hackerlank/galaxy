#pragma once
#include "block.h"

namespace gg
{
	namespace apply_state
	{
		enum{
			// 申请状态
			_wait_for_verify = 0,             // 等待验证
			_have_ally,                              // 已有战友
			_refused,                                // 被拒绝
			_invalid,                                  // 已失效

			// 被申请状态
			_wait_for_deal,                     // 等待处理
			_cancelled,                            // 被撤销

			// 战友状态
			_normal,                                  // 正常状态
			_agreed,                                 // 被同意
			_dissolved,                            //  被解散
			_backup,                                //  备份数据
		};
	}

	const static string ally_player_id_field_str = "pi";
	const static string ally_nickname_field_str = "nn";
	const static string ally_level_field_str = "lv";
	const static string ally_face_id_field_str = "fi";

	class allyBaseInfo
	{
	public:
		allyBaseInfo() : _player_id(-1){}
		allyBaseInfo(playerDataPtr d);
		bool empty(){ return _player_id == -1; }
		void clear(){ _player_id = -1; }

		int getPlayerId() const { return _player_id; }
		string getName() const { return _nickname; }
		int getLv() const { return _lv; }

		void package(Json::Value& info)
		{
			info.append(_player_id);
			info.append(_nickname);
			info.append(_lv);
			info.append(_face_id);
		}

		void toBSONBuilder(mongo::BSONObjBuilder& b)
		{
			if(_player_id == -1)
				b << ally_player_id_field_str << _player_id;
			else
				b << ally_player_id_field_str << _player_id
					<< ally_nickname_field_str << _nickname
					<< ally_level_field_str << _lv
					<< ally_face_id_field_str << _face_id;
		}

		template<typename T>
		void setFromBSON(const T& obj)
		{
			if(obj[ally_player_id_field_str].eoo())
				return;
			checkNotEoo(obj[ally_player_id_field_str])
				_player_id = obj[ally_player_id_field_str].Int();
			checkNotEoo(obj[ally_nickname_field_str])
				_nickname = obj[ally_nickname_field_str].String();
			checkNotEoo(obj[ally_level_field_str])
				_lv = obj[ally_level_field_str].Int();
			checkNotEoo(obj[ally_face_id_field_str])
				_face_id = obj[ally_face_id_field_str].Int();
		}
		
		void alter(playerDataPtr d);

	private:
		int _player_id;
		string _nickname;
		int _lv;
		int _face_id;
	};

	const static string ally_state_field_str = "st";

	class applicant : public allyBaseInfo
	{
	public:
		applicant(){}
		applicant(playerDataPtr d, int state);

		bool empty(){ return allyBaseInfo::empty() || _state == apply_state::_dissolved || _state == apply_state::_backup; }

		void toBSONBuilder(mongo::BSONObjBuilder& b)
		{
			allyBaseInfo::toBSONBuilder(b);
			b << ally_state_field_str << _state;
		}

		template<typename T>
		void setFromBSON(const T& obj)
		{
			allyBaseInfo::setFromBSON(obj);
			checkNotEoo(obj[ally_state_field_str])
				_state = obj[ally_state_field_str].Int();
		}

		void package(Json::Value& info)
		{
			allyBaseInfo::package(info);
			info.append(_state);
		}

		void setState(int state){ _state = state; }
		int getState() const { return _state; }
	private:
		int _state;
	};

	const static string ally_charge_gold_field_str = "cg";

	class allyInfo : public applicant
	{
	public:
		allyInfo() : _charge_gold(0){}
		allyInfo(playerDataPtr d, int state);

		void toBSONBuilder(mongo::BSONObjBuilder& b)
		{
			applicant::toBSONBuilder(b);
			b << ally_charge_gold_field_str << _charge_gold;
		}

		template<typename T>
		void setFromBSON(const T& obj)
		{
			applicant::setFromBSON(obj);
			checkNotEoo(obj[ally_charge_gold_field_str])
				_charge_gold = obj[ally_charge_gold_field_str].Int();
		}

		void package(Json::Value& info);
		void alter(playerDataPtr targetP);
		int getChargeGold() const { return _charge_gold; }

	private:
		int _charge_gold;
	};

	typedef std::list<applicant> applicants;

	const static string allyLogDBStr = "log_ally";

	const static string allyDbStr = "gl.player_ally";

	const static string ally_next_update_time_field_str = "nt";
	const static string ally_times_field_str = "tm";
	const static string ally_had_allys_field_str = "ha";
	const static string ally_reward_field_str = "rw";
	const static string ally_ally_info_field_str = "ai";
	const static string ally_apply_list_field_str = "ayl";
	const static string ally_applied_list_field_str = "adl";
	const static string ally_bLogin_own_field_str = "ow";
	const static string ally_bLogin_other_field_str = "ot";
	const static string ally_active_field_str = "at";
	const static string ally_reward_info_field_str = "ri";
	const static string ally_new_applied_field_str = "nl";
	const static string ally_is_first_enter_field_str = "ife";


	class playerAlly : public Block
	{
	public:
		enum{
			_success = 0,

			// send
			_times_limit = 1,                 // 申请次数限制
			_to_same_person,              // 当天已向该玩家申请
			_had_ally,                             // 当天有或有过战友
			_op_had_ally,                      // 对方已有战友

			// cancel, refuse, agree, del
			_not_in_list = 1,                  // 没在列表中

			// dissolve
			_not_had_ally = 1,             //  没战友

			// reward
			_had_reward = 1,
			_op_not_logined,

			_apply_times = 5,              // 每天最多可申请次数

			_apply = 0,
			_applied,

			_own = 0,
			_other,
			_side_max,

			_default = 0,
			_login,
			_logout,

			_log_agree = 0,
			_log_dissolve,
			_log_reward
		};

		playerAlly(playerData& own);
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate();

		void package(Json::Value& info);
		void update();                                                          // 主界面更新

		int send(playerDataPtr targetP);                          // 发送申请
		int cancel(playerDataPtr targetP);                       // 取消申请

		int agree(playerDataPtr targetP);                        // 同意申请
		int refuse(playerDataPtr targetP);                       // 拒绝申请

		int dissolve(playerDataPtr targetP);                   // 解散战友
		int del(int type, int target_id);                             // 删除消息

		int getReward(int& get_gold, int& extra_gold);                                                     // 领取奖励

		void handleApplicant(int type, int target_id, int state);

		void checkAndUpdate(unsigned now);
		void playerLogin();
		void playerLogout();

		bool getbLogin(int side){ return _bLogins[side]; }
		void setbLogin(int side, bool flag);

		allyInfo& getAllyInfo(){ return _allyInfo; }
		void alterAllyInfo(playerDataPtr targetP);

		bool actived(){ return _active; }
		void active(bool flag);

		void setFlag(bool flag);

	private:
		bool getRewardState();
		void resetbLogin(int state);

		bool hadAlly(){ return _had_allys; }
		void setAllyInfo(const allyInfo& info);

		void addApplicant(const applicant& a, int type);
		void clearApplicant();

		void redPoint();
		void redPoint(bool flag);

		void postToActiveReward();
		void postToUpdateState();
		void postToUpdateState(int type, int target_id, int state);

		void handleCancelled(int target_id);
		void handleHadAlly(int target_id);
		void handleRefused(int target_id);
		void handleDissolved();
		void handleInvalid(int target_id);
		bool checkInApplyList(int target_id);

		void log(int type, int get_gold = 0);

	private:
		unsigned _next_update_time;
		int _times;
		bool _bLogins[_side_max];
		bool _reward;
		bool _had_allys;
		allyInfo _allyInfo;
		applicants _apply_list;
		applicants _applied_list;

		bool _active;            // 激活盟友奖励

		bool _new_applied;
		bool _rp_state;        // 红点状态
		bool _rp_valid;
		
		bool _is_first;
	};
}

