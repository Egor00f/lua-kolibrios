#include "ARP_entry.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ksys.h>


static int syscalls_gcARPEntry(lua_State *L)
{
    free(lua_touserdata(L, 1));

    return 0;
}

static bool syscalls_cmpAPREntry(const struct ARP_entry *entry1, const struct ARP_entry *entry2)
{
    return memcmp(entry1, entry2, sizeof(struct ARP_entry));
}

int syscalls_indexARPEntry(lua_State *L)
{
    struct ARP_entry *entry = (struct ARP_entry *)luaL_checkudata(L, 1, syscalls_ARPEntry_metatable_name);

    const char *index = luaL_checkstring(L, 2);

    if (strcmp(index, "IP") == 0)
    {
        lua_pushinteger(L, entry->IP);
    }
    else if (strcmp(index, "MAC") == 0)
    {
        char str[7];
        memset(str, entry->MAC, 6);
        str[6] = '\n';
        lua_pushstring(L, str);
    }
    else if (strcmp(index, "Status") == 0)
    {
        lua_pushinteger(L, entry->Status);
    }
    else if (strcmp(index, "TTL") == 0)
    {
        lua_pushinteger(L, entry->TTL);
    }
    else
    {
        _ksys_debug_puts("err\n");
    }

    return 1;
}

int syscalls_newindexARPEntry(lua_State *L)
{
    struct ARP_entry *entry = (struct ARP_entry *)luaL_checkudata(L, 1, syscalls_ARPEntry_metatable_name);

    const char *index = luaL_checkstring(L, 2);

    if (strcmp(index, "IP"))
    {
        entry->IP = luaL_checkinteger(L, 3);
    }
    else if (strcmp(index, "MAC"))
    {
        memset(entry->MAC, luaL_checkstring(L, 3), 6);
    }
    else if (strcmp(index, "Status"))
    {
        entry->Status = luaL_checkinteger(L, 3);
    }
    else if (strcmp(index, "TTL"))
    {
        entry->TTL = luaL_checkinteger(L, 3);
    }
    else
    {
        _ksys_debug_puts("err\n");
    }

    return 0;
}

static int syscalls_eqAPREntry(lua_State *L)
{
    lua_pushboolean(
        L, 
        syscalls_cmpAPREntry(
            (struct ARP_entry *)lua_touserdata(L, 1), 
            (struct ARP_entry *)lua_touserdata(L, 2)
        )
    );

    return 1;
}



int syscalls_newARPEntry(lua_State *L)
{
    struct ARP_entry *entry = malloc(sizeof(struct ARP_entry));

    entry->IP = luaL_checkinteger(L, 1);
    memcpy(entry->MAC, luaL_checkstring(L, 2), 6);
    entry->Status = luaL_checkinteger(L, 3);
    entry->TTL = luaL_checkinteger(L, 4);

    syscalls_pushARPEntry(
        L,
        entry);

    return 1;
}

static const luaL_Reg syscalls_ARPEntry_m[] = {
    {"__index", syscalls_indexARPEntry},
    {"__newindex", syscalls_newindexARPEntry},
    {"__eq", syscalls_eqAPREntry},
    {"__gc", syscalls_gcARPEntry},
    {NULL, NULL}};

void syscalls_pushARPEntry(lua_State* L, struct ARP_entry* entry)
{
    *(struct ARP_entry**)lua_newuserdata(L, sizeof(struct ARP_entry)) = entry;

    luaL_newlibtable(L, syscalls_ARPEntry_m);
    luaL_setfuncs(L, syscalls_ARPEntry_m, 0);

    lua_setmetatable(L, -2);
}

static const luaL_Reg syscalls_ARPEntry_lib[] = {
    {"new", syscalls_newARPEntry},
    {NULL, NULL}};

void syscalls_register_ARPEntry(lua_State *L)
{
    luaL_newlib(L, syscalls_ARPEntry_lib);
    lua_setfield(L, -2, syscalls_ARPEntry_name);

    luaL_newlibtable(L, syscalls_ARPEntry_m);
    luaL_setfuncs(L, syscalls_ARPEntry_m, 0);
}
