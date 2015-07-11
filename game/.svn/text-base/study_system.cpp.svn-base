#include "study_system.h"
#include "response_def.h"
#include "playerManager.h"
#include "helper.h"
#include "params.hpp"
#include "item_system.h"
#include "file_system.h"
#include "email_system.h"
#include "task_system.h"
#include "chat_system.h"
#include "record_system.h"
#include "activity_game_param.h"
#include "activity_system.h"
using namespace std;

#define ARTIL_OPEN_LEVEL 13
#define SHIELD_OPEN_LEVEL 21
#define PSIONIC_OPEN_LEVEL 32

namespace gg
{
	study_system* const study_system::studySys = new study_system();

	study_system::study_system()
	{
		drawingMap[ARTIL][0] = 14012;
		drawingMap[ARTIL][1] = 14013;
		drawingMap[ARTIL][2] = 14014;
		drawingMap[ARTIL][3] = 14015;

		drawingMap[SHIELD][0] = 14017;
		drawingMap[SHIELD][1] = 14018;
		drawingMap[SHIELD][2] = 14019;
		drawingMap[SHIELD][3] = 14020;
	}

	study_system::~study_system()
	{
	}

	void study_system::initData()
	{
		db_mgr.ensure_index(study::studyDBStr, BSON(playerIDStr << 1));
		initDelegate();
		initPsionic();		
	}

	void study_system::initDelegate()
	{
		map<int, Json::Value> json_dele_map;
		na::file_system::load_jsonfiles_from_dir(study::strDelegateDataDir, json_dele_map);

		map<int, Json::Value>::iterator ite_dele = json_dele_map.begin();
		for (; ite_dele != json_dele_map.end(); ite_dele++)
		{
			if (ite_dele->second["id"] == Json::Value::null)
			{
				continue;
			}

			deleConfig dc;
			dc.id = ite_dele->second["id"].asInt();
			dc.isPermanent = ite_dele->second["isPermanent"].asBool();
			dc.cost = ite_dele->second["cost"].asInt();
			dc.canComeMerchantId = ite_dele->second["canComeMerchantId"].asInt();
			dc.canComeMerchantRate = int((ite_dele->second["canComeMerchantRate"].asDouble()+PRECISION)*RANGE);

			for (unsigned idx = 0; idx < ite_dele->second["canGetEquipmentRawIds"].size(); idx++)
			{
				dc.canGetEquipmentRawIds.push_back(ite_dele->second["canGetEquipmentRawIds"][idx].asInt());
				dc.canGetEquipmentRates.push_back(int((ite_dele->second["canGetEquipmentRates"][idx].asDouble()+PRECISION)*RANGE));
			}

			for (unsigned idx = 1; idx < dc.canGetEquipmentRates.size(); idx++)
			{
				dc.canGetEquipmentRates[idx] += dc.canGetEquipmentRates[idx-1];
			}

			_deleMap[dc.id] = dc;
		}
	}

	void study_system::initPsionic()
	{
		map<int, Json::Value> json_psion_map;
		na::file_system::load_jsonfiles_from_dir(study::strPsionicDataDir, json_psion_map);

		map<int, Json::Value>::iterator ite_psion = json_psion_map.begin();
		int rates[11];
		memset(rates, 0, sizeof(rates));
		for (; ite_psion != json_psion_map.end(); ite_psion++)
		{
			if (ite_psion->second["id"] == Json::Value::null || ite_psion->second["id"].asInt() == 0)
			{
				continue;
			}

			psionicConfig pc;
			pc.id = ite_psion->second["id"].asInt();
			pc.probability = int((ite_psion->second["probability"].asDouble()+PRECISION)*RANGE);
			rates[pc.id] = pc.probability;

			for (unsigned idx = 0; idx < ite_psion->second["event"].size(); idx++)
			{
				eventConfig ecf;
				ecf.event_chance = int((ite_psion->second["event"][idx]["event_chance"].asDouble()+PRECISION)*RANGE);
				ecf.reward_eq = ite_psion->second["event"][idx]["reward_eq"].asInt();
				ecf.reward_jungong = ite_psion->second["event"][idx]["reward_jungong"].asInt();
				pc.eventRates.push_back(ecf);
			}

			for (unsigned idx = 1; idx < pc.eventRates.size(); idx++)
			{
				pc.eventRates[idx].event_chance += pc.eventRates[idx-1].event_chance;
			}

			_psionicMap[pc.id] = pc;
		}

		for (unsigned idx = 1; idx < 11; idx++)
		{
			rates[idx] += rates[idx-1];
		}

		for (psionicMap::iterator ite_psion = _psionicMap.begin(); ite_psion != _psionicMap.end(); ite_psion++)
		{
			ite_psion->second.probability = rates[ite_psion->second.id];
		}
	}

