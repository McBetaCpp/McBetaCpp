#include "client/gui/DeleteWorldScreen.h"

#include "client/gui/ConfirmScreen.h"

#include "client/Minecraft.h"
#include "world/level/Level.h"

DeleteWorldScreen::DeleteWorldScreen(Minecraft &minecraft, std::shared_ptr<Screen> lastScreen) : SelectWorldScreen(minecraft, lastScreen)
{
	title = u"Delete world";
}

void DeleteWorldScreen::postInit()
{
	buttons.push_back(std::make_shared<Button>(6, width / 2 - 100, height / 6 + 168, u"Cancel"));
}

void DeleteWorldScreen::worldSelected(int_t id)
{
	jstring worldName = getWorldName(id);
	if (!worldName.empty())
	{
		jstring title1 = u"Are you sure you want to delete this world?";
		jstring title2 = u"'" + worldName + u"' will be lost forever!";
		minecraft.setScreen(std::make_shared<ConfirmScreen>(minecraft, minecraft.screen, title1, title2, id));
	}
}

void DeleteWorldScreen::confirmResult(bool result, int_t id)
{
	if (result)
		Level::deleteLevel(*minecraft.getWorkingDirectory(), getWorldName(id));
	minecraft.setScreen(lastScreen);
}
