#include "game_handle.h"
#include "playerManager.h"
#include "common_protocol.h"
#include "gate_game_protocol.h"
#include "game_log_protocol.h"
#include "gate_account_protocol.h"
#include "mongoDB.h"
#include "system_response.h"
#include "helper.h"
#include "game_server.h"
#include "playerManager.h"
#include "pilotManager.h"
#include "embattle_sys.h"
#include "war_story.h"
#include "world_system.h"
#include "item_system.h"
#include "chat_system.h"
#include "vip_system.h"
#include "office_system.h"
#include "workshop_system.h"
#include "science_system.h"
#include "junling_system.h"
#include "guild_system.h"
#include "study_system.h"
#include "email_system.h"
#include "shop_system.h"
#include "sign_system.h"
#include "OnlineAwardSys.h"
#include "arena_system.h"
#include "gm_tools.h"
#include "HeroInheritanceSys.h"
#include "ruler_system.h"
#include "strategy_system.h"
#include "task_system.h"
#include "mine_resource_system.h"
#include "activity_rebate.h"
#include "activity_point.h"
#include "guild_battle_system.h"
#include "service_config.h"
#include "explore_manager.h"
#include "reportShare.h"
#include "activity_game_param.h"
#include "ally_system.h"
#include "activity_gift_defined.h"
#include "starwar_system.h"
#include "inspect_tour_system.h"
#include "InviteSystem.h"
#include "space_explore_system.h"
#include "danmu.hpp"
#include "world_boss_system.h"
#include "trade_system.h"
#include "market_system.h"
#include "secretary_system.h"
#include "activity_system.h"
#include "paper_system.h"

using namespace gate_client;
namespace gg
{
#define RegisterFunction(REQ_PROTOCOL, RESP_PROTOCOL, FUNCTION) \
	{\
	f = boost::bind(&FUNCTION, system_response::respSys, _1, _2); \
	reg_func(REQ_PROTOCOL, RESP_PROTOCOL, f);\
}

#define RegisterNoResponFunction(REQ_PROTOCOL, FUNCTION) \
	{\
	f = boost::bind(&FUNCTION, system_response::respSys, _1, _2); \
	reg_func(REQ_PROTOCOL, -1, f);\
}

#define RegisterFunctionPlus(REQ_PROTOCOL, RESP_PROTOCOL, FUNCTION, POINTER) \
	{\
	f = boost::bind(&FUNCTION, POINTER, _1, _2); \
	reg_func(REQ_PROTOCOL, RESP_PROTOCOL, f);\
}

#define RegisterUpdateFunction(STATE_VALUE, FUNCTION, POINTER) \
	{\
		uf = boost::bind(&FUNCTION, POINTER, _1); \
		updateFunction func;\
		func.state_ = STATE_VALUE;\
		func.f_ = uf;\
		func_vector.push_back(func);\
	}

#define RegisterNoResponFunctionPlus(REQ_PROTOCOL, FUNCTION, POINTER) \
	{\
	f = boost::bind(&FUNCTION, POINTER, _1, _2); \
	reg_func(REQ_PROTOCOL, -1, f);\
}


	enum{
	};

