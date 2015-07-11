#include "starwar_helper.h"
#include "starwar_system.h"
#include "ruler_system.h"
#include "chat_system.h"
#include "time_helper.h"
#include "record_system.h"

namespace gg
{
	namespace starwar
	{
		playerInfo::playerInfo(const mongo::BSONElement& obj)
		{
			_player_id = obj[starwar_player_id_field_str].Int();
			_name = obj[starwar_name_field_str].String();
			_country_id = obj[starwar_country_id_field_str].Int();
			_lv = obj[starwar_level_field_str].Int();
			_face_id = obj[starwar_face_id_field_str].Int();
			_title = obj[starwar_title_field_str].Int();
		}

		inline mongo::BSONObj playerInfo::toBSON()
		{
			return BSON(starwar_player_id_field_str << _player_id
				<< starwar_name_field_str << _name
				<< starwar_country_id_field_str << _country_id
				<< starwar_level_field_str << _lv
				<< starwar_face_id_field_str << _face_id
				<< starwar_title_field_str << _title);
		}

		pathItem::pathItem(const mongo::BSONElement& obj)
			: _playerInfo(obj[starwar_player_info_field_str])
		{
			_type = obj[starwar_type_field_str].Int();
			_side = obj[starwar_side_field_str].Int();
			_time = obj[starwar_time_field_str].Int();
			_distance = obj[starwar_distance_field_str].Int();
		}

		inline mongo::BSONObj pathItem::toBSON()
		{
			return BSON(starwar_player_info_field_str << _playerInfo.toBSON()
				<< starwar_type_field_str << _type
				<< starwar_side_field_str << _side
				<< starwar_time_field_str << _time
				<< starwar_distance_field_str << _distance);
		}

		inline void pathItem::getValue(Json::Value& info)
		{
			info.append(_playerInfo.getPlayerId());
			info.append(_playerInfo.getCountryId());
			info.append(_time);
			info.append(_distance);
			if (_playerInfo.getTitle() != _no_title)
			{
				info.append(_playerInfo.getTitle());
				info.append(_playerInfo.getName());
			}
		}

		inline void pathItem::getExtraValue(Json::Value& info)
		{
			info.append(0);
			info.append(_playerInfo.getPlayerId());
			info.append(_playerInfo.getCountryId());
			info.append(_time);
			info.append(_distance);
			if (_playerInfo.getTitle() != _no_title)
			{
				info.append(_playerInfo.getTitle());
				info.append(_playerInfo.getName());
			}
		}

		inline void updateItem::getValue(Json::Value& info)
		{
			info.append(_type);
			info.append(_player_id);
			if (_type == updateItem::_enter)
			{
				info.append(_country_id);
				info.append(_time);
				info.append(_distance);
				if (_title != _no_title)
				{
					info.append(_title);
					info.append(_name);
				}
			}
		}

		guild::guild(const mongo::BSONElement& obj)
		{
			_type = obj[starwar_type_field_str].Int();
			_id = obj[starwar_player_id_field_str].Int();
			if (_type == _player)
			{
				_name = obj[starwar_name_field_str].String();
				_face_id = obj[starwar_face_id_field_str].Int();
				_time = obj[starwar_time_field_str].Int();
			}
			else if (_type == _npc)
			{
				_time = 0;
				_supply = obj[starwar_supply_field_str].Int();
			}
			else if (_type == _player_npc)
			{
				_owner_id = obj[starwar_owner_id_field_str].Int();
				_title = obj[ruler_title_field_str].Int();
				_time = obj[starwar_time_field_str].Int();
				_supply = obj[starwar_supply_field_str].Int();
			}
		}

		inline mongo::BSONObj guild::toBSON() const
		{
			mongo::BSONObjBuilder obj;
			obj << starwar_type_field_str << _type
				<< starwar_player_id_field_str << _id;
			if (_type == _player)
			{
				obj << starwar_name_field_str << _name
					<< starwar_face_id_field_str << _face_id
					<< starwar_time_field_str << _time;
			}
			else if (_type == _npc)
			{
				obj << starwar_supply_field_str << _supply;
			}
			else if (_type == _player_npc)
			{
				obj << ruler_title_field_str << _title
					<< starwar_time_field_str << _time
					<< starwar_owner_id_field_str << _owner_id
					<< starwar_supply_field_str << _supply;
			}
			return obj.obj();
		}

		inline void guild::getValue(Json::Value& info, int star_supply) const
		{
			info.append(_type);
			info.append(_id);
			if (_type == _player)
			{
				info.append(_name);
				info.append(_face_id);
				info.append(getSupply(star_supply));
			}
			else if (_type == _npc)
			{
				info.append(_supply);
			}
			else if (_type == _player_npc)
			{
				info.append(_title);
				info.append(_supply);
			}
		}

		int guild::getSupply(int star_supply /* = 0 */) const
		{
			if (star_supply == 0 || _type != _player)
				return _supply;

			int total = star_supply - _base_supply + _supply;
			return total > (int)starwar_sys.getParam().getMaxSupply() ? starwar_sys.getParam().getMaxSupply() : total;
		}

		void guild::alterSupply(int supply, int base_supply /* = 0 */) const
		{
			_supply = supply;
			_base_supply = base_supply;
		}

		void guildList::push(const mongo::BSONElement& obj)
		{
			guild g(obj);
			_guildSet.insert(g);
			if (g.getType() == _player_npc)
				++_player_npc_num;
		}

		void guildList::push(int player_id, const string& name, int face_id, unsigned time, int supply, int base_supply)
		{
			_guildSet.insert(guild(player_id, name, face_id, time, supply, base_supply));
		}

		void guildList::push(int npc_id, int supply)
		{
			_guildSet.insert(guild(npc_id, supply));
		}

		void guildList::push(int player_npc_id, int owner_id, int title, unsigned time, int supply)
		{
			_guildSet.insert(guild(player_npc_id, owner_id, title, time, supply));
			++_player_npc_num;
		}

		void guildList::pop(int player_id, unsigned time)
		{
			_guildSet.erase(guild::makeKey(player_id, time));
		}

		void guildList::pop(guildPtr ptr)
		{
			if (ptr->getType() == _player_npc)
				--_player_npc_num;
			_guildSet.erase(ptr);
		}

		void guildList::pop_front()
		{
			if (_guildSet.begin()->getType() == _player_npc)
				--_player_npc_num;
			_guildSet.erase(_guildSet.begin());
		}

		const guild& guildList::front() const
		{
			return *(_guildSet.begin());
		}

		bool guildList::find(int player_id, unsigned time, guildPtr& ptr)
		{
			ptr = _guildSet.find(guild::makeKey(player_id, time));
			return ptr != _guildSet.end();
		}

		bool guildList::find(int npc_id, guildPtr& ptr)
		{
			for (guildSet::iterator iter = _guildSet.begin(); iter != _guildSet.end(); ++iter)
			{
				if (iter->getId() == npc_id)
				{
					ptr = iter;
					return true;
				}
			}
			return false;
		}

		void guildList::run(const handler& h)
		{
			for (guildSet::iterator iter = _guildSet.begin(); iter != _guildSet.end(); ++iter)
				h(*iter);
		}

		void guildList::getValue(Json::Value& info, int begin, int count, int star_supply)
		{
			info[starwar_guild_count_field_str] = (int)_guildSet.size();
			info[starwar_guilds_list_field_str] = Json::arrayValue;
			if (begin < 1 || count < 1)
				return;
			Json::Value& ref = info[starwar_guilds_list_field_str];
			int end = begin + count;
			int i = 0;
			guildSet::iterator iter = _guildSet.begin();
			for (; iter != _guildSet.end(); ++iter)
			{
				if (++i == begin)
					break;
			}
			for (; iter != _guildSet.end(); ++iter)
			{
				if (i++ == end)
					break;
				Json::Value item;
				iter->getValue(item, star_supply);
				ref.append(item);
			}
		}

		inline mongo::BSONArray guildList::toBSON() const
		{
			mongo::BSONArrayBuilder obj;
			if(!_guildSet.empty() && _guildSet.begin()->getType() == _npc)
			{
				for (guildSet::const_iterator iter = _guildSet.begin(); iter != _guildSet.end(); ++iter)
					obj.append(iter->toBSON());
			}
			return obj.arr();
		}

		pathItemMgr::pathItemMgr(int path_id)
			: _playerList(getInitPlayerList()), _leftIter(_playerList.begin()), _rightIter(++_playerList.begin()), _path_id(path_id)
		{
			_count[_left_side] = 0;
			_count[_right_side] = 0;
			_distance = -1;
			_def_speed[_left_side] = -1;
			_def_speed[_right_side] = -1;
			_rate = 1;
		}

		pathItemList pathItemMgr::getInitPlayerList()
		{
			pathItemList pL;
			pL.push_back(pathItem());
			pL.push_back(pathItem());
			return pL;
		}

		int pathItemMgr::init(int side, int cost_time)
		{
			if (_distance == -1)
			{
				_distance = _default_speed * cost_time;
				_def_speed[side] = _default_speed;
				return _def_speed[side];
			}
			else
			{
				_def_speed[side] = divide32(_distance, cost_time);
				return _def_speed[side];
			}
		}

