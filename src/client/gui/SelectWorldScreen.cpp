#include "client/gui/SelectWorldScreen.h"

#include "client/Minecraft.h"
#include "client/locale/Language.h"
#include "client/gui/DeleteWorldScreen.h"

#include "client/gamemode/SurvivalMode.h"

#include "world/level/Level.h"

SelectWorldScreen::SelectWorldScreen(Minecraft &minecraft, std::shared_ptr<Screen> lastScreen) : Screen(minecraft)
{
	if (lastScreen != nullptr)
		lastScreen->removed();
	this->lastScreen = lastScreen;
}

void SelectWorldScreen::init()
{
	Language &language = Language::getInstance();
	title = language.getElement(u"selectWorld.title");
	jstring str_empty = language.getElement(u"selectWorld.empty");
	jstring str_world = language.getElement(u"selectWorld.world");

	std::unique_ptr<File> workingDirectory = minecraft.getWorkingDirectory();

	for (byte_t i = 0; i < 5; i++)
	{
		std::shared_ptr<CompoundTag> tag = Level::getDataTagFor(*workingDirectory, u"World" + String::toString(i + 1));
		if (tag == nullptr)
		{
			buttons.push_back(Util::make_shared<Button>(i, width / 2 - 100, height / 6 + 24 * i, u"- " + str_empty + u" -"));
		}
		else
		{
			jstring name = str_world + u" " + String::toString(i + 1);
			long_t size = tag->getLong(u"SizeOnDisk");
			// name = name + u" (" + String::toString(static_cast<float>(size / 1024 * 100 / 1024) / 100.0f) + u" MB)";
			buttons.push_back(Util::make_shared<Button>(i, width / 2 - 100, height / 6 + 24 * i, name));
		}
	}

	postInit();
}

jstring SelectWorldScreen::getWorldName(int_t i)
{
	std::unique_ptr<File> workingDirectory = minecraft.getWorkingDirectory();
	std::shared_ptr<CompoundTag> tag = Level::getDataTagFor(*workingDirectory, u"World" + String::toString(i));
	if (tag == nullptr)
		return u"";
	return u"World" + String::toString(i);
}

void SelectWorldScreen::postInit()
{
	Language &language = Language::getInstance();
	buttons.push_back(Util::make_shared<Button>(5, width / 2 - 100, height / 6 + 120 + 12, language.getElement(u"selectWorld.delete")));
	buttons.push_back(Util::make_shared<Button>(6, width / 2 - 100, height / 6 + 168, language.getElement(u"gui.cancel")));
}

void SelectWorldScreen::buttonClicked(Button &button)
{
	if (!button.active) return;

	if (button.id < 5)
		worldSelected(button.id + 1);
	if (button.id == 5)
		minecraft.setScreen(Util::make_shared<DeleteWorldScreen>(minecraft, minecraft.screen));
	if (button.id == 6)
		minecraft.setScreen(lastScreen);
}

void SelectWorldScreen::worldSelected(int_t i)
{
	minecraft.setScreen(nullptr);
	if (!done)
	{
		done = true;
		minecraft.gameMode = Util::make_shared<SurvivalMode>(minecraft);
		minecraft.selectLevel(u"World" + String::toString(i));
		minecraft.setScreen(nullptr);
	}
}

void SelectWorldScreen::render(int_t xm, int_t ym, float a)
{
	renderBackground();
	drawCenteredString(font, title, width / 2, 20, 0xFFFFFF);
	Screen::render(xm, ym, a);
}
