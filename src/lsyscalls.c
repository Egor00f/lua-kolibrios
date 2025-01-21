/*
    syscalls
*/

#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <sys/ksys.h>

/*
    Режим ввода с клавиатуры

    функции для получения текущего режима нет, поэтому выкручиваемся таким образом
*/
static ksys_key_input_mode_t syscalls_KeyInputState = KSYS_KEY_INPUT_MODE_ASCII;

/*
    Кеш размера экрана.

    Нужно для того чтобы не вызывать систменое прерывание лишний раз (другие функции тоже используют это значение)

    Сомневаюсь что в размер экрана в колибри вообще может меняться без перезагрузки

    обновляется функцией syscalls_updateScreenSize
*/
static ksys_pos_t syscalls_screenSizeCache = {0};


/*
    функции для того чтобы возвращаемые значения функций были болле-мение едиообразны
*/


inline void syscalls_ReturnIntegerOrNil(LUA_INTEGER value, lua_State *L)
{
    if (value == -1)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushinteger(L, value);
    }
}

inline void syscalls_ReturnIntegerValueOrNil(LUA_INTEGER cond, LUA_INTEGER value, lua_State *L)
{
    if (cond == -1)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushinteger(L, value);
    }
}

inline void syscalls_ReturnTrueOrNil(LUA_INTEGER value, lua_State *L)
{
    if (value == -1)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushboolean(L, true);
    }
}

inline void syscalls_ReturnStringOrNil(LUA_INTEGER cond, const char *value, lua_State *L)
{
    if (value == -1)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushstring(L, value);
    }
}


static int syscalls_createWindow(lua_State *L)
{
    uint32_t x = luaL_checkinteger(L, 1);
    uint32_t y = luaL_checkinteger(L, 2);
    uint32_t w = luaL_checkinteger(L, 3);
    uint32_t h = luaL_checkinteger(L, 4);
    ksys_color_t workcolor = luaL_checkinteger(L, 6);
    uint32_t style = luaL_checkinteger(L, 7);

    if (style == 0 || style == 2)
    {
        uint32_t borderColor = luaL_checkinteger(L, 5);
        uint32_t titleColor = luaL_checkinteger(L, 8);
        asm_inline(
            "int $0x40" ::"a"(0),
            "b"((x << 16) | ((w - 1) & 0xFFFF)),
            "c"((y << 16) | ((h - 1) & 0xFFFF)),
            "d"((style << 24) | (workcolor & 0xFFFFFF)),
            "D"(borderColor),
            "S"(titleColor)
            : "memory");
    }
    else
    {
        _ksys_create_window(
            x, y, w, h,
            luaL_checkstring(L, 5),
            workcolor,
            style);
    }

    return 0;
}

static int syscalls_changeWindow(lua_State *L)
{
    _ksys_change_window(
        luaL_checkinteger(L, 1),
        luaL_checkinteger(L, 2),
        luaL_checkinteger(L, 3),
        luaL_checkinteger(L, 4));

    return 0;
}

static int syscalls_setWindowTitle(lua_State *L)
{
    _ksys_set_window_title(luaL_checkstring(L, 1));

    return 0;
}

static int syscalls_startRedraw(lua_State *L)
{
    _ksys_start_draw();

    return 0;
}

static int syscalls_endRedraw(lua_State *L)
{
    _ksys_end_draw();

    return 0;
}

static int syscalls_SetSkin(lua_State *L)
{
    uint ret;
    asm_inline(
        "int $0x40"
        : "=a"(ret)
        : "a"(48), "b"(8), "c"(luaL_checkstring(L, 1)));

    lua_pushinteger(L, ret);

    return 1;
}

static int syscalls_GetSkinTilteArea(lua_State *L)
{
    ksys_pos_t leftRight, topBottom;

    asm_inline(
        "int $0x40"
        : "=a"(leftRight), "=b"(topBottom)
        : "a"(48), "b"(7));

    lua_createtable(L, 0, 4);

    lua_pushinteger(L, leftRight.x);
    lua_setfield(L, -2, "Left");

    lua_pushinteger(L, leftRight.y);
    lua_setfield(L, -2, "Right");

    lua_pushinteger(L, topBottom.x);
    lua_setfield(L, -2, "Top");

    lua_pushinteger(L, topBottom.y);
    lua_setfield(L, -2, "Bottom");

    return 1;
}

static int syscalls_SetWorkArea(lua_State *L)
{
    uint32_t left = luaL_checkinteger(L, 1);
    uint32_t top = luaL_checkinteger(L, 2);
    uint32_t right = luaL_checkinteger(L, 3);
    uint32_t bottom = luaL_checkinteger(L, 4);

    asm_inline(
        "int $0x40" ::"a"(48), "b"(6), "c"(left * 65536 + right), "d"(top * 65536 + bottom));

    return 0;
}

static int syscalls_GetWorkArea(lua_State *L)
{
    ksys_pos_t leftlright;
    ksys_pos_t toplbottom;

    asm_inline(
        "int $0x40"
        :"=a"(leftlright), "=b"(toplbottom)
        :"a"(48), "b"(5)
    );

    lua_createtable(L, 0, 4);

    lua_pushinteger(L, leftlright.x);
    lua_setfield(L, -2, "Left");

    lua_pushinteger(L, leftlright.y);
    lua_setfield(L, -2, "Right");

    lua_pushinteger(L, toplbottom.x);
    lua_setfield(L, -2, "Top");

    lua_pushinteger(L, toplbottom.y);
    lua_setfield(L, -2, "Bottom");

    return 1;
}

static int syscalls_defineButton(lua_State *L)
{
    _ksys_define_button(
        luaL_checkinteger(L, 1),
        luaL_checkinteger(L, 2),
        luaL_checkinteger(L, 3),
        luaL_checkinteger(L, 4),
        luaL_checkinteger(L, 5),
        luaL_checkinteger(L, 6));

    return 0;
}

