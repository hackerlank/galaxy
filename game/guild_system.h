#pragma once

#include <boost/unordered_map.hpp>
#include <map>
#include <string>
#include "msg_base.h"
#include "playerManager.h"
#include "json/json.h"
#include <boost/shared_ptr.hpp>
#include "game_helper.hpp"
#include <vector>
#include "time_helper.h"
#include "mongoDB.h"
using namespace na::msg;
using namespace std;

#define guild_sys (*gg::guild_system::guildSys)

namespace gg
{

	namespace Guild
	{
		enum
		{
			guild_leader,
			guild_vice_leader,
			guild_old_member,
			guild_senior_member,
			guild_middle_member,
			guild_lower_member,
			guild_comon_member,
		};

		class HelpData;
		typedef boost::shared_ptr<HelpData> HelpDataPtr;
		typedef boost::unordered_map<string, HelpDataPtr> HelpMap;
		class guildMember
		{
		public:
			static void destory(guildMember* point)
			{
				point->~guildMember();
				GGDelete(point);
			}
			~guildMember(){}
			inline int getPlayerID(){return playerID;}
			inline string getPlayerName(){return playerName;}
			inline int getFaceID(){ return faceID; }
			inline int getPlayerLv(){ return playerLevel; }
//			void formatDonate(const guildMember& mem);
			bool formatBaseInfomation(playerDataPtr player);
			mongo::BSONObj package();
			Json::Value packageUpdate();
			guildMember(playerDataPtr player, const int gID);
			guildMember(mongo::BSONElement& el, const int gID);
			int offcial;
			int rank;
			inline int getContribute()
			{
				return contribute;
			}
			void alterContribute(const int num);
			unsigned lastLoginTime;
			string joinWord;
			int warProcess;
			int arenaRank;
			HelpMap ownMap;
			const int guildID;
			inline int getHistory()
			{
				return history;
			}
		private:
			int contribute;
			int history;
			void initialData();
			int playerID;
			string playerName;
			int playerLevel;
			int faceID;
		};
		typedef boost::shared_ptr<guildMember> memberPtr;
		typedef boost::unordered_map<int, memberPtr> memberMap;
		typedef boost::unordered_map<string ,memberPtr> memNaMap;
		typedef vector<memberPtr> memberList;

		class JoinLimit
		{
		public:
			JoinLimit(){
				levelLimit = 10;
				rankLimit = -1;//无限制
				storyLimit = 0;
				autoApply = true;
			}
			~JoinLimit(){}
			Json::Value packageJLimit();
			mongo::BSONArray packageBLimit();
			bool autoAdd(){return autoApply;}
			bool passLimit(playerDataPtr player);
			void initLimit(mongo::BSONObj& obj);
			void setLimit(const int level, const int rank, const int war, const bool aApp);
		private:
			int levelLimit;
			int rankLimit;
			int storyLimit;
			bool autoApply;
		};


		class GuildScience
		{
		protected:
			struct science 
			{
				science(){
					level = 0;
					exp = 0;
					id = -1;
				}
				bool isVaild()
				{
					return id != -1;
				}
				int id;
				int level;
				int exp;
			};
		public:
			struct Config
			{
				Config()
				{
					ID = -1;
					Exp.clear();
					Add.clear();
					Vip.clear();
					Silver.clear();
					Gold.clear();
					Type = -1;
					LimitLevel = -1;
					LimitID = -1;
					LimitEffectIdx = -1;
				}
				int getADD(const int level)
				{
					if(level < 0 || Add.empty())return 0;
					if(level >= (int)Add.size())return Add.back();
					return Add[level];
				}
				bool isVaild()
				{
					return ID != -1;
				}
				int ID;
				vector<int> Exp;
				vector<int> Add;
				vector<int> Vip;
				vector<int> Silver;
				vector<int> Gold;
				int Type;
				int LimitLevel;
				int LimitID;
				int LimitEffectIdx;
			};
			struct ScienceData
			{
				ScienceData()
				{
					memset(this, 0x0, sizeof(ScienceData));
				}
				int ID;
				int LV;
				int ADD;
				int EXP;
			};
			GuildScience();
			~GuildScience(){}
			static void initConfig();
			void formatBson(mongo::BSONObj& obj);
			ScienceData getScienceData(const int sID);
			static Config getConfig(const int sID);
			int addScienceExp(const int sID, const int aExp);
			int setScienceLevel(int sID, int sLevel);
			void setFlagIcon(const int iID){++flagChange;flagIcon = iID;}
			int getFlagIcon(){return flagIcon;}
			int getFlagChange(){return flagChange;}
			mongo::BSONObj packageBScience();
			Json::Value packageJScience();

