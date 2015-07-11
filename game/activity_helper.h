#pragma once

#include "playerManager.h"
#include "action_def.h"

namespace gg
{
	namespace activity
	{
		enum
		{
			_levy = 1,
			_force_levy,
			_war_story,
			_strengthen,
			_bring_up,
			_occupy_mine,
			_study,
			_arena_challenge,
			_attack_player,
			_get_salary,
			_invest,
			_produce,
			_force_produce,
			_psionic_research,
			_online_reward,
			_guild_reward,
			_guild_donate,
			_buy_limited_resources,
			_get_month_card,
			_buy_month_card,
			_mining,
			_transfer,
			_trade,
			_explore,
			_upgrade_secretary,		

			_ri_li_wan_ji = 0,
			_jing_bing_qiang_zhen,
			_fu_xing_gao_zhao,
			_cai_yuan_guang_jin,
			_tian_dao_chou_qin,
			_wei_zhen_huan_yu,
			_guo_fu_min_qiang,
			_unused_1,
			_tu_fei_meng_jin,
			_fort_war,
			_arena,
			_ruler_competition,
			_guild_battle,
			_mine_battle,
			_starwar,
			_ruler_betting,
			_world_boss,
			_time_limited_activity_end,

			_open = 0,
			_access,

			_success = 0,
			_client_error,
			_received,
			_points_not_enough,
		};

		class iCondition
		{
			public:
				virtual ~iCondition(){}
				virtual bool isTrue(playerDataPtr d) = 0;
		};
		typedef boost::shared_ptr<iCondition> cnnPtr;
		typedef vector<cnnPtr> conditionList;
		class cnnLevel : public iCondition
		{
			public:
				cnnLevel(int lv) : _lv(lv){}
				virtual ~cnnLevel(){}
				virtual bool isTrue(playerDataPtr d)
				{
					return d->Base.getLevel() >= _lv;
				}
			private:
				int _lv;
		};
		class cnnGuild : public iCondition
		{
			public:
				virtual ~cnnGuild(){}
				virtual bool isTrue(playerDataPtr d)
				{
					return d->Guild.getGuildID() != -1;
				}
		};
		class cnnCountry : public iCondition
		{
			public:
				virtual ~cnnCountry(){}
				virtual bool isTrue(playerDataPtr d)
				{
					return d->Base.getSphereID() >= 0 && d->Base.getSphereID() <= 2;
				}
		};
		class mAcitivityInfo;
		class cnnSeason : public iCondition
		{
			public:
				virtual ~cnnSeason(){}
				cnnSeason(int sprint, int summer, int autumn, int winter, mAcitivityInfo* ptr);
				virtual bool isTrue(playerDataPtr d);
			private:
				int _season[4];
		};
		class cnnTime : public iCondition
		{
			public:
				virtual ~cnnTime(){}
				cnnTime(const int begin_time, const int end_time);
				cnnTime(const string& begin_time, const string& end_time);
				virtual bool isTrue(playerDataPtr d);
			private:
				int _begin_time;
				int _end_time;
				unsigned _next_update_time;
				bool _state;
		};

		class cnnAnd : public iCondition
		{
			public:
				virtual ~cnnAnd(){}
				cnnAnd(const conditionList& cnn_list) : _cnn_list(cnn_list){}
				virtual bool isTrue(playerDataPtr d)
				{
					ForEach(conditionList, iter, _cnn_list)
					{
						if (!(*iter)->isTrue(d))
							return false;
					}
					return true;
				}
			private:
				conditionList _cnn_list;
		};
		class cnnOr : public iCondition
		{
			public:
				virtual ~cnnOr(){}
				cnnOr(const conditionList& cnn_list) : _cnn_list(cnn_list){}
				virtual bool isTrue(playerDataPtr d)
				{
					ForEach(conditionList, iter, _cnn_list)
					{
						if ((*iter)->isTrue(d))
							return true;
					}
					return false;
				}
			private:
				conditionList _cnn_list;
		};
		class cnnGuildReward : public iCondition
		{
			public:
				virtual ~cnnGuildReward(){}
				virtual bool isTrue(playerDataPtr d);
		};
		class mAcitivityInfo
		{
			public:
				mAcitivityInfo();
				mAcitivityInfo(const Json::Value& info);

				void load(const Json::Value& info);

				void addCondition(int type, const cnnPtr& ptr)
				{
					if (type == _open)
						_opened_cnns.push_back(ptr);
					else
						_access_cnns.push_back(ptr);
				}

				bool opened(playerDataPtr d)
				{
					ForEach(conditionList, iter, _opened_cnns)
					{
						if (!(*iter)->isTrue(d))
							return false;
					}
					return true;
				}
				bool isAccess(playerDataPtr d)
				{
					ForEach(conditionList, iter, _access_cnns)
					{
						if (!(*iter)->isTrue(d))
							return false;
					}
					return true;
				}

				int getId() const { return _id; }
				int getLimitTimes() const { return _limit_times; }
				int getPoints() const { return _points; }

				Json::Value getSeason();
				void setSeason(const Json::Value& season);

				double getRate(playerDataPtr d);
				Json::Value& getValue(){ return _cValue; }

			private:
				typedef vector<double> rateList;
				static rateList initDefaultRate();
				static Json::Value initDefaultSeason();
				static rateList _default_rates;
				static Json::Value _default_season;

				int _id;
				int _limit_times;
				int _points;
				Json::Value _season;

				int _state;
				Json::Value _cValue;

				conditionList _access_cnns;
				conditionList _opened_cnns;
		};

		class mReward
		{
			public:
				void load(const Json::Value& info);
				reward getReward(playerDataPtr d);
				int getPoints() const { return _points; }

			private:
				reward _reward;
				bool _rate;
				int _points;
		};
	}
	typedef boost::shared_ptr<activity::mAcitivityInfo> activityPtr;
}