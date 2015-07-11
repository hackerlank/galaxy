#pragma once
#include "block.h"
#include <map>
#include <list>
#include "reward.h"
#include <bitset>
#include "starwar_report.h"

namespace gg
{
	namespace starwar
	{
		enum{
			// task_type
			_occupy_specified = 1,
			_transfer_times,
			_occupy_times,
			_win_times,
			_lose_times,
			_win_streak,
			_stand_guard,
			_to_different_star,
			_kill_king,
			_exploit_num,
			_silver_num,
			_use_items,
			_task_max,
		};

		class taskInfo
		{
		public:

			void load(Json::Value& info);
			int getId() const { return _id; }
			int getType() const { return _type; }
			int update(playerDataPtr d, vector<int>& args, int temp = -1);
			void getReward(playerDataPtr d);
			double getRate() const { return _rate; }
			int getReward();
		private:
			int _id;
			int _type;
			int _condition1;
			int _condition2;
			reward _reward;
			double _rate;
		};

		typedef boost::shared_ptr<taskInfo> taskPtr;

		const static string starwar_task_id_field_str = "ti";
		const static string starwar_task_state_field_str = "ts";
		const static string starwar_task_args_field_str = "ta";

		class taskRecord
		{
		public:
			taskRecord() : _state(-1){}

			void setValue(const taskPtr& ptr, playerDataPtr d);
			bool setValue(const mongo::BSONElement& obj);
			mongo::BSONObj toBSON();
			void getValue(Json::Value& info, playerDataPtr d);
			int getId() const { return _taskPtr->getId(); }
			int getState() const { return _state; }
			int getType() const { return _taskPtr->getType(); }
			void getReward(playerDataPtr d);
			int getReward();
			void commit();
			bool update(playerDataPtr d, int temp);

		private:
			int _state;
			vector<int> _args;
			taskPtr _taskPtr;
		};

		class action
		{
			public:
				action() : _type(-1){}
				int getType() const { return _type; }
				void setType(int type){ _type = type; }

			private:
				int _type;
		};

		class position
		{
			public:
				position() : _type(-1){}
				position(int type, int id, unsigned time)
					: _type(type), _id(id), _time(time), _from_star_id(-1){}

				int getType() const { return _type; }
				int getId() const { return _id; }
				unsigned getTime() const { return _time; }
				int getFromStarId() const { return _from_star_id; }
				const action& getAction() const { return _action; }
				void setAction(int type){ _action.setType(type); }

				void getValue(Json::Value& info) const
				{
					info.append(_type);
					info.append(_id);
					info.append(_time);
					Json::Value temp;
					for (vector<int>::const_reverse_iterator iter = _path.rbegin(); iter != _path.rend(); ++iter)
						temp.append(*iter);
					info.append(temp);
				}

				void load(const mongo::BSONElement& obj);
				mongo::BSONObj toBSON();

				void setPosition(int type, int id, unsigned time, int from_star_id = -1)
				{
					_type = type;
					_id = id;
					_time = time;
					if (from_star_id != -1)
						_from_star_id = from_star_id;
				}

				void setPath(const vector<int>& paths){ _path = paths; }
				void popPath(){ _path.pop_back(); }
				int frontPath() const{ return _path.back(); }
				int backPath() const { return _path.front(); }
				bool empty() const { return _path.empty(); }
				void clearPath(){ _path.clear(); }
				int sizePath() const { return _path.size(); }

		private:
			int _type;
			int _id;
			unsigned _time;
			int _from_star_id;
			action _action;
			vector<int> _path;
		};
	}

	const static string starwar_rate_field_str = "rt";
	const static string starwar_cd_field_str = "cd";

	class battleParam
	{
	public:
		enum{
			_index_atk = 0,
			_index_def,
			_index_max
		};

		battleParam()
		{
			clear();
		}

		void setValue(const mongo::BSONElement& obj);
		void setValue(double rate, unsigned cd = 0)
		{
			_rate = rate;
			_cd = cd;
		}
		mongo::BSONObj toBSON();

		unsigned getCd() const { return _cd; }
		double getRate(unsigned now = 0)
		{
			if (now != 0 && now >= _cd)
				clear();
			return _rate;
		}
		void clear(){ _rate = 0.0; _cd = 0; }

	private:
		double _rate;
		unsigned _cd;
	};

	const static string starwar_result_field_str = "rs";
	const static string starwar_report_id_field_str = "ri";