		private:
			int flagChange;
			int flagIcon;
			science& getScience(const int sID);
			void upgrade(science& sc);
			std::map<int, science> scienceMap;
			static std::map<int, Config> configMap;
		};

		class GuildJoin
		{
		public:
			GuildJoin();
			~GuildJoin(){}
			void addJoinMember(memberPtr mem);
			memberPtr getJoinMember(const int playerID);
			bool removeJoinMember(const int playerID);
			void initGuildJoin(mongo::BSONObj& obj, const int gID);
			Json::Value packageJoinMember();
			mongo::BSONArray packageBJoin();
			bool isFullList(){return JoinVec.size() >= 50;}
			bool hasJoin(const int playerID);
			int getJoinMemberNum(){return (int)JoinVec.size();}
		private:
			memberList JoinVec;
		};


		class GuildHelp;
		class guild;
		class HelpData
		{
		public:
			static HelpDataPtr Create(GuildHelp* h, memberPtr m, const int t)
			{
				void* p = GGNew(sizeof(HelpData));
				return HelpDataPtr(new(p)HelpData(h, m, t), destory);
			}
			static HelpDataPtr Create(GuildHelp* h, memberPtr m, mongo::BSONObj& obj)
			{
				void* p = GGNew(sizeof(HelpData));
				return HelpDataPtr(new(p)HelpData(h, m, obj), destory);
			}
			static void initData();
			inline static int TYPECAL(const int type, const int seed = 0)
			{
				return seed * 100 + type;
			}
			memberPtr const M_;
			inline string _KEY()
			{
				return _SH_KEY;
			}
			inline int _GET_TYPE() const
			{
				return TYPE_ % 100;
			}
			inline int _GET_EXTRA() const
			{
				return TYPE_ / 100;
			}
			Json::Value _JSON();
			mongo::BSONObj _BSON();
			inline int _NUM()
			{
				return H_NUM_;
			}
			bool _IN_HELPER(const int HELPER);
			bool _CanTick(const int From);
			bool _TICK(const int From);
			void _DEAD();
			inline bool _Over()
			{
				if (_PASTLIFE())_DEAD();
				return _over;
			}
			inline unsigned _COMEOUT()
			{
				return C_TIME;
			}
		private:
			bool _over;
			bool _PASTLIFE()
			{
				return na::time_helper::get_current_time() >= D_TIME;
			}
			static void destory(HelpData* point)
			{
				point->~HelpData();
				GGDelete(point);
			}
			HelpData(GuildHelp* h, memberPtr m, const int t);
			HelpData(GuildHelp* h, memberPtr m, mongo::BSONObj& obj);
			~HelpData(){}
			int H_NUM_;
			string _SH_KEY;
			int T_H_NUM_;
			GuildHelp* const Father;
			int TYPE_;
			unsigned C_TIME;
			unsigned D_TIME;
			typedef boost::unordered_set<int> HELPERLIST;
			HELPERLIST P_LIST_;
			static vector< vector<int> > TotalHelpVec;
		};

		class GuildHelp
		{
			friend class HelpData;
		public:
			const static unsigned SET_LIFE = 500;//数组500
			enum
			{
				mine_battle,
				pilot_core,
				world_boss,
				war_story_cd,

				help_end,
			};
			GuildHelp(guild* hand);
			~GuildHelp(){}

			bool AddHelp(playerDataPtr player, memberPtr mem, const int type, const int seed);
			int TickHelp(playerDataPtr player, memberPtr player_m, playerManager::BHIDMAP playerList, vector<string> keyList, int* cri = NULL);

			Json::Value packageGHelp(const int helper);
			void formatHelp(memberPtr mem, mongo::BSONObj& obj);
			bool removeHelp(const string key);
			bool removeHelp(const int playerID, const int TYPE);
			HelpDataPtr getHelpPtr(const string key);
		protected:
			void sortHelp();
			void removeHelp(memberPtr mem);
			void saveHelp(memberPtr mem);
			HelpMap ownMap;
			vector<HelpDataPtr> ownVec;
		private:
			//注册
			typedef boost::function< bool(playerDataPtr, memberPtr, const int, const int) > regFunc;
			typedef boost::unordered_map<int, regFunc> regMap;
			regMap _regMap;
			bool mine_battle_reg(playerDataPtr player, memberPtr mem, const int type, const int seed);
			bool pilot_core_reg(playerDataPtr player, memberPtr mem, const int type, const int seed);
			bool world_boss_reg(playerDataPtr player, memberPtr mem, const int type, const int seed);
			bool war_story_reg(playerDataPtr player, memberPtr mem, const int type, const int seed);
			//帮助
			typedef boost::function< bool(playerDataPtr, HelpDataPtr) > tickFunc;
			typedef boost::unordered_map<int, tickFunc> tickMap;
			tickMap _tickMap;
			bool mine_battle_tick(playerDataPtr player, HelpDataPtr help);
			bool pilot_core_tick(playerDataPtr player, HelpDataPtr help);
			bool world_boss_tick(playerDataPtr player, HelpDataPtr help);
			bool war_story_tick(playerDataPtr player, HelpDataPtr help);

