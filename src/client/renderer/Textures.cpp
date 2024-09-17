#include "client/renderer/Textures.h"

#include <cassert>

#include "client/Options.h"
#include "client/skins/TexturePackRepository.h"

#include "OpenGL.h"

Textures::Textures(TexturePackRepository &skins, Options &options) : skins(skins), options(options)
{

}

int_t Textures::loadTexture(const jstring &resourceName)
{
	auto *skin = skins.selected;

	auto it = idMap.find(resourceName);
	if (it != idMap.end())
	{
		return it->second;
	}

	MemoryTracker::genTextures(ib);
	int_t i = ib[0];

	if (!resourceName.compare(0, 2, u"##"))
	{
		std::unique_ptr<std::istream> is(skin->getResource(resourceName.substr(2)));
		BufferedImage img = readImage(*is);
		img = makeStrip(img);
		loadTexture(img, i);
	}
	else if (!resourceName.compare(0, 7, u"%clamp%"))
	{
		std::unique_ptr<std::istream> is(skin->getResource(resourceName.substr(7)));
		BufferedImage img = readImage(*is);
		clamp = true;
		loadTexture(img, i);
		clamp = false;
	}
	else if (!resourceName.compare(0, 6, u"%blur%"))
	{
		std::unique_ptr<std::istream> is(skin->getResource(resourceName.substr(6)));
		BufferedImage img = readImage(*is);
		blur = true;
		loadTexture(img, i);
		blur = false;
	}
	else
	{
		std::unique_ptr<std::istream> is(skin->getResource(resourceName));
		BufferedImage img = readImage(*is);
		loadTexture(img, i);
	}

	idMap.emplace(resourceName, i);
	return i;
}

BufferedImage Textures::makeStrip(BufferedImage &source)
{
	int_t cols = source.getWidth() / 16;

	BufferedImage out(16, source.getHeight() * cols);

	for (int_t i = 0; i < cols; i++)
	{
		// g.drawImage(source, -i * 16, i * source.getHeight(), null);
		// TODO
	}

	return out;
}

int_t Textures::getTexture(BufferedImage &img)
{
	MemoryTracker::genTextures(Textures::ib);
	loadTexture(img, ib[0]);
	loadedImages.emplace(ib[0], std::move(img));
	return ib[0];
}

void Textures::loadTexture(BufferedImage &img, int_t id)
{
	glBindTexture(GL_TEXTURE_2D, id);

	if (MIPMAP)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	if (blur)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (clamp)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	int_t w = img.getWidth();
	int_t h = img.getHeight();

	const unsigned char *rawPixels = img.getRawPixels();
	std::unique_ptr<unsigned char[]> newPixels(new unsigned char[w * h * 4]);

	const unsigned char *rawPixels_p = rawPixels;
	unsigned char *newPixels_p = newPixels.get();

	for (int_t i = 0; i < w * h; i++, rawPixels_p += 4, newPixels_p += 4)
	{
		int_t a = rawPixels_p[3];
		int_t r = rawPixels_p[0];
		int_t g = rawPixels_p[1];
		int_t b = rawPixels_p[2];

		if (options.anaglyph3d)
		{
			int_t rr = (r * 30 + g * 59 + b * 11) / 100;
			int_t gg = (r * 30 + g * 70) / 100;
			int_t bb = (r * 30 + b * 70) / 100;

			r = rr;
			g = gg;
			b = bb;
		}

		newPixels_p[0] = r;
		newPixels_p[1] = g;
		newPixels_p[2] = b;
		newPixels_p[3] = a;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, newPixels.get());

	if (MIPMAP)
	{
		std::unique_ptr<int_t[]> mipmapPixels(new int_t[w * h * 4]);
		int_t *inPixels = (int_t*)newPixels.get();

		for (int level = 1; ; level++)
		{
			if (level > 4)
				break;

			int_t ow = w >> (level - 1);

			int_t ww = w >> level;
			int_t hh = h >> level;

			for (int_t x = 0; x < ww; x++)
			{
				for (int_t y = 0; y < hh; y++)
				{
					int_t c0 = inPixels[x * 2 + 0 + (y * 2 + 0) * ow];
					int_t c1 = inPixels[x * 2 + 1 + (y * 2 + 0) * ow];
					int_t c2 = inPixels[x * 2 + 1 + (y * 2 + 1) * ow];
					int_t c3 = inPixels[x * 2 + 0 + (y * 2 + 1) * ow];
					int_t col = crispBlend(crispBlend(c0, c1), crispBlend(c3, c2));
					mipmapPixels[x + y * ww] = col;
				}
			}

			glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, ww, hh, 0, GL_RGBA, GL_UNSIGNED_BYTE, mipmapPixels.get());

			// if (ww == 1 || hh == 1)
			// 	break;
		}
	}
}

