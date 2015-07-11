#include "player_starwar.h"
#include "starwar_system.h"
#include "helper.h"
#include "season_system.h"
#include "recorder.hpp"
#include "activity_system.h"
namespace gg
{
	namespace starwar
	{
		void taskInfo::load(Json::Value& info)
		{
			_id = info["id"].asInt();
			_type = info["type"].asInt();
			_condition1 = info["condition1"].asInt();
			_condition2 = info["condition2"].asInt();
			_reward.setValue(info["reward"]);
			_rate = info["probability"].asDouble();
		}

		int taskInfo::update(playerDataPtr d, vector<int>& args, int temp)  // temp = -1 init  = 0 recheck > 0 change
		{
			switch (_type)
			{
				case _win_streak:
				{
					if (temp == -1)
					{
						args.push_back(0);
						args.push_back(0);
					}
					else if (temp == 1)
						++args[1];
					else if (temp == 2)
						args[1] = 0;
					if (args[1] >= _condition2)
					{
						++args[0];
						args[1] = 0;
					}

					return args[0] >= _condition1 ? _task_finished : _task_running;
				}
				case _occupy_specified:
				{
					if (temp == -1)
						args.push_back(0);
					else if (temp != 0)
					{
						--temp;
						if (temp == _condition2)
							++args[0];
					}
					return args[0] >= _condition1 ? _task_finished : _task_running;
				}
				case _occupy_times:
				case _transfer_times:
				case _win_times:
				case _lose_times:
				case _kill_king:
				case _use_items:
				{
					if (temp == -1)
						args.push_back(0);
					else if (temp != 0)
						++args[0];
					return args[0] >= _condition1 ? _task_finished : _task_running;
				}
				case _stand_guard:
				{
					// temp = -1 初始化任务, temp = 0 重新检查状态, temp = 1 进入星球, = 2 离开星球
					unsigned now = na::time_helper::get_current_time();
					if (temp == -1)
					{
						const starwar::position& position = d->Starwar.getPosition();
						if (position.getType() == _in_star && position.getId() != star::mainstar[d->Base.getSphereID()])
							args.push_back(now + _condition1 * na::time_helper::HOUR);
						else
							args.push_back(-1);
					}
					else
					{
						if (temp == 1)
						{
							if (d->Starwar.getPosition().getId() != star::mainstar[d->Base.getSphereID()])
								args[0] = now + _condition1 * na::time_helper::HOUR;
							else
								args[0] = -1;
						}
						else if (temp == 2 && args[0] != -1)
						{
							if ((int)now < args[0])
								args[0] = -1;
						}
					}

					if (args[0] != -1)
						return (int)now >= args[0] ? starwar::_task_finished : starwar::_task_running;
					return starwar::_task_running;
				}
				case _to_different_star:
				{
					if (temp == -1)
						args.push_back(0);
					else if (temp != 0)
					{
						--temp;
						bool flag = false;
						vector<int>::iterator iter = args.begin();
						for (++iter; iter != args.end(); ++iter)
						{
							if (*iter == temp)
							{
								flag = true;
								break;
							}
						}
						if (!flag)
						{
							args.push_back(temp);
							++args[0];
						}
					}
					return args[0] >= _condition1 ? _task_finished : _task_running;
				}
				case _exploit_num:
				case _silver_num:
				{
					if (temp == -1)
						args.push_back(0);
					else
						args[0] += temp;

					return args[0] >= _condition1 ? _task_finished : _task_running;
				}

				default:
					return _task_running;
			}
		}

		void taskInfo::getReward(playerDataPtr d)
		{
			_reward.getRewardWithMail(d);
		}

		int taskInfo::getReward()
		{
			return _reward.getItemNum(action_add_silver); 
		}

