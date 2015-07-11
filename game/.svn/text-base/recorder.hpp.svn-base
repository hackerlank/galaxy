#pragma once

#include "record_system.h"

namespace gg
{
	template<typename T>
	inline string toString(T t)
	{
		return boost::lexical_cast<string, T>(t);
	}

	typedef vector<string> FieldList;

	template<typename T0>
	inline void LogTemplate(const string& db, playerDataPtr ptr, int tag, T0 preV, T0 nowV)
	{
		DoLog(db, ptr, toString(preV), toString(nowV), FieldList(), tag);
	}

	template<typename T0, typename T1>
	inline void LogTemplate(const string& db, playerDataPtr ptr, int tag, T0 preV, T0 nowV, T1 t1)
	{
		FieldList field_list;
		field_list.push_back(toString(t1));
		DoLog(db, ptr, toString(preV), toString(nowV), field_list, tag);
	}

	template<typename T0, typename T1, typename T2>
	inline void LogTemplate(const string& db, playerDataPtr ptr, int tag, T0 preV, T0 nowV, T1 t1, T2 t2)
	{
		FieldList field_list;
		field_list.push_back(toString(t1));
		field_list.push_back(toString(t2));
		DoLog(db, ptr, toString(preV), toString(nowV), field_list, tag);
	}

	template<typename T0, typename T1, typename T2, typename T3>
	inline void LogTemplate(const string& db, playerDataPtr ptr, int tag, T0 preV, T0 nowV, T1 t1, T2 t2, T3 t3)
	{
		FieldList field_list;
		field_list.push_back(toString(t1));
		field_list.push_back(toString(t2));
		field_list.push_back(toString(t3));
		DoLog(db, ptr, toString(preV), toString(nowV), field_list, tag);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	inline void LogTemplate(const string& db, playerDataPtr ptr, int tag, T0 preV, T0 nowV, T1 t1, T2 t2, T3 t3, T4 t4)
	{
		FieldList field_list;
		field_list.push_back(toString(t1));
		field_list.push_back(toString(t2));
		field_list.push_back(toString(t3));
		field_list.push_back(toString(t4));
		DoLog(db, ptr, toString(preV), toString(nowV), field_list, tag);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
	inline void LogTemplate(const string& db, playerDataPtr ptr, int tag, T0 preV, T0 nowV, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
	{
		FieldList field_list;
		field_list.push_back(toString(t1));
		field_list.push_back(toString(t2));
		field_list.push_back(toString(t3));
		field_list.push_back(toString(t4));
		field_list.push_back(toString(t5));
		DoLog(db, ptr, toString(preV), toString(nowV), field_list, tag);
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	inline void LogTemplate(const string& db, playerDataPtr ptr, int tag, T0 preV, T0 nowV, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
	{
		FieldList field_list;
		field_list.push_back(toString(t1));
		field_list.push_back(toString(t2));
		field_list.push_back(toString(t3));
		field_list.push_back(toString(t4));
		field_list.push_back(toString(t5));
		field_list.push_back(toString(t6));
		DoLog(db, ptr, toString(preV), toString(nowV), field_list, tag);
	}

	inline void DoLog(const string& db, playerDataPtr ptr, const string& preV, const string& nowV, const FieldList& field_list, int tag)
	{
		if (ptr == playerDataPtr())
			StreamLog::Log(db, -1, "", -1, preV, nowV, field_list, tag);
		else
			StreamLog::Log(db, ptr, preV, nowV, field_list, tag);
	}
}