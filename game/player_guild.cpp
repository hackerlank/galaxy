#include "player_guild.h"
#include "helper.h"
#include "mongoDB.h"
#include "guild_system.h"
#include "record_system.h"
#include "task_system.h"
#include "gate_game_protocol.h"
#include "player_base.h"
#include "player_paper.h"

using namespace mongo;

namespace gg
{
	const static string playerCurrentGuildIDStr = "cgi";
	const static string playerFrontGuildIDStr = "fgi";
	const static string playerJoinGuildIDStr = "pid";
	const static string playerApplyJoinStr = "aj";//今日批准入会次数
	const static string playerDeclareWordStr = "dw";//今日修改宣言次数
	const static string playerKickMemberStr = "km";//今日踢人次数
	const static string playerTeamMailStr = "tm";//今日群邮件次数
	const static string playerGuildStr = "gu";//公会id
	const static string playerGuildNameStr = "gun";//公会名字
	const static string playerJoinTimeStr = "pjt";//加入公会的时间
	const static string playerHelpTickStr = "ht";//
	const static string strRemainLevyTimes = "rlt";
	const static string strLevyUpdateTimeAfterBattle = "lutab";
	const static string strlevyUpdateTimeDaily = "lutd";
	const static string strLeaveContribute = "lco";
	const static string strLastGuildName = "lgna";

	playerGuild::playerGuild(playerData& own) : Block(own)
	{
		helpTick = 0;
		GuildID = -1;
		GuildIDList.clear();
		applyJoin = 0;
		declareWords = 0;
		kickMember = 0;
		TeamEmail = 0;
		lastGuildName = "";
		joinTime = na::time_helper::get_current_time();
		sDonate = 0;
		cDonate = 0;

		donatePointer[flag_level - 1] = &sDonate;
		donatePointer[guild_level - 1] = &cDonate;
		donatePointer[levy_way - 1] = &cDonate;
		donatePointer[guild_contribute - 1] = &cDonate;
		donatePointer[mine_refine - 1] = &cDonate;
		donatePointer[privateer_license - 1] = &cDonate;
		donatePointer[financial_planning - 1] = &cDonate;

		scFirst = playerGuild::guild_level;

		remainLevyTimes = 0;
		levyUpdateTimeAfterBattle = 0;
		levyUpdateTimeDaily = 0;

		leaveContribute = 0;
	}

	void playerGuild::autoUpdate()
	{
		update();
	}

	static const string applyGuildListStr = "agl";
	void playerGuild::update()
	{
		Json::Value updateJson = packageGuildJson();
		own.sendToClient(gate_client::guild_palyer_base_info_resp, updateJson);
	}

	Json::Value playerGuild::packageGuildJson()
	{
		own.TickMgr.Tick();
		Json::Value updateJson;
		updateJson[msgStr][0u] =  0;
		updateJson[msgStr][1u][updateFromStr] = State::getState();
		updateJson[msgStr][1u][playerGuildStr] = GuildID;
		updateJson[msgStr][1u][playerGuildNameStr] = guild_sys.getGuildName(GuildID);
		updateJson[msgStr][1u][playerTeamMailStr] = TeamEmail;
		updateJson[msgStr][1u][playerKickMemberStr] = kickMember;
		updateJson[msgStr][1u][playerDeclareWordStr] = declareWords;
		updateJson[msgStr][1u][playerApplyJoinStr] = applyJoin;
		updateJson[msgStr][1u][playerHelpTickStr] = helpTick;
		updateJson[msgStr][1u][playerJoinTimeStr] = na::time_helper::get_current_time() - joinTime;
		Json::Value& gList = updateJson[msgStr][1u][applyGuildListStr];
		gList = Json::arrayValue;
		for (unsigned i = 0; i < GuildIDList.size(); ++i)
		{
			gList[i] = GuildIDList[i];
		}
		return updateJson;
	}

	bool playerGuild::save()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		BSONObjBuilder objBuild;
		objBuild << playerIDStr << own.playerID << playerCurrentGuildIDStr << GuildID <<
			playerApplyJoinStr << applyJoin << playerKickMemberStr << kickMember <<
			playerTeamMailStr << TeamEmail << playerDeclareWordStr << declareWords <<
			playerJoinTimeStr << joinTime << playerHelpTickStr << helpTick <<
			playerCDonateStr << cDonate << playerSDonateStr << sDonate << memFirstStr << scFirst << 
			strRemainLevyTimes << remainLevyTimes << strLevyUpdateTimeAfterBattle << levyUpdateTimeAfterBattle <<
			strlevyUpdateTimeDaily << levyUpdateTimeDaily << strLeaveContribute << leaveContribute <<
			strLastGuildName << lastGuildName;
		BSONArrayBuilder arr;
		for (unsigned i = 0; i < GuildIDList.size();++i)arr << GuildIDList[i];
		objBuild << playerJoinGuildIDStr << arr.arr();
		return db_mgr.save_mongo(playerGuildDBStr, key, objBuild.obj());
	}

