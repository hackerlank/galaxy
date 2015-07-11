#pragma once

#include "block.h"
#include "DataNotice.hpp"
#include <boost/unordered_map.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>

namespace gg
{
	const static string strTradeItemDB = "gl.player_trade_item";
	const static string strDBPlayerTrade = "gl.player_trade";
	const static string strDBTradeSkill = "gl.player_trade_skill";
	class playerBase;
	class TTManager;
	class playerTrade :
		public Block
	{
		friend class TTManager;
		friend class playerBase;
	public:
		playerTrade(playerData& own);
		~playerTrade();
		virtual void autoUpdate();
		virtual void update();
		virtual bool save();
		virtual bool get();
		spInt PlaneLevel;
		spUInt PassTimes;
		spInt Voucher;//��ǰ��õ�ó��Ʊ
		spInt PassVoucher;//�����Ҫ
		spInt VoucherMaxToday, VoucherMaxHis;//�������, ��ʷ���
		spInt TaskState;// 0δ������ 1��������Ϊ��� 2��������û�콱
 		spInt Destination;//�趨Ŀ��
		spUInt FlyTime;//��ʼ����ʱ��
		spDouble Speed;//
		int getRWTimes();
		void tickRWTimes();
		spInt orgPosition;//���λ��
		spInt Position;//��ǰλ��
		spInt VoucherHistory;//��ʷ���
		spInt reqTimes;//�������
		static int moveToNext(const int pos, const int des);
		void moveToNext(bool tickTime = false);
		int getPosition();
		inline bool TaskComplete()
		{
			return TaskState > 1;
		}
		inline bool hasTask(){
			return TaskState > 0;
		}
		inline bool doTask()
		{
			return TaskState == 1;
		}
		unsigned _FlyTime();
		bool Flying();
		bool Berth();
		void addBuff(const int buffID, const unsigned endTime = 0);
		bool hasBuff(const int buffID);
		void updateRichgName(const string nName);
		void updateRichpName(const string nName);
		unsigned showCD;
		int addExp(const int num);
	private:
		spInt PlaneExp;
		spInt TodayRewardTimes;//�����콱����
		unsigned historyTime, vTTime, vHTime;
		inline bool internalIsFly()
		{
			return Position != Destination();
		}
		inline bool internalIsBerth()
		{
			return Position == Destination();
		}
		typedef boost::unordered_map<int, spUInt> buffMap;
		buffMap _buffMap;
		void dataUpdate();
		void VoucherAlter(int oV, int nV);
		void HistoryVoucherAlter(int oV, int nV);
		void PassTimesAlter(int oV, int nV);
		void reqTimesAlter(int oV, int nV);
	};

	class playerTradeBag;
	class tradeItem
	{
	public:
		tradeItem(playerData& own);
		tradeItem(playerData& own, const int Id, const int num, const int aver);
		~tradeItem(){}
		inline int getID()
		{
			return itemID();
		}
		int numAlter(const int num, const int price = 0);
		inline int Num()
		{
			return stackNum();
		}
		inline int AverPrice()
		{
			return averPrice();
		}
		inline bool nullItem()
		{
			return itemID < 0 || stackNum <= 0;
		}
	private:
		spInt itemID;
		spInt stackNum;
		spInt averPrice;
		playerData& own;
	};

	class playerTradeBag : public Block
	{
		friend class tradeItem;
	public:
		playerTradeBag(playerData& own);
		~playerTradeBag(){}
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate();
		virtual void update();
		virtual void doEnd();
		bool canAddItem(const int itemID, const int num);
		bool addItem(const int itemID, const int num, const int totalCost);
		bool canDelItem(const int itemID, const int num);
		bool delItem(const int itemID, const int num);
		void clearBag();
		tradeItem& getItem(const int itemID);
	private:
		int getCapacity();
		int totalNum();
		void itemUpdate(tradeItem& item, const int num);
		typedef boost::unordered_map<int, tradeItem> itemMap;
		itemMap Bag;
		typedef boost::unordered_set<int> removeItem;
		removeItem rmItem;
		int capacity;
		int totalN;
		void dataUpdate();
	};

	namespace TradeSkill
	{
		static const int IDX = 6;
	}

	class playerTradeSkill : public Block
	{
	public:
		playerTradeSkill(playerData& own);
		~playerTradeSkill(){}
		static void initSkill();
		virtual bool get();
		virtual bool save();
		virtual void autoUpdate();
		virtual void update();
		int getSkill(const int id);
		bool setSkill(const int id, const int level);
		void battleAdd(double* point);
	private:
		static vector< vector<double> > ATTRI;
		int SKILL[TradeSkill::IDX];
	};
}