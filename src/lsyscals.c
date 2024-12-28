/*
    syscalls
*/


#ifndef __lsyscalls_h__
#define __lsyscalls_h__

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <stdlib.h>
#include <sys/ksys.h>

static int syscalls_createWindow(lua_State *L)
{
    _ksys_create_window(
        luaL_checkinteger(L, 1), 
        luaL_checkinteger(L, 2), 
        luaL_checkinteger(L, 3), 
        luaL_checkinteger(L, 4), 
        luaL_checkstring(L, 5), 
        luaL_checkinteger(L, 6), 
        luaL_checkinteger(L, 7)
    );

    return 0;
}

static int syscalls_defineButton(lua_State *L)
{
    _ksys_define_button(
        luaL_checkinteger(L, 1), 
        luaL_checkinteger(L, 2), 
        luaL_checkinteger(L, 3), 
        luaL_checkinteger(L, 4), 
        luaL_checkinteger(L, 5), 
        luaL_checkinteger(L, 6)
    );

    return 0;
}

static int syscalls_deletebutton(lua_State *L)
{
    _ksys_delete_button(luaL_checkinteger(L, 1));

    return 0;
}

/* static void syscalls_DrawBitmap(lua_State *L)
{
    
    _ksys_draw_bitmap(,);
} */

static int syscalls_drawPixel(lua_State *L)
{
    _ksys_draw_pixel(
        luaL_checkinteger(L, 1),
        luaL_checkinteger(L, 2),
        luaL_checkinteger(L, 3)
    );

    return 0;
}

static int syscalls_threadInfo(lua_State *L)
{
    ksys_thread_t *t = malloc(sizeof(ksys_thread_t));
    
    _ksys_thread_info(t, luaL_checkinteger(L, 1));

    lua_createtable(L, 0, 8);

    lua_pushstring(L, t->name);
    lua_setfield(L, -2, "name");

    lua_pushnumber(L, t->cpu_usage);
    lua_setfield(L, -2, "cpu_usage");

    lua_pushnumber(L, t->memused);
    lua_setfield(L, -2, "memused");
    
    lua_pushnumber(L, t->pid);
    lua_setfield(L, -2, "pid");

    lua_pushnumber(L, t->key_input_mode);
    lua_setfield(L, -2, "key_input_mode");

    lua_pushnumber(L, t->pos_in_window_stack);
    lua_setfield(L, -2, "pos_in_window_stack");

    lua_pushnumber(L, t->slot_num_window_stack);
    lua_setfield(L, -2, "slot_num_window_stack");
    
    lua_pushnumber(L, t->slot_state);
    lua_setfield(L, -2, "slot_state");

    return 1;
}

static int syscalls_waitEvent(lua_State *L)
{
    lua_pushnumber(L, _ksys_wait_event());

    return 1;
}

static int syscalls_checkEvent(lua_State *L)
{
    lua_pushnumber(L, _ksys_check_event());

    return 1;
}

static int syscalls_waitEventTimeout(lua_State *L)
{
    lua_pushnumber(L, _ksys_wait_event_timeout(luaL_checkinteger(L, 1)));

    return 1;
}

static int syscalls_getFreeRam(lua_State *L)
{
    lua_pushnumber(L, _ksys_get_ram_size());

    return 1;
}

static int syscalls_getRamSize(lua_State *L)
{
    lua_pushnumber(L, _ksys_get_full_ram());

    return 1;
}

