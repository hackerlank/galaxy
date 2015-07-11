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
	//��Ϊԭ����luabridge��֧�ֶ෵��,
	//�����������˴���һ�����ط��ص�ѡ��
	//ʹ��return�Ļ�ֱ����LuaRef::cast
	//ʹ��mReturn�Ļ�,��ʹ������ķ���
	//ÿ�ε���root��ʱ�����ϴεķ����������
	//�����һֱ����
	//Ŀǰ֧�����12��ͬʱ���ݷ���
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