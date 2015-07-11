

#include "explore_manager.h"
#include "response_def.h"
#include "mongoDB.h"
#include "helper.h"
#include "file_system.h"
#include "time_helper.h"
#include "chat_system.h"
#include "gm_tools.h"
#include "mongoDB.h"
#include "activity_system.h"
#include "activity_game_param.h"

#define LOTTERYSIZE  30

namespace gg{

	explore_manager* const explore_manager::playerExploreMgr = new explore_manager();

	const static string exploreRewards = "./instance/explore/Explore_1.json";			//奖励内容
	const static string exploreRewards2 = "./instance/explore/Explore_2.json";				//概率


	const static string startTimeStr = "st";
	const static string endTimeStr = "et";
	const static string materialsConsumptionStr_fin = "mcf";
	const static string materialsConsumptionStr_tag = "mct";

	const static string lottery_info_db_name = "gl.lottery_info";
	const static string lottery_key = "key";
	const static string lottery_fin_key = "fin";
	const static string lottery_tag_key = "tag";
	const static string lottery_content = "cn";

	const static string explore_set_content_db_name = "gl.explore_content";
	const static string explore_content_key = "e_key";
	const static string explore_content = "cn";

	explore_manager::explore_manager(void)
	{
	}


	explore_manager::~explore_manager(void)
	{
	}



	void explore_manager::initData()
	{
		//读取配置，得到内容和概率
		vector<double> probabilityVec;
		ExploreRewardsMap exploreRewardsMap;
		vector<double> probabilityVec2;
		ExploreRewardsMap exploreRewardsMap2;
		getExploreContent();
		//LotteryInfo内容初始化
		finLotteryList.clear();
		tagLotteryList.clear();
		getLotteryInfo();
	}

	//0：成功！
	//1：时间设置错误！
	//2：当前活动已开启，活动结束时间不能设置成早于当前时间的值
	void explore_manager::setExploreRewards(msg_json& m, Json::Value& r)
	{
		//AsyncGetPlayerData(m);
		ReadJsonArray;
		//下标是从0开始的
		int rResult = setExploreRewards(js_msg);
		if (rResult == 0)
		{
			finLotteryList.clear();
			tagLotteryList.clear();
			playerManager::playerDataVec vec = player_mgr.onlinePlayer();
			for (playerManager::playerDataVec::iterator iter = vec.begin(); iter != vec.end(); ++iter)
			{
				if ((*iter) != playerDataPtr())
				{
					updateLottery(*iter, 0);
					updateLottery(*iter, 1);
				}
			}
		}
		saveExploreContent(js_msg);
		//cout << js_msg.toStyledString() << endl;
		Return(r, rResult);
	}

	void explore_manager::queryExplore(msg_json& m, Json::Value& r)
	{
		if (startTime == endTime &&startTime == 0)
			Return(r, 0);
		r[msgStr][1u][0u] = startTime;
		r[msgStr][1u][1u] = endTime;
		r[msgStr][2u] = female_insect_nest.getMaterialsConsumptionNum();
		r[msgStr][3u] = the_ancient_god_remains.getMaterialsConsumptionNum();
		r[msgStr][4u] = materialsMax;
		//普通探险
		female_insect_nest.addPackageBack(r[msgStr][5u]);
		r[msgStr][6u][0u] = female_insect_nest.getGrowth();
		r[msgStr][6u][1u] = female_insect_nest.getGrowthMaxNum();
		the_ancient_god_remains.addPackageBack(r[msgStr][7u]);
		r[msgStr][8u][0u] = the_ancient_god_remains.getGrowth();
		r[msgStr][8u][1u] = the_ancient_god_remains.getGrowthMaxNum();
		Return(r, 0);
	}

	void explore_manager::refreshPlayerData(playerDataPtr player)
	{
		unsigned now = na::time_helper::get_current_time();
		unsigned updateTime = player->playerExplore.getUpdateTime();
		if (!(startTime <= updateTime&&updateTime <= endTime))
		{
			player->playerExplore.clearMaterials();
		}
	}

