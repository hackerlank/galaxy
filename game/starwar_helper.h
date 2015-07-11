#pragma once

#include <vector>
#include <list>
#include <set>
#include <boost/shared_ptr.hpp>
#include <json/json.h>
#include "mongoDB.h"
#include <boost/unordered_map.hpp>
//#include "battle_def.h"
#include "starwar_base.h"
#include "playerManager.h"
#include "ruler_helper.h"
using namespace std;

namespace gg
{
	namespace starwar
	{
		enum{
			// error_code
			_success = 0,
			_server_error,
			_client_error,
			_over_time,
			_async_throw,
			_no_country,
			_id_not_found,
			_not_in_star,
			_not_in_path,
			_not_access,
			_in_cd,
			_not_in_cd,
			_gold_not_enough,
			_times_limit,
			_no_items,
			_same_buff,
			_task_error,
			_no_reward,
			_not_war_time,
			_not_prime_time,
			_not_king,
			_not_gather,
			_lv_limit,
			_supply_not_enough,
			_supply_full,
			_your_own_star,
			_no_ememy,
			_items_full,
			_s_no_title,
			_num_limit,

			// position
			_in_path = 0,
			_in_star,

			// action_type
			_move = 0,
			_transfer,
			_raid,
			_auto_move,
			_auto_transfer,
			_king_gather,
			_officer_gather,
			_use_item,
			_settle_npc,

			// event_type
			_not_set = 0,
			_arrive,
			_attack,
			_meet,
			_war_time,
			_truce_time,
			_prime_time,
			_ordinary_time,
			_resource,
			_ranking,
			_unity,
			_prime_time_coming,
			_prime_time_begin,
			_load_player,
			_prime_time_running,
			_prime_time_summary,
			_clear_sign_info,

			_test_bug = 100,

			// guild_type
			_player = 0,
			_npc,
			_player_npc,
			_key,

			// params
			_default_speed = 10,
			_max_server_rank = 500,
			_max_client_rank = 50,
			_max_package_size = 200,

			// buff_type
			_atk_buff = 1,
			_def_buff,
			_supply_effect,

			// task_state
			_task_running = 0,
			_task_finished,
			_task_commit,

			// index_buff
			_index_star_supply = 0,
			_index_star_gold,
			_index_star_atk,
			_index_star_def,
			_index_item_atk,
			_index_item_def,

			// index_db
			_resource_index = 0,
			_ranking_index,
			_unity_check_index,
			_unity_time_index,
			_unity_reward_index,
			_gather_index,
			_last_war_time_index,
			_last_prime_time_index,
			_great_event_index,
			_sign_info_index,
			_index_max,


			// report_type
			_report_type_0 = 0,
			_report_type_1,
			_report_type_2,
			_report_type_3,
			_report_type_4,

			// battle_reward
			_win_silver = 1000,
			_lose_silver = 700,

			// sign_type
			_sign_attack = 0,
			_sign_defence,
		};

		union effect{
			effect(){}
			effect(unsigned num) : _num(num){}
			effect(double rate) : _rate(rate){}
			double _rate;
			unsigned _num;
		};

		class message
		{
		public:
			message(int type, const Json::Value& args, unsigned time, playerDataPtr d)
				: _type(type), _args(args), _time(time), _ptr(d){}

			int getType() const { return _type; }
			playerDataPtr getPlayerPtr() const { return _ptr; }
			unsigned getTime() const { return _time; }
			const Json::Value& getArgs() const { return _args; }

		private:
			int _type;
			unsigned _time;
			playerDataPtr _ptr;
			Json::Value _args;
		};

		typedef list<message> msgQueue;

		const static string starwar_player_id_field_str = "pi";
		const static string starwar_name_field_str = "nm";
		const static string starwar_country_id_field_str = "ci";
		const static string starwar_level_field_str = "lv";
		const static string starwar_face_id_field_str = "fi";
		const static string starwar_title_field_str = "tt";

		class playerInfo
		{
		public:
			playerInfo() : _player_id(-1){}
			playerInfo(playerDataPtr d, unsigned now)
				: _player_id(d->playerID), _name(d->Base.getName()), _country_id(d->Base.getSphereID()), _lv(d->Base.getLevel())
				, _face_id(d->Base.getFaceID()), _title(d->Ruler.getTitle()), _supply(d->Starwar.getSupply(now)){}

