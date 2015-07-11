#include "commom.h"
#include <stdlib.h>
#include <time.h>
#include "boost/date_time/posix_time/posix_time.hpp" 
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <algorithm>

commom::commom(void)
{
	unsigned int seed = (unsigned int)::time(NULL);
	srand(seed);
}


commom::~commom(void)
{
}

std::string	commom::json2string(const Json::Value& json)
{
	std::string str = json.toStyledString();
	str = unshell(str);
	return str;
}

Json::Value commom::string2json(const std::string str)
{
	Json::Value value;
	Json::Reader reader;
	reader.parse(str, value);
	return value;
}

int commom::serverID()
{
	static const int sID = config_ins.get_config_prame("server_id").asInt();
	return sID;
}

std::string	commom::unshell(const std::string &str)
{
	bool in_string = false;
	std::string tmp;
	for (std::string::const_iterator iter = str.begin(); iter != str.end(); iter++)
	{
		char ch = *iter;
		int a = (unsigned)(ch + 1);
		if (Between(a, 0, 256) && isspace(ch) && in_string == false)
		{
			continue;
		}

		tmp.insert(tmp.end(), ch);
		if (ch == '\"')
		{
			in_string = !in_string;
		}
	}
	return tmp;
}

size_t commom::SimpleCnEg(const string& str)
{
	size_t strLength = 0;
	for (unsigned i = 0; i < str.length(); i++){
		char v = str[i];
		unsigned leg = 1;
		if ((v & 0xe0) == 0xc0){ leg = 2; }
		else if ((v & 0xf0) == 0xe0){ leg = 3; }
		else if ((v & 0xf8) == 0xf0)	{ leg = 4; }
		else if ((v & 0xfc) == 0xf8)	{ leg = 5; }
		else if ((v & 0xfe) == 0xfc)	{ leg = 6; }
		i += leg - 1;
		if (leg == 1 && v < 128)++strLength;//简单字符1个字符
		else strLength += 2;//中文强制两个字符
	}
	return strLength;
}

size_t commom::UTF8Len(const string& str)
{
	size_t strLength = 0;
	for (unsigned i = 0; i < str.length(); i++){
		char v = str[i];
		unsigned leg = 1;
		if ((v & 0xe0) == 0xc0){ leg = 2; }
		else if ((v & 0xf0) == 0xe0){ leg = 3; }
		else if ((v & 0xf8) == 0xf0)	{ leg = 4; }
		else if ((v & 0xfc) == 0xf8)	{ leg = 5; }
		else if ((v & 0xfe) == 0xfc)	{ leg = 6; }
		if (leg > 1)i += leg - 1;
		++strLength;
	}
	return strLength;
}

int commom::random()
{
	return rand();
}

void commom::random_array(int a[], const unsigned length)
{
	random_shuffle(a, a + length);
}

//闭合区间 [1 , 2]
int commom::randomBetween(int low, int high)
{
	if (low >= high)
		return low;

	return random() % (high - low + 1) + low;
}

int commom::randomList(const Json::Value &rateList)
{
	static int base = 1000000;
	int rate = random() % 1000 * 1000 + random() % 1000 + 1;
	//rate = (rate + base) % base;
	int low = 1;
	int high = 1;
	for (unsigned i = 0; i < rateList.size(); i++)
	{
		low = high;
		high = high + (int)(rateList[i].asDouble() * base);
		if (Between(rate, low, high))
		{
			return i;
		}
	}
	return 0;
}

bool commom::randomOk(double rate)
{
	if(rate >= 1.00000)
		return true;

	static int base = 1000000;
	int rateInt = (int)(rate * base);

	int v = random() % 1000 * 1000 + random() % 1000;
	return v < rateInt;
}

bool commom::randomOk(int rate)
{
	if(rate <= 0 )return false;
	if(rate >= 10000)return true;
	int v = random() % 1000 * 10 + random() % 10;
	return v < rate;
}

void commom::json_array_remove_null_member( Json::Value& json_array ) const
{
	if(json_array.type()!=Json::arrayValue) return;
	Json::Value new_array = Json::arrayValue;
	for (unsigned i=0;i<json_array.size();++i)
	{
		if(!json_array[i].isNull())
			new_array.append(json_array[i]);
	}
	json_array = new_array;
}
// bool commom::check_four_passin_class(const std::string& val_str, ClassType class_1,ClassType class_2,ClassType class_3,ClassType class_4)
// {	
// 	Json::Value val_list;
// 	if(!struct_json_from_str(val_str, val_list))
// 		return false;
// 
// 	bool result = true;
// 	result &= check_one_passin_class(val_list[0u],class_1);
// 	result &= check_one_passin_class(val_list[1u],class_2);
// 	result &= check_one_passin_class(val_list[1u],class_3);
// 	result &= check_one_passin_class(val_list[1u],class_4);
// 	return result;
// }
// 
// bool commom::check_three_passin_class(const std::string& val_str, ClassType class_1,ClassType class_2,ClassType class_3)
// {
// 	Json::Value val_list;
// 	if(!struct_json_from_str(val_str, val_list))
// 		return false;
// 
// 	bool result = true;
// 	result &= check_one_passin_class(val_list[0u],class_1);
// 	result &= check_one_passin_class(val_list[1u],class_2);
// 	result &= check_one_passin_class(val_list[1u],class_3);
// 	return result;
// }
// 
// bool commom::check_two_passin_class(const std::string& val_str, ClassType class_1,ClassType class_2)
// {
// 	Json::Value val_list;
// 	if(!struct_json_from_str(val_str, val_list))
// 		return false;
// 
// 	bool result = true;
// 	result &= check_one_passin_class(val_list[0u],class_1);
// 	result &= check_one_passin_class(val_list[1u],class_2);
// 	return result;
// }
// 
// bool commom::check_one_passin_class(const std::string& val_str, ClassType class_type)
// {
// 	Json::Value val;
// 	if(!struct_json_from_str(val_str, val))
// 		return false;
// 
// 	return check_one_passin_class(val[0u],class_type);
// }
// 
// bool commom::check_one_passin_class(Json::Value& val_one, ClassType class_type)
// {
// 	if (val_one == Json::Value::null)
// 		return false;
// 
// 	bool result = false;
// 	switch(class_type)
// 	{
// 	case int_value:
// 		result = val_one.isInt();
// 		break;
// 	case string_value:
// 		result = val_one.isString();
// 		break;
// 	default:
// 		result = false;
// 		break;
// 	}
// 	return result;
// }
// 
// bool commom::struct_json_from_str(const std::string& val_utf8_str,Json::Value& resp_json)
// {
// 	try
// 	{
// 		Json::Reader reader;
// 		if(!(reader.parse(val_utf8_str, resp_json)))
// 			return false;
// 	}
// 	catch (std::exception& e)
// 	{
// 		return false;
// 	}
// 
// 	return true;
// }
