#include "systemColors.h"
#include <string.h>
#include <stdlib.h>

int syscalls_newSystemColors(lua_State* L)
{
    ksys_colors_table_t* colorsTable = malloc(sizeof(ksys_colors_table_t));

    colorsTable->frame_area = luaL_optinteger(L, 1, 0);
    colorsTable->grab_bar = luaL_optinteger(L, 2, 0);
    colorsTable->grab_bar_button = luaL_optinteger(L, 3, 0);
    colorsTable->grab_button_text = luaL_optinteger(L, 4, 0);
    colorsTable->grab_text = luaL_optinteger(L, 5, 0);
    colorsTable->work_area = luaL_optinteger(L, 6, 0);
    colorsTable->work_button = luaL_optinteger(L, 7, 0);
    colorsTable->work_button_text = luaL_optinteger(L, 8, 0);
    colorsTable->work_graph = luaL_optinteger(L, 9, 0);
    colorsTable->work_text = luaL_optinteger(L, 10, 0);

    syscalls_pushSystemColors(L, colorsTable);

    return 1;
}

static int syscalls_indexSystemColors(lua_State* L)
{
    const ksys_colors_table_t* t = (const ksys_colors_table_t*)luaL_checkudata(L, 1, syscalls_SystemColors_metatable_name);

    const char* index = luaL_checkstring(L, 2);

    if (strcmp("frameArea", index) == 0)
    {
        lua_pushinteger(L, t->frame_area);
    }
    else if (strcmp("grabBar", index) == 0)
    {
        lua_pushinteger(L, t->grab_bar);
    }
    else if (strcmp("grabBarButton", index) == 0)
    {
        lua_pushinteger(L, t->grab_bar_button);
    }
    else if (strcmp("grab_button_text", index) == 0)
    {
        lua_pushinteger(L, t->grab_button_text);
    }
    else if (strcmp("grabText", index) == 0)
    {
        lua_pushinteger(L, t->grab_text);
    }
    else if (strcmp("workArea", index) == 0)
    {
        lua_pushinteger(L, t->work_area);
    }
    else if (strcmp("workButton", index) == 0)
    {
        lua_pushinteger(L, t->work_button);
    }
    else if (strcmp("workButtonText", index) == 0)
    {
        lua_pushinteger(L, t->work_button_text);
    }
    else if (strcmp("workGraph", index) == 0)
    {
        lua_pushinteger(L, t->work_graph);
    }
    else if (strcmp("workText", index) == 0)
    {
        lua_pushinteger(L, t->work_text);
    }
    else
    {
        luaL_error(L, "wrong index: %s", index);
    }

    return 1;
}

static int syscalls_newindexSystemColors(lua_State* L)
{
    ksys_colors_table_t* t = luaL_checkudata(L, 1, syscalls_SystemColors_metatable_name);

    const char* index = luaL_checkstring(L, 2);

    LUA_INTEGER val = luaL_checkinteger(L, 3);

    if (strcmp("frameArea", index) == 0)
    {
        t->frame_area = val;
    }
    else if (strcmp("grabBar", index) == 0)
    {
        t->grab_bar = val;
    }
    else if (strcmp("grabBarButton", index) == 0)
    {
        t->grab_bar_button = val;
    }
    else if (strcmp("grab_button_text", index) == 0)
    {
        t->grab_button_text = val;
    }
    else if (strcmp("grabText", index) == 0)
    {
        t->grab_text = val;
    }
    else if (strcmp("workArea", index) == 0)
    {
        t->work_area = val;
    }
    else if (strcmp("workButton", index) == 0)
    {
        t->work_button = val;
    }
    else if (strcmp("workButtonText", index) == 0)
    {
        t->work_button_text = val;
    }
    else if (strcmp("workGraph", index) == 0)
    {
        t->work_graph = val;
    }
    else if (strcmp("workText", index) == 0)
    {
        t->work_text = val;
    }
    else
    {
        luaL_error(L, "wrong index: %s", index);
    }

    return 0;
}

static int sycalls_eqSystemColors(lua_State* L)
{
    lua_pushboolean(
        L,
        memcmp(
            luaL_checkudata(L, 1, syscalls_SystemColors_metatable_name),
            luaL_checkudata(L, 2, syscalls_SystemColors_metatable_name),
            sizeof(ksys_colors_table_t)
        )
    );

    return 1;
}

static int sycalls_gcSystemColors(lua_State* L)
{
    ksys_colors_table_t* t = luaL_checkudata(L, 1, syscalls_SystemColors_metatable_name);
    free(t);
    return 1;
}

static const luaL_Reg syscalls_SystemColors_m[] = {
    {"__index", syscalls_indexSystemColors},
    {"__newindex", syscalls_newindexSystemColors},
    {"__eq", sycalls_eqSystemColors},
    {NULL, NULL} };

void syscalls_pushSystemColors(lua_State* L, ksys_colors_table_t* colorsTable)
{
    *(ksys_colors_table_t**)lua_newuserdata(L, sizeof(ksys_colors_table_t)) = colorsTable;

    luaL_newlibtable(L, syscalls_SystemColors_m);
    luaL_setfuncs(L, syscalls_SystemColors_m, 0);

    lua_setmetatable(L, -2);
}


static const luaL_Reg syscalls_SystemColors_lib[] = {
    {"new", syscalls_newSystemColors},
    {NULL, NULL} };

void syscalls_register_SystemColors(lua_State* L)
{
    luaL_newlib(L, syscalls_SystemColors_lib);
    lua_setfield(L, -2, syscalls_SystemColors_name);

    luaL_newlibtable(L, syscalls_SystemColors_m);
    luaL_setfuncs(L, syscalls_SystemColors_m, 0);
}
