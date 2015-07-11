#pragma once

#include <boost/unordered_map.hpp>
#include "msg_base.h"
#include "json/json.h"
#include "commom.h"
#include "mongoDB.h"
#include "playerManager.h"
#include "params.hpp"
#include <set>
#include <string>
using namespace std;
using namespace na::msg;
#define trade_sys (*gg::trade_system::trade_pointer)

namespace gg
{
	enum TRADEBUFF
	{
		trade_world_buff_start = 0,
		//世界BUFF
		tonghuopengzhang = 1,
		xinpinrexiao = 2,
		zhengfufuchi = 3,
		haidaotuxi = 4,
		jingweixuncha = 5,
		tonghuojinsuo = 6,
		fengmishangpin = 7,

		trade_world_buff_end,

		trade_item_buff_start = 1000,
		//道具BUFF
		jiasu = 1001,
		zhencha = 1002,
		fangshui = 1003,
		weishe = 1004,
		yijia = 1005,

		trade_item_buff_end,
	};
	const static int TradeEventCount = 7;
	const static int tradeItemNum = 30;
	const static unsigned TradeFlyStep = 180;

	class planetTrade
	{
	public:
		planetTrade() : planetID(-1), allowLevel(999)
		{

		}
		planetTrade(Json::Value& config, mongo::BSONObj& obj);
		const int planetID;
		const int allowLevel;
		inline bool nullPlanet()
		{
			return planetID < 0;
		}
		inline static int calKingdom(const int pID)
		{
			int kid = pID / 1000;
			kid = kid < 0 ? 0 : kid;
			kid = kid > 2 ? 2 : kid;
			return kid;
		}
		inline static int calRealDistance(const int pID)
		{
			return pID % 1000;
		}
		int buyItem(const int itemID)
		{
			//返回所需价格
			//0是无法购买
			itemMap::iterator it = buy.find(itemID);
			if (it == buy.end())return 0;
			item& cItem = it->second;
			return cItem._Price();
		}
		int saleItem(const int itemID)
		{
			//返回所需价格
			//0是无法贩卖
			itemMap::iterator it = sale.find(itemID);
			if (it == sale.end())return 0;
			item& cItem = it->second;
			return cItem._modulePrice();
		}
		int randomSaleItemID()
		{
			int rk = commom_sys.randomBetween(0, sale.size() - 1);
			itemMap::iterator it = sale.begin();
			for (int i = 0; i < rk; ++i)
			{
				++it;
			}
			return it->first;
		}
		inline int getKingdom()
		{
			return calKingdom(planetID);
		}
		static int distance(const int ownPlanet, const int otherPlanet)
		{
			bool same = calKingdom(otherPlanet) == calKingdom(ownPlanet);
			int offset = same ? 0 : 1;//同国路径+0, 异国路径+1
			int m_distance = calRealDistance(ownPlanet);
			int o_distance = calRealDistance(otherPlanet);
			if (same)o_distance = -o_distance;//同国相减,异国相加,取绝对值
			return std::abs(m_distance + o_distance + offset);
		}
		int distance(const int otherPlanet)
		{
			return distance(planetID, otherPlanet);
		}
		Json::Value toJson();
		mongo::BSONObj toBson();
		void TickPrice();
	private:
		class item
		{
			friend class planetTrade;
		public:
			item() :
				minPrice(-1), maxPrice(-1),
				minAlter(-1), maxAlter(-1),
				itemID(-1), module(0.0)
			{

			}
			//可卖的
			item(mongo::BSONObj& obj, Json::Value& config) :
				minPrice(config["minsalePrice"].asInt()), maxPrice(config["maxsalePrice"].asInt()),
				minAlter(config["minsaleAlter"].asInt()), maxAlter(config["maxsaleAlter"].asInt()),
				itemID(config["itemID"].asInt()), module(config["module"].asDouble())
			{
				Price = -1;
				if (obj.isEmpty() || obj["id"].Int() != itemID)
				{
					Price = commom_sys.randomBetween(minPrice, maxAlter);
				}
				else
				{
					Price = obj["p"].Int();
				}
			}
			//可买的
			item(Json::Value& config, mongo::BSONObj& obj) :
				minPrice(config["minbuyPrice"].asInt()), maxPrice(config["maxbuyPrice"].asInt()),
				minAlter(config["minbuyAlter"].asInt()), maxAlter(config["maxbuyAlter"].asInt()),
				itemID(config["itemID"].asInt()), module(1.0)
			{
				Price = -1;
				if (obj.isEmpty() || obj["id"].Int() != itemID)
				{
					Price = commom_sys.randomBetween(minPrice, maxAlter);
				}
				else
				{
					Price = obj["p"].Int();
				}
			}
			bool Vaild();
			inline int _Price()const
			{
				return Price;
			}
			inline int _modulePrice()const
			{
				return int(Price * module);
			}
			const int itemID;
			const int minPrice;
			const int maxPrice;
			const int minAlter;
			const int maxAlter;
			inline bool canBuy()const
			{
				return Price > 0;
			}
		private:
			void randomPrice()
			{
				if (Price < 0)return;
				int alter = commom_sys.randomBetween(minAlter, maxAlter);
				if (Price + alter < minPrice || Price + alter > maxPrice)
				{
					Price -= alter;
				}
				else
				{
					Price += alter;
				}
			}
			int Price;
			double module;
		};
		typedef boost::unordered_map<int, item> itemMap;
		itemMap buy, sale;
	};
	class trade_system
	{
		friend void planetTrade::TickPrice();
	public:
		struct Rank
		{
			Rank()
			{
				playerID = -1;
				playerName = "";
				voucher = 0;
				createTime = 0;
			}
			Rank(const int vou, const unsigned cT, const int pID)
			{
				new(this) Rank();
				playerID = pID;
				createTime = cT;
				voucher = vou;
			}
			Rank(const int vou, const unsigned cT, const int pID, const string pN)
			{
				new(this) Rank(vou, cT, pID);
				playerName = pN;
			}
			bool operator<(const Rank& other)const
			{
				if (voucher != other.voucher)return voucher > other.voucher;
				if (createTime != other.createTime)return createTime < other.createTime;
				return playerID < other.playerID;
			}
			bool operator==(const Rank& other)const
			{
				return (playerID == other.playerID && createTime == other.createTime &&
					playerName == other.playerName && voucher == other.voucher);
			}
			int playerID;
			string playerName;
			int voucher;
			unsigned createTime;
		};

