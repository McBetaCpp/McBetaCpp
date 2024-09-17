#pragma once

#include "java/Type.h"

#include "lwjgl/Mouse.h"

class Minecraft;

class MouseHandler
{
public:
	static constexpr bool USE_LWJGL_MOUSE = true;

private:
	Minecraft &minecraft;

	int_t xOld = 0;
	int_t toSkip = 10;
	int_t yOld = 0;

public:
	int_t xd = 0;
	int_t yd = 0;

public:
	MouseHandler(Minecraft &minecraft);
	void grab();
	void release();
	void poll();
};