	void study_system::studyUpdateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		d->Study.autoUpdate();
	}

	void study_system::delegateReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int deleType = js_msg[0u].asInt();
		int deleOpt = js_msg[1].asInt();

		if (deleType != 0 && deleType != 1)
			Return(r, 1);
		if (deleOpt < 0 ||  deleOpt > 3)
			Return(r, 2);
		if (!d->Study.isInDeleSet(deleType, deleOpt))
			Return(r, 3);
		delegateStudy& stu = d->Study.getDeleStu(deleType);

		if (deleType == ARTIL)
		{
			if (d->Base.getLevel() < ARTIL_OPEN_LEVEL)
			{
				Return(r, -1);
			}
		}
		else
		{
			if (d->Base.getLevel() < SHIELD_OPEN_LEVEL)
			{
				Return(r, -1);
			}
		}

		int costSilver = _deleMap[TYPEOPT2ID].cost;
		if (stu.todayDelegateTimes < 1)
			costSilver = 0;

		if (d->Base.getSilver() < costSilver)
			Return(r, 4);
		CDData &cd = stu.delegateCD.getCD();
		if (cd.Lock)
		{
			Return(r, 6);
		}

// 		int drawingId = d->Study.getDrawingId(deleType);
// 		if (drawingId > 0)
// 		{
// 			int res = 1;
// 			if(d->Item.addItem(drawingId) != playerItemPtr())
// 			{
// 			}
// 			else
// 			{
// 				Json::Value attachJson;
// 				Json::Value attachItem;
// 				attachItem[email::actionID] = action_add_item;
// 				attachItem[email::addNum] = 1;
// 				attachItem[email::itemID] = drawingId;
// 				attachJson.append(attachItem);
// 
// 				email_sys.sendSystemEmailCommon(d, email_type_system_attachment, email_team_system_bag_full, Json::Value::null, attachJson);
// 			}
// 			d->Study.setDrawingId(deleType, 0);
// 		}

		int drawingId;
		bool isBagFull;
		d->Study.player_delegate(deleType, deleOpt, drawingId, isBagFull, costSilver);
		d->Base.alterSilver(-costSilver);
		Json::Value updateJson;
		d->Study.package(updateJson, deleType);
		d->Study.save();
		activity_sys.updateActivity(d, 0, activity::_study);
		r[msgStr][0] = 0;
		r[msgStr][1] = drawingId;
		r[msgStr][2] = isBagFull;
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::delegate_resp, r);
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);
		r = Json::Value::null;
	}

	void study_system::delegateStoreReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int deleType = js_msg[0u].asInt();
		int drawingId = d->Study.getDrawingId(deleType);
		if (drawingId == 0)
		{
			Return(r, -1);
		}
		if (deleType != 0 && deleType != 1)
			Return(r, -1);
		int res = 1;
		if(d->Item.addItem(drawingId) != playerItemPtr())res = 0;
		if (res == 0)
		{
			d->Study.setDrawingId(deleType, 0);
			Json::Value updateJson;
			d->Study.package(updateJson, deleType);
			d->Study.save();
			player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);
		}

		Return(r, res);
	}

	void study_system::delegateSellReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int deleType = js_msg[0u].asInt();
		if (deleType != 0 && deleType != 1)
			Return(r, -1);
		int drawingId = d->Study.getDrawingId(deleType);
		if (drawingId == 0)
		{
			Return(r, -1);
		}

		Params::ObjectValue ptr = item_sys.getConfig(drawingId / itemOffset);		
		if(ptr.isEmpty())Return(r,-1);
		d->Item.removeItem(drawingId, 1);
		playerItemPtr piPtr = d->Item.getItem(drawingId);
		if (piPtr != playerItemPtr())
		{
			piPtr->Remove(1);
		}
		int salePrice = ptr[ItemDefine::merchandise][ItemDefine::itemSalePriceStr].asInt();
		d->Base.alterSilver(salePrice);

		d->Study.setDrawingId(deleType, 0);
		Json::Value updateJson;
		d->Study.package(updateJson, deleType);
		d->Study.save();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);

		Return(r, 0);
	}

	void study_system::delegateConvertReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int converNum = 200;
		int deleType = js_msg[0u].asInt();
		if (deleType != 0 && deleType != 1)
			Return(r, -1);
		int convertOpt = js_msg[1u].asInt();

		if (convertOpt != 2)