	void explore_manager::updateExploreRewardsContent(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		unsigned now = na::time_helper::get_current_time();
		unsigned updateTime = d->playerExplore.getUpdateTime();
		if (!(startTime <= updateTime&&updateTime <= endTime))
		{
			d->playerExplore.clearMaterials();
		}
		if (startTime <= now&&now <= endTime){
			//r[msgStr][0u] = 0;
			//r[msgStr][1u][updateFromStr] = State::getState();
			//r[msgStr][1u][startTimeStr] = startTime;
			//r[msgStr][1u][endTimeStr] = endTime;
			//r[msgStr][1u][materialsConsumptionStr_fin] = female_insect_nest.getMaterialsConsumptionNum();
			//r[msgStr][1u][materialsConsumptionStr_tag] = the_ancient_god_remains.getMaterialsConsumptionNum();
			//female_insect_nest.addPackage(r[msgStr][1u][female_insect_nest.getExploreName()]);
			//the_ancient_god_remains.addPackage(r[msgStr][1u][the_ancient_god_remains.getExploreName()]);
			//d->playerExplore.addPlayerPackage(r[msgStr][2u]);
			//packageLotteryInfo_fin(r[msgStr][3u]);		//打包虫族LotteryInfo信息
			//packageLotteryInfo_tag(r[msgStr][4u]);		//打包神族LotteryInfo信息
			//LogW<<"interplanetary_exploration"<<r.toStyledString()<<"//test"<<LogEnd;
			//Return(r,0);
			updateBaseInfo(d);
		}
		else{
			Return(r, 1);
		}
	}
	void explore_manager::updateLottery_fin(msg_json& m, Json::Value& r){
		AsyncGetPlayerData(m);
		updateLottery(d, 0);
	}
	void explore_manager::updateLottery_tag(msg_json& m, Json::Value& r){
		AsyncGetPlayerData(m);
		updateLottery(d, 1);
	}

	void explore_manager::femaleInsectNest(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int times = js_msg[0].asInt();
		if (!(times == 1 || times == 10))Return(r, -1);
		int randNum[] = { 2, 3, 4, 5, 6 };
		unsigned now = na::time_helper::get_current_time();
		if (startTime <= now&&now <= endTime){
			//物资条件是否满足
			int totalMaterials = female_insect_nest.getMaterialsConsumptionNum()*times;
			totalMaterials = (int)ceil((double)totalMaterials + (double)totalMaterials * activity_sys.getRate(param_interplane_explore_discount, d));
			if (d->playerExplore.getOrdinaryMaterials() < totalMaterials)
				Return(r, 1);
			d->playerExplore.alterOrdinaryMaterials(-totalMaterials);

			//n次连抽
			r[msgStr][0u] = 0;
			r[msgStr][1u][female_insect_nest.getExploreName()] = Json::arrayValue;
			bool flagLottery = false;
			Json::Value fail_json = Json::arrayValue;
			for (int i = 0; i < times; i++){
				int r1 = commom_sys.randomBetween(0, 9);
				d->playerExplore.alterHQMaterials(randNum[r1 / 2]);
				ExploreRewards& rewards = female_insect_nest.startExplore();
				if (NullExploreRewards == rewards)
					Return(r, -1);
				Json::Value temp;
				int result = d->playerExplore.GetAReward(rewards.rewardResource, temp);
				if (result != 0)
					fail_json.append(temp);
				Json::Value& rewardJson = r[msgStr][1u][female_insect_nest.getExploreName()][i];
				female_insect_nest.addPackage(rewardJson, rewards);
				//是否抽到高级奖励？是，发送广播
				if (rewards.ID < 3){
					flagLottery = true;
					Json::Value jStr;
					jStr[playerNameStr] = d->Base.getName();
					jStr["type"] = 0;
					jStr["cn"] = rewards.rewardResource;
					jStr["pid"] = d->playerID;
					chat_sys.sendToAllBroadCastCommon(BROADCAST::interplanetary_broadcast, jStr);
					insertLotteryInfo_fin(jStr);
				}
			}
			if (flagLottery) updateLottery(d, 0);
			updateBaseInfo(d);
			r[msgStr][2u] = fail_json;
			Return(r, 0);
		}
		else{
			Return(r, -1);
		}
	}

