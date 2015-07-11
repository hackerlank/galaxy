#pragma once
#include "player_base.h"
#include "block.h"
#include "file_system.h"
#include <boost/unordered_map.hpp>
namespace gg{
	const static string player_onlineaward_db_name="gl.player_online_award";		//���ݿ���

	const static string player_award_mongoDB = "aw";	//���ݿ��е�����
	const static string player_award_type = "at";	//��������,��int�ͱ�ʾ����1��ʼ����ÿ�ε�������ʽ�ۼӣ�ÿ���ۼ�1
	const static string player_award_total_cd = "tc";	//��cdʱ��
	const static string player_award_content = "c";	//��������
	//const static string player_award_silver = "asl";	//��������
	//const static string player_award_gold	= "agl";	//�������
	//const static string player_award_keji	= "akj";	//�����Ƽ�
	//const static string player_award_goldticket = "aglt";	//������Ʊ
	//const static string player_award_credit	= "ac";	//�������鵤
	//const static string player_award_honor = "ah";	//��������ֵ
	//const static string player_award_supply = "as";	//��������
	const static string player_award_limit_level = "lv";	//�������Ƶȼ�

	const static string player_award_login_time = "li";	//�û���¼ʱ��
	const static string player_award_logout_time = "lo";	//�û��ǳ�ʱ��
	const static string player_award_content_indicate = "aci";	//��������ָʾ
	const static string player_award_type_indicate = "ati";	//��������ָʾ
	const static string player_initial_time = "it";		//��ȡ�����ļ�ʱʱ��
	const static string player_award_cd_time = "cd";  //��ȡ����ʣ��CDʱ��
	const static string player_get_award_num = "an";	//����ȡ��������
	//�����Ϣ
	struct playerMsg{
		unsigned int playerLoginTime;		//�û���¼ʱ��
		unsigned int playerLogoutTime;		//�û��ǳ�ʱ��
		int playerContentIndicate;			//��������ָʾ
		int awardTypeIndicate;	//��������ָʾ
		int awardNum;	//���콱������
		unsigned int initialTime;//�콱ʱ��
		unsigned int awardCD;	//�콱CDʱ��
		playerMsg(){
			playerLoginTime = 0;
			playerLogoutTime = 0;
			playerContentIndicate = 0;
			initialTime= 0;
			awardCD= 0;
			awardNum= 0;
			awardTypeIndicate = 1;
		}
	};

	struct Award{
		int actionID;
		int itemID;
		//vector<int> value;			//��������⼸����Դ������
		vector<int> addNum;			//��Ʒ������	
		Award(){
			actionID = -1;
			itemID = -1;
			//value.clear();
			addNum.clear();	
		}
	};
	//��������
	struct AwardContent{
		int awardType;	//��������
		int awardTotalCD;//��CDʱ��
		vector<int> levelLimit;	//���Ƶȼ�
		vector<Award> m_Award;
		AwardContent(){
			awardType  = -1;
			awardTotalCD = -1;
			m_Award.clear();
			levelLimit.clear();
		}

	};
	

	//struct AwardContent{
	//	int awardType;	//��������
	//	int awardTotalCD;//��CDʱ��
	//	vector<int> awardSilver;//����
	//	vector<int> awardGold;	//���
	//	vector<int> awardGoldTicket;//��Ʊ
	//	vector<int> awardKeJI;	//�Ƽ�

	//	vector<int> awardCredit;	//���鵤
	//	vector<int> awardHonor;	//����ֵ
	//	vector<int> awardSupply;	//����

	//	vector<int> levelLimit;	//���Ƶȼ�
	//	AwardContent(){
	//		memset(this,0,sizeof(AwardContent));
	//	}
	//};
	typedef boost::unordered_map<int,AwardContent> AwardContentMap;
	class playerOnlineAward :
		public Block
	{
	public:
		playerOnlineAward(playerData &own);
		~playerOnlineAward(void);
		//��ȡ����
		int getAward(playerDataPtr player, int typeIndicate,double ratio, Json::Value& fail_value);
		//���ý�����Ϣ
		void resetAward(const AwardContentMap& contentMap);
		//��¼�û���¼ʱ��
		void playerLogin();
		//����˳���Ϣ����
		bool playerLogout();
		//�������ݿ�ָʾ������ָ���ĸ����ݿ�
		//void setDBIndicate(int DBIndicate);
		void packagePlayerMsg(Json::Value &pack);
		//�̳и��෽��
		virtual void autoUpdate();
		virtual void update();
		virtual bool save();
		virtual bool get();
	private:

		//���������Ϣ
		bool updatePlayer(playerDataPtr player, double ratio, Json::Value& fail_value);
		//��ʼ��������Ϣ
		void initAwardMsg();
		//�ж�CDʱ��
		bool judgeCD(unsigned now);
		void package(Json::Value &pack);
		//���actionDo json
		int packageActionDoJson(int type,double ratio,Json::Value& msg);
		//�����Դjson
		int packageClientJson(int type,Json::Value& msg);
	private: 
		playerMsg m_PlayerMsg;
		AwardContentMap m_PlayerAwardContent;
	    //unsigned int  m_DbUpdateTime;	//��¼�ϴ����ݿ�ĸ���ʱ��
		//int m_DBIndicate;	//���ݿ�ָʾ��
		int goodsID;		//��ƷID
		int limitBase;
		unsigned updateTime;
	};
}
