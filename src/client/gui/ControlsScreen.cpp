#include "client/gui/ControlsScreen.h"

#include "client/Minecraft.h"
#include "client/locale/Language.h"

#include "client/gui/SmallButton.h"

ControlsScreen::ControlsScreen(Minecraft &minecraft, std::shared_ptr<Screen> lastScreen, Options &options) : Screen(minecraft), options(options)
{
	this->lastScreen = lastScreen;
}

int_t ControlsScreen::getLeftScreenPosition()
{
	return width / 2 - ROW_WIDTH + 5;
}

void ControlsScreen::init()
{
	Language &language = Language::getInstance();

	int_t leftPos = getLeftScreenPosition();
	for (int_t i = 0; i < std::size(options.keyMappings); i++)
		buttons.push_back(std::make_shared<SmallButton>(i, leftPos + (i % 2) * ROW_WIDTH, height / 6 + 24 * (i >> 1), BUTTON_WIDTH, 20, options.getKeyMessage(i)));

	buttons.push_back(std::make_shared<Button>(200, width / 2 - 100, height / 6 + 168, language.getElement(u"gui.done")));
	title = language.getElement(u"controls.title");
}

void ControlsScreen::buttonClicked(Button &button)
{
	for (int_t i = 0; i < std::size(options.keyMappings); i++)
		buttons[i]->msg = options.getKeyMessage(i);

	if (button.id == 200)
	{
		minecraft.setScreen(lastScreen);
	}
	else
	{
		selectedKey = button.id;
		button.msg = u"> " + button.msg + u" <";
	}
}

void ControlsScreen::keyPressed(char_t eventCharacter, int_t eventKey)
{
	if (selectedKey >= 0)
	{
		options.setKey(selectedKey, eventKey);
		buttons[selectedKey]->msg = options.getKeyMessage(selectedKey);
		selectedKey = -1;
	}
	else
	{
		Screen::keyPressed(eventCharacter, eventKey);
	}
}

void ControlsScreen::render(int_t xm, int_t ym, float a)
{
	renderBackground();
	drawCenteredString(font, title, width / 2, 20, 0xFFFFFF);

	int_t leftPos = getLeftScreenPosition();
	for (int_t i = 0; i < std::size(options.keyMappings); i++)
		drawString(font, options.getKeyDescription(i), leftPos + (i % 2) * ROW_WIDTH + 70 + 6, height / 6 + 24 * (i >> 1) + 7, 0xFFFFFFFF);

	Screen::render(xm, ym, a);
}
