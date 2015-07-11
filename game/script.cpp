#include "script.hpp"
#include <boost/algorithm/string.hpp>
#include "time_helper.h"

extern void luaopen_lua_extensions(lua_State *L);


lua_State* mLua::L;

class LuaLog
{
public:
	static void LI(const char* val)
	{
		LogI << val << LogEnd;
	}
	static void LW(const char* val)
	{
		LogW << val << LogEnd;
	}
	static void LE(const char* val)
	{
		LogE << val << LogEnd;
	}
};

inline string AdjustPath(string str)
{
#ifdef WIN32
	str = boost::replace_all_copy(str, "/", "\\");
#else
	str = boost::replace_all_copy(str, "\\", "/");
#endif
	return str;
}

void mLua::RegAndRun()
{
	//зЂВс
	Space().
		beginNamespace("CPP").
		addFunction("lw", &LuaLog::LW).
		addFunction("le", &LuaLog::LE).
		addFunction("li", &LuaLog::LI).
		addFunction("tm", &na::time_helper::get_current_time).
		endNamespace();

	luaopen_lua_extensions(L);

	const static string mpath = AdjustPath("./lua/begin.lua");
	if (luaL_dofile(L, mpath.c_str()))
	{
		LogW << lua_tostring(L, -1) << LogEnd;
		lua_pop(L, 1);
		abort();
	}
	SafeCallLua(Root("LuaMain")());
}

template<>
int mLua::PickMultiResult<int>(const unsigned index)
{
	int val = 0;
	try
	{
		val = Root("mGet")(index).cast< int >();
	}
	catch (std::exception& e)
	{
		LogW << "try to get integer failed ..." << e.what() << LogEnd;
	}
	return val;
}

template<>
unsigned mLua::PickMultiResult<unsigned>(const unsigned index)
{
	int val = 0;
	try
	{
		val = Root("mGet")(index).cast< unsigned >();
	}
	catch (std::exception& e)
	{
		LogW << "try to get integer failed ..." << e.what() << LogEnd;
	}
	return val;
}

template<>
bool mLua::PickMultiResult<bool>(const unsigned index)
{
	bool val = 0;
	try
	{
		val = Root("mGet")(index).cast< bool >();
	}
	catch (std::exception& e)
	{
		LogW << "try to get integer failed ..." << e.what() << LogEnd;
	}
	return val;
}

template<>
double mLua::PickMultiResult<double>(const unsigned index)
{
	double val = 0.0;
	try
	{
		val = Root("mGet")(index).cast< double >();
	}
	catch (std::exception& e)
	{
		LogW << "try to get double failed ..." << e.what() << LogEnd;
	}
	return val;
}

template<>
float mLua::PickMultiResult<float>(const unsigned index)
{
	float val = 0.0f;
	try
	{
		val = Root("mGet")(index).cast< float >();
	}
	catch (std::exception& e)
	{
		LogW << "try to get float failed ..." << e.what() << LogEnd;
	}
	return val;
}

template<>
string mLua::PickMultiResult<string>(const unsigned index)
{
	string val = "";
	try
	{
		val = Root("mGet")(index).cast< string >();
	}
	catch (std::exception& e)
	{
		LogW << "try to get string failed ..." << e.what() << LogEnd;
	}
	return val;
}