		struct Rich
		{
			Rich()
			{
				playerID = -1;
				createTime = 0;
				pName = "";
				gName = "";
				force = -1;
				passTime = 0;
				voucher = 0;
			}
			Rich(const int vou, const int pass, const unsigned cT, const int pID)
			{
				new(this) Rich();
				playerID = pID;
				createTime = cT;
				passTime = pass;
				voucher = vou;
			}
			Rich(const int vou, const int pass, const unsigned cT, const int pID, const string pN , const string gN, const int kingdom)
			{
				playerID = pID;
				createTime = cT;
				pName = pN;
				passTime = pass;
				voucher = vou;
				gName = gN;
				force = kingdom;
			}
			bool operator<(const Rich& other)const
			{
				if (voucher != other.voucher)return voucher > other.voucher;
				if (passTime != other.passTime)return passTime < other.passTime;
				if (createTime != other.createTime)return createTime < other.createTime;
				return playerID < other.playerID;
			}
			bool operator==(const Rich& other)const
			{
				return (playerID == other.playerID && createTime == other.createTime &&
					pName == other.pName && passTime == other.passTime &&
					voucher == other.voucher && gName == other.gName &&
					force == other.force);
			}
			int playerID;
			unsigned createTime;
			string pName;
			int passTime;
			int voucher;
			string gName;
			int force;
		};

		static trade_system* const trade_pointer;
		void initData();
		Params::ArrayValue PlaneConfig;

		void tradeFrist(msg_json& m, Json::Value& r);
		void tradeUpdate(msg_json& m, Json::Value& r);
		void tradeItemUpdate(msg_json& m, Json::Value& r);
		void tradeCenterUpdate(msg_json& m, Json::Value& r);
		void acceptTask(msg_json& m, Json::Value& r);
		void cancelTask(msg_json& m, Json::Value& r);
		void completeTask(msg_json& m, Json::Value& r);
		void move(msg_json& m, Json::Value& r);
		void buy(msg_json& m, Json::Value& r);
		void sale(msg_json& m, Json::Value& r);
		void updateTable(msg_json& m, Json::Value& r);
		void park(msg_json& m, Json::Value& r);
		void useItem(msg_json& m, Json::Value& r);
		void Teleport(msg_json& m, Json::Value& r);
		void tradeUpdateSkill(msg_json& m, Json::Value& r);
		void updateSimData(msg_json& m, Json::Value& r);
		void FastTask(msg_json& m, Json::Value& r);
		void showVoucher(msg_json& m, Json::Value& r);
		void upgradePlane(msg_json& m, Json::Value& r);
		
