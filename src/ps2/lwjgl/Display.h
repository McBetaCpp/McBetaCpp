#pragma once

#include "java/Type.h"
#include "java/String.h"

namespace lwjgl
{

// Display types
struct DisplayMode
{
private:
	int_t width = 0, height = 0;
	int_t bpp = 0, freq = 0;
	bool fullscreen = false;

	DisplayMode(int_t _width, int_t _height, int_t _bpp, int_t _freq, bool _fullscreen)
	{
		width = _width;
		height = _height;
		bpp = _bpp;
		freq = _freq;
		fullscreen = _fullscreen;
	}

public:
	DisplayMode(const DisplayMode &) = default;
	DisplayMode(DisplayMode &&) = default;
	DisplayMode &operator=(const DisplayMode &) = default;

	DisplayMode(int_t _width, int_t _height) : DisplayMode(_width, _height, 0, 0, false)
	{
		
	}

	DisplayMode(int_t _width, int_t _height, int_t _bpp, int_t _freq) : DisplayMode(_width, _height, _bpp, _freq, true)
	{
		
	}

	int_t getWidth() const
	{
		return width;
	}

	int_t getHeight() const
	{
		return height;
	}

	int_t getBitsPerPixel() const
	{
		return bpp;
	}

	int_t getFrequency() const
	{
		return freq;
	}

	int_t isFullscreen() const
	{
		return fullscreen;
	}
};

namespace Display
{

// Display functions
void setDisplayMode(const DisplayMode &display_mode);
DisplayMode getDisplayMode();

void setTitle(const jstring &string);
void setFullscreen(bool fullscreen);

bool isCloseRequested();
bool isVisible();
bool isActive();

void processMessages();

void swapBuffers();

void update(bool doProcessMessages = true);

void create();

int_t getX();
int_t getY();
int_t getWidth();
int_t getHeight();

}

}
