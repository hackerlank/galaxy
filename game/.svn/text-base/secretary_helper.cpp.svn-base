#include "secretary_helper.h"
#include "secretary_system.h"

namespace gg
{
	void secRandom::load(int id, unsigned weight)
	{
		_max_weight += weight;
		_randomList.push_back(IdWeight(id, _max_weight));
		if (_max_weight > 30000)
			LogE << "secRandom: out of weight" << LogEnd;
	}

	int secRandom::run()
	{
		int ram = commom_sys.random() % _max_weight;
		for (unsigned i = 0; i < _randomList.size(); ++i)
		{
			if (ram < (int)_randomList[i].weight)
				return _randomList[i].id;
		}
		return -1;
	}

	void qualityRandom::load(const Json::Value& info)
	{
		_randQuality = Json::arrayValue;
		_randQuality.append(info["D"].asDouble());
		_randQuality.append(info["C"].asDouble());
		_randQuality.append(info["B"].asDouble());
		_randQuality.append(info["A"].asDouble());
		_randQuality.append(info["S"].asDouble());
		const Json::Value& chip = info["chip"];
		for (unsigned i = 0; i < chip.size(); ++i)
		{
			_chipNum.push_back(chip[i][0u].asInt());
			_randQuality.append(chip[i][1u].asDouble());
		}
	}

	void qualityRandom::randQuality(int& quality, int& chip_num)
	{
		quality = commom_sys.randomList(_randQuality) + 1;
		if (quality > secretary::_Quality_Max)
			chip_num = _chipNum[quality - secretary::_Quality_Max - 1];
	}

	void critRandom::load(const Json::Value& info)
	{
		for (unsigned i = 0; i < info.size(); ++i)
		{
			_rates.push_back(info[i][0u].asInt());
			_propList.append(info[i][1u].asDouble());
		}
	}

	int critRandom::run()
	{
		return _rates[commom_sys.randomList(_propList)];
	}

	void positionLimit::load(const Json::Value& info)
	{
		for (unsigned i = 0; i < info.size(); ++i)
		{
			int pos = info[i]["position"].asInt();
			_ql[pos - 1]._quality = info[i]["star"].asInt();
			_ql[pos - 1]._lv = info[i]["lv"].asInt();
		}
	}

	int positionLimit::getQualityLimit(int pos)
	{
		if (pos < 1 || pos > secretary::_max_upload_num)
			return secretary::_Quality_Max;
		return _ql[pos - 1]._quality;
	}

	int positionLimit::getOpenLv(int pos)
	{
		if (pos < 1 || pos > secretary::_max_upload_num)
			return 999;
		return _ql[pos - 1]._lv;
	}

	void combineConfig::load(const Json::Value& sid_array, const Json::Value& attr_types, const Json::Value& attr_values)
	{
		for (unsigned i = 0; i < sid_array.size(); ++i)
			_sids.push_back(sid_array[i].asInt());
		for (unsigned i = 0; i < attr_types.size(); ++i)
			_attrs[attr_types[i].asInt()] = attr_values[i].asDouble();
	}

	bool combineConfig::isTrue(int* sids, int size)
	{
		for (unsigned i = 0; i < _sids.size(); ++i)
		{
			bool find = false;
			for (int j = 0; j < size; ++j)
			{
				if (_sids[i] == sids[j])
				{
					find = true;
					break;
				}
			}
			if (!find)
				return false;
		}
		return true;
	}

	void combineConfig::getAttrs(void* point)
	{
		memcpy(point, _attrs, sizeof(_attrs));
	}

	secretaryConfig::secretaryConfig(const Json::Value& info)
	{
		_sid = info["id"].asInt();
		_quality = info["quality"].asInt();
		_max_level = info["maxLevel"].asInt();
		_init_level = info["startLevel"].asInt();
		secretary_sys.getGainRand()[_quality - 1].load(_sid, info["gainProbability"].asUInt());
		secretary_sys.getCombineRand()[_quality - 1].load(_sid, info["combineProbability"].asUInt());
		const Json::Value& attr_array = info["attrArray"];
		for (unsigned i = 0; i < attr_array.size(); ++i)
			_attrs.push_back(attr_array[i].asInt());
		const Json::Value& cid_array = info["combineGirlId"];
		const Json::Value& ctype_array = info["comineValueTypeArray"];
		const Json::Value& cvalue_array = info["CombineArray"];
		for (unsigned i = 0; i < cid_array.size(); ++i)
		{
			combinePtr ptr = creator<combineConfig>::run(_sid);
			ptr->load(cid_array[i], ctype_array[i], cvalue_array[i]);
			_combines.push_back(ptr);
		}
	}

	void secretaryConfig::getAttrs(void* point, int lv)
	{
		memset(point, 0x0, sizeof(double) * characterNum);
		double total_attrs[characterNum];
		memset(total_attrs, 0x0, sizeof(double) * characterNum);
		secretary_sys.getAttrByLv(total_attrs, lv, _quality);
		for (attrTypes::iterator iter = _attrs.begin(); iter != _attrs.end(); ++iter)
			((double*)point)[*iter] = total_attrs[*iter];
	}

	void secretaryConfig::getCombineAttrs(void* point, int* sids, int size)
	{
		memset(point, 0x0, sizeof(double) * characterNum);
		for (unsigned i = 0; i < _combines.size(); ++i)
		{
			if (_combines[i]->isTrue(sids, size))
			{
				double added[characterNum];
				_combines[i]->getAttrs(added);
				for (unsigned j = 0; j < characterNum; ++j)
					((double*)point)[j] += added[j];
			}
		}
	}

	void attrConfig::load(const Json::Value& info)
	{
		for (unsigned i = 0; i < info.size(); ++i)
		{
			const Json::Value& temp = info[i];
			double added[characterNum];
			memset(added, 0x0, sizeof(added));
			for (unsigned i = 0; i < temp.size(); ++i)
				added[i] = temp[i].asDouble();
			_attrs.push_back(attrs(added));
		}
	}

	void attrConfig::getAttr(void* point, int lv)
	{
		memset(point, 0x0, sizeof(double) * characterNum);
		if (lv > (int)_attrs.size() || lv < 1)
			return;
		memcpy(point, _attrs[lv - 1]._value, sizeof(double) * characterNum);
	}
}