void Textures::releaseTexture(int_t id)
{
	loadedImages.erase(id);

	ib[0] = id;
	glDeleteTextures(1, reinterpret_cast<GLuint*>(ib.data()));
}

int_t Textures::loadHttpTexture(const jstring &url, const jstring *backup)
{
	// TODO
	return 0;
}

int_t Textures::loadHttpTexture(const jstring &url)
{
	// TODO
	return 0;
}

void Textures::removeHttpTexture(const jstring &url)
{

}

void Textures::tick()
{
	
}

int_t Textures::smoothBlend(int_t c0, int_t c1)
{
	int_t a0 = (c0 & 0xFF000000) >> 24 & 0xFF;
	int_t a1 = (c1 & 0xFF000000) >> 24 & 0xFF;
	return (((a0 + a1) >> 1) << 24) + (((c0 & 0xFEFEFE) + (c1 & 0xFEFEFE)) >> 1);
}

int_t Textures::crispBlend(int_t c0, int_t c1)
{
	int_t a0 = ((c0 & 0xFF000000) >> 24) & 0xFF;
	int_t a1 = ((c1 & 0xFF000000) >> 24) & 0xFF;

	int_t a = 255;
	if (a0 + a1 == 0)
	{
		a0 = 1;
		a1 = 1;
		a = 0;
	}

	int_t r0 = ((c0 >> 16) & 0xFF) * a0;
	int_t g0 = ((c0 >> 8) & 0xFF) * a0;
	int_t b0 = (c0 & 0xFF) * a0;

	int_t r1 = ((c1 >> 16) & 0xFF) * a1;
	int_t g1 = ((c1 >> 8) & 0xFF) * a1;
	int_t b1 = (c1 & 0xFF) * a1;

	int_t r = (r0 + r1) / (a0 + a1);
	int_t g = (g0 + g1) / (a0 + a1);
	int_t b = (b0 + b1) / (a0 + a1);

	return (a << 24) | (r << 16) | (g << 8) | b;
}

void Textures::reloadAll()
{
	TexturePack *skin = skins.selected;

	// Reload buffered textures
	for (auto &entry : loadedImages)
	{
		loadTexture(entry.second, entry.first);
	}

	// Reload resource textures
	for (auto &entry : idMap)
	{
		const jstring &name = entry.first;

		BufferedImage image;

		if (!name.compare(0, 2, u"##"))
		{
			std::unique_ptr<std::istream> is(skin->getResource(name.substr(2)));
			image = readImage(*is);
			image = makeStrip(image);
		}
		else if (!name.compare(0, 7, u"%clamp%"))
		{
			std::unique_ptr<std::istream> is(skin->getResource(name.substr(7)));
			clamp = true;
			image = readImage(*is);
		}
		else if (!name.compare(0, 6, u"%blur%"))
		{
			std::unique_ptr<std::istream> is(skin->getResource(name.substr(6)));
			blur = true;
			image = readImage(*is);
		}
		else
		{
			std::unique_ptr<std::istream> is(skin->getResource(name));
			image = readImage(*is);
		}

		int_t id = entry.second;
		loadTexture(image, id);
		blur = false;
		clamp = false;
	}
}

BufferedImage Textures::readImage(std::istream &in)
{
	return BufferedImage::ImageIO_read(in);
}

void Textures::bind(int_t id)
{
	if (id < 0) return;
	glBindTexture(GL_TEXTURE_2D, id);
}