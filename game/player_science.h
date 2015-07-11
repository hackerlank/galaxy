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
		mother_inspire = 0,			//ĸ�弤��
		cross_longbow_array,		//ʮ�ֳ�����
		hull_strengthening,			//����ӹ�
		dipper_formation,			//�����ɺ���
		ballistic_Analysis,			//��������
		t_hammer_formation,			//T�ִ�����
		ghost_armor,				//�Ŀ�װ��
		energy_grid,				//����դ��
		dipper_hook_formation,		//�����카��
		force_Manipulation,			//ԭ���ٿ�
		wedge_raid_formation,		//Ш��ͻ����
		magnetic_array_block,		//��������
		z_long_snake_formation,		//Z�ͳ�����
		Crystal_strengthening,		//����ǿ��
		macedonia_square,			//����ٷ���
		real_shield_formation,		//���׽����
		strategic_deployment,		//ս�Բ���
		fleet_training,				//���Ӽ�ѵ
		motor_advanced,				//��������
		shaped_armor,				//����װ��
		energy_principle,			//����ԭ��
		shield_dispersio,			//����Ƶɢ
		psionic_convergence,		//���ܻ��
		force_antibody,				//ԭ������
		triangle_attack_formation,	//���ǽ�����
		keith_formation,			//��˹������
		roman_double_formation,		//����˫����
		crystal_refining,			//��ʯ����


		the_end
	};


	struct ConfigEntity{
		/*{"id":0,
			"comeOutLevel":20,//������Ҫ����˵ȼ��ż���Ƽ�
			"maxLevel":100,
			"updateLevel":1,
			"cost":[6,12,18,24,30,36,42,48,54,60,66,72,78,84,90,96,102,108,114,120,192,198,204,222,228,234,246,258,264,276,342,354,366,384,390,408,414,432,438,456,720,750,780,810,840,870,900,930,960,990,1470,1530,1590,1650,1710,1770,1830,1890,1950,2010,2490,2550,2610,2670,2730,2790,2850,2910,2970,3030,3090,3150,3210,3270,3330,3390,3450,3510,3570,3630,3690,3750,3810,3870,3930,4800,5100,5400,5700,6000,6300,6600,6900,7200,7500,7800,8100,8400,8700,9000],
			"effect":[[9,0.004]]
		  }
			���佫�ļӳ�
			character:

			//0�չ� 1�շ� 2ս�� 3ս�� 4�߹� 5�߷� 6���� 7�� 8����

			//9���� 10�������� 11������� 12ս������ 13ս������ 14ħ������ 15ħ������

			//16������ϵ�� 17��������ϵ��

			
			�Ƽ����еļӳɣ�100 ��ʼ��
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
		double getKeJiBuffer();//��ȡ�Ƽ���ӳ�   �ٷֱ�  (�ӳ� 80% �򷵻�  0.8)
		int getMaxScienceLv();
		int getScienceLv(int scienceID);	//�õ�ĳ���Ƽ��ĵȼ�
		void del(int id);
		bool set_level(int id, int lv);
		scienceMaps science_maps;

	};
}