static int syscalls_deleteButton(lua_State *L)
{
    _ksys_delete_button(luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_SetButtonsStyle(lua_State *L)
{
    uint32_t style = luaL_checkinteger(L, 1);
    asm_inline(
        "int $0x40" ::"a"(48), "b"(1), "c"(style));
    
    asm_inline(
        "int $0x40" ::"a"(48), "b"(0), "c"(0));

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
        luaL_checkinteger(L, 3));

    return 0;
}

static int syscalls_threadInfo(lua_State *L)
{
    ksys_thread_t t;

    _ksys_thread_info(&t, luaL_checkinteger(L, 1));

    lua_createtable(L, 0, 13);

    lua_pushstring(L, t.name);
    lua_setfield(L, -2, "name");

    lua_pushinteger(L, t.cpu_usage);
    lua_setfield(L, -2, "cpu_usage");

    lua_pushinteger(L, t.memused);
    lua_setfield(L, -2, "memused");

    lua_pushinteger(L, t.pid);
    lua_setfield(L, -2, "pid");

    lua_pushinteger(L, t.key_input_mode);
    lua_setfield(L, -2, "keyInputMode");

    lua_pushinteger(L, t.pos_in_window_stack);
    lua_setfield(L, -2, "posInWindowStack");

    lua_pushinteger(L, t.slot_num_window_stack);
    lua_setfield(L, -2, "slotNumWindowStack");

    lua_pushinteger(L, t.slot_state);
    lua_setfield(L, -2, "slotState");

    lua_pushinteger(L, t.window_state);
    lua_setfield(L, -2, "windowState");

    lua_pushinteger(L, t.winx_size);
    lua_setfield(L, -2, "winXSize");

    lua_pushinteger(L, t.winy_size);
    lua_setfield(L, -2, "winYSize");

    lua_pushinteger(L, t.winx_start);
    lua_setfield(L, -2, "winXPos");

    lua_pushinteger(L, t.winy_start);
    lua_setfield(L, -2, "winYPos");

    return 1;
}

static int syscalls_KillBySlot(lua_State *L)
{

    _ksys_kill_by_slot(
        luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_setEventMask(lua_State *L)
{
    _ksys_set_event_mask(luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_waitEvent(lua_State *L)
{
    lua_pushinteger(L, _ksys_wait_event());

    return 1;
}

static int syscalls_checkEvent(lua_State *L)
{
    lua_pushinteger(L, _ksys_check_event());

    return 1;
}

static int syscalls_waitEventTimeout(lua_State *L)
{
    lua_pushinteger(L, _ksys_wait_event_timeout(luaL_checkinteger(L, 1)));

    return 1;
}

static int syscalls_getFreeRam(lua_State *L)
{
    lua_pushinteger(L, _ksys_get_ram_size());

    return 1;
}

static int syscalls_getRamSize(lua_State *L)
{
    lua_pushinteger(L, _ksys_get_full_ram());

    return 1;
}

static int syscalls_shutdownPowerOff(lua_State *L)
{
    _ksys_shutdown(KSYS_SHD_POWEROFF);

    return 0;
}

static int syscalls_shutdownReboot(lua_State *L)
{
    _ksys_shutdown(KSYS_SHD_REBOOT);

    return 0;
}

static int syscalls_shutdownRestartKRN(lua_State *L)
{
    _ksys_shutdown(KSYS_SHD_RESTART_KRN);

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
    uint32_t val;

    asm_inline(
        "int $0x40"
        : "=a"(val)
        : "a"(17));

    if (val != 0)
    {
        lua_pushinteger(L, val >> 8);

        lua_pushinteger(L, val & 0xFF);
    }
    else
    {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 2;
}

static void syscalls_updateScreenSize()
{
    syscalls_screenSizeCache = _ksys_screen_size();
}

static int syscalls_screenSize(lua_State *L)
{
    syscalls_updateScreenSize();

    lua_createtable(L, 0, 2);

    lua_pushinteger(L, syscalls_screenSizeCache.x);
    lua_setfield(L, -2, "x");

    lua_pushinteger(L, syscalls_screenSizeCache.y);
    lua_setfield(L, -2, "y");

    return 1;
}

/* 
    Backgound
*/

static int syscalls_backgroundSetSize(lua_State *L)
{
    _ksys_bg_set_size(
        luaL_checkinteger(L, 1),
        luaL_checkinteger(L, 2));

    return 0;
}

static int syscalls_backgroundPutPixel(lua_State *L)
{
    _ksys_bg_put_pixel(
        luaL_checkinteger(L, 1),
        luaL_checkinteger(L, 2),
        luaL_checkinteger(L, 3),
        luaL_checkinteger(L, 4));

    return 0;
}

static int syscalls_backgroundRedraw(lua_State *L)
{
    _ksys_bg_redraw();

    return 0;
}

static int syscalls_getCPUClock(lua_State *L)
{
    lua_pushinteger(L, _ksys_get_cpu_clock());

    return 1;
}

static int syscalls_drawLine(lua_State *L)
{
    _ksys_draw_line(
        luaL_checkinteger(L, 1),
        luaL_checkinteger(L, 2),
        luaL_checkinteger(L, 3),
        luaL_checkinteger(L, 4),
        luaL_checkinteger(L, 5));

    return 0;
}

enum TextScale
{
    TextScale_SIZE_6x9 = 1,    // 1x 6x9
    TextScale_SIZE_8x16,   // 1x 8x16
    TextScale_SIZE_12x18,  // 2x 6x9
    TextScale_SIZE_16x32,  // 2x 8x16
    TextScale_SIZE_18x27,  // 3x 6x9
    TextScale_SIZE_24x36,  // 4x 6x9
    TextScale_SIZE_24x48,  // 3x 8x16
    TextScale_SIZE_30x45,  // 5x 6x9
    TextScale_SIZE_32x64,  // 4x 8x16
    TextScale_SIZE_36x54,  // 6x 6x9
    TextScale_SIZE_40x80,  // 5x 8x16
    TextScale_SIZE_42x63,  // 7x 6x9
    TextScale_SIZE_48x72,  // 8x 6x9
    TextScale_SIZE_48x96,  // 6x 8x16
    TextScale_SIZE_56x112, // 7x 8x16
    TextScale_SIZE_64x128   // 8x 8x16
};

static void syscall_drawText(const char *text, uint32_t x, uint32_t y, ksys_color_t color, enum TextScale size, uint32_t len, bool fillBackground, ksys_color_t backgroundColor)
{
    enum DrawTextEncoding
    {
        cp866_6x9 = 0,
        cp866_8x16 = 1,
        utf8 = 3,
        utf16= 4
    };

    enum scale
    {
        scale_x1 = 0,
        scale_x2 = 1,
        scale_x3 = 2,
        scale_x4 = 3,
        scale_x5 = 4,
        scale_x6 = 5,
        scale_x7 = 6,
        scale_x8 = 7
    };

    color &= 0x00FFFFFF;

    color |= (fillBackground << 30);

    switch(size)
    {
    case TextScale_SIZE_6x9:
        color |= (cp866_8x16 << 28) | (scale_x1 << 24);
        break;
    case TextScale_SIZE_8x16:
        color |= (cp866_8x16 << 28);
        break;
    case TextScale_SIZE_12x18:
        color |= (cp866_6x9 << 28) | (scale_x2 << 24);
        break;
    case TextScale_SIZE_16x32:
        color |= (cp866_8x16 << 28) | (scale_x2 << 24);
        break;
    case TextScale_SIZE_18x27:
        color |= (cp866_6x9 << 28) | (scale_x3 << 24);
        break;
    case TextScale_SIZE_24x36:
        color |= (cp866_6x9 << 28) | (scale_x4 << 24);
        break;
    case TextScale_SIZE_24x48:
        color |= (cp866_8x16 << 28) | (scale_x3 << 24);
        break;
    case TextScale_SIZE_30x45:
        color |= (cp866_6x9 << 28) | (scale_x5 << 24);
        break;
    case TextScale_SIZE_36x54:
        color |= (cp866_6x9 << 28) | (scale_x6 << 24);
        break;
    case TextScale_SIZE_40x80:
        color |= (cp866_8x16 << 28) | (scale_x5 << 24);
        break;
    case TextScale_SIZE_42x63:
        color |= (cp866_6x9 << 28) | (scale_x7 << 24);
        break;
    case TextScale_SIZE_48x72:
        color |= (cp866_6x9 << 28) | (scale_x8 << 24);
        break;
    case TextScale_SIZE_48x96:
        color |= (cp866_8x16 << 28) | (scale_x6 << 24);
        break;
    case TextScale_SIZE_56x112:
        color |= (cp866_8x16 << 28) | (scale_x7 << 24);
        break;
    case TextScale_SIZE_64x128:
        color |= (cp866_8x16 << 28) | (scale_x8 << 24);
        break;
    default:
        break;
    };

    if (len > 0)
        color |= (1 << 31);

    asm_inline(
        "int $0x40" ::"a"(4),
        "b"((x << 16) | y),
        "c"(color),
        "d"(text),
        "S"(len),
        "D"(backgroundColor));
}

static int syscalls_drawText(lua_State *L)
{
    syscall_drawText(
        luaL_checkstring(L, 1),
        luaL_checkinteger(L, 2),
        luaL_checkinteger(L, 3),
        luaL_checkinteger(L, 4),
        luaL_optinteger(L, 5, TextScale_SIZE_8x16),
        luaL_optinteger(L, 6, 0),
        luaL_optinteger(L, 7, 0),
        luaL_optinteger(L, 8, 0));

    return 0;
}

static int syscalls_drawRectangle(lua_State *L)
{
    _ksys_draw_bar(
        luaL_checkinteger(L, 1),
        luaL_checkinteger(L, 2),
        luaL_checkinteger(L, 3),
        luaL_checkinteger(L, 4),
        luaL_checkinteger(L, 5));

    return 0;
}

static int syscalls_ReadPoint(lua_State *L)
{
    ksys_color_t color;

    if (syscalls_screenSizeCache.val == 0)
        syscalls_updateScreenSize();

    uint32_t x = luaL_checkinteger(L, 1);
    uint32_t y = luaL_checkinteger(L, 2);

    asm_inline(
        "int $ 0x40"
        : "=a"(color)
        : "a"(35), "b"(x * syscalls_screenSizeCache.x + y));

    lua_pushnumber(L, color);

    return 1;
}

static int syscalls_getSystemColors(lua_State *L)
{
    ksys_colors_table_t t;

    _ksys_get_system_colors(&t);

    lua_createtable(L, 0, 10);

    lua_pushinteger(L, t.frame_area);
    lua_setfield(L, -2, "frameArea");

    lua_pushinteger(L, t.grab_bar);
    lua_setfield(L, -2, "grabBar");

    lua_pushinteger(L, t.grab_bar_button);
    lua_setfield(L, -2, "grabBarButton");

    lua_pushinteger(L, t.grab_button_text);
    lua_setfield(L, -2, "grabButtonText");

    lua_pushinteger(L, t.grab_text);
    lua_setfield(L, -2, "grabText");

    lua_pushinteger(L, t.work_area);
    lua_setfield(L, -2, "workArea");

    lua_pushinteger(L, t.work_button);
    lua_setfield(L, -2, "workButton");

    lua_pushinteger(L, t.work_button_text);
    lua_setfield(L, -2, "workButtonText");

    lua_pushinteger(L, t.work_graph);
    lua_setfield(L, -2, "workGraph");

    lua_pushinteger(L, t.work_text);
    lua_setfield(L, -2, "workText");

    return 1;
}

static int syscalls_SetSystemColors(lua_State *L)
{
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "frameArea");
    lua_setfield(L, 1, "grabBar");
    lua_setfield(L, 1, "grabBarButton");
    lua_setfield(L, 1, "grabButtonText");
    lua_setfield(L, 1, "grabText");
    lua_setfield(L, 1, "workArea");
    lua_setfield(L, 1, "workButton");
    lua_setfield(L, 1, "workButtonText");
    lua_setfield(L, 1, "workGraph");
    lua_setfield(L, 1, "workText");

    ksys_colors_table_t t;

    t.frame_area = luaL_checkinteger(L, -10);
    t.grab_bar = luaL_checkinteger(L, -9);
    t.grab_bar_button = luaL_checkinteger(L, -8);
    t.grab_button_text = luaL_checkinteger(L, -7);
    t.grab_text = luaL_checkinteger(L, -6);
    t.work_area = luaL_checkinteger(L, -5);
    t.work_button = luaL_checkinteger(L, -4);
    t.work_button_text = luaL_checkinteger(L, -3);
    t.work_graph = luaL_checkinteger(L, -2);
    t.work_text = luaL_checkinteger(L, -1);

    asm_inline(
        "int $0x40" ::"a"(48), "b"(2), "c"(&t), "d"(40));

    asm_inline(
        "int $0x40" ::"a"(48), "b"(0), "c"(0));

    return 0;
}

static int syscalls_getSkinHeight(lua_State *L)
{
    lua_pushinteger(L, _ksys_get_skin_height());

    return 1;
}

static int syscalls_setKeyInputMode(lua_State *L)
{
    syscalls_KeyInputState = luaL_checkinteger(L, 1);
    _ksys_set_key_input_mode(syscalls_KeyInputState);

    return 0;
}

static int syscalls_getKeyInputMode(lua_State *L)
{
    lua_pushinteger(L, syscalls_KeyInputState);

    return 1;
}

static int syscalls_getKey(lua_State *L)
{
    ksys_oskey_t a = _ksys_get_key();

    if (a.val == 1)
    {
        lua_pushnil(L);
    }
    else if (a.state == 0)
    {
        if (syscalls_KeyInputState == KSYS_KEY_INPUT_MODE_ASCII)
        {
            char s[2];
            s[0] = a.code;
            s[1] = '\n';
            lua_pushstring(L, s);
        }
        else
        {
            lua_pushinteger(L, a.code);
        }
    }

    if (a.state == 2)
    {
        lua_pushinteger(L, a.code);
    }
    else
    {
        lua_pushnil(L);
    }

    return 2;
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

    lua_pushboolean(L, state & KSYS_CONTROL_SCROLL_LOCK);
    lua_setfield(L, -2, "ScrollLock");

    return 1;
}

static int syscalls_SetHotkey(lua_State *L)
{
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    if (_ksys_set_sys_hotkey(
            lua_getfield(L, -2, "Scancode"),
            (lua_getfield(L, -2, "Shift")) |
                (lua_getfield(L, -2, "Ctrl") << 4) |
                (lua_getfield(L, -2, "Alt") << 8)))
    {
        luaL_pushfail(L);
    }

    return 1;
}

static int syscalls_DeleteHotkey(lua_State *L)
{
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    if (_ksys_del_sys_hotkey(
            lua_getfield(L, -2, "Scancode"),
            lua_getfield(L, -2, "Shift") |
                (lua_getfield(L, -2, "Ctrl") << 4) |
                (lua_getfield(L, -2, "Alt") << 8)))
    {
        luaL_pushfail(L);
    }

    return 1;
}

static int syscalls_LockNormalInput(lua_State *L)
{
    asm_inline(
        "int $0x40" ::"a"(66), "b"(6));

    return 0;
}

static int syscalls_UnlockNormalInput(lua_State *L)
{
    asm_inline(
        "int $0x40" ::"a"(66), "b"(7));

    return 0;
}

/*
    Mouse Funcs
*/

static int syscalls_getMousePositionScreen(lua_State *L)
{
    ksys_pos_t pos = _ksys_get_mouse_pos(KSYS_MOUSE_SCREEN_POS);

    lua_createtable(L, 0, 2);

    lua_pushinteger(L, pos.x);
    lua_setfield(L, -2, "x");

    lua_pushinteger(L, pos.y);
    lua_setfield(L, -2, "y");

    return 1;
}

static int syscalls_getMousePositionWindow(lua_State *L)
{
    ksys_pos_t pos = _ksys_get_mouse_pos(KSYS_MOUSE_WINDOW_POS);

    lua_createtable(L, 0, 2);

    lua_pushinteger(L, pos.x);
    lua_setfield(L, -2, "x");

    lua_pushinteger(L, pos.y);
    lua_setfield(L, -2, "y");

    return 1;
}

static int syscalls_getMouseWheels(lua_State *L)
{
    uint32_t state = _ksys_get_mouse_wheels();

    lua_createtable(L, 0, 2);

    lua_pushinteger(L, state & 0xFFFF);
    lua_setfield(L, -2, "y");

    lua_pushinteger(L, state << 16);
    lua_setfield(L, -2, "x");

    return 1;
}

void createMouseState(uint32_t state, lua_State *L)
{
    lua_pushboolean(L, state & KSYS_MOUSE_LBUTTON_PRESSED);
    lua_setfield(L, -2, "LeftButton");

    lua_pushboolean(L, state & KSYS_MOUSE_RBUTTON_PRESSED);
    lua_setfield(L, -2, "RightButton");

    lua_pushboolean(L, state & KSYS_MOUSE_MBUTTON_PRESSED);
    lua_setfield(L, -2, "MiddleButton");

    lua_pushboolean(L, state & KSYS_MOUSE_4BUTTON_PRESSED);
    lua_setfield(L, -2, "Button4");

    lua_pushboolean(L, state & KSYS_MOUSE_5BUTTON_PRESSED);
    lua_setfield(L, -2, "Button5");
}

static int syscalls_getMouseButtons(lua_State *L)
{
    lua_createtable(L, 0, 5);

    createMouseState(_ksys_get_mouse_buttons(), L);

    return 1;
}

static int syscalls_getMouseEvents(lua_State *L)
{
    uint32_t state = _ksys_get_mouse_eventstate();

    lua_createtable(L, 0, 5 + 9);

    createMouseState(state, L);

    lua_pushboolean(L, state & (1 << 8));
    lua_setfield(L, -2, "getMouseEvents");

    lua_pushboolean(L, state & (1 << 9));
    lua_setfield(L, -2, "RightButtonPressed");

    lua_pushboolean(L, state & (1 << 10));
    lua_setfield(L, -2, "MiddleButtonPressed");

    lua_pushboolean(L, state & (1 << 15));
    lua_setfield(L, -2, "VerticalScroll");

    lua_pushboolean(L, state & (1 << 16));
    lua_setfield(L, -2, "LeftButtonReleased");

    lua_pushboolean(L, state & (1 << 17));
    lua_setfield(L, -2, "RightButtonReleased");

    lua_pushboolean(L, state & (1 << 18));
    lua_setfield(L, -2, "MiddleButtonReleased");

    lua_pushboolean(L, state & (1 << 23));
    lua_setfield(L, -2, "HorizontalScroll");

    lua_pushboolean(L, state & (1 << 24));
    lua_setfield(L, -2, "DoubleClick");

    return 1;
}

inline uint32_t getMouseSettings(ksys_mouse_settings_t settings)
{
    uint32_t result;

    asm_inline(
        "int $0x40"
        : "=a"(result)
        : "a"(18), "b"(19), "c"(settings)
        : "memory");

    return result;
}

static int syscalls_GetMouseSpeed(lua_State *L)
{
    lua_pushinteger(L, getMouseSettings(KSYS_MOUSE_GET_SPEED));

    return 1;
}

static int syscalls_GetMouseSens(lua_State *L)
{
    lua_pushinteger(L, getMouseSettings(KSYS_MOUSE_GET_SENS));

    return 1;
}

static int syscalls_GetMouseDoubleClickDelay(lua_State *L)
{
    lua_pushinteger(L, getMouseSettings(KSYS_MOUSE_GET_DOUBLE_CLICK_DELAY));

    return 1;
}

static int syscalls_GetMouseSettings(lua_State *L)
{
    lua_createtable(L, 0, 3);

    lua_pushinteger(L, getMouseSettings(KSYS_MOUSE_GET_SPEED));
    lua_setfield(L, -2, "speed");

    lua_pushinteger(L, getMouseSettings(KSYS_MOUSE_GET_SENS));
    lua_setfield(L, -2, "sensitivity");

    lua_pushinteger(L, getMouseSettings(KSYS_MOUSE_GET_DOUBLE_CLICK_DELAY));
    lua_setfield(L, -2, "doubleClickDelay");

    return 1;
}

static int syscalls_MouseSimulateState(lua_State *L)
{
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "LeftButton");
    lua_getfield(L, 1, "RightButton");
    lua_getfield(L, 1, "MiddleButton");
    lua_getfield(L, 1, "Button4");
    lua_getfield(L, 1, "Button5");

    _ksys_set_mouse_settings(
        KSYS_MOUSE_SIM_STATE,
        (luaL_checkinteger(L, -1) << 4) |
            (luaL_checkinteger(L, -2) << 3) |
            (luaL_checkinteger(L, -3) << 2) |
            (luaL_checkinteger(L, -4) << 1) |
            (luaL_checkinteger(L, -5)));

    lua_pop(L, 2);

    return 1;
}

static int syscalls_SetMouseSpeed(lua_State *L)
{
    _ksys_set_mouse_settings(KSYS_MOUSE_SET_SPEED, luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_SetMouseSens(lua_State *L)
{
    _ksys_set_mouse_settings(KSYS_MOUSE_SET_SENS, luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_SetMousePos(lua_State *L)
{
    _ksys_set_mouse_pos(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2));

    return 0;
}

static int syscalls_SetMouseDoubleClickDelay(lua_State *L)
{
    _ksys_set_mouse_settings(KSYS_MOUSE_SET_DOUBLE_CLICK_DELAY, luaL_checkinteger(L, 1));

    return 0;
}

static int syscalls_SetMouseSettings(lua_State *L)
{
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "Speed");
    lua_getfield(L, 1, "Sens");
    lua_getfield(L, 1, "DoubleClickDelay");

    _ksys_set_mouse_settings(KSYS_MOUSE_GET_SPEED, luaL_checkinteger(L, -3));
    _ksys_set_mouse_settings(KSYS_MOUSE_SET_SENS, luaL_checkinteger(L, -2));
    _ksys_set_mouse_settings(KSYS_MOUSE_SET_DOUBLE_CLICK_DELAY, luaL_checkinteger(L, -1));

    return 0;
}

static int syscalls_LoadCursor(lua_State *L)
{
    lua_pushinteger(
        L,
        (LUA_INTEGER)_ksys_load_cursor(
            (void *)luaL_checkstring(L, 1),
            KSYS_CURSOR_FROM_FILE |
                (luaL_checkinteger(L, 2) << 24) |
                (luaL_checkinteger(L, 3) << 16)));
    return 1;
}

static int syscalls_SetCursor(lua_State *L)
{
    lua_pushinteger(
        L, 
        (LUA_INTEGER) _ksys_set_cursor(
            (void *) luaL_checkinteger(L, 1)
        )
    );
    return 1;
}

static int syscalls_DeleteCursor(lua_State *L)
{
    _ksys_delete_cursor((void*)luaL_checkinteger(L, 1));

    return 0;
}

/* 
    network funcs 
*/

static int syscalls_GetDevicesNum(lua_State *L)
{
    uint32_t num;

    asm_inline(
        "int $0x40"
        :"=a"(num)
        :"a"(74), "b"(-1)
    );

    lua_pushinteger(L, num);

    return 1;
}

static int syscalls_GetDeviceType(lua_State *L)
{
    uint32_t type;

    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(type)
        : "a"(74), "b"(0 | device << 8));

    lua_pushinteger(L, type);

    return 1;
}

static int syscalls_GetDeviceName(lua_State *L)
{
    char name[64];
    uint32_t ret;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(ret)
        : "a"(74), "b"(1 | device << 8), "c"(name));

    syscalls_ReturnStringOrNil(ret, name, L);

    return 1;
}

static int syscalls_ResetDevice(lua_State *L)
{
    uint32_t ret = 0;

    asm_inline(
        "int $0x40"
        : "=a"(ret)
        : "a"(74), "b"(2));

    syscalls_ReturnTrueOrNil(ret, L);

    return 1;
}

static int syscalls_StopDevice(lua_State *L)
{
    uint32_t ret = 0;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(ret)
        : "a"(74), "b"(3 | device << 8));

    syscalls_ReturnTrueOrNil(ret, L);

    return 1;
}

static int syscalls_GetTXPacketCount(lua_State *L)
{
    uint32_t num;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num)
        : "a"(74), "b"(6 | device << 8));

    syscalls_ReturnIntegerOrNil(num, L);

    return 1;
}

static int syscalls_GetRXPacketCount(lua_State *L)
{
    uint32_t num;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num)
        : "a"(74), "b"(7 | device << 8));

    syscalls_ReturnIntegerOrNil(num, L);

    return 1;
}

