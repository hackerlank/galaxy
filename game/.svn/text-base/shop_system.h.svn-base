#pragma once
#include "json/json.h"
#include <boost/unordered_map.hpp>
#include "msg_base.h"
#include "playerManager.h"
#include "mongoDB.h"
#include "player_shop.h"

using namespace na::msg;

#define shop_sys (*gg::shop_system::shopSys)

namespace gg
{
	const static string shopGoodsDBStr = "gl.shop_goods_info";
	const static string shopLogDBStr = "log_shop";

	const static string shop_num_field_str = "num";
	const static string shop_limit_num_field_str = "ln";

	namespace shop
	{
		enum{
			_cnn_not_set = 0,
			_cnn_role_lv,
			_cnn_vip_lv,
			_cnn_ruler_title,
			_cnn_king,
			_cnn_office_level,

			_success = 0,
			_error_1,
			_error_cnn,
			_error_lmt_num,
			_error_lmt_day,
			_error_lmt_total,
			_error_price,
			_error_7,
			_error_package_full,

			_hot_products = 0,
			_crystal_products,
			_purchase_products,
			_honor_products,
			_paper_products,
		};

		class condition
		{
			public:
				void setFromValue(const Json::Value& var);
				bool check(playerDataPtr d);

			private:
				int _type;
				int _arg1;
				int _arg2;
		};

		class limits
		{
			public:
				void setFromValue(const Json::Value& var);
				int check(playerDataPtr d, int gid);
				void alterNum(int num) const;
				int num() const{ return _num; }
				int total() const{ return _total; }
				int all() const { return _all_num; }

			private:
				int _day;
				int _total;
				mutable int _num;
				int _all_num;
		};

		class item
		{
			public:
				void setFromValue(const Json::Value& var, int tabbar);
				int check(playerDataPtr d);
				int getItem(playerDataPtr d);

				int tabbar() const { return _tabbar; }
				const rewardItem& goods(){ return _goods; }
				int gid() const { return _gid; }
				int price() const { return _price; }
				const condition& cnn() const { return _cnn; }
				const limits& lmt() const { return _lmt; }

			private:
				Json::Value transfer(const Json::Value& var);
				bool checkPrice(playerDataPtr d);
				int checkOther(playerDataPtr d);

			private:
				int _tabbar;
				int _gid;
				rewardItem _goods;
				int _price;
				condition _cnn;
				limits _lmt;
		};
	}


	class shop_system
	{
	public:
		enum{
			_log_shop_deal = 0
		};

		static shop_system* const shopSys;

		static const string shopHotFileStr;
		static const string shopCrystalFileStr;
		static const string shopCrystalFileStr2;
		static const string shopPurchaseFileStr;
		static const string shopHonorFileStr;
		static const string shopPaperFileStr;

		shop_system() : _next_update_time(0), _param(Json::nullValue){}
		void initData();
		void shopUpdateReq(msg_json& m, Json::Value& r);
		void shopDealReq(msg_json& m, Json::Value& r);
		void shopInfoReq(msg_json& m, Json::Value& r);
		void buyMonthCardReq(msg_json& m, Json::Value& r);

		void update(playerDataPtr d, int gid);
		void info(playerDataPtr d);

		void gmShopInfoReq(msg_json& m, Json::Value& r);
		void gmShopModifyReq(msg_json& m, Json::Value& r);

		Json::Value& param(){ return _param; }

	private:
		void checkAndUpdate();

		void loadFile();
		void loadFile(const string& filename, int tabbar, unsigned now = 0);
		void loadDB();

		void alterNextUpdateTime(unsigned nt);
		void alterItemNum(const shop::item& it);
		void resetItemNum();
		void save(const shop::item& it);

	private:
		typedef boost::unordered_map<int, shop::item> ItemMap;
		ItemMap _items;
		ItemMap _backup;
		ItemMap _crystal;
		ItemMap _crystal2;
		unsigned _next_update_time;

		Json::Value _param;
	};
}