			playerInfo(int player_id, const string& name, int country_id, int face_id, int lv, int supply, int title = -1)
				: _player_id(player_id), _name(name), _country_id(country_id), _face_id(face_id), _lv(lv), _supply(supply), _title(title){}

			playerInfo(const mongo::BSONElement& obj);
			inline mongo::BSONObj toBSON();

			int getPlayerId() const { return _player_id; }
			string getName() const { return _name; }
			int getCountryId() const { return _country_id; }
			int getFaceId() const { return _face_id; }
			int getLv() const { return _lv; }
			int getTitle() const { return _title; }
			int getSupply() const { return _supply; }

			void alterSupply(int sup) const { _supply = sup; }

		private:
			int _player_id;
			string _name;
			int _country_id;
			int _face_id;
			int _lv;
			int _title;
			mutable int _supply;
		};

		const static string starwar_player_info_field_str = "pi";
		const static string starwar_type_field_str = "ty";
		const static string starwar_time_field_str = "tm";
		const static string starwar_side_field_str = "sd";
		const static string starwar_distance_field_str = "dt";

		class pathItem
		{
		public:
			pathItem() : _type(-1), _trigger_time(-1){}
			pathItem(int type, unsigned now, playerDataPtr d, int side)
				: _type(type), _time(now), _playerInfo(d, now), _side(side), _distance(0), _trigger_time(-1){}
			pathItem(const playerInfo& player_info, int type, int side, unsigned now)
				: _playerInfo(player_info), _type(type), _side(side), _time(now), _distance(0), _trigger_time(-1){}
			pathItem(const mongo::BSONElement& obj);
			inline mongo::BSONObj toBSON();

			const playerInfo& getPlayerInfo() const { return _playerInfo; }
			int getType() const { return _type; }
			int getSide() const { return _side; }
			unsigned getTime() const { return _time; }
			int getDistance() const { return _distance; }
			int getTriggerTime() const { return _trigger_time; }

			void setDistance(int distance){ _distance = distance; }
			void setTime(unsigned time){ _time = time; }
			void setTriggerTime(int time){ _trigger_time = time; }

			inline void getValue(Json::Value& info);
			inline void getExtraValue(Json::Value& info);

			void alterSupply(int sup){ _playerInfo.alterSupply(sup); }

		private:
			int _type;
			int _side;
			unsigned _time;
			playerInfo _playerInfo;
			int _distance;
			int _trigger_time;
		};

		class cacheInfo
		{
		public:
			void setValue(int type, unsigned time, const playerInfo& player_info, int from_star_id)
			{
				_type = type;
				_time = time;
				_playerInfo = player_info;
				_from_star_id = from_star_id;
				_target_country = -1;
				_different_country = false;
			}
			void clear(){ _type = -1; }

			int getType() const { return _type; }
			int getFromStarId() const { return _from_star_id; }
			unsigned getTime() const { return _time; }
			const playerInfo& getPlayerInfo() const { return _playerInfo; }

			void alterSupply(int sup){ _playerInfo.alterSupply(sup); }

			int getTargetCountry(int current_country)
			{
				if (_different_country)
					return -1;
				else
				{
					if (_target_country == -1)
					{
						_target_country = current_country;
						return _target_country;
					}
					else
					{
						if (_target_country == current_country)
							return _target_country;
						else
						{
							_different_country = true;
							return -1;
						}
					}
				}
			}

		private:
			int _type;
			int _from_star_id;
			unsigned _time;
			playerInfo _playerInfo;
			int _target_country;
			bool _different_country;
		};

		class triggerItem
		{
		public:
			triggerItem() : _code(_not_set){}
			triggerItem(int code, int id, unsigned time) : _code(code), _id(id), _time(time){}
			void setValue(int code, int id, unsigned time)
			{
				_code = code;
				_id = id;
				_time = time;
			}
			bool operator==(const triggerItem& rhs) const
			{
				return _code == rhs.getCode()
					&& _id == rhs.getId()
					&& _time == rhs.getTime();
			}
			bool operator<(const triggerItem& rhs) const
			{
				return _time < rhs.getTime();
			}
			bool empty() const { return _code == _not_set; }
			void clear(){ _code = _not_set; }
			int getCode() const { return _code; }
			int getId() const { return _id; }
			unsigned getTime() const { return _time; }

		private:
			int _code;
			int _id;
			unsigned _time;
		};
		typedef trigger<triggerItem> starwarTrigger;

