#include "core_helper.h"
#include "time_helper.h"
#include "msg_base.h"
#include "commom.h"
#include "mysqlManager.h"
#include <mysql++.h>
#include <iomanip>
#include <ssqls.h>
#include <boost/lexical_cast.hpp>
#include "file_system.h"
#include <boost/date_time/posix_time/posix_time.hpp>

//#define LOG_SAVE_DEBUFF

namespace db
{
	logSave_system::logSave_system(void)
	{
		const std::string file_name = "./server/db_cfg.json";
		mysql_info = na::file_system::load_jsonfile_val(file_name);
		
		db = mysql_info["mysqlDbName"].asString();
		server = mysql_info["addressOfMysql"].asString();
		user = mysql_info["mysqlUser"].asString();
		pass = mysql_info["mysqlPw"].asString();
		db_port = mysql_info["mysqlPort"].asInt();

		dbConnect();
	}

	logSave_system::~logSave_system(void)
	{
		//net_core.stop();
		logSave_sys.conn.disconnect();
	}

	void logSave_system::log_string(const char* data_ptr, size_t len)
	{
		na::msg::msg_json::ptr recv_msg = na::msg::msg_json::create(data_ptr,len);
		Json::Value val;
		Json::Reader r;
		r.parse(recv_msg->_json_str_utf8,val);
		Json::Value resp_json;

		string logTable = val[0u].asString();
		int logType = val[1u].asInt();
		int logPlayer = val[2u].asInt();
		string logName = val[3u].asString();
		int logLevel = val[4u].asInt();
		string logValue = val[5u].asString();
		string logValueChange =val[6u].asString();
		int logRelate = val[7u].asInt();
		int logTag = val[8u].asInt();
		int logServer = val[9u].asInt();
		tran_time(logRelate);
		string nowTime = CurrentDate;

		logName = replaceIllegalChar(logName);

		std::string sql = "insert into "+ logTable +
			" (log_server, log_time, log_user, log_name, log_level, log_type, log_previous, log_now, log_relate, log_tag) values("+
			boost::lexical_cast<string , int>(logServer)+",\'"+nowTime+"\',"+boost::lexical_cast<string , int>(logPlayer)
			+",\'"+ logName +"\'," + boost::lexical_cast<string, int>(logLevel) + "," + boost::lexical_cast<string , int>(logType) + ",\'"+ logValue +"\',\'"+ 
			logValueChange + "\'," + nowTime + boost::lexical_cast<string, int>(logRelate) + "," + boost::lexical_cast<string , int>(logTag) + ")";

		coreM.get_io(mysql_io).post(boost::bind(&logSave_system::write, this, sql));
	}

	void logSave_system::log_system(const char* data_ptr, size_t len)
	{
		na::msg::msg_json::ptr recv_msg = na::msg::msg_json::create(data_ptr,len);
		Json::Value val;
		Json::Reader r;
		r.parse(recv_msg->_json_str_utf8,val);

		string logTable = val[0u].asString();
		int logType = val[1u].asInt();
		int logPlayer = val[2u].asInt();
		string logName = val[3u].asString();
		int logLevel = val[4u].asInt();
		string logValue = val[5u].asString();
		string logValueChange =val[6u].asString();
		int logRelate = val[7u].asInt();
		int logTag = val[8u].asInt();
		int logServer = val[9u].asInt();
		tran_time(logRelate);
		string nowTime = CurrentDate;

		logName = replaceIllegalChar(logName);

		unsigned n = val.size() - 10;

		string sql = "insert into ";
		sql += logTable;
		sql += "(log_server, log_time, log_user, log_name, log_level, log_type, log_previous, log_now, log_relate, log_tag";
		unsigned i = 0;
		while(i++ < n)
		{
			sql += ", f";
			sql += boost::lexical_cast<string , unsigned>(i);
		}

		sql += ") values(";
		sql += boost::lexical_cast<string , int>(logServer);
		sql += ", \'";
		sql += nowTime;
		sql += "\', ";
		sql += boost::lexical_cast<string , int>(logPlayer);
		sql += ", \'";
		sql += logName;
		sql += "\', ";
		sql += boost::lexical_cast<string ,int>(logLevel);
		sql += ",";
		sql += boost::lexical_cast<string , int>(logType);
		sql += ", \'";
		sql += logValue;
		sql += "\', \'";
		sql += logValueChange;
		sql += "\', ";
		sql += nowTime;
		sql += boost::lexical_cast<string, int>(logRelate);
		sql += ", ";
		sql += boost::lexical_cast<string , int>(logTag);
		i = 0;
		while(i++ < n)
		{
			sql += ", \'";
			sql += val[9+i].asString();
			sql += "\' ";
		}
		sql += ")";
		coreM.get_io(mysql_io).post(boost::bind(&logSave_system::write, this, sql));
	}