//		if (convertOpt < 0 || convertOpt > 2)
		{
			Return(r, -1);
		}

		delegateStudy& stu = d->Study.getDeleStu(deleType);
		if (stu.convertDrawing[convertOpt] < converNum)
		{
			Return(r, -1);
		}
		
		int res = 1;
		if(d->Item.addItem(drawingMap[deleType][convertOpt+1]) != playerItemPtr())res = 0;
		if (res != 0)
		{
			Return(r, res);
		}
		stu.convertDrawing[convertOpt] -= converNum;
//		stu.convertDrawing[convertOpt+1]++;//?
		Json::Value updateJson;
		d->Study.package(updateJson, deleType);
		d->Study.save();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);

		Return(r, res);
	}

	void study_system::delegateClearCdReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int deleType = js_msg[0u].asInt();
		if (deleType != 0 && deleType != 1)
			Return(r, -1);
		unsigned cur_time = na::time_helper::get_current_time();
		delegateStudy& delestu = d->Study.getDeleStu(deleType);
		delestu.delegateCD.getCD();
		if (cur_time >= delestu.delegateCD.Cdd.CD)
		{
			Return(r, 1);//不需要清CD
		}
		double cd_10_min = (double)(delestu.delegateCD.Cdd.CD - cur_time) / 600.0f;
		int clear_cd_ost = int(1 * ceil(cd_10_min));
		if (clear_cd_ost > d->Base.getAllGold())
		{
			Return(r, 2);//不够金币
		}
		delestu.delegateCD.clearCD();
		d->Base.alterBothGold(-clear_cd_ost);

		Json::Value updateJson;
		d->Study.package(updateJson, deleType);
		d->Study.save();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);

		Return(r, 0);
	}

	void study_system::delegateActivateTheThirdGridReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int deleType = js_msg[0u].asInt();
		if (deleType != 0 && deleType != 1)
			Return(r, -1);
		if (d->Study.isInDeleSet(deleType, 2))
			Return(r, 1);

		int costGold = 30;
		costGold = (int)ceil(costGold*(1+activity_sys.getRate(param_active_cost, d)));
		if (d->Base.getAllGold() < costGold)
			Return(r, 2);

		if (d->Vip.getVipLevel() < 2)
			Return(r, 3);

		delegateStudy& delestu = d->Study.getDeleStu(deleType);
		delestu.delegateSet[2] = 1;
		d->Base.alterBothGold(-costGold);

		Json::Value updateJson;
		d->Study.package(updateJson, deleType);
		d->Study.save();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);

		Return(r, 0);
	}

	void study_system::delegateFreeTimeEndNotifyReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		ReadJsonArray;

		int deleType = js_msg[0u].asInt();
		delegateStudy& delestu = d->Study.getDeleStu(deleType);
		delestu.todayHasPopupUI = true;

		Json::Value updateJson;
		d->Study.package(updateJson, deleType);
		d->Study.save();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);

		Return(r, 0);
	}

	void study_system::psionicResearchReq(msg_json& m, Json::Value& r)
	{
// 		AsyncGetPlayerData(m);
// 		psionicStudy& psStu = d->Study.getPsionicStu();
// 		int status = psStu.studyStatus;
// 		if (status != RESEARCH)
// 		{
// 			Return(r, 1);
// 		}
// 		if (psStu.studyTimes >= MAX_STUDY_TIMES)
// 		{
// 			Return(r, 2);
// 		}
// 
// 		d->Study.setPsionicStatus(REWARD);
// 
// //		psStu.psionicCD = na::time_helper::get_current_time() + psStu.currentStep * 60;
// 		Json::Value updateJson;
// 		d->Study.package(updateJson, PSIONIC);
// 		d->Study.save();
// 		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);
// 
// 		Return(r, 0);

	}

	//立即研究
	void study_system::psionicRewardReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		psionicStudy& psStu = d->Study.getPsionicStu();
		int status = psStu.studyStatus;