		void pathItemMgr::setFromBSON(const mongo::BSONElement& obj)
		{
			vector<mongo::BSONElement> ele = obj.Array();
			for (unsigned i = 0; i < ele.size(); ++i)
				_playerList.insert(_rightIter, pathItem(ele[i]));

			pLIter temp = _leftIter;
			++temp;
			for (; temp != _rightIter; ++temp)
			{
				pLIter tIter = getNext(temp);
				setTriggerTime(temp, getTriggerTime(temp, tIter));
			}
		}

		inline mongo::BSONArray pathItemMgr::toBSON() const
		{
			mongo::BSONArrayBuilder obj;
			pLIter iter = _leftIter;
			++iter;
			for (; iter != _rightIter; ++iter)
				obj.append(iter->toBSON());
			return obj.arr();
		}

		void pathItemMgr::clear()
		{
			_updateItems.clear();
			_sortHelper.clear();
			_playerList.clear();
			_playerList = getInitPlayerList();
			_leftIter = _playerList.begin();
			_rightIter = ++_playerList.begin();
			_count[_left_side] = 0;
			_count[_right_side] = 0;
		}

		void pathItemMgr::run(const handler& h)
		{
			pLIter iter = _leftIter;
			++iter;
			for (; iter != _rightIter; ++iter)
				h(*iter);
		}

		void pathItemMgr::alterSup(int player_id, int sup)
		{
			pLIter iter = _leftIter;
			++iter;
			for (; iter != _rightIter; ++iter)
			{
				if (iter->getPlayerInfo().getPlayerId() == player_id)
				{
					iter->getPlayerInfo().alterSupply(sup);
					return;
				}
			}
		}

		void pathItemMgr::reset(int rate, unsigned now)
		{
			if (_rate != rate)
			{
				_updateItems.clear();
				//_sortHelper.clear();

				pLIter iter = _leftIter;
				++iter;
				for (; iter != _rightIter; ++iter)
				{
					int distance = (now - iter->getTime()) * _def_speed[iter->getSide()] * _rate + iter->getDistance();
					iter->setDistance(distance > _distance ? _distance : distance);
					iter->setTime(now);
				}

				_rate = rate;

				iter = _leftIter;
				++iter;
				for (; iter != _rightIter; ++iter)
					setTriggerTime(iter, getTriggerTime(iter, getNext(iter)));
			}
		}

		void pathItemMgr::kickPlayer(int player_id)
		{
			pLIter iter = _leftIter;
			++iter;
			for (; iter != _rightIter; ++iter)
			{
				if (iter->getPlayerInfo().getPlayerId() == player_id)
				{
					pop(iter, updateItem::_arrive);
					return;
				}
			}
		}

		void pathItemMgr::pushMove(const pathItem& item)
		{
			pLIter iIter;
			if (item.getSide() == _left_side)
			{
				iIter = _leftIter;
				++iIter;
			}
			else
				iIter = _rightIter;

			iIter = _playerList.insert(iIter, item);

			pLIter tIter = getNext(iIter);
			int meet_time = getTriggerTime(iIter, tIter);
			setTriggerTime(iIter, meet_time);
			if (tIter != _leftIter && tIter != _rightIter && tIter->getSide() != iIter->getSide())
				setTriggerTime(tIter, meet_time);
		}

		void pathItemMgr::pushRaid(const pathItem& item)
		{
			pLIter iIter;
			if (item.getSide() == _left_side)
			{
				iIter = _leftIter;
				++iIter;
			}
			else
				iIter = _rightIter;

			iIter = _playerList.insert(iIter, item);
			int meet_time = getTriggerTime(iIter, item.getSide() == _left_side ? _rightIter : _leftIter);
			setTriggerTime(iIter, meet_time);
		}

		void pathItemMgr::push(const pathItem& item)
		{
			++_count[item.getSide()];
			++starwar_sys.getPlaneCount();
			const playerInfo& player_info = item.getPlayerInfo();
			_updateItems.push_back(
				updateItem(updateItem::_enter, player_info.getPlayerId(), player_info.getCountryId(), item.getTime(), item.getDistance()
				, player_info.getTitle(), item.getSide(), player_info.getName()));

			if (item.getType() == _move)
				pushMove(item);
			else if (item.getType() == _raid)
				pushRaid(item);
		}

		void pathItemMgr::pop(const pLIter& iter, int type)
		{
			--starwar_sys.getPlaneCount();
			--_count[iter->getSide()];
			const playerInfo& player_info = iter->getPlayerInfo();
			_updateItems.push_back(
				updateItem(type, player_info.getPlayerId(), player_info.getCountryId(), iter->getTime(), iter->getDistance()
				, player_info.getTitle(), iter->getSide(), player_info.getName()));

			_sortHelper.pop(iter);
			if (iter->getType() == _raid)
			{
				_playerList.erase(iter);
			}
			else
			{
				pLIter left_iter = getLeft(iter);
				pLIter right_iter = getRight(iter);
				_playerList.erase(iter);

				if (left_iter != _leftIter && left_iter->getSide() == _left_side)
				{
					int meet_time = getTriggerTime(left_iter, right_iter);
					setTriggerTime(left_iter, meet_time);
				}
				if (right_iter != _rightIter && right_iter->getSide() == _right_side)
				{
					int meet_time = getTriggerTime(right_iter, left_iter);
					setTriggerTime(right_iter, meet_time);
				}
			}
		}

		void pathItemMgr::swap(pLIter& left_iter, pLIter& right_iter)
		{
			pathItem info = *left_iter;
			*left_iter = *right_iter;
			*right_iter = info;

			pLIter tIter = getLeft(left_iter);
			if (tIter != _leftIter && tIter->getSide() == _left_side)
			{
				int meet_time = getTriggerTime(tIter, left_iter);
				setTriggerTime(tIter, meet_time);
			}

			{
				int meet_time = getTriggerTime(left_iter, tIter);
				setTriggerTime(left_iter, meet_time);
			}

			tIter = getRight(right_iter);

			{
				int meet_time = getTriggerTime(right_iter, tIter);
				setTriggerTime(right_iter, meet_time);
			}

			if (tIter != _rightIter && tIter->getSide() == _right_side)
			{
				int meet_time = getTriggerTime(tIter, right_iter);
				setTriggerTime(tIter, meet_time);
			}
		}

		int pathItemMgr::getTriggerTime(const pLIter& left_iter, const pLIter& right_iter)
		{
			if (right_iter == _leftIter || right_iter == _rightIter)
				return left_iter->getTime() + divide32(_distance - left_iter->getDistance(), _def_speed[left_iter->getSide()] * _rate);
			else
			{
				if (left_iter->getSide() == right_iter->getSide())
					return -1;
				else
				{
					int left_speed = _def_speed[left_iter->getSide()] * _rate;
					int right_speed = _def_speed[right_iter->getSide()] * _rate;
					unsigned max_time = left_iter->getTime() > right_iter->getTime() ? left_iter->getTime() : right_iter->getTime();
					unsigned min_time = left_iter->getTime() > right_iter->getTime() ? right_iter->getTime() : left_iter->getTime();
					int speed = left_iter->getTime() > right_iter->getTime() ? right_speed : left_speed;
					i64 L = (i64)_distance - right_iter->getDistance() - left_iter->getDistance() - (max_time - min_time) * speed;
					i64 s = (i64)left_speed + (i64)right_speed;
					return (int)divide64(L, s) + max_time;
				}
			}
		}

		void pathItemMgr::setTriggerTime(pLIter& iter, int time)
		{
			if (iter->getTriggerTime() != time)
			{
				if (iter->getTriggerTime() != -1)
					_sortHelper.pop(iter);
				iter->setTriggerTime(time);
				if (iter->getTriggerTime() != -1)
					_sortHelper.push(iter);
			}
		}

		Json::Value pathItemMgr::getInfo()
		{
			if (empty())
				return Json::nullValue;
			Json::Value info = Json::arrayValue;
			info.append(_path_id);
			pLIter iter = _leftIter;
			++iter;
			Json::Value left_value = Json::arrayValue;
			Json::Value right_value = Json::arrayValue;
			for (; iter != _rightIter; ++iter)
			{
				Json::Value temp = Json::arrayValue;
				iter->getValue(temp);
				if (iter->getSide() == _left_side)
					left_value.append(temp);
				else
					right_value.append(temp);
			}
			info.append(left_value);
			info.append(right_value);
			return info;
		}

		Json::Value pathItemMgr::getExtraInfo()
		{
			if (empty())
				return Json::nullValue;
			Json::Value info = Json::arrayValue;
			info.append(_path_id);
			pLIter iter = _leftIter;
			++iter;
			Json::Value left_value = Json::arrayValue;
			Json::Value right_value = Json::arrayValue;
			for (; iter != _rightIter; ++iter)
			{
				Json::Value temp = Json::arrayValue;
				iter->getExtraValue(temp);
				if (iter->getSide() == _left_side)
					left_value.append(temp);
				else
					right_value.append(temp);
			}
			info.append(left_value);
			info.append(right_value);
			return info;
		}

		Json::Value pathItemMgr::getChangeInfo()
		{
			if (_updateItems.empty())
				return Json::nullValue;
			Json::Value update_info = Json::arrayValue;
			update_info.append(_path_id);
			Json::Value left_value = Json::arrayValue;
			Json::Value right_value = Json::arrayValue;
			for (updateItems::iterator iter = _updateItems.begin(); iter != _updateItems.end(); ++iter)
			{
				Json::Value temp;
				iter->getValue(temp);
				if (iter->getSide() == _left_side)
					left_value.append(temp);
				else
					right_value.append(temp);
			}
			update_info.append(left_value);
			update_info.append(right_value);
			_updateItems.clear();
			return update_info;
		}

