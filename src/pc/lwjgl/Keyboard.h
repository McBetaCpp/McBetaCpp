#pragma once

#include <string>

#include "java/Type.h"
#include "java/String.h"

#include "SDL_events.h"

namespace lwjgl
{
namespace Keyboard
{
namespace detail
{

void pushEvent(const SDL_Event &e);

}

// Key enum
enum Key
{
	KEY_NONE = 0x00,

	KEY_ESCAPE = 0x01,
	KEY_1 = 0x02,
	KEY_2 = 0x03,
	KEY_3 = 0x04,
	KEY_4 = 0x05,
	KEY_5 = 0x06,
	KEY_6 = 0x07,
	KEY_7 = 0x08,
	KEY_8 = 0x09,
	KEY_9 = 0x0A,
	KEY_0 = 0x0B,
	KEY_MINUS = 0x0C, /* - on main keyboard */
	KEY_EQUALS = 0x0D,
	KEY_BACK = 0x0E, /* backspace */
	KEY_TAB = 0x0F,
	KEY_Q = 0x10,
	KEY_W = 0x11,
	KEY_E = 0x12,
	KEY_R = 0x13,
	KEY_T = 0x14,
	KEY_Y = 0x15,
	KEY_U = 0x16,
	KEY_I = 0x17,
	KEY_O = 0x18,
	KEY_P = 0x19,
	KEY_LBRACKET = 0x1A,
	KEY_RBRACKET = 0x1B,
	KEY_RETURN = 0x1C, /* Enter on main keyboard */
	KEY_LCONTROL = 0x1D,
	KEY_A = 0x1E,
	KEY_S = 0x1F,
	KEY_D = 0x20,
	KEY_F = 0x21,
	KEY_G = 0x22,
	KEY_H = 0x23,
	KEY_J = 0x24,
	KEY_K = 0x25,
	KEY_L = 0x26,
	KEY_SEMICOLON = 0x27,
	KEY_APOSTROPHE = 0x28,
	KEY_GRAVE = 0x29, /* accent grave */
	KEY_LSHIFT = 0x2A,
	KEY_BACKSLASH = 0x2B,
	KEY_Z = 0x2C,
	KEY_X = 0x2D,
	KEY_C = 0x2E,
	KEY_V = 0x2F,
	KEY_B = 0x30,
	KEY_N = 0x31,
	KEY_M = 0x32,
	KEY_COMMA = 0x33,
	KEY_PERIOD = 0x34, /* . on main keyboard */
	KEY_SLASH = 0x35, /* / on main keyboard */
	KEY_RSHIFT = 0x36,
	KEY_MULTIPLY = 0x37, /* * on numeric keypad */
	KEY_LMENU = 0x38, /* left Alt */
	KEY_SPACE = 0x39,
	KEY_CAPITAL = 0x3A,
	KEY_F1 = 0x3B,
	KEY_F2 = 0x3C,
	KEY_F3 = 0x3D,
	KEY_F4 = 0x3E,
	KEY_F5 = 0x3F,
	KEY_F6 = 0x40,
	KEY_F7 = 0x41,
	KEY_F8 = 0x42,
	KEY_F9 = 0x43,
	KEY_F10 = 0x44,
	KEY_NUMLOCK = 0x45,
	KEY_SCROLL = 0x46, /* Scroll Lock */
	KEY_NUMPAD7 = 0x47,
	KEY_NUMPAD8 = 0x48,
	KEY_NUMPAD9 = 0x49,
	KEY_SUBTRACT = 0x4A, /* - on numeric keypad */
	KEY_NUMPAD4 = 0x4B,
	KEY_NUMPAD5 = 0x4C,
	KEY_NUMPAD6 = 0x4D,
	KEY_ADD = 0x4E, /* + on numeric keypad */
	KEY_NUMPAD1 = 0x4F,
	KEY_NUMPAD2 = 0x50,
	KEY_NUMPAD3 = 0x51,
	KEY_NUMPAD0 = 0x52,
	KEY_DECIMAL = 0x53, /* . on numeric keypad */
	KEY_F11 = 0x57,
	KEY_F12 = 0x58,
	KEY_F13 = 0x64, /*                     (NEC PC98) */
	KEY_F14 = 0x65, /*                     (NEC PC98) */
	KEY_F15 = 0x66, /*                     (NEC PC98) */
	KEY_F16 = 0x67, /* Extended Function keys - (Mac) */
	KEY_F17 = 0x68,
	KEY_F18 = 0x69,
	KEY_KANA = 0x70, /* (Japanese keyboard)            */
	KEY_F19 = 0x71, /* Extended Function keys - (Mac) */
	KEY_CONVERT = 0x79, /* (Japanese keyboard)            */
	KEY_NOCONVERT = 0x7B, /* (Japanese keyboard)            */
	KEY_YEN = 0x7D, /* (Japanese keyboard)            */
	KEY_NUMPADEQUALS = 0x8D, /* = on numeric keypad (NEC PC98) */
	KEY_CIRCUMFLEX = 0x90, /* (Japanese keyboard)            */
	KEY_AT = 0x91, /*                     (NEC PC98) */
	KEY_COLON = 0x92, /*                     (NEC PC98) */
	KEY_UNDERLINE = 0x93, /*                     (NEC PC98) */
	KEY_KANJI = 0x94, /* (Japanese keyboard)            */
	KEY_STOP = 0x95, /*                     (NEC PC98) */
	KEY_AX = 0x96, /*                     (Japan AX) */
	KEY_UNLABELED = 0x97, /*                        (J3100) */
	KEY_NUMPADENTER = 0x9C, /* Enter on numeric keypad */
	KEY_RCONTROL = 0x9D,
	KEY_SECTION = 0xA7, /* Section symbol (Mac) */
	KEY_NUMPADCOMMA = 0xB3, /* , on numeric keypad (NEC PC98) */
	KEY_DIVIDE = 0xB5, /* / on numeric keypad */
	KEY_SYSRQ = 0xB7,
	KEY_RMENU = 0xB8, /* right Alt */
	KEY_FUNCTION = 0xC4, /* Function (Mac) */
	KEY_PAUSE = 0xC5, /* Pause */
	KEY_HOME = 0xC7, /* Home on arrow keypad */
	KEY_UP = 0xC8, /* UpArrow on arrow keypad */
	KEY_PRIOR = 0xC9, /* PgUp on arrow keypad */
	KEY_LEFT = 0xCB, /* LeftArrow on arrow keypad */
	KEY_RIGHT = 0xCD, /* RightArrow on arrow keypad */
	KEY_END = 0xCF, /* End on arrow keypad */
	KEY_DOWN = 0xD0, /* DownArrow on arrow keypad */
	KEY_NEXT = 0xD1, /* PgDn on arrow keypad */
	KEY_INSERT = 0xD2, /* Insert on arrow keypad */
	KEY_DELETE = 0xD3, /* Delete on arrow keypad */
	KEY_CLEAR = 0xDA, /* Clear key (Mac) */
	KEY_LMETA = 0xDB, /* Left Windows/Option key */
	/**
	 * The left windows key, mapped to KEY_LMETA
	 *
	 * @deprecated Use KEY_LMETA instead
	 */
	KEY_LWIN = KEY_LMETA, /* Left Windows key */
	KEY_RMETA = 0xDC, /* Right Windows/Option key */
	/**
	 * The right windows key, mapped to KEY_RMETA
	 *
	 * @deprecated Use KEY_RMETA instead
	 */
	KEY_RWIN = KEY_RMETA, /* Right Windows key */
	KEY_APPS = 0xDD, /* AppMenu key */
	KEY_POWER = 0xDE,
	KEY_SLEEP = 0xDF,

	KEY_MAX,
};

// Keyboard functions
jstring getKeyName(int_t key);

// Event handling
bool next();
void enableRepeatEvents(bool repeat);
bool areRepeatEventsEnabled();

char_t getEventCharacter();
int_t getEventKey();
bool getEventKeyState();

// Polling
void poll();
bool isKeyDown(int_t key);

}
}
