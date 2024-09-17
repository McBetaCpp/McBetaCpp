#include "lwjgl/Keyboard.h"

#include <queue>
#include <locale>
#include <codecvt>

#include "external/SDLException.h"

#include "SDL_events.h"

namespace lwjgl
{
namespace Keyboard
{

// Key enum conversion
static int_t keyLWJGLToSDL(int_t key)
{
	switch (key)
	{
		case 223: return SDLK_SLEEP;
		case 222: return SDLK_POWER;
			// case 221: return SDLK_APPS;
		case 220: return SDLK_RGUI;
			// case 58: return SDLK_CAPITAL;
		case 57: return SDLK_SPACE;
			// case 184: return SDLK_RMENU;
			// case 56: return SDLK_LMENU;
		case 183: return SDLK_SYSREQ;
		case 55: return SDLK_KP_MULTIPLY;
		case 54: return SDLK_RSHIFT;
		case 181: return SDLK_KP_DIVIDE;
		case 53: return SDLK_SLASH;
		case 52: return SDLK_PERIOD;
		case 179: return SDLK_KP_COMMA;
		case 51: return SDLK_COMMA;
		case 50: return SDLK_m;
		case 49: return SDLK_n;
		case 48: return SDLK_b;
		case 47: return SDLK_v;
		case 46: return SDLK_c;
		case 45: return SDLK_x;
		case 44: return SDLK_z;
		case 43: return SDLK_BACKSLASH;
		case 42: return SDLK_LSHIFT;
		case 41: return SDLK_BACKQUOTE;
		case 40: return SDLK_QUOTE;
			// case 167: return SDLK_SECTION;
		case 39: return SDLK_SEMICOLON;
		case 38: return SDLK_l;
		case 37: return SDLK_k;
		case 36: return SDLK_j;
		case 35: return SDLK_h;
		case 34: return SDLK_g;
		case 33: return SDLK_f;
		case 32: return SDLK_d;
		case 31: return SDLK_s;
		case 30: return SDLK_a;
		case 157: return SDLK_RCTRL;
		case 29: return SDLK_LCTRL;
		case 156: return SDLK_KP_ENTER;
		case 12: return SDLK_MINUS;
		case 11: return SDLK_0;
		case 10: return SDLK_9;
		case 9: return SDLK_8;
		case 8: return SDLK_7;
		case 7: return SDLK_6;
		case 6: return SDLK_5;
		case 5: return SDLK_4;
		case 4: return SDLK_3;
		case 3: return SDLK_2;
		case 2: return SDLK_1;
		case 1: return SDLK_ESCAPE;
			// case 0: return SDLK_NONE;
		case 13: return SDLK_EQUALS;
			// case 14: return SDLK_AC_BACK;
		case 141: return SDLK_KP_EQUALS;
		case 15: return SDLK_TAB;
		case 16: return SDLK_q;
		case 17: return SDLK_w;
			// case 144: return SDLK_CIRCUMFLEX;
		case 18: return SDLK_e;
		case 145: return SDLK_AT;
		case 19: return SDLK_r;
		case 146: return SDLK_COLON;
		case 20: return SDLK_t;
			// case 147: return SDLK_UNDERLINE;
		case 21: return SDLK_y;
			// case 148: return SDLK_KANJI;
		case 22: return SDLK_u;
		case 149: return SDLK_STOP;
		case 23: return SDLK_i;
			// case 150: return SDLK_AX;
		case 24: return SDLK_o;
			// case 151: return SDLK_UNLABELED;
		case 25: return SDLK_p;
		case 26: return SDLK_LEFTBRACKET;
		case 27: return SDLK_RIGHTBRACKET;
		case 28: return SDLK_RETURN;
		case 59: return SDLK_F1;
		case 60: return SDLK_F2;
		case 61: return SDLK_F3;
		case 62: return SDLK_F4;
		case 63: return SDLK_F5;
		case 64: return SDLK_F6;
		case 65: return SDLK_F7;
		case 66: return SDLK_F8;
		case 67: return SDLK_F9;
		case 68: return SDLK_F10;
		case 69: return SDLK_NUMLOCKCLEAR;
			// case 196: return SDLK_FUNCTION;
			// case 70: return SDLK_SCROLL;
		case 197: return SDLK_PAUSE;
		case 71: return SDLK_KP_7;
		case 72: return SDLK_KP_8;
		case 199: return SDLK_HOME;
		case 73: return SDLK_KP_9;
		case 200: return SDLK_UP;
		case 74: return SDLK_KP_MINUS;
		case 201: return SDLK_PRIOR;
		case 75: return SDLK_KP_4;
		case 76: return SDLK_KP_5;
		case 203: return SDLK_LEFT;
		case 77: return SDLK_KP_6;
		case 78: return SDLK_KP_PLUS;
		case 205: return SDLK_RIGHT;
		case 79: return SDLK_KP_1;
		case 80: return SDLK_KP_2;
		case 207: return SDLK_END;
		case 81: return SDLK_KP_3;
		case 208: return SDLK_DOWN;
		case 82: return SDLK_KP_0;
			// case 209: return SDLK_AC_FORWARD;
		case 83: return SDLK_KP_DECIMAL;
		case 210: return SDLK_INSERT;
		case 87: return SDLK_F11;
		case 88: return SDLK_F12;
		case 100: return SDLK_F13;
		case 101: return SDLK_F14;
		case 102: return SDLK_F15;
		case 103: return SDLK_F16;
		case 104: return SDLK_F17;
		case 105: return SDLK_F18;
			// case 112: return SDLK_KANA;
		case 113: return SDLK_F19;
			// case 121: return SDLK_CONVERT;
			// case 123: return SDLK_NOCONVERT;
			// case 125: return SDLK_YEN;
		case 211: return SDLK_DELETE;
		case 218: return SDLK_CLEAR;
		case 219: return SDLK_LGUI;
		default: return SDLK_UNKNOWN;
	}
}

static int_t keySDLToLWJGL(int_t key)
{
	switch (key)
	{
		case SDLK_SLEEP: return 223;
		case SDLK_POWER: return 222;
			// case SDLK_APPS: return 221;
			// case SDLK_CAPITAL: return 58;
		case SDLK_KP_MULTIPLY: return 55;
		case SDLK_DELETE: return 211;
		case SDLK_COMMA: return 51;
		case SDLK_r: return 19;
		case SDLK_t: return 20;
			// case SDLK_AX: return 150;
		case SDLK_BACKQUOTE: return 41;
		case SDLK_END: return 207;
		case SDLK_k: return 37;
		case SDLK_m: return 50;
		case SDLK_x: return 45;
		case SDLK_DOWN: return 208;
		case SDLK_g: return 34;
		case SDLK_f: return 33;
		case SDLK_c: return 46;
			// case SDLK_SECTION: return 167;
		case SDLK_a: return 30;
			// case SDLK_SCROLL: return 70;
		case SDLK_QUOTE: return 40;
		case SDLK_KP_EQUALS: return 141;
		case SDLK_z: return 44;
			// case SDLK_AC_BACK: return 14;
		case SDLK_6: return 7;
		case SDLK_h: return 35;
		case SDLK_5: return 6;
		case SDLK_v: return 47;
		case SDLK_BACKSLASH: return 43;
		case SDLK_8: return 9;
		case SDLK_EQUALS: return 13;
			// case SDLK_LMENU: return 56;
		case SDLK_F12: return 88;
		case SDLK_RSHIFT: return 54;
		case SDLK_ESCAPE: return 1;
			// case SDLK_YEN: return 125;
		case SDLK_LGUI: return 219;
			// case SDLK_NONE: return 0;
		case SDLK_LSHIFT: return 42;
		case SDLK_F9: return 67;
		case SDLK_LCTRL: return 29;
		case SDLK_F10: return 68;
		case SDLK_4: return 5;
		case SDLK_e: return 18;
		case SDLK_SPACE: return 57;
		case SDLK_2: return 3;
		case SDLK_F11: return 87;
		case SDLK_o: return 24;
		case SDLK_KP_5: return 76;
		case SDLK_RGUI: return 220;
		case SDLK_F13: return 100;
		case SDLK_KP_DIVIDE: return 181;
		case SDLK_KP_0: return 82;
		case SDLK_9: return 10;
		case SDLK_q: return 16;
		case SDLK_KP_3: return 81;
			// case SDLK_KANJI: return 148;
		case SDLK_STOP: return 149;
		case SDLK_b: return 48;
		case SDLK_RIGHT: return 205;
		case SDLK_SEMICOLON: return 39;
		case SDLK_MINUS: return 12;
		case SDLK_l: return 38;
		case SDLK_RIGHTBRACKET: return 27;
		case SDLK_w: return 17;
		case SDLK_TAB: return 15;
		case SDLK_y: return 21;
		case SDLK_NUMLOCKCLEAR: return 69;
		case SDLK_KP_MINUS: return 74;
		case SDLK_u: return 22;
		case SDLK_F17: return 104;
		case SDLK_p: return 25;
		case SDLK_F4: return 62;
		case SDLK_1: return 2;
		case SDLK_0: return 11;
		case SDLK_LEFTBRACKET: return 26;
		case SDLK_CLEAR: return 218;
		case SDLK_F1: return 59;
		case SDLK_F2: return 60;
		case SDLK_F3: return 61;
		case SDLK_F5: return 63;
		case SDLK_F6: return 64;
		case SDLK_F7: return 65;
		case SDLK_i: return 23;
			// case SDLK_UNLABELED: return 151;
		case SDLK_F16: return 103;
		case SDLK_F8: return 66;
		case SDLK_d: return 32;
		case SDLK_KP_7: return 71;
		case SDLK_KP_8: return 72;
		case SDLK_KP_9: return 73;
		case SDLK_7: return 8;
		case SDLK_KP_4: return 75;
		case SDLK_KP_6: return 77;
		case SDLK_j: return 36;
		case SDLK_HOME: return 199;
		case SDLK_n: return 49;
		case SDLK_KP_1: return 79;
		case SDLK_KP_ENTER: return 156;
		case SDLK_KP_2: return 80;
		case SDLK_SLASH: return 53;
			// case SDLK_CONVERT: return 121;
		case SDLK_INSERT: return 210;
			// case SDLK_RMENU: return 184;
		case SDLK_s: return 31;
		case SDLK_KP_DECIMAL: return 83;
		case SDLK_F14: return 101;
		case SDLK_F15: return 102;
		case SDLK_F18: return 105;
		case SDLK_F19: return 113;
		case SDLK_3: return 4;
		case SDLK_RETURN: return 28;
			// case SDLK_KANA: return 112;
		case SDLK_PERIOD: return 52;
			// case SDLK_NOCONVERT: return 123;
			// case SDLK_FUNCTION: return 196;
			// case SDLK_CIRCUMFLEX: return 144;
		case SDLK_AT: return 145;
		case SDLK_COLON: return 146;
			// case SDLK_UNDERLINE: return 147;
		case SDLK_RCTRL: return 157;
		case SDLK_KP_PLUS: return 78;
		case SDLK_KP_COMMA: return 179;
		case SDLK_SYSREQ: return 183;
		case SDLK_PAUSE: return 197;
		case SDLK_PRIOR: return 201;
		case SDLK_UP: return 200;
		case SDLK_LEFT: return 203;
			// case SDLK_AC_FORWARD: return 209;
		default: return KEY_NONE;
	}
}

namespace detail
{

struct Event
{
	int_t key;
	int_t character;
	bool repeat;
	bool down;
};

static Event event_current = {};
static std::queue<Event> event_queue;

static bool has_retained_event = false;
static Event event_retained = {};

static void flushRetained()
{
	if (!has_retained_event)
		return;

	has_retained_event = false;
	event_queue.push(event_retained);
}

static void handleKey(int_t key, bool repeat, bool down)
{
	flushRetained();
	has_retained_event = true;

	event_retained.key = key;
	event_retained.character = 0;
	event_retained.repeat = repeat;
	event_retained.down = down;
}

static void handleCharacter(int_t character)
{
	if (has_retained_event && event_retained.character != 0)
		flushRetained();
	if (!has_retained_event)
		event_queue.push({KEY_NONE, character, false, true});
	else
		event_retained.character = character;
}

void pushEvent(const SDL_Event &e)
{
	if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
	{
		handleKey(keySDLToLWJGL(e.key.keysym.sym), e.key.repeat, e.key.state == SDL_PRESSED);
	}
	else
	{
		// Iterate through unicode codepoints
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		std::u32string utf32 = conv.from_bytes(e.text.text);
		for (char32_t c : utf32)
			handleCharacter(c);
	}
}

}

// Keyboard functions
jstring getKeyName(int_t key)
{
	return String::fromUTF8(SDL_GetKeyName(keyLWJGLToSDL(key)));
}

// Event handling
static bool allow_repeat_events = false;

bool next()
{
	detail::flushRetained();

	if (detail::event_queue.empty())
		return false;

	if (!allow_repeat_events)
	{
		while (1)
		{
			if (detail::event_queue.empty())
				return false;
			if (detail::event_queue.front().repeat)
				detail::event_queue.pop();
			else
				break;
		}
	}

	if (detail::event_queue.empty())
		return false;

	detail::event_current = detail::event_queue.front();
	detail::event_queue.pop();
	return true;
}

void enableRepeatEvents(bool repeat)
{
	allow_repeat_events = repeat;
}

bool areRepeatEventsEnabled()
{
	return allow_repeat_events;
}

char_t getEventCharacter()
{
	return detail::event_current.character;
}

int_t getEventKey()
{
	return detail::event_current.key;
}

bool getEventKeyState()
{
	return detail::event_current.down;
}

// Polling
static const Uint8 *keyboard_state = nullptr;
static int keyboard_state_size = 0;

bool isKeyDown(int_t key)
{
	if (keyboard_state == nullptr)
	{
		keyboard_state = SDL_GetKeyboardState(&keyboard_state_size);
		if (keyboard_state == nullptr)
			throw SDLException();
	}

	int_t sdl_key = keyLWJGLToSDL(key);
	if (sdl_key == SDLK_UNKNOWN)
		return false;
	SDL_Scancode sc = SDL_GetScancodeFromKey(sdl_key);
	if (sc == SDL_SCANCODE_UNKNOWN)
		return false;
	if (sc < 0 || sc >= keyboard_state_size)
		return false;
	return keyboard_state[sc] != 0;
}


}
}