	handler::handler()
	{
		showLog = false;

		//update function

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///不要再使用这里作为系统循环
		///使用现成的timmer.hpp
		///通过计时器事件,绑定事件和促发函数
		update_function uf;
		RegisterUpdateFunction(-1, workshopManager::scheduleUpdate, workshopManager::workshopMgr);
		//RegisterUpdateFunction(-1, item_system::itemSystemUpdate, item_system::itemSys);
		RegisterUpdateFunction(-1, ruler_system::checkAndUpdate, ruler_system::rulerSys);
		RegisterUpdateFunction(-1, chat_system::chatUpdate, chat_system::chatSys);
		RegisterUpdateFunction(-1, guild_battle_system::guildBattleUpdate, guild_battle_system::guildBattleSys);
		RegisterUpdateFunction(-1, arena_system::checkAndUpdate, arena_system::arenaSys);
		RegisterUpdateFunction(-1, warStory::warStoryTeamLoopUpdate, warStory::warSys);
		RegisterUpdateFunction(-1, mine_resource_system::mineResLoopUpdate, mine_resource_system::mineResSys);
		RegisterUpdateFunction(-1, starwar_system::checkAndUpdate, starwar_system::starwarSys);
		RegisterUpdateFunction(-1, world_boss_system::worldBossLoopUpdate, world_boss_system::worldBossSys);
		RegisterUpdateFunction(-1, PaperManager::scheduleUpdate, PaperManager::paperSys);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		handler_function f;
		RegisterFunctionPlus(server_info_req, server_info_resp, season::getServerOpenTime, season::seasonSys);
		RegisterFunctionPlus(gate_restart_req, gate_restart_resp, system_response::gateRestart, system_response::respSys);
		//battle//to be continue

		//主城系统
// 		RegisterFunctionPlus(building_update_req, building_update_resp, buildManager::buildingUpdate, buildManager::buildMgr);
// 		RegisterFunctionPlus(building_upgrade_req, building_upgrade_resp, buildManager::buildingUpgrade, buildManager::buildMgr);
// 		RegisterFunctionPlus(building_cd_clear_req, building_cd_clear_resp, buildManager::buildingCDClear, buildManager::buildMgr);

		//武将系统
		RegisterFunctionPlus(pilot_update_req, pilot_update_resp, pilotManager::pilotsUpdate, pilotManager::pilotMgr);
		RegisterFunctionPlus(pilot_update_base_req, pilot_update_base_resp, pilotManager::pilotsUpdateBase, pilotManager::pilotMgr);
		RegisterFunctionPlus(pilot_single_update_req, pilot_single_update_resp, pilotManager::pilotsSingleUpdate, pilotManager::pilotMgr);
		RegisterFunctionPlus(pilot_enlist_req, pilot_enlist_resp, pilotManager::pilotEnlist, pilotManager::pilotMgr);
		RegisterFunctionPlus(pilot_fire_req, pilot_fire_resp, pilotManager::pilotFire, pilotManager::pilotMgr);
		RegisterFunctionPlus(equip_pilot_req, equip_pilot_resp, pilotManager::pilotEquip, pilotManager::pilotMgr);
		RegisterFunctionPlus(off_equip_pilot_req, off_equip_pilot_resp, pilotManager::pilotOffEquip, pilotManager::pilotMgr);
		RegisterFunctionPlus(pilot_one_key_equip_req, pilot_one_key_equip_resp, pilotManager::oneKeyEquip, pilotManager::pilotMgr);

		RegisterFunctionPlus(pilot_upgrade_attribute_req, pilot_upgrade_attribute_resp, pilotManager::upgradeAttribute, pilotManager::pilotMgr);/*洗练属性*/
		RegisterFunctionPlus(pilot_replace_attribute_req, pilot_replace_attribute_resp, pilotManager::replaceAttribute, pilotManager::pilotMgr);
		RegisterFunctionPlus(pilot_revert_attribute_req, pilot_revert_attribute_resp, pilotManager::revertAttribute, pilotManager::pilotMgr);//还原历史最好属性
		RegisterFunctionPlus(pilot_mode_convert_req, pilot_mode_convert_resp, pilotManager::converMode, pilotManager::pilotMgr);
		RegisterFunctionPlus(pilot_cd_clear_req, pilot_cd_clear_resp, pilotManager::useItemCDClear, pilotManager::pilotMgr);
		RegisterFunctionPlus(pilot_upgrade_level_req, pilot_upgrade_level_resp, pilotManager::pilotUpgrade, pilotManager::pilotMgr);
		//布阵系统
		RegisterFunctionPlus(embattle_update_req, embattle_update_resp, embattle_system::embattleUpdate, embattle_system::embattleSys);
		RegisterFunctionPlus(embattle_inFormat_req, embattle_inFormat_resp, embattle_system::embattleInFormat, embattle_system::embattleSys);
		RegisterFunctionPlus(enbattle_set_current_format_req, enbattle_set_current_format_resp, embattle_system::setCurrentFormat, embattle_system::embattleSys);

		//征战系统
		RegisterFunctionPlus(war_story_update_req, war_story_update_resp, warStory::warStoryUpdate, warStory::warSys);
		RegisterFunctionPlus(war_story_battle_with_npc_req, war_story_battle_with_npc_resp, warStory::battleWithNpc, warStory::warSys);
//		RegisterFunctionPlus(war_story_team_update_req, war_story_team_update_resp, warStory::battleWithNpcTeam, warStory::warSys);

		RegisterFunctionPlus(war_story_team_update_req, war_story_team_update_resp, warStory::teamUpdateReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_member_update_req, war_story_team_member_update_resp, warStory::teamMemberUpdateReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_create_req, war_story_team_create_resp, warStory::teamCreateReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_invite_req, war_story_team_invite_resp, warStory::teamInviteReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_change_member_pos_req, war_story_team_change_member_pos_resp, warStory::teamChangeMemberPosReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_remove_member_req, war_story_team_remove_member_resp, warStory::teamRemoveMemberReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_dismiss_req, war_story_team_dismiss_resp, warStory::teamDismissReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_attack_req, war_story_team_attack_resp, warStory::teamAttackReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_join_req, war_story_team_join_resp, warStory::teamJoinReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_quit_req, war_story_team_quit_resp, warStory::teamQuitReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_map_update_req, war_story_team_map_update_resp, warStory::teamMapUpdateReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_manual_update_req, war_story_team_manual_update_resp, warStory::teamManualUpdateReq, warStory::warSys);
		RegisterFunctionPlus(war_story_chaper_reward1_req, war_story_chaper_reward1_resp, warStory::chaper1RewardReq, warStory::warSys);
		RegisterFunctionPlus(war_story_chaper_reward2_req, war_story_chaper_reward2_resp, warStory::chaper2RewardReq, warStory::warSys);
		RegisterFunctionPlus(war_story_chaper_reward3_req, war_story_chaper_reward3_resp, warStory::chaper3RewardReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_quit_interface_req, war_story_team_quit_interface_resp, warStory::teamQuitInterfaceReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_bonus_update_req, war_story_team_bonus_update_resp, warStory::teamBonusUpdateReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_leader_alive_req, war_story_team_leader_alive_resp, warStory::teamLeaderAliveReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_npc_help_update_req, war_story_team_npc_help_update_resp, warStory::teamNpcHelpUpdateReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_npc_help_req, war_story_team_npc_help_resp, warStory::teamNpcHelpReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_add_npc_from_player_req, war_story_team_add_npc_from_player_resp, warStory::teamAddNpcFromPlayerReq, warStory::warSys);
		RegisterFunctionPlus(war_story_team_sweep_req, war_story_team_sweep_resp, warStory::teamSweepReq, warStory::warSys);

		//世界系统
		RegisterFunctionPlus(world_get_total_page_req, world_get_total_page_resp, world::updateMapPage, world::worldSys);
		RegisterFunctionPlus(world_update_part_req, world_update_part_resp, world::updateClient, world::worldSys);
		RegisterFunctionPlus(world_change_planet_req, world_change_planet_resp, world::changePlanet, world::worldSys);
		RegisterFunctionPlus(world_area_motify_flag_name_req, world_area_motify_flag_name_resp, world::motifyFlagName, world::worldSys);
		RegisterFunctionPlus(world_area_motify_leave_message_req, world_area_motify_leave_message_resp, world::motifyLeaveMessage, world::worldSys);
//		RegisterFunctionPlus(world_area_invested_myself_req, world_area_invested_myself_resp, world::investedMyself, world::worldSys);
		RegisterFunctionPlus(world_area_battle_req, world_area_battle_resp, world::battleAreaPlayer, world::worldSys);

		//物品系统
		RegisterFunctionPlus(player_item_update_req, player_item_update_resp, item_system::updatePlayerItems, item_system::itemSys);
		RegisterFunctionPlus(buy_item_req, buy_item_resp, item_system::buyItem, item_system::itemSys);
		RegisterFunctionPlus(sale_item_req, sale_item_resp, item_system::saleItem, item_system::itemSys);
//		RegisterFunctionPlus(batch_sale_item_req, batch_sale_item_resp, item_system::updatePlayerItems, item_sys);//批量卖 还没有做
		RegisterFunctionPlus(upgrade_equipmen_req, upgrade_equipmen_resp, item_system::upgradeEquip, item_system::itemSys);
		RegisterFunctionPlus(degrade_equipmen_req, degrade_equipmen_resp, item_system::degradeEquipReq, item_system::itemSys);
		RegisterFunctionPlus(item_system_info_update_req, item_system_info_update_resp, item_system::updateItemSystemInfo, item_system::itemSys);
		//RegisterFunctionPlus(item_equip_succinct_req, item_equip_succinct_resp, item_system::equipSuccinct, item_system::itemSys);
		RegisterFunctionPlus(item_equip_revolution_req, item_equip_revolution_resp, item_system::equipRevolution, item_system::itemSys);
		RegisterFunctionPlus(buy_bag_capacity_req, buy_bag_capacity_resp, item_system::buyCapacity, item_system::itemSys);
		RegisterFunctionPlus(player_item_use_req, player_item_use_resp, item_system::useItem, item_system::itemSys);
		RegisterFunctionPlus(item_compose_req, item_compose_resp, item_system::composeItem, item_system::itemSys);
		RegisterFunctionPlus(eq_deputy_properties_generate_req,eq_deputy_properties_generate_resp,item_system::ordinaryTransformEquip,item_system::itemSys);	//普通洗练
		RegisterFunctionPlus(eq_directional_deputy_properties_generate_req,eq_directional_deputy_properties_generate_resp,item_system::directionalTransformEquip,item_system::itemSys);
		RegisterFunctionPlus(eq_deputy_properites_replace_req,eq_deputy_properites_replace_resp,item_system::replaceEqAttribut,item_system::itemSys);
		RegisterFunctionPlus(eq_deputy_properites_keep_req,eq_deputy_properites_keep_resp,item_system::keepEqAttribut,item_system::itemSys);
		RegisterFunctionPlus(upgrade_equip_cd_clear_req, upgrade_equip_cd_clear_resp, item_system::clearUpgradeCD, item_system::itemSys); 
		RegisterFunctionPlus(item_disintegrate_req, item_disintegrate_resp, item_system::DisintegrationReq, item_system::itemSys);
		RegisterFunctionPlus(update_crystal_exchange_req, update_crystal_exchange_resp, item_system::updateExchangeItemsInformation, item_system::itemSys);			//水晶兑换物品信息
		RegisterFunctionPlus(exchange_crystal_req, exchange_crystal_resp, item_system::exchangeItems, item_system::itemSys);
		RegisterFunctionPlus(embedded_equipment_req, embedded_equipment_resp, item_system::Embedded, item_system::itemSys);
		RegisterFunctionPlus(crytal_takeoff_equipment_req, crytal_takeoff_equipment_resp, item_system::crytalTakeOff, item_system::itemSys);
		//主角系统
		RegisterFunction(player_role_update_req, player_role_update_resp, system_response::roleUpdateClient);
		RegisterFunction(player_login_req, player_login_resp, system_response::playerLogin);
		RegisterFunction(player_notic_account_name_req, player_notic_account_name_resp, system_response::syncAccount);
		RegisterFunction(player_generate_role_req, player_generate_role_resp, system_response::createRole);
//		RegisterFunction(player_get_CD_List_req, player_get_CD_List_resp, system_response::updateCDList);
//		RegisterFunction(protocol::c2l::charge_gold_req, protocol::l2c::charge_gold_resp, system_response::chargeGold);
		RegisterFunction(player_levy_req, player_levy_resp, system_response::levy);
		RegisterFunction(player_levy_update_req, player_levy_update_resp, system_response::updateLevy);
		RegisterFunction(player_levy_cd_clear_req, player_levy_cd_clear_resp, system_response::levyCDClear);
		RegisterFunction(player_set_face_id_req, player_set_face_id_resp, system_response::setFaceIdReq);
		RegisterFunction(player_change_name_req, player_change_name_resp, system_response::changeName);
		RegisterFunction(player_client_process_req, player_client_process_resp, system_response::playerProcess);
		RegisterFunction(player_invest_req, player_invest_resp, system_response::invest);
		RegisterFunction(player_join_kingdom_req, player_join_kingdom_resp, system_response::joinKingdom);
		RegisterFunction(player_join_min_kingdom_req, player_join_min_kingdom_resp, system_response::joinMinKingdom);
		RegisterFunction(player_set_gender_req, player_set_gender_resp, system_response::setGender);

		//system callback
		RegisterNoResponFunction(player_guild_notice_req, system_response::noticePlayerGuild);
		RegisterNoResponFunction(player_guild_upgrade_to_leader_notice_req, system_response::upgradeToLeaderNoticeReq);

		//chat system
		RegisterFunctionPlus(chat_voice_req, chat_voice_resp, chat_system::playerChatVoice, chat_system::chatSys);
		RegisterFunctionPlus(chat_send_to_part_req, chat_send_to_part_resp, chat_system::playerChat, chat_system::chatSys);
		RegisterFunctionPlus(gm_forbid_speaker_req, gm_forbid_speaker_resp, chat_system::gmForbidSpeakerReq, chat_system::chatSys);
		RegisterFunctionPlus(gm_allow_speaker_req, gm_allow_speaker_resp, chat_system::gmAllowSpeakerReq, chat_system::chatSys);
		RegisterFunctionPlus(gm_forbid_speaker_update_req, gm_forbid_speaker_update_resp, chat_system::gmForbidSpeakerUpdateReq, chat_system::chatSys);
		RegisterFunctionPlus(gm_roll_broadcast_update_req, gm_roll_broadcast_update_resp, chat_system::gmRollBroadcastUpdateReq, chat_system::chatSys);
		RegisterFunctionPlus(gm_roll_broadcast_set_req, gm_roll_broadcast_set_resp, chat_system::gmRollBroadcastSetReq, chat_system::chatSys);
		RegisterFunctionPlus(gm_roll_broadcast_del_req, gm_roll_broadcast_del_resp, chat_system::gmRollBroadcastDelReq, chat_system::chatSys);
		RegisterFunctionPlus(chat_table_update_req, chat_table_update_resp, chat_system::chatTableUpdate, chat_system::chatSys);
		RegisterFunctionPlus(chat_show_req, chat_show_resp, chat_system::ChatShow, chat_system::chatSys);
		RegisterFunctionPlus(chat_danmu_req, chat_danmu_resp, barrage::DanmuReq, barrage::barrageSys);
		RegisterFunctionPlus(chat_danmu_listen_req, chat_danmu_listen_resp, barrage::ListenerTick, barrage::barrageSys);

		//vip system
		RegisterFunctionPlus(vip_update_req, vip_update_resp, vip_system::vipUpdateReq, vip_system::vipSys);
		RegisterFunctionPlus(vip_pickup_vip_gift_req, vip_pickup_vip_gift_resp, vip_system::pickUpVipGift, vip_system::vipSys);
		RegisterFunctionPlus(vip_pickup_first_gift_req, vip_pickup_first_gift_resp, vip_system::pickUpFirstGift, vip_system::vipSys);
		RegisterFunctionPlus(gm_charge_gold_req, gm_charge_gold_resp, vip_system::chargeGold, vip_system::vipSys);
		//RegisterFunctionPlus(gm_buy_month_card_req, gm_buy_month_card_resp, vip_system::buyMonthCard, vip_system::vipSys);
		RegisterFunctionPlus(vip_month_card_reward_req, vip_month_card_reward_resp, vip_system::monthCardReward, vip_system::vipSys);

		//office system
		RegisterFunctionPlus(office_update_req, office_update_resp, office_system::officeUpdateReq, office_system::officeSys);
		RegisterFunctionPlus(office_receive_salary_req, office_receive_salary_resp, office_system::officeReceiveSalaryReq, office_system::officeSys);
//		RegisterFunctionPlus(office_promote_req, office_promote_resp, office_system::officePromoteReq, office_system::officeSys);

		//生产系统
		RegisterFunctionPlus(workshop_update_req, workshop_update_resp, workshopManager::update, workshopManager::workshopMgr);
		RegisterFunctionPlus(workshop_yield_req, workshop_yield_resp, workshopManager::yield, workshopManager::workshopMgr);
		RegisterFunctionPlus(workshop_sale_req, workshop_sale_resp, workshopManager::sales, workshopManager::workshopMgr);
		RegisterFunctionPlus(workshop_auto_req, workshop_auto_resp, workshopManager::autoSaleReq, workshopManager::workshopMgr);

		//科技系统
		RegisterFunctionPlus(science_update_req, science_update_resp, ScienceManager::reqUpdate, ScienceManager::scienceMgr);
		RegisterFunctionPlus(science_upgreade_req, science_upgrade_resp, ScienceManager::reqUpgrade, ScienceManager::scienceMgr);


		//军令系统
		RegisterFunctionPlus(junling_update_req, junling_update_resp, junlingSystem::update, junlingSystem::junlingSys);
		RegisterFunctionPlus(junling_clear_cd_req, junling_clear_cd_resp, junlingSystem::clearCD, junlingSystem::junlingSys);
		RegisterFunctionPlus(junling_buy_req, junling_buy_resp, junlingSystem::BuyJunling, junlingSystem::junlingSys);

		//星盟系统
		RegisterFunctionPlus(guild_create_group_req, guild_create_group_resp, guild_system::createGuild, guild_system::guildSys);
//		RegisterFunctionPlus(guild_dismiss_group_req, guild_dismiss_group_resp, guild_system::dismissGuild, guild_system::guildSys);
		RegisterFunctionPlus(guild_join_group_req, guild_join_group_resp, guild_system::joinGuild, guild_system::guildSys);
		RegisterFunctionPlus(guild_leave_group_req, guild_leave_group_resp, guild_system::leaveGuild, guild_system::guildSys);
		RegisterFunctionPlus(guild_member_up_offcial_req, guild_member_up_offcial_resp, guild_system::memberUpgrade, guild_system::guildSys);
		RegisterFunctionPlus(guild_kick_member_req, guild_kick_member_resp, guild_system::kickMember, guild_system::guildSys);
		RegisterFunctionPlus(guild_motify_words_req, guild_motify_words_resp, guild_system::motifyWords, guild_system::guildSys);
		RegisterFunctionPlus(guild_donate_group_req, guild_donate_group_resp, guild_system::donateGuild, guild_system::guildSys);
//		RegisterFunctionPlus(guild_buy_group_flag_req, guild_buy_group_flag_resp, guild_system::upgradeGuildFlag, guild_system::guildSys);
		RegisterFunctionPlus(guild_response_join_member_req, guild_response_join_member_resp, guild_system::responseJoin, guild_system::guildSys);
		RegisterFunctionPlus(guild_update_group_list_req, guild_update_group_list_resp, guild_system::updateGuildList, guild_system::guildSys);
		RegisterFunctionPlus(guild_update_group_member_req, guild_update_group_member_resp, guild_system::updateGuildMember, guild_system::guildSys);
		RegisterFunctionPlus(guild_update_join_member_req, guild_update_join_member_resp, guild_system::updateGulildJoinMemBer, guild_system::guildSys);
		RegisterFunctionPlus(guild_update_history_req, guild_update_history_resp, guild_system::updateGuildHistory, guild_system::guildSys);
		RegisterFunctionPlus(guild_set_limit_req, guild_set_limit_resp, guild_system::setJoinLimit, guild_system::guildSys);
		RegisterFunctionPlus(guild_transfer_leader_req, guild_transfer_leader_resp, guild_system::abdication, guild_system::guildSys);
		RegisterFunctionPlus(guild_cancel_join_req, guild_cancel_join_resp, guild_system::cancelJoin, guild_system::guildSys);
		RegisterFunctionPlus(guild_update_join_list_req, guild_update_join_list_resp, guild_system::updateJoinGuild, guild_system::guildSys);
		RegisterFunctionPlus(guild_update_base_req, guild_update_base_resp, guild_system::updateGuildBase, guild_system::guildSys);
		RegisterFunctionPlus(guild_update_science_req, guild_update_science_resp, guild_system::updateGuildScience, guild_system::guildSys);
		RegisterFunctionPlus(guild_set_scient_first_req, guild_set_scient_first_resp, guild_system::memScientFirst, guild_system::guildSys);
		RegisterFunctionPlus(guild_change_flag_icon_req, guild_change_flag_icon_resp, guild_system::changeFlagIcon, guild_system::guildSys);
		RegisterFunctionPlus(guild_help_update_req, guild_help_update_resp, guild_system::updateGuildHelp, guild_system::guildSys);
		RegisterFunctionPlus(guild_request_help_req, guild_request_help_resp, guild_system::guildHelpReq, guild_system::guildSys);
		RegisterFunctionPlus(guild_respon_help_req, guild_respon_help_resp, guild_system::tickHelpReq, guild_system::guildSys);
		RegisterFunctionPlus(guild_trade_skill_upgrade_req, guild_trade_skill_upgrade_resp, guild_system::upgradeTradeSkillReq, guild_system::guildSys);
		RegisterFunctionPlus(guild_trade_skill_update_req, guild_trade_skill_update_resp, trade_system::tradeUpdateSkill, trade_system::trade_pointer);

		//研究系统 
		RegisterFunctionPlus(study_update_req, study_update_resp, study_system::studyUpdateReq, study_system::studySys);
		RegisterFunctionPlus(delegate_req, delegate_resp, study_system::delegateReq, study_system::studySys);
// 		RegisterFunctionPlus(delegate_store_req, delegate_store_resp, study_system::delegateStoreReq, study_system::studySys);
// 		RegisterFunctionPlus(delegate_sell_req, delegate_sell_resp, study_system::delegateSellReq, study_system::studySys);
//		RegisterFunctionPlus(delegate_convert_req, delegate_convert_resp, study_system::delegateConvertReq, study_system::studySys);
		RegisterFunctionPlus(delegate_clear_cd_req, delegate_clear_cd_resp, study_system::delegateClearCdReq, study_system::studySys);
//		RegisterFunctionPlus(psionic_research_req, psionic_research_resp, study_system::psionicResearchReq, study_system::studySys);
//		RegisterFunctionPlus(psionic_reward_req, psionic_reward_resp, study_system::psionicRewardReq, study_system::studySys);//立即研究
//		RegisterFunctionPlus(psionic_refresh_req, psionic_refresh_resp, study_system::psionicRefreshReq, study_system::studySys);//灵能刷新
//		RegisterFunctionPlus(psionic_finish_immediate_req, psionic_finish_immediate_resp, study_system::psionicFinishImmediateReq, study_system::studySys);
//		RegisterFunctionPlus(psionic_10_refresh_req, psionic_10_refresh_resp, study_system::psionic10RefreshReq, study_system::studySys);
//		RegisterFunctionPlus(psionic_cancel_research_req, psionic_cancel_research_resp, study_system::psionicCancelResearchReq, study_system::studySys);
		RegisterFunctionPlus(delegate_activate_the_third_grid_req, delegate_activate_the_third_grid_resp, study_system::delegateActivateTheThirdGridReq, study_system::studySys);
		RegisterFunctionPlus(delegate_free_time_end_notify_req, delegate_free_time_end_notify_resp, study_system::delegateFreeTimeEndNotifyReq, study_system::studySys);

		//邮件系统
		RegisterFunctionPlus(email_update_req, email_update_resp, email_system::mailUpdateReq, email_system::emailSys);
		RegisterFunctionPlus(email_write_to_player_req, email_write_to_player_resp, email_system::writeToPlayerReq, email_system::emailSys);
		RegisterFunctionPlus(email_save_req, email_save_resp, email_system::saveMailReq, email_system::emailSys);
		RegisterFunctionPlus(email_get_gift_req, email_get_gift_resp, email_system::getGiftReq, email_system::emailSys);
		RegisterFunctionPlus(email_del_req, email_del_resp, email_system::delListReq, email_system::emailSys);
		RegisterFunctionPlus(email_del_savelist_req, email_del_savelist_resp, email_system::delSavelistReq, email_system::emailSys);
		RegisterFunctionPlus(email_write_to_all_guild_member_req, email_write_to_all_guild_member_resp, email_system::writeToAllGuildMemberReq, email_system::emailSys);

		//商城系统
		RegisterFunctionPlus(shop_update_req, shop_update_resp, shop_system::shopUpdateReq, shop_system::shopSys);
		RegisterFunctionPlus(shop_deal_req, shop_deal_resp, shop_system::shopDealReq, shop_system::shopSys);
		RegisterFunctionPlus(shop_info_req, shop_info_resp, shop_system::shopInfoReq, shop_system::shopSys);
		RegisterFunctionPlus(shop_buy_month_card_req, shop_buy_month_card_resp, shop_system::buyMonthCardReq, shop_system::shopSys);
		RegisterFunctionPlus(gm_shop_info_req, gm_shop_info_resp, shop_system::gmShopInfoReq, shop_system::shopSys);
		RegisterFunctionPlus(gm_shop_modify_req, gm_shop_modify_resp, shop_system::gmShopModifyReq, shop_system::shopSys);

		//签到系统
		RegisterFunctionPlus(sign_update_req, sign_update_resp, sign_system::signUpdateReq, sign_system::signSys);
		RegisterFunctionPlus(sign_reward_req, sign_reward_resp, sign_system::signRewardReq, sign_system::signSys);

		//在线累积奖励领取系统
		RegisterFunctionPlus(online_award_reset_rep,online_award_reset_resp,OnlineAwardSys::ResetAwardMsg,OnlineAwardSys::onlineAwardMgr);
		RegisterFunctionPlus(online_award_get_rewards_rep,online_award_get_rewards_resp,OnlineAwardSys::GetAward,OnlineAwardSys::onlineAwardMgr);


		//竞技场系统
		RegisterFunctionPlus(arena_main_update_req, arena_main_update_resp, arena_system::arenaMainUpdateReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_hero_rank_new_update_req, arena_hero_rank_new_update_resp, arena_system::arenaHeroRankNewUpdateReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_hero_rank_old_update_req, arena_hero_rank_old_update_resp, arena_system::arenaHeroRankOldUpdateReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_famous_person_rank_update_req, arena_famous_person_rank_update_resp, arena_system::arenaFamousPersonRankUpdateReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_challege_req, arena_challege_resp, arena_system::arenaChallegeReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_get_reward_req, arena_get_reward_resp, arena_system::arenaGetRewardReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_get_year_reward_req, arena_get_year_reward_resp, arena_system::arenaGetYearRewardReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_clear_cd_req, arena_clear_cd_resp, arena_system::arenaClearCdReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_buy_challege_times_req, arena_buy_challege_times_resp, arena_system::arenaBuyChallegeTimes, arena_system::arenaSys);
		RegisterFunctionPlus(arena_new_lucky_ranking_list_req, arena_new_lucky_ranking_list_resp, arena_system::arenaNewLuckyRankListReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_old_lucky_ranking_list_req, arena_old_lucky_ranking_list_resp, arena_system::arenaOldLuckyRankListReq, arena_system::arenaSys);
		RegisterFunctionPlus(arena_get_lucky_reward_req, arena_get_lucky_reward_resp, arena_system::arenaGetLuckyRewardReq, arena_system::arenaSys);

		RegisterFunctionPlus(arena_summary_broadcast_inner_req, arena_summary_broadcast_inner_resp, arena_system::arenaSummaryBroadcastInnerReq, arena_system::arenaSys);
		
		//世界buff
//		RegisterFunctionPlus(global_buff_update_req, global_buff_update_resp, buff_system::GlobalBuffUpdate, buff_system::buffSys);

		//gm工具
		RegisterFunctionPlus(gm_player_base_info_update_req, gm_player_base_info_update_resp, gm_tools::playerBaseInfoUpdateReq, gm_tools::gmMgr);
		RegisterFunctionPlus(gm_modify_player_base_info_req, gm_modify_player_base_info_resp, gm_tools::modifyPlayerBaseInfoReq, gm_tools::gmMgr);
		RegisterFunctionPlus(gm_send_resource_to_player_req, gm_send_resource_to_player_resp, gm_tools::sendResourceToPlayerReq, gm_tools::gmMgr);
		RegisterFunctionPlus(gm_send_email_to_player_req, gm_send_email_to_player_resp, gm_tools::gmSendEmailToPlayerReq, gm_tools::gmMgr);
		RegisterFunctionPlus(gm_send_gift_card_to_player_req, gm_send_gift_card_to_player_resp, gm_tools::gmSendGiftCardToPlayerReq, gm_tools::gmMgr);
		RegisterFunctionPlus(gm_custom_service_reply_req, gm_custom_service_reply_resp, gm_tools::gmCustomServiceReplyReq, gm_tools::gmMgr);
		RegisterFunctionPlus(gm_pilots_list_req, gm_pilots_list_resp, pilotManager::gmPilotsListReq, pilotManager::pilotMgr);
		RegisterFunctionPlus(gm_single_pilot_info_req, gm_single_pilot_info_resp, pilotManager::gmSinglePilotInfoReq, pilotManager::pilotMgr);
		RegisterFunctionPlus(gm_update_server_time_req, gm_update_server_time_resp, system_response::serverTimeGet, system_response::respSys);
		RegisterFunctionPlus(gm_add_server_time_req, gm_add_server_time_resp, system_response::serverTimeSet, system_response::respSys);
		RegisterFunctionPlus(gm_motify_player_name_req, gm_motify_player_name_resp, system_response::gmMotifyName, system_response::respSys);
		RegisterFunctionPlus(gm_motify_war_process_req, gm_motify_war_process_resp, system_response::gmAddWarStroy, system_response::respSys);
		RegisterFunctionPlus(gm_motify_guild_name_req, gm_motify_guild_name_resp, guild_system::changeGName, guild_system::guildSys); 
		RegisterFunctionPlus(gm_motify_player_kingdom_req, gm_motify_player_kingdom_resp, system_response::gmMotifyKingdom, system_response::respSys);

		RegisterFunctionPlus(gm_add_pilot_exp_req, gm_add_pilot_exp_resp, pilotManager::gmAddPilotExpReq, pilotManager::pilotMgr);
		RegisterFunctionPlus(gm_set_pilot_extra_attribute_req, gm_set_pilot_extra_attribute_resp, pilotManager::gmSetPilotExtraAttributeReq, pilotManager::pilotMgr);
		RegisterFunctionPlus(gm_set_equip_level_req, gm_set_equip_level_resp, item_system::gmSetEquipLevelReq, item_system::itemSys);
		RegisterFunctionPlus(gm_set_equip_add_attribute_req, gm_set_equip_add_attribute_resp, item_system::gmSetEquipAddAttributeReq, item_system::itemSys);
		RegisterFunctionPlus(gm_player_item_update_req, gm_player_item_update_resp, item_system::gmPlayerItemUpdateReq, item_system::itemSys);
		RegisterFunctionPlus(gm_add_guild_science_exp_req, gm_add_guild_science_exp_resp, guild_system::gmAddGuildScienceExpReq, guild_system::guildSys);
		RegisterFunctionPlus(gm_update_guild_base_req, gm_update_guild_base_resp, guild_system::gmUpdateGuildBaseReq, guild_system::guildSys);
		RegisterFunctionPlus(gm_update_guild_group_member_req, gm_update_guild_group_member_resp, guild_system::gmUpdateGuildMember, guild_system::guildSys);

		RegisterFunctionPlus(gm_activity_game_param_update_req, gm_activity_game_param_update_resp, act_game_param::gmActivityGameParamUpdateReq, act_game_param::actGameParamMgr);
		RegisterFunctionPlus(gm_activity_game_param_modify_req, gm_activity_game_param_modify_resp, act_game_param::gmActivityGameParamModifyReq, act_game_param::actGameParamMgr);

		RegisterFunctionPlus(gm_activity_rebate_modify_req, gm_activity_rebate_modify_resp, act_rebate::gmActivityRebateModifyReq, act_rebate::rebateSys);
		RegisterFunctionPlus(gm_activity_point_modify_req, gm_activity_point_modify_resp, act_point::gmActivityPointModifyReq, act_point::pointSys);

		RegisterFunctionPlus(gm_point_update_req, gm_point_update_resp, act_point::pointUpdateReq, act_point::pointSys);
		RegisterFunctionPlus(gm_rebate_update_req, gm_rebate_update_resp, act_rebate::rebateUpdateReq, act_rebate::rebateSys);

		RegisterFunctionPlus(gm_activity_gift_defined_modify_req, gm_activity_gift_defined_modify_resp, act_gift_defined::gmActivityGiftDefinedModifyReq, act_gift_defined::giftDefinedSys);

		//ruler
		RegisterFunctionPlus(ruler_update_req, ruler_update_resp, ruler_system::rulerUpdateReq, ruler_system::rulerSys);
		RegisterFunctionPlus(ruler_term_info_req, ruler_term_info_resp, ruler_system::rulerTermInfoReq, ruler_system::rulerSys);
		RegisterFunctionPlus(ruler_challenge_req, ruler_challenge_resp, ruler_system::rulerChallengeReq, ruler_system::rulerSys);
		RegisterFunctionPlus(ruler_title_info_req, ruler_title_info_resp, ruler_system::rulerTitleInfoReq, ruler_system::rulerSys);
		RegisterFunctionPlus(ruler_set_title_info_req, ruler_set_title_info_resp, ruler_system::rulerSetTitleReq, ruler_system::rulerSys);
		RegisterFunctionPlus(ruler_betting_req, ruler_betting_resp, ruler_system::rulerBettingReq, ruler_system::rulerSys);
		RegisterFunctionPlus(ruler_get_reward_req, ruler_get_reward_resp, ruler_system::rulerGetRewardReq, ruler_system::rulerSys);
		RegisterFunctionPlus(ruler_clear_cd_req, ruler_clear_cd_resp, ruler_system::rulerClearCdReq, ruler_system::rulerSys);

		RegisterFunctionPlus(ruler_auto_set_title_info_inner_req, ruler_auto_set_title_info_inner_resp, ruler_system::rulerAutoSetTitleInnerReq, ruler_system::rulerSys);
		RegisterFunctionPlus(ruler_final_battle_inner_req, ruler_final_battle_inner_resp, ruler_system::rulerFinalBattleInnerReq, ruler_system::rulerSys);
		RegisterFunctionPlus(ruler_broadcast_inner_req, ruler_broadcast_inner_resp, ruler_system::rulerBroadcastInnerReq, ruler_system::rulerSys);

		//player active sys
		RegisterFunctionPlus(activity_info_req, activity_info_resp, activity_system::infoReq, activity_system::activitySys);
		RegisterFunctionPlus(activity_time_limited_update_req, activity_time_limited_update_resp, activity_system::timeLimitedUpdateReq, activity_system::activitySys);
		RegisterFunctionPlus(activity_get_reward_req, activity_get_reward_resp, activity_system::getRewardReq, activity_system::activitySys);

		//传承系统
		//RegisterFunctionPlus(hero_inherit_preview_req,hero_inherit_preview_resp,HeroInheritanceSys::inheritingThePreview,HeroInheritanceSys::playerInheritMgr);
		RegisterFunctionPlus(hero_inherit_start_inherit_req,hero_inherit_start_inherit_resp,HeroInheritanceSys::startInheritance,HeroInheritanceSys::playerInheritMgr);

		// 攻略
		RegisterFunctionPlus(strategy_update_req,strategy_update_resp,strategy_system::strategyUpdateReq,strategy_system::strategySys);

		//矿源争夺
		RegisterFunctionPlus(mine_res_update_req,mine_res_update_resp,mine_resource_system::mineResUpdateReq,mine_resource_system::mineResSys);
		RegisterFunctionPlus(mine_res_turn_page_req,mine_res_turn_page_resp,mine_resource_system::mineResTurnPageReq,mine_resource_system::mineResSys);
		RegisterFunctionPlus(mine_res_fight_record_req,mine_res_fight_record_resp,mine_resource_system::mineResFightRecordReq,mine_resource_system::mineResSys);
		RegisterFunctionPlus(mine_res_clear_fight_cd_req,mine_res_clear_fight_cd_resp,mine_resource_system::mineResClearFightCdReq,mine_resource_system::mineResSys);
		RegisterFunctionPlus(mine_res_single_update_req,mine_res_single_update_resp,mine_resource_system::mineResSingleUpdateReq,mine_resource_system::mineResSys);
		RegisterFunctionPlus(mine_res_fight_req,mine_res_fight_resp,mine_resource_system::mineResFightReq,mine_resource_system::mineResSys);
		RegisterFunctionPlus(mine_res_free_scan_req,mine_res_free_scan_resp,mine_resource_system::mineResFreeScanReq,mine_resource_system::mineResSys);
		RegisterFunctionPlus(mine_res_general_scan_req,mine_res_general_scan_resp,mine_resource_system::mineResGeneralScanReq,mine_resource_system::mineResSys);
		RegisterFunctionPlus(mine_res_advance_scan_req,mine_res_advance_scan_resp,mine_resource_system::mineResAdvanceScanReq,mine_resource_system::mineResSys);
		RegisterFunctionPlus(mine_res_mine_req,mine_res_mine_resp,mine_resource_system::mineResMineReq,mine_resource_system::mineResSys);

		// 任务系统
		RegisterFunctionPlus(task_main_update_req,task_main_update_resp,task_system::taskMainUpdateReq,task_system::taskSys);
		RegisterFunctionPlus(task_main_commit_req,task_main_commit_resp,task_system::taskMainCommitReq,task_system::taskSys);
		RegisterFunctionPlus(task_branch_update_req,task_branch_update_resp,task_system::taskBranchUpdateReq,task_system::taskSys);
		RegisterFunctionPlus(task_branch_commit_req,task_branch_commit_resp,task_system::taskBranchCommitReq,task_system::taskSys);
		RegisterFunctionPlus(task_branch_tips_req,task_branch_tips_resp,task_system::taskBranchTipsReq,task_system::taskSys);

		//活动展示系统
		//RegisterFunctionPlus(update_action_show_list_req,update_action_show_list_resp,ActivitiesShowSys::updateActionMsg,ActivitiesShowSys::actionShowSys);
		//RegisterFunctionPlus(update_action_active_list_req, update_action_active_list_resp, ActivitiesShowSys::updateActionActiveMsg, ActivitiesShowSys::actionShowSys);
		//RegisterNoResponFunction(notice_client_update_bonus_par_req,ActivitiesShowSys::noticeClientUpdateBonusPar);

		//充值返利
		RegisterFunctionPlus(rebate_update_req,rebate_update_resp,act_rebate::rebateUpdateReq,act_rebate::rebateSys);
		RegisterFunctionPlus(rebate_pickup_gift_req,rebate_pickup_gift_resp,act_rebate::rebatePickupGiftReq,act_rebate::rebateSys);
		RegisterFunctionPlus(rebate_player_info_update_req,rebate_player_info_update_resp,act_rebate::rebatePlayerInfoUpdateReq,act_rebate::rebateSys);

		//充值积分
		RegisterFunctionPlus(point_update_req,point_update_resp,act_point::pointUpdateReq,act_point::pointSys);
		RegisterFunctionPlus(point_exchange_req,point_exchange_resp,act_point::pointExchangeReq,act_point::pointSys);
		RegisterFunctionPlus(point_player_info_update_req,point_player_info_update_resp,act_point::pointPlayerInfoUpdateReq,act_point::pointSys);

		//星域争夺战系统
		RegisterFunctionPlus(guild_battle_total_update_req,guild_battle_total_update_resp,guild_battle_system::guildBattleTotalUpdateReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_single_update_req,guild_battle_single_update_resp,guild_battle_system::guildBattleSingleUpdateReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_levy_req,guild_battle_levy_resp,guild_battle_system::guildBattleLevyReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_apply_req,guild_battle_apply_resp,guild_battle_system::guildBattleApplyReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_enter_req,guild_battle_enter_resp,guild_battle_system::guildBattleEnterReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_enter_limit_req,guild_battle_enter_limit_resp,guild_battle_system::guildBattleEnterLimitReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_enter_cancel_limit_req,guild_battle_enter_cancel_limit_resp,guild_battle_system::guildBattleEnterCancelLimitReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_enter_update_req,guild_battle_enter_update_resp,guild_battle_system::guildBattleEnterUpdateReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_inspire_req,guild_battle_inspire_resp,guild_battle_system::guildBattleInspireReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_exit_req,guild_battle_exit_resp,guild_battle_system::guildBattleExitReq,guild_battle_system::guildBattleSys);
		RegisterFunctionPlus(guild_battle_invite_req,guild_battle_invite_resp,guild_battle_system::guildBattleInviteReq,guild_battle_system::guildBattleSys);

		//星际探险
		RegisterFunctionPlus(interplanetary_exploration_update_req,interplanetary_exploration_update_resp,explore_manager::updateExploreRewardsContent,explore_manager::playerExploreMgr);
		RegisterFunctionPlus(female_insect_nest_req,female_insect_nest_resp,explore_manager::femaleInsectNest,explore_manager::playerExploreMgr);
		RegisterFunctionPlus(the_ancient_god_remains_req,the_ancient_god_remains_resp,explore_manager::theAncientGodRemains,explore_manager::playerExploreMgr);
		RegisterFunctionPlus(gm_set_explore_reward_req,gm_set_explore_reward_resp,explore_manager::setExploreRewards,explore_manager::playerExploreMgr);
		RegisterFunctionPlus(gm_query_explore_reward_req,gm_query_explore_reward_resp,explore_manager::queryExplore,explore_manager::playerExploreMgr);
		RegisterFunctionPlus(female_insect_nest_lottery_req, female_insect_nest_lottery_resp, explore_manager::updateLottery_fin, explore_manager::playerExploreMgr);
		RegisterFunctionPlus(the_ancient_god_remains_lottery_req, the_ancient_god_remains_lottery_resp, explore_manager::updateLottery_tag, explore_manager::playerExploreMgr);
		//战报分享
		RegisterFunctionPlus(share_link_report_req,share_link_report_resp,ReportShare::shareLinkReq,ReportShare::RShare);

		//战报分享
		RegisterFunctionPlus(ally_update_req,ally_update_resp,ally_system::updateReq,ally_system::allySys);
		RegisterFunctionPlus(ally_search_req,ally_search_resp,ally_system::searchReq,ally_system::allySys);
		RegisterFunctionPlus(ally_send_req,ally_send_resp,ally_system::sendReq,ally_system::allySys);
		RegisterFunctionPlus(ally_cancel_req,ally_cancel_resp,ally_system::cancelReq,ally_system::allySys);
		RegisterFunctionPlus(ally_agree_req,ally_agree_resp,ally_system::agreeReq,ally_system::allySys);
		RegisterFunctionPlus(ally_refuse_req,ally_refuse_resp,ally_system::refuseReq,ally_system::allySys);
		RegisterFunctionPlus(ally_dissolve_req,ally_dissolve_resp,ally_system::dissolveReq,ally_system::allySys);
		RegisterFunctionPlus(ally_del_req,ally_del_resp,ally_system::delReq,ally_system::allySys);
		RegisterFunctionPlus(ally_reward_req,ally_reward_resp,ally_system::rewardReq,ally_system::allySys);

		RegisterFunctionPlus(ally_info_update_inner_req,ally_info_update_inner_resp,ally_system::infoUpdateInnerReq,ally_system::allySys);
		RegisterFunctionPlus(ally_op_update_inner_req,ally_op_update_inner_resp,ally_system::opUpdateInnerReq,ally_system::allySys);
		RegisterFunctionPlus(ally_active_op_reward_inner_req,ally_active_op_reward_inner_resp,ally_system::activeOpRewardInnerReq,ally_system::allySys);

		//自定义礼包
		RegisterFunctionPlus(gift_defined_update_req,gift_defined_update_resp,act_gift_defined::giftDefinedUpdateReq,act_gift_defined::giftDefinedSys);
		RegisterFunctionPlus(gift_defined_pickup_gift_req,gift_defined_pickup_gift_resp,act_gift_defined::giftDefinedPickupGiftReq,act_gift_defined::giftDefinedSys);
		RegisterFunctionPlus(gift_defined_player_info_update_req,gift_defined_player_info_update_resp,act_gift_defined::giftDefinedPlayerInfoUpdateReq,act_gift_defined::giftDefinedSys);

		//国战
		RegisterFunctionPlus(starwar_player_info_req, starwar_player_info_resp, starwar_system::playerInfoReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_move_req, starwar_move_resp, starwar_system::moveReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_report_list_req, starwar_report_list_resp, starwar_system::reportListReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_kick_req, starwar_kick_resp, starwar_system::kickReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_reward_req, starwar_reward_resp, starwar_system::rewardReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_star_info_req, starwar_star_info_resp, starwar_system::starInfoReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_use_item_req, starwar_use_item_resp, starwar_system::useItemReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_open_item_req, starwar_open_item_resp, starwar_system::openItemReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_flush_item_req, starwar_flush_item_resp, starwar_system::flushItemReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_buy_item_req, starwar_buy_item_resp, starwar_system::buyItemReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_player_rank_req, starwar_player_rank_resp, starwar_system::playerRankReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_country_rank_req, starwar_country_rank_resp, starwar_system::countryRankReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_task_info_req, starwar_task_info_resp, starwar_system::taskInfoReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_task_commit_req, starwar_task_commit_resp, starwar_system::taskCommitReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_buy_transfer_time_req, starwar_buy_transfer_time_resp, starwar_system::buyTransferTimeReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_clear_transfer_cd_req, starwar_clear_transfer_cd_resp, starwar_system::clearTransferCdReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_king_gather_req, starwar_king_gather_resp, starwar_system::kingGatherReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_officer_gather_req, starwar_officer_gather_resp, starwar_system::officerGatherReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_ranking_reward_req, starwar_ranking_reward_resp, starwar_system::rankingRewardReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_great_event_req, starwar_great_event_resp, starwar_system::greatEventReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_settle_npc_req, starwar_settle_npc_resp, starwar_system::settleNpcReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_get_shortest_path_req, starwar_get_shortest_path_resp, starwar_system::getShortestPathReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_sign_star_req, starwar_sign_star_resp, starwar_system::signStarReq, starwar_system::starwarSys);

		RegisterFunctionPlus(starwar_update_info_inner_req, starwar_update_info_inner_resp, starwar_system::updateInfoInnerReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_attack_with_npc_inner_req, starwar_attack_with_npc_inner_resp, starwar_system::attackWithNpcInnerReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_attack_with_player_inner_req, starwar_attack_with_player_inner_resp, starwar_system::attackWithPlayerInnerReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_task_update_inner_req, starwar_task_update_inner_resp, starwar_system::taskUpdateInnerReq, starwar_system::starwarSys);
		RegisterFunctionPlus(starwar_notice_npc_battle_inner_req, starwar_notice_npc_battle_inner_resp, starwar_system::noticeNpcBattleInnerReq, starwar_system::starwarSys);

		//巡查系统
// 		RegisterFunctionPlus(inspect_tour_update_req, inspect_tour_update_resp, inspectTourSystem::inspectTourUpdateReq, inspectTourSystem::inspectTourSys);
// 		RegisterFunctionPlus(inspect_tour_simple_finish_req, inspect_tour_simple_finish_resp, inspectTourSystem::inspectTourSimpleFinishReq, inspectTourSystem::inspectTourSys);
// 		RegisterFunctionPlus(inspect_tour_xingjidadao_req, inspect_tour_xingjidadao_resp, inspectTourSystem::inspectTourXingjidadaoReq, inspectTourSystem::inspectTourSys);
// 		RegisterFunctionPlus(inspect_tour_beikunshangchuan_req, inspect_tour_beikunshangchuan_resp, inspectTourSystem::inspectTourBeikunshangchuanReq, inspectTourSystem::inspectTourSys);
// 		RegisterFunctionPlus(inspect_tour_fukuangyunshi_req, inspect_tour_fukuangyunshi_resp, inspectTourSystem::inspectTourFukuangyunshiReq, inspectTourSystem::inspectTourSys);
// 		RegisterFunctionPlus(inspect_tour_xijiehaidao_req, inspect_tour_xijiehaidao_resp, inspectTourSystem::inspectTourXijiehaidaoReq, inspectTourSystem::inspectTourSys);
// 		RegisterFunctionPlus(inspect_tour_shenmishangdui_req, inspect_tour_shenmishangdui_resp, inspectTourSystem::inspectTourShenmishangduiReq, inspectTourSystem::inspectTourSys);

		//推荐系统
		RegisterFunctionPlus(invite_sys_info_update_req, invite_sys_info_update_resp, InviteSystem::updatePlayerInviterInfo, InviteSystem::playerInviteMgr);
		RegisterFunctionPlus(input_invite_code_req, input_invite_code_resp, InviteSystem::inputInviteCode, InviteSystem::playerInviteMgr);
		RegisterFunctionPlus(get_invite_reward_req, get_invite_reward_resp, InviteSystem::getInviteReward, InviteSystem::playerInviteMgr);
		RegisterFunctionPlus(inviter_info_update_oneself_update_req, inviter_info_update_oneself_update_resp, InviteSystem::updateInviterInfo, InviteSystem::playerInviteMgr);

		//太空寻宝系统
		RegisterFunctionPlus(space_explore_player_info_req, space_explore_player_info_resp, space_explore_system::spaceExplorePlayerInfoReq, space_explore_system::spaceExploreSys);
		RegisterFunctionPlus(space_explore_one_explore_req, space_explore_one_explore_resp, space_explore_system::spaceExploreOneHuntReq, space_explore_system::spaceExploreSys);
		RegisterFunctionPlus(space_explore_ten_explore_req, space_explore_ten_explore_resp, space_explore_system::spaceExploreTenHuntReq, space_explore_system::spaceExploreSys);
		RegisterFunctionPlus(space_explore_update_req, space_explore_update_resp, space_explore_system::spaceExploreUpdateReq, space_explore_system::spaceExploreSys);
		
		//世界BOSS
		RegisterFunctionPlus(world_boss_update_client_req, world_boss_update_client_resp, world_boss_system::worldBossUpdateClientReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_manual_update_client_req, world_boss_manual_update_client_resp, world_boss_system::worldBossUpdateClientReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_attack_req, world_boss_attack_resp, world_boss_system::worldBossAttackReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_reward_req, world_boss_reward_resp, world_boss_system::worldBossRewardReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_keji_inspire_req, world_boss_keji_inspire_resp, world_boss_system::worldBossKejiInspireReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_gold_inspire_req, world_boss_gold_inspire_resp, world_boss_system::worldBossGoldInspireReq, world_boss_system::worldBossSys); 
		RegisterFunctionPlus(world_boss_clear_attack_cd_req, world_boss_clear_attack_cd_resp, world_boss_system::worldBossClearAttackCdReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_close_ui_req, world_boss_close_ui_resp, world_boss_system::worldBossCloseUiReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_inner_deal_end_req, world_boss_inner_deal_end_resp, world_boss_system::worldBossInnerDealEndReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_storage_capacity_update_req, world_boss_storage_capacity_update_resp, world_boss_system::worldBossStorageCapacityUpdateReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_storage_capacity_req, world_boss_storage_capacity_resp, world_boss_system::worldBossStorageCapacityReq, world_boss_system::worldBossSys);
		RegisterFunctionPlus(world_boss_inner_auto_attack_req, world_boss_inner_auto_attack_resp, world_boss_system::worldBossInnerAutoAttackReq, world_boss_system::worldBossSys);

		//贸易
		RegisterFunctionPlus(player_trade_update_req, player_trade_update_resp, trade_system::tradeUpdate, trade_system::trade_pointer);
		RegisterFunctionPlus(player_trade_item_update_req, player_trade_item_update_resp, trade_system::tradeItemUpdate, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_center_event_update_req, trade_center_event_update_resp, trade_system::tradeCenterUpdate, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_accept_task_req, trade_accept_task_resp, trade_system::acceptTask, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_cancel_task_req, trade_cancel_task_resp, trade_system::cancelTask, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_complete_task_req, trade_complete_task_resp, trade_system::completeTask, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_move_req, trade_move_resp, trade_system::move, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_buy_req, trade_buy_resp, trade_system::buy, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_sale_req, trade_sale_resp, trade_system::sale, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_table_update_req, trade_table_update_resp, trade_system::updateTable, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_park_req, trade_park_resp, trade_system::park, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_use_item_req, trade_use_item_resp, trade_system::useItem, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_blanking_req, trade_blanking_resp, trade_system::Teleport, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_sim_update_req, trade_sim_update_resp, trade_system::updateSimData, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_task_fast_complete_req, trade_task_fast_complete_resp, trade_system::FastTask, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_voucher_show_req, trade_voucher_show_resp, trade_system::showVoucher, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_first_time_req, trade_first_time_resp, trade_system::tradeFrist, trade_system::trade_pointer);
		RegisterFunctionPlus(trade_plane_upgrade_req, trade_plane_upgrade_resp, trade_system::upgradePlane, trade_system::trade_pointer);

		//黑市系统
		RegisterFunctionPlus(market_info_req, market_info_resp, market_system::infoReq, market_system::marketSys);
		RegisterFunctionPlus(market_update_req, market_update_resp, market_system::updateReq, market_system::marketSys);
		RegisterFunctionPlus(market_deal_req, market_deal_resp, market_system::dealReq, market_system::marketSys);

		RegisterFunctionPlus(gm_market_update_req, gm_market_update_resp, market_system::gmMarketUpdateReq, market_system::marketSys);
		RegisterFunctionPlus(gm_market_modify_req, gm_market_modify_resp, market_system::gmMarketModifyReq, market_system::marketSys);

		// 秘书系统
		RegisterFunctionPlus(secretary_info_req, secretary_info_resp, secretary_system::infoReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_lottery_req, secretary_lottery_resp, secretary_system::lotteryReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_embattle_req, secretary_embattle_resp, secretary_system::embattleReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_resolve_req, secretary_resolve_resp, secretary_system::resolveReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_combine_req, secretary_combine_resp, secretary_system::combineReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_swallow_req, secretary_swallow_resp, secretary_system::swallowReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_upgrade_req, secretary_upgrade_resp, secretary_system::upgradeReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_swap_exp_req, secretary_swap_exp_resp, secretary_system::swapExpReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_rename_req, secretary_rename_resp, secretary_system::renameReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_lock_req, secretary_lock_resp, secretary_system::lockReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_param_req, secretary_param_resp, secretary_system::paramReq, secretary_system::secretarySys);
		RegisterFunctionPlus(secretary_show_req, secretary_show_resp, secretary_system::showReq, secretary_system::secretarySys);

		//红包系统
		RegisterFunctionPlus(paper_update_personal_req, paper_update_personal_resp, PaperManager::update, PaperManager::paperSys);
		RegisterFunctionPlus(paper_update_table_req, paper_update_table_resp, PaperManager::updatePaperTable, PaperManager::paperSys);
		RegisterFunctionPlus(paper_dispacth_paper_req, paper_dispacth_paper_resp, PaperManager::dispatchPaper, PaperManager::paperSys);
		RegisterFunctionPlus(paper_rob_req, paper_rob_resp, PaperManager::robPaper, PaperManager::paperSys);
		RegisterFunctionPlus(paper_recovery_paper_req, paper_recovery_paper_resp, PaperManager::recoveryPaper, PaperManager::paperSys);
		RegisterFunctionPlus(paper_send_again_req, paper_send_again_resp, PaperManager::paperSendagain, PaperManager::paperSys);
		RegisterFunctionPlus(paper_exchange_req, paper_exchange_resp, PaperManager::paperExchange, PaperManager::paperSys);
		RegisterFunctionPlus(paper_reward_list_req, paper_reward_list_resp, PaperManager::updateRewardList, PaperManager::paperSys);
		RegisterFunctionPlus(gm_dispatch_paper_req, gm_dispatch_paper_resp, PaperManager::gmDispatchPaper, PaperManager::paperSys);
		RegisterFunctionPlus(gm_paper_update_req, gm_paper_update_resp, PaperManager::gmPaperUpdateReq, PaperManager::paperSys);

		RegisterFunctionPlus(paper_inner_send_gm_paper_req, paper_inner_send_gm_paper_resp, PaperManager::innerSendGmPaperReq, PaperManager::paperSys);


		RegisterFunctionPlus(gm_secretory_list_req, gm_secretory_list_resp, secretary_system::gmSecretaryListReq, secretary_system::secretarySys);
	}

