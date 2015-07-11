#include "starwar_report.h"
#include "starwar_helper.h"

namespace gg
{
	namespace starwar
	{
		describeRep::describeRep(const mongo::BSONElement& obj)
			: _type(starwar::_report_error), _arg(-1), _time(0)
		{
			checkNotEoo(obj[starwar_type_field_str])
				_type = obj[starwar_type_field_str].Int();
			checkNotEoo(obj[starwar_id_field_str])
				_arg = obj[starwar_id_field_str].Int();
			checkNotEoo(obj[starwar_time_field_str])
				_time = obj[starwar_time_field_str].Int();
		}

		void describeRep::getValue(Json::Value& info)
		{
			info.append(_type);
			info.append(_arg);
			info.append(_time);
		}

		void describeRep::toBSON(mongo::BSONObjBuilder& obj)
		{
			obj << starwar_type_field_str << _type
				<< starwar_id_field_str << _arg
				<< starwar_time_field_str << _time;
		}

		supRep::supRep(const mongo::BSONElement& obj)
			: _lv(1), _time(0), _name(""), _dead(false)
		{
			checkNotEoo(obj[starwar_level_field_str])
				_lv = obj[starwar_level_field_str].Int();
			checkNotEoo(obj[starwar_name_field_str])
				_name = obj[starwar_name_field_str].String();
			checkNotEoo(obj[starwar_time_field_str])
				_time = obj[starwar_time_field_str].Int();
			checkNotEoo(obj[starwar_result_field_str])
				_dead = obj[starwar_result_field_str].Bool();
		}

		void supRep::getValue(Json::Value& info)
		{
			info.append(starwar::_report_3);
			info.append(_lv);
			info.append(_time);
			info.append(_name);
			info.append(_dead);
		}

		void supRep::toBSON(mongo::BSONObjBuilder& obj)
		{
			obj << starwar_type_field_str << (int)starwar::_report_3
				<< starwar_level_field_str << _lv
				<< starwar_time_field_str << _time
				<< starwar_name_field_str << _name
				<< starwar_result_field_str << _dead;
		}

		guildRep::guildRep(const mongo::BSONElement& obj)
			: _time(0), _star_id(0), _name("")
		{
			checkNotEoo(obj[starwar_time_field_str])
				_time = obj[starwar_time_field_str].Int();
			checkNotEoo(obj[starwar_id_field_str])
				_star_id = obj[starwar_id_field_str].Int();
			checkNotEoo(obj[starwar_name_field_str])
				_name = obj[starwar_name_field_str].String();
		}

		void guildRep::getValue(Json::Value& info)
		{
			info.append(starwar::_report_4);
			info.append(_star_id);
			info.append(_time);
			info.append(_name);
		}

		void guildRep::toBSON(mongo::BSONObjBuilder& obj)
		{
			obj << starwar_type_field_str << (int)starwar::_report_4
				<< starwar_time_field_str << _time
				<< starwar_id_field_str << _star_id
				<< starwar_name_field_str << _name;
		}

		battleRep::battleRep(const mongo::BSONElement& obj)
			: _result(0), _report_id(""), _target_name(""), _exploit(0), _sup(0), _target_sup(0), _bNpc(false), _silver(0), _title(-1)
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
				_sup = obj[starwar_supply_field_str].Int();
			checkNotEoo(obj[starwar_target_supply_field_str])
				_target_sup = obj[starwar_target_supply_field_str].Int();
			checkNotEoo(obj[starwar_is_npc_field_str])
				_bNpc = obj[starwar_is_npc_field_str].Bool();
			checkNotEoo(obj[starwar_reward_field_str])
				_silver = obj[starwar_reward_field_str].Int();
			checkNotEoo(obj[ruler_title_field_str])
				_title = obj[ruler_title_field_str].Int();
		}

		void battleRep::getValue(Json::Value& info)
		{
			info.append(_result);
			info.append(_report_id);
			info.append(_target_name);
			info.append(_exploit);
			info.append(_sup);
			info.append(_target_sup);
			info.append(_bNpc);
			info.append(_silver);
			info.append(_title);
		}

		void battleRep::toBSON(mongo::BSONObjBuilder& obj)
		{
			obj << starwar_type_field_str << (int)starwar::_report_5
				<< starwar_result_field_str << _result
				<< starwar_report_id_field_str << _report_id
				<< starwar_name_field_str << _target_name
				<< starwar_exploit_field_str << _exploit
				<< starwar_supply_field_str << _sup
				<< starwar_target_supply_field_str << _target_sup
				<< starwar_reward_field_str << _silver
				<< starwar_is_npc_field_str << _bNpc
				<< ruler_title_field_str << _title;
		}

