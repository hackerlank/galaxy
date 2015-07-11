#include "activity_game_param.h"
#include "mongoDB.h"
#include "response_def.h"
#include "space_explore_system.h"
#include "activity_system.h"

namespace gg
{
	act_game_param* const act_game_param::actGameParamMgr = new act_game_param();

	act_game_param::act_game_param()
	{
		action_show_par_backstage_default[activity::_ri_li_wan_ji] = 0.0;	//日理万机
		action_show_par_backstage_default[activity::_jing_bing_qiang_zhen] = 0.2;	//精兵强阵
		action_show_par_backstage_default[activity::_fu_xing_gao_zhao] = 0.0;	//福星高照
		action_show_par_backstage_default[activity::_cai_yuan_guang_jin] = 0.0;	//财源广进
		action_show_par_backstage_default[activity::_tian_dao_chou_qin] = 0.0;	//天道酬勤
		//		action_show_par_default[weizhenhuanyu]=0.3;	//威镇寰宇
		action_show_par_backstage_default[activity::_guo_fu_min_qiang] = 0.0;	//国富民强
		action_show_par_backstage_default[activity::_mine_battle] = 0.5;	//矿源争夺
		action_show_par_backstage_default[activity::_tu_fei_meng_jin] = 0.0;	//突飞猛进

		action_show_par_backstage_default[activity::_fort_war] = 0.2;
		action_show_par_backstage_default[param_vip_buy_junling]=0.0;
		action_show_par_backstage_default[param_force_levy_cost]=0.0;
		action_show_par_backstage_default[param_transform_cost_silver]=0.0;
		action_show_par_backstage_default[param_transform_cost_gold]=0.0;
		action_show_par_backstage_default[param_force_produce_cost]=0.0;
		action_show_par_backstage_default[param_active_cost] = 0.0;
		action_show_par_backstage_default[param_resolve_equip_cost] = 0.0;
		action_show_par_backstage_default[param_crystal_products_cost] = 0.0;
		action_show_par_backstage_default[param_purchase_products_cost] = 0.0;
		action_show_par_backstage_default[param_interplane_explore_discount] = 0;
		action_show_par_backstage_default[param_space_ten_explore_discount] = 0;


//		action_show_par_current = action_show_par_default;
		action_show_par_current = action_show_par_backstage_default;
	}

	act_game_param::~act_game_param()
	{
	}

	void act_game_param::package(Json::Value& update_json)
	{
//		activity_base::package(update_json);
		update_json[game_param_def::str_param_list] = Json::arrayValue;
		for (map<int,double>::iterator it = action_show_par_current.begin(); it != action_show_par_current.end(); it++)
		{
			Json::Value itemJson;
			itemJson.append(it->first);
			itemJson.append(it->second);
			update_json[game_param_def::str_param_list].append(itemJson);
		}
	}

	int act_game_param::modify(Json::Value& modify_json)
	{
		//LogI << "modify: " << modify_json.toStyledString() << LogEnd;
		for (unsigned i = 0; i < modify_json[game_param_def::str_param_list].size(); i++)
		{
			Json::Value &paramItem = modify_json[game_param_def::str_param_list][i];
			int paramID = paramItem[0u].asInt();
			double paramValue = paramItem[1u].asDouble();
			if ((paramID <= param_discount_start || paramID >= param_discount_end) && paramValue < 0
				|| (paramID > param_discount_start && paramID < param_discount_end) && paramValue > 0)
			{
				return 1;
			}
		}

//		action_show_par_current = action_show_par_backstage_default;

		for (unsigned i = 0; i < modify_json[game_param_def::str_param_list].size(); i++)
		{
			Json::Value &paramItem = modify_json[game_param_def::str_param_list][i];
			int paramID = paramItem[0u].asInt();
			double paramValue = paramItem[1u].asDouble();
			if (isActivityParamID(paramID))
			{
				if (paramValue != 0)
				{
					action_show_par_current[paramID] = paramValue;
				}
				else
				{
					action_show_par_current[paramID] = action_show_par_backstage_default[paramID];
				}
			}
		}

		space_explore_sys.setSystemState();
// 		act_time_.start_time = start_time;
// 		act_time_.end_time = end_time;
		
		save();
		return 0;
	}

	void act_game_param::set_to_default()
	{
		activity_base::set_to_default();
	}  