		bool pathItemMgr::getMin(pLIter& iter)
		{
			return _sortHelper.getMin(iter);
		}

		pLIter pathItemMgr::getNext(const pLIter& iter)
		{
			pLIter nextIter = iter;
			if (iter->getType() == _move)
			{
				if (iter->getSide() == _left_side)
					nextIter = getRight(iter);
				else
					nextIter = getLeft(iter);
			}
			else if (iter->getType() == _raid)
			{
				if (iter->getSide() == _left_side)
					return _rightIter;
				return _leftIter;
			}
			return nextIter;
		}

		pLIter pathItemMgr::getLeft(const pLIter& iter)
		{
			pLIter temp = iter;
			while (true)
			{
				--temp;
				if (temp == _leftIter || temp->getType() == _move)
					return temp;
			}
		}

		pLIter pathItemMgr::getRight(const pLIter& iter)
		{
			pLIter temp = iter;
			while (true)
			{
				++temp;
				if (temp == _rightIter || temp->getType() == _move)
					return temp;
			}
		}

		void pathItemMgr::getTargetList(list<pLIter>& target_list, int country_id, int side)
		{
			if (side == _left_side)
			{
				pLIter iter = _leftIter;
				++iter;
				for (; iter != _rightIter; ++iter)
				{
					if (iter->getType() == _move && iter->getPlayerInfo().getCountryId() != country_id)
						target_list.push_back(iter);
				}
			}
			else
			{
				pLIter iter = _rightIter;
				--iter;
				for (; iter != _leftIter; --iter)
				{
					if (iter->getType() == _move && iter->getPlayerInfo().getCountryId() != country_id)
						target_list.push_back(iter);
				}
			}
		}

		pLIter pathItemMgr::getTargetList(list<pLIter>& target_list, int player_id)
		{
			pLIter iter = _leftIter;
			++iter;
			for (; iter != _rightIter; ++iter)
			{
				if (iter->getPlayerInfo().getPlayerId() == player_id)
					break;
			}
			if (iter == _rightIter)
				return _rightIter;

			int country_id = iter->getPlayerInfo().getCountryId();
			pLIter target = iter;
			if (iter->getSide() == _left_side)
			{
				++iter;
				for (; iter != _rightIter; ++iter)
				{
					if (iter->getType() == _move && iter->getPlayerInfo().getCountryId() != country_id)
						target_list.push_back(iter);
				}
			}
			else
			{
				--iter;
				for (; iter != _leftIter; --iter)
				{
					if (iter->getType() == _move && iter->getPlayerInfo().getCountryId() != country_id)
						target_list.push_back(iter);
				}
			}
			return target;
		}

		path::path(int path_id, int left_star, int right_star)
			: _path_id(path_id), _playerMgr(_path_id), _pathInfo(Json::arrayValue), _pathChangeInfo(Json::nullValue)
		{
			_linked_star[_left_side] = left_star;
			_linked_star[_right_side] = right_star;
		}

		void path::setPathType(int from_star, int to_star, int type)
		{
			int side = from_star == _linked_star[_left_side] ? _left_side : _right_side;
			_path_type[side] = type;
		}

		void path::reset(unsigned now)
		{
			starwar_sys.getTrigger().pop(_eventInfo.getItem());
			_playerMgr.run(boost::bind(&path::sendBack, this, _1, now));
			_playerMgr.clear();
			_pathInfo = Json::arrayValue;
		}

		void path::setCostTime(int from_star, int to_star, int cost_time)
		{
			//cost_time /= 100;
			int side = from_star == _linked_star[_left_side] ? _left_side : _right_side;
			_cost_time[side] = cost_time;
			_playerMgr.init(side, cost_time);
		}

		int path::getCostTime(int from_id, int to_id) const
		{
			int side = from_id == _linked_star[_left_side] ? _left_side : _right_side;
			return _cost_time[side];
		}

		void path::resetRate(int rate, unsigned now)
		{
			_playerMgr.reset(rate, now);
			resetEvent();
		}

		bool path::kickPlayer(int player_id)
		{
			_playerMgr.kickPlayer(player_id);
			resetEvent();
			return true;
		}

		void path::load(int type, unsigned time, const playerInfo& player_info, int from_star_id)
		{
			int side = _linked_star[_left_side] == from_star_id? _left_side : _right_side;
			pathItem item(player_info, type, side, time);
			_playerMgr.push(item);
			resetEvent();
		}

		int path::doEnter(int type, unsigned time, playerDataPtr d, int to_star_id)
		{
			int side = _linked_star[_left_side] == to_star_id ? _right_side : _left_side;
			pathItem item(type, time, d, side);
			_playerMgr.push(item);
			resetEvent();
			return _success;
		}

		int path::doTransfer(unsigned time, playerDataPtr d, int to_star_id, bool flag /* = false */)
		{
			int side = _linked_star[_left_side] == to_star_id ? _right_side : _left_side;
			if (flag)
			{
				pLIter iter = getTargetList(d->playerID);
				if (iter == _playerMgr.rightIter())
					return _id_not_found;

				d->Starwar.setAction(_transfer);
				_playerMgr.pop(iter, updateItem::_arrive);
				resetEvent();

				vector<string> fields;
				fields.push_back(boost::lexical_cast<string, int>(_transfer));
				fields.push_back(boost::lexical_cast<string, int>(getLinkedStarId(to_star_id)));
				fields.push_back(boost::lexical_cast<string, int>(to_star_id));
				StreamLog::Log(starwarLogDBStr, d, "", "", fields, playerStarwar::_log_move);
			}
			else
				getTargetList(d->Base.getSphereID(), side);

			playerInfo player_info(d, time);

			if (_transferTargetList.empty())
				return doLeave(_transfer, time, player_info, to_star_id);
			else
			{
				int from_star_id = _linked_star[side];
				_cacheInfo.setValue(_transfer, time, player_info, from_star_id);
				return postToAttack(_transfer);
			}
		}

		int path::doLeave(int type, unsigned time, const playerInfo& player_info, int to_star_id)
		{
			starPtr ptr = starwar_sys.getStar(to_star_id);
			if (ptr == starPtr())
				return _id_not_found;
			int from_star_id = _linked_star[_left_side] == to_star_id ? _linked_star[_right_side] : _linked_star[_left_side];
			int result = ptr->doEnter(type, time, player_info, from_star_id);
			return result;
		}

		void path::sendBack(const pathItem& item, unsigned now)
		{
			starwar_sys.sendBack(now, item.getPlayerInfo().getPlayerId(), item.getPlayerInfo().getName()
				, item.getPlayerInfo().getFaceId(), item.getPlayerInfo().getCountryId(), false);
		}

		void path::update(bool flag)
		{
			if (flag)
			{
				_pathChangeInfo = _playerMgr.getChangeInfo();
				if (starwar_sys.getPlaneCount() <= _max_package_size)
				{
					_pathInfo = _playerMgr.getInfo();
					_extra = false;
				}
				else
				{
					_pathInfo = _playerMgr.getExtraInfo();
					_extra = true;
				}
			}
			else
			{
				_pathChangeInfo = _playerMgr.getChangeInfo();
				if (_pathChangeInfo != Json::nullValue && _pathChangeInfo.size() != 0)
				{
					if (starwar_sys.getPlaneCount() <= _max_package_size)
					{
						_pathInfo = _playerMgr.getInfo();
						_extra = false;
					}
					else
					{
						_pathInfo = _playerMgr.getExtraInfo();
						_extra = true;
					}
				}
				else
				{
					if (starwar_sys.getPlaneCount() <= _max_package_size && _extra)
					{
						_pathInfo = _playerMgr.getInfo();
						_extra = false;
					}
					else if (starwar_sys.getPlaneCount() > _max_package_size && !_extra)
					{
						_pathInfo = _playerMgr.getExtraInfo();
						_extra = true;
					}
				}
			}
		}

		void path::getTargetList(int country_id, int side)
		{
			_transferTargetList.clear();
			_playerMgr.getTargetList(_transferTargetList, country_id, side);
		}

		pLIter path::getTargetList(int player_id)
		{
			_transferTargetList.clear();
			return _playerMgr.getTargetList(_transferTargetList, player_id);
		}

