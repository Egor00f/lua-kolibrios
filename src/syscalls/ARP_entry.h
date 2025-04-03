#ifndef _SYSCALLS_ARP_ENTRY_
#define _SYSCALLS_ARP_ENTRY

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdint.h>


struct ARP_entry
{
    uint32_t IP;
    char MAC[6];
    uint16_t Status;
    uint16_t TTL;
};

#define syscalls_ARPEntry_name "ARPEntry"
#define syscalls_ARPEntry_metatable_name syscalls_ARPEntry_name ".mt"

/*
 * Create ARPEntry
 */
void syscalls_pushARPEntry(lua_State *L, struct ARP_entry *entry);

/*
 * shell of syscalls_createARPEntry for lua
 */
int syscalls_newARPEntry(lua_State *L);
int syscalls_indexARPEntry(lua_State *L);
int syscalls_newindexARPEntry(lua_State *L);

void syscalls_register_ARPEntry(lua_State *L);

#endif