		bool taskRecord::setValue(const mongo::BSONElement& obj)
		{
			int task_id = -1;
			checkNotEoo(obj[starwar_task_id_field_str])
				task_id = obj[starwar_task_id_field_str].Int();
			taskPtr ptr = starwar_sys.getTaskPtr(task_id);
			if (ptr == taskPtr())
				return false;
			_taskPtr = ptr;
			checkNotEoo(obj[starwar_task_state_field_str])
				_state = obj[starwar_task_state_field_str].Int();
			checkNotEoo(obj[starwar_task_args_field_str])
			{
				_args.clear();
				vector<mongo::BSONElement> ele = obj[starwar_task_args_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
					_args.push_back(ele[i].Int());
			}
			return true;
		}

		void taskRecord::setValue(const taskPtr& ptr, playerDataPtr d)
		{
			_taskPtr = ptr;
			_state = ptr->update(d, _args, -1);
		}

		mongo::BSONObj taskRecord::toBSON()
		{
			mongo::BSONObjBuilder obj;
			obj << starwar_task_id_field_str << _taskPtr->getId()
				<< starwar_task_state_field_str << _state;
			{
				mongo::BSONArrayBuilder array_sw;
				for (vector<int>::iterator iter = _args.begin(); iter != _args.end(); ++iter)
					array_sw.append(*iter);
				obj << starwar_task_args_field_str << array_sw.arr();
			}
			return obj.obj();
		}

		void taskRecord::getValue(Json::Value& info, playerDataPtr d)
		{
			info = Json::arrayValue;
			info.append(_taskPtr->getId());
			info.append(_args[0]);
			if (_state == starwar::_task_running)
				_state = _taskPtr->update(d, _args, 0);
			info.append(_state);
		}

		void taskRecord::getReward(playerDataPtr d)
		{
			_taskPtr->getReward(d);
		}

		int taskRecord::getReward()
		{
			return _taskPtr->getReward();
		}

		void taskRecord::commit()
		{
			_state = _task_commit;
		}

		bool taskRecord::update(playerDataPtr d, int temp)
		{
			int old_arg = _args[0];
			int old_state = _state;
			_state = _taskPtr->update(d, _args, temp);
			if (old_arg == _args[0] || _state == old_state)
				return true;
			return false;
		}

		void position::load(const mongo::BSONElement& obj)
		{
			checkNotEoo(obj[starwar_type_field_str])
				_type = obj[starwar_type_field_str].Int();
			checkNotEoo(obj[starwar_id_field_str])
				_id = obj[starwar_id_field_str].Int();
			checkNotEoo(obj[starwar_time_field_str])
				_time = obj[starwar_time_field_str].Int();
			checkNotEoo(obj[starwar_from_star_id_field_str])
				_from_star_id = obj[starwar_from_star_id_field_str].Int();
			checkNotEoo(obj[starwar_action_field_str])
				_action.setType(obj[starwar_action_field_str].Int());
			checkNotEoo(obj[starwar_path_info_field_str])
			{
				vector<mongo::BSONElement> ele = obj[starwar_path_info_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
					_path.push_back(ele[i].Int());
			}
		}

		mongo::BSONObj position::toBSON()
		{
			mongo::BSONObjBuilder obj;
			obj << starwar_type_field_str << _type
				<< starwar_id_field_str << _id
				<< starwar_time_field_str << _time
				<< starwar_from_star_id_field_str << _from_star_id
				<< starwar_action_field_str << _action.getType();
			{
				mongo::BSONArrayBuilder array_sw;
				for (vector<int>::const_iterator iter = _path.begin(); iter != _path.end(); ++iter)
					array_sw.append(*iter);
				obj << starwar_path_info_field_str << array_sw.arr();
			}
			return obj.obj();
		}
	}

	void battleParam::setValue(const mongo::BSONElement& obj)
	{
		_rate = obj[starwar_rate_field_str].Double();
		_cd = obj[starwar_cd_field_str].Int();
	}

	mongo::BSONObj battleParam::toBSON()
	{
		return BSON(starwar_rate_field_str << _rate
			<< starwar_cd_field_str << _cd);
	}

	inline starwarItem::starwarItem(const mongo::BSONElement& obj)
	{
		checkNotEoo(obj[starwar_type_field_str])
			_type = obj[starwar_type_field_str].Int();
		checkNotEoo(obj[starwar_level_field_str])
			_lv = obj[starwar_level_field_str].Int();
	}

	inline mongo::BSONObj starwarItem::toBSON()
	{
		return BSON(starwar_type_field_str << _type
			<< starwar_level_field_str << _lv);
	}

	inline Json::Value starwarItem::getValue()
	{
		Json::Value info = Json::arrayValue;
		info.append(_type);
		if (_type != _empty)
			info.append(_lv);
		return info;
	}

	inline starwarReport::starwarReport(const mongo::BSONElement& obj)
	{
		checkNotEoo(obj[starwar_result_field_str])
			_result = obj[starwar_result_field_str].Int();
		checkNotEoo(obj[starwar_report_id_field_str])
			_report_id = obj[starwar_report_id_field_str].String();
		checkNotEoo(obj[starwar_name_field_str])
			_target_name = obj[starwar_name_field_str].String();
		checkNotEoo(obj[starwar_exploit_field_str])
			_exploit = obj[starwar_exploit_field_str].Int();
		checkNotEoo(obj[starwar_supply_field_str])
			_supply = obj[starwar_supply_field_str].Int();
		checkNotEoo(obj[starwar_target_supply_field_str])
			_target_supply = obj[starwar_target_supply_field_str].Int();
		checkNotEoo(obj[starwar_is_npc_field_str])
			_is_npc = obj[starwar_is_npc_field_str].Bool();
		else 
			_is_npc = false;
		checkNotEoo(obj[starwar_reward_field_str])
			_silver = obj[starwar_reward_field_str].Int();
		else
			_silver = 0;
		checkNotEoo(obj[ruler_title_field_str])
			_title = obj[ruler_title_field_str].Int();
		else
			_title = -1;
	}

	inline mongo::BSONObj starwarReport::toBSON()
	{
		return BSON(starwar_result_field_str << _result
			<< starwar_report_id_field_str << _report_id
			<< starwar_name_field_str << _target_name
			<< starwar_exploit_field_str << _exploit
			<< starwar_supply_field_str << _supply
			<< starwar_target_supply_field_str << _target_supply
			<< starwar_reward_field_str << _silver
			<< starwar_is_npc_field_str << _is_npc
			<< ruler_title_field_str << _title);
	}

	inline void starwarReport::getValue(Json::Value& info) const
	{
		info.append(_result);
		info.append(_report_id);
		info.append(_target_name);
		info.append(_exploit);
		info.append(_supply);
		info.append(_target_supply);
		info.append(_is_npc);
		info.append(_silver);
		info.append(_title);
	}

	starwarReportPackage::starwarReportPackage(int type, int arg, unsigned time, const starwarReportList& rl)
		: _type(type), _arg(arg), _time(time), _reportList(rl)
	{
	}

	inline starwarReportPackage::starwarReportPackage(const mongo::BSONElement& obj)
	{
		_type = obj[starwar_type_field_str].Int();
		_arg = obj[starwar_id_field_str].Int();
		_time = obj[starwar_time_field_str].Int();
		if (_type >= starwar::_report_type_0 && _type <= starwar::_report_type_2)
		{
			vector<mongo::BSONElement> ele = obj[starwar_report_list_field_str].Array();
			for (unsigned i = 0; i < ele.size(); ++i)
				_reportList.push_back(starwarReport(ele[i]));
		}
		if (_type == starwar::_report_type_3)
		{
			_name = obj[starwar_name_field_str].String();
			_send_back = obj[starwar_result_field_str].Bool();
		}
		if (_type == starwar::_report_type_4)
		{
			_name = obj[starwar_name_field_str].String();
		}
	}

	inline mongo::BSONObj starwarReportPackage::toBSON()
	{
		mongo::BSONObjBuilder obj;
		obj << starwar_type_field_str << _type
			<< starwar_id_field_str << _arg
			<< starwar_time_field_str << _time;
		if (_type >= starwar::_report_type_0 && _type <= starwar::_report_type_2)
		{
			{
				mongo::BSONArrayBuilder array_sw;
				for (starwarReportList::iterator iter = _reportList.begin(); iter != _reportList.end(); ++iter)
					array_sw.append(iter->toBSON());
				obj << starwar_report_list_field_str << array_sw.arr();
			}
		}
		if (_type == starwar::_report_type_3)
		{
			obj << starwar_name_field_str << _name
				<< starwar_result_field_str << _send_back;
		}
		if (_type == starwar::_report_type_4)
		{
			obj << starwar_name_field_str << _name;
		}

		return obj.obj();
	}

	inline void starwarReportPackage::getValue(Json::Value& info) const
	{
		info.append(_type);
		info.append(_arg);
		info.append(_time);
		if (_type == starwar::_report_type_3)
		{
			info.append(_name);
			info.append(_send_back);
		}
		else if (_type == starwar::_report_type_4)
		{
			info.append(_name);
		}
		else
		{
			Json::Value reports;
			for (starwarReportList::const_iterator iter = _reportList.begin(); iter != _reportList.end(); ++iter)
			{
				Json::Value temp;
				iter->getValue(temp);
				reports.append(temp);
			}
			info.append(reports);
		}
	}

	playerStarwar::playerStarwar(playerData& own)
		: Block(own), _max_file_id(0), _exploit(0), _on_main(false), _items(starwarItem::_init_item_num), _next_update_time(0)
		, _next_term_time(0), _ranking_reward(false), _gather_times(0), _gather_cd(0), _first_time(0), _last_leave_time(0), _old_title(_no_title)
		, _settle_npc_num(0), _battle_cd(0), _battle_times(0)
	{
		string pathname = reportStarwarDir;
		pathname += boost::lexical_cast<string, int>(own.playerID);
		boost::filesystem::path path_dir(pathname);
		if (!boost::filesystem::exists(path_dir))
			boost::filesystem::create_directory(path_dir);
	}

	bool playerStarwar::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(starwar_db_str, key);
		if (!obj.isEmpty()){
			checkNotEoo(obj[starwar_position_field_str]){ _position.load(obj[starwar_position_field_str]); }
			checkNotEoo(obj[starwar_max_file_id_field_str]){ _max_file_id = obj[starwar_max_file_id_field_str].Int(); }
			checkNotEoo(obj[starwar_exploit_field_str]){ _exploit = obj[starwar_exploit_field_str].Int(); }
			checkNotEoo(obj[starwar_transfer_cd_field_str]){ _transfer_cd = obj[starwar_transfer_cd_field_str].Int(); }
			checkNotEoo(obj[starwar_transfer_times_field_str]){ _transfer_times = obj[starwar_transfer_times_field_str].Int(); }
			checkNotEoo(obj[starwar_transfer_buy_times_field_str]){ _transfer_buy_times = obj[starwar_transfer_buy_times_field_str].Int(); }
			checkNotEoo(obj[starwar_raid_times_field_str]){ _raid_times = obj[starwar_raid_times_field_str].Int(); }
			checkNotEoo(obj[starwar_supply_field_str]){ _supply = obj[starwar_supply_field_str].Int(); }
			checkNotEoo(obj[starwar_supply_cd_field_str]){ _supply_cd = obj[starwar_supply_cd_field_str].Int(); }
			checkNotEoo(obj[starwar_base_supply_field_str]){ _base_supply = obj[starwar_base_supply_field_str].Int(); }
			checkNotEoo(obj[starwar_get_transfer_times_field_str]){ _get_transfer_times = obj[starwar_get_transfer_times_field_str].Int(); }
			checkNotEoo(obj[starwar_next_update_time_field_str]){ _next_update_time = obj[starwar_next_update_time_field_str].Int(); }
			checkNotEoo(obj[starwar_ranking_reward_field_str]){ _ranking_reward = obj[starwar_ranking_reward_field_str].Bool(); }
			checkNotEoo(obj[starwar_next_term_time_field_str]){ _next_term_time = obj[starwar_next_term_time_field_str].Int(); }
			checkNotEoo(obj[starwar_unity_reward_field_str]){ _unity_reward = obj[starwar_unity_reward_field_str].Bool(); }
			checkNotEoo(obj[starwar_last_unity_time_field_str]){ _last_unity_time = obj[starwar_last_unity_time_field_str].Int(); }
			checkNotEoo(obj[starwar_gather_cd_field_str]){ _gather_cd = obj[starwar_gather_cd_field_str].Int(); }
			checkNotEoo(obj[starwar_gather_times_field_str]){ _gather_times = obj[starwar_gather_times_field_str].Int(); }
			checkNotEoo(obj[starwar_first_time_field_str]){ _first_time = obj[starwar_first_time_field_str].Int(); }
			checkNotEoo(obj[starwar_last_leave_time_field_str]){ _last_leave_time = obj[starwar_last_leave_time_field_str].Int(); }
			checkNotEoo(obj[starwar_settle_npc_num_field_str]){ _settle_npc_num = obj[starwar_settle_npc_num_field_str].Int(); }
			checkNotEoo(obj[starwar_old_title_field_str]){ _old_title = obj[starwar_old_title_field_str].Int(); }
			checkNotEoo(obj[starwar_battle_times_field_str]){ _battle_times = obj[starwar_battle_times_field_str].Int(); }
			checkNotEoo(obj[starwar_battle_cd_field_str]){ _battle_cd = obj[starwar_battle_cd_field_str].Int(); }
			checkNotEoo(obj[starwar_item_buy_times_field_str]){ _item_buy_times = obj[starwar_item_buy_times_field_str].Int(); }
			checkNotEoo(obj[starwar_task_record_field_str])
			{ 
				_taskRecord.clear();
				vector<mongo::BSONElement> ele = obj[starwar_task_record_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
				{
					taskRecord record;
					if (!record.setValue(ele[i]))
						continue;
					_taskRecord.push_back(record);
					if (record.getState() == starwar::_task_running)
						_taskTypes.set(record.getType());
				}
			}
			checkNotEoo(obj[starwar_report_list_field_str])
				_repManager.setFromBSON(obj[starwar_report_list_field_str]);
			checkNotEoo(obj[starwar_resource_list_field_str])
			{
				vector<mongo::BSONElement> ele = obj[starwar_resource_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
					_resource.push_back(ele[i].Int());
			}
			checkNotEoo(obj[starwar_atk_buff_field_str])
				_buff[battleParam::_index_atk].setValue(obj[starwar_atk_buff_field_str]);
			checkNotEoo(obj[starwar_def_buff_field_str])
				_buff[battleParam::_index_def].setValue(obj[starwar_def_buff_field_str]);
			checkNotEoo(obj[starwar_items_field_str])
			{
				_items.clear();
				vector<mongo::BSONElement> ele = obj[starwar_items_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
					_items.push_back(starwarItem(ele[i]));
			}
		}
		return true;
	}

	bool playerStarwar::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObjBuilder obj;
		obj << playerIDStr << own.playerID << starwar_max_file_id_field_str << _max_file_id 
			<< starwar_exploit_field_str << _exploit << starwar_supply_field_str << _supply
			<< starwar_supply_cd_field_str << _supply_cd << starwar_transfer_times_field_str << _transfer_times
			<< starwar_transfer_cd_field_str << _transfer_cd << starwar_transfer_buy_times_field_str << _transfer_buy_times
			<< starwar_raid_times_field_str << _raid_times << starwar_get_transfer_times_field_str << _get_transfer_times
			<< starwar_next_update_time_field_str << _next_update_time
			<< starwar_atk_buff_field_str << _buff[battleParam::_index_atk].toBSON()
			<< starwar_def_buff_field_str << _buff[battleParam::_index_def].toBSON()
			<< starwar_base_supply_field_str << _base_supply << starwar_ranking_reward_field_str << _ranking_reward
			<< starwar_next_term_time_field_str << _next_term_time << starwar_unity_reward_field_str << _unity_reward
			<< starwar_last_unity_time_field_str << _last_unity_time << starwar_gather_times_field_str << _gather_times
			<< starwar_gather_cd_field_str << _gather_cd << starwar_position_field_str << _position.toBSON()
			<< starwar_first_time_field_str << _first_time << starwar_last_leave_time_field_str << _last_leave_time
			<< starwar_settle_npc_num_field_str << _settle_npc_num << starwar_old_title_field_str << _old_title
			<< starwar_battle_times_field_str << _battle_times << starwar_battle_cd_field_str << _battle_cd
			<< starwar_item_buy_times_field_str << _item_buy_times << starwar_report_list_field_str << _repManager.toBSON();
		{
			mongo::BSONArrayBuilder array_starwar;
			for (vector<taskRecord>::iterator it = _taskRecord.begin(); it != _taskRecord.end(); ++it)
				array_starwar.append(it->toBSON());
			obj << starwar_task_record_field_str << array_starwar.arr();
		}
		{
			mongo::BSONArrayBuilder array_starwar;
			for (vector<unsigned>::iterator it = _resource.begin(); it != _resource.end(); ++it)
				array_starwar.append(*it);
			obj << starwar_resource_list_field_str << array_starwar.arr();
		}
		{
			mongo::BSONArrayBuilder array_starwar;
			for (vector<starwarItem>::iterator it = _items.begin(); it != _items.end(); ++it)
				array_starwar.append(it->toBSON());
			obj << starwar_items_field_str << array_starwar.arr();
		}

		return db_mgr.save_mongo(starwar_db_str, key, obj.obj());
	}

	void playerStarwar::autoUpdate()
	{
		if (_on_main)
			update();
	}

	void playerStarwar::package(Json::Value& msg)
	{
		_position.getValue(msg[starwar_position_field_str]);

		msg[starwar_transfer_times_field_str] = _transfer_times;
		msg[starwar_transfer_cd_field_str] = _transfer_cd;
		msg[starwar_transfer_buy_times_field_str] = _transfer_buy_times;
		msg[starwar_get_transfer_times_field_str] = _get_transfer_times;
		msg[starwar_unity_reward_field_str] = _unity_reward ? 0 : starwar_sys.getUnityReward(own.Base.getSphereID());
		msg[starwar_time_field_str] = starwar_sys.getNextGoldTime();
		msg[starwar_battle_cd_field_str] = _battle_cd;
		unsigned now = na::time_helper::get_current_time();
		if (_supply_cd == 0)
			msg[starwar_supply_field_str] = _supply;
		else
		{
			starPtr ptr = starwar_sys.getStar(_position.getId());
			double temp = ptr->getSupplyFloat(now, own.Base.getSphereID()) - (double)_base_supply + (double)_supply;
			if ((int)temp >= (int)starwar_sys.getParam().getMaxSupply())
				msg[starwar_supply_field_str] = starwar_sys.getParam().getMaxSupply();
			else
				msg[starwar_supply_field_str] = temp;
		}
		msg[starwar_supply_cd_field_str] = now;

		msg[starwar_raid_times_field_str] = _raid_times;

		msg[starwar_ranking_reward_field_str] = _ranking_reward;
		rankRecord record = starwar_sys.getOldRank(own.playerID, own.Base.getSphereID());
		msg[starwar_old_rank_field_str] = record.getRank();
		msg[starwar_old_exploit_field_str] = record.getExploit();
		msg[starwar_exploit_field_str] = _exploit;

		msg[starwar_gather_cd_field_str] = _gather_cd;
		msg[starwar_gather_times_field_str] = _gather_times;


		bool flag = false;
		for (vector<taskRecord>::iterator iter = _taskRecord.begin(); iter != _taskRecord.end(); ++iter)
		{
			if (iter->getState() == starwar::_task_finished)
			{
				flag = true;
				break;
			}
		}
		msg[starwar_task_record_field_str] = flag;

		msg[starwar_item_buy_times_field_str] = _item_buy_times;
		msg[starwar_items_field_str] = Json::arrayValue;
		for (vector<starwarItem>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
			msg[starwar_items_field_str].append(iter->getValue());

		msg[starwar_buff_field_str] = Json::arrayValue;
		{
			Json::Value temp = starwar_sys.getStarBuff(own.getOwnDataPtr());
			if (temp != Json::nullValue)
				msg[starwar_buff_field_str].append(temp);
		}

		msg[starwar_buff_field_str].append(starwar_sys.getOutputBuff(own.getOwnDataPtr()));

		if (_buff[battleParam::_index_atk].getRate(now) != 0.0)
		{
			Json::Value info = Json::arrayValue;
			info.append(starwar::_index_item_atk);
			info.append(_buff[battleParam::_index_atk].getRate(now));
			info.append(_buff[battleParam::_index_atk].getCd());
			msg[starwar_buff_field_str].append(info);
		}
		if (_buff[battleParam::_index_def].getRate(now) != 0.0)
		{
			Json::Value info = Json::arrayValue;
			info.append(starwar::_index_item_def);
			info.append(_buff[battleParam::_index_def].getRate(now));
			info.append(_buff[battleParam::_index_def].getCd());
			msg[starwar_buff_field_str].append(info);
		}

		packageResource(msg);
	}

	void playerStarwar::updateData()
	{
		if (_position.getType() == -1)
			initData();
		unsigned now = na::time_helper::get_current_time();
		if (_last_unity_time != starwar_sys.getLastUnityTime())
		{
			_last_unity_time = starwar_sys.getLastUnityTime();
			_unity_reward = false;
			if (_position.getType() != starwar::_in_star || _position.getId() != star::mainstar[own.Base.getSphereID()])
				resetPosition(starwar_sys.getLastUnityTime());
			helper_mgr.insertSaveAndUpdate(this);
		}
		checkAndUpdate(now);
	}

	void playerStarwar::checkAndUpdate(unsigned now)
	{
		if (now >= _next_update_time)
		{
			_next_update_time = na::time_helper::get_next_update_time(now, 5);
			_transfer_buy_times = 0;
			_transfer_times = 0;
			_raid_times = 0;
			_item_buy_times = 0;
			_gather_times = 2;
			_settle_npc_num = 0;
			for (vector<starwarItem>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
				iter->setValue(starwar_sys.getParam().getItemType(), starwar_sys.getParam().getItemLv());

			helper_mgr.insertSaveAndUpdate(this);
		}

		if (now >= _next_term_time)
		{
			_ranking_reward = false;
			clearExploit();
			_next_term_time = season_sys.getNextSeasonTime(season_quarter_one, starwar_sys.getParam().getWarTime().beginTm().hour,
				starwar_sys.getParam().getWarTime().beginTm().min);
			resetTask();

			helper_mgr.insertSaveAndUpdate(this);
		}
	}

	void playerStarwar::update()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		package(msg[msgStr][1u]);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::starwar_player_info_resp, msg);
	}

	bool playerStarwar::toAddCache()
	{
		if (_position.getAction().getType() == starwar::_transfer && _position.sizePath() > 1)
			return true;
		return false;
	}

	void playerStarwar::addCacheReport(const starwar::reportPtr& ptr)
	{
		_cacheRepList.push_back(ptr);
	}

	void playerStarwar::addReport(const starwar::reportPtr& ptr, bool update)
	{
		_repManager.push(ptr);
		_cacheRepList.clear();
		if (update && own.netID >= 0 && _on_main)
			_repManager.updateLast(own.getOwnDataPtr());
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerStarwar::packageReport(Json::Value& msg)
	{
		msg[starwar_report_list_field_str] = Json::arrayValue;
		_repManager.getValue(msg[starwar_report_list_field_str]);
	}

	void playerStarwar::initData()
	{
		unsigned now = na::time_helper::get_current_time();
		resetPosition(now, true);

		_transfer_times = 0;
		_get_transfer_times = 5;
		_transfer_cd = 0;
		_transfer_buy_times = 0;

		_raid_times = 0;

		_gather_cd = 0;
		_gather_times = 2;

		_next_update_time = na::time_helper::get_next_update_time(now);
		_next_term_time = season_sys.getNextSeasonTime(season_quarter_one, starwar_sys.getParam().getWarTime().beginTm().hour,
			starwar_sys.getParam().getWarTime().beginTm().min);

		_exploit = 0;

		_ranking_reward = false;

		_unity_reward = true;
		_last_unity_time = starwar_sys.getLastUnityTime();

		_first_time = now;

		_item_buy_times = 0;
		for (vector<starwarItem>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
			iter->setValue(starwar_sys.getParam().getItemType(), starwar_sys.getParam().getItemLv());

		starList& stars = starwar_sys.getStarList();
		for (starList::iterator iter = stars.begin(); iter != stars.end(); ++iter)
			_resource.push_back((*iter)->getRecourse(own.Base.getSphereID()));

		resetTask();

		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerStarwar::resetPosition(unsigned now, bool init)
	{
		_position.setPosition(starwar::_in_star, star::mainstar[own.Base.getSphereID()], now);

		starPtr ptr = starwar_sys.getStar(star::mainstar[own.Base.getSphereID()]);
		_supply = starwar_sys.getParam().getMaxSupply();
		_base_supply = ptr->getSupply(now, own.Base.getSphereID());
		_supply_cd = now;

		if(init)
			ptr->load(own.playerID, own.Base.getName(), own.Base.getFaceID(), now, _supply, _base_supply);
	}

	void playerStarwar::setPosition(int type, int id, unsigned now, bool next)
	{
		starwar::position temp = _position;
		if(temp.getType() == starwar::_in_star)
			_position.setPosition(type, id, now, temp.getId());
		else
			_position.setPosition(type, id, now);

		if (temp.getType() == starwar::_in_star && _position.getType() == starwar::_in_path)
		{
			updateTask(starwar::_stand_guard, 2);
			starPtr ptr = starwar_sys.getStar(temp.getId());
			if (ptr->getEffectType() == starwar::_supply_effect)
			{
				_supply = ptr->getSupply(now, own.Base.getSphereID()) - _base_supply + _supply;
				_supply = _supply > (int)starwar_sys.getParam().getMaxSupply() ? (int)starwar_sys.getParam().getMaxSupply() : _supply;
				_supply_cd = 0;
			}
		}
		else if (temp.getType() == starwar::_in_path && _position.getType() == starwar::_in_star)
		{
			updateTask(starwar::_to_different_star, _position.getId() + 1);
			updateTask(starwar::_stand_guard, 1);
			starPtr ptr = starwar_sys.getStar(_position.getId());
			if (ptr->getEffectType() == 3)
			{
				_base_supply = ptr->getSupply(now, own.Base.getSphereID());
				_supply_cd = now;
			}
		}
		else if (temp.getType() == starwar::_in_star && _position.getType() == starwar::_in_star)
		{
			updateTask(starwar::_to_different_star, _position.getId() + 1);
			updateTask(starwar::_stand_guard, 2);
			updateTask(starwar::_stand_guard, 1);
			{
				starPtr ptr = starwar_sys.getStar(temp.getId());
				if (ptr->getEffectType() == starwar::_supply_effect)
				{
					_supply = ptr->getSupply(now, own.Base.getSphereID()) - _base_supply + _supply;
					_supply = _supply > (int)starwar_sys.getParam().getMaxSupply() ? (int)starwar_sys.getParam().getMaxSupply() : _supply;
					_supply_cd = 0;
				}
			}
			{
				starPtr ptr = starwar_sys.getStar(_position.getId());
				if (ptr->getEffectType() == starwar::_supply_effect)
				{
					_base_supply = ptr->getSupply(now, own.Base.getSphereID());
					_supply_cd = now;
				}
			}
		}

		if (next)
		{
			if (!_position.empty())
				_position.popPath();
			if (!_position.empty())
			{
				int to_star_id = _position.frontPath();
				Json::Value arg;
				arg.append(to_star_id);
				starwar::message m(_position.getAction().getType(), arg, now, own.getOwnDataPtr());
				starwar_sys.pushMessage(m);
			}
			else
			{
				battleCdEffect(now);
			}
		}
		else
		{
			_battle_times = 0;
			_battle_cd = 0;
		}

		starwarLog(own.getOwnDataPtr(), _log_position, temp.getId(), _position.getId(), now, _supply, _base_supply, _supply_cd);
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerStarwar::received(int star_id)
	{
		if (star_id >= (int)_resource.size())
			_resource.insert(_resource.end(), star_id + 1 - _resource.size(), 0);
		return _resource[star_id];
	}

	void playerStarwar::packageResource(Json::Value& msg)
	{
		msg[starwar_resource_list_field_str] = Json::arrayValue;
		Json::Value& ref = msg[starwar_resource_list_field_str];
		starList& stars = starwar_sys.getStarList();
		int total = 0;
		for (starList::iterator iter = stars.begin(); iter != stars.end(); ++iter)
		{
			int num = (*iter)->getRecourse(own.Base.getSphereID()) - received((*iter)->getStarId());
			if (num > (int)(*iter)->getMaxOutput())
				num = (*iter)->getMaxOutput();
			total += num;
			ref.append(num);
		}
		//total += (starwar_sys.getUnityReward(own.Base.getSphereID()) - _unity_reward);
		msg[starwar_resource_total_field_str] = total;
	}

	int playerStarwar::getResource(int star_id)
	{
		int get_num = 0;

		if (star_id == -1)
		{
			starList& stars = starwar_sys.getStarList();
			for (starList::iterator iter = stars.begin(); iter != stars.end(); ++iter)
			{
				int num = (*iter)->getRecourse(own.Base.getSphereID()) - received((*iter)->getStarId());
				if (num > (int)(*iter)->getMaxOutput())
					num = (*iter)->getMaxOutput();
				_resource[(*iter)->getStarId()] = (*iter)->getRecourse(own.Base.getSphereID());
				get_num += num;
			}
		}
		else if (star_id == -2)
		{
			get_num += _unity_reward ? 0 : starwar_sys.getUnityReward(own.Base.getSphereID());
			_unity_reward = true;
		}
		else
		{
			starPtr ptr = starwar_sys.getStar(star_id);
			if (ptr != starPtr())
			{
				get_num = ptr->getRecourse(own.Base.getSphereID()) - received(star_id);
				if (get_num > (int)ptr->getMaxOutput())
					get_num = ptr->getMaxOutput();
				_resource[star_id] = ptr->getRecourse(own.Base.getSphereID());
			}
		}

		if (get_num > 0)
		{
			own.Base.alterSilver(get_num);
			helper_mgr.insertSaveAndUpdate(this);
			updateTask(starwar::_silver_num, get_num);
		}
		return get_num;
	}

	string playerStarwar::getFileName()
	{
		if (++_max_file_id > 135)
			_max_file_id = 1;

		helper_mgr.insertSaveAndUpdate(this);

		string str = boost::lexical_cast<string, int>(own.playerID);
		str += "/";
		str += boost::lexical_cast<string, unsigned>(_max_file_id);
		return str;
	}

	void playerStarwar::clearExploit()
	{
		int temp = _exploit;
		_exploit = 0;
		helper_mgr.insertSaveAndUpdate(this);
		starwarLog(own.getOwnDataPtr(), _log_flush_exploit, temp, (int)_exploit);
	}

	int playerStarwar::alterExploit(unsigned num)
	{
		int temp = _exploit;
		_exploit += num;
		helper_mgr.insertSaveAndUpdate(this);
		starwarLog(own.getOwnDataPtr(), _log_get_exploit, temp, (int)_exploit, num);
		updateTask(starwar::_exploit_num, (int)num);
		return temp;
	}

	void playerStarwar::onMain(bool flag, bool offline)
	{
		if (_on_main != flag)
		{
			_on_main = flag;
			if (!_on_main)
				_last_leave_time = na::time_helper::get_current_time();
			if (offline)
				helper_mgr.insertSaveAndUpdate(this);
		}
	}

	double playerStarwar::getAtkBuff(unsigned now)
	{
		return _buff[battleParam::_index_atk].getRate(now);
	}

	double playerStarwar::getDefBuff(unsigned now)
	{
		return _buff[battleParam::_index_def].getRate(now);
	}

	void playerStarwar::useTransferTimes(int num)
	{
		_get_transfer_times -= num;
		++_transfer_times;
		_transfer_cd = na::time_helper::get_current_time() + starwar_sys.getParam().getTransferCd();
		own.Item.removeItem(_transfer_item_id, num);

		updateTask(starwar::_transfer_times);
		activity_sys.updateActivity(own.getOwnDataPtr(), 0, activity::_transfer);

		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerStarwar::useItem(int position, int star_id, int player_id, playerDataPtr targetP /* = playerDataPtr() */)
	{
		if (position > (int)_items.size() || position < 1)
			return starwar::_client_error;
		starwarItem& item = _items[position - 1];
		if (item.empty())
			return starwar::_no_items;
		if (item.getType() != starwarItem::_sub_supply)
			return starwar::_client_error;

		if (_position.getType() == starwar::_in_star)
		{
			starPtr ptr = starwar_sys.getStar(_position.getId());
			if (ptr == starPtr())
				return starwar::_server_error;

			if (ptr->getPath(star_id) == pathPtr())
				return starwar::_not_access;
		}
		else
		{
			int path_id = _position.getId();
			if (star_id != path_id % 100 && star_id != path_id / 100)
				return starwar::_not_access;
		}


		starwar::effect e = starwar_sys.getParam().getItemEffect(item.getType(), item.getLv());

		if (star_id == star::mainstar[0] || star_id == star::mainstar[1]
			|| star_id == star::mainstar[2])
			return starwar::_client_error;
		starPtr ptr = starwar_sys.getStar(star_id);
		if (ptr == starPtr())
			return starwar::_client_error;
		if (ptr->getCountryId() == own.Base.getSphereID())
			return starwar::_your_own_star;

		unsigned now = na::time_helper::get_current_time();

		if (targetP == playerDataPtr())
		{
			if (!ptr->alterSup(player_id, 0 - (int)e._num))
				return starwar::_no_ememy;
		}
		else
		{
			int target_sup = targetP->Starwar.getSupply(now);
			if (!ptr->alterSup(now, targetP, 0 - (int)e._num))
				return starwar::_no_ememy;
			bool send_back = false;
			if (target_sup <= (int)e._num)
				send_back = true;

			targetP->Starwar.addReport(repFactory::create(now, own.Base.getName(), item.getLv(), send_back), false);
		}
		
		starwarLog(own.getOwnDataPtr(), _log_item, "", "", item.getType(), item.getLv(), (int)(targetP == playerDataPtr() ? starwar::_npc : starwar::_player), player_id);
		item.clear();
		helper_mgr.insertSaveAndUpdate(this);
		updateTask(_use_items);
		return starwar::_success;
	}

	int playerStarwar::useItem(int position)
	{
		if (position > (int)_items.size() || position < 1)
			return starwar::_client_error;
		starwarItem& item = _items[position - 1];
		if (item.empty())
			return starwar::_no_items;

		unsigned now = na::time_helper::get_current_time();
		starwar::effect e = starwar_sys.getParam().getItemEffect(item.getType(), item.getLv());

		switch (item.getType())
		{
			case starwarItem::_add_atk:
				_buff[battleParam::_index_atk].setValue(e._rate, now + starwar_sys.getParam().getItemCd(item.getType()));
				break;
			case starwarItem::_add_def:
				_buff[battleParam::_index_def].setValue(e._rate, now + starwar_sys.getParam().getItemCd(item.getType()));
				break;
			case starwarItem::_sub_supply:
				return starwar::_client_error;
			case starwarItem::_add_supply:
			{
				int supply_before = getSupply(now);
				if (supply_before >= (int)starwar_sys.getParam().getMaxSupply())
					return starwar::_supply_full;
				{
					int sup = getSupply(now);
					if (_position.getType() == starwar::_in_path)
					{
						pathPtr ptr = starwar_sys.getPath(_position.getId());
						ptr->alterSupply(own.playerID, sup);
						alterSupply(e._num, now);
					}
					else
					{
						starPtr ptr = starwar_sys.getStar(_position.getId());
						ptr->alterSup(now, own.getOwnDataPtr(), e._num);
					}
					starwarLog(own.getOwnDataPtr(), _log_item, supply_before, getSupply(now), (int)_log_supply_cost_type_item);
				}
			}
				break;
			default:
				break;
		}

		starwarLog(own.getOwnDataPtr(), _log_item, "", "", item.getType(), item.getLv());

		item.clear();
		helper_mgr.insertSaveAndUpdate(this);
		updateTask(_use_items);
		return starwar::_success;
	}

	int playerStarwar::openItem()
	{
		if (_items.size() == starwarItem::_max_item_num)
			return starwar::_no_items;
		if (own.Base.getAllGold() < _open_item_cost)
			return starwar::_gold_not_enough;
		own.Base.alterBothGold(0 - (int)_open_item_cost);
		_items.push_back(starwarItem());
		_items.back().setValue(starwar_sys.getParam().getItemType(), starwar_sys.getParam().getItemLv());
		helper_mgr.insertSaveAndUpdate(this);
		return starwar::_success;
	}

	int playerStarwar::flushItem()
	{
		bool flag = false;
		for (vector<starwarItem>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
		{
			if (!iter->empty())
			{
				flag = true;
				break;
			}
		}
		if (!flag)
			return starwar::_no_items;

		if (own.Base.getAllGold() < (int)_flush_item_cost)
			return starwar::_gold_not_enough;
		own.Base.alterBothGold(0 - (int)_flush_item_cost);
		for (vector<starwarItem>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
		{
			if (!iter->empty())
				iter->setValue(starwar_sys.getParam().getItemType(), starwar_sys.getParam().getItemLv());
		}

		helper_mgr.insertSaveAndUpdate(this);
		return starwar::_success;
	}

	int playerStarwar::buyItem(int position)
	{
		if (position > (int)_items.size() || position < 1)
			return starwar::_client_error;
		starwarItem& item = _items[position - 1];
		if (!item.empty())
			return starwar::_client_error;

		unsigned cost = starwar_sys.getParam().getItemCost(_item_buy_times);
		if (own.Base.getAllGold() < (int)cost)
			return starwar::_gold_not_enough;
		own.Base.alterBothGold(0 - (int)cost);

		++_item_buy_times;
		item.setValue(starwar_sys.getParam().getItemType(), starwar_sys.getParam().getItemLv());
		helper_mgr.insertSaveAndUpdate(this);
		return starwar::_success;
	}

	int playerStarwar::buyTransferTimes(int num)
	{
		if (num <= 0)
			return starwar::_client_error;

		if (_transfer_buy_times + num > starwar_sys.getParam().getMaxTransferTimes())
			return starwar::_times_limit;

		int all_gold = 0;

		for (int i = 0; i < num; ++i)
			all_gold += starwar_sys.getParam().getTransferCost(_transfer_buy_times + (unsigned)i);

		if (own.Base.getAllGold() < all_gold)
			return starwar::_gold_not_enough;

		if (!own.Item.canAddItem(_transfer_item_id, num))
			return starwar::_items_full;

		own.Item.addItem(_transfer_item_id, num);

		own.Base.alterBothGold(0 - all_gold);

		_transfer_buy_times += num;
		_get_transfer_times += num;
		helper_mgr.insertSaveAndUpdate(this);
		return starwar::_success;
	}

	int playerStarwar::clearTransferCd()
	{
		unsigned now = na::time_helper::get_current_time();
		if (now >= _transfer_cd)
			return starwar::_not_in_cd;

		int min = (_transfer_cd - now -1) / na::time_helper::MINUTE + 1;
		int cost = starwar_sys.getParam().getTransferCdCostPerMin() * min;

		if (own.Base.getAllGold() < cost)
			return starwar::_gold_not_enough;

		own.Base.alterBothGold(0 - cost);

		_transfer_cd = 0;
		helper_mgr.insertSaveAndUpdate(this);
		return starwar::_success;
	}

	void playerStarwar::calculateSupply(unsigned now)
	{
		if (_supply_cd != 0)
		{
			starPtr ptr = starwar_sys.getStar(_position.getId());
			int temp = ptr->getSupply(now, own.Base.getSphereID());
			_supply = temp - _base_supply + _supply;
			if (_supply > (int)starwar_sys.getParam().getMaxSupply())
				_supply = (int)starwar_sys.getParam().getMaxSupply();
			_base_supply = temp;
			_supply_cd = now;
		}
	}

	int playerStarwar::getSupply(unsigned now)
	{
		if (_supply_cd == 0)
			return _supply;
		else
		{
			starPtr ptr = starwar_sys.getStar(_position.getId());
			int supply = ptr->getSupply(now, own.Base.getSphereID()) - _base_supply + _supply;
			return supply > (int)starwar_sys.getParam().getMaxSupply() ? (int)starwar_sys.getParam().getMaxSupply() : supply;
		}
	}

	void playerStarwar::alterSupply(int num, unsigned now, int type)
	{
		int sup = getSupply(now);
		doAlterSup(now, num);
		starwarLog(own.getOwnDataPtr(), _log_supply_cost, sup, getSupply(now), type);
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerStarwar::resetTask()
	{
		_taskRecord.clear();
		starwar_sys.getTaskList(_taskRecord, own.getOwnDataPtr());
		_taskTypes.reset();
		for (vector<taskRecord>::iterator iter = _taskRecord.begin(); iter != _taskRecord.end(); ++iter)
		{
			if (iter->getState() == starwar::_task_running)
				_taskTypes.set(iter->getType());
		}
	}

	void playerStarwar::getTaskInfo(Json::Value& info)
	{
		info[starwar_task_record_field_str] = Json::arrayValue;
		Json::Value& ref = info[starwar_task_record_field_str];
		for (vector<taskRecord>::iterator iter = _taskRecord.begin(); iter != _taskRecord.end(); ++iter)
		{
			Json::Value temp;
			iter->getValue(temp, own.getOwnDataPtr());
			ref.append(temp);
		}
		info[starwar_time_field_str] = _next_term_time;
	}

	void playerStarwar::updateTask()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		getTaskInfo(msg[msgStr][1u]);
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::starwar_task_info_resp, msg);
	}

	void playerStarwar::updateTask(int type, int temp)
	{
		if (!_taskTypes.test(type))
			return;

		bool flag = false;
		for (vector<taskRecord>::iterator iter = _taskRecord.begin(); iter != _taskRecord.end(); ++iter)
		{
			if (iter->getState() != starwar::_task_running || iter->getType() != type)
				continue;

			if (iter->update(own.getOwnDataPtr(), temp))
				flag = true;

			if (iter->getState() == starwar::_task_finished)
				_taskTypes.reset(type);
		}

		if (flag)
			helper_mgr.insertSaveAndUpdate(this);
	}

	int playerStarwar::taskCommit(int task_id)
	{
		for (vector<taskRecord>::iterator iter = _taskRecord.begin(); iter != _taskRecord.end(); ++iter)
		{
			if (iter->getId() == task_id)
			{
				if (iter->getState() != starwar::_task_finished)
					return iter->getState();
				iter->getReward(own.getOwnDataPtr());
				iter->commit();
				starwarLog(own.getOwnDataPtr(), _log_task_reward, "", "", task_id, (int)action_add_silver, iter->getReward());
				helper_mgr.insertSaveAndUpdate(this);
				return starwar::_task_finished;
			}
		}
		return -1;
	}

	int playerStarwar::isTransferLimit(unsigned now, int num)
	{
		if (_transfer_times >= starwar_sys.getParam().getTransferTimesPerDay())
			return starwar::_times_limit;
		if (now < _transfer_cd)
			return starwar::_in_cd;
		if (!own.Item.canRemoveItem(_transfer_item_id, num))
			return starwar::_no_items;
		return starwar::_success;
	}

	int playerStarwar::isRaidLimit(unsigned now)
	{
		if (own.Base.getLevel() < (int)starwar_sys.getParam().getRaidEnableLvLimit())
			return starwar::_lv_limit;
		if (getSupply(now) <= (int)starwar_sys.getParam().getRaidCost())
			return starwar::_supply_not_enough;
		return _raid_times >= starwar_sys.getParam().getRaidTimesPerDay() ? starwar::_times_limit : starwar::_success;
	}

	int playerStarwar::getRankingReward()
	{
		if (_ranking_reward)
			return starwar::_no_reward;
		rankRecord record = starwar_sys.getOldRank(own.playerID, own.Base.getSphereID());
		int get_reward = starwar_sys.getParam().getRankingReward(record.getRank());
		own.Base.alterSilver(get_reward);
		_ranking_reward = true;
		starwarLog(own.getOwnDataPtr(), _log_ranking_reward, "", "", record.getRank(), record.getExploit(), (int)action_add_silver, get_reward);
		helper_mgr.insertSaveAndUpdate(this);
		return starwar::_success;
	}

	int playerStarwar::gather(unsigned now)
	{
		if (_gather_times <= 0)
			return starwar::_times_limit;
		if (now < _gather_cd)
			return starwar::_in_cd;
		--_gather_times;
		_gather_cd = now + 10 * na::time_helper::MINUTE;
		helper_mgr.insertSaveAndUpdate(this);
		return starwar::_success;
	}

	void playerStarwar::doRaid(unsigned now)
	{
		++_raid_times;
		alterSupply(0 - (int)starwar_sys.getParam().getRaidCost(), now);
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerStarwar::playerLogin()
	{
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][starwar_type_field_str] = 1;
		msg[msgStr][1u][starwar_time_field_str] = _last_leave_time;
		player_mgr.sendToPlayer(own.playerID, own.netID, gate_client::starwar_notice_unity_reward_resp, msg);
	}

	void playerStarwar::addBattleTimes()
	{
		++_battle_times;
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerStarwar::battleCdEffect(unsigned now)
	{
		_battle_cd = now + _battle_times * 4;
		_battle_times = 0;
		helper_mgr.insertSaveAndUpdate(this);
	}

	inline void playerStarwar::doAlterSup(unsigned now, int sup)
	{
		recalSup(now);
		_supply += sup;
		adjustSup();
	}

	inline void playerStarwar::recalSup(unsigned now)
	{
		if (_supply_cd != 0)
		{
			int star_id = _position.getId();
			starPtr ptr = starwar_sys.getStar(star_id);
			int star_sup = ptr->getSupply(now, own.Base.getSphereID());
			_supply = star_sup - _base_supply + _supply;
			_base_supply = star_sup;
			_supply_cd = now;
			adjustSup();
		}
	}

	inline void playerStarwar::adjustSup()
	{
		if (_supply < 0)
			_supply = 0;
		if (_supply >(int)starwar_sys.getParam().getMaxSupply())
			_supply = (int)starwar_sys.getParam().getMaxSupply();
	}

	void playerStarwar::settleNpc(int num)
	{
		_settle_npc_num += num;
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerStarwar::isSettleNpcLimit(int num)
	{
		if (getSettleNpcNum() < num)
			return starwar::_times_limit;

		return starwar::_success;
	}

	int playerStarwar::getSettleNpcNum(int title) const
	{
		static int num[_title_max] = { 0, 10, 8, 8, 6, 6, 6, 5, 5, 5, 5, 5 };
		if (title < 0 || title >= _title_max)
			title = 0;
		return num[title];
	}

	int playerStarwar::getSettleNpcNum()
	{
		int title = own.Ruler.getTitle();
		if (title != _old_title)
		{
			_old_title = title;
			_settle_npc_num = 0;
		}

		if (title < _title_king || title >= _title_max)
			return 0;

		return getSettleNpcNum(title) - _settle_npc_num;
	}
}