		class updateItem
		{
		public:
			enum{
				_enter = 0,
				_arrive,
				_attack,
				_transfer,
			};

			updateItem(int type, int player_id, int country_id, unsigned time, int distance, int title, int side, const string& name)
				: _type(type), _player_id(player_id), _country_id(country_id), _time(time), _distance(distance), _title(title), _side(side), _name(name){}

			inline void getValue(Json::Value& info);
			int getSide() const { return _side; }

		private:
			int _type;
			int _player_id;
			int _country_id;
			unsigned _time;
			int _distance;
			int _title;
			int _side;
			string _name;
		};

		typedef vector<updateItem> updateItems;

		typedef list<pathItem> pathItemList;
		typedef pathItemList::iterator pLIter;

		class pathSortHelper
		{
		public:
			class sortIter
			{
			public:
				sortIter(pLIter iter)
					: _iter(iter){}
				bool operator<(const sortIter& rhs) const { return _iter->getTriggerTime() < rhs._iter->getTriggerTime(); }
				bool operator==(const sortIter& rhs) const { return _iter == rhs._iter; }
				pLIter _iter;
			};

			bool getMin(pLIter& iter)
			{
				if (_sortList.empty())
					return false;
				iter = _sortList.begin()->_iter;
				return true;
			}
			void push(const pLIter& iter)
			{
				_sortList.insert(sortIter(iter));
			}
			void pop(const pLIter& iter)
			{
				typedef sortList::iterator sortListIter;
				sortIter target(iter);
				pair<sortListIter, sortListIter> pair_iter = _sortList.equal_range(target);
				sortListIter bIter = pair_iter.first;
				for (; bIter != pair_iter.second; ++bIter)
				{
					if (*bIter == target)
					{
						_sortList.erase(bIter);
						break;
					}
				}
			}
			void clear(){ _sortList.clear(); }
			bool empty() const { return _sortList.empty(); }

		private:
			typedef multiset<sortIter> sortList;
			sortList _sortList;
		};

		class pathItemMgr
		{
		public:
			typedef boost::function<void(const pathItem&)> handler;

			pathItemMgr(int path_id);

			int init(int side, int cost_time);
			void reset(int rate, unsigned now);

			void clear();

			unsigned size() const { return _playerList.size() - 2; }
			void push(const pathItem& item);
			void pushMove(const pathItem& item);
			void pushRaid(const pathItem& item);
			void kickPlayer(int player_id);
			void pop(const pLIter& iter, int type); 
			void swap(pLIter& left_iter, pLIter& right_iter);
			bool getMin(pLIter& iter);
			pLIter getNext(const pLIter& iter);
			pLIter getLeft(const pLIter& iter);
			pLIter getRight(const pLIter& iter);
			const pLIter& leftIter() const { return _leftIter; }
			const pLIter& rightIter() const { return _rightIter; }
			void getTargetList(list<pLIter>& target_list, int country_id, int side);
			pLIter getTargetList(list<pLIter>& target_list, int player_id);

			Json::Value getChangeInfo();
			Json::Value getInfo();
			Json::Value getExtraInfo();

			bool empty() const { return _playerList.size() <= 2; }

			void setFromBSON(const mongo::BSONElement& obj);
			inline mongo::BSONArray toBSON() const;

			void run(const handler& h);
			void alterSup(int player_id, int sup);

		private:
			static pathItemList getInitPlayerList();
			int getTriggerTime(const pLIter& left_iter, const pLIter& right_iter);
			void setTriggerTime(pLIter& iter, int meet_time);

			pathItemList _playerList;
			pLIter _leftIter;
			pLIter _rightIter;
			pathSortHelper _sortHelper;
			int _count[_max_side];

			int _distance;
			int _def_speed[_max_side];
			int _rate;
			updateItems _updateItems;
			int _path_id;
		};

		const static string starwar_guild_type_field_str = "gt";
		const static string starwar_owner_id_field_str = "oi";

		class guild
		{
		public:
			guild(const mongo::BSONElement& obj);

			guild(int player_id, const string& name, int face_id, unsigned time, int supply, int base_supply)
				: _id(player_id), _name(name), _face_id(face_id), _time(time), _supply(supply), _base_supply(base_supply), _type(_player){}
			guild(int player_npc_id, int own_id, int title, unsigned time, int supply)
				: _id(player_npc_id), _owner_id(own_id), _title(title), _time(time), _supply(supply), _type(_player_npc){}
			guild(int npc_id, int supply)
				: _id(npc_id), _supply(supply), _time(0), _type(_npc){}

