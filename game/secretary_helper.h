#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>
#include "pilot_def.h"
#include "game_helper.hpp"
using namespace std;

namespace gg
{
	namespace secretary
	{
		enum 
		{
			// 错误代码
			_success = 0,
			_client_error,
			_server_error,
			_id_error,
			_resource_limit,
			_quality_limit,
			_locked,
			_not_opened,
			_in_cd,
			_times_limit,
			_exp_limit,
			_num_limit,
			_lv_limit,
			_same_sid,
			_in_format,
			_total_limit,
			_gold_limit,
			_silver_limit,
			_invalid_name,

			// 调教
			_use_silver = 0,
			_use_gold,

			// 阵型
			_closed = -2,
			_free = -1,
			_init_free_num = 3,
			_max_upload_num = 5,

			// 品质
			_Chip = 0,
			_Quality_D = 1,
			_Quality_C,
			_Quality_B,
			_Quality_A,
			_Quality_S,
			_Quality_Max = _Quality_S,

			// log
			_log_lottery = 1,
			_log_upgrade,
			_log_swallow,
			_log_swap,
			_log_combine,

			// 参数
			_rename_cost = 10,
			_swap_exp_cost = 1,
			_upgrade_silver_times = 5,
			_upgrade_gold_times = 30,
			_lottery_low_free_times = 5,
			_lottery_min_cost = 100,
			_lottery_high_cost = 300,
			_ten_lottery_min_cost = 988,
			_ten_lottery_high_cost = 2888,
			_lottery_getS_times_limit = 20,
			_resolveS_chip_num = 500,
			_resolveA_chip_num = 20,
			_combineS_chip_num = 1200,
			_combineA_chip_num = 120,
			_max_secretary_num = 150,
			_lottery_max_low_times = 10,
			_open_level = 28,
			_role_lv_limit = 85,
		};
	}

	class secRandom
	{
		public:
			secRandom() : _max_weight(0){}

			void load(int id, unsigned weight);
			int run();

		private:
			struct IdWeight
			{
				public:
					IdWeight(int i, unsigned w)
						: id(i), weight(w){}

					int id;
					unsigned weight;
			};

			vector<IdWeight> _randomList;
			unsigned _max_weight;
	};

	class qualityRandom
	{
		public:
			void load(const Json::Value& info);
			void randQuality(int& quality, int& chip_num);

		private:
			Json::Value _randQuality;
			vector<int> _chipNum;
	};

	class positionLimit
	{
		public:
			void load(const Json::Value& info);
			int getQualityLimit(int pos);
			int getOpenLv(int pos);

		private:
			struct QL
			{
				public:
					int _quality;
					int _lv;
			};
			QL _ql[secretary::_max_upload_num];
	};

	class critRandom
	{
		public:
			void load(const Json::Value& info);
			int run();
		private:
			Json::Value _propList;
			vector<int> _rates;
	};

	class combineConfig
	{
		public:

			void load(const Json::Value& sid_array, const Json::Value& attr_types, const Json::Value& attr_values);
			bool isTrue(int* sids, int size);
			void getAttrs(void* point);
		
			combineConfig(int source_id)
			{
				memset(_attrs, 0x0, sizeof(_attrs));
				_sids.push_back(source_id);
			}

		private:
			vector<int> _sids;
			double _attrs[characterNum];
	};

	typedef boost::shared_ptr<combineConfig> combinePtr;

	class secretaryConfig
	{
		public:

			int getSid() const { return _sid; }
			int getQuality() const { return _quality; }
			int getMaxLevel() const { return _max_level; }
			int getInitLevel() const { return _init_level; }

			void getAttrs(void* point, int lv);
			void getCombineAttrs(void* point, int* sids, int size);

			secretaryConfig(const Json::Value& info);

		private:
			int _sid;
			int _quality;
			int _max_level;
			int _init_level;
			typedef vector<int> attrTypes;
			attrTypes _attrs;
			typedef vector<combinePtr> combinePtrs;
			combinePtrs _combines;
	};

	typedef boost::shared_ptr<secretaryConfig> secConfigPtr;

	class attrConfig
	{
		public:
			void getAttr(void* point, int lv);
			void load(const Json::Value& info);

		private:
			class attrs
			{
				public:
					attrs(double* ptr)
					{
						memcpy(_value, ptr, sizeof(_value));
					}

					double _value[characterNum];
			};
			vector<attrs> _attrs;
	};
}