		int path::postToAttack(int type)
		{
			playerInfo player_info, target_info;
			int star_id, target_star_id;
			unsigned now;
			if (type == _transfer)
			{
				player_info = _cacheInfo.getPlayerInfo();
				target_info = _transferTargetList.front()->getPlayerInfo();
				star_id = _cacheInfo.getFromStarId();
				target_star_id = _linked_star[_transferTargetList.front()->getSide()];
				now = _cacheInfo.getTime();
			}
			else
			{
				pLIter iter = _eventInfo.getIter();
				pLIter next_iter = _playerMgr.getNext(iter);
				player_info = iter->getPlayerInfo();
				target_info = next_iter->getPlayerInfo();
				star_id = _linked_star[iter->getSide()];
				target_star_id = _linked_star[next_iter->getSide()];
				now = _eventInfo.getItem().getTime();
			}

			starPtr ptr = starwar_sys.getStar(star_id);
			starPtr other_ptr = starwar_sys.getStar(target_star_id);

			Json::Value msg;
			msg[msgStr] = Json::arrayValue;
			msg[msgStr].append(_in_path);
			msg[msgStr].append(player_info.getPlayerId());
			msg[msgStr].append(ptr->getAtkEffect(player_info.getCountryId()));
			msg[msgStr].append(ptr->getDefEffect(player_info.getCountryId()));
			msg[msgStr].append(target_info.getPlayerId());
			msg[msgStr].append(ptr->getAtkEffect(target_info.getCountryId()));
			msg[msgStr].append(ptr->getDefEffect(target_info.getCountryId()));
			msg[msgStr].append(_path_id);
			msg[msgStr].append(now);
			msg[msgStr].append(type);
			string str = msg.toStyledString();
			na::msg::msg_json m(gate_client::starwar_attack_with_player_inner_req, str);
			player_mgr.postMessage(m);
			return _async_throw;
		}

		void path::attackResp(int type, int supply, int target_supply)
		{
			int target_country = -1;
			unsigned now = 0;
			if (type == _transfer)
			{
				pLIter iter = _transferTargetList.front();
				target_country = _cacheInfo.getTargetCountry(iter->getPlayerInfo().getCountryId());
				now = _cacheInfo.getTime();
				_cacheInfo.alterSupply(supply);
				iter->alterSupply(target_supply);

				if (target_supply <= 0)
				{
					starwar_sys.sendBack(now, iter->getPlayerInfo().getPlayerId(), iter->getPlayerInfo().getName(), iter->getPlayerInfo().getFaceId()
						, iter->getPlayerInfo().getCountryId());
					_transferTargetList.pop_front();
					_playerMgr.pop(iter, updateItem::_attack);
					resetEvent();
				}
				if (supply > 0)
				{
					if (_transferTargetList.empty())
					{
						int side = _linked_star[_left_side] == _cacheInfo.getFromStarId() ? _right_side : _left_side;
						int to_star_id = _linked_star[side];
						doLeave(_transfer, _cacheInfo.getTime(), _cacheInfo.getPlayerInfo(), to_star_id);
					}
					else
						postToAttack(type);
					return;
				}
				else
				{
					starwar_sys.sendBack(_cacheInfo.getTime(), _cacheInfo.getPlayerInfo().getPlayerId(), _cacheInfo.getPlayerInfo().getName()
						, _cacheInfo.getPlayerInfo().getFaceId(), _cacheInfo.getPlayerInfo().getCountryId());
				}
			}
			else
			{
				pLIter iter = _eventInfo.getIter();
				pLIter next_iter = _playerMgr.getNext(iter);
				target_country = next_iter->getPlayerInfo().getCountryId();
				now = iter->getTriggerTime();
				iter->alterSupply(supply);
				next_iter->alterSupply(target_supply);

				if (supply <= 0 && target_supply <= 0)
				{
					starwar_sys.sendBack(now, iter->getPlayerInfo().getPlayerId(), iter->getPlayerInfo().getName(), iter->getPlayerInfo().getFaceId()
						, iter->getPlayerInfo().getCountryId());
					starwar_sys.sendBack(now, next_iter->getPlayerInfo().getPlayerId(), next_iter->getPlayerInfo().getName(), next_iter->getPlayerInfo().getFaceId()
						, next_iter->getPlayerInfo().getCountryId());
					_playerMgr.pop(iter, updateItem::_attack);
					_playerMgr.pop(next_iter, updateItem::_attack);
				}
				else
				{
					if (supply <= 0)
					{
						starwar_sys.sendBack(now, iter->getPlayerInfo().getPlayerId(), iter->getPlayerInfo().getName(), iter->getPlayerInfo().getFaceId()
							, iter->getPlayerInfo().getCountryId());
						_playerMgr.pop(iter, updateItem::_attack);
					}
					if (target_supply <= 0)
					{
						starwar_sys.sendBack(now, next_iter->getPlayerInfo().getPlayerId(), next_iter->getPlayerInfo().getName(), next_iter->getPlayerInfo().getFaceId()
							, next_iter->getPlayerInfo().getCountryId());
						_playerMgr.pop(next_iter, updateItem::_attack);
					}
				}
				_eventInfo.getItem().clear();
			}
			resetEvent();
			starwar_sys.callBack(_report_type_0, target_country, now);
		}

		void path::resetEvent()
		{
			if (!_eventInfo.getItem().empty())
				starwar_sys.getTrigger().pop(_eventInfo.getItem());

			pLIter minIter;
			if (!_playerMgr.getMin(minIter))
				return;
			setEvent(minIter);
		}

		void path::setEvent(const pLIter& minIter)
		{
			pLIter tIter = _playerMgr.getNext(minIter);
			triggerItem item;
			if (tIter == _playerMgr.leftIter() || tIter == _playerMgr.rightIter())
				item.setValue(_arrive, _path_id, minIter->getTriggerTime());
			else
			{
				if (minIter->getPlayerInfo().getCountryId() == tIter->getPlayerInfo().getCountryId())
					item.setValue(_meet, _path_id, minIter->getTriggerTime());
				else
					item.setValue(_attack, _path_id, minIter->getTriggerTime());
			}

			_eventInfo.setValue(item, minIter);
			starwar_sys.getTrigger().push(item);
		}

		void path::alterSupply(int player_id, int sup)
		{
			_playerMgr.alterSup(player_id, sup);
		}

		int path::handleEventReq()
		{
			int result = -1;
			switch (_eventInfo.getItem().getCode())
			{
			case _meet:
				result = meetReq();
				break;
			case _attack:
				result = postToAttack(_move);
				break;
			case _arrive:
				result = arriveReq();
				break;
			default:
				return _server_error;
			}

			if (_eventInfo.getItem().getCode() != _attack)
			{
				_eventInfo.getItem().clear();
				resetEvent();
			}

			return result;
		}

		int path::meetReq()
		{
			pLIter iIter = _eventInfo.getIter();
			pLIter tIter = _playerMgr.getNext(iIter);
			if (iIter->getSide() == _left_side)
				_playerMgr.swap(iIter, tIter);
			else
				_playerMgr.swap(tIter, iIter);
			return _success;
		}

		int path::arriveReq()
		{
			pLIter iIter = _eventInfo.getIter();
			int other_side = iIter->getSide() == _left_side ? _right_side : _left_side;
			int to_star_id = _linked_star[other_side];
			int result = doLeave(iIter->getType(), iIter->getTriggerTime(), iIter->getPlayerInfo(), to_star_id);
			if (result == _async_throw)
				_playerMgr.pop(iIter, updateItem::_attack);
			else
				_playerMgr.pop(iIter, updateItem::_arrive);
			return result;
		}

		const int star::mainstar[3] = { 7, 18, 29 };

		bool star::alterSup(unsigned now, playerDataPtr d, int sup)
		{
			guildList::guildPtr ptr;
			if (!_guardList.find(d->playerID, d->Starwar.getPosition().getTime(), ptr))
				return false;

			d->Starwar.alterSupply(sup, now, playerStarwar::_log_supply_cost_type_item);
			int current_sup = d->Starwar.getSupply(now);
			if (d->Starwar.getSupply(now) <= 0)
			{
				_guardList.pop(ptr);
				starwar_sys.sendBack(now, d->playerID, d->Base.getName(), d->Base.getFaceID(), d->Base.getSphereID());
			}
			else
				ptr->alterSupply(current_sup, getSupply(now, _country_id));
			
			return true;
		}

		bool star::alterSup(int npc_id, int sup)
		{
			guildList::guildPtr ptr;
			if (!_guardList.find(npc_id, ptr))
				return false;
			sup = 0 - sup;
			if (ptr->getSupply() <= sup)
				_guardList.pop(ptr);
			else
				ptr->alterSupply(ptr->getSupply() - sup);

			save();
			return true;
		}

		void star::attackResp(int supply, int target_supply)
		{
			if (_cacheInfo.getType() == _transfer)
				_attacked = true;

			if (target_supply <= 0)
			{
				const guild& g = _guardList.front();
				if(g.getType() == _player)
					starwar_sys.sendBack(_cacheInfo.getTime(), g.getId(), g.getName(), g.getFaceId(), _country_id);
				_guardList.pop_front();
			}
			else
			{
				const guild& g = _guardList.front();
				g.alterSupply(target_supply);
			}

			if (supply > 0)
			{
				_cacheInfo.alterSupply(supply);
				if (doEnter(_cacheInfo.getType(), _cacheInfo.getTime(), _cacheInfo.getPlayerInfo(), _cacheInfo.getFromStarId()) == _async_throw)
					return;
			}
			else
			{
				starwar_sys.sendBack(_cacheInfo.getTime(), _cacheInfo.getPlayerInfo().getPlayerId(), _cacheInfo.getPlayerInfo().getName()
					, _cacheInfo.getPlayerInfo().getFaceId(), _cacheInfo.getPlayerInfo().getCountryId());
			}

			starwar_sys.record(this);
			starwar_sys.callBack(starwar::_report_type_1, _star_id, _cacheInfo.getTime());
		}

