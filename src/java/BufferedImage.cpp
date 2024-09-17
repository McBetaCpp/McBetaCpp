#include "java/BufferedImage.h"

#include <cassert>

#include "stb_image.h"

BufferedImage::BufferedImage(int_t width, int_t height, std::unique_ptr<unsigned char[]> raw_pixels)
{
	this->width = width;
	this->height = height;
	this->raw_pixels = std::move(raw_pixels);
}

BufferedImage::BufferedImage(int_t width, int_t height)
{
	this->width = width;
	this->height = height;
	this->raw_pixels = std::make_unique<unsigned char[]>(width * height * 4);
}

int_t BufferedImage::getWidth() const
{
	return width;
}
int_t BufferedImage::getHeight() const
{
	return height;
}
const unsigned char *BufferedImage::getRawPixels() const
{
	return raw_pixels.get();
}

void BufferedImage::getRGB(int_t startX, int_t startY, int_t w, int_t h, unsigned char *rgbArray) const
{
	for (int_t y = 0; y < h; y++)
	{
		for (int_t x = 0; x < w; x++)
		{
			int_t srcIndex = ((startY + y) * width + (startX + x)) * 4;
			int_t dstIndex = (y * w + x) * 4;

			rgbArray[dstIndex + 0] = raw_pixels[srcIndex + 0];
			rgbArray[dstIndex + 1] = raw_pixels[srcIndex + 1];
			rgbArray[dstIndex + 2] = raw_pixels[srcIndex + 2];
			rgbArray[dstIndex + 3] = raw_pixels[srcIndex + 3];
		}
	}
}

void BufferedImage::setRGB(int_t startX, int_t startY, int_t w, int_t h, unsigned char *rgbArray)
{
	for (int_t y = 0; y < h; y++)
	{
		for (int_t x = 0; x < w; x++)
		{
			int_t srcIndex = (y * w + x) * 4;
			int_t dstIndex = ((startY + y) * width + (startX + x)) * 4;

			raw_pixels[dstIndex + 0] = rgbArray[srcIndex + 0];
			raw_pixels[dstIndex + 1] = rgbArray[srcIndex + 1];
			raw_pixels[dstIndex + 2] = rgbArray[srcIndex + 2];
			raw_pixels[dstIndex + 3] = rgbArray[srcIndex + 3];
		}
	}
}

static int istream_read(void *user, char *data, int size)
{
	auto &in = *reinterpret_cast<std::istream *>(user);
	in.read(reinterpret_cast<char *>(data), size);
	return static_cast<int>(in.gcount());
}

static void istream_skip(void *user, int n)
{
	auto &in = *reinterpret_cast<std::istream *>(user);
	in.seekg(n, std::ios::cur);
}

static int istream_eof(void *user)
{
	auto &in = *reinterpret_cast<std::istream *>(user);
	return in.eof();
}

stbi_io_callbacks stbi_io_callbacks_istream = { istream_read, istream_skip, istream_eof };

BufferedImage BufferedImage::ImageIO_read(std::istream &in)
{
	// Decode image
	int w, h, comp;
	stbi_uc *raw_data = stbi_load_from_callbacks(&stbi_io_callbacks_istream, &in, &w, &h, &comp, 0);
	assert(raw_data != nullptr);

	// Convert to RGBA
	std::unique_ptr<unsigned char[]> data;

	if (comp == 3)
	{
		data = std::make_unique<unsigned char[]>(w * h * 4);
		for (int i = 0; i < w * h; i++)
		{
			data[i * 4 + 0] = raw_data[i * 3 + 0];
			data[i * 4 + 1] = raw_data[i * 3 + 1];
			data[i * 4 + 2] = raw_data[i * 3 + 2];
			data[i * 4 + 3] = 255;
		}
		stbi_image_free(raw_data);
	}
	else if (comp == 4)
	{
		data = std::unique_ptr<unsigned char[]>(raw_data);
	}
	else
	{
		assert(0);
	}

	return BufferedImage(w, h, std::move(data));
}