			static guild makeKey(int npc_id, int supply){ return guild(npc_id, supply); }
			static guild makeKey(int player_id, unsigned time){ return guild(player_id, time); }
			static guild makeKey(int npc_id, unsigned time, int own_id){ return guild(npc_id, time, own_id); }

			inline mongo::BSONObj toBSON() const;
			inline void getValue(Json::Value& info, int star_supply) const;

			int getType() const { return _type; }
			int getId() const { return _id; }
			const string& getName() const { return _name; }
			int getFaceId() const { return _face_id; }
			int getTitle() const { return _title; }
			unsigned getTime() const { return _time; }
			int getOwnerId() const { return _owner_id; }

			int getSupply(int star_supply = 0) const;
			void alterSupply(int supply, int base_supply = 0) const;

			bool operator<(const guild& rhs) const
			{
				if (_type == rhs.getType())
				{
					if (_time == rhs.getTime())
						return _id < rhs.getId();
					return _time > rhs.getTime();
				}
				return _type > rhs.getType();
			}

		private:
			guild(int player_id, unsigned time)
				: _id(player_id), _time(time), _type(_player){}
			guild(int npc_id, unsigned time, int own_id)
				: _id(npc_id), _time(time), _owner_id(own_id), _type(_player_npc){}

		private:
			int _type;
			int _id;
			int _owner_id;
			string _name;
			union{
				int _face_id;
				int _title;
			};
			unsigned _time;
			mutable int _supply;
			mutable int _base_supply;
		};

		class guildList
		{
			public:
				guildList() : _player_npc_num(0){}

				typedef boost::function<void(const guild& g)> handler;
				typedef set<guild> guildSet;
				typedef guildSet::iterator guildPtr;

				void push(const mongo::BSONElement& obj);
				void push(int player_id, const string& name, int face_id, unsigned time, int supply, int base_supply);
				void push(int npc_id, int supply);
				void push(int player_npc_id, int owner_id, int title, unsigned time, int supply);
				
				void pop(int player_id, unsigned time);
				void pop(guildPtr ptr);
				void pop_front();
				const guild& front() const;

				bool alterSup(int player_id, unsigned time, int sup, int star_sup);
				bool alterSup(int npc_id, int sup, bool sub = true);

				//bool find(int player_id, unsigned time);
				bool find(int player_id, unsigned time, guildPtr& ptr);
				bool find(int npc_id, guildPtr& ptr);

				void run(const handler& h);
				void clear(){ _guildSet.clear(); }
				bool empty() const { return _guildSet.empty(); }

				void getValue(Json::Value& info, int begin, int count, int star_supply);
				inline mongo::BSONArray toBSON() const;

				int getPlayerNpcNum() const { return _player_npc_num; }

			private:
				guildSet _guildSet;
				int _player_npc_num;
		};

		class eventInfo
		{
		public:
			eventInfo() : _state(false){}
			void setValue(const triggerItem& item, const pLIter& iter)
			{
				_item = item;
				_iter = iter;
				_state = true;
			}
			triggerItem& getItem(){ return _item; }
			pLIter getIter() const { return _iter; }
			bool getState() const { return _state; }
			void setState(bool flag){ _state = flag; }

		private:
			triggerItem _item;
			pLIter _iter;
			bool _state;
		};

		const static string starwar_star_id_field_str = "si";
		const static string starwar_state_field_str = "st";
		const static string starwar_path_info_db_str = "gl.starwar_path_info";
		const static string starwar_path_id_field_str = "pi";
		const static string starwar_player_list_field_str = "pl";

		class saveHelper
		{
		public:
			virtual void save() = 0;
		};

		class path
		{
		public:
			path(int path_id, int left_star, int right_star);

			void setCostTime(int from_star, int to_star, int cost_time);
			void setPathType(int from_star, int to_star, int type);
			void load(int type, unsigned time, const playerInfo& player_info, int from_star_id);

			int getCostTime(int from_id, int to_id) const;

			void reset(unsigned now);
			void resetRate(int rate, unsigned now);
			bool kickPlayer(int player_id);

