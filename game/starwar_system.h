#pragma once

#include "json/json.h"
#include "msg_base.h"
#include "starwar_helper.h"
#include "time_helper.h"
#include "war_story.h"
#include <set>
#include "commom.h"

#define starwar_sys (*gg::starwar_system::starwarSys)

namespace gg
{
	using namespace starwar;

	const static string starwar_config_file_dir_str = "./instance/countrymap/";
	const static string starwar_npc_file_dir_str = "./instance/country_war/";
	const static string starwar_country_reward_file_dir_str = "./instance/countryreward/countreward.json";
	const static string starwar_country_reward_xiuzhan_file_dir_str = "./instance/countryreward/countreward_xiuzhan.json";
	const static string starwar_country_huangjintime_file_dir_str = "./instance/countryreward/country_huangjintime.json";
	const static string starwar_country_player_hp_file_dir_str = "./instance/countryreward/country_player_hp.json";
	const static string starwar_country_touxi_file_dir_str = "./instance/countryreward/country_touxi.json";
	const static string starwar_country_yueqian_file_dir_str = "./instance/countryreward/country_yueqian.json";
	const static string starwar_country_yueqian_cost_file_dir_str = "./instance/countryreward/country_yueqian_cost.json";
	const static string starwar_country_item_file_dir_str = "./instance/countryreward/country_item.json";
	const static string starwar_country_item_cost_file_dir_str = "./instance/countryreward/country_item_cost.json";
	const static string starwar_country_item_effect_file_dir_str = "./instance/countryreward/country_item_effect.json";
	const static string starwar_country_item_time_file_dir_str = "./instance/countryreward/country_item_time.json";
	const static string starwar_country_gongxun_file_dir_str = "./instance/countryreward/country_gongxun.json";
	const static string starwar_country_reward_tongyi_file_dir_str = "./instance/countryreward/country_reward_tongyi.json";
	const static string starwar_countreward2_file_dir_str = "./instance/countryreward/countreward2.json";
	const static string starwar_countrewardyear_file_dir_str = "./instance/countryreward/countrewardyear.json";
	const static string starwar_task_file_dir_str = "./instance/countryreward/wartask.json";
	const static string starwar_country_to_load_npc_dir_str = "./instance/countryreward/country_to_load_npc.json";

	const static string reportStarwarDir = "./report/engross/StarwarPVP/";

	const static string starwar_path_info_field_str = "paths";
	const static string starwar_star_info_field_str = "stars";
	const static string starwar_star_num_field_str = "sn";
	const static string starwar_king_info_field_str = "ki";
	const static string starwar_move_rate_field_str = "mr";
	const static string starwar_supply_rate_field_str = "sr";
	const static string starwar_war_state_field_str = "ws";
	const static string starwar_unity_state_field_str = "us";

	class basePlayer
	{
		public:
			basePlayer(int player_id, int country_id)
				: _player_id(player_id), _country_id(country_id){}

			static basePlayer makeKey(int player_id)
			{
				return basePlayer(player_id);
			}

			int getPlayerId() const { return _player_id; }
			int getCountryId() const { return _country_id; }

			bool operator<(const basePlayer& rhs) const
			{
				return _player_id < rhs.getPlayerId();
			}

		private:
			basePlayer(int player_id)
				: _player_id(player_id){}

		private:
			int _player_id;
			int _country_id;
	};

	class starwarTime
	{
		public:
			struct tm
			{
				public:
					int hour;
					int min;
			};

			void load(const Json::Value& info);
			unsigned beginSecs() const { return _begin_secs; }
			unsigned endSecs() const { return _end_secs; }
			tm beginTm() const { return _begin_tm; }
			tm endTm() const { return _end_tm; }

		private:
			unsigned _begin_secs;
			tm _begin_tm;
			unsigned _end_secs;
			tm _end_tm;
	};

	class starwarParam
	{
		public:
			starwarParam() : _load_npc(true){}

			typedef vector<effect> effects;

			void loadFile();
			bool toLoadNpc() const { return _load_npc; }

			unsigned getOutput(int type) const { return _output[type]; }
			unsigned getMaxOutput() const { return _max_output; }

			double getTruceRate() const { return _truce_rate; }
			const starwarTime& getWarTime() const { return _war_time; }

			const starwarTime& getPrimeTime() const { return _prime_time; }
			int getMoveRate() const { return _move_rate; }
			int getSupplyRate() const { return _supply_rate; }

			unsigned getMaxSupply() const { return _max_supply; }
			unsigned getSupplyCost(int type, int position) const { return _supply_cost[position][type]; }