	string logSave_system::replaceIllegalChar(string& source)
	{
		string str("");
		for(string::iterator it = source.begin(); it != source.end(); ++it){
			char c = *it;
			if(isMatchIllegal(c))str.append("\\");
			str.insert(str.end(), c);
		}
		return str;
	}

	bool logSave_system::isMatchIllegal(char& c)
	{
// 		if(c == ',' || c == '.' || c == ';' || c == '\\' || c == '\'' ||
// 			c == '\"' || c == '\\' || c == '*' || c == '-')return true;
		if(c == '\'')return true;
		return false;
	}

	void logSave_system::write(string sql)
	{
		if(dbConnect() == -1)return;
		try 
		{
			mysqlpp::Query query = conn.query(sql.c_str());
			query.exec();
		}
		catch (const mysqlpp::BadQuery& er) {
			// Handle any query errors
			LogE<<  "Query error: " << er.what() << LogEnd;
			LogE<<  "Error sql: " << sql << LogEnd;
			//return -1;
		}
		catch (const mysqlpp::BadConversion& er) {  
			// Handle bad conversions
			LogE<<  "Conversion error: " << er.what() << LogEnd <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << LogEnd;
			//return -1;
		}
		catch (const mysqlpp::Exception& er) {
			// Catch-all for any other MySQL++ exceptions
			LogE<<  "Error: " << er.what() << LogEnd;
			//return -1;
		}
	}

	int logSave_system::CurrentRelateID = -1;
	string logSave_system::CurrentDate = "19000101000000";

	void logSave_system::tran_time(const int relate)
	{		
		if(CurrentRelateID != relate)tran_time();
		CurrentRelateID = relate;
	}

	void logSave_system::tran_time()
	{
		unsigned now = na::time_helper::get_current_time();
		tm pt_tm = na::time_helper::toTm(now);
		int year = pt_tm.tm_year+1900;
		int mon = pt_tm.tm_mon+1;
		int day = pt_tm.tm_mday;
		int hour = pt_tm.tm_hour;
		int min = pt_tm.tm_min;
		int sec = pt_tm.tm_sec;

		CurrentDate.clear();
		CurrentDate += boost::lexical_cast<std::string,unsigned int> (year);
		if(mon<10)CurrentDate += "0";
		CurrentDate += boost::lexical_cast<std::string,unsigned int> (mon);
		if(day<10)CurrentDate += "0";
		CurrentDate += boost::lexical_cast<std::string,unsigned int> (day);
		if(hour<10)CurrentDate += "0";
		CurrentDate += boost::lexical_cast<std::string,unsigned int> (hour);
		if(min<10)	CurrentDate += "0";
		CurrentDate += boost::lexical_cast<std::string,unsigned int> (min);
		if(sec<10)CurrentDate += "0";
		CurrentDate += boost::lexical_cast<std::string,unsigned int> (sec);
	}

	int logSave_system::dbConnect()
	{
		try                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
		{
			if(conn.ping())
			{
				return 1;
			}

			conn.disconnect();

			mysqlpp::SetCharsetNameOption *charsetName = new mysqlpp::SetCharsetNameOption("utf8");
			//mysqlpp::SetCharsetNameOption *charsetName = new mysqlpp::SetCharsetNameOption("gb2312");
			conn.set_option(charsetName);
			if(db_port==0) 
				db_port = 3306;
			if (conn.connect(db.c_str(), server.c_str(), user.c_str(), pass.c_str()),db_port) 
			{
				LogI<<  "Mysql connection success:" << LogEnd;
				//LogE << "Mysql connection success: " << conn.error() << endl;
			}
			else 
			{
				LogE<<  "Mysql connection failed: " << conn.error() << LogEnd;
			}
		}
		catch (const mysqlpp::BadQuery& er) {
			// Handle any query errors
			LogE <<  "Query error: " << er.what() << LogEnd;
			return -1;
		}
		catch (const mysqlpp::BadConversion& er) {  
			// Handle bad conversions
			LogE<<  "Conversion error: " << er.what() << LogEnd <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << LogEnd;
			return -1;
		}
		catch (const mysqlpp::Exception& er) {
			// Catch-all for any other MySQL++ exceptions
			LogE <<  "Error: " << er.what() << LogEnd;
			return -1;
		}
		return 0;
	}

}

