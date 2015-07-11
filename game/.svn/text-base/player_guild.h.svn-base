#pragma once

#include "block.h"

namespace gg
{
	const static string playerGuildDBStr = "gl.player_guild";
	const static string playerCDonateStr = "cdn";
	const static string playerSDonateStr = "sdn";
	const static string logPlayerGuild = "log_player_guild";
	namespace NPPlayerGuild
	{
		enum{
			log_apply_guild,
			log_cancel_apply,
			log_change_guild,
			log_alter_contribute,
		};
	}

	class playerBase;
	class TTManager;
	class playerGuild : public Block
	{
		friend class TTManager;
		friend class playerBase;
	public:
		enum
		{
			flag_level = 1,
			guild_level = 2,
			levy_way = 3,
			guild_contribute = 4,
			mine_refine = 5,
			privateer_license = 6,
			financial_planning = 7,

			science_count,
		};
		playerGuild(playerData& own);
		virtual void autoUpdate();
		virtual void update();
		virtual bool save();
		virtual bool get();

		Json::Value packageGuildJson();

		int getApplyTimes();
		int getDeclareTimes();
		int getKickTimes();
		int getEmailTimes();
		unsigned getJoinDayTime();
		inline unsigned getJoinTime(){return joinTime;}
		inline int getLeaveContribute(){ return leaveContribute; }
		int PopLeaveContribute();
		inline int getFirst(){return scFirst;}

		void alterApplyTimes(const int num);
		void alterDeclareTimes(const int num);
		void alterKickTimes(const int num);
		void alterEmailTimes(const int num);

		void setApplyTimes(const int num);
		void setDeclareTimes(const int num);
		void setKickTimes(const int num);
		void setEmailTimes(const int num);

		void setFirst(const int scID);
		void setGuildID(const int ID, const int contribute = 0);
		void pushGuild(const int ID);
		void removeGuild(const int ID);
		void clearGuildIDList();
		inline bool hasJoinGuild()
		{
			return GuildID >= 0;
		}
		bool hasApplyGuild(const int gID);
		//
		int getGuildID(){return GuildID;}
		vector<int> getJoinList(){return GuildIDList;}

		int getDonate(const int sID);
		void tickDonate(const int sID);

		//
		int getHelpTick();
		void tickHelp();

		int getRemainLeveyTimes(unsigned updateTime);
		void alterRemainLeveyTimes(int num);
		int getMaxLeveyTimes();
		string getLastGName()
		{
			return lastGuildName;
		}
	private:
		int getCDonate();
		int getSDonate();
		int GuildID;
		int applyJoin;
		int declareWords;
		int kickMember;
		int TeamEmail;
		unsigned joinTime;
		vector<int> GuildIDList;
		int cDonate;
		int sDonate;
		int* donatePointer[science_count - 1];//数据库
		int scFirst;
		int helpTick;
		int leaveContribute; 
		string  lastGuildName;

		//星盟战
		int remainLevyTimes;
		int levyUpdateTimeAfterBattle;//不同的征税次数刷新标识
		unsigned levyUpdateTimeDaily;//每天的征税次数刷新时间
	};
}