			unsigned getTransferCdCostPerMin() const { return _transfer_cd_cost_per_min; }
			unsigned getTransferCost(int buy_times) const 
			{
				if (buy_times >= (int)_transfer_cost.size())
					buy_times = (int)_transfer_cost.size() - 1;
				return _transfer_cost[buy_times];
			}

			unsigned getTransferTimesPerDay() const { return _transfer_times_limit_per_day; }
			unsigned getMaxTransferTimes() const { return _max_transfer_times; }
			unsigned getTransferCd() const { return _transfer_cd; }

			unsigned getRaidCost() const { return _raid_cost; }
			unsigned getRaidTimesPerDay() const { return _raid_times_limit_per_day; }
			unsigned getRaidEnableLvLimit() const { return _raid_enable_lv_limit; }

			int getItemLv() const { return commom_sys.randomList(_lv_rate) + 1; }
			int getItemType() const { return commom_sys.randomList(_type_rate); }
			unsigned getItemCost(int buy_times) const
			{
				if (buy_times >= (int)_item_cost.size())
					buy_times = (int)_item_cost.size() - 1;
				return _item_cost[buy_times];
			}
			effect getItemEffect(int type, int lv) const { return _item_effect[type][lv - 1]; }
			int getExploit(int lv, bool result) const
			{
				int index = (lv - 1) / 10;
				if (index >= (int)_exploits.size())
					index = (int)_exploits.size() - 1;
				return result ? _exploits[index].first : _exploits[index].second;
			}

			int getItemCd(int type) const
			{
				if (type >= (int)_item_cd.size())
					return 0;
				return _item_cd[type];
			}

			int getUnityReward() const { return _unity_reward; }
			int getRankingReward(int rank) const
			{
				if (rank > (int)_ranking_reward.size() || rank <= 0)
					rank = _ranking_reward.size();
				return (int)((double)_ranking_reward[rank - 1] * getRewardRate());
			}

			double getRewardRate() const
			{ 
				int index = (na::time_helper::get_current_time() - season_sys.getServerOpenTime()) / (4 * na::time_helper::DAY) - 1;
				if (index >= (int)_reward_rate.size())
					index = _reward_rate.size() - 1;
				if (index < 0)
					index = 0;
				return _reward_rate[index];
			}
			
		private:
			void loadOutput();
			void loadTruce();
			void loadPrime();
			void loadSupply();
			void loadTransfer();
			void loadRaid();
			void loadItem();
			void loadExploit();
			void loadUnityReward();
			void loadRankingReward();
			void loadRewardRate();
			void loadToLoadNpc();

		private:
			vector<unsigned> _output;
			unsigned _max_output;
			starwarTime _war_time;
			starwarTime _prime_time;
			double _truce_rate;
			int _move_rate;
			int _supply_rate;
			unsigned _max_supply;
			vector<unsigned> _supply_cost[2];
			unsigned _transfer_cd_cost_per_min;
			vector<unsigned> _transfer_cost;
			unsigned _transfer_cd;
			unsigned _transfer_times_limit_per_day;
			unsigned _max_transfer_times;
			unsigned _raid_cost;
			unsigned _raid_enable_lv_limit;
			unsigned _raid_times_limit_per_day;
			Json::Value _lv_rate;
			Json::Value _type_rate;
			vector<unsigned> _item_cost;
			vector<effects> _item_effect;
			vector<int> _item_cd;
			vector<pair<int, int> > _exploits;
			unsigned _unity_reward;
			vector<int> _ranking_reward;
			vector<double> _reward_rate;
			bool _load_npc;
	};

	struct loadHelper
	{
		public:
			loadHelper(const starwar::position& pos, const starwar::playerInfo& info)
				: _position(pos), _playerInfo(info){}
			starwar::position _position;
			starwar::playerInfo _playerInfo;
	};

	typedef map<int, loadHelper> loadHelperMap;

#define CheckCountryId() \
		if (d->Base.getSphereID() < 0 || d->Base.getSphereID() > 2) \
			Return(r, _no_country); \
		d->Starwar.updateData(); \
		enterPlayer(d); \

	class starwar_system
	{
		public:
			enum{
				_max_rank_num = 50,
				_task_num = 5,
				_resource_period = 300,
				_mainInfo_period = 2,
			};

			typedef boost::unordered_map<int, taskPtr> starwarTaskList;
			typedef boost::unordered_map< int, mapDataPtr > npcMap;

			typedef boost::function<void()> tickHandler;