			//更新前判断
			typedef boost::function< bool(const int, HelpDataPtr) > updateFunc;
			typedef boost::unordered_map<int, updateFunc> updateMap;
			updateMap _updateMap;
			bool world_boss_update(const int helper, HelpDataPtr help);

			//other
			HelpDataPtr reomveHelpD(const string key);
			void AddHelp(HelpDataPtr helpPtr, bool save = true);
			guild* const Father;
			void saveHelp(HelpDataPtr help);
		};

		class GuildHistory
		{
		public:
			GuildHistory();
			~GuildHistory(){}
			void addEvent(Json::Value& eventJson);
			void initGuildHistory(mongo::BSONObj& obj);
			Json::Value packageHistory(unsigned beginIdx, unsigned endIdx);
			mongo::BSONArray packageBHis();
		private:
			vector<Json::Value> eventVec;
		};

		class guild :
			public GuildScience , public JoinLimit , 
			public GuildJoin, public GuildHistory ,
			public GuildHelp
		{
		public:
			static bool descend(memberPtr member_1, memberPtr member_2)
			{
				return member_1->getHistory() > member_2->getHistory();
			}
			static bool ascendByOffcial(memberPtr member_1, memberPtr member_2)
			{
				if(member_1->offcial != member_2->offcial)return member_1->offcial < member_2->offcial;
				return member_1->getHistory() > member_2->getHistory();
			}
			static void destory(guild* point)
			{
				point->~guild();
				GGDelete(point);
			}
			guild(const int gID);
			~guild(){}
			bool initGuildMember(mongo::BSONObj& obj);
			size_t  getMemberNum(){return members.size();}
			memberPtr getMember(const int playerID);
			memberPtr getMember(const string name);
			int motifyWords(playerDataPtr player, string word);
			int kickMember(playerDataPtr player, playerDataPtr target);
			int memberLeave(playerDataPtr target);
			int alterMemContribute(playerDataPtr player, const int num);
			int memberDonate(playerDataPtr player, const int num, const int scID, bool& GL);
			int memberUpgrade(playerDataPtr player);
			int addMember(playerDataPtr player, const int of = guild_comon_member, bool sotrM = true);
			int memberJoin(playerDataPtr player, string words);
			int applyMember(playerDataPtr player, vector<playerDataPtr> targetVec, const int type);//剔除玩家或者自动退出
			int dismissGuild(const int playerID);
			int abdication(playerDataPtr player, playerDataPtr target);
			int cancelJoin(playerDataPtr player);
			void getOnline(playerManager::playerDataVec& vec);
			void sendToAllOnlineMember(const short protocol, Json::Value& msg);
			Json::Value packageBase(playerDataPtr player);
			Json::Value gmPackageBase();
			Json::Value packageMember(unsigned beginIdx, unsigned endIdx);
			mongo::BSONObj package();
			bool asyncPlayerData(playerDataPtr player);
			unsigned getCreateTime(){return createTime;}
			void sortMember();
			int getRemainPlace();
			int getKingdom(){return guildKingdom;}
			void setKingdom(const int kd){guildKingdom = kd;}
			int getOffcial(int player_id);
			void getAllMembers(vector<int> &memList);
			string guildName;
			string strLogGID();
			const int guildID;
			int rank;//排名
			string words;
		private:
			int guildKingdom;
			void addMember(memberPtr mem);
			bool canUpgrade(memberPtr mem);
			vector<memberPtr> getOffcialRank(const int offcial);
			void freeAllData();
			bool canKick(playerDataPtr player, playerDataPtr target);
			memberMap members;
			memNaMap memNas;
			memberList memberRank;
			unsigned createTime;
			int serverID;
		};
		typedef boost::shared_ptr<guild> guildPtr;
	}