	handler::updateFunction::updateFunction()
	{
		state_ = -1;
	}

	handler::CallFunction::CallFunction()
	{
		resp_ = -1;
	}

	handler::~handler()
	{

	}

	void handler::on_system_operating()
	{
		static boost::system_time tmp;
		tmp = na::time_helper::get_sys_time();
		for (unsigned i = 0; i < func_vector.size(); ++i)
		{
			updateFunction& func = func_vector[i];
			State::setState(func.state_);
			NumberCounter::Step();
			func.f_(tmp);
			helper_mgr.runSaveAndUpdate();
		}
	}

	void handler::logShowState(const bool state)
	{
		showLog = state;
	}

	void handler::internalCallback(msg_json::ptr ptr)
	{
		++ptr->_post_times;
		despatch(*ptr);
	}

	void handler::recv_client_handler(tcp_session_ptr session, msg_json& m)
	{
		session->keep_alive();
		despatch(m);
	}

	//gm tools
	void handler::recv_local_handler(tcp_session_ptr session, msg_json& m)
	{
		session->keep_alive();
		State::setState(m._type);
		NumberCounter::Step();
		if(m._type == game_protocol::comomon::keep_alive_req)
		{
			string str;
			na::msg::msg_json mj(game_protocol::comomon::keep_alive_resp,str);
			session->write_json_msg(mj);
			return;
		}

		if(m._type == game_protocol::comomon::del_playerInfo_req)
		{
			player_mgr.playerOffline(m._player_id);
			return;
		}
		despatch(m);
	}

