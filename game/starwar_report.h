#pragma once

#include "json/json.h"
#include "mongoDB.h"
#include "block.h"

namespace gg
{
	namespace starwar
	{
		enum
		{
			_report_error = -1,
			_report_0,                 // 在移动遭遇,或在星球被攻击 ==> n小时前, 您与xx国舰队发生作战
			_report_1,                 // 攻击星球但没占领 ==> n小时前, 您与xx星球的驻防舰队发生作战
			_report_2,                 // 攻击星球并成功占领 ==> n小时前, 您占领了xx星球
			_report_3,                 // 破坏令 ==> n小时前, xx对您使用了y级破坏令
			_report_4,                 // 攻击亲卫兵 ==> n小时前, xx星球中的亲卫兵与yy发生作战
			_report_5,                 // 战报 ==> 你击败了xxx [战报]
		};

		class iReport
		{
		public:
			virtual ~iReport(){}
			virtual void getValue(Json::Value& info) = 0;
			virtual void toBSON(mongo::BSONObjBuilder& obj) = 0;
		};

		typedef boost::shared_ptr<iReport> reportPtr;
		typedef list<reportPtr> reportList;

		class describeRep : public iReport
		{
		public:
			describeRep(int type, unsigned time, int arg)
				: _type(type), _time(time), _arg(arg){}
			describeRep(const mongo::BSONElement& obj);
			virtual ~describeRep(){}

			virtual void getValue(Json::Value& info);
			virtual void toBSON(mongo::BSONObjBuilder& obj);

		private:
			int _type;
			unsigned _time;
			int _arg;
		};

		class supRep : public iReport
		{
		public:
			supRep(unsigned time, const string& name, int lv, bool dead)
				: _time(time), _name(name), _lv(lv), _dead(dead){}
			supRep(const mongo::BSONElement& obj);
			virtual ~supRep(){}

			virtual void getValue(Json::Value& info);
			virtual void toBSON(mongo::BSONObjBuilder& obj);

		private:
			unsigned _time;
			string _name;
			int _lv;
			bool _dead;
		};

		class guildRep : public iReport
		{
		public:
			guildRep(unsigned time, int star_id, const string& name)
				: _time(time), _star_id(star_id), _name(name){}
			guildRep(const mongo::BSONElement& obj);
			virtual ~guildRep(){}

			virtual void getValue(Json::Value& info);
			virtual void toBSON(mongo::BSONObjBuilder& obj);

		private:
			unsigned _time;
			int _star_id;
			string _name;
		};

		class battleRep : public iReport
		{
		public:
			battleRep(const int result, const string& report_id, const string& target_name, int exploit, int sup, int target_sup, int silver, bool bNpc, int title = -1)
				: _result(result), _report_id(report_id), _target_name(target_name), _exploit(exploit), _sup(sup), _target_sup(target_sup), _silver(silver), _bNpc(bNpc), _title(title){}
			battleRep(const mongo::BSONElement& obj);
			virtual ~battleRep(){}

			virtual void getValue(Json::Value& info);
			virtual void toBSON(mongo::BSONObjBuilder& obj);

		private:
			int _result;
			string _report_id;
			string _target_name;
			int _exploit;
			int _sup;
			int _target_sup;
			int _silver;
			bool _bNpc;
			int _title;
		};

		class combineRep : public iReport
		{
		public:
			combineRep(const reportPtr& describe, const reportList& rep_list)
				: _describe(describe), _reportList(rep_list){}
			combineRep(const mongo::BSONElement& obj);
			virtual ~combineRep(){}

			virtual void getValue(Json::Value& info);
			virtual void toBSON(mongo::BSONObjBuilder& obj);

		private:
			reportPtr _describe;
			reportList _reportList;
		};

		class repFactory
		{
		public:
			static reportPtr create(const mongo::BSONElement& obj);

			static reportPtr create(unsigned time, int star_id, const string& name);
			static reportPtr create(unsigned time, const string& name, int lv, bool dead);
			static reportPtr create(const int result, const string& report_id, const string& target_name, int exploit, int sup, int target_sup, int silver, bool bNpc, int title = -1);
			static reportPtr create(int type, unsigned time, int arg);
			static reportPtr create(const reportPtr& describe, const reportList& rep_list);
		};

		class reportManager
		{
		public:
			enum{
				_max_rep = 10
			};

			mongo::BSONArray toBSON();
			void setFromBSON(const mongo::BSONElement& obj);

			void getValue(Json::Value& info);
			void updateLast(playerDataPtr d);
			void push(reportPtr ptr);

		private:
			reportList _reportList;
		};
	}
}
