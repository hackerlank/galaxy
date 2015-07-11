#pragma once
#include "block.h"
#include "game_helper.hpp"
//#include "game_handle.h"

namespace gg
{
#define ONE_PAKGE_MAIL_NUM 10
#define MAX_MAIL_IN_DB 100
#define MAX_MAIL_CONTENT_WORD_NUM 420
#define MAX_MAIL_ID 100000

	enum emailType
	{
		email_type_begin = 0,
		email_type_player,
		email_type_system_ordinary,
		email_type_system_attachment,
		email_type_system_battle_report,
		email_type_end
	};

	enum emailTeam
	{
		email_team_null = 0,
		email_team_player,
		email_team_guild,

		email_team_system_pvp = 10,
		email_team_system_bag_full,//系统赠送道具，默认
		email_team_system_bag,
		email_team_system_mine_res_gain,
		email_team_system_mine_res_atk,
		email_team_system_gm,
		email_team_system_charge,
		email_team_system_replace_guild_leader,
		email_team_system_guild_battle,
		email_team_system_bag_full_war_story,//仓库满时征战获得道具
		email_team_system_bag_full_study,//仓库满时设计获得道具
		email_team_system_bag_full_shop,//仓库满时商城购买道具
		email_team_system_bag_full_daily_login,//仓库满时每日登陆获得道具
		email_team_system_bag_full_vip_gift,//仓库满时领取VIP奖励
		email_team_system_new_player,//新建号时给新玩家发邮件
		email_team_system_bag_full_online,//仓库满时在线奖励
		email_team_system_space_explore,//仓库满时寻宝奖励
		email_team_system_charge_point,//仓库满时积分兑换
		email_team_system_world_boss,//世界boss
		email_team_system_bag_full_world_boss,//仓库满时世界boss获得道具
		email_team_system_leave_guild,
		email_team_system_bag_full_market,
		email_team_system_secretary_full,
		email_team_system_trade_reward_full,//仓库背包满了奖励钛合金
		email_team_system_bag_full_sweep,//仓库满时征战获得道具

		email_team_system_end
	};

	namespace email
	{
		static const string emailDBStr = "gl.player_email";
		static const string cur_page_mail_list = "ml";		/**当前页邮件列表*/
		static const string email_id = "id";		/**邮件ID*/
		static const string player_email_list = "pel";		/**玩家邮件列表*/
		static const string ordinary_email_list = "oel";		/**系统邮件列表*/
		static const string attach_email_list = "ael";		/**附件邮件列表*/
		static const string report_email_list = "rel";		/**战报邮件列表*/
		static const string save_email_list	= "sel"; /**保存邮件列表*/
		static const string email_list = "ls";		/**邮件列表*/
		static const string content	= "ct";		/**邮件内容*/
		static const string attachment	= "at";		/**邮件内容*/
		static const string send_time					= "st";		/**发送时间*/
		static const string type						= "tp";		/**邮件类型*/
		static const string team						= "tm";		/**邮件分组*/
		static const string sender_name					= "sn";		/**发件人名字*/
		static const string sender_id					= "si";		/**发件人Id*/
		//		static const string content_player_battle_report_adrss	= "ba";		/*战报地址*/

		static const string gift_has_get = "hg";
		static const string gift_action = "act";
		static const string gift_name = "gn";
		static const string gift_picture_id = "gpid";
		static const string gift_type = "gt";
		static const string strLogoutTime = "lot";
		static const string strHasNewEmail = "hne";

		static const string charge_gold = "cg";

		static const string leave_word = "lw";

		static const string actionID = "aid";
		static const string value = "v";
		static const string addNum = "an";
		static const string pilotActiveList = "pal";
		static const string itemID = "iid";
		static const string addItemRate = "air";
		static const string actionItemType = "tp";
		static const string secretaryList = "sl";

		static const string mysqlLogEmail = "log_email";
	}

	enum
	{
		log_email_get_gift,
		log_email_del
	};

	struct attachManager
	{
		attachManager();
		Json::Value toJval();
		bool isValid();
		bool getGift(playerDataPtr d);
		Json::Value attachAction;
		bool has_get;
		string packageName;
		int packagePictureId;
		int packageType;
// 		Json::Value complex2simple(Json::Value& complexAction);
// 		Json::Value simple2complex(Json::Value& simpleAction);
	};

	struct playerEmailItem
	{
		playerEmailItem();
		int email_id;
		emailType type;
		emailTeam team;
		string sender_name;
		int sender_id;
		unsigned send_time;
		vector<attachManager> attachments;
		Json::Value content;

	};

	typedef boost::shared_ptr<playerEmailItem> playerEmailItemPtr;

	struct playerEmailData
	{
		int cur_email_id;
		playerEmailData();
		vector<playerEmailItemPtr> vecPlayerEmail;
		vector<playerEmailItemPtr> vecSysOrdEmail;
		vector<playerEmailItemPtr> vecSysAttachEmail;
		vector<playerEmailItemPtr> vecSysReportEmail;
		vector<playerEmailItemPtr> vecSaveEmail;
	};

	class playerEmail : public Block
	{
	public:
		playerEmail(playerData& own);
		bool add_email_to_db_emaillist(playerEmailItemPtr email_item_ptr);
		bool add_email_to_save_emaillist(playerEmailItemPtr email_item_ptr);
		void autoUpdate();
		bool get();
		bool save();
		void package(Json::Value& pack);
		int delEmail(int email_type, int email_id);
		int delSaveEmail(int email_id);
		playerEmailItemPtr json2mail_item(Json::Value& jval);
		playerEmailData player_email_data;
		void playerLogout();
		bool newEmailCheck();

		static playerEmailItemPtr createPlayerEmailItemPtr()
		{
			void* p = GGNew(sizeof(playerEmailItem));
			return playerEmailItemPtr(new(p) playerEmailItem(), destroyPlayerEmailItemPtr);
		}

		static void destroyPlayerEmailItemPtr(playerEmailItem* point)
		{
			point->~playerEmailItem();
			GGDelete(point);
		}

	private:
//		bool saveJson();
		void getAllEmail(vector<playerEmailItemPtr> &allEmails, vector<int> &allIDs);
		int genNewMailID();
		void singleGet(mongo::BSONElement& sobj, vector<playerEmailItemPtr> &eItem);
		int singleSave(mongo::BSONArrayBuilder& em_arr, vector<playerEmailItemPtr> eItem);
//		void singleSaveJson(Json::Value& sjval, vector<playerEmailItem> eItem);
		void singlePackage(Json::Value& pack, vector<playerEmailItemPtr> eItem);
		void singleItemPackage(Json::Value& itemPack, playerEmailItemPtr eItem);
		bool newEmailSingleCheck(vector<playerEmailItemPtr> &eItem);
		unsigned logoutTime_;

	};
}
