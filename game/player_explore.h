#pragma once
#include "block.h"
namespace gg{

	const static string mysqlLogExplore = "log_explore";
	const static string player_explore_db_name = "gl.player_explore";
	const static string ordinary_materials = "om";		//普通物资
	const static string hq_materials = "hm";			//高品质物资
	const static string ordinary_materials_max = "omm";	//普通物质上限
	const static string get_ordinary_materials = "gom";		//得到的普通物资
	const static string interplanetary_update_time = "iut";			//上次更新时间

	class player_explore :
		public Block
	{
	public:
		player_explore(playerData& own);
		~player_explore(void);
	
		virtual void autoUpdate();
		virtual bool save();
		virtual bool get();

		int getOrdinaryMaterials();
		int getGetOrdinaryMaterials();
		//int getOrdinaryMaterialsMax();
		int getHQMaterials();
		int GetAReward(Json::Value& jv, Json::Value& fail_json);
		unsigned getUpdateTime();
		void alterOrdinaryMaterials(int num);
		void alterHQMaterials(int num);
		void clearMaterials();
		void addPlayerPackage(Json::Value& msg);
	private:
		void update();
		int m_HQMaterials;
		int m_OrdinaryMaterials;
		int m_GetOrdinaryMaterials;
		//int m_OrdinaryMaterialsMax;
		unsigned m_UpdateTime;
	};
}
