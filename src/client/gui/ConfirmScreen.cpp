#include "client/gui/ConfirmScreen.h"

#include "client/gui/SmallButton.h"

ConfirmScreen::ConfirmScreen(Minecraft &minecraft, std::shared_ptr<Screen> parent, jstring title1, jstring title2, int_t id) : Screen(minecraft)
{
	this->parent = parent;
	this->title1 = title1;
	this->title2 = title2;
	this->id = id;
}

void ConfirmScreen::init()
{
	buttons.push_back(std::make_shared<SmallButton>(0, width / 2 - 155 + 0, height / 6 + 96, u"Yes"));
	buttons.push_back(std::make_shared<SmallButton>(1, width / 2 - 155 + 160, height / 6 + 96, u"No"));
}

void ConfirmScreen::buttonClicked(Button &button)
{
	parent->confirmResult((button.id == 0), id);
}

void ConfirmScreen::render(int_t xm, int_t ym, float a)
{
	renderBackground();

	drawCenteredString(font, title1, width / 2, 70, 0xFFFFFF);
	drawCenteredString(font, title2, width / 2, 90, 0xFFFFFF);

	Screen::render(xm, ym, a);
}