static int syscalls_GetTXByteCount(lua_State *L)
{
    int num;
    uint32_t NUM;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num), "=b"(NUM)
        : "a"(74), "b"(8 | device << 8));

    if (num == -1)
        lua_pushnil(L);
    else
        lua_pushinteger(L, (uint64_t)(num | NUM << 31));

    return 1;
}

static int syscalls_GetRXByteCount(lua_State *L)
{
    int num;
    uint32_t NUM;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num), "=b"(NUM)
        : "a"(74), "b"(9 | device << 8));

    if (num == -1)
        lua_pushnil(L);
    else
        lua_pushinteger(L, (uint64_t)(num | NUM << 31));

    return 1;
}

static int syscalls_GetTXErrorPacketCount(lua_State *L)
{
    int num;
    uint32_t NUM;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num), "=b"(NUM)
        : "a"(74), "b"(11 | device << 8));

    if (num == -1)
        lua_pushnil(L);
    else
        lua_pushinteger(L, (uint64_t)(num | NUM << 31));

    return 1;
}

static int syscalls_GetTXDropPacketCount(lua_State *L)
{
    int num;
    uint32_t NUM;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num), "=b"(NUM)
        : "a"(74), "b"(12 | device << 8));

    if (num == -1)
        lua_pushnil(L);
    else
        lua_pushinteger(L, (uint64_t)(num | NUM << 31));

    return 1;
}

