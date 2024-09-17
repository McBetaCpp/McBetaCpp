#pragma once

#include <iostream>
#include <memory>

#include "java/Type.h"

class BufferedImage
{
private:
	int_t width = 0;
	int_t height = 0;
	std::unique_ptr<unsigned char[]> raw_pixels;

public:
	BufferedImage() {}
	BufferedImage(int_t width, int_t height, std::unique_ptr<unsigned char[]> raw_pixels);
	BufferedImage(int_t width, int_t height);

	int_t getWidth() const;
	int_t getHeight() const;
	const unsigned char *getRawPixels() const;

	void getRGB(int_t startX, int_t startY, int_t w, int_t h, unsigned char *rgbArray) const;
	void setRGB(int_t startX, int_t startY, int_t w, int_t h, unsigned char *rgbArray);

	static BufferedImage ImageIO_read(std::istream &is);
};