	const static string dbGuildMember = "gl.guild_member";
	const static string dbGuildJoinList = "gl.guild_join";
	const static string dbGuildHelpList = "gl.guild_help";
	const static string dbGuildJoinLimit = "gl.guild_limit";
	const static string dbGuildScience = "gl.guild_science";
	const static string dbGuildHistory = "gl.guild_history";

	const static string guildIDStr = "gid";//公会ID
	const static string guildServerIDStr = "gsid";//公会创建的原始服务器ID
	const static string guildNameStr = "na";//公会名字
	const static string guildKingdomStr = "gki";//公会国属
	const static string guildWordsStr = "w";//公会宣言	
	const static string guildCreateTimeStr = "gct";//公会创建时间
	
	const static string guildMemberStr = "m";//公会成员 m:[]

	const static string joinMemberStr = "jm";//申请加入成员
	
	const static string joinLimitStr = "jl";//加入限制

	const static string guildHistorystr = "his";//历史

	const static string guildHelpStr = "ghl";//求助

	const static string guildFlagIconStr = "fic";//图标id
	const static string guildFlagChangeStr = "gfc";//更换图标
	const static string guildScienceStr = "sc";//公会科技 sc:[]
	const static string guildScienceIDStr = "sid";///科技id
	const static string guildScienceLevelStr = "slv";///科技等级
	const static string guildScienceExpStr = "sexp";//科技经验
	const static string guildLeagueLevelStr = "gll";//星盟等级
	const static string guildFlagLevelStr = "flv";//军徽等级
	
	const static string guildIsApplyStr = "ia";//是否加入该星盟
	
	//just update
	const static string guildListStr = "gl";//公会列表 gl:[]
	const static string guildLeaderInfoStr = "ln";//公会会长名字
	const static string guildViceLeaderInfoStr = "vln";//副会长信息
	const static string guildScienceIndexStr = "si";//科技的index
	const static string guildMemberNumStr = "mn";//当前会员数量
	const static string guildMemberContainStr = "mc";//公会容纳上限
	const static string guildRankStr = "rk";//公会排名
	const static string memberIsOnlineStr = "io";//成员是否在线
	const static string memberYourSelfStr = "ys";//你自己的信息
	const static string applyMemberStr = "amn";//申请加入人数
	const static string canUpgradeStr = "cu";//能否升官

	const static string memberIDStr = "pi";//成员的玩家ID
	const static string memberNameStr = "mna";//成员名字
	const static string memberFaceIDStr = "fid";//头像ID
	const static string memberLevelStr = "mlv";//成员等级
	const static string memberOffcialStr = "of";//成员官职
	const static string memberContributeStr = "co";//成员贡献
	const static string memberHistoryContributeStr = "hco";//成员历史贡献
	const static string memberLastLoginTimeStr = "llt";//成员最后登入时间
	const static string memberJoinWordStr ="jw";//入会宣言
	const static string memberWarProcessStr = "mwp";//推图
	const static string memberArenaRankStr = "mar";//竞技场排名
	const static string memberctiRank = "mcr";//贡献值排名
	const static string memFirstStr = "fsc";//优先科技


