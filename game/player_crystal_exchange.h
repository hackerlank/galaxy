#pragma once
#include "block.h"
#include <boost/unordered_map.hpp>
namespace gg{

	const static string player_crystal_exchange_db_name = "gl.player_crystal_exchage";
	const static string last_update_time_str = "lut";		//上次更新时间
	const static string player_exchange_items_msg_str = "pem";
	const static string exchange_items_id_str = "eid";		
	const static string remaining_num_str = "rn";			//剩余可以兑换物品数
	const static string exchange_max_num_str = "em";		//可以兑换的最大数

	struct PlayerExItemsMsg 
	{
		int ID;
		int remainingNum;
		int maxNum;
		//Json::Value exItemsJson;
		PlayerExItemsMsg(){
			ID = 0;
			remainingNum = 0;
			maxNum = 0;
			//exItemsJson = Json::arrayValue;
		}
	};

	struct ExItemsMsg
	{
		int ID;
		int costCrystalID;			//晶石
		int costCrystalNum;			
		int costSparID;				//水晶
		int costSparNum;
		int maxNum;
		Json::Value exItemsJson;
		bool operator==(const ExItemsMsg &e)const {
			return (e.ID == ID&&e.costCrystalID == costCrystalID&&e.costSparID == costSparID&&e.maxNum == maxNum);
		}
		ExItemsMsg(){
			ID = 0;
			costCrystalID = -1;
			costCrystalNum = 0;
			costSparID = -1;
			costSparNum = 0;
			maxNum = 1;
			exItemsJson = Json::arrayValue;
		}
	};
	
	static ExItemsMsg nullExItem;
	typedef boost::unordered_map<int, ExItemsMsg>	ExItemsMap;
	class player_crystal_exchange : public Block
	{
	public:
		player_crystal_exchange(playerData &own);
		~player_crystal_exchange();
		virtual void autoUpdate();
		virtual bool save();
		virtual bool get();
		void update();
		//void setExItemsMsg();
		//return:  -1 非法物品 -2 剩余数量不够 0 兑换成功 1仓库满，发邮件
		int exchageItems(int ID, Json::Value itemsJson);
		int getRemainingNum(int itemsID);
		const static ExItemsMsg& getExItemsMsg(int exItemsID);
		static void setExItemsMsg(ExItemsMap& ex);
	private:
		void package(Json::Value& msg);
		void resetData();			//5点是要重置数据的
	private:
		static ExItemsMap exItemsMsg;
		typedef boost::unordered_map<int, PlayerExItemsMsg> PlayerExItemMap;
		PlayerExItemMap m_PlayerExItemMsg;
		unsigned m_LastUpdateTime;
	};

}
