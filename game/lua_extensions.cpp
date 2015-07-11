#include <lua.hpp>
extern int luaopen_cjson(lua_State *l);
extern int luaopen_cjson_safe(lua_State *l);

static luaL_Reg luax_exts[] = {
	{ "cjson", luaopen_cjson_safe },
	{ NULL, NULL }
};

void luaopen_lua_extensions(lua_State *L)
{
	// load extensions
	luaL_Reg* lib = luax_exts;
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	for (; lib->func; lib++)
	{
		lua_pushcfunction(L, lib->func);
		lua_setfield(L, -2, lib->name);
	}
	lua_pop(L, 2);

	// load extensions script
	//luaopen_socket_scripts(L);
}