		void star::resetRate(int rate, unsigned now)
		{
			if (_rate == -1)
			{
				_rate = rate;
				return;
			}

			if (_rate != rate)
			{
				bool flag = false;
				for (unsigned i = 0; i < 3; ++i)
				{
					if (isSupplyEffect(i) && now > _base_time[i])
					{
						_supply[i] = getSupply(now, i);
						_base_time[i] = now;
						flag = true;
					}
				}
				if (flag)
					save();
				
				_rate = rate;
			}
		}

		bool star::isSupplyEffect(int country_id)
		{
			return _effect_type == _supply_effect && _effects[country_id]._num != 0;
		}

		int star::getSupply(unsigned now, int country_id)
		{
			if (isSupplyEffect(country_id) && now >= _base_time[country_id])
			{
				return ((now - _base_time[country_id]) * starwar_sys.getParam().getMaxSupply() * _rate / _effects[country_id]._num) + _supply[country_id];
			}
			return 0;
		}

		double star::getSupplyFloat(unsigned now, int country_id)
		{
			if (isSupplyEffect(country_id) && now >= _base_time[country_id])
			{
				double temp = (double)((now - _base_time[country_id]) * starwar_sys.getParam().getMaxSupply() * _rate);
				return temp / (double)(_effects[country_id]._num) + (double)_supply[country_id];
			}
			return 0.0;
		}

		void star::reset(unsigned now)
		{
			if (_type != 0)
			{
				_guardList.run(boost::bind(&star::sendBack, this, _1, now));
				_guardList.clear();
				loadNpc();
				_country_id = -1;
				save();
			}
		}

		void star::tick()
		{
			if (_country_id < 0 || _country_id > 2)
				return;
			_resource[_country_id] += (int)(starwar_sys.getOutputRate() * _gold);
			save();
		}

		void star::kickPlayer(int player_id, unsigned time)
		{
			_guardList.pop(player_id, time);
		}

		void star::sendBack(const guild& g, unsigned now)
		{
			if (g.getType() == _player)
				starwar_sys.sendBack(now, g.getId(), g.getName(), g.getFaceId(), _country_id, false);
		}

		int star::doLeave(int type, unsigned time, playerDataPtr d, int to_star_id)
		{
			pathPtr ptr = getPath(to_star_id);
			if (ptr == pathPtr())
				return _id_not_found;

			_guardList.pop(d->playerID, d->Starwar.getPosition().getTime());

			d->Starwar.setAction(type);
			int result = _server_error;
			if (type == _move || type == _raid)
			{ 
				d->Starwar.setPosition(_in_path, ptr->getPathId(), time);
				result = ptr->doEnter(type, time, d, to_star_id);
			}
			else if (type == _transfer)
			{
				result = ptr->doTransfer(time, d, to_star_id, false);
			}

			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(type));
			fields.push_back(boost::lexical_cast<string, int>(_star_id));
			fields.push_back(boost::lexical_cast<string, int>(to_star_id));
			StreamLog::Log(starwarLogDBStr, d, "", "", fields, playerStarwar::_log_move);

			return result;
		}

		void star::load(int player_id, const string& name, int face_id, unsigned time, int supply, int base_supply)
		{
			_guardList.push(player_id, name, face_id, time, supply, base_supply);
		}

		int star::doEnter(int type, unsigned time, const playerInfo& player_info, int from_star_id)
		{
			starwar_sys.record(this);
			if (_country_id != player_info.getCountryId() && !_guardList.empty())
			{
				_cacheInfo.setValue(type, time, player_info, from_star_id);
				return postToAttack();
			}
			else
			{
				if (_type != 0)
				{
					if (_country_id != -1)
						starwar_sys.getOccupyInfo().pop(_country_id);
					starwar_sys.getOccupyInfo().push(player_info.getCountryId());
				}
				if (_country_id != player_info.getCountryId())
				{
					postToUpdateTaskInfo(player_info.getPlayerId(), _star_id);
					vector<string> fields;
					fields.push_back(boost::lexical_cast<string, int>(_star_id));
					fields.push_back(boost::lexical_cast<string, int>(player_info.getPlayerId()));
					fields.push_back(player_info.getName());
					fields.push_back(boost::lexical_cast<string, int>(_country_id));
					fields.push_back(boost::lexical_cast<string, int>(player_info.getCountryId()));
					StreamLog::Log(starwarLogDBStr, -1, "", -1, "", "", fields, playerStarwar::_log_occupy);
				}

				int temp = _country_id;
				_country_id = player_info.getCountryId();
				starwar_sys.postToUpdatePosition(player_info.getPlayerId(), _in_star, _star_id, time, true);
				_guardList.push(player_info.getPlayerId(), player_info.getName(), player_info.getFaceId(), time, player_info.getSupply(), getSupply(time, player_info.getCountryId()));
				if (starwar_sys.isAsync())
				{
					if (temp != player_info.getCountryId())
						starwar_sys.callBack(_report_type_2, _star_id, time, true);
					else
						starwar_sys.callBack(_report_type_0, -1, time, true);
				}
				else if (temp != player_info.getCountryId())
					starwar_sys.noticeOccupy(player_info.getPlayerId(), _star_id);

				return _success;
			}
		}

		star::star(int star_id) : _star_id(star_id), _country_id(-1), _rate(-1), _init_npc(false), _current_id(0)
		{
			if (mainstar[0] == star_id)
				_country_id = 0;
			if (mainstar[1] == star_id)
				_country_id = 1;
			if (mainstar[2] == star_id)
				_country_id = 2;

			unsigned now = na::time_helper::get_current_time();
			for (unsigned i = 0; i < 3; ++i)
			{
				_supply[i] = 0;
				_base_time[i] = now;

				_resource[i] = 0;
			}
		}

		void star::init(const Json::Value& info)
		{
			_type = info["type"].asInt();
			_effect_type = info["effect_tpye"].asInt();
			_gold = info["gold"].asUInt() / 12;
			_max_gold = info["max"].asUInt();
			const Json::Value& effects = info["country_effect"];
			if (_effect_type == _atk_buff || _effect_type == _def_buff)
			{
				_effects[0]._rate = effects[0u].asDouble();
				_effects[1]._rate = effects[1u].asDouble();
				_effects[2]._rate = effects[2u].asDouble();
			}
			else
			{
				_effects[0]._num = effects[0u].asInt();
				_effects[1]._num = effects[1u].asInt();
				_effects[2]._num = effects[2u].asInt();
			}

			loadNpc();
		}