	void explore_manager::theAncientGodRemains(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;
		int times = js_msg[0].asInt();
		if (!(times == 1 || times == 10))Return(r, -1);
		unsigned now = na::time_helper::get_current_time();
		if (startTime <= now&&now <= endTime){
			//物资条件是否满足
			int totalMaterials = the_ancient_god_remains.getMaterialsConsumptionNum()*times;
			if (d->playerExplore.getHQMaterials() < totalMaterials)
				Return(r, 1);
			d->playerExplore.alterHQMaterials(-totalMaterials);

			//n次连抽
			r[msgStr][0u] = 0;
			r[msgStr][1u][the_ancient_god_remains.getExploreName()] = Json::arrayValue;
			bool flagLottery = false;
			Json::Value fail_json = Json::arrayValue;
			for (int i = 0; i < times; i++){
				ExploreRewards& rewards = the_ancient_god_remains.startExplore();
				if (NullExploreRewards == rewards)
					Return(r, -1);
				Json::Value temp;
				int result = d->playerExplore.GetAReward(rewards.rewardResource, temp);
				if (result != 0)
					fail_json.append(temp);
				Json::Value& rewardJson = r[msgStr][1u][the_ancient_god_remains.getExploreName()][i];
				the_ancient_god_remains.addPackage(rewardJson, rewards);

				//是否抽到高级奖励？是，发送广播
				
				if (rewards.ID < 3){
					flagLottery = true;
					Json::Value jStr;
					jStr[playerNameStr] = d->Base.getName();
					jStr["type"] = 1;
					jStr["cn"] = rewards.rewardResource;
					jStr["pid"] = d->playerID;
					chat_sys.sendToAllBroadCastCommon(BROADCAST::interplanetary_broadcast, jStr);
					insertLotteryInfo_tag(jStr);
				}
			}
			if (flagLottery) updateLottery(d, 1);
			updateBaseInfo(d);
			r[msgStr][2u] = fail_json;
			Return(r, 0);
		}
		else{
			Return(r, -1);
		}
	}

	void explore_manager::updateBaseInfo()
	{
		playerManager::playerDataVec vec = player_mgr.onlinePlayer();
		for (playerManager::playerDataVec::iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			if (*iter != playerDataPtr())
				updateBaseInfo(*iter);
		}
	}