static int syscalls_GetTXMissPacketCount(lua_State *L)
{
    int num;
    uint32_t NUM;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num), "=b"(NUM)
        : "a"(74), "b"(13 | device << 8));

    if (num == -1)
        lua_pushnil(L);
    else
        lua_pushinteger(L, (uint64_t)(num | NUM << 31));

    return 1;
}

static int syscalls_GetRXErrorPacketCount(lua_State *L)
{
    int num;
    uint32_t NUM;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num), "=b"(NUM)
        : "a"(74), "b"(14 | device << 8));

    if (num == -1)
        lua_pushnil(L);
    else
        lua_pushinteger(L, (uint64_t)(num | NUM << 31));

    return 1;
}

static int syscalls_GetRXDropPacketCount(lua_State *L)
{
    int num;
    uint32_t NUM;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num), "=b"(NUM)
        : "a"(74), "b"(15 | device << 8));
    
    syscalls_ReturnIntegerValueOrNil(num, (uint64_t)(num | NUM << 31), L);

    return 1;
}

static int syscalls_GetRXMissPacketCount(lua_State *L)
{
    int num;
    uint32_t NUM;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num), "=b"(NUM)
        : "a"(74), "b"(16 | device << 8));

    syscalls_ReturnIntegerValueOrNil(num, (uint64_t)(num | NUM << 31), L);

    return 1;
}

enum ConnectionStatus
{
    NoConnect = 0,
    Unknown = 1,
    Mb10 = 4,         // 10Mb
    Mb100 = 5,        // 100Mb
    Gb = 6,           // 1Gb
    FullDuplex = 0b10 //
};

static int syscalls_GetConnectionStatus(lua_State *L)
{
    enum ConnectionStatus num;
    uint8_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(num)
        : "a"(74), "b"(10 | device << 8));

    if (num == -1)
        lua_pushnil(L);
    else
        lua_pushinteger(L, num & 0x101);
        lua_pushinteger(L, (num & FullDuplex) != 0);

    return 2;
}

static int syscalls_ReadMAC(lua_State *L)
{
    uint32_t eax, ebx;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax), "=b"(ebx)
        : "a"(76), "b"(0 | device << 8));

    syscalls_ReturnIntegerValueOrNil(eax, eax | (ebx << 31), L);

    return 1;
}

typedef enum SYSCALLS_PROTOCOLS
{
    IPv4 = 1,
    ICMP = 2,
    UDP = 3,
    TCP = 4,
    ARP = 5
} SYSCALLS_PROTOCOLS;