	void handler::reg_func(const short command_id, const short response_id, handler_function func)
	{
		CallFunction f;
		f.resp_ = response_id;
		f.f_ = func;
		func_keeper[command_id] = f;
	}

	void handler::despatch(na::msg::msg_json& m)
	{
		try
		{
			if (showLog)
			{
				LogS << color_yellow("==============recv================") << LogEnd;
				LogS << "type		id" << LogEnd;
				LogS << m._type << "		" << m._player_id << LogEnd;
			}

			if (m._type > system_callback__req_start
				&& m._type < system_callback_req_end
				&& m._net_id >= 0)
			{
				LogE << "Inner Req: type (" << m._type << "), net_id (" << m._net_id << ")" << LogEnd;
				return;
			}

			Json::Value resp_json = Json::Value::null;
			function_keeper::iterator it = func_keeper.find(m._type);
			if(it == func_keeper.end())return;
			State::setState(m._type);
			NumberCounter::Step();
			CallFunction& f = it->second;
			f.f_(m,resp_json);
			helper_mgr.runSaveAndUpdate();
//			cout << "m.type:" << m._type << ",resp_json:" << resp_json.toStyledString() << endl;
			if(!resp_json.isNull() && f.resp_ != -1 && (m._net_id >= 0 && m._player_id > 0
				|| gm_tools_mgr.is_from_gm_http(m._net_id))){
				string str = resp_json.toStyledString();
				na::msg::msg_json mj(m._player_id, m._net_id, f.resp_, str);
				game_svr->async_send_to_gate(mj);
			}
		}catch(std::exception& e)
		{
			LogE << e.what() << LogEnd;
		}
	}
}