// 		if (status != REWARD)
// 		{
// 			Return(r, -1);
// 		}

// 		if (psStu.psionicCD > na::time_helper::get_current_time())
// 		{
// 			Return(r, 2);
// 		}

		if (psStu.studyTimes >= MAX_STUDY_TIMES)
		{
			Return(r, 2);
		}

		if (d->Base.getLevel() < PSIONIC_OPEN_LEVEL)
			Return(r, -1);

		int reward_eq = d->Study.getEventEqReward();
		int reward_jungong = d->Study.getEventJgReward();

		//产生CD
//		psStu.psionicCD = na::time_helper::get_current_time() + psStu.studyTimes * 60;
//		psStu.psionicCD = 0;

		int res = 1;
		bool isBagFull = false;
		int currentStep = psStu.currentStep;

		if (reward_eq != -1)
		{
			if(d->Item.addItem(reward_eq) != playerItemPtr())res = 0;
			if (res != 0)
			{
				Json::Value attachJson;
				Json::Value attachItem;
				attachItem[email::actionID] = action_add_item;
				attachItem[email::addNum] = 1;
				attachItem[email::itemID] = reward_eq;
				attachJson.append(attachItem);

				isBagFull = true;
				email_sys.sendSystemEmailCommon(d, email_type_system_attachment, email_team_system_bag_full_study, Json::Value::null, attachJson);
			}

			studyRewardBroadcast(d, reward_eq, PSIONIC);
		}
		psStu.studyTimes++;
		psStu.totalStudyTimes++;
		//更新最高进度
		if (psStu.currentStep > psStu.currentMaxStep)
		{
			psStu.currentMaxStep = psStu.currentStep;
		}
		task_sys.updateBranchTaskInfo(d, _task_spirit_study_times);

		d->Base.alterKeJi(reward_jungong);
		d->Study.setPsionicStatus(RESEARCH);
		int refresh_step;
		d->Study.refreshPsionic(refresh_step);
