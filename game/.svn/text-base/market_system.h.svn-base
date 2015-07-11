#pragma once

#include "json/json.h"
#include "msg_base.h"
#include "mongoDB.h"
#include <boost/unordered_map.hpp>
#include "playerManager.h"
#include "action_format.hpp"
#include "recorder.hpp"

#define market_sys (*gg::market_system::marketSys)

namespace gg
{
	class commodity
	{
		public:
			struct price
			{
				public:
					Json::Value getValue() const
					{
						Json::Value temp;
						temp.append(_type);
						temp.append(_cost);
						return temp;
					}
					void load(const Json::Value& info)
					{
						_type = info[0u].asInt();
						_cost = info[1u].asInt();
					}
					void load(const mongo::BSONElement& info)
					{
						_type = info["type"].Int();
						_cost = info["cost"].Int();
					}
					mongo::BSONObj toBSON() const
					{
						return BSON("type" << _type << "cost" << _cost);
					}
					int _type;
					int _cost;
			};

			int getItems(playerDataPtr d);

			void load(const mongo::BSONElement& obj);
			void load(const Json::Value& info);
			mongo::BSONObj toBSON() const;

			void infoValue(Json::Value& info);
			void gmInfoValue(Json::Value& info);

			int getId() const { return _id; }
			void resetNum() { _remain_num = _total_num; }
			int getRemainNum() const { return _remain_num; }

		private:
			bool isPriceLimit(playerDataPtr d);
			bool isBuyLimit(playerDataPtr d);
			void doPrice(playerDataPtr d);
			bool checkPilots(playerDataPtr d);

		private:
			int _id;
			string _name;
			price _price;
			Json::Value _complexValue;
			Json::Value _simpleValue;
			int _total_num;
			int _remain_num;
			int _limit;
			bool _crystal_limit;

			vector<int> _pilots;
	};

	const static string market_info_db_str = "gl.market_info";
	const static string marketLogDBStr = "log_market";

#define marketLog(player, tag, ...)\
	LogTemplate(marketLogDBStr, player, tag, __VA_ARGS__)       // __VA_ARGS__ = preV, nowV, f1, f2, ... 

	class market_system
	{
		public:
			enum{
				_log_deal = 0,
			};

			static market_system* const marketSys;

			market_system() : _key_id(-1), _clientCache(Json::nullValue), _gmCache(Json::nullValue), _next_update_time(0), _timer_id(-1), _updateFlag(false){}
			void initData();

			void gmMarketUpdateReq(msg_json& m, Json::Value& r);
			void gmMarketModifyReq(msg_json& m, Json::Value& r);

			void infoReq(msg_json& m, Json::Value& r);
			void updateReq(msg_json& m, Json::Value& r);
			void dealReq(msg_json& m, Json::Value& r);
			void tickUpdate();
			void tickClose();

			int getKeyId() const { return _key_id; }

		private:
			void update(playerDataPtr d);
			void info(playerDataPtr d);
			void package();
			void save();

		private:
			int _key_id;
			unsigned _activity_time[2];
			int _business_time[2];
			unsigned _business_time_work[2];
			typedef std::map<int, commodity> commodityMap;
			commodityMap _cMap;

			unsigned _next_update_time;
			Json::Value _gmCache;
			Json::Value _clientCache;

			int _timer_id;
			bool _updateFlag;

		public:
			static bool _packageFull;
	};
}