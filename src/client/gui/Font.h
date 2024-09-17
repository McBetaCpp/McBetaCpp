#pragma once

#include <array>
#include <string>
#include <vector>

#include "client/MemoryTracker.h"

#include "java/Type.h"
#include "java/String.h"

class Options;
class Textures;

class Font
{
private:
	std::array<int_t, 256> charWidths;
public:
	int_t fontTexture = 0;

	Font(Options &options, const jstring &name, Textures &textures);

private:
	int_t listPos = 0;
	std::vector<int_t> ib = MemoryTracker::createIntBuffer(1024);

public:
	void drawShadow(const jstring &str, int_t x, int_t y, int_t color);
	void draw(const jstring &str, int_t x, int_t y, int_t color);
	void draw(const jstring &str, int_t x, int_t y, int_t color, bool darken);

	int_t width(const jstring &str);

	static jstring sanitize(const jstring &str);
};
