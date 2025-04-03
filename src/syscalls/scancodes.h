#ifndef _SYSCALLS_ENUM_SCANCODES_
#define _SYSCALLS_ENUM_SCANCODES_

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sys/ksys.h>

static inline void syscalls_register_scancodes(lua_State *L)
{
    lua_newtable(L);

    lua_pushinteger(L, KSYS_SCANCODE_0);
    lua_setfield(L, -2, "0");

    lua_pushinteger(L, KSYS_SCANCODE_1);
    lua_setfield(L, -2, "1");

    lua_pushinteger(L, KSYS_SCANCODE_2);
    lua_setfield(L, -2, "2");

    lua_pushinteger(L, KSYS_SCANCODE_3);
    lua_setfield(L, -2, "3");

    lua_pushinteger(L, KSYS_SCANCODE_4);
    lua_setfield(L, -2, "4");

    lua_pushinteger(L, KSYS_SCANCODE_5);
    lua_setfield(L, -2, "6");

    lua_pushinteger(L, KSYS_SCANCODE_7);
    lua_setfield(L, -2, "7");

    lua_pushinteger(L, KSYS_SCANCODE_8);
    lua_setfield(L, -2, "8");

    lua_pushinteger(L, KSYS_SCANCODE_9);
    lua_setfield(L, -2, "9");

    lua_pushinteger(L, KSYS_SCANCODE_A);
    lua_setfield(L, -2, "A");

    lua_pushinteger(L, KSYS_SCANCODE_B);
    lua_setfield(L, -2, "B");

    lua_pushinteger(L, KSYS_SCANCODE_C);
    lua_setfield(L, -2, "C");

    lua_pushinteger(L, KSYS_SCANCODE_D);
    lua_setfield(L, -2, "D");

    lua_pushinteger(L, KSYS_SCANCODE_E);
    lua_setfield(L, -2, "E");

    lua_pushinteger(L, KSYS_SCANCODE_F);
    lua_setfield(L, -2, "F");

    lua_pushinteger(L, KSYS_SCANCODE_G);
    lua_setfield(L, -2, "G");

    lua_pushinteger(L, KSYS_SCANCODE_H);
    lua_setfield(L, -2, "H");

    lua_pushinteger(L, KSYS_SCANCODE_J);
    lua_setfield(L, -2, "J");

    lua_pushinteger(L, KSYS_SCANCODE_K);
    lua_setfield(L, -2, "K");

    lua_pushinteger(L, KSYS_SCANCODE_L);
    lua_setfield(L, -2, "L");

    lua_pushinteger(L, KSYS_SCANCODE_M);
    lua_setfield(L, -2, "M");

    lua_pushinteger(L, KSYS_SCANCODE_N);
    lua_setfield(L, -2, "N");

    lua_pushinteger(L, KSYS_SCANCODE_O);
    lua_setfield(L, -2, "O");

    lua_pushinteger(L, KSYS_SCANCODE_P);
    lua_setfield(L, -2, "P");

    lua_pushinteger(L, KSYS_SCANCODE_Q);
    lua_setfield(L, -2, "Q");

    lua_pushinteger(L, KSYS_SCANCODE_R);
    lua_setfield(L, -2, "R");

    lua_pushinteger(L, KSYS_SCANCODE_S);
    lua_setfield(L, -2, "S");

    lua_pushinteger(L, KSYS_SCANCODE_T);
    lua_setfield(L, -2, "T");

    lua_pushinteger(L, KSYS_SCANCODE_U);
    lua_setfield(L, -2, "U");

    lua_pushinteger(L, KSYS_SCANCODE_V);
    lua_setfield(L, -2, "V");

    lua_pushinteger(L, KSYS_SCANCODE_W);
    lua_setfield(L, -2, "W");

    lua_pushinteger(L, KSYS_SCANCODE_X);
    lua_setfield(L, -2, "X");

    lua_pushinteger(L, KSYS_SCANCODE_Y);
    lua_setfield(L, -2, "Y");

    lua_pushinteger(L, KSYS_SCANCODE_Z);
    lua_setfield(L, -2, "Z");

    lua_pushinteger(L, KSYS_SCANCODE_F1);
    lua_setfield(L, -2, "F1");

    lua_pushinteger(L, KSYS_SCANCODE_F2);
    lua_setfield(L, -2, "F2");

    lua_pushinteger(L, KSYS_SCANCODE_F3);
    lua_setfield(L, -2, "F3");

    lua_pushinteger(L, KSYS_SCANCODE_F4);
    lua_setfield(L, -2, "F4");

    lua_pushinteger(L, KSYS_SCANCODE_F5);
    lua_setfield(L, -2, "F5");

    lua_pushinteger(L, KSYS_SCANCODE_F6);
    lua_setfield(L, -2, "F6");

    lua_pushinteger(L, KSYS_SCANCODE_F7);
    lua_setfield(L, -2, "F7");

    lua_pushinteger(L, KSYS_SCANCODE_F8);
    lua_setfield(L, -2, "F8");

    lua_pushinteger(L, KSYS_SCANCODE_F9);
    lua_setfield(L, -2, "F9");

    lua_pushinteger(L, KSYS_SCANCODE_F10);
    lua_setfield(L, -2, "F10");

    lua_pushinteger(L, KSYS_SCANCODE_F11);
    lua_setfield(L, -2, "F11");

    lua_pushinteger(L, KSYS_SCANCODE_F12);
    lua_setfield(L, -2, "F12");

    lua_pushinteger(L, KSYS_SCANCODE_LSHIFT);
    lua_setfield(L, -2, "LeftShift");

    lua_pushinteger(L, KSYS_SCANCODE_RSHIFT);
    lua_setfield(L, -2, "RightShift");

    lua_pushinteger(L, KSYS_SCANCODE_BACKSLASH);
    lua_setfield(L, -2, "Backslash");

    lua_pushinteger(L, KSYS_SCANCODE_COMMA);
    lua_setfield(L, -2, "Comma");

    lua_pushinteger(L, KSYS_SCANCODE_SLASH);
    lua_setfield(L, -2, "Slash");

    lua_pushinteger(L, KSYS_SCANCODE_LALT);
    lua_setfield(L, -2, "LeftAlt");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_RALT);
    lua_setfield(L, -2, "RightAlt");

    lua_pushinteger(L, KSYS_SCANCODE_LCTRL);
    lua_setfield(L, -2, "LeftCtrl");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_RCTRL);
    lua_setfield(L, -2, "RightCtrl");

    lua_pushinteger(L, KSYS_SCANCODE_CAPSLOCK);
    lua_setfield(L, -2, "CapsLock");

    lua_pushinteger(L, KSYS_SCANCODE_NUMLOCK);
    lua_setfield(L, -2, "NumLock");

    lua_pushinteger(L, KSYS_SCANCODE_POINT);
    lua_setfield(L, -2, "Point");

    lua_pushinteger(L, KSYS_SCANCODE_ENTER);
    lua_setfield(L, -2, "Enter");

    lua_pushinteger(L, KSYS_SCANCODE_ESC);
    lua_setfield(L, -2, "Esc");

    lua_pushinteger(L, KSYS_SCANCODE_TAB);
    lua_setfield(L, -2, "Tab");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_HOME);
    lua_setfield(L, -2, "Home");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_PGUP);
    lua_setfield(L, -2, "PageUp");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_PGDOWN);
    lua_setfield(L, -2, "PageDown");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_END);
    lua_setfield(L, -2, "End");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_UP);
    lua_setfield(L, -2, "Up");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_DOWN);
    lua_setfield(L, -2, "Down");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_LEFT);
    lua_setfield(L, -2, "Left");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_RIGHT);
    lua_setfield(L, -2, "Right");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_DELETE);
    lua_setfield(L, -2, "Delete");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_INSERT);
    lua_setfield(L, -2, "Insert");

    lua_pushinteger(L, KSYS_SCANCODE_MINUS);
    lua_setfield(L, -2, "Minus");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_NUMPAD_ENTER);
    lua_setfield(L, -2, "NumpadEnter");

    lua_pushinteger(L, KSYS_SCANCODE_EXT_NUMPAD_DIV);
    lua_setfield(L, -2, "NumpadDiv");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_MULT);
    lua_setfield(L, -2, "NumpadMult");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_MINUS);
    lua_setfield(L, -2, "NumpadMinus");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_PLUS);
    lua_setfield(L, -2, "NumpadPlus");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_0);
    lua_setfield(L, -2, "Numpad_0");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_1);
    lua_setfield(L, -2, "Numpad_1");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_2);
    lua_setfield(L, -2, "Numpad_2");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_3);
    lua_setfield(L, -2, "Numpad_3");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_4);
    lua_setfield(L, -2, "Numpad_4");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_5);
    lua_setfield(L, -2, "Numpad_5");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_6);
    lua_setfield(L, -2, "Numpad_6");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_7);
    lua_setfield(L, -2, "Numpad_7");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_8);
    lua_setfield(L, -2, "Numpad_8");

    lua_pushinteger(L, KSYS_SCANCODE_NUMPAD_9);
    lua_setfield(L, -2, "Numpad_9");

    lua_setfield(L, -2, "scancode");
}

#endif // _SYSCALLS_ENUM_SCANCODES_
