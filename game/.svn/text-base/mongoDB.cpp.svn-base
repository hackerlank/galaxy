#include "mongoDB.h"
#include "commom.h"
#include <config.h>
#include <boost/lexical_cast.hpp>
#ifndef WIN32
#include "DebugInfo.h"
#endif
#pragma warning (disable : 4244)

namespace gg
{
//#define MongoLog


	db_manager* const db_manager::dbMgr = new db_manager();

	db_manager::db_manager(void):
		_DB_READ(true), _DB_WRITE(true)
	{
	}
		
	db_manager::~db_manager(void)
	{
	}

	bool db_manager::connect_db(const string ip_str)
	{		
		std::string error_str;
		if (!_DB_READ.connect(ip_str.c_str(), error_str))
		{
			LogE << "Read Failed to connect db :" << error_str << LogEnd;
			return false;
		}
		if (!_DB_WRITE.connect(ip_str.c_str(), error_str))
		{
			LogE << "Write Failed to connect db :" << error_str << LogEnd;
			return false;
		}
		return true;
	}

	void db_manager::ensure_index(const std::string &collection, const mongo::BSONObj& key)
	{
		string turn_str = convert_server_db_name(collection);
		//LogI << "ensure index " << turn_str << "	key:" << key.jsonString() << LogEnd;
		_DB_WRITE.ensureIndex(turn_str, key, true);
	}

	bool db_manager::save_mongo(const std::string& db_name_str, const mongo::BSONObj& key, const mongo::BSONObj& val)
	{
		bool ret = true;
		string db_turn_str = convert_server_db_name(db_name_str);
#ifdef MongoLog
		LogI << "save mongo " << db_turn_str << "	key:" << key.jsonString() << LogEnd;
#endif
		try{
			_DB_WRITE.update(db_turn_str, mongo::Query(key), val, true);
		}
		catch(mongo::DBException &e){
			LogE << "caught error:" << e.what() << LogEnd; 
			LogE << "error db name:" << db_name_str << " key:" << key.jsonString() << " value:" << val.jsonString() << LogEnd;
			ret = false;
		}
		return ret;
	}

// 	bool db_manager::update_part_mongo(const std::string& db_name_str,const mongo::BSONObj& key, const mongo::BSONObj& val)
// 	{
// 		mongo::BSONObj obj = BSON("$set" << val);
// 		return save_mongo(db_name_str,key,obj);
// 	}
// 
// 	bool db_manager::increase_part_mongo(const std::string& db_name_str,const mongo::BSONObj& key, const mongo::BSONObj& val)
// 	{
// 		mongo::BSONObj obj = BSON("$inc" << val);
// 		return save_mongo(db_name_str,key,obj);
// 	}

// 	int db_manager::load_all_collection(const std::string& db_name_str, objCollection& collections)
// 	{
// 		string db_trun_str = convert_server_db_name(db_name_str);
// 		mongo::Query query = mongo::Query();
// 		std::auto_ptr<mongo::DBClientCursor> cursor = _db_conn.query(db_trun_str,query);
// 		LogI << "Start load all collection in:" + db_trun_str << LogEnd;
// 
// 		int load_obj_num = 0;
// 		while (cursor->more()) 
// 		{  
// 			mongo::BSONObj res = cursor->next(); 
// 			collections.push_back(res.copy());
// 			++load_obj_num;
// 		}
// 
// 		LogI << "Load finish.("<<load_obj_num<<" collections loaded)"<<LogEnd;
// 		return load_obj_num;
// 	}

	void db_manager::remove_collection(const std::string &collection, const mongo::BSONObj& key)
	{
		string collection_trun = convert_server_db_name(collection);
#ifdef MongoLog
		LogI << "remove colloection " << collection_trun << LogEnd;
#endif // MongoLog
		_DB_WRITE.remove(collection_trun, mongo::Query(key));
	}

	bool db_manager::drop_the_collection(const std::string& collection_name)
	{
		string collection_name_trun = convert_server_db_name(collection_name); 
		return _DB_WRITE.dropCollection(collection_name_trun);
	}

	objCollection db_manager::Query(const std::string& db_name_str, const mongo::BSONObj key_word /* = mongo::BSONObj() */)
	{
		string db_trun_str = convert_server_db_name(db_name_str);
		boost::mutex::scoped_lock l(LOCK);// no idea
		mongo::Query query = mongo::Query(key_word);
#ifdef MongoLog
		LogI << "load DB name:" << db_trun_str << "	and key :" << key_word.jsonString() << LogEnd;
#endif // MongoLog
		std::auto_ptr<mongo::DBClientCursor> cursor = _DB_READ.query(db_trun_str, query);
		objCollection objs;
		while (cursor->more()) 
		{  
			mongo::BSONObj res = cursor->next(); 
			objs.push_back(res.copy());
		}
		return objs;
	}

	mongo::BSONObj db_manager::FindOne(const std::string& db_name_str,const mongo::BSONObj& key_word)
	{
		objCollection objs = Query(db_name_str, key_word);
		if (objs.empty())return mongo::BSONObj();
		return objs.begin()->copy();
// 		string db_trun_str = convert_server_db_name(db_name_str);
// 		boost::mutex::scoped_lock l(LOCK);// no idea
// 		LogI << "load DB name:" << db_trun_str << "and key :" << key_word.jsonString() << LogEnd;
// 		mongo::Query query = mongo::Query(key_word);
// 		return _db_conn.findOne(db_trun_str, query);
	}

	std::string db_manager::convert_server_db_name(const std::string& db_str )
	{		
		static int sid = config_ins.get_config_prame("server_id").asInt();
		static std::string prefix_str = "sid" + boost::lexical_cast<std::string,int>(sid) + ".";
		std::string tmp =  prefix_str + db_str;
		return tmp;
	}
}