	void explore_manager::updateBaseInfo(playerDataPtr d){
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		msg[msgStr][1u][startTimeStr] = startTime;
		msg[msgStr][1u][endTimeStr] = endTime;
		msg[msgStr][1u][materialsConsumptionStr_fin] = female_insect_nest.getMaterialsConsumptionNum();
		msg[msgStr][1u][materialsConsumptionStr_tag] = the_ancient_god_remains.getMaterialsConsumptionNum();
		female_insect_nest.addPackage(msg[msgStr][1u][female_insect_nest.getExploreName()]);
		the_ancient_god_remains.addPackage(msg[msgStr][1u][the_ancient_god_remains.getExploreName()]);
		d->playerExplore.addPlayerPackage(msg[msgStr][2u]);
		player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::interplanetary_exploration_update_resp, msg);
	}

	void explore_manager::updateLottery(playerDataPtr d,int type){
		Json::Value msg;
		msg[msgStr][0u] = 0;
		msg[msgStr][1u][updateFromStr] = State::getState();
		if (type == 0)
		{
			packageLotteryInfo_fin(msg[msgStr][1u]["fin"]);		//打包虫族LotteryInfo信息
			player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::female_insect_nest_lottery_resp, msg);
		}
		else{
			packageLotteryInfo_tag(msg[msgStr][1u]["tag"]);		//打包虫族LotteryInfo信息
			player_mgr.sendToPlayer(d->playerID, d->netID, gate_client::the_ancient_god_remains_lottery_resp, msg);
		}
	}

	int explore_manager::setExploreRewards(Json::Value& msg){
		//LogI << msg.toStyledString() << LogEnd;
		unsigned startTimeTemp = 0, endTimeTemp = 0;
		startTimeTemp = msg[0u][0u].asUInt();
		endTimeTemp = msg[0u][1u].asUInt();
		if (startTimeTemp == endTimeTemp&&startTimeTemp == 0){
			startTime = startTimeTemp;
			endTime = endTimeTemp;
			materialsMax = 0;
			female_insect_nest.clearExplorationMsg();
			the_ancient_god_remains.clearExplorationMsg();
			updateBaseInfo();
			return 0;
		}
		//记得要加上时区
		startTimeTemp += na::time_helper::timeZone() * 3600;
		endTimeTemp += na::time_helper::timeZone() * 3600;
		unsigned now = na::time_helper::get_current_time();
		if (startTimeTemp >= endTimeTemp)
			return 1;
		if (startTimeTemp < now&&endTimeTemp <= now)
			return 2;
		//设置每次抽奖消耗数
		int mater1 = msg[1u].asInt(), mater2 = msg[2u].asInt();
		if (mater1 <= 0 || mater2 <= 0)	return 3;
		if (msg[3u].asInt() <= 0)return 3;

		//=======================		普通      ================
		vector<double> probabilityVec_fin;
		ExploreRewardsMap exploreRewardsMap_fin;
		Json::Value& rewardsJson_fin = msg[4u];
		double totalPro = 0.0,pro = 0.0;
		for (unsigned i = 0; i<rewardsJson_fin.size(); i++)
		{
			ExploreRewards rewards;
			rewards.ID = rewardsJson_fin[i]["ID"].asInt();
			rewards.rewardResource = gm_tools_mgr.simple2complex(rewardsJson_fin[i]["re"]);
			pro = rewardsJson_fin[i]["pr"].asDouble();
			if (pro <= 0.0000001)return 3;
			totalPro += pro;
			probabilityVec_fin.push_back(pro);
			exploreRewardsMap_fin[rewards.ID] = rewards;
		}
		if (!(totalPro - 1.0<0.0000001&&totalPro - 1.0>-0.0000001)) return 3;
		double growFin = msg[5u][0u].asDouble();
		int growMFin = msg[5u][1u].asInt();
		if (growFin < 0.0 || growFin>1.0)return 3;
		if (growMFin < 0)return 3;

		//===============   成长校验   ==========
		double dg = 0.0;
		for (int i = 0; i < 3; i++)
			dg += probabilityVec_fin[i];
		double tempD = dg + growMFin*growFin;
		if (tempD - 1.0>0.0000001) return 3;

		//======================      高级     ==================
		vector<double> probabilityVec_tag;
		ExploreRewardsMap exploreRewardsMap_tag;
		Json::Value& rewardsJson_tag = msg[6u];
		totalPro = 0.0, pro = 0.0;
		for (unsigned i = 0; i<rewardsJson_tag.size(); i++)
		{
			ExploreRewards rewards;
			rewards.ID = rewardsJson_tag[i]["ID"].asInt();
			rewards.rewardResource = gm_tools_mgr.simple2complex(rewardsJson_tag[i]["re"]);
			pro = rewardsJson_tag[i]["pr"].asDouble();
			if (pro <= 0.0000001)return 3;
			totalPro += pro;
			probabilityVec_tag.push_back(pro);
			exploreRewardsMap_tag[rewards.ID] = rewards;
		}
		if (!(totalPro - 1.0<0.0000001&&totalPro - 1.0>-0.0000001)) return 3;
		double growTag = msg[7u][0u].asDouble();
		int growMTag = msg[7u][1u].asInt();
		if (growTag < 0.0 || growTag>1.0)return 3;
		if (growMTag < 0)return 3;

		//===============   成长校验   ==========
		dg = 0.0;
		for (int i = 0; i < 3; i++)
			dg += probabilityVec_tag[i];
		tempD = dg + growMTag*growTag;
		if (tempD - 1.0>0.0000001) return 3;


		startTime = startTimeTemp;
		endTime = endTimeTemp;
		materialsMax = msg[3u].asInt();		//最大上限
		//基本数据
		//设置母虫巢穴基本数据
		female_insect_nest.setExploreName("fin");
		female_insect_nest.setHQRange(3);
		female_insect_nest.setMaterialsConsumptionNum(mater1);
		female_insect_nest.setGrowth(growFin);
		female_insect_nest.setGrowthMaxNum(growMFin);
		female_insect_nest.setExploreProbability(probabilityVec_fin);
		female_insect_nest.setExploreRewards(exploreRewardsMap_fin);



		//设置神族遗迹基本数据
		the_ancient_god_remains.setExploreName("tag");
		the_ancient_god_remains.setHQRange(3);
		the_ancient_god_remains.setMaterialsConsumptionNum(mater2);
		the_ancient_god_remains.setExploreProbability(probabilityVec_tag);
		the_ancient_god_remains.setExploreRewards(exploreRewardsMap_tag);
		the_ancient_god_remains.setGrowth(growTag);
		the_ancient_god_remains.setGrowthMaxNum(growMTag);

		updateBaseInfo();
		return 0;
	}

	void explore_manager::getExploreContent(){
		//虫族
		mongo::BSONObj key = BSON(explore_content_key << explore_content_key);
		mongo::BSONObj obj = db_mgr.FindOne(explore_set_content_db_name, key);
		if (!obj.isEmpty()){
			string str = obj[explore_content].String();
			Json::Reader reader;
			Json::Value jattach;
			reader.parse(str, jattach);
			setExploreRewards(jattach);
			//cout << "db_get_content:" << jattach.toStyledString() << endl;
		}
		else{
			startTime = 0;
			endTime = 0;
			materialsMax = 0;
		}
	}
	bool explore_manager::saveExploreContent(Json::Value& setJson){
		mongo::BSONObj key = BSON(explore_content_key << explore_content_key);
		mongo::BSONObjBuilder obj;
		//string str = setJson.toStyledString();
		//mongo::BSONObj b = mongo::fromjson(commom_sys.tighten(str));
		obj << explore_content_key << explore_content_key;
		obj << explore_content << commom_sys.json2string(setJson);
		return db_mgr.save_mongo(explore_set_content_db_name, key, obj.obj());
	}


	int explore_manager::getMaterialsMax()
	{
		return materialsMax;
	}


	int explore_manager::insertLotteryInfo_fin(Json::Value& content){
		if (content == Json::nullValue)
			return -1;
		if (finLotteryList.size() >= LOTTERYSIZE){
			finLotteryList.push_back(content);
			finLotteryList.pop_front();
		}
		else{
			finLotteryList.push_back(content);
		}
		//cout << "列表的长度为：" << lotteryList.size() << endl;
		//cout << "具体内容：" << content.toStyledString() << endl;
		saveLotteryInfo_fin();
		
		return 0;
	}
	int explore_manager::insertLotteryInfo_tag(Json::Value& content){
		if (content == Json::nullValue)
			return -1;
		if (tagLotteryList.size() >= LOTTERYSIZE){
			tagLotteryList.push_back(content);
			tagLotteryList.pop_front();
		}
		else{
			tagLotteryList.push_back(content);
		}
		saveLotteryInfo_tag();

		return 0;
	}

	void explore_manager::packageLotteryInfo_fin(Json::Value& msg){
		msg = Json::arrayValue;
		int i = 0;
		for (list<Json::Value>::iterator it = finLotteryList.begin(); it != finLotteryList.end(); it++, i++){
			msg[i] = Json::arrayValue;
			msg[i] = *it;
		}
	}
	void explore_manager::packageLotteryInfo_tag(Json::Value& msg){
		msg = Json::arrayValue;
		int i = 0;
		for (list<Json::Value>::iterator it = tagLotteryList.begin(); it != tagLotteryList.end(); it++, i++){
			msg[i] = Json::arrayValue;
			msg[i] = *it;
		}
	}

	using namespace mongo;
	void explore_manager::getLotteryInfo(){
		//虫族
		mongo::BSONObj key = BSON(lottery_key << lottery_fin_key);
		mongo::BSONObj obj = db_mgr.FindOne(lottery_info_db_name, key);
		if (!obj.isEmpty()){
			vector<BSONElement> els = obj[lottery_content].Array();	
			for (unsigned i = 0; i < els.size();i++){
				Json::Reader reader;
				Json::Value jattach;
				reader.parse(els[i].Obj().jsonString(), jattach);
				//cout << "#################################" << endl;
				//string str = els[i].Obj().jsonString();
				//commom_sys.tighten(str);
				//cout << str << endl;
				finLotteryList.push_back(jattach);
			}
		}

		//神族
		mongo::BSONObj key1 = BSON(lottery_key << lottery_tag_key);
		mongo::BSONObj obj1 = db_mgr.FindOne(lottery_info_db_name, key1);
		if (!obj1.isEmpty()){
			vector<BSONElement> els = obj1[lottery_content].Array();

			for (unsigned i = 0; i < els.size(); i++){
				Json::Reader reader;
				Json::Value jattach;
				reader.parse(els[i].Obj().jsonString(), jattach);
				tagLotteryList.push_back(jattach);
			}
		}
	}

	bool explore_manager::saveLotteryInfo_fin(){
		mongo::BSONObj key = BSON(lottery_key << lottery_fin_key);
		mongo::BSONObjBuilder obj;
		mongo::BSONArrayBuilder arry;
		for (list<Json::Value>::iterator it = finLotteryList.begin(); it != finLotteryList.end(); it++){
			//string str = (*it).toStyledString();
			mongo::BSONObj b = mongo::fromjson(commom_sys.json2string(*it));
			arry.append(b);
		}
		obj << lottery_key << lottery_fin_key;
		obj << lottery_content << arry.arr();
		return db_mgr.save_mongo(lottery_info_db_name, key, obj.obj());
	}

	bool explore_manager::saveLotteryInfo_tag(){
		mongo::BSONObj key = BSON(lottery_key << lottery_tag_key);
		mongo::BSONObjBuilder obj;
		mongo::BSONArrayBuilder arry;
		for (list<Json::Value>::iterator it = tagLotteryList.begin(); it != tagLotteryList.end(); it++){
			//string str = (*it).toStyledString();
			mongo::BSONObj b = mongo::fromjson(commom_sys.json2string(*it));
			arry.append(b);
		}
		obj << lottery_key << lottery_tag_key;
		obj << lottery_content << arry.arr();
		return db_mgr.save_mongo(lottery_info_db_name, key, obj.obj());
	}
}