		void star::load(const mongo::BSONObj& obj)
		{
			_country_id = obj[starwar_country_id_field_str].Int();
			if (_type != 0)
				starwar_sys.getOccupyInfo().push(_country_id);

			checkNotEoo(obj[starwar_init_npc_field_str])
				_init_npc = obj[starwar_init_npc_field_str].Bool();

			checkNotEoo(obj[starwar_current_id_field_str])
				_current_id = obj[starwar_current_id_field_str].Int();

			checkNotEoo(obj[starwar_supply_field_str])
			{
				vector<mongo::BSONElement> ele = obj[starwar_supply_field_str].Array();
				for (unsigned i = 0; i < 3; ++i)
					_supply[i] = ele[i].Int();
			}
			checkNotEoo(obj[starwar_base_time_field_str])
			{
				vector<mongo::BSONElement> ele = obj[starwar_base_time_field_str].Array();
				for (unsigned i = 0; i < 3; ++i)
					_base_time[i] = ele[i].Int();
			}
			if (!obj[starwar_guilds_list_field_str].eoo())
			{
				_guardList.clear();
				vector<mongo::BSONElement> ele = obj[starwar_guilds_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
				{
					guild g(ele[i]);
					//if (starwar_sys.checkNpcConfig(_type, g.getId()))
						_guardList.push(ele[i]);
						/*else
							LogE << "can not find npc config: type(" << _type << "), id(" << g.getId() << ").." << LogEnd;*/
				}
			}
			else if(_init_npc)
			{
				_guardList.clear();
			}
			checkNotEoo(obj[starwar_sources_field_str])
			{
				vector<mongo::BSONElement> ele = obj[starwar_sources_field_str].Array();
				for (unsigned i = 0; i < 3; ++i)
					_resource[i] = ele[i].Int();
			}
		}


		int star::settlePlayerNpc(playerDataPtr d, unsigned now, int num)
		{
			int title = d->Ruler.getTitle();

			if (title < _title_king || title >= _title_max)
				return _client_error;

			if (_guardList.getPlayerNpcNum() + num > 20)
				return _num_limit;

			for (int i = 0; i < num; ++i)
			{
				if (_current_id == 100000)
					_current_id = 0;
				int npc_id = _current_id++;
				npc_id += 100000;
				_guardList.push(npc_id, d->playerID, title, now, (int)starwar_sys.getParam().getMaxSupply());
			}

			starwar_sys.record(this);
			return _success;
		}

		void star::loadNpc()
		{
			starwar_sys.getNpcGuildList(_guardList, _type);
		}

		void star::save()
		{
			mongo::BSONObj key = BSON(starwar_star_id_field_str << _star_id);
			mongo::BSONObjBuilder obj;
			obj << starwar_star_id_field_str << _star_id << starwar_country_id_field_str << _country_id
				<< starwar_init_npc_field_str << true;
			{
				obj << starwar_guilds_list_field_str << _guardList.toBSON();
			}
			{
				mongo::BSONArrayBuilder array_sw;
				for (unsigned i = 0; i < 3; ++i)
					array_sw.append(_resource[i]);
				obj << starwar_sources_field_str << array_sw.arr();
			}
			{
				mongo::BSONArrayBuilder array_sw;
				for (unsigned i = 0; i < 3; ++i)
					array_sw.append(_supply[i]);
				obj << starwar_supply_field_str << array_sw.arr();
			}
			{
				mongo::BSONArrayBuilder array_sw;
				for (unsigned i = 0; i < 3; ++i)
					array_sw.append(_base_time[i]);
				obj << starwar_base_time_field_str << array_sw.arr();
			}
			db_mgr.save_mongo(starwar_star_info_db_str, key, obj.obj());
		}

		bool star::checkGuildList(playerDataPtr d)
		{
			guildList::guildPtr ptr;
			return _guardList.find(d->playerID, d->Starwar.getPosition().getTime(), ptr);
		}

		void star::postToUpdateTaskInfo(int player_id, int star_id)
		{
			Json::Value msg;
			msg[msgStr] = Json::arrayValue;
			msg[msgStr].append(player_id);
			msg[msgStr].append(star_id);
			string str = msg.toStyledString();
			na::msg::msg_json m(gate_client::starwar_task_update_inner_req, str);
			player_mgr.postMessage(m);
		}

		Json::Value star::getInfo(int begin, int count)
		{
			Json::Value info;
			info[starwar_country_id_field_str] = _country_id;
			info["pnn"] = _guardList.getPlayerNpcNum();
			_guardList.getValue(info, begin, count, getSupply(na::time_helper::get_current_time(), _country_id));
			return info;
		}

		int star::postToAttack()
		{
			const guild& target = _guardList.front();
			starPtr ptr = starwar_sys.getStar(_cacheInfo.getFromStarId());

			Json::Value msg;
			msg[msgStr] = Json::arrayValue;
			msg[msgStr].append(_in_star);
			msg[msgStr].append(_cacheInfo.getPlayerInfo().getPlayerId());
			msg[msgStr].append(ptr == starPtr()? 0.0 : ptr->getAtkEffect(_cacheInfo.getPlayerInfo().getCountryId()));
			msg[msgStr].append(ptr == starPtr() ? 0.0 : ptr->getDefEffect(_cacheInfo.getPlayerInfo().getCountryId()));
			msg[msgStr].append(target.getType() == _player_npc? target.getId() / 100000 : target.getId());
			if (target.getType() == _player)
			{
				msg[msgStr].append(getAtkEffect(_country_id));
				msg[msgStr].append(getDefEffect(_country_id));
			}
			else
			{
				msg[msgStr].append(0.0);
				msg[msgStr].append(0.0);
			}
			msg[msgStr].append(_star_id);
			msg[msgStr].append(_cacheInfo.getTime());

			if (target.getType() == _player)
			{
				string str = msg.toStyledString();
				na::msg::msg_json m(gate_client::starwar_attack_with_player_inner_req, str);
				player_mgr.postMessage(m);
			}
			else
			{
				msg[msgStr].append(target.getType() == _npc? _type : 4);
				msg[msgStr].append(target.getSupply());
				msg[msgStr].append(target.getTitle());
				msg[msgStr].append(target.getOwnerId());
				string str = msg.toStyledString();
				na::msg::msg_json m(gate_client::starwar_attack_with_npc_inner_req, str);
				player_mgr.postMessage(m);
			}
			return _async_throw;
		}

		pathPtr star::getPath(int star_id)
		{
			for (vector<pathPtr>::iterator iter = _paths.begin(); iter != _paths.end(); ++iter)
			{
				if ((*iter)->getLinkedStarId(_star_id) == star_id)
					return *iter;
			}
			return pathPtr();
		}

		inline bool star::getAttacked()
		{
			bool flag = _attacked;
			_attacked = false;
			return flag;
		}

		void star::getBuffValue(Json::Value& info, int country_id)
		{
			switch (_effect_type)
			{
				case _not_set:
					break;
				case _atk_buff:
					if (_effects[country_id]._rate != 0.0)
					{
						info.append(_index_star_atk);
						info.append(_effects[country_id]._rate);
					}
					break;
				case _def_buff:
					if (_effects[country_id]._rate != 0.0)
					{
						info.append(_index_star_def);
						info.append(_effects[country_id]._rate);
					}
					break;
				case _supply_effect:
					if (_effects[country_id]._num != 0)
					{
						info.append(_index_star_supply);
						info.append(_effects[country_id]._num * starwar_sys.getSupplyRate());
					}
					break;
				default:
					break;
			}
		}

		starList::starList()
			: _occupyInfo(Json::arrayValue), _occupyChangeInfo(Json::arrayValue)
		{
		}

		void starList::push(const starPtr& ptr)
		{
			if (ptr->getStarId() >= (int)_stars.size())
				_stars.insert(_stars.end(), ptr->getStarId() + 1 - _stars.size(), starPtr());
			_stars[ptr->getStarId()] = ptr;
		}

		unsigned starList::getOutputBuff(int country_id)
		{
			unsigned num = 0;
			for (iterator iter = _stars.begin(); iter != _stars.end(); ++iter)
			{
				if ((*iter)->getCountryId() == country_id)
					num += ((*iter)->getOutput() * 12);
			}
			return num;
		}

		void starList::update(bool is_first)
		{
			if (is_first)
			{
				_occupyInfo = Json::arrayValue;
				for (iterator iter = _stars.begin(); iter != _stars.end(); ++iter)
				{
					int country_id = (*iter)->getCountryId();
					_occupyInfo.append(country_id);
				}
			}
			else
			{
				_occupyChangeInfo = Json::arrayValue;
				for (unsigned i = 0; i < _occupyInfo.size(); ++i)
				{
					if (_stars[i]->getCountryId() != _occupyInfo[i].asInt())
					{
						Json::Value temp = Json::arrayValue;
						temp.append(_stars[i]->getStarId());
						temp.append(_stars[i]->getCountryId());
						_occupyChangeInfo.append(temp);
						_occupyInfo[i] = _stars[i]->getCountryId();
					}
				}
				for (iterator iter = _stars.begin(); iter != _stars.end(); ++iter)
				{
					if ((*iter)->getAttacked())
					{
						Json::Value temp = Json::arrayValue;
						temp.append((*iter)->getStarId());
						temp.append(3);
						_occupyChangeInfo.append(temp);
					}
				}
			}
		}

		void starList::tickSupplyRate(int rate, unsigned now)
		{
			for (starMap::iterator iter = _stars.begin(); iter != _stars.end(); ++iter)
			{
				if ((*iter)->getEffectType() == _supply_effect)
					(*iter)->resetRate(rate, now);
			}
		}

		void starList::reset(unsigned now)
		{
			for (starMap::iterator iter = _stars.begin(); iter != _stars.end(); ++iter)
			{
				(*iter)->reset(now);
			}
		}

		pathList::pathList()
			: _pathInfo(Json::arrayValue), _pathChangeInfo(Json::arrayValue), _extra(0)
		{

		}

		void pathList::push(const pathPtr& ptr)
		{
			_paths.insert(make_pair(ptr->getPathId(), ptr));
		}

		void pathList::update(bool is_first)
		{
			if (is_first)
			{
				_pathInfo = Json::arrayValue;
				for (unsigned i = 0; i < 10; ++i)
					_extraInfo[i] = Json::arrayValue;
				_extra = 0;

				if (starwar_sys.getPlaneCount() <= _max_package_size)
				{
					for (pathMap::iterator iter = _paths.begin(); iter != _paths.end(); ++iter)
					{
						iter->second->update(true);
 						if (iter->second->getPathInfo() != Json::nullValue && iter->second->getPathInfo().size() != 0)
							_pathInfo.append(iter->second->getPathInfo());
					}
				}
				else
				{
					int count = 0;
					for (pathMap::iterator iter = _paths.begin(); iter != _paths.end(); ++iter)
					{
						iter->second->update(true);
						if (iter->second->getPathInfo() != Json::nullValue && iter->second->getPathInfo().size() != 0)
						{
							count += (int)(iter->second->size());
							int index = count / _max_package_size;
							if (index > 9)
								index = 9;
							_extraInfo[index].append(iter->second->getPathInfo());
						}
					}
					_extra = count / _max_package_size + 1;
					if (_extra > 10)
						_extra = 10;
				}
			}
			else
			{
				_pathChangeInfo = Json::arrayValue;
				for (pathMap::iterator iter = _paths.begin(); iter != _paths.end(); ++iter)
				{
					iter->second->update();
					if (iter->second->getPathChangeInfo() != Json::nullValue && iter->second->getPathChangeInfo().size() != 0)
						_pathChangeInfo.append(iter->second->getPathChangeInfo());
				}
				if (_pathChangeInfo.size() != 0)
				{
					if (starwar_sys.getPlaneCount() <= _max_package_size)
					{
						_pathInfo = Json::arrayValue;
						_extra = 0;
						for (pathMap::iterator iter = _paths.begin(); iter != _paths.end(); ++iter)
						{
							if (iter->second->getPathInfo() != Json::nullValue && iter->second->getPathInfo().size() != 0)
								_pathInfo.append(iter->second->getPathInfo());
						}
					}
					else
					{
						for (unsigned i = 0; i < 10; ++i)
							_extraInfo[i] = Json::arrayValue;
						_extra = 0;
						int count = 0;
						for (pathMap::iterator iter = _paths.begin(); iter != _paths.end(); ++iter)
						{
							if (iter->second->getPathInfo() != Json::nullValue && iter->second->getPathInfo().size() != 0)
							{
								count += (int)(iter->second->size());
								int index = count / _max_package_size;
								if (index > 9)
									index = 9;
								_extraInfo[index].append(iter->second->getPathInfo());
							}
						}
						_extra = count / _max_package_size + 1;
						if (_extra > 10)
							_extra = 10;
					}
				}
			}
		}

		void pathList::resetRate(unsigned now)
		{
			for (pathMap::iterator iter = _paths.begin(); iter != _paths.end(); ++iter)
			{
				iter->second->resetRate(starwar_sys.getMoveRate(), now);
			}
			update(true);
		}

		void pathList::reset(unsigned now)
		{
			for (pathMap::iterator iter = _paths.begin(); iter != _paths.end(); ++iter)
				iter->second->reset(now);
		}

		int rankMgr::getRank(playerDataPtr d)
		{
			if (d->Starwar.getExploit() == 0 || _rankItems.empty())
				return 0;
			if (d->Starwar.getExploit() < _rankItems.begin()->getExploit())
				return -1;
			int rank = 0;
			for (rankItemSet::reverse_iterator iter = _rankItems.rbegin(); iter != _rankItems.rend(); ++iter)
			{
				if (++rank > _max_server_rank)
					break;
				if (iter->getPlayerId() == d->playerID)
					return rank;
			}
			return -1;
		}

		rankRecord rankMgr::getOldRank(int player_id)
		{
			if (_lastTermRecords.empty())
				return rankRecord(0, 0);
			pid2Rank::iterator iter = _lastTermRecords.find(player_id);
			if (iter == _lastTermRecords.end())
				return rankRecord(-1, 0);
			return iter->second;
		}

		void rankMgr::update(playerDataPtr d, int temp)
		{
			if (d->Starwar.getExploit() <= 0)
				return;

			_rankItems.erase(rankItem::makeKey(temp, d->Starwar.getFirstTime(), d->playerID));
			_rankItems.insert(rankItem(d));
		}

		void rankMgr::load(int player_id, const string& name, int lv, int exploit, unsigned first_time)
		{
			_rankItems.insert(rankItem(player_id, name, lv, exploit, first_time));
		}

		void rankMgr::broadcast()
		{
			int i = 0;
			for (rankItemSet::reverse_iterator iter = _rankItems.rbegin(); iter != _rankItems.rend(); ++iter)
			{
				if (i++ >= 5)
					break;
				Json::Value msg;
				msg[msgStr][0u] = 0;
				msg[msgStr][1u][chatBroadcastID] = BROADCAST::starwar_exploit_summary;
				msg[msgStr][1u][senderChannelStr] = chat_kingdom;
				msg[msgStr][1u]["pl"].append(i);
				msg[msgStr][1u]["pl"].append(iter->getName());
				msg[msgStr][1u]["pl"].append(starwar_sys.getParam().getRankingReward(i));
				msg[msgStr][1u]["pl"].append(iter->getExploit());
				player_mgr.sendToSphere(gate_client::chat_broadcast_resp, msg, _country_id);
			}
		}

		void rankMgr::primeTimeSummary()
		{
			int i = 0;
			for (rankItemSet::reverse_iterator iter = _rankItems.rbegin(); iter != _rankItems.rend(); ++iter)
			{
				if (i++ >= 3)
					break;
				Json::Value msg;
				msg[msgStr][0u] = 0;
				msg[msgStr][1u][chatBroadcastID] = BROADCAST::starwar_prime_time_summary;
				msg[msgStr][1u][senderChannelStr] = chat_kingdom;
				msg[msgStr][1u]["pl"].append(i);
				msg[msgStr][1u]["pl"].append(iter->getName());
				msg[msgStr][1u]["pl"].append(iter->getExploit());
				player_mgr.sendToSphere(gate_client::chat_broadcast_resp, msg, _country_id);
			}
		}

		void rankMgr::getValue(Json::Value& info, playerDataPtr d)
		{
			info[starwar_rank_list_field_str] = Json::arrayValue;
			Json::Value& ref = info[starwar_rank_list_field_str];
			int num = 0;
			for (rankItemSet::reverse_iterator iter = _rankItems.rbegin(); iter != _rankItems.rend(); ++iter)
			{
				if (++num > _max_client_rank)
					break;
				Json::Value temp;
				iter->getValue(temp, _country_id);
				ref.append(temp);
			}
			info[starwar_rank_field_str] = getRank(d);
		}

		void rankMgr::tick()
		{
			_lastTermRecords.clear();
			int rank = 0;
			for (rankItemSet::reverse_iterator iter = _rankItems.rbegin(); iter != _rankItems.rend(); ++iter)
				_lastTermRecords.insert(make_pair(iter->getPlayerId(), rankRecord(++rank, iter->getExploit())));
			_rankItems.clear();
			save();
		}

		void rankMgr::save()
		{
			for (pid2Rank::iterator iter = _lastTermRecords.begin(); iter != _lastTermRecords.end(); ++iter)
			{
				mongo::BSONObj key = BSON(starwar_rank_field_str << iter->second.getRank() + _country_id * 1000);
				mongo::BSONObj obj = BSON(starwar_rank_field_str << iter->second.getRank() + _country_id * 1000
					<< starwar_player_id_field_str << iter->first << starwar_exploit_field_str << iter->second.getExploit());
				db_mgr.save_mongo(starwar_old_ranking_db_str, key, obj);
			}
		}

		void rankMgr::loadLastTerm(int player_id, int rank, unsigned exploit)
		{
			_lastTermRecords.insert(make_pair(player_id, rankRecord(rank, exploit)));
		}

		int occupyInfo::getUnityCountry()
		{
			for (unsigned i = 0; i < 3; ++i)
			{
				if (_star_num[i] == starwar_sys.getStarNum() - 3)
					return i;
			}
			return -1;
		}

		void gatherInfo::init(int country_id)
		{
			_country_id = country_id;
		}

		void gatherInfo::reset(unsigned now, int to_star_id)
		{
			_cd_time = now + 20 * na::time_helper::MINUTE;
			_to_star_id = to_star_id;
			_gatherList.clear();
			ruler_sys.getOfficerVec(_gatherList, _country_id);
		}

		void gatherInfo::setFromBSON(const mongo::BSONElement& obj)
		{
			_to_star_id = obj[starwar_star_id_field_str].Int();
			if (_to_star_id == -1)
				return;
			_cd_time = obj[starwar_cd_field_str].Int();
			{
				vector<mongo::BSONElement> ele = obj[starwar_player_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
					_gatherList.insert(ele[i].Int());
			}
		}
		
		void gatherInfo::clear()
		{
			_to_star_id = -1;
		}

		bool gatherInfo::isGather(int player_id)
		{
			return _gatherList.find(player_id) != _gatherList.end();
		}

		void gatherInfo::kickPlayer(int player_id)
		{
			_gatherList.erase(player_id);
		}

		bool gatherInfo::empty()
		{
			if (_to_star_id == -1)
				return true;
			unsigned now = na::time_helper::get_current_time();
			if (now >= _cd_time)
			{
				clear();
				return true;
			}
			return false;
		}

		greatEvent::greatEvent(const mongo::BSONElement& obj)
		{
			for (unsigned i = _no_title; i < _title_max; ++i)
				_titleInfoList.push_back(rulerTitleInfo(i));

			_year = obj[starwar_year_field_str].Int();
			_country_id = obj[starwar_country_id_field_str].Int();
			vector<mongo::BSONElement> ele = obj[ruler_title_info_list_field_str].Array();
			for (unsigned i = 0; i < ele.size(); ++i)
			{
				int title = ele[i][ruler_title_field_str].Int();
				_titleInfoList[title].setFromBSONElement(ele[i]);
			}
		}

		mongo::BSONObj greatEvent::toBSON() const
		{
			mongo::BSONObjBuilder obj;
			obj << starwar_year_field_str << _year << starwar_country_id_field_str << _country_id;
			mongo::BSONArrayBuilder array_sw;
			for (rulerTitleInfoList::const_iterator iter = _titleInfoList.begin(); iter != _titleInfoList.end(); ++iter)
				array_sw.append(iter->toBSON());
			obj << ruler_title_info_list_field_str << array_sw.arr();
			return obj.obj();
		}

		void greatEvent::getValue(Json::Value& info) const
		{
			info.append(_year);
			info.append(_country_id);
			Json::Value title_list;
			for (rulerTitleInfoList::const_iterator iter = _titleInfoList.begin(); iter != _titleInfoList.end(); ++iter)
			{
				Json::Value temp;
				iter->package(temp);
				title_list.append(temp);
			}
			info.append(title_list);
		}

		void greatEventList::setFromBSON(const mongo::BSONElement& obj)
		{
			vector<mongo::BSONElement> ele = obj.Array();
			for (unsigned i = 0; i < ele.size(); ++i)
				_greatEventList.push_back(greatEvent(ele[i]));

			package();
		}

		mongo::BSONArray greatEventList::toBSON() const
		{
			mongo::BSONArrayBuilder obj;
			for (greatEvents::const_iterator iter = _greatEventList.begin(); iter != _greatEventList.end(); ++iter)
				obj.append(iter->toBSON());
			return obj.arr();
		}

		void greatEventList::push(const greatEvent& e)
		{
			_greatEventList.push_back(e);
			package();
		}

		void greatEventList::package()
		{
			_cache = Json::arrayValue;
			for (greatEvents::const_iterator iter = _greatEventList.begin(); iter != _greatEventList.end(); ++iter)
			{
				Json::Value temp;
				iter->getValue(temp);
				_cache.append(temp);
			}
		}

		int signInfo::clearSignInfo(int key_id)
		{
			for (unsigned i = 0; i < 3; ++i)
			{
				for (signList::iterator iter = _signInfo[i].begin(); iter != _signInfo[i].end(); ++i)
				{
					if (iter->getKeyId() == key_id)
					{
						_signInfo[i].erase(iter);
						return i;
					}
				}
			}
			return -1;
		}

		bool signInfo::setSign(int country_id, int star_id, bool state)
		{
			if (state)
			{
				if (_signInfo[country_id].size() >= 3)
					return false;

				for (signList::iterator iter = _signInfo[country_id].begin(); iter != _signInfo[country_id].end(); ++iter)
				{
					if (iter->getStarId() == star_id)
						return false;
				}

				if (++_max_key_id >= 100000)
					_max_key_id = 1;

				unsigned cd = na::time_helper::get_current_time() + na::time_helper::HOUR;
				_signInfo[country_id].push_back(sign(star_id, cd, _max_key_id));
				starwar_sys.getTrigger().push(triggerItem(_clear_sign_info, _max_key_id, cd));
				return true;
			}
			else
			{
				for (signList::iterator iter = _signInfo[country_id].begin(); iter != _signInfo[country_id].end(); ++iter)
				{
					if (iter->getStarId() == star_id)
					{
						starwar_sys.getTrigger().pop(triggerItem(_clear_sign_info, iter->getKeyId(), iter->getCd()));
						_signInfo[country_id].erase(iter);
						return true;
					}
				}
				return false;
			}
		}

		void signInfo::setFromBSON(const mongo::BSONObj& obj)
		{
			checkNotEoo(obj[starwar_sign_info_db_list_field_str])
			{
				vector<mongo::BSONElement> ele = obj[starwar_sign_info_db_list_field_str].Array();
				for (unsigned i = 0; i < ele.size(); ++i)
				{
					vector<mongo::BSONElement> sw_array = ele[i].Array();
					for (unsigned j = 0; j < sw_array.size(); ++j)
					{
						if (sw_array[j][starwar_star_id_field_str].eoo()
							|| sw_array[j][starwar_cd_field_str].eoo()
							|| sw_array[j][starwar_type_field_str].eoo())
							continue;

						_signInfo[i].push_back(sign(sw_array[j][starwar_star_id_field_str].Int(), sw_array[j][starwar_cd_field_str].Int(), sw_array[j][starwar_type_field_str].Int()));
						starwar_sys.getTrigger().push(triggerItem(_clear_sign_info, sw_array[j][starwar_type_field_str].Int(), sw_array[j][starwar_cd_field_str].Int()));
					}
				}
			}
			checkNotEoo(obj[starwar_max_file_id_field_str])
				_max_key_id = obj[starwar_max_file_id_field_str].Int();
		}

		mongo::BSONArray signInfo::toBSON() const
		{
			mongo::BSONArrayBuilder obj;
			for (unsigned i = 0; i < 3; ++i)
			{
				mongo::BSONArrayBuilder temp;
				for (signList::const_iterator iter = _signInfo[i].begin(); iter != _signInfo[i].end(); ++iter)
				{
					mongo::BSONObj t = BSON(starwar_star_id_field_str << iter->getStarId()
						<< starwar_cd_field_str << iter->getCd() << starwar_type_field_str << iter->getKeyId());
					temp.append(t);
				}
				obj.append(temp.arr());
			}
			return obj.arr();
		}

		systemInfo::systemInfo()
		{
			for (unsigned i = 0; i < 3; ++i)
				_unity_reward[i] = 0;
			for (unsigned i = 0; i < _index_max; ++i)
				_system_time[i] = 0;
			for (unsigned i = 0; i < _index_max; ++i)
				_state[i] = -1;
			for (unsigned i = 0; i < 3; ++i)
				_gatherInfo[i].init(i);
		}

		void systemInfo::load()
		{
			objCollection objs = db_mgr.Query(starwar_system_info_db_str);

			for (unsigned i = 0; i < objs.size(); ++i)
			{
				int type = objs[i][starwar_type_field_str].Int();
				if (type >= _index_max)
					continue;
				if (type == _unity_reward_index)
				{
					vector<mongo::BSONElement> ele = objs[i][starwar_reward_field_str].Array();
					for (unsigned i = 0; i < 3; ++i)
						_unity_reward[i] = ele[i].Int();
				}
				else if (type == _gather_index)
				{
					vector<mongo::BSONElement> ele = objs[i][starwar_gather_info_field_str].Array();
					for (unsigned i = 0; i < ele.size(); ++i)
					{
						int country_id = ele[i][starwar_country_id_field_str].Int();
						_gatherInfo[country_id].setFromBSON(ele[i]);
					}
				}
				else if (type == _great_event_index)
				{
					_greatEventList.setFromBSON(objs[i][starwar_great_event_list_field_str]);
				}
				else if (type == _sign_info_index)
				{
					_signInfo.setFromBSON(objs[i]);
				}
				else
				{
					_system_time[type] = objs[i][starwar_time_field_str].Int();
					_state[type] = objs[i][starwar_state_field_str].Int();
				}
			}
		}

		void systemInfo::save(int index)
		{
			if (index < 0 || index >= _index_max)
				return;

			mongo::BSONObj key = BSON(starwar_type_field_str << index);
			mongo::BSONObjBuilder obj;
			obj << starwar_type_field_str << index;
			if (index == _unity_reward_index)
			{
				mongo::BSONArrayBuilder array_sw;
				for (unsigned i = 0; i < 3; ++i)
					array_sw.append(_unity_reward[i]);
				obj << starwar_reward_field_str << array_sw.arr();
			}
			else if (index == _gather_index)
			{
				mongo::BSONArrayBuilder array_sw;
				for (unsigned i = 0; i < 3; ++i)
				{
					mongo::BSONObjBuilder temp;
					temp << starwar_country_id_field_str << i
						<< starwar_star_id_field_str << _gatherInfo[i].getToStarId();
					if (_gatherInfo[i].getToStarId() != -1)
					{
						temp << starwar_cd_field_str << _gatherInfo[i].getCd();
						{
							mongo::BSONArrayBuilder array_sw;
							for (gatherInfo::gatherList::iterator iter = _gatherInfo[i].getGatherList().begin(); 
								iter != _gatherInfo[i].getGatherList().end(); ++iter)
								array_sw.append(*iter);
							temp << starwar_player_list_field_str << array_sw.arr();
						}
					}
					array_sw.append(temp.obj());
				}
				obj << starwar_gather_info_field_str << array_sw.arr();
			}
			else if (index == _great_event_index)
			{
				obj << starwar_great_event_list_field_str << _greatEventList.toBSON();
			}
			else if (index == _sign_info_index)
			{
				obj << starwar_sign_info_db_list_field_str << _signInfo.toBSON()
					<< starwar_max_file_id_field_str << _signInfo.getMaxKeyId();
			}
			else
			{
				obj << starwar_time_field_str << _system_time[index]
					<< starwar_state_field_str << _state[index];
			}
			db_mgr.save_mongo(starwar_system_info_db_str, key, obj.obj());
		}

		void systemInfo::alterSystemTime(int index, unsigned time, int state)
		{
			_system_time[index] = time;
			if (state != -1)
				_state[index] = state;
			save(index);
		}

		void systemInfo::alterUnityReward(int country_id, int num)
		{
			_unity_reward[country_id] = num;
			save(_unity_reward_index);
		}

		void systemInfo::resetGatherInfo(int country_id, unsigned now, int to_star_id)
		{
			_gatherInfo[country_id].reset(now, to_star_id);
			save(_gather_index);
		}

		bool systemInfo::isGather(playerDataPtr d)
		{
			if (_gatherInfo[d->Base.getSphereID()].empty())
				return false;
			return _gatherInfo[d->Base.getSphereID()].isGather(d->playerID);
		}

		void systemInfo::kickPlayer(playerDataPtr d)
		{
			_gatherInfo[d->Base.getSphereID()].kickPlayer(d->playerID);
			save(_gather_index);
		}

		void systemInfo::pushGreatEvent(const greatEvent& e)
		{
			_greatEventList.push(e);
			save(_great_event_index);
		}

		pathInfo shortestPath::_null;

		void pathInfo::init(const mPath& pass_star_list)
		{
			for (mPath::const_reverse_iterator iter = pass_star_list.rbegin(); iter != pass_star_list.rend(); ++iter)
			{
				_pass_star_list.push_back(*iter);
			}

			if (!pass_star_list.empty())
			{
				int from_id = pass_star_list.front();
				for (unsigned i = 1; i < pass_star_list.size(); ++i)
				{
					if (_cost_time == -1)
						_cost_time = starwar_sys.getCostTime(from_id, pass_star_list[i]);
					else
						_cost_time += starwar_sys.getCostTime(from_id, pass_star_list[i]);
					from_id = pass_star_list[i];
				}
			}
		}
	}
}