inline int syscalls_ReadPacketSend(lua_State *L, SYSCALLS_PROTOCOLS protocol)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((protocol << 24) | (device << 8) | 0));

    syscalls_ReturnIntegerOrNil(eax, L);

    return 1;
}

inline int syscalls_ReadPacketReceive(lua_State *L, SYSCALLS_PROTOCOLS protocol)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((protocol << 24) | (device << 8) | 1));

    syscalls_ReturnIntegerOrNil(eax, L);

    return 1;
}

// IPv4

static int syscalls_IPv4ReadPacketSend(lua_State *L)
{
    return syscalls_ReadPacketSend(L, IPv4);
}

static int syscalls_IPv4ReadPacketReceive(lua_State *L)
{
    return syscalls_ReadPacketReceive(L, IPv4);
}

static int syscalls_ReadIPv4Address(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((IPv4 << 24) | (device << 8) | 2));

    syscalls_ReturnIntegerOrNil(eax, L);

    return 1;
}

static int syscalls_SetIPv4Address(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);
    uint32_t IP = luaL_checkinteger(L, 2);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((IPv4 << 24) | (device << 8) | 3), "c"(IP));

    syscalls_ReturnTrueOrNil(eax, L);

    return 1;
}

static int syscalls_ReadIPv4DNSAddress(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((IPv4 << 24) | (device << 8) | 4));

    syscalls_ReturnIntegerOrNil(eax, L);

    return 1;
}

static int syscalls_SetIPv4DNSAddress(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);
    uint32_t IP = luaL_checkinteger(L, 2);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((IPv4 << 24) | (device << 8) | 5), "c"(IP));

    syscalls_ReturnTrueOrNil(eax, L);

    return 1;
}

static int syscalls_ReadIPv4SubnetMask(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((IPv4 << 24) | (device << 8) | 6));

    syscalls_ReturnIntegerOrNil(eax, L);

    return 1;
}

static int syscalls_SetIPv4SubnetMask(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);
    uint32_t IP = luaL_checkinteger(L, 2);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((IPv4 << 24) | (device << 8) | 7), "c"(IP));

    syscalls_ReturnTrueOrNil(eax, L);

    return 1;
}

static int syscalls_ReadIPv4Gateway(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((IPv4 << 24) | (device << 8) | 8));

    syscalls_ReturnIntegerOrNil(eax, L);

    return 1;
}

static int syscalls_SetIPv4Gateway(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);
    uint32_t IP = luaL_checkinteger(L, 2);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((IPv4 << 24) | (device << 8) | 7), "c"(IP));

    syscalls_ReturnTrueOrNil(eax, L);

    return 1;
}

// ICMP

static int syscalls_ICMPReadPacketSend(lua_State *L)
{
    return syscalls_ReadPacketSend(L, ICMP);
}

static int syscalls_ICMPReadPacketReceive(lua_State *L)
{
    return syscalls_ReadPacketReceive(L, ICMP);
}

// UDP

static int syscalls_UDPReadPacketSend(lua_State *L)
{
    return syscalls_ReadPacketSend(L, UDP);
}

static int syscalls_UDPReadPacketReceive(lua_State *L)
{
    return syscalls_ReadPacketReceive(L, UDP);
}

// TCP

static int syscalls_TCPReadPacketSend(lua_State *L)
{
    return syscalls_ReadPacketSend(L, TCP);
}

static int syscalls_TCPReadPacketReceive(lua_State *L)
{
    return syscalls_ReadPacketReceive(L, TCP);
}

// ARP

struct ARP_entry
{
    uint32_t IP;
    char MAC[6];
    uint16_t Status;
    uint16_t TTL;
};

static int syscalls_ARPReadPacketSend(lua_State *L)
{
    return syscalls_ReadPacketSend(L, ARP);
}

static int syscalls_ARPReadPacketReceive(lua_State *L)
{
    return syscalls_ReadPacketReceive(L, ARP);
}

static int syscalls_ReadARPEntries(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((ARP << 24) | (device << 8) | 2));

    syscalls_ReturnIntegerOrNil(eax, L);

    return 1;
}

static int syscalls_ReadARPEntry(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);
    uint32_t entryNum = luaL_checkinteger(L, 2);
    struct ARP_entry* buffer;

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((ARP << 24) | (device << 8) | 3), "c"(entryNum), "D"(buffer));

    syscalls_ReturnIntegerValueOrNil(eax, (LUA_INTEGER)buffer, L);

    return 1;
}

static int syscalls_AddARPEntry(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);
    uint32_t entryNum = luaL_checkinteger(L, 2);
    struct ARP_entry *buffer = (struct ARP_entry *)luaL_checkinteger(L, 3);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((ARP << 24) | (device << 8) | 4), "c"(entryNum), "S"(buffer));

    syscalls_ReturnTrueOrNil(eax, L);

    return 1;
}

static int syscalls_RemoveARPEntry(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);
    uint32_t entryNum = luaL_checkinteger(L, 2);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((ARP << 24) | (device << 8) | 5), "c"(entryNum));

    syscalls_ReturnTrueOrNil(eax, L);

    return 1;
}

static int syscalls_SendARPAnnounce(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((ARP << 24) | (device << 8) | 6));

    syscalls_ReturnTrueOrNil(eax, L);

    return 1;
}

static int syscalls_ReadARPConflicts(lua_State *L)
{
    uint32_t eax;
    uint32_t device = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax)
        : "a"(76), "b"((ARP << 24) | (device << 8) | 7));

    syscalls_ReturnTrueOrNil(eax, L);

    return 1;
}

/* Сокеты */

static int syscalls_OpenSocket(lua_State *L)
{
    uint32_t socketNum;
    uint32_t errorCode;

    uint32_t family = luaL_checkinteger(L, 1);
    uint32_t type = luaL_checkinteger(L, 2);
    uint32_t protocol = luaL_checkinteger(L, 3);

    asm_inline(
        "int $0x40"
        :"=a"(socketNum), "=b"(errorCode)
        :"a"(77), "b"(0), "c"(family), "d"(type), "S"(protocol)
    );

    if(socketNum == -1)
    {
        lua_pushnil(L);                 // Push socketNum
        lua_pushnumber(L, socketNum);   // Push error Code
    }
    else
    {
        lua_pushnumber(L, socketNum);   // Push socketNum
        lua_pushnil(L);                 // Push error code
    }

    return 2;
}

static int syscalls_CloseSocket(lua_State *L)
{
    uint32_t eax;
    uint32_t errorCode;
    uint32_t socketNum = luaL_checkinteger(L, 1);

    asm_inline(
        "int $0x40"
        : "=a"(eax), "=b"(errorCode)
        : "a"(77), "b"(1), "c"(socketNum));

    syscalls_ReturnIntegerValueOrNil(eax, errorCode, L);

    return 1;
}

static int syscalls_Bind(lua_State *L)
{
    uint32_t eax;
    uint32_t errorCode;

    uint32_t socketNum = luaL_checkinteger(L, 1);
    uint32_t sockaddr = luaL_checkinteger(L, 2);
    uint32_t sockaddrLen = luaL_checkinteger(L, 3);

    asm_inline(
        "int $0x40"
        : "=a"(eax), "=b"(errorCode)
        : "a"(77), "b"(2), "c"(socketNum), "d"(sockaddr), "S"(sockaddrLen));

    syscalls_ReturnIntegerValueOrNil(eax, errorCode, L);

    return 1;
}

static int syscalls_Listen(lua_State *L)
{
    uint32_t eax;
    uint32_t errorCode;

    uint32_t socketNum = luaL_checkinteger(L, 1);
    uint32_t backlog = luaL_checkinteger(L, 2);

    asm_inline(
        "int $0x40"
        : "=a"(eax), "=b"(errorCode)
        : "a"(77), "b"(3), "c"(socketNum), "d"(backlog));

    syscalls_ReturnIntegerValueOrNil(eax, errorCode, L);

    return 1;
}

static int syscalls_Connect(lua_State *L)
{
    uint32_t eax;
    uint32_t errorCode;

    uint32_t socketNum = luaL_checkinteger(L, 1);
    uint32_t sockaddr = luaL_checkinteger(L, 2);
    uint32_t sockaddrLen = luaL_checkinteger(L, 3);

    asm_inline(
        "int $0x40"
        : "=a"(eax), "=b"(errorCode)
        : "a"(77), "b"(4), "c"(socketNum), "d"(sockaddr), "S"(sockaddrLen));

    syscalls_ReturnIntegerValueOrNil(eax, errorCode, L);

    return 1;
}