			bool isAccess(int from_star_id, int to_star_id) const
			{
				if (from_star_id == _linked_star[_left_side] && to_star_id == _linked_star[_right_side])
					return _path_type[_left_side] == 1;
				if (from_star_id == _linked_star[_right_side] && to_star_id == _linked_star[_left_side])
					return _path_type[_right_side] == 1;
				return false;
			}

			bool isRaid(int from_star_id, int to_star_id) const
			{
				int side = _linked_star[_left_side] == from_star_id ? _left_side : _right_side;
				return _path_type[side] == 2;
			}

			void alterSupply(int player_id, int sup);

			int getPathId() const { return _path_id; }
			int getLinkedStarId(int star_id){ return _linked_star[_left_side] == star_id ? _linked_star[_right_side] : _linked_star[_left_side]; }

			int handleEventReq();
			void attackResp(int type, int supply, int target_supply);

			int doEnter(int type, unsigned time, playerDataPtr d, int to_star_id);
			int doTransfer(unsigned time, playerDataPtr d, int to_star_id, bool flag = false);
			int doLeave(int type, unsigned time, const playerInfo& player_info, int to_star_id);

			const Json::Value& getPathInfo() const { return _pathInfo; }
			const Json::Value& getPathChangeInfo() const { return _pathChangeInfo; }
			unsigned size() const { return _playerMgr.size(); }

			void update(bool flag = false);

			void sendBack(const pathItem& item, unsigned now);

		private:
			int meetReq();
			int arriveReq();
			int postToAttack(int type);
			void resetEvent();
			void setEvent(const pLIter& iter);
			void getTargetList(int country_id, int side);
			pLIter getTargetList(int player_id);

		private:
			int _path_id;
			int _linked_star[_max_side];
			int _path_type[_max_side];
			int _cost_time[_max_side];

			cacheInfo _cacheInfo;
			eventInfo _eventInfo;
			pathItemMgr _playerMgr;
			list<pLIter> _transferTargetList;

			Json::Value _pathInfo;
			Json::Value _pathChangeInfo;
			bool _extra;
		};

		typedef boost::shared_ptr<path> pathPtr;

		const static string starwar_star_info_db_str = "gl.starwar_star_info";
		const static string starwar_guilds_list_field_str = "gl";
		const static string starwar_sources_field_str = "sc";
		const static string starwar_guild_count_field_str = "gc";
		const static string starwar_base_time_field_str = "bt";
		const static string starwar_supply_field_str = "sp";
		const static string starwar_target_supply_field_str = "tsp";
		const static string starwar_is_npc_field_str = "in";
		const static string starwar_init_npc_field_str = "in";
		const static string starwar_current_id_field_str = "cri";

		class star : public saveHelper
		{
		public:
			const static int mainstar[3];

			star(int star_id);
			void init(const Json::Value& info);
			void load(const mongo::BSONObj& obj);
			void load(int player_id, const string& name, int face_id, unsigned time, int supply, int base_supply);
			virtual void save();

			void tick();
			void loadNpc();
			void reset(unsigned now);
			void kickPlayer(int player_id, unsigned time);

			bool alterSup(unsigned now, playerDataPtr d, int sup);
			bool alterSup(int npc_id, int sup);

			void addPath(const pathPtr& ptr){ _paths.push_back(ptr); }
			void attackResp(int supply, int target_supply);
			pathPtr getPath(int star_id);
			int getStarId() const { return _star_id; }

			int doLeave(int type, unsigned time, playerDataPtr d, int to_star_id);
			int doEnter(int type, unsigned time, const playerInfo& player_info, int from_star_id);
			int settlePlayerNpc(playerDataPtr d, unsigned now, int num);

			void sendBack(const guild& g, unsigned now);

			unsigned getRecourse(int country_id) const
			{
				return _resource[country_id];
			}
			int getCountryId() const { return _country_id; }
			Json::Value getInfo(int begin, int count);
			int getEffectType() const { return _effect_type; }
			effect getEffect(int country_id) const { return _effects[country_id]; }
			double getAtkEffect(int country_id)
			{
				if (_effect_type == _atk_buff)
					return _effects[country_id]._rate;
				return 0.0;
			}
			double getDefEffect(int country_id)
			{
				if (_effect_type == _def_buff)
					return _effects[country_id]._rate;
				return 0.0;
			}

			bool isSupplyEffect(int country_id);
			int getSupply(unsigned now, int country_id);
			double getSupplyFloat(unsigned now, int country_id);

			void resetRate(int rate, unsigned now);

			bool checkGuildList(playerDataPtr d);

