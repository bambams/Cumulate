#include "script_manager.h"
#include "lua_player.h"
#include <iostream>

Script_manager::Script_manager()
{
	state = luaL_newstate();
	luaL_openlibs (state);

	lua_newtable (state);
	register_player(state);
	lua_setglobal (state, "cumulate");
}

Script_manager::~Script_manager()
{
	lua_close(state);
}

void Script_manager::Run_string(const char* s)
{
	int r = luaL_dostring (state, s);
	if(r)
	{
		const char *e = luaL_checkstring (state, 1);
		std::cout<<"Lua error "<< r <<": "<< e <<std::endl;
		lua_pop(state, 1);
	}
}

void Script_manager::Set_player(Player* player)
{
	push_player(state, player);
	lua_setglobal (state, "player");
}
