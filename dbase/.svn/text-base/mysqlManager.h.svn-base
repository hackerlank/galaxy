#pragma once
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <json/json.h>
#include <boost/thread/thread_time.hpp>
#include <mysql++.h>
#include <queue>
#include <string>
using namespace std;

#define logSave_sys boost::detail::thread::singleton<db::logSave_system>::instance()

namespace db
{
	class logSave_system
	{
	public:
		logSave_system(void);
		~logSave_system(void);

		void log_string(const char* data_ptr, size_t len);
		void log_system(const char* data_ptr, size_t len);
		int dbConnect();

	private:
		static int CurrentRelateID;
		static string CurrentDate;
		
		string replaceIllegalChar(string& source);
		bool isMatchIllegal(char& c);
		void write(string sql);
		void tran_time(const int relateID);
		void tran_time();
		mysqlpp::Connection conn;
		Json::Value mysql_info;
		std::string db,server,user,pass;
		unsigned int	db_port;
	};
}
