#include "client/skins/DefaultTexturePack.h"

#include "client/Minecraft.h"
#include "java/Resource.h"

#include "OpenGL.h"

DefaultTexturePack::DefaultTexturePack()
{
	name = u"Default";
	desc1 = u"The default look of Minecraft";

	std::unique_ptr<std::istream> is(Resource::getResource(u"/pack.png"));
	icon = Util::make_unique<BufferedImage>(BufferedImage::ImageIO_read(*is));
}

void DefaultTexturePack::unload(Minecraft &minecraft)
{
	if (icon != nullptr)
		minecraft.textures.releaseTexture(texture);
}

void DefaultTexturePack::bindTexture(Minecraft &minecraft)
{
	if (icon != nullptr && texture < 0)
		texture = minecraft.textures.getTexture(*icon);
	if (icon != nullptr)
		minecraft.textures.bind(texture);
	else
		glBindTexture(GL_TEXTURE_2D, minecraft.textures.loadTexture(u"/gui/unknown_pack.png"));
}