	class guild_system
	{
	public:
		static bool descend(Guild::guildPtr guild_1, Guild::guildPtr guild_2)
		{
			int g1L = guild_1->getScienceData(playerGuild::guild_level).LV;
			int g2L = guild_2->getScienceData(playerGuild::guild_level).LV;
			if(g1L != g2L)return g1L > g2L;
			int g1F =  guild_1->getScienceData(playerGuild::flag_level).LV;
			int g2F =  guild_2->getScienceData(playerGuild::flag_level).LV;
			if(g1F != g2F)return g1F > g2F;
			return guild_1->getCreateTime() < guild_2->getCreateTime();
		}
		static Guild::memberPtr CreateMember(playerDataPtr player, const int gID){
			void* p = GGNew(sizeof(Guild::guildMember));
			return Guild::memberPtr(new(p) Guild::guildMember(player, gID), Guild::guildMember::destory);
		}
		static Guild::memberPtr CreateMember(mongo::BSONElement& el, const int gID){
			void* p = GGNew(sizeof(Guild::guildMember));
			return Guild::memberPtr(new(p) Guild::guildMember(el, gID), Guild::guildMember::destory);
		}
		static Guild::guildPtr CreateGuild(const int gID){
			void* p = GGNew(sizeof(Guild::guild));
			return Guild::guildPtr(new(p) Guild::guild(gID), Guild::guild::destory);
		}
		static guild_system* const guildSys;
		void initData();
		void createGuild(msg_json& m, Json::Value& r);
//		void dismissGuild(msg_json& m, Json::Value& r);
		void changeGName(msg_json& m, Json::Value& r);
		void joinGuild(msg_json& m, Json::Value& r);
		void leaveGuild(msg_json& m, Json::Value& r);
		void memberUpgrade(msg_json& m, Json::Value& r);
		void kickMember(msg_json& m, Json::Value& r);
		void motifyWords(msg_json& m, Json::Value& r);
		void donateGuild(msg_json& m, Json::Value& r);
//		void upgradeGuildFlag(msg_json& m, Json::Value& r);
		void responseJoin(msg_json& m, Json::Value& r);
		void updateGuildList(msg_json& m, Json::Value& r);
		void updateGuildMember(msg_json& m, Json::Value& r);
		void gmUpdateGuildMember(msg_json& m, Json::Value& r);
		void updateGulildJoinMemBer(msg_json& m, Json::Value& r);
		void updateGuildHistory(msg_json& m, Json::Value& r);
		void setJoinLimit(msg_json& m, Json::Value& r);
		void updateGuildBase(msg_json& m, Json::Value& r);
		void abdication(msg_json& m, Json::Value& r);
		void cancelJoin(msg_json& m, Json::Value& r);
		void updateJoinGuild(msg_json& m, Json::Value& r);
		void updateGuildScience(msg_json& m, Json::Value& r);
		void memScientFirst(msg_json& m, Json::Value& r);
		void changeFlagIcon(msg_json& m, Json::Value& r);
		void gmAddGuildScienceExpReq(msg_json& m, Json::Value& r);
		void gmUpdateGuildBaseReq(msg_json& m, Json::Value& r);
		void updateGuildHelp(msg_json& m, Json::Value& r);
		void guildHelpReq(msg_json& m, Json::Value& r);
		void tickHelpReq(msg_json& m, Json::Value& r);
		void upgradeTradeSkillReq(msg_json& m, Json::Value& r);
		
		void removeHelp(playerDataPtr player, const int TYPE);
		Json::Value getPersonal(playerDataPtr player);
		int donateDefault(playerDataPtr player, int num);
		int donate(playerDataPtr player, int scID, int num);
		int alterContribute(playerDataPtr player, int num);
		playerManager::playerDataVec getOnlineMember(const int gID);
		void removeJoinAndBackUpInfo(const int playerID, const int guildID);
		void asyncPlayerData(playerDataPtr player);
		void addGuildScienceExp(playerDataPtr player, const int sID, const int num);
		Guild::GuildScience::ScienceData getScienceData(const int gID, const int sID);
		string getGuildName(const int gID);
		int getGuildOfficial(playerDataPtr player);
		Guild::guildPtr getGuild(const int gID);
		Guild::guildPtr getGuild(const string name);
		void addGuilBattleEvent(string guildName, int areaID);
		void sendHelpList(playerDataPtr player);
	private:
		int createNewGuildID();
		void sendToOnlineHelpList(Guild::guildPtr gPtr);
		void sendHelpList(playerDataPtr player, Guild::guildPtr gPtr);
		void sendScience(playerDataPtr player, Guild::guildPtr gPtr);
		void sendJoinMember(playerDataPtr player, Guild::guildPtr gPtr);
		void sendGuildBase(playerDataPtr player, Guild::guildPtr gPtr);
		void sortRank(const bool log = true);
		bool hasSameName(string name);
		Json::Value packageGuild(unsigned begin, unsigned end);

		void saveJoin(Guild::guildPtr gPtr);
		void saveLimit(Guild::guildPtr gPtr);
		void saveScience(Guild::guildPtr gPtr);
		void saveHistory(Guild::guildPtr gPtr);
		void saveMember(Guild::guildPtr gPtr);
		void removeGuild(const int gID, const string gName);
		void addGuild(Guild::guildPtr gPtr);
		void loadAllGuild();
		typedef boost::unordered_map<int, Guild::guildPtr> guildMap;
		typedef boost::unordered_map<string , Guild::guildPtr> gNameMap;
		typedef vector<Guild::guildPtr> guildList;
		guildMap guilds;
		gNameMap guildsName;
		guildList guildRank;
		struct TradeSkillCost
		{
			TradeSkillCost(int gx, int jb, int ry)
			{
				gongxian = gx;
				jinbi = jb;
				rongyu = ry;
			}
			int gongxian;
			int jinbi;
			int rongyu;
		};
		int currentGID;
		vector<TradeSkillCost> TradeSkillCostVec;
	};
}