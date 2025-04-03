#ifndef _SYSCALLS_SYSTEMCOLORS_H_
#define _SYSCALLS_SYSTEMCOLORS_H_

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sys/ksys.h>


#define syscalls_SystemColors_name "SystemColors"
#define syscalls_SystemColors_metatable_name syscalls_SystemColors_name ".mt"

int syscalls_newSystemColors(lua_State *L);

void syscalls_pushSystemColors(lua_State *L, ksys_colors_table_t *t);

void syscalls_register_SystemColors(lua_State *L);

#endif // _SYSCALLS_SYSTEMCOLORS_H_