	class starwarReport
	{
		public:
			starwarReport(int result, string report_id, string name, int exploit, int supply, int target_supply, int silver, bool is_npc, int title)
				: _result(result), _report_id(report_id), _target_name(name), _exploit(exploit), _supply(supply), _target_supply(target_supply), _silver(silver), _is_npc(is_npc), _title(title){}

			inline starwarReport(const mongo::BSONElement& obj);
			inline mongo::BSONObj toBSON();
			inline void getValue(Json::Value& info) const;

		private:
			int _result;
			string _report_id;
			string _target_name;
			int _exploit;
			int _supply;
			int _target_supply;
			int _silver;
			bool _is_npc;
			int _title;
	};

	
	const static string starwar_id_field_str = "id";
	const static string starwar_from_star_id_field_str = "fsi";
	const static string starwar_action_field_str = "ac";

	class starwarItem
	{
		public:
			enum{
				_empty = -1,
				_add_atk,
				_add_def,
				_sub_supply,
				_add_supply,
				_max_type,

				_init_item_num = 2,
				_max_item_num
			};
			starwarItem() : _type(_empty){}
			inline starwarItem(const mongo::BSONElement& obj);
			void setValue(int type, int lv)
			{
				_type = type;
				_lv = lv;
			}
			bool empty() const { return _type == _empty; }
			int getType() const { return _type; }
			int getLv() const { return _lv; }
			void clear(){ _type = _empty; }
			inline mongo::BSONObj toBSON();
			inline Json::Value getValue();

		private:
			int _type;
			int _lv;
	};

	const static string starwar_db_str = "gl.player_starwar";
	const static string starwar_position_field_str = "pos";
	const static string starwar_report_list_field_str = "rl";
	const static string starwar_supply_cd_field_str = "scd";
	const static string starwar_transfer_times_field_str = "tt";
	const static string starwar_transfer_buy_times_field_str = "tbt";
	const static string starwar_transfer_cd_field_str = "tcd";
	const static string starwar_get_transfer_times_field_str = "gtt";
	const static string starwar_raid_times_field_str = "rt";
	const static string starwar_battle_times_field_str = "bt";
	const static string starwar_resource_list_field_str = "rsl";
	const static string starwar_resource_total_field_str = "rst";
	const static string starwar_max_file_id_field_str = "mfi";
	const static string starwar_exploit_field_str = "ep";
	const static string starwar_items_field_str = "its";
	const static string starwar_buff_field_str = "bf";
	const static string starwar_item_buy_times_field_str = "ibt";
	const static string starwar_atk_buff_field_str = "ab";
	const static string starwar_def_buff_field_str = "db";
	const static string starwar_base_supply_field_str = "bs";
	const static string starwar_next_update_time_field_str = "nut";
	const static string starwar_task_record_field_str = "tr";
	const static string starwar_next_term_time_field_str = "ntt";
	const static string starwar_ranking_reward_field_str = "rr";
	const static string starwar_old_rank_field_str = "ork";
	const static string starwar_old_exploit_field_str = "oe";
	const static string starwar_unity_reward_field_str = "ur";
	const static string starwar_last_unity_time_field_str = "lut";
	const static string starwar_gather_times_field_str = "gt";
	const static string starwar_gather_cd_field_str = "gcd";
	const static string starwar_first_time_field_str = "ft";
	const static string starwar_last_leave_time_field_str = "llt";
	const static string starwar_old_title_field_str = "ottl";
	const static string starwar_settle_npc_num_field_str = "snn";
	const static string starwar_battle_cd_field_str = "bcd";

	typedef vector<starwarReport> starwarReportList;

	class starwarReportPackage
	{
		public:
			starwarReportPackage(int type, int arg, unsigned time, const starwarReportList& rl);
			starwarReportPackage(int type, int arg, unsigned time, const string& name, bool send_back)
				: _type(type), _arg(arg), _time(time), _name(name), _send_back(send_back){}
			inline starwarReportPackage(const mongo::BSONElement& obj);
			inline mongo::BSONObj toBSON();
			inline void getValue(Json::Value& info) const;

		private:
			int _type;
			unsigned _time;
			int _arg;
			string _name;
			bool _send_back;
			starwarReportList _reportList;
	};

	typedef list<starwarReportPackage> starwarReportPackageList;
		