static int syscalls_shutdown(lua_State *L)
{
    _ksys_shutdown(luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_focusWindow(lua_State *L)
{
    _ksys_focus_window(luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_unfocusWindow(lua_State *L)
{
    _ksys_unfocus_window(luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_getButton(lua_State *L)
{
    lua_pushnumber(L, _ksys_get_button());

    return 1;
}

static int syscalls_screenSize(lua_State *L)
{
    lua_createtable(L, 0, 2);

    ksys_pos_t size = _ksys_screen_size();

    lua_pushnumber(L, size.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, size.y);
    lua_setfield(L, -2, "y");

    return 1;
}

static int syscalls_backgroundSetSize(lua_State *L)
{
    _ksys_bg_set_size(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2));

    return 0;
}

static int syscalls_backgroundPutPixel(lua_State *L)
{
    _ksys_bg_put_pixel(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));

    return 0;
}

static int syscalls_backgroundRedraw(lua_State *L)
{
    _ksys_bg_redraw();

    return 0;
}

static int syscalls_getCPUClock(lua_State *L)
{
    lua_pushnumber(L, _ksys_get_cpu_clock());

    return 1;
}

static int syscalls_getMousePositionScreen(lua_State *L)
{
    ksys_pos_t pos = _ksys_get_mouse_pos(KSYS_MOUSE_SCREEN_POS);

    lua_createtable(L, 0, 2);

    lua_pushnumber(L, pos.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, pos.y);
    lua_setfield(L, -2, "y");

    return 1;
}

static int syscalls_getMousePositionWindow(lua_State *L)
{
    ksys_pos_t pos = _ksys_get_mouse_pos(KSYS_MOUSE_WINDOW_POS);

    lua_createtable(L, 0, 2);

    lua_pushnumber(L, pos.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, pos.y);
    lua_setfield(L, -2, "y");

    return 1;
}

static int sysclas_getMouseButtons(lua_State *L)
{
    uint32_t state = _ksys_get_mouse_buttons();

    lua_createtable(L, 0, 5);

    lua_pushboolean(L, state & KSYS_MOUSE_LBUTTON_PRESSED);
    lua_setfield(L, -2, "LeftButton");

    lua_pushboolean(L, state & KSYS_MOUSE_RBUTTON_PRESSED);
    lua_setfield(L, -2, "RightButton");

    lua_pushboolean(L, state & KSYS_MOUSE_MBUTTON_PRESSED);
    lua_setfield(L, -2, "MButton");

    lua_pushboolean(L, state & KSYS_MOUSE_4BUTTON_PRESSED);
    lua_setfield(L, -2, "Button4");

    lua_pushboolean(L, state & KSYS_MOUSE_5BUTTON_PRESSED);
    lua_setfield(L, -2, "Button5");

    return 1;
}

static int syscalls_drawLine(lua_State *L)
{
    _ksys_draw_line(
        luaL_checkinteger(L, 1),
        luaL_checkinteger(L, 2),
        luaL_checkinteger(L, 3),
        luaL_checkinteger(L, 4),
        luaL_checkinteger(L, 5)
    );

    return 0;
}

static int syscalls_getSystemColors(lua_State *L)
{
    ksys_colors_table_t t;

    _ksys_get_system_colors(&t);

    lua_createtable(L, 0, 10);

    lua_pushnumber(L, t.frame_area);
    lua_setfield(L, -2, "frameArea");

    lua_pushnumber(L, t.grab_bar);
    lua_setfield(L, -2, "grabBar");

    lua_pushnumber(L, t.grab_bar_button);
    lua_setfield(L, -2, "grabBarButton");

    lua_pushnumber(L, t.grab_button_text);
    lua_setfield(L, -2, "grabButtonText");

    lua_pushnumber(L, t.grab_text);
    lua_setfield(L, -2, "grabText");

    lua_pushnumber(L, t.work_area);
    lua_setfield(L, -2, "workArea");

    lua_pushnumber(L, t.work_button);
    lua_setfield(L, -2, "workButton");

    lua_pushnumber(L, t.work_button_text);
    lua_setfield(L, -2, "workButtonText");

    lua_pushnumber(L, t.work_graph);
    lua_setfield(L, -2, "workGraph");

    lua_pushnumber(L, t.work_text);
    lua_setfield(L, -2, "workText");

    return 1;
}

static int syscalls_getSkinHeight(lua_State *L)
{
    lua_pushnumber(L, _ksys_get_skin_height());
    return 1;
}

static int syscalls_setKeyInputMode(lua_State *L)
{
    _ksys_set_key_input_mode(luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_getControlKeyState(lua_State *L)
{
    uint32_t state = _ksys_get_control_key_state();

    lua_createtable(L, 0, 9);

    lua_pushboolean(L, state & KSYS_CONTROL_LSHIFT);
    lua_setfield(L, -2, "LeftShift");

    lua_pushboolean(L, state & KSYS_CONTROL_RSHIFT);
    lua_setfield(L, -2, "RightShift");

    lua_pushboolean(L, state & KSYS_CONTROL_LCTRL);
    lua_setfield(L, -2, "LeftCtrl");

    lua_pushboolean(L, state & KSYS_CONTROL_RCTRL);
    lua_setfield(L, -2, "RightCtrl");

    lua_pushboolean(L, state & KSYS_CONTROL_LALT);
    lua_setfield(L, -2, "LeftAlt");

    lua_pushboolean(L, state & KSYS_CONTROL_RALT);
    lua_setfield(L, -2, "RightAlt");

    lua_pushboolean(L, state & KSYS_CONTROL_CAPS);
    lua_setfield(L, -2, "Caps");

    lua_pushboolean(L, state & KSYS_CONTROL_NUM_LOCK);
    lua_setfield(L, -2, "NumLock");

    lua_pushboolean(L, state & KSYS_CONTROL_NUM_LOCK);
    lua_setfield(L, -2, "ScrolLock");

    return 1;
}

static int syscalls_change_window(lua_State *L)
{
    _ksys_change_window(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));

    return 0;
}

static int syscalls_setWindowTitle(lua_State *L)
{
    _ksys_set_window_title(luaL_checkstring(L, 1));

    return 0;
}

/*
** functions for 'syscalls' library
*/
static const luaL_Reg syscallsLib[] = {
    {"createWindow", syscalls_createWindow},
    {"defineButton", syscalls_defineButton},
    {"deleteButton", syscalls_deletebutton},
    {"drawPixel", syscalls_drawPixel},
    {"waitEvent", syscalls_waitEvent},
    {"checkEvent", syscalls_checkEvent},
    {"waitEventTimeout", syscalls_waitEventTimeout},
    {"shutdown", syscalls_shutdown},
    {"focusWindow", syscalls_focusWindow},
    {"unfocusWindow", syscalls_unfocusWindow},
    {"getButton", syscalls_getButton},
    {"backgroundSetSize", syscalls_backgroundSetSize},
    {"backgroundPutPixel", syscalls_backgroundPutPixel},
    {"backgroundRedraw", syscalls_backgroundRedraw},
    {"getRamSize", syscalls_getRamSize},
    {"getFreeRam", syscalls_getFreeRam},
    {"getCPUClock", syscalls_getCPUClock},
    {"getMousePositionScreen", syscalls_getMousePositionScreen},
    {"getMousePositionWindow", syscalls_getMousePositionWindow},
    {"getSystemColors", syscalls_getSystemColors},
    {"drawLine", syscalls_drawLine},
    {"getSkinHeight", syscalls_getSkinHeight},
    {"setKeyInputMode", syscalls_setKeyInputMode},
    {"getControlKeyState", syscalls_getControlKeyState},
    {NULL, NULL}
};

LUAMOD_API int luaopen_syscalls(lua_State *L)
{
    luaL_newlib(L, syscallsLib);

    return 1;
}

#endif // __lsyscalls_h__