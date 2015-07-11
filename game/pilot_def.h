#pragma once

namespace gg
{
	const static unsigned attributeNum = 3;
	const static unsigned armsModulesNum = 6;
	const static unsigned characterNum = 19;

	struct armsRestraint
	{
		armsRestraint(){
			atkModule = 0.0;
			defModule = 0.0;
		}
		armsRestraint(double aM, double dM){
			atkModule = aM;
			defModule = dM;
		}
		double atkModule;
		double defModule;
	};

	enum{
		idx_dominance,
		idx_power,
		idx_intelligence,
	};

	enum{
		idx_atkModule,
		idx_defModule,
		idx_warModule,
		idx_warDefModule,
		idx_magicModule,
		idx_magicDefModule,
	};

	enum{
		idx_atk,
		idx_def,
		idx_war,
		idx_war_def,
		idx_magic,
		idx_magic_def,
		idx_dodge,
		idx_block,
		idx_crit,
		idx_counter,
		idx_phyAddHurt,
		idx_phyCutHurt,
		idx_warAddHurt,
		idx_warCutHurt,
		idx_magicAddHurt,
		idx_magicCutHurt,
		idx_cureModule,
		idx_beCureModule,
		idx_hp,
	};
}