	const static string starwarLogDBStr = "log_starwar";

#define starwarLog(player, tag, ...)\
	LogTemplate(starwarLogDBStr, player, tag, __VA_ARGS__)       // __VA_ARGS__ = preV, nowV, f1, f2, ... 

	class playerStarwar : public Block
	{
		public:
			enum{
				_open_item_cost = 100,
				_flush_item_cost = 10,
				_transfer_item_id = 13002,
			};


			enum{
				_log_get_exploit = 1,
				_log_flush_exploit,
				_log_ranking_reward,
				_log_task_reward,
				_log_supply_cost,
				_log_item,
				_log_unity,
				_log_great_event,
				_log_move,
				_log_occupy,

				_log_position = 100,

				_log_supply_cost_type_item = 0,
				_log_supply_cost_type_battle,
			};

			playerStarwar(playerData& own);
			virtual bool get();
			virtual bool save();
			virtual void autoUpdate();

			void checkAndUpdate(unsigned now);

			void updateData();

			void update();
			void initData();
			void package(Json::Value& msg);

			bool toAddCache();
			void packageReport(Json::Value& msg);
			void addReport(const starwar::reportPtr& ptr, bool update = true);
			void addCacheReport(const starwar::reportPtr& ptr);
			const starwar::reportList& getCacheReport() const { return _cacheRepList; }

			void resetPosition(unsigned now, bool init = false);
			void setPosition(int type, int id, unsigned now, bool next = false);
			const starwar::position& getPosition() const { return _position; }
			starwar::position& getPosition() { return _position; }
			void setAction(int type){ _position.setAction(type); }

			int getResource(int star_id = -1);
			int received(int star_id);
			void packageResource(Json::Value& msg);

			string getFileName();
			unsigned getExploit() const { return _exploit; }
			int alterExploit(unsigned num);
			void clearExploit();

			void onMain(bool flag, bool offline = false);
			bool onMain() const { return _on_main; }

			int isSettleNpcLimit(int num);
			int getSettleNpcNum(int title) const;
			int getSettleNpcNum();

			void settleNpc(int num);

			double getAtkBuff(unsigned now);
			double getDefBuff(unsigned now);

			int useItem(int position, int star_id, int player_id, playerDataPtr targetP = playerDataPtr());
			int useItem(int position);
			int openItem();
			int flushItem();
			int buyItem(int position);

			int getSupply(unsigned now);
			void calculateSupply(unsigned now);
			void alterSupply(int num, unsigned now, int type = _log_supply_cost_type_battle);

			void getTaskInfo(Json::Value& info);
			int taskCommit(int task_id);
			void updateTask();
			void updateTask(int type, int temp = 1);
			void resetTask();

			int buyTransferTimes(int num);
			int clearTransferCd();
			void useTransferTimes(int num);
			int isTransferLimit(unsigned now, int num);

			int getRankingReward();
			int gather(unsigned now);

			int isRaidLimit(unsigned now);
			void doRaid(unsigned now);

			unsigned getFirstTime() const { return _first_time; }

			void playerLogin();

			void addBattleTimes();
			void battleCdEffect(unsigned now);
			bool inBattleCd(unsigned now) const { return now < _battle_cd; }

		private:
			inline void doAlterSup(unsigned now, int sup);
			inline void recalSup(unsigned now);
			inline void adjustSup();

		private:
			starwar::position _position;
			bool _on_main;
			vector<unsigned> _resource;
			vector<starwarItem> _items;

			starwarReportPackageList _reportPackages;
			unsigned _max_file_id;

			unsigned _exploit;

			int _supply;
			int _base_supply;
			unsigned _supply_cd;

			unsigned _transfer_times;
			unsigned _transfer_buy_times;
			unsigned _transfer_cd;
			unsigned _get_transfer_times;

			unsigned _raid_times;
			unsigned _battle_times;
			unsigned _battle_cd;

			unsigned _item_buy_times;
			unsigned _next_update_time;
			unsigned _next_term_time;
			bool _ranking_reward;
			bool _unity_reward;
			unsigned _last_unity_time;

			unsigned _gather_times;
			unsigned _gather_cd;

			battleParam _buff[battleParam::_index_max];
			vector<starwar::taskRecord> _taskRecord; 
			bitset<starwar::_task_max> _taskTypes;

			starwarReportList _reportCache;
			unsigned _first_time;
			unsigned _last_leave_time;

			int _settle_npc_num;
			int _old_title;

			starwar::reportList _cacheRepList;
			starwar::reportManager _repManager;
	};
}