static int syscalls_Accept(lua_State *L)
{
    uint32_t eax;
    uint32_t errorCode;

    uint32_t socketNum = luaL_checkinteger(L, 1);
    uint32_t sockaddr = luaL_checkinteger(L, 2);
    uint32_t sockaddrLen = luaL_checkinteger(L, 3);

    asm_inline(
        "int $0x40"
        : "=a"(eax), "=b"(errorCode)
        : "a"(77), "b"(5), "c"(socketNum), "d"(sockaddr), "S"(sockaddrLen));

    syscalls_ReturnIntegerValueOrNil(eax, errorCode, L);

    return 1;
}

static int syscalls_Send(lua_State *L)
{
    uint32_t eax;
    uint32_t errorCode;

    uint32_t socketNum = luaL_checkinteger(L, 1);
    uint32_t buffer = luaL_checkinteger(L, 2);
    uint32_t bufferLen = luaL_checkinteger(L, 3);
    uint32_t flags = luaL_checkinteger(L, 4);

    asm_inline(
        "int $0x40"
        : "=a"(eax), "=b"(errorCode)
        : "a"(77), "b"(6), "c"(socketNum), "d"(buffer), "S"(bufferLen), "D"(flags));

    syscalls_ReturnIntegerValueOrNil(eax, errorCode, L);

    return 1;
}

static int syscalls_Receive(lua_State *L)
{
    uint32_t eax;
    uint32_t errorCode;

    uint32_t socketNum = luaL_checkinteger(L, 1);
    uint32_t buffer = luaL_checkinteger(L, 2);
    uint32_t bufferLen = luaL_checkinteger(L, 3);
    uint32_t flags = luaL_checkinteger(L, 4);

    asm_inline(
        "int $0x40"
        : "=a"(eax), "=b"(errorCode)
        : "a"(77), "b"(7), "c"(socketNum), "d"(buffer), "S"(bufferLen), "D"(flags));

    syscalls_ReturnIntegerValueOrNil(eax, errorCode, L);

    return 1;
}

static int syscalls_GetSocketOptions(lua_State *L)
{
    uint32_t eax;
    uint32_t errorCode;

    uint32_t socketNum = luaL_checkinteger(L, 1);
    uint32_t optstruct = luaL_checkinteger(L, 2);

    asm_inline(
        "int $0x40"
        : "=a"(eax), "=b"(errorCode)
        : "a"(77), "b"(8), "c"(socketNum), "d"(optstruct));

    syscalls_ReturnIntegerValueOrNil(eax, errorCode, L);

    return 1;
}

static int syscalls_GetPairSocket(lua_State *L)
{
    int32_t firstSocketNum;
    uint32_t secondSocketNum;

    asm_inline(
        "int $0x40"
        : "=a"(firstSocketNum), "=b"(secondSocketNum)
        : "a"(77), "b"(9));

    if(firstSocketNum == -1)
    {
        lua_pushnil(L);
        lua_pushinteger(L, secondSocketNum);
    }
    else
    {
        lua_pushinteger(L, firstSocketNum);
        lua_pushinteger(L, secondSocketNum);
    }

    return 2;
}

/*
** functions for 'syscalls' library
*/
static const luaL_Reg syscallsLib[] = {
    /* Window funcs */
    {"CreateWindow", syscalls_createWindow},
    {"StartRedraw", syscalls_startRedraw},
    {"EndRedraw", syscalls_endRedraw},
    {"ChangeWindow", syscalls_changeWindow},
    {"FocusWindow", syscalls_focusWindow},
    {"UnfocusWindow", syscalls_unfocusWindow},
    {"SetWindowTitle", syscalls_setWindowTitle},
    {"GetSkinHeight", syscalls_getSkinHeight},
    {"SetSkin", syscalls_SetSkin},
    {"GetSkinTitleArea", syscalls_GetSkinTilteArea},
    /* Buttons funcs*/
    {"DefineButton", syscalls_defineButton},
    {"DeleteButton", syscalls_deleteButton},
    {"GetButton", syscalls_getButton},
    {"SetButtonStyle", syscalls_SetButtonsStyle},
    /* Events funcs */
    {"SetEventMask", syscalls_setEventMask},
    {"WaitEvent", syscalls_waitEvent},
    {"CheckEvent", syscalls_checkEvent},
    {"WaitEventTimeout", syscalls_waitEventTimeout},
    /* Background funcs */
    {"BackgroundSetSize", syscalls_backgroundSetSize},
    {"BackgroundPutPixel", syscalls_backgroundPutPixel},
    {"BackgroundRedraw", syscalls_backgroundRedraw},
    /* system funcs */
    {"GetRamSize", syscalls_getRamSize},
    {"GetFreeRam", syscalls_getFreeRam},
    {"GetCPUClock", syscalls_getCPUClock},
    {"ShutdownPowerOff", syscalls_shutdownPowerOff},
    {"ShutdownReboot", syscalls_shutdownReboot},
    {"ShutdownRestartKernel", syscalls_shutdownRestartKRN},
    {"GetSystemColors", syscalls_getSystemColors},
    {"SetSystemColors", syscalls_SetSystemColors},
    {"ScreenSize", syscalls_screenSize},
    {"GetWorkArea", syscalls_GetWorkArea},
    {"SetWorkArea", syscalls_SetWorkArea},
    /* Draw funcs*/
    {"DrawLine", syscalls_drawLine},
    {"DrawPixel", syscalls_drawPixel},
    {"DrawText", syscalls_drawText},
    {"DrawRectangle", syscalls_drawRectangle},
    {"ReadPoint", syscalls_ReadPoint},
    /* keyboard funcs */
    {"SetKeyInputMode", syscalls_setKeyInputMode},
    {"GetKeyInputMouse", syscalls_getKeyInputMode},
    {"getKey", syscalls_getKey},
    {"getControlKeyState", syscalls_getControlKeyState},
    {"SetHotkey", syscalls_SetHotkey},
    {"DeleteHotkey", syscalls_DeleteHotkey},
    {"LockNormalInput", syscalls_LockNormalInput},
    {"UnlockNormalInput", syscalls_UnlockNormalInput},
    /* Threads funcs */
    {"ThreadInfo", syscalls_threadInfo},
    {"KillBySlot", syscalls_KillBySlot},
    /* Mouse funcs */
    {"GetMouseButtons", syscalls_getMouseButtons},
    {"GetMouseEvents", syscalls_getMouseEvents},
    {"GetMousePositionScreen", syscalls_getMousePositionScreen},
    {"GetMousePositionWindow", syscalls_getMousePositionWindow},
    {"GetMouseWheels", syscalls_getMouseWheels},
    {"GetMouseSpeed", syscalls_GetMouseSpeed},
    {"GetMouseSens", syscalls_GetMouseSens},
    {"GetMouseDoubleClickDelay", syscalls_GetMouseDoubleClickDelay},
    {"GetMouseSettings", syscalls_GetMouseSettings},
    {"MouseSimulateState", syscalls_MouseSimulateState},
    {"SetMouseSpeed", syscalls_SetMouseSpeed},
    {"SetMouseSens", syscalls_SetMouseSens},
    {"SetMousePos", syscalls_SetMousePos},
    {"SetMouseDoubleClickDelay", syscalls_SetMouseDoubleClickDelay},
    {"SetMouseSettings", syscalls_SetMouseSettings},
    {"LoadCursor", syscalls_LoadCursor},
    {"SetCursor", syscalls_SetCursor},
    {"DeleteCursor", syscalls_DeleteCursor},
    /* network funcs */
    {"GetDevicesNum", syscalls_GetDevicesNum},
    {"GetDeviceType", syscalls_GetDeviceType},
    {"GetDeviceName", syscalls_GetDeviceName},
    {"ResetDevice", syscalls_ResetDevice},
    {"StopDevice", syscalls_StopDevice},
    {"GetConnectionStatus", syscalls_GetConnectionStatus},
    /* statistic funcs */
    {"GetTXPacketCount", syscalls_GetTXPacketCount},
    {"GetRXPacketCount", syscalls_GetRXPacketCount},
    {"GetTXByteCount", syscalls_GetTXByteCount},
    {"GetRXByteCount", syscalls_GetRXByteCount},
    {"GetTXErrorPacketCount", syscalls_GetTXErrorPacketCount},
    {"GetTXDropPacketCount", syscalls_GetTXDropPacketCount},
    {"GetTXMissPacketCount", syscalls_GetTXMissPacketCount},
    {"GetRXErrorPacketCount", syscalls_GetRXErrorPacketCount},
    {"GetRXDropPacketCount", syscalls_GetRXDropPacketCount},
    {"GetRXMissPacketCount", syscalls_GetRXMissPacketCount},
    {"IPv4ReadPacketSend", syscalls_IPv4ReadPacketSend},
    {"IPv4ReadPacketReceive", syscalls_IPv4ReadPacketReceive},
    {"ICMPReadPacketSend", syscalls_ICMPReadPacketSend},
    {"ICMPReadPacketReceive", syscalls_ICMPReadPacketReceive},
    {"UDPReadPacketSend", syscalls_UDPReadPacketSend},
    {"UDPReadPacketReceive", syscalls_UDPReadPacketReceive},
    {"TCPReadPacketSend", syscalls_TCPReadPacketSend},
    {"TCPReadPacketReceive", syscalls_TCPReadPacketReceive},
    {"ARPReadPacketSend", syscalls_ARPReadPacketSend},
    {"ARPReadPacketReceive", syscalls_ARPReadPacketReceive},
    {"ReadIPv4Address", syscalls_ReadIPv4Address},
    {"SetIPv4Address", syscalls_SetIPv4Address},
    {"ReadIPv4DNSAddress", syscalls_ReadIPv4DNSAddress},
    {"SetIPv4DNSAddress", syscalls_SetIPv4DNSAddress},
    {"ReadIPv4SubnetMask", syscalls_ReadIPv4SubnetMask},
    {"SetIPv4SubnetMask", syscalls_SetIPv4SubnetMask},
    {"ReadIPv4Gateway", syscalls_ReadIPv4Gateway},
    {"SetIPv4Gateway", syscalls_SetIPv4Gateway},
    {"ReadARPEntries", syscalls_ReadARPEntries},
    {"RemoveARPEntry", syscalls_RemoveARPEntry},
    {"AddARPEntry", syscalls_AddARPEntry},
    {"SendARPAnnounce", syscalls_SendARPAnnounce},
    {"ReadARPConflicts", syscalls_ReadARPConflicts},
    /* Socket funcs */
    {"OpenSocket", syscalls_OpenSocket},
    {"CloseSocket", syscalls_CloseSocket},
    {"Bind", syscalls_Bind},
    {"Listen", syscalls_Listen},
    {"Connect", syscalls_Connect},
    {"Accept", syscalls_Accept},
    {"Send", syscalls_Send},
    {"Receive", syscalls_Receive},
    {"GetSocketOptions", syscalls_GetSocketOptions},
    {"GetPairSocket", syscalls_GetPairSocket},
    {NULL, NULL}};