			static starwar_system* const starwarSys;

			starwar_system();
			void initData();

			void callBack(int type, int id, unsigned time, bool enter_star = false);
			void record(saveHelper* ptr){ _save_helper.insert(ptr); }
			void kickPlayer(playerDataPtr d, bool offline = false);
			void enterPlayer(playerDataPtr d);
			void sendMainInfo(playerDataPtr d);

			void playerInfoReq(msg_json& m, Json::Value& r);
			void moveReq(msg_json& m, Json::Value& r);
			void reportListReq(msg_json& m, Json::Value& r);
			void kickReq(msg_json& m, Json::Value& r);
			void rewardReq(msg_json& m, Json::Value& r);
			void starInfoReq(msg_json& m, Json::Value& r);
			void useItemReq(msg_json& m, Json::Value& r);
			void openItemReq(msg_json& m, Json::Value& r);
			void flushItemReq(msg_json& m, Json::Value& r);
			void buyItemReq(msg_json& m, Json::Value& r);
			void playerRankReq(msg_json& m, Json::Value& r);
			void countryRankReq(msg_json& m, Json::Value& r);
			void taskInfoReq(msg_json& m, Json::Value& r);
			void taskCommitReq(msg_json& m, Json::Value& r);
			void buyTransferTimeReq(msg_json& m, Json::Value& r);
			void clearTransferCdReq(msg_json& m, Json::Value& r);
			void rankingRewardReq(msg_json& m, Json::Value& r);
			void kingGatherReq(msg_json& m, Json::Value& r);
			void officerGatherReq(msg_json& m, Json::Value& r);
			void greatEventReq(msg_json& m, Json::Value& r);
			void settleNpcReq(msg_json& m, Json::Value& r);
			void getShortestPathReq(msg_json& m, Json::Value& r);
			void signStarReq(msg_json& m, Json::Value& r);

			int getCostTime(int from_id, int to_id);

			void taskUpdateInnerReq(msg_json& m, Json::Value& r);
			void updateInfoInnerReq(msg_json& m, Json::Value& r);
			void attackWithPlayerInnerReq(msg_json& m, Json::Value& r);
			void attackWithNpcInnerReq(msg_json& m, Json::Value& r);
			void attackWithPlayerInnerResp(const int battleResult, Battlefield field);
			void attackWithNpcInnerResp(const int battleResult, Battlefield field);
			void noticeNpcBattleInnerReq(msg_json& m, Json::Value& r);
			void noticeOccupy(int player_id, int star_id);
			void pushMessage(const message& m);

			starwarTrigger& getTrigger(){ return _trigger; }
			const starwarParam& getParam() const { return _param; }
			int getMoveRate() const 
			{
				if (_prime_state)
					return _param.getMoveRate();
				return 1;
			}
			int getSupplyRate() const
			{
				if (_prime_state)
					return _param.getSupplyRate();
				return 1;
			}

			double getOutputRate()
			{
				return _war_state ? 1.0 : (_unity_state ? 0.0 : 0.1);
			}

			bool getWarState() const { return _war_state; }

			void checkAndUpdate(boost::system_time& tmp);

			void addBattleBuff(playerDataPtr pdata, battleSide& side);
			int getResource(int star_id, int country_id)
			{
				return _starList.getStar(star_id)->getRecourse(country_id);
			}
			starList& getStarList(){ return _starList; }
			int getStarNum() const { return _starList.num(); }

			starPtr getStar(int star_id){ return _starList.getStar(star_id); }
			pathPtr getPath(int path_id){ return _pathList.getPath(path_id); }
			bool isAsync(){ return _async; }
			occupyInfo& getOccupyInfo(){ return _occupyInfo; }

			void getTaskList(vector<taskRecord>& task_list, playerDataPtr d);
			taskPtr getTaskPtr(int task_id);

			void getNpcGuildList(guildList& guild_list, int type);

			Json::Value getStarBuff(playerDataPtr d);
			Json::Value getOutputBuff(playerDataPtr d);

			rankRecord getOldRank(int player_id, int country_id);
			unsigned getUnityReward(int country_id);
			unsigned getLastUnityTime() const { return _systemInfo.getSystemTime(_unity_time_index); }

			void sendBack(unsigned now, int player_id, const string& name, int face_id, int country_id, bool update = true);

			unsigned getNextPrimeTime() const { return _next_prime_time; }
			bool getPrimeState() const { return _prime_state; }

			void testBroadcastReq(msg_json& m, Json::Value& r);
			void postToUpdatePosition(int player_id, int type, int id, unsigned now, bool next = false);