	bool act_game_param::get()
	{
		mongo::BSONObj key = BSON(act_base_def::str_act_key << KEY_ACT_GAME_PARAM);
		mongo::BSONObj obj = db_mgr.FindOne(act_base_def::str_act_db_total, key);
		if(!obj.isEmpty())
		{
//			superGet(obj);
			checkNotEoo(obj[game_param_def::str_param_list])
			{
				vector<mongo::BSONElement> paramList = obj[game_param_def::str_param_list].Array();
				action_show_par_current = action_show_par_backstage_default;
				for (unsigned i = 0; i < paramList.size(); i++)
				{
					mongo::BSONElement paramItem = paramList[i];
//					mongo::BSONArrayBuilder paramItem = paramList[i].Array();
					int paramID = paramItem.Array()[0u].Int();
					double paramValue = paramItem.Array()[1u].Double();
					if (!isActivityParamID(paramID))
						continue;
					action_show_par_current[paramID] = paramValue;
				}
			}
			return true;
		}
		return false;
	}

	bool act_game_param::save()
	{
		mongo::BSONObj key = BSON(act_base_def::str_act_key << KEY_ACT_GAME_PARAM);
		mongo::BSONObjBuilder obj;

//		superSave(obj);
		obj << act_base_def::str_act_key << KEY_ACT_GAME_PARAM;
		mongo::BSONArrayBuilder arrParamList;
		for (map<int,double>::iterator it = action_show_par_current.begin(); it != action_show_par_current.end(); it++)
		{
			int paramID = it->first;
			double paramValue = it->second;
			mongo::BSONArrayBuilder arrParamItem;
			if (isActivityParamID(paramID))
			{
				arrParamItem << paramID;
				arrParamItem << paramValue;
				arrParamList << arrParamItem.arr();
			}
		}
		obj << game_param_def::str_param_list << arrParamList.arr();
		return db_mgr.save_mongo(act_base_def::str_act_db_total, key, obj.obj());
	}

	double act_game_param::get_game_param_by_id(int actionID)
	{
//		refreshData();
// 		if (is_valid())
// 		{
			return action_show_par_current[actionID];
// 		}
// 		else
// 		{
//			return action_show_par_default[actionID];
//		}
// 		if(action_show_sys.getActionState(actionID)==1)
// 			return action_show_par[actionID];
// 		return 0;
	}

	void act_game_param::gmActivityGameParamUpdateReq(msg_json& m, Json::Value& r)
	{
//		refreshData();
		Json::Value update_json;
		package(update_json);
		//LogI << "update: " << update_json.toStyledString() << LogEnd;
		r[msgStr][1] = update_json;
		Return(r, 0);
	}

	void act_game_param::gmActivityGameParamModifyReq(msg_json& m, Json::Value& r)
	{
		ReadJsonArray;
		Json::Value modify_content = js_msg[0u];

		int ret = 0;
		ret = modify(modify_content);
//		cout << "modify_content:" << modify_content.toStyledString();

		if (ret == 0)
		{
			activity_sys.noticeClientUpdateBonusPar();
			// 			Json::Value pack_json, update_json;
			// 			get_game_param_inst().package(pack_json);
			// 			update_json[msgStr][0u] = 0;
			// 			update_json[msgStr][1] = pack_json;
			// 
			// 			player_mgr.sendToAll(gate_client::gm_activity_game_param_update_resp, update_json);
		}
		Return(r, ret);
	}

	bool act_game_param::isActivityParamID( int actionID )
	{
		if (param_discount_start < actionID && actionID < param_discount_end)
			return true;

		if (param_other_start < actionID && actionID < param_other_end)
			return true;

		if (actionID == activity::_fort_war)
			return true;

		if (actionID == activity::_jing_bing_qiang_zhen)
			return false;

		if (activity::_ri_li_wan_ji <= actionID && actionID <= activity::_tu_fei_meng_jin && actionID != activity::_wei_zhen_huan_yu)
			return true;

		if (actionID == activity::_mine_battle)
			return true;

		return false;
	}

	void act_game_param::initData()
	{
		get();
	}

	void act_game_param::refreshData()
	{
//		if (is_overdue())
		{
//			action_show_par_current = action_show_par_default;
		}
	}

	bool act_game_param::is_param_valid( int actionID )
	{
		if (!isActivityParamID(actionID))
			return false;
		if (param_discount_start < actionID && actionID < param_discount_end)
			return true;
		if (action_show_par_current[actionID] != action_show_par_backstage_default[actionID])
			return true;
		return false;
	}

}