		combineRep::combineRep(const mongo::BSONElement& obj)
		{
			int type = obj[starwar_type_field_str].Int();
			unsigned time = obj[starwar_time_field_str].Int();
			int arg = obj[starwar_id_field_str].Int();
			_describe = repFactory::create(type, time, arg);
			checkNotEoo(obj[starwar_report_list_field_str])
			{
				vector<mongo::BSONElement> ele = obj[starwar_report_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
					_reportList.push_back(repFactory::create(ele[i]));
			}
		}

		void combineRep::getValue(Json::Value& info)
		{
			_describe->getValue(info);
			Json::Value rep_array = Json::arrayValue;
			for (reportList::reverse_iterator iter = _reportList.rbegin(); iter != _reportList.rend(); ++iter)
			{
				Json::Value temp = Json::arrayValue;
				(*iter)->getValue(temp);
				rep_array.append(temp);
			}
			info.append(rep_array);
		}

		void combineRep::toBSON(mongo::BSONObjBuilder& obj)
		{
			_describe->toBSON(obj);
			mongo::BSONArrayBuilder rep_array;
			for (reportList::iterator iter = _reportList.begin(); iter != _reportList.end(); ++iter)
			{
				mongo::BSONObjBuilder temp;
				(*iter)->toBSON(temp);
				rep_array.append(temp.obj());
			}
			obj << starwar_report_list_field_str << rep_array.arr();
		}

		reportPtr repFactory::create(int type, unsigned time, int arg)
		{
			return creator<describeRep>::run(type, time, arg);
		}

		reportPtr repFactory::create(unsigned time, int star_id, const string& name)
		{
			return creator<guildRep>::run(time, star_id, name);
		}

		reportPtr repFactory::create(unsigned time, const string& name, int lv, bool dead)
		{
			return creator<supRep>::run(time, name, lv, dead);
		}

		reportPtr repFactory::create(const int result, const string& report_id, const string& target_name, int exploit, int sup, int target_sup, int silver, bool bNpc, int title /* = -1 */)
		{
			return creator<battleRep>::run(result, report_id, target_name, exploit, sup, target_sup, silver, bNpc, title);
		}

		reportPtr repFactory::create(const reportPtr& describe, const reportList& rep_list)
		{
			return creator<combineRep>::run(describe, rep_list);
		}

		reportPtr repFactory::create(const mongo::BSONElement& obj)
		{
			int type = _report_5;
			checkNotEoo(obj[starwar_type_field_str])
				type = obj[starwar_type_field_str].Int();
			switch (type)
			{
			case _report_0:
			case _report_1:
			case _report_2:
				return creator<combineRep>::run(obj);
			case _report_3:
				return creator<supRep>::run(obj);
			case _report_4:
				return creator<guildRep>::run(obj);
			case _report_5:
				return creator<battleRep>::run(obj);
			default:
				return reportPtr();
			}
		}

		mongo::BSONArray reportManager::toBSON()
		{
			mongo::BSONArrayBuilder obj;
			for (reportList::iterator iter = _reportList.begin(); iter != _reportList.end(); ++iter)
			{
				mongo::BSONObjBuilder temp;
				(*iter)->toBSON(temp);
				obj.append(temp.obj());
			}
			return obj.arr();
		}

		void reportManager::setFromBSON(const mongo::BSONElement& obj)
		{
			vector<mongo::BSONElement> ele = obj.Array();
			for (unsigned i = 0; i < ele.size(); ++i)
				push(repFactory::create(ele[i]));
		}

		void reportManager::push(reportPtr ptr)
		{
			_reportList.push_back(ptr);
			while (_reportList.size() > _max_rep)
				_reportList.pop_front();
		}

		void reportManager::getValue(Json::Value& info)
		{
			for (reportList::reverse_iterator iter = _reportList.rbegin(); iter != _reportList.rend(); ++iter)
			{
				Json::Value temp;
				(*iter)->getValue(temp);
				info.append(temp);
			}
		}

		void reportManager::updateLast(playerDataPtr d)
		{
			if (!_reportList.empty())
			{
				Json::Value msg;
				msg[msgStr][0u] = 0;
				_reportList.back()->getValue(msg[msgStr][1u]);
				player_mgr.sendToPlayer(d->playerID, gate_client::starwar_notice_report_resp, msg);
			}
		}
	}
}