static inline void syscalls_push_events(lua_State *L)
{
    lua_pushinteger(L, KSYS_EVENT_NONE);
    lua_setfield(L, -2, "EventNone");

    lua_pushinteger(L, KSYS_EVENT_REDRAW);
    lua_setfield(L, -2, "EventRedraw");

    lua_pushinteger(L, KSYS_EVENT_KEY);
    lua_setfield(L, -2, "EventKey");

    lua_pushinteger(L, KSYS_EVENT_BUTTON);
    lua_setfield(L, -2, "EventButton");

    lua_pushinteger(L, KSYS_EVENT_DESKTOP);
    lua_setfield(L, -2, "EventDesktop");

    lua_pushinteger(L, KSYS_EVENT_MOUSE);
    lua_setfield(L, -2, "EventMouse");

    lua_pushinteger(L, KSYS_EVENT_IPC);
    lua_setfield(L, -2, "EventIPC");

    lua_pushinteger(L, KSYS_EVENT_NETWORK);
    lua_setfield(L, -2, "EventNetwork");

    lua_pushinteger(L, KSYS_EVENT_DEBUG);
    lua_setfield(L, -2, "EventDebug");

    lua_pushinteger(L, KSYS_EVENT_IRQBEGIN);
    lua_setfield(L, -2, "EventIRQBegin");
}

static inline void syscalls_push_buttonCodes(lua_State *L)
{
    lua_pushinteger(L, KSYS_MOUSE_LBUTTON_PRESSED);
    lua_setfield(L, -2, "LeftButton");

    lua_pushinteger(L, KSYS_MOUSE_RBUTTON_PRESSED);
    lua_setfield(L, -2, "RightButton");

    lua_pushinteger(L, KSYS_MOUSE_MBUTTON_PRESSED);
    lua_setfield(L, -2, "MiddleButton");

    lua_pushinteger(L, KSYS_MOUSE_4BUTTON_PRESSED);
    lua_setfield(L, -2, "Button4");

    lua_pushinteger(L, KSYS_MOUSE_5BUTTON_PRESSED);
    lua_setfield(L, -2, "Button5");
}

static inline void syscalls_push_slotStates(lua_State *L)
{
    lua_pushinteger(L, KSYS_SLOT_STATE_RUNNING);
    lua_setfield(L, -2, "stateRunning");

    lua_pushinteger(L, KSYS_SLOT_STATE_SUSPENDED);
    lua_setfield(L, -2, "stateSuspended");

    lua_pushinteger(L, KSYS_SLOT_STATE_SUSPENDED_WAIT_EVENT);
    lua_setfield(L, -2, "stateSuspendedWaitEvent");

    lua_pushinteger(L, KSYS_SLOT_STATE_NORMAL_TERM);
    lua_setfield(L, -2, "stateNormalTerm");

    lua_pushinteger(L, KSYS_SLOT_STATE_EXCEPT_TERM);
    lua_setfield(L, -2, "stateExceptTerm");

    lua_pushinteger(L, KSYS_SLOT_STATE_EXCEPT_TERM);
    lua_setfield(L, -2, "stateWaitEvent");

    lua_pushinteger(L, KSYS_SLOT_STATE_WAIT_EVENT);
    lua_setfield(L, -2, "stateFree");
}