//		psStu.currentStep = refresh_step;

		Json::Value updateJson;
		d->Study.package(updateJson, PSIONIC);
		d->Study.save();
		activity_sys.updateActivity(d, 0, activity::_psionic_research);
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);

		{//立即研究记录：玩家id，时间 角色ID 角色名 设计星数 获得科技点 获得装备 原次数 现次数
			int cur_time = na::time_helper::get_current_time();
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(reward_jungong));
			fields.push_back(boost::lexical_cast<string, int>(reward_eq));
			fields.push_back(boost::lexical_cast<string, int>(psStu.studyTimes-1));
			fields.push_back(boost::lexical_cast<string, int>(psStu.studyTimes));
			fields.push_back(boost::lexical_cast<string, int>(currentStep));
			StreamLog::Log(study::mysqlLogStudy, d, 
				boost::lexical_cast<string, int>(cur_time - na::time_helper::timeZone() * 3600), "", fields, study_log_psionic_reward);

		}
		r[msgStr][1] = isBagFull;
		Return(r, 0);
	}

	void study_system::studyRewardBroadcast(playerDataPtr player, int itemID, int opt)
	{
		if (opt != ARTIL && opt != SHIELD && opt != PSIONIC)
			return;

		Params::ObjectValue obj = item_sys.getConfig(itemID);
		Json::Value objJson = Params::ObjectValue::toJson(obj);
		int quality = obj[ItemDefine::itemQualityStr].asInt();
		if (quality < 3 || !objJson[ItemDefine::chip].isNull())
			return;

		int channelID = chat_area;
		if(quality == 5)channelID = chat_all;
		else if(quality == 4)channelID = chat_kingdom;

		Json::Value bCast;
		bCast[msgStr][0u] = 0;
		bCast[msgStr][1u][senderChannelStr] = channelID;
		bCast[msgStr][1u][chatBroadcastID] = BROADCAST::study_reward;
		const static string ParamListStr = "pl";
		bCast[msgStr][1u][ParamListStr].append(player->Base.getName());
		bCast[msgStr][1u][ParamListStr].append(opt+2);
		bCast[msgStr][1u][ParamListStr].append(itemID);
		bCast[msgStr][1u][ParamListStr].append(1);

		if (quality == 3)player_mgr.sendToArea(gate_client::chat_broadcast_resp, bCast, player->Base.getPosition().first);
		else if (quality == 4)player_mgr.sendToSphere(gate_client::chat_broadcast_resp, bCast, player->Base.getSphereID());
		else player_mgr.sendToAll(gate_client::chat_broadcast_resp, bCast);
	}

	//灵能刷新
	void study_system::psionicRefreshReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int refresh_step;
		psionicStudy& psStu = d->Study.getPsionicStu();
		int costGold = 10;

		if (d->Base.getLevel() < PSIONIC_OPEN_LEVEL)
			Return(r, -1);

		if (psStu.studyTimes >= MAX_STUDY_TIMES)
		{
			Return(r, 1);
		}
		if (d->Base.getAllGold() < costGold)
		{
			Return(r, 2);
		}

		d->Study.refreshPsionic(refresh_step);
		d->Base.alterBothGold(-costGold);

		Json::Value updateJson;

		int reward_eq = d->Study.getEventEqReward();
		int reward_jungong = d->Study.getEventJgReward();

//		psStu.currentStep = refresh_step;
		d->Study.package(updateJson, PSIONIC);
		d->Study.save();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);

		{//灵能刷新记录：玩家id、操作类型（单次实验/批量实验）、操作时间、获得物品、消耗信用点、操作前剩余信用点数量、操作后剩余信用点数量。
			int cur_time = na::time_helper::get_current_time();
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(0));//单次操作
			fields.push_back(boost::lexical_cast<string, int>(cur_time - na::time_helper::timeZone() * 3600));
			fields.push_back(boost::lexical_cast<string, int>(reward_jungong));
			fields.push_back(boost::lexical_cast<string, int>(reward_eq));
			fields.push_back(boost::lexical_cast<string, int>(costGold));
			StreamLog::Log(study::mysqlLogStudy, d, boost::lexical_cast<string, int>(d->Base.getAllGold() + costGold), 
				boost::lexical_cast<string, int>(d->Base.getAllGold()), fields, study_log_psionic_refresh);

		}

		Return(r, 0);
	}

	void study_system::psionic10RefreshReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		int refresh_step;
		psionicStudy& psStu = d->Study.getPsionicStu();

		if (d->Base.getAllGold() < 10 * 10)
		{
			Return (r, 1);
		}

		if (psStu.studyTimes >= MAX_STUDY_TIMES)
		{
			Return(r, 2);
		}

		int refresh_times = 0;
		Json::Value refresh_list = Json::arrayValue;
		for (unsigned idx = 0; idx < 10; idx++)
		{
			d->Study.refreshPsionic(refresh_step);
			refresh_list.append(refresh_step);
			refresh_times++;
			if (refresh_step >= psStu.getMaxStepCanRefresh() || refresh_step >= 9)
			{
				break;
			}
		}

		int costGold = 10 * refresh_times;
//		psStu.currentStep = refresh_step;
		d->Base.alterBothGold(0 - costGold);
