#include "client/gui/Font.h"

#include "SharedConstants.h"
#include "client/Options.h"
#include "client/renderer/Textures.h"
#include "client/renderer/Tesselator.h"

#include "java/Resource.h"
#include "java/BufferedImage.h"

Font::Font(Options &options, const jstring &name, Textures &textures)
{
	std::unique_ptr<std::istream> is(Resource::getResource(name));
	BufferedImage img = BufferedImage::ImageIO_read(*is);

	int_t w = img.getWidth();
	int_t h = img.getHeight();
	const unsigned char *rawPixels = img.getRawPixels();

	// Determine character widths
	for (int_t i = 0; i < 256; i++)
	{
		int_t xt = i % 16;
		int_t yt = i / 16;

		int_t x = 7;
		for (; x >= 0; x--)
		{
			int_t xPixel = xt * 8 + x;
			bool emptyColumn = true;
			for (int_t y = 0; y < 8 && emptyColumn; y++)
			{
				int_t yPixel = (yt * 8 + y) * w;
				int_t pixel = rawPixels[(xPixel + yPixel) * 4 + 3] & 0xFF;
				if (pixel > 0)
					emptyColumn = false;
			}
			if (!emptyColumn)
				break;
		}

		if (i == 32) x = 2;
		charWidths[i] = x + 2;
	}

	fontTexture = textures.getTexture(img);

	listPos = MemoryTracker::genLists(256 + 32);
	Tesselator &t = Tesselator::instance;
	for (int_t j = 0; j < 256; j++)
	{
		glNewList(listPos + j, GL_COMPILE);

		t.begin();
		
		int_t ix = j % 16 * 8;
		int_t iy = j / 16 * 8;

		float s = 7.99f;

		float uo = 0.0f;
		float vo = 0.0f;

		t.vertexUV(0.0, (0.0f + s), 0.0, (ix / 128.0f + uo), ((iy + s) / 128.0f + vo));
		t.vertexUV((0.0f + s), (0.0f + s), 0.0, ((ix + s) / 128.0f + uo), ((iy + s) / 128.0f + vo));
		t.vertexUV((0.0f + s), 0.0, 0.0, ((ix + s) / 128.0f + uo), (iy / 128.0f + vo));
		t.vertexUV(0.0, 0.0, 0.0, (ix / 128.0f + uo), (iy / 128.0f + vo));

		t.end();

		glTranslatef(charWidths[j], 0.0f, 0.0f);
		glEndList();
	}

	for (int_t j = 0; j < 32; j++)
	{
		int_t br = ((j >> 3) & 1) * 85;
		int_t r = ((j >> 2) & 1) * 170 + br;
		int_t g = ((j >> 1) & 1) * 170 + br;
		int_t b = ((j >> 0) & 1) * 170 + br;
		if (j == 6)
			r += 85;

		bool darken = (j >= 16);

		if (options.anaglyph3d)
		{
			int_t cr = (r * 30 + g * 59 + b * 11) / 100;
			int_t cg = (r * 30 + g * 70) / 100;
			int_t cb = (r * 30 + b * 70) / 100;
			r = cr;
			g = cg;
			b = cb;
		}

		if (darken)
		{
			r /= 4;
			g /= 4;
			b /= 4;
		}

		glNewList(listPos + 256 + j, GL_COMPILE);
		glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
		glEndList();
	}
}

void Font::drawShadow(const jstring &str, int_t x, int_t y, int_t color)
{
	draw(str, x + 1, y + 1, color, true);
	draw(str, x, y, color);
}

void Font::draw(const jstring &str, int_t x, int_t y, int_t color)
{
	draw(str, x, y, color, false);
}

void Font::draw(const jstring &str, int_t x, int_t y, int_t color, bool darken)
{
	if (darken)
	{
		int_t oldAlpha = color & 0xFF000000;
		color = (color & 0xFCFCFC) >> 2;
		color += oldAlpha;
	}

	glBindTexture(GL_TEXTURE_2D, fontTexture);

	float r = ((color >> 16) & 0xFF) / 255.0f;
	float g = ((color >> 8) & 0xFF) / 255.0f;
	float b = (color & 0xFF) / 255.0f;
	float a = ((color >> 24) & 0xFF) / 255.0f;
	if (a == 0.0f) a = 1.0f;

	glColor4f(r, g, b, a);

	ib.clear();
	glPushMatrix();
	glTranslatef(x, y, 0.0f);

	for (int_t i = 0; i < str.length(); i++)
	{
		while (str[i] == 223 && str.length() > i + 1)
		{
			static const std::string codes = "0123456789abcdef";
			int_t cc = codes.find(std::tolower(str[i]));
			if (cc == jstring::npos || cc > 15) cc = 15;
			ib.push_back(listPos + 256 + cc + (darken ? 16 : 0));
			i += 2;
		}

		int_t ch = SharedConstants::acceptableLetters.find(str[i]);
		if (ch != jstring::npos)
			ib.push_back(listPos + ch + 32);
	}

	glCallLists(ib.size(), GL_UNSIGNED_INT, ib.data());
	glPopMatrix();
}

int_t Font::width(const jstring &str)
{
	int_t len = 0;

	for (int_t i = 0; i < str.length(); i++)
	{
		char_t c = str[i];
		if (c == 223)
			i++;
		else
		{
			int_t ch = SharedConstants::acceptableLetters.find(c);
			if (ch != jstring::npos)
				len += charWidths.at(ch + 32);
		}
	}

	return len;
}

jstring Font::sanitize(const jstring &str)
{
	jstring result;

	for (int_t i = 0; i < str.length(); i++)
	{
		char_t c = str[i];
		if (c == 223)
			i++;
		else if (SharedConstants::acceptableLetters.find(c) != jstring::npos)
			result.push_back(c);
	}

	return result;
}
