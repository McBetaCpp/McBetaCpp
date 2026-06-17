#include "lwjgl/Mouse.h"

namespace lwjgl
{
namespace Mouse
{

static bool created = false;
static bool grabbed = false;
static int x = 0, y = 0;
static int dx = 0, dy = 0;
static int dwheel = 0;
static bool button_states[5] = {false};

bool isButtonDown(int button)
{
	if (button < 0 || button >= 5)
		return false;
	return button_states[button];
}

int getDX()
{
	int result = dx;
	dx = 0;
	return result;
}

int getDY()
{
	int result = dy;
	dy = 0;
	return result;
}

int getX()
{
	return x;
}

int getY()
{
	return y;
}

int getDWheel()
{
	int result = dwheel;
	dwheel = 0;
	return result;
}

bool isCreated()
{
	return created;
}

void create()
{
	created = true;
}

void grab()
{
	grabbed = true;
}

void release()
{
	grabbed = false;
}

void poll()
{
	dx = 0;
	dy = 0;
	dwheel = 0;
	// PS2 doesn't have a mouse, would need to emulate with analog sticks
}

}
}