// 		Json::Value updateJson;
// 		d->Study.package(updateJson, PSIONIC);
// 		d->Study.save();
// 		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);
		r[msgStr][0u] = 0;
		r[msgStr][1u] = refresh_list;
		for (unsigned i = 0; i < 10; ++i)
			activity_sys.updateActivity(d, 0, activity::_psionic_research);

		int reward_eq = d->Study.getEventEqReward();
		int reward_jungong = d->Study.getEventJgReward();

		{//灵能刷新记录：玩家id、操作类型（单次实验/批量实验）、操作时间、获得物品、消耗信用点、操作前剩余信用点数量、操作后剩余信用点数量。
			int cur_time = na::time_helper::get_current_time();
			vector<string> fields;
			fields.push_back(boost::lexical_cast<string, int>(1));//单次操作
			fields.push_back(boost::lexical_cast<string, int>(cur_time - na::time_helper::timeZone() * 3600));
			fields.push_back(boost::lexical_cast<string, int>(reward_jungong));
			fields.push_back(boost::lexical_cast<string, int>(reward_eq));
			fields.push_back(boost::lexical_cast<string, int>(costGold));
			StreamLog::Log(study::mysqlLogStudy, d, boost::lexical_cast<string, int>(d->Base.getAllGold() + costGold), 
				boost::lexical_cast<string, int>(d->Base.getAllGold()), fields, study_log_psionic_refresh);
		}

		helper_mgr.insertSaveAndUpdate(&d->Study);
	}

	void study_system::psionicFinishImmediateReq(msg_json& m, Json::Value& r)
	{
// 		AsyncGetPlayerData(m);
// 		psionicStudy& psStu = d->Study.getPsionicStu();
// 		int status = psStu.studyStatus;
// 
// // 		psionicStudy& ps = d->_study.getPsionicStu();
// // 		if (ps.psionicCD > na::time_helper::get_current_time())
// // 		{
// // 			Return(r, 1);
// // 		}
// 
// 		int cost_gold = 10;
// 		if (d->Base.getGold() < cost_gold)
// 		{
// 			Return(r, 3);
// 		}
// 
// 		if (status != REWARD)
// 		{
// 			Return(r, 2);
// 		}
// 
// 		int reward_eq = d->Study.getEventEqReward();
// 		int reward_jungong = d->Study.getEventJgReward();
// 
// 		//更新最高进度
// 		if (psStu.currentStep > psStu.currentMaxStep)
// 		{
// 			psStu.currentMaxStep = psStu.currentStep;
// 		}
// 
// //		psStu.psionicCD = 0;
// 		psStu.studyTimes++;
// 
// 		int res;
// 		if (reward_eq != -1)
// 		{
// 			int res = 1;
// 			if(d->Ware.addItem(reward_eq) != playerItemPtr())res = 0;
// 			if (res != 0)
// 			{
// 				Return(r, res);
// 			}
// 		}
// 		d->Base.alterKeJi(reward_jungong);
// 		d->Study.setPsionicStatus(RESEARCH);
// 		int refresh_step;
// 		res = d->Study.refreshPsionic(refresh_step);
// //		psStu.currentStep = refresh_step;
// 
// 		Json::Value updateJson;
// 		d->Study.package(updateJson, PSIONIC);
// 		d->Study.save();
// 		d->Base.alterGold(-cost_gold);
// 		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);
// 		Return(r, 0);
	}

	void study_system::psionicCancelResearchReq(msg_json& m, Json::Value& r)
	{
		AsyncGetPlayerData(m);
		psionicStudy& psStu = d->Study.getPsionicStu();

		if (psStu.studyStatus != REWARD)
		{
			Return(r, 1);
		}
// 		if (psStu.psionicCD <= na::time_helper::get_current_time())
// 		{
// 			Return(r, 1);
// 		}

		d->Study.setPsionicStatus(RESEARCH);
//		psStu.psionicCD = 0;
		Json::Value updateJson;
		d->Study.package(updateJson, PSIONIC);
		d->Study.save();
		player_mgr.sendToPlayer(m._player_id, m._net_id, gate_client::study_update_resp, updateJson);
		Return(r, 0);
	}
}
