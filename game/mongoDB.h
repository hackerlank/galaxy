#pragma once

#include <client/dbclient.h>
#include <json/json.h>
#include <boost/thread/mutex.hpp>
#include <vector>
using namespace std;

#define db_mgr (*gg::db_manager::dbMgr)

namespace gg
{	
	typedef vector<mongo::BSONObj> objCollection;
	class db_manager
	{
	public:
		static db_manager* const dbMgr;
		db_manager(void);
		~db_manager(void);

		bool			connect_db(const string ip_str);
		
		void			ensure_index(const std::string &collection, const mongo::BSONObj& key);
		bool			save_mongo(const std::string& db_name_str, const mongo::BSONObj& key, const mongo::BSONObj& val);
// 		bool			update_part_mongo(const std::string& db_name_str,const mongo::BSONObj& key, const mongo::BSONObj& val);
// 		bool			increase_part_mongo(const std::string& db_name_str,const mongo::BSONObj& key, const mongo::BSONObj& val);
		mongo::BSONObj	FindOne(const std::string& db_name_str,const mongo::BSONObj& key_word);//multi thread
		objCollection Query(const std::string& db_name_str, const mongo::BSONObj key_word = mongo::BSONObj());
//		int				load_all_collection(const std::string& db_name_str, objCollection& collections);
		void			remove_collection(const std::string &collection, const mongo::BSONObj& key);
		bool			drop_the_collection(const std::string& collection_name);

	private:
		std::string		convert_server_db_name(const std::string& db_str);
		mongo::DBClientConnection _DB_READ, _DB_WRITE;
		boost::mutex LOCK;
	};
}