#pragma once

#include <lua.hpp>
#include "luabridge/LuaBridge.h"
#include <exception>
#include <Glog.h>
#include <string>
using namespace std;

#define SafeCallLua(ACTION)\
	try\
				{\
		ACTION;\
				}catch(std::exception& e)\
				{\
		LogW << "call lua failed ..." << e.what() << LogEnd;\
				}

#define CallAndGetInt(ACTION, VALUE)\
int VALUE = 0;\
try\
{\
VALUE = ACTION.cast<int>(); \
}catch (std::exception& e)\
{\
LogW << "call lua failed ..." << e.what() << LogEnd; \
}

#define CallAndGetUInt(ACTION, VALUE)\
int VALUE = 0;\
try\
{\
VALUE = ACTION.cast<unsigned>(); \
}catch (std::exception& e)\
{\
LogW << "call lua failed ..." << e.what() << LogEnd; \
}

#define CallAndGetBool(ACTION, VALUE)\
bool VALUE = 0;\
try\
{\
VALUE = ACTION.cast<bool>(); \
}catch (std::exception& e)\
{\
LogW << "call lua failed ..." << e.what() << LogEnd; \
}

#define CallAndGetDouble(ACTION, VALUE)\
double VALUE = 0.0;\
try\
{\
VALUE = ACTION.cast<double>(); \
}catch (std::exception& e)\
{\
LogW << "call lua failed ..." << e.what() << LogEnd; \
}

#define CallAndGetString(ACTION, VALUE)\
string VALUE = "";\
try\
{\
VALUE = ACTION.cast<string>(); \
}catch (std::exception& e)\
{\
LogW << "call lua failed ..." << e.what() << LogEnd; \
}

class mLua
{
public:
	static void initialLua()
	{
		L = luaL_newstate();
		luaL_openlibs(L);
		RegAndRun();
	}
	static void reloadLua()
	{
		endLua();
		initialLua();
	}
	static luabridge::LuaRef Root(const char* name)
	{
		return luabridge::getGlobal(L, name);
	}
	static void endLua()
	{
		lua_close(L);
	}
	//因为原生的luabridge不支持多返回,
	//所以这里做了处理一个多重返回的选项
	//使用return的话直接用LuaRef::cast
	//使用mReturn的话,请使用下面的方法
	//每次调用root的时候会吧上次的返回数据清除
	//否则会一直保留
	//目前支持最多12个同时数据返回
	//index 1 ~ 12
	template<typename TYPE>
	static TYPE PickMultiResult(const unsigned index);
private:
	static luabridge::Namespace Space()
	{
		return luabridge::getGlobalNamespace(L);
	}
	static void RegAndRun();
	static lua_State* L;
};