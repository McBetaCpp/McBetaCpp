#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "client/skins/TexturePack.h"
#include "client/skins/DefaultTexturePack.h"

class Minecraft;

class TexturePackRepository
{
private:
	std::vector<TexturePack> texturePacks;
	DefaultTexturePack defaultTexturePack;

public:
	TexturePack *selected = nullptr;

private:
	std::unordered_map<jstring, TexturePack *> skinCache;

	Minecraft &minecraft;
	jstring chosenSkinName;

public:
	TexturePackRepository(Minecraft &minecraft);

	void updateList();
	void updateListAndSelect();


};