			void getBuffValue(Json::Value& info, int country_id);

			bool subSupplyItem(int supply, int player_id, playerDataPtr d = playerDataPtr());
			bool subSupplyItem(int supply, int& type, int& id);

			inline bool getAttacked();

			unsigned getOutput() const { return _gold; }
			unsigned getMaxOutput() const { return _max_gold; }

		private:
			int postToAttack();
			void postToUpdateTaskInfo(int player_id, int star_id);

		private:
			const int _star_id;
			int _country_id;
			unsigned _last_time;
			guildList _guardList;
			vector<pathPtr> _paths;

			cacheInfo _cacheInfo;

			unsigned _occupy_time[3];
			unsigned _resource[3];

			unsigned _supply[3];
			unsigned _base_time[3];

			int _type;
			int _effect_type;
			unsigned _gold;
			unsigned _max_gold;
			effect _effects[3];
			int _rate;
			bool _init_npc;
			bool _attacked;
			unsigned _current_id;
		};

		typedef boost::shared_ptr<star> starPtr;

		class starList
		{
		public:
			typedef vector<starPtr> starMap;
			typedef starMap::iterator iterator;

			starList();

			starPtr getStar(int star_id)
			{
				if (star_id >= (int)_stars.size() || star_id < 0)
					return starPtr();
				return _stars[star_id];
			}
			void push(const starPtr& ptr);
			iterator begin(){ return _stars.begin(); }
			iterator end(){ return _stars.end(); }

			void update(bool is_first = false);
			void tickSupplyRate(int rate, unsigned now);
			const Json::Value& getOccupyInfo() const { return _occupyInfo; }
			const Json::Value& getOccupyChangeInfo() const { return _occupyChangeInfo; }

			unsigned getOutputBuff(int country_id);

			int num() const { return _stars.size(); }
			void reset(unsigned now);

		private:
			starMap _stars;
			Json::Value _occupyInfo;
			Json::Value _occupyChangeInfo;
		};

		class pathList
		{
		public:
			typedef boost::unordered_map<int, pathPtr> pathMap;

			pathList();

			pathPtr getPath(int path_id)
			{
				pathMap::iterator iter = _paths.find(path_id);
				if (iter != _paths.end())
					return iter->second;
				return pathPtr();
			}
			void push(const pathPtr& ptr);
			pathMap& getAllPath(){ return _paths; }
			void update(bool is_first = false);
			int isExtra() const { return _extra; }
			const Json::Value* getExtraInfo() const { return _extraInfo; }
			const Json::Value& getPathInfo() const { return _pathInfo; }
			const Json::Value& getPathChangeInfo() const { return _pathChangeInfo; }

			void resetRate(unsigned now);
			void reset(unsigned now);

		private:
			pathMap _paths;
			Json::Value _pathInfo;
			Json::Value _pathChangeInfo;

			int _extra;
			Json::Value _extraInfo[10];
		};

		class occupyInfo
		{
		public:
			occupyInfo()
			{
				for (unsigned i = 0; i < 3; ++i)
					_star_num[i] = 0;
			}

			int getUnityCountry();

			void push(int country_id)
			{
				if (country_id < 0 || country_id > 2)
					return;
				++_star_num[country_id]; 
			}
			void pop(int country_id)
			{
				if (country_id < 0 || country_id > 2)
					return;
				--_star_num[country_id]; 
			}
			void getValue(Json::Value& info) const
			{
				for (unsigned i = 0; i < 3; ++i)
					info.append(_star_num[i]);
			}

			void clear()
			{
				for (unsigned i = 0; i < 3; ++i)
					_star_num[i] = 0;
			}

		private:
			int _star_num[3];
		};

		class gatherInfo
		{
		public:
			typedef set<int> gatherList;

			gatherInfo() : _to_star_id(-1){}
			void init(int country_id);
			int getToStarId() const { return _to_star_id; }
			unsigned getCd() const { return _cd_time; }
			void reset(unsigned now, int to_star_id);
			void setFromBSON(const mongo::BSONElement& obj);
			void clear();
			bool isGather(int player_id);
			void kickPlayer(int player_id);
			bool empty();
			const gatherList& getGatherList() const { return _gatherList; }

		private:
			int _country_id;
			int _to_star_id;
			gatherList _gatherList;
			unsigned _cd_time;
		};

		const static string starwar_year_field_str = "yr";