			bool checkNpcConfig(int type, int npc_id);

			unsigned getNextGoldTime() const 
			{ 
				if (_unity_state)
					return _next_prime_time;
				else
					return _next_gold_time;
			}

			unsigned& getPlaneCount(){ return _plane_count; }

		private:
			void postToUpdateTask(int player_id, int type, int temp = -1);

			unsigned getTickTime(unsigned cycle_time, unsigned last_time = 0);
			void resetTickTime(unsigned index);

			void run(); 
			void handleEvent(const triggerItem& e);
			int handlePathEvent(const triggerItem& e);
			int handleMessage(const message& m);
			int move(const message& m);
			int transfer(const message& m);
			int raid(const message& m);
			int kingGather(const message& m);
			int officerGather(const message& m);
			int useItem(const message& m);
			int settleNpc(const message& m);
			int autoMove(const message& m);
			int autoTransfer(const message& m);

			int getTransferTimes(int star_num);

			typedef map<int, starwar::playerInfo> playerMap;
			void loadDB();
			void loadPlayerInfo(playerMap& player_map);
			void loadStarwarInfo(playerMap& player_map);
			void loadStarDB();
			void loadRankingDB();
			void loadSystemInfoDB();
			void loadFile();
			void loadTask();
			void loadNpc();
			void createDir();
			void tickMainInfo();
			void tickSupplyRate(unsigned now);
			void tick(unsigned now);
			void initTriggerItems(unsigned now);
			triggerItem getWarStateTriggerItem(bool init = false);
			triggerItem getPrimeStateTriggerItem(bool init = false);
			triggerItem getResourceTriggerItem();
			triggerItem getRankingTriggerItem(bool init = false);
			triggerItem getUnityTriggerItem(bool init = false);
			triggerItem getBroadcastItem(int type, unsigned now);
			triggerItem getPrimeTimeBroadcast(unsigned now, bool init = false);
			triggerItem getTestBugItem(unsigned now);
			int tickWarState(const triggerItem& item);
			int tickPrimeState(const triggerItem& item);
			int tickResource(unsigned now);
			int tickRanking(unsigned now);
			int tickUnity(const triggerItem& item);
			int tickBroadcast(const triggerItem& item);
			int tickLoadPlayer(const triggerItem& item);
			int tickPrimeTimeBroadcast(const triggerItem& item);
			int tickPrimeTimeSummaryBroadcast(const triggerItem& item);
			int tickClearSignInfo(const triggerItem& item);
			int tickTestBug(const triggerItem& item);
			void reset(unsigned now);
			
			void save();
			void response(int player_id, int type, int result);
			void noticeUnity(int country_id);
			void noticeKingGather(int country_id, int player_id = -1);
			void gatherBroadcast(playerDataPtr d, int star_id);
			void killKingBroadcast(playerDataPtr d, playerDataPtr targetP);
			void mainInfoBroadcast();
			void updateStarInfo(playerDataPtr d, int star_id, int begin = 1, int count = 5);
			void updateSignInfo(playerDataPtr d);
			void updateSignInfo(int country_id);
			double getRateBySup(int sup, bool type);
			void adjustRate(double& rate, bool type);

			inline void updateBattleTask(bool result, playerDataPtr d, playerDataPtr targetP = playerDataPtr());
			inline int getBattleSilver(bool result);
			inline int getBattleCost(bool result, unsigned now, int position, battleSidePtr ptr, playerDataPtr d, playerDataPtr targetP);

		private:
			starList _starList;
			pathList _pathList;
			msgQueue _mQueue;
			starwarTrigger _trigger;
			vector<unsigned> _exploit_npc;
			vector<npcMap> _npcMap;
			
			bool _busy;
			bool _war_state;
			bool _prime_state;
			bool _unity_state;
			bool _async;
			unsigned _next_war_time;

			unsigned _plane_count;
			
			set<saveHelper*> _save_helper;
			set<basePlayer> _playerList;
			set<int> _cachePlayer;
			set<int> _ownerPlayer;
			string _targetName;
			battleParam _buff[battleParam::_index_max];

			starwarParam _param;
			rankMgr _rankList[3];
			occupyInfo _occupyInfo;
			systemInfo _systemInfo;

			Json::Value _taskProbs;
			starwarTaskList _taskList;

			unsigned _next_prime_time;
			unsigned _next_gold_time;

			loadHelperMap _loadHelperMap;

			unsigned _max_num;


			starwar::shortestPath _shortestPath;
	};
}
