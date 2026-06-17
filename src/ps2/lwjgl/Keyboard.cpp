#include "lwjgl/Keyboard.h"

#include <tamtypes.h>
#include <libpad.h>

namespace lwjgl
{
namespace Keyboard
{

static bool created = false;
static bool key_states[256] = {false};

bool isKeyDown(int key)
{
	if (key < 0 || key >= 256)
		return false;
	return key_states[key];
}

bool isCreated()
{
	return created;
}

void create()
{
	created = true;
}

void poll()
{
	// Reset all keys
	for (int i = 0; i < 256; i++)
		key_states[i] = false;
	
	// Map PS2 controller buttons to keyboard keys
	// This would need proper pad library integration
	// For now, just stub implementation
}

}
}
