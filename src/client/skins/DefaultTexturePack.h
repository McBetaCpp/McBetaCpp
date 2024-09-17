#pragma once

#include "client/skins/TexturePack.h"

#include "java/BufferedImage.h"

class DefaultTexturePack : public TexturePack
{
private:
	int texture = -1;
	std::unique_ptr<BufferedImage> icon;

public:
	DefaultTexturePack();

	void unload(Minecraft &minecraft) override;
	void bindTexture(Minecraft &minecraft) override;
};