#define SetValue(value, num)\
	{\
	value = num;\
	value = value < 0 ? 0 : value;\
	}

	void playerGuild::setApplyTimes(const int num)
	{
		SetValue(applyJoin, num);
		helper_mgr.insertSaveAndUpdate(this);
	}
	void playerGuild::setDeclareTimes(const int num)
	{
		SetValue(declareWords, num);
		helper_mgr.insertSaveAndUpdate(this);
	}
	void playerGuild::setKickTimes(const int num)
	{
		SetValue(kickMember, num);
		helper_mgr.insertSaveAndUpdate(this);
	}
	void playerGuild::setEmailTimes(const int num)
	{
		SetValue(TeamEmail, num);
		helper_mgr.insertSaveAndUpdate(this);
	}

#define PlusValue(value, num)\
	{\
		value += num;\
		value = value < 0 ? 0 : value;\
	}

	void playerGuild::alterApplyTimes(const int num)
	{
		PlusValue(applyJoin, num);
		helper_mgr.insertSaveAndUpdate(this);
	}
	void playerGuild::alterDeclareTimes(const int num)
	{
		PlusValue(declareWords, num);
		helper_mgr.insertSaveAndUpdate(this);
	}
	void playerGuild::alterKickTimes(const int num)
	{
		PlusValue(kickMember, num);
		helper_mgr.insertSaveAndUpdate(this);
	}
	void playerGuild::alterEmailTimes(const int num)
	{
		PlusValue(TeamEmail, num);
		helper_mgr.insertSaveAndUpdate(this);
	}

	bool playerGuild::get()
	{
		mongo::BSONObj key = BSON(playerIDStr << own.playerID);
		mongo::BSONObj obj = db_mgr.FindOne(playerGuildDBStr, key);
		if(!obj.isEmpty()){			
			checkNotEoo(obj[playerCDonateStr]){cDonate = obj[playerCDonateStr].Int();}
			checkNotEoo(obj[playerSDonateStr]){sDonate= obj[playerSDonateStr].Int();}
			checkNotEoo(obj[playerCurrentGuildIDStr]){GuildID = obj[playerCurrentGuildIDStr].Int();}
			checkNotEoo(obj[playerApplyJoinStr]){applyJoin= obj[playerApplyJoinStr].Int();}
			checkNotEoo(obj[playerDeclareWordStr]){declareWords= obj[playerDeclareWordStr].Int();}
			checkNotEoo(obj[playerKickMemberStr]){kickMember= obj[playerKickMemberStr].Int();}
			checkNotEoo(obj[playerTeamMailStr]){TeamEmail = obj[playerTeamMailStr].Int();}
			checkNotEoo(obj[playerHelpTickStr]){ helpTick = obj[playerHelpTickStr].Int(); }
			checkNotEoo(obj[memFirstStr]){scFirst = obj[memFirstStr].Int();}
			checkNotEoo(obj[playerJoinTimeStr]){ joinTime = (unsigned)obj[playerJoinTimeStr].Int(); }
			checkNotEoo(obj[strRemainLevyTimes]){ remainLevyTimes = (unsigned)obj[strRemainLevyTimes].Int(); }
			checkNotEoo(obj[strLevyUpdateTimeAfterBattle]){ levyUpdateTimeAfterBattle = (unsigned)obj[strLevyUpdateTimeAfterBattle].Int(); }
			checkNotEoo(obj[strlevyUpdateTimeDaily]){ levyUpdateTimeDaily = (unsigned)obj[strlevyUpdateTimeDaily].Int(); }
			checkNotEoo(obj[strLeaveContribute]){ leaveContribute = obj[strLeaveContribute].Int(); }
			checkNotEoo(obj[strLastGuildName]){ lastGuildName = obj[strLastGuildName].String(); }
			checkNotEoo(obj[playerJoinGuildIDStr]){
				GuildIDList.clear();
				vector<BSONElement> vec = obj[playerJoinGuildIDStr].Array();
				for (unsigned i = 0; i < vec.size(); ++i)GuildIDList.push_back(vec[i].Int());
			}
			return true;
		}				
		return false;
	}

	void playerGuild::setFirst(const int scID)
	{
		scFirst = scID;
		helper_mgr.insertSaveAndUpdate(this);
	}

	void playerGuild::setGuildID(const int ID, const int contribute)
	{
		StreamLog::Log(logPlayerGuild, own, GuildID, ID, NPPlayerGuild::log_change_guild);
		int oldGID = GuildID;
		GuildID = ID;
		scFirst = playerGuild::guild_level;
		if (GuildID < 0)
		{
			joinTime = 0;
			leaveContribute += int(contribute * 0.85);
			lastGuildName = guild_sys.getGuildName(oldGID);
			own.Paper.deadType(chat_guild);
		}
		else
		{
			own.Trade.updateRichgName(guild_sys.getGuildName(GuildID));
			joinTime = na::time_helper::get_current_time();
		}
		for (unsigned i = 0; i < GuildIDList.size();++i)
			guild_sys.removeJoinAndBackUpInfo(own.playerID, GuildIDList[i]);
		GuildIDList.clear();
		helper_mgr.insertSaveAndUpdate(this);

		if(ID != -1)
			task_sys.updateBranchTaskInfo(own.getOwnDataPtr(), _task_join_guild);
	}

	int playerGuild::PopLeaveContribute()
	{
		int num = leaveContribute;
		leaveContribute = 0;
		helper_mgr.insertSaveAndUpdate(this);
		return num;
	}

	void playerGuild::pushGuild(const int ID)
	{
		GuildIDList.push_back(ID);
		helper_mgr.insertSaveAndUpdate(this);
		StreamLog::Log(logPlayerGuild, own, "joinList", ID, NPPlayerGuild::log_apply_guild);
	}

	void playerGuild::removeGuild(const int ID)
	{
		for (unsigned i = 0; i < GuildIDList.size(); ++i)
		{
			if(ID == GuildIDList[i]){
				GuildIDList.erase(GuildIDList.begin() + i);
				helper_mgr.insertSaveAndUpdate(this);
				StreamLog::Log(logPlayerGuild, own, "cancel", ID, NPPlayerGuild::log_cancel_apply);
				break;
			}
		}
	}

	void playerGuild::clearGuildIDList()
	{
		GuildIDList.clear();
		helper_mgr.insertSaveAndUpdate(this);
	}

	bool playerGuild::hasApplyGuild(const int gID)
	{
		for (unsigned i = 0; i < GuildIDList.size(); ++i)
		{
			if(gID == GuildIDList[i])return true;
		}
		return false;
	}

	int playerGuild::getCDonate()
	{
		own.TickMgr.Tick();
		return cDonate;
	}

	int playerGuild::getSDonate()
	{
		own.TickMgr.Tick();
		return sDonate;
	}

	int playerGuild::getApplyTimes()
	{
		own.TickMgr.Tick();
		return applyJoin;
	}

	int playerGuild::getDeclareTimes()
	{
		own.TickMgr.Tick();
		return declareWords;
	}

	unsigned playerGuild::getJoinDayTime()
	{
		return na::time_helper::get_time_zero(joinTime);
	}

	int playerGuild::getEmailTimes()
	{
		own.TickMgr.Tick();
		return TeamEmail;
	}

	int playerGuild::getKickTimes()
	{
		own.TickMgr.Tick();
		return kickMember;
	}

	int playerGuild::getHelpTick()
	{
		own.TickMgr.Tick();
		return helpTick;
	}

	void playerGuild::tickHelp()
	{
		++helpTick;
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerGuild::getDonate(const int sID)
	{
		if(sID < 1 || sID >= science_count)return 0;
		own.TickMgr.Tick();
		return *donatePointer[sID - 1];
	}

	void playerGuild::tickDonate(const int sID)
	{
		if(sID < 1 || sID >= science_count)return;
		++(*donatePointer[sID - 1]);
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerGuild::getRemainLeveyTimes(unsigned updateTime)
	{
		unsigned cur_time = na::time_helper::get_current_time();
		if (updateTime != levyUpdateTimeAfterBattle || cur_time > levyUpdateTimeDaily)
		{
			if (cur_time > levyUpdateTimeDaily)
			{
				boost::posix_time::ptime p = boost::posix_time::from_time_t(cur_time);
				tm t = boost::posix_time::to_tm(p);
				unsigned standard_time = cur_time - t.tm_hour * 3600 - t.tm_min * 60 - t.tm_sec;
				if (t.tm_hour < 5)
					levyUpdateTimeDaily = standard_time + 5 * 3600;
				else
					levyUpdateTimeDaily = standard_time + 5 * 3600 + 86400;
			}
			if (updateTime != levyUpdateTimeAfterBattle)
			{
				levyUpdateTimeAfterBattle = updateTime;
			}
			remainLevyTimes = getMaxLeveyTimes();
			helper_mgr.insertSaveAndUpdate(this);
		}
		return remainLevyTimes;
	}

	void playerGuild::alterRemainLeveyTimes(int num)
	{
		remainLevyTimes += num;
		remainLevyTimes = remainLevyTimes >= 0 ? remainLevyTimes : 0;
		helper_mgr.insertSaveAndUpdate(this);
	}

	int playerGuild::getMaxLeveyTimes()
	{
		Guild::guildPtr gdPtr = guild_sys.getGuild(own.Guild.getGuildID());
		Guild::memberPtr mem = gdPtr->getMember(own.playerID);
		int official = mem->offcial;

		int lvy = 0;
		switch (official)
		{
		case Guild::guild_leader:
			lvy = 9;
			break;
		case Guild::guild_vice_leader:
			lvy = 8;
			break;
		case Guild::guild_old_member:
			lvy = 7;
			break;
		case Guild::guild_senior_member:
			lvy = 6;
			break;
		default:
			lvy = 5;
			break;
		}

		return lvy;
	}
}