		class greatEvent
		{
			public:
				greatEvent(const mongo::BSONElement& obj);
				greatEvent(int year, int country_id, const rulerTitleInfoList& titleInfoList)
					: _year(year), _country_id(country_id), _titleInfoList(titleInfoList){}

				void getValue(Json::Value& info) const;
				mongo::BSONObj toBSON() const;

			private:
				unsigned _year;
				int _country_id;
				rulerTitleInfoList _titleInfoList;
		};

		const static string starwar_great_event_list_field_str = "gel";

		typedef list<greatEvent> greatEvents;

		class greatEventList
		{
			public:
				greatEventList() : _cache(Json::arrayValue){}
				void setFromBSON(const mongo::BSONElement& obj);

				mongo::BSONArray toBSON() const;

				void push(const greatEvent& e);
				Json::Value getValue() const { return _cache; }

			private:
				void package();

				Json::Value _cache;
				greatEvents _greatEventList;
		};

		const static string starwar_sign_info_list_field_str = "sil";
		const static string starwar_sign_info_db_list_field_str = "silt";

		class sign
		{
			public:
				sign(int star_id, unsigned cd, int key_id)
					: _star_id(star_id), _cd(cd), _key_id(key_id){}

				int getStarId() const { return _star_id; }
				int getCd() const { return _cd; }
				unsigned getKeyId() const { return _key_id; }

			private:
				int _star_id;
				unsigned _cd;
				int _key_id;
		};

		class signInfo
		{
			public:
				typedef list<sign> signList;

				signInfo() : _max_key_id(0){}

				bool getSign(int country_id, Json::Value& info) const
				{
					if (country_id < 0 || country_id > 2)
						return false;
					
					info = Json::arrayValue;

					for (signList::const_iterator iter = _signInfo[country_id].begin(); iter != _signInfo[country_id].end(); ++iter)
						info.append(iter->getStarId());

					return true;
				}

				bool setSign(int country_id, int star_id, bool state);
				int clearSignInfo(int key_id);

				void setFromBSON(const mongo::BSONObj& obj);
				mongo::BSONArray toBSON() const;
				int getMaxKeyId() const{ return _max_key_id; }

			private:
				signList _signInfo[3];
				int _max_key_id;
		};

		const static string starwar_system_info_db_str = "gl.starwar_system_info";
		const static string starwar_reward_field_str = "rw";
		const static string starwar_gather_info_field_str = "gi";

		class systemInfo
		{
		public:
			systemInfo();
			void load();
			void save(int index);

			unsigned getSystemTime(int index) const { return _system_time[index]; }
			int getState(int index) const { return _state[index]; }
			unsigned getUnityReward(int country_id) const { return _unity_reward[country_id]; }
			void alterUnityReward(int country_id, int num);
			void alterSystemTime(int index, unsigned time, int state = -1);

			void resetGatherInfo(int country_id, unsigned now, int to_star_id);
			bool isGather(playerDataPtr d);
			void kickPlayer(playerDataPtr d);
			const gatherInfo& getGatherInfo(int country_id) const { return _gatherInfo[country_id]; }

			void pushGreatEvent(const greatEvent& e);
			Json::Value getGreatEventValue() const { return _greatEventList.getValue(); }

			bool getSignInfo(int country_id, Json::Value& info) const { return _signInfo.getSign(country_id, info); }
			bool setSignInfo(int country_id, int star_id, bool state)
			{
				bool result = _signInfo.setSign(country_id, star_id, state);
				if (result)
					save(_sign_info_index);
				return result;
			}

			int clearSignInfo(int key_id)
			{
				return _signInfo.clearSignInfo(key_id);
			}

		private:
			unsigned _unity_reward[3];
			unsigned _system_time[_index_max];
			int _state[_index_max];
			gatherInfo _gatherInfo[3];
			greatEventList _greatEventList;
			signInfo _signInfo;
		};

		const static string starwar_rank_field_str = "rk";
		const static string starwar_rank_list_field_str = "rl";

		class rankRecord
		{
		public:
			rankRecord(int rank, int exploit) : _rank(rank), _exploit(exploit){}

			int getRank() const { return _rank; }
			unsigned getExploit() const { return _exploit; }

		private:
			unsigned _exploit;
			int _rank;
		};

		class rankItem
		{
		public:
			static rankItem makeKey(unsigned exploit, unsigned first_time, int player_id)
			{
				return rankItem(exploit, first_time, player_id);
			}