static inline void syscalls_push_scancodes(lua_State *L)
{
    lua_pushinteger(L, KSYS_SCANCODE_0);
    lua_setfield(L, -2, "Scancode_0");

    lua_pushinteger(L, KSYS_SCANCODE_1);
    lua_setfield(L, -2, "Scancode_1");

    lua_pushinteger(L, KSYS_SCANCODE_2);
    lua_setfield(L, -2, "Scancode_2");

    lua_pushinteger(L, KSYS_SCANCODE_3);
    lua_setfield(L, -2, "Scancode_3");

    lua_pushinteger(L, KSYS_SCANCODE_4);
    lua_setfield(L, -2, "Scancode_4");

    lua_pushinteger(L, KSYS_SCANCODE_5);
    lua_setfield(L, -2, "Scancode_6");

    lua_pushinteger(L, KSYS_SCANCODE_7);
    lua_setfield(L, -2, "Scancode_7");

    lua_pushinteger(L, KSYS_SCANCODE_8);
    lua_setfield(L, -2, "Scancode_8");

    lua_pushinteger(L, KSYS_SCANCODE_9);
    lua_setfield(L, -2, "Scancode_9");

    lua_pushinteger(L, KSYS_SCANCODE_A);
    lua_setfield(L, -2, "Scancode_A");

    lua_pushinteger(L, KSYS_SCANCODE_B);
    lua_setfield(L, -2, "Scancode_B");

    lua_pushinteger(L, KSYS_SCANCODE_C);
    lua_setfield(L, -2, "Scancode_C");

    lua_pushinteger(L, KSYS_SCANCODE_D);
    lua_setfield(L, -2, "Scancode_D");

    lua_pushinteger(L, KSYS_SCANCODE_E);
    lua_setfield(L, -2, "Scancode_E");

    lua_pushinteger(L, KSYS_SCANCODE_F);
    lua_setfield(L, -2, "Scancode_F");

    lua_pushinteger(L, KSYS_SCANCODE_G);
    lua_setfield(L, -2, "Scancode_G");

    lua_pushinteger(L, KSYS_SCANCODE_H);
    lua_setfield(L, -2, "Scancode_H");

    lua_pushinteger(L, KSYS_SCANCODE_J);
    lua_setfield(L, -2, "Scancode_J");

    lua_pushinteger(L, KSYS_SCANCODE_K);
    lua_setfield(L, -2, "Scancode_K");

    lua_pushinteger(L, KSYS_SCANCODE_L);
    lua_setfield(L, -2, "Scancode_L");

    lua_pushinteger(L, KSYS_SCANCODE_M);
    lua_setfield(L, -2, "Scancode_M");

    lua_pushinteger(L, KSYS_SCANCODE_N);
    lua_setfield(L, -2, "Scancode_N");

    lua_pushinteger(L, KSYS_SCANCODE_O);
    lua_setfield(L, -2, "Scancode_O");

    lua_pushinteger(L, KSYS_SCANCODE_P);
    lua_setfield(L, -2, "Scancode_P");

    lua_pushinteger(L, KSYS_SCANCODE_Q);
    lua_setfield(L, -2, "Scancode_Q");

    lua_pushinteger(L, KSYS_SCANCODE_R);
    lua_setfield(L, -2, "Scancode_R");

    lua_pushinteger(L, KSYS_SCANCODE_S);
    lua_setfield(L, -2, "Scancode_S");

    lua_pushinteger(L, KSYS_SCANCODE_T);
    lua_setfield(L, -2, "Scancode_T");

    lua_pushinteger(L, KSYS_SCANCODE_U);
    lua_setfield(L, -2, "Scancode_U");

    lua_pushinteger(L, KSYS_SCANCODE_V);
    lua_setfield(L, -2, "Scancode_V");

    lua_pushinteger(L, KSYS_SCANCODE_W);
    lua_setfield(L, -2, "Scancode_W");

    lua_pushinteger(L, KSYS_SCANCODE_X);
    lua_setfield(L, -2, "Scancode_X");

    lua_pushinteger(L, KSYS_SCANCODE_Y);
    lua_setfield(L, -2, "Scancode_Y");

    lua_pushinteger(L, KSYS_SCANCODE_Z);
    lua_setfield(L, -2, "Scancode_Z");

    lua_pushinteger(L, KSYS_SCANCODE_F1);
    lua_setfield(L, -2, "Scancode_F1");

    lua_pushinteger(L, KSYS_SCANCODE_F2);
    lua_setfield(L, -2, "Scancode_F2");

    lua_pushinteger(L, KSYS_SCANCODE_F3);
    lua_setfield(L, -2, "Scancode_F3");

    lua_pushinteger(L, KSYS_SCANCODE_F4);
    lua_setfield(L, -2, "Scancode_F4");

    lua_pushinteger(L, KSYS_SCANCODE_F5);
    lua_setfield(L, -2, "Scancode_F5");

    lua_pushinteger(L, KSYS_SCANCODE_F6);
    lua_setfield(L, -2, "Scancode_F6");

    lua_pushinteger(L, KSYS_SCANCODE_F7);
    lua_setfield(L, -2, "Scancode_F7");

    lua_pushinteger(L, KSYS_SCANCODE_F8);
    lua_setfield(L, -2, "Scancode_F8");

    lua_pushinteger(L, KSYS_SCANCODE_F9);
    lua_setfield(L, -2, "Scancode_F9");

    lua_pushinteger(L, KSYS_SCANCODE_F10);
    lua_setfield(L, -2, "Scancode_F10");

    lua_pushinteger(L, KSYS_SCANCODE_F11);
    lua_setfield(L, -2, "Scancode_F11");

    lua_pushinteger(L, KSYS_SCANCODE_F12);
    lua_setfield(L, -2, "Scancode_F12");

    lua_pushinteger(L, KSYS_SCANCODE_LSHIFT);
    lua_setfield(L, -2, "Scancode_LeftShift");

    lua_pushinteger(L, KSYS_SCANCODE_RSHIFT);
    lua_setfield(L, -2, "Scancode_RightShift");

    lua_pushinteger(L, KSYS_SCANCODE_BACKSLASH);
    lua_setfield(L, -2, "Scancode_Backslash");

    lua_pushinteger(L, KSYS_SCANCODE_COMMA);
    lua_setfield(L, -2, "Scancode_Comma");

    lua_pushinteger(L, KSYS_SCANCODE_SLASH);
    lua_setfield(L, -2, "Scancode_Slash");

    lua_pushinteger(L, KSYS_SCANCODE_LALT);
    lua_setfield(L, -2, "Scancode_LeftAlt");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_RALT);
    lua_setfield(L, -2, "Scancode_RightAlt");

    lua_pushinteger(L, KSYS_SCANCODE_LCTRL);
    lua_setfield(L, -2, "Scancode_LeftCtrl");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_RCTRL);
    lua_setfield(L, -2, "Scancode_RightCtrl");

    lua_pushinteger(L, KSYS_SCANCODE_CAPSLOCK);
    lua_setfield(L, -2, "Scancode_CapsLock");

    lua_pushinteger(L, KSYS_SCANCODE_NUMLOCK);
    lua_setfield(L, -2, "Scancode_NumLock");

    lua_pushinteger(L, KSYS_SCANCODE_POINT);
    lua_setfield(L, -2, "Scancode_Point");

    lua_pushinteger(L, KSYS_SCANCODE_ENTER);
    lua_setfield(L, -2, "Scancode_Enter");

    lua_pushinteger(L, KSYS_SCANCODE_ESC);
    lua_setfield(L, -2, "Scancode_Esc");

    lua_pushinteger(L, KSYS_SCANCODE_TAB);
    lua_setfield(L, -2, "Scancode_Tab");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_HOME);
    lua_setfield(L, -2, "Scancode_Home");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_PGUP);
    lua_setfield(L, -2, "Scancode_PageUp");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_PGDOWN);
    lua_setfield(L, -2, "Scancode_PageDown");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_END);
    lua_setfield(L, -2, "Scancode_End");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_UP);
    lua_setfield(L, -2, "Scancode_Up");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_DOWN);
    lua_setfield(L, -2, "Scancode_Down");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_LEFT);
    lua_setfield(L, -2, "Scancode_Left");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_RIGHT);
    lua_setfield(L, -2, "Scancode_Right");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_DELETE);
    lua_setfield(L, -2, "Scancode_Delete");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_INSERT);
    lua_setfield(L, -2, "Scancode_Insert");

    lua_pushinteger(L, KSYS_SCANCODE_MINUS);
    lua_setfield(L, -2, "Scancode_Minus");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_NUMPAD_ENTER);
    lua_setfield(L, -2, "Scancode_NumpadEnter");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_NUMPAD_DIV);
    lua_setfield(L, -2, "Scancode_NumpadDiv");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_MULT);
    lua_setfield(L, -2, "Scancode_NumpadMult");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_MINUS);
    lua_setfield(L, -2, "Scancode_NumpadMinus");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_PLUS);
    lua_setfield(L, -2, "Scancode_NumpadPlus");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_0);
    lua_setfield(L, -2, "Scancode_Numpad_0");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_1);
    lua_setfield(L, -2, "Scancode_Numpad_1");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_2);
    lua_setfield(L, -2, "Scancode_Numpad_2");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_3);
    lua_setfield(L, -2, "Scancode_Numpad_3");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_4);
    lua_setfield(L, -2, "Scancode_Numpad_4");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_5);
    lua_setfield(L, -2, "Scancode_Numpad_5");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_6);
    lua_setfield(L, -2, "Scancode_Numpad_6");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_7);
    lua_setfield(L, -2, "Scancode_Numpad_7");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_8);
    lua_setfield(L, -2, "Scancode_Numpad_8");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_9);
    lua_setfield(L, -2, "Scancode_Numpad_9");
}

static inline void syscalls_push_hotkey_states(lua_State *L)
{
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "hotkeyNoOne");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "hotkeyOnlyOne");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "hotkeyBoth");

    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "hotkeyLeftOnly");

    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "hotkeyRightOnly");
}

static inline void syscalls_push_buttonsStyle(lua_State *L)
{
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "ButtonStyleFlat");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "ButtonStyleVolume");
}

static inline void syscalls_push_windowStyles(lua_State *L)
{
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "windowFixSizes");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "windowNoDraw");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "windowCanChangeSizes");

    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "windowWithSkin");

    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "windowWithSkinFixSizes");
}

static inline void syscalls_push_buttons(lua_State *L)
{
    lua_pushnumber(L, 0xffff);
    lua_setfield(L, -2, "minimizationButton");

    lua_pushnumber(L, 1);
    lua_setfield(L, -2, "closeButton");
}

static inline void syscalls_push_connectionStatus(lua_State *L)
{
    lua_pushnumber(L, NoConnect);
    lua_setfield(L, -2, "noConnect");

    lua_pushnumber(L, Unknown);
    lua_setfield(L, -2, "unknownConnection");

    lua_pushnumber(L, Mb10);
    lua_setfield(L, -2, "Connection10Mb");

    lua_pushnumber(L, Mb100);
    lua_setfield(L, -2, "Connection100Mb");

    lua_pushnumber(L, Gb);
    lua_setfield(L, -2, "Connection1Gb");

}

LUALIB_API int luaopen_syscalls(lua_State *L)
{
    luaL_newlib(L, syscallsLib);

    syscalls_push_events(L);
    syscalls_push_slotStates(L);
    syscalls_push_scancodes(L);
    syscalls_push_hotkey_states(L);
    syscalls_push_buttonsStyle(L);
    syscalls_push_windowStyles(L);
    syscalls_push_buttons(L);
    syscalls_push_connectionStatus(L);

    _ksys_set_event_mask(7); // set default event mask

    return 1;
}
