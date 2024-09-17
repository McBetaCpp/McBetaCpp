#include "client/gui/OptionsScreen.h"

#include "client/Minecraft.h"
#include "client/Options.h"
#include "client/locale/Language.h"

#include "client/gui/ControlsScreen.h"

#include "client/gui/SmallButton.h"
#include "client/gui/SlideButton.h"

OptionsScreen::OptionsScreen(Minecraft &minecraft, std::shared_ptr<Screen> lastScreen, Options &options) : Screen(minecraft), options(options)
{
	this->lastScreen = lastScreen;
}


void OptionsScreen::init()
{
	Language &l = Language::getInstance();
	title = l.getElement(u"options.title");

	for (int_t i = 0; i < Util::size(Options::Option::values); i++)
	{
		auto &item = *Options::Option::values[i];
		if (!item.isProgress)
			buttons.push_back(Util::make_shared<SmallButton>(i, width / 2 - 155 + i % 2 * 160, height / 6 + 24 * (i >> 1), &item, options.getMessage(item)));
		else
			buttons.push_back(Util::make_shared<SlideButton>(i, width / 2 - 155 + i % 2 * 160, height / 6 + 24 * (i >> 1), &item, options.getMessage(item), options.getProgressValue(item)));
	}

	buttons.push_back(Util::make_shared<Button>(100, width / 2 - 100, height / 6 + 120 + 12, l.getElement(u"options.controls")));
	buttons.push_back(Util::make_shared<Button>(200, width / 2 - 100, height / 6 + 168, l.getElement(u"gui.done")));
}

void OptionsScreen::buttonClicked(Button &button)
{
	if (!button.active)
		return;

	if (button.id < 100 && button.isSmallButton())
	{
		auto &smallButton = reinterpret_cast<SmallButton &>(button);
		options.toggle(*smallButton.getOption(), 1);
		smallButton.msg = options.getMessage(*smallButton.getOption());
	}

	if (button.id == 100)
	{
		minecraft.options.save();
		minecraft.setScreen(Util::make_shared<ControlsScreen>(minecraft, minecraft.screen, options));
	}
	if (button.id == 200)
	{
		minecraft.options.save();
		minecraft.setScreen(lastScreen);
	}
}

void OptionsScreen::render(int_t xm, int_t ym, float a)
{
	renderBackground();
	drawCenteredString(font, title, width / 2, 20, 0xFFFFFF);

	Screen::render(xm, ym, a);
}
