#include "world/level/FoliageColor.h"

#include "java/Resource.h"
#include "java/BufferedImage.h"

namespace FoliageColor
{

class ImgContainer
{
private:
	std::unique_ptr<std::istream> is()
	{
		std::unique_ptr<std::istream> is(Resource::getResource(u"/misc/foliagecolor.png"));
		return is;
	}
public:
	BufferedImage img;

	ImgContainer() : img(BufferedImage::ImageIO_read(*is()))
	{

	}
};

static BufferedImage &getImg()
{
	static ImgContainer container;
	return container.img;
}

int_t get(double x, double y)
{
	BufferedImage &img = getImg();
	const unsigned char *rawPixels = img.getRawPixels();

	y *= x;
	int_t px = static_cast<int_t>((1.0 - x) * 255.0);
	int_t py = static_cast<int_t>((1.0 - y) * 255.0);
	
	rawPixels += ((py << 8) + px) * 4;
	return 0xFF000000 | (static_cast<int_t>(rawPixels[0]) << 16) | (static_cast<int_t>(rawPixels[1]) << 8) | rawPixels[2];
}

int_t getEvergreenColor()
{
	return 0x619961;
}

int_t getBirchColor()
{
	return 0x80A755;
}

}