#pragma once

#include "block.h"
#include <boost/unordered_map.hpp>
#include "secretary_helper.h"

namespace gg
{
	const static string secretary_id_field_str = "id";
	const static string secretary_sid_field_str = "sid";
	const static string secretary_name_field_str = "nm";
	const static string secretary_level_field_str = "lv";
	const static string secretary_exp_field_str = "exp";
	const static string secretary_c_exp_field_str = "cep";
	const static string secretary_locked_field_str = "lk";

	class playerSecretary
	{
		public:

			int getId() const { return _id; }
			int getSid() const { return _sid; }
			bool isLocked() const { return _locked; }
			int getExp() const { return _exp; }
			int getCExp() const { return _cExp; }
			int getLv() const { return _level; }

			void setName(const string& name){ _name = name; }
			void setLocked(bool flag){ _locked = flag; }
			bool alterExp(int& exp, int max_lv);
			bool resetExp(int& exp, int max_lv);
			int getAddExp(int exp, int max_lv);

			void package(Json::Value& info);
			void simplePackage(Json::Value& info);
			const secConfigPtr& getConfigPtr();

			void init(secConfigPtr ptr);
			void init(const mongo::BSONElement& obj);
			mongo::BSONObj toBSON();

			playerSecretary(int id)
				: _id(id), _sid(-1), _name(""), _level(0), _exp(0), _cExp(0), _locked(false), _ptr(secConfigPtr()){}

		private:
			int _id;
			int _sid;
			string _name;
			int _level;
			int _exp;
			int _cExp;
			bool _locked;

			secConfigPtr _ptr;
	};

	typedef boost::shared_ptr<playerSecretary> playerSecPtr;

	const static string secretary_db_str = "gl.player_secretary";
	const static string secretary_list_field_str = "sec";
	const static string secretary_formation_field_str = "fm";
	const static string secretary_owned_sid_list_field_str = "osl";
	const static string secretary_chip_num_field_str = "cn";
	const static string secretary_cd_field_str = "cd";
	const static string secretary_lottery_low_free_times = "lft";
	const static string secretary_lottery_low_times = "llt";
	const static string secretary_lottery_high_times = "lht";
	const static string secretary_upgrade_times = "upt";
	const static string secretary_next_update_time_field_str = "nut";
	const static string secretary_max_id_field_str = "mi";
	const static string secretary_max_num_field_str = "mn";
	const static string secretary_first_lottery_field_str = "fl";
	const static string secretary_show_cd_field_str = "scd";

	class playerSecretaries : public Block
	{
		public:
			playerSecretaries(playerData& own);
			virtual bool get();
			virtual bool save();
			virtual void autoUpdate();
			void packageInfo(Json::Value& info);
			void packageSidList(Json::Value& info);
			void packageParam(Json::Value& info);
			void packageGmList(Json::Value& info);

			void info();
			void update();
			void param();

			void checkAndUpdate();

			int lottery(int type, bool state, bool is_free);
			int swap_exp(int left_id, int right_id);
			int rename(int id, const string& name);
			int lock(int id, bool state);
			int upgrade(int id, int type);
			int swallow(int left_id, const Json::Value& right_list);
			int resolve(int id);
			int combine(bool state);
			int embattle(int id, int pos, bool up);
			int upLoad(int id, int pos);
			int unLoad(int id, int pos);
			int show(int id, int channel, const string& target_name);

			int addSecretary(int sid);
			int addSecretatyWithoutCheck(int sid);
			int addSecretaryWithMail(secConfigPtr ptr);
			int getChipNum() const { return _chip_num; }

			void getAttrs(void* point);
			void resetAttrs();

			bool canAddSecretary(int num) const;
			bool inSidList(int sid) const { return _ownedSidList.find(sid) != _ownedSidList.end(); }

			int inFormat(int id);
			bool resetExp(playerSecPtr ptr, int& exp, int max_lv);
			bool alterExp(playerSecPtr ptr, int& exp, int max_lv);

			void updateInfo(playerSecPtr ptr);
			void updateFm();
			void delInfo(int id);

			void openFm();

		private:
			unsigned getCdInterval(int type);
			int getLotteryCost(int type, bool state);
			int upgradeLimit(playerSecPtr ptr);
			int getCurrentMaxLv();
			void getSecretaryBroadcast(int type, int sid);

			typedef boost::unordered_map<int, playerSecPtr> playerSecMap;
			playerSecMap _secMap;
			playerSecPtr _formation[secretary::_max_upload_num];
			int _chip_num;
			
			set<int> _ownedSidList;

			unsigned _cd[3];
			unsigned _low_free_times;
			unsigned _upgrade_times[2];
			unsigned _lottery_high_times;
			unsigned _lottery_low_times;
			bool _first_lottery;
			
			unsigned _next_update_time;
			unsigned _show_cd;

			int _max_id;

			bool _inited;
			double _attrs[characterNum];

			static playerSecPtr _closePtr;
			static playerSecPtr _freePtr;

			Json::Value _updateInfo;
	};
}