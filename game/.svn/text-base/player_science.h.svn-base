#pragma once
#include "block.h"
#include "pilot_def.h"
#include "player_pilots.h"
#include <vector>
#include <boost/unordered_map.hpp>

using namespace std;

namespace gg{
//#define HP_BUFFER_EFFECT_ID   18
#define KEJI_BUFFER_EFFECT_ID 100

	enum sciencedescribe
	{
		mother_inspire = 0,			//母体激发
		cross_longbow_array,		//十字长弓阵
		hull_strengthening,			//船体加固
		dipper_formation,			//北天仙后阵
		ballistic_Analysis,			//弹道分析
		t_hammer_formation,			//T字锤形阵
		ghost_armor,				//幽控装甲
		energy_grid,				//能量栅格
		dipper_hook_formation,		//北斗天勾阵
		force_Manipulation,			//原力操控
		wedge_raid_formation,		//楔形突击阵
		magnetic_array_block,		//磁阵阻扰
		z_long_snake_formation,		//Z型长蛇阵
		Crystal_strengthening,		//晶核强化
		macedonia_square,			//马其顿方阵
		real_shield_formation,		//铁甲金盾阵
		strategic_deployment,		//战略部署
		fleet_training,				//舰队集训
		motor_advanced,				//机动进阶
		shaped_armor,				//聚能装甲
		energy_principle,			//能量原理
		shield_dispersio,			//护盾频散
		psionic_convergence,		//灵能汇聚
		force_antibody,				//原力抗体
		triangle_attack_formation,	//三角进攻阵
		keith_formation,			//凯斯顿梯阵
		roman_double_formation,		//罗马双线阵
		crystal_refining,			//晶石精炼


		the_end
	};


	struct ConfigEntity{
		/*{"id":0,
			"comeOutLevel":20,//建筑需要到达此等级才激活科技
			"maxLevel":100,
			"updateLevel":1,
			"cost":[6,12,18,24,30,36,42,48,54,60,66,72,78,84,90,96,102,108,114,120,192,198,204,222,228,234,246,258,264,276,342,354,366,384,390,408,414,432,438,456,720,750,780,810,840,870,900,930,960,990,1470,1530,1590,1650,1710,1770,1830,1890,1950,2010,2490,2550,2610,2670,2730,2790,2850,2910,2970,3030,3090,3150,3210,3270,3330,3390,3450,3510,3570,3630,3690,3750,3810,3870,3930,4800,5100,5400,5700,6000,6300,6600,6900,7200,7500,7800,8100,8400,8700,9000],
			"effect":[[9,0.004]]
		  }
			对武将的加成
			character:

			//0普攻 1普防 2战攻 3战防 4策攻 5策防 6闪避 7格挡 8暴击

			//9反击 10物理增伤 11物理减伤 12战法增伤 13战法减伤 14魔法增伤 15魔法减伤

			//16治疗量系数 17被治疗量系数

			
			科技独有的加成（100 开始）
			100 
			*/

		ConfigEntity():id(0), comeOutLevel(0), maxLevel(0), updateLevel(0), zhenxing(0){};
		int id;
		int comeOutLevel;
		int maxLevel;
		int updateLevel;
		int zhenxing;
		vector<int> cost_config;
		vector< vector< double > > effect;
		bool isAttri()
		{
			for (unsigned i = 0; i < effect.size(); ++i)
			{
				int effect_id = static_cast<int>(effect[i][0]);
				if (effect_id >= 0 && effect_id < characterNum)return true;
			}
			return false;
		}
	};

	typedef boost::unordered_map<int, ConfigEntity> scienceConfigMap;
	const static string playerScienceDBStr	   = "gl.player_science";
	const static string player_science_data_field = "d";
	/* 
		d:[
			{id:xx, lv:xx},
			{id:x2, lv:x2}
		]
	*/
	const static string player_science_id_field = "id";
	const static string player_science_lv_field = "lv";
	const static string player_science_need_keji_field = "kj";

	struct  playerScienceItem
	{
		const static playerScienceItem NullValue;
		playerScienceItem():id(0), lv(0){};
		int id;
		int lv;
	};

	struct playerScienceBuffer
	{
		playerScienceBuffer(){
			memset(this, 0x0, sizeof(playerScienceBuffer));
		}
		double initialCharacter[characterNum];
	};

	class  playerScience:public Block{
	public:
		typedef boost::unordered_map<int, playerScienceItem> scienceMaps;
		playerScience(playerData& own):Block(own){};
		static void initData(); 
		virtual bool save();
		virtual bool get();
		virtual void update();
		virtual void autoUpdate();
		void add(playerScienceItem item);

		int embattleLV(const int id);
		playerScienceBuffer getBufferNoFormat();
		playerScienceBuffer getBuffer();
		double getKeJiBuffer();//获取科技点加成   百分比  (加成 80% 则返回  0.8)
		int getMaxScienceLv();
		int getScienceLv(int scienceID);	//得到某个科技的等级
		void del(int id);
		bool set_level(int id, int lv);
		scienceMaps science_maps;

	};
}