			rankItem(int player_id, const string& name, int lv, unsigned exploit, unsigned first_time) : _player_id(player_id), _name(name), _lv(lv), _exploit(exploit), _first_time(first_time){}
			rankItem(playerDataPtr d)
				: _player_id(d->playerID), _name(d->Base.getName()), _lv(d->Base.getLevel()), _exploit(d->Starwar.getExploit()), _first_time(d->Starwar.getFirstTime()){}
			bool operator<(const rankItem& rhs) const
			{
				if (_exploit == rhs.getExploit())
				{
					if (_first_time == rhs.getFirstTime())
						return _player_id > rhs.getPlayerId();
					return _first_time > rhs.getFirstTime();
				}
				return _exploit < rhs.getExploit();
			}

			unsigned getFirstTime() const { return _first_time; }
			int getPlayerId() const { return _player_id; }
			unsigned getExploit() const { return _exploit; }
			string getName() const { return _name; }
			int getLv() const { return _lv; }
			void getValue(Json::Value& info, int country_id) const
			{
				info.append(_player_id);
				info.append(_name);
				info.append(_lv);
				info.append(country_id);
				info.append(_exploit);
			}

		private:
			rankItem(unsigned exploit, unsigned first_time, int player_id)
				: _exploit(exploit), _first_time(first_time), _player_id(player_id){}

		private:
			int _player_id;
			string _name;
			int _lv;
			unsigned _exploit;
			unsigned _first_time;
		};

		const static string starwar_old_ranking_db_str = "gl.starwar_old_ranking";

		class rankMgr
		{
		public:
			typedef set<rankItem> rankItemSet;
			typedef map<int, rankRecord> pid2Rank;

			void init(int country_id){ _country_id = country_id; }
			int getRank(playerDataPtr d);
			rankRecord getOldRank(int player_id);
			void update(playerDataPtr d, int temp);
			void getValue(Json::Value& info, playerDataPtr d);
			void tick();
			void save();
			void broadcast();
			void primeTimeSummary();

			void load(int player_id, const string& name, int lv, int exploit, unsigned first_time);
			void loadLastTerm(int player_id, int rank, unsigned exploit);

		private:
			rankItemSet _rankItems;
			pid2Rank _lastTermRecords;
			int _country_id;
		};

		class pathInfo
		{
			public:
				pathInfo() : _cost_time(-1){}

				void init(const mPath& pass_star_list);

				int getTime() const { return _cost_time; }
				int getPassStarNum() const { return _pass_star_list.size() - 1; }
				const mPath& getPassStarList() const { return _pass_star_list; }

				bool isNull() const { return _cost_time == -1; }

			private:
				int _cost_time;
				mPath _pass_star_list;
		};

		class shortestPath
		{
			public:
				typedef vector<pathInfo> pathInfos;
				typedef vector<pathInfos> pathInfoList;

				void load(const vector<mPathList>& path_list)
				{
					for (vector<mPathList>::const_iterator iter = path_list.begin(); iter != path_list.end(); ++iter)
					{
						const mPathList& p1 = *iter;
						pathInfos infos;
						for (mPathList::const_iterator it1 = p1.begin(); it1 != p1.end(); ++it1)
						{
							pathInfo temp;
							temp.init(*it1);
							infos.push_back(temp);
						}
						_pathInfoList.push_back(infos);
					}
				}
				const pathInfo& getPathInfo(int from_id, int to_id) const
				{
					if (from_id >= (int)_pathInfoList.size() || from_id < 0)
						return _null;
					if (to_id >= (int)_pathInfoList.size() || to_id < 0)
						return _null;
					return _pathInfoList[from_id][to_id];
				}

			private:
				pathInfoList _pathInfoList;
				static pathInfo _null;
		};

		//class rankHelper
		//{
		//	public:
		//		void push(const rankItem& item);
		//		void pop(int player_id = -1);
		//		int getRank(int player_id);
		//		int& num(){ return _num; }

		//	private:
		//		typedef list<rankItem> rankItemList;
		//		rankItemList _rankItemList;
		//		int _num;
		//};

		//class rankMgr
		//{
		//	public:
		//		enum{
		//			_max_rank_num = 500,
		//		};

		//		void update(playerDataPtr d, int old_exploit);

		//	private:
		//		typedef map<int, rankHelper> rankItemMap;
		//		rankItemMap _rankItemMap;
		//};
	}
}