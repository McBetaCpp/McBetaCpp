#include "client/skins/TexturePackRepository.h"

#include "client/Minecraft.h"

TexturePackRepository::TexturePackRepository(Minecraft &minecraft) : minecraft(minecraft)
{
	// TODO
	chosenSkinName = minecraft.options.skin;
	updateListAndSelect();
}

void TexturePackRepository::updateList()
{
	// TODO
	selected = &defaultTexturePack;
}
void TexturePackRepository::updateListAndSelect()
{
	updateList();
	selected->select();
}
