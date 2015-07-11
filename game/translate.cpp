// #include "translate.h"
// 
// namespace gg
// {
// #define TRANSMAP boost::unordered_map<string, string>
// 
// 	TRANSMAP TransLate::translateMap;
// 
// 	const static string translateFile = "./instance/Translate/translate.json";
// 	void TransLate::initTranslateFile()
// 	{
// 		translateMap.clear();
// 		Json::Value transJson = na::file_system::load_jsonfile_val(translateFile);
// 		for (Json::Value::iterator it = transJson.begin(); it != transJson.end(); ++it)
// 		{
// 			string key = it.key().asString();
// 			string value = (*it).asString();
// 			translateMap[key] = value;
// 		}
// 	}
// 
// 	string TransLate::getTranslate(string key)
// 	{
// 		TRANSMAP::iterator it = translateMap.find(key);
// 		if(it == translateMap.end())return "";
// 		return it->second;
// 	}
// }