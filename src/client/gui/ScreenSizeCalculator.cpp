#include "client/gui/ScreenSizeCalculator.h"

ScreenSizeCalculator::ScreenSizeCalculator(int_t width, int_t height)
{
	w = width;
	h = height;
	scale = 1;

	while (w / (scale + 1) >= 320 && h / (scale + 1) >= 240)
		scale++;

	w /= scale;
	h /= scale;
}

int ScreenSizeCalculator::getWidth() const
{
	return w;
}

int ScreenSizeCalculator::getHeight() const
{
	return h;
}
