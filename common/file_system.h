#pragma once
#include <string>
#include <vector>
#include <map>
#include <boost/filesystem.hpp>
#include "json/json.h"
using namespace std;
namespace na
{
	namespace file_system
	{
		typedef vector<std::string>			json_file_vec;
		typedef vector<Json::Value>		json_value_vec;
		typedef map<int, Json::Value>	json_value_map;
		std::string load_jsonfile(const std::string& file_name);
		Json::Value load_jsonfile_val(const std::string& file_name);
		int load_jsonfiles_from_dir(const std::string& files_path,json_file_vec& vec );
		int load_jsonfiles_from_dir(const std::string& files_path,json_value_vec& vec );
		int load_jsonfiles_from_dir(const std::string& files_path,json_value_map& m );

		void try_create_directory(const string dir);

		int write_file(const std::string& file_path, const std::string& content_str);
	}
}



