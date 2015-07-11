#pragma once

#include "block.h"
#include <boost/unordered_map.hpp>
#include "battle_member.h"
#include "game_helper.hpp"

namespace gg
{
	const static string playerWarStoryLastInfoDBStr = "gl.player_war_story_last_info";
	const static string playerWarStoryDBStr = "gl.player_war_story_m";
	const static string playerWarStoryTeamDBStr = "gl.player_war_story_team";
	
	const static string playerLastWarStroyTargetNameStr = "tin";//taregt information
	const static string playerLastWarStroyTargetIDStr = "tid";//taregt information

	const static string playerMapRewardStr = "mr";
	const static string playerChaperRewardStr = "cr";
	const static string playerChaperReward2Str = "cr2";
	const static string playerWarStoryDataStr = "d";
	const static string playerWarStoryNpcTeamFirstBloodStr = "fb";
	const static string playerWarStoryMapIDStr = "m";
	const static string playerWarStoryIDStr = "id";
	const static string playerWarStoryPastStarStr = "ps";

	struct playerMapData{
		playerMapData() : localID(-1), pastStar(-1){}
		playerMapData(const int lID, const int pStar) : localID(lID), pastStar(pStar){}
		int localID;
		int pastStar;
	};

	class playerAllWarStory;
	class playerWarStory : public Block
	{
	public:		
		playerWarStory(playerData& own, const int mID);
		~playerWarStory(){};
		virtual void update();
		virtual bool save();
		virtual bool get();
		virtual void autoUpdate();
		void processMapData(const int localID, const int star);
		void processMapData(const int localID, Battlefield field);
		bool isProcessLocal(const int localID);
		void setHandler(playerAllWarStory* hand){father =  hand;}
		int getStarNum();
		const int mapID;
		bool Chaper1RW(){return chaper1Reward;}
		bool Chaper2RW(){return chaper2Reward;}
		bool Chaper3RW(){ return chaper3Reward; }
		void SetChaper1RW(const bool val);
		void SetChaper2RW(const bool val);
		void SetChaper3RW(const bool val);
	private:
		typedef boost::unordered_map<int, playerMapData> playerLocalMap;
		playerLocalMap playerLocal_;
		playerAllWarStory* father;
		bool chaper1Reward;
		bool chaper2Reward;
		bool chaper3Reward;
	};

	struct playerTeamMapData 
	{
//	public:
		playerTeamMapData();
		playerTeamMapData(int mapID, bool firstBlood = false);
		int mapID_;
		bool hasNpcTeamFirstBlood_;
	};

// 	class playerWarStoryTeam : public Block
// 	{
// 	public:
// 		playerWarStoryTeam(playerData& own, const int mID);
// 		~playerWarStoryTeam(){};
// 		virtual void update();
// 		virtual bool save();
// 		virtual bool get();
// 		virtual void autoUpdate();
// 		void updateTeam();
// 	private:
// 		vector<playerTeamMapData> team_;
// 	};

	typedef boost::shared_ptr<playerWarStory> playerWarStoryPtr;
//	typedef boost::shared_ptr<playerTeamMapData> playerWarStoryTeamPtr;

	class playerAllWarStory : public Block
	{
	public:
		virtual void autoUpdate(){}
		virtual bool get();
		virtual bool save();
		playerAllWarStory(playerData& own);
		bool isProcessMap(const int localID);
		
		int updateClientMap(const int mapID);
		void processWarStory(const int localID, const int star);
		void processWarStory(playerDataPtr own, Battlefield field);
		int  currentProcess;
		int lastTargetID;
		string lastTargetName;
		void SetLastWSInfo(const int tID, const string tName);
		playerWarStoryPtr getMap(const int mapID);

		bool saveTeam(playerData& own);
		bool getTeam(playerData& own);
		void autoUpdateTeam(int mapID, playerData& own);
		bool hasFirstBlood(int mapID);
		void setFirstBlood(int mapID);
		void packageTeam(int mapID, playerData& own, Json::Value &packJson);
	private:		
		void load();
		typedef boost::unordered_map<int, playerWarStoryPtr> playerWMMap;
		typedef boost::unordered_map<int, playerTeamMapData> playerWMTMap;
		static playerWarStoryPtr Create(playerData& own, const int mID)
		{
			void* p = GGNew(sizeof(playerWarStory));
			if(p == NULL)return playerWarStoryPtr();
			return playerWarStoryPtr(new(p) playerWarStory(own, mID), Destory);
		}
		static void Destory(playerWarStory* point)
		{
			point->~playerWarStory();
			GGDelete(point);
		}
		playerWMMap playerMap_;
		playerWMTMap playerTMap_;
	};

}