		void updategName(Rich key, const string name);
		void updatepName(Rich key, const string name);
		void updatepName(Rank key, const string name);
		void insertRich(const int playerID, Rich value);
		void insertRankToday(const int playerID, Rank value);
		void insertRankHistory(const int playerID, Rank value);
	private:
		void eventCreate(bool force);
		void priceCreate(bool force);
		
		void runItemEvent(playerDataPtr player, const int itemID);
		void runPlanetEvent(playerDataPtr player);
		typedef boost::function< void(playerDataPtr) > TradeHandler;
		typedef boost::unordered_map<int, TradeHandler> boostHandlerMap;
		typedef boostHandlerMap::iterator HandlerIt;
		boostHandlerMap HandlerMap;
		//world
		void world_tonghuopengzhang(playerDataPtr player);
		void world_xinpinrexiao(playerDataPtr player);
		void world_zhengfufuchi(playerDataPtr player);
		void world_haidaotuxi(playerDataPtr player);
		void world_jingweixuncha(playerDataPtr player);
		void world_tonghuojinsuo(playerDataPtr player);
		void world_fengmishangpin(playerDataPtr player);
		//item
		void item_jiasu(playerDataPtr player);
		void item_zhencha(playerDataPtr player);
		void item_fangshui(playerDataPtr player);
		void item_weishe(playerDataPtr player);
		void item_yijia(playerDataPtr player);
	private:
		bool hasEvent(const int level);
		void AddBuyPrice(const int itemID, const int price);
		void AddSalePrice(const int planetID, const int itemID, const int price);
		void writePrice();
		void savePrice();
		void saveEvent();
		void saveTick();
		void saveRich();
		void saveRankToday();
		void saveRankHistory();
		void writeRank(bool force);
		Json::Value PackagePlanetEvent(const int level, bool seeall, const int Fromitem = -1);
		struct Event
		{
			Event(const int id = -1, const unsigned eT = 0) :
				ID(id), endTime(eT), eventParams(Json::objectValue)
			{

			}
			Event& operator=(const Event& other)
			{
				memmove(this, &other, sizeof(unsigned) + sizeof(int));
				eventParams = other.eventParams;
				return *this;
			}
			const unsigned endTime;
			const int ID;
			Json::Value eventParams;
		};
		Event getEvent(const int planetID);
		typedef boost::unordered_map<int, Event> iuiMap;
		iuiMap eventMap;
		planetTrade& getPlanet(const int planetID);
		typedef boost::unordered_map<int, planetTrade> planetMap;
		planetMap ownPlanet;
		unsigned PriceTick;
		unsigned EventTick;
		unsigned rankTick;
		typedef vector<int> priceVec;
		typedef boost::unordered_map<int, priceVec> jsonMap;
		jsonMap buyHistory;
		typedef boost::unordered_map<int, jsonMap> i_jsonMap;
		i_jsonMap saleHistory;
		typedef std::set<Rich> RichList;
		typedef RichList::iterator RichIter;
		RichList _RichList;
		typedef std::set<Rank> RankList;
		typedef RankList::iterator RankIter;
		RankList _TodayList, _HistoryList;
		struct simPlane
		{
			simPlane(int id, string name, int lv, int sphere, int pos, int des, unsigned fly)
			{
				ID = id;
				NAME = name;
				LV = lv;
				SPHERE = sphere;
				POS = pos;
				DES = des;
				FLY = fly;
			}
			bool operator==(const simPlane& sP)const
			{
				return ID == sP.ID;
			}
			Json::Value toJson()
			{
				if (POS != DES)
				{
					unsigned now = na::time_helper::get_current_time();
					int count = 0;
					do 
					{
						unsigned nFly = FLY + TradeFlyStep;
						if (nFly > now)break;
						FLY = nFly;
						POS = playerTrade::moveToNext(POS, DES);
						if (POS == DES)break;
						++count;
						if (count > 100)break;//防止死循环
					} while (true);
				}
				Json::Value updateJson;
				updateJson["spi"] = SPHERE;
				updateJson["na"] = NAME;
				updateJson["pos"] = POS;
				updateJson["dt"] = DES;
				updateJson["ft"] = FLY;
				updateJson["plv"] = LV;
				return updateJson;
			}
			int ID;
			int LV;
			int SPHERE;
			string NAME;
			int POS;
			int DES;
			unsigned FLY;
		};
		void insertSimData(simPlane sP);
		vector<simPlane> simData;
	};
}