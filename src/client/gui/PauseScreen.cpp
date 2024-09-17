#include "client/gui/PauseScreen.h"

#include "client/Minecraft.h"
#include "world/level/Level.h"

#include "client/title/TitleScreen.h"
#include "client/gui/OptionsScreen.h"

#include "util/Mth.h"

PauseScreen::PauseScreen(Minecraft &minecraft) : Screen(minecraft)
{

}

void PauseScreen::init()
{
	saveStep = 0;
	buttons.clear();
	
	buttons.push_back(Util::make_shared<Button>(1, width / 2 - 100, height / 4 + 48, u"Save and quit to title"));
	if (minecraft.isOnline())
		buttons[0]->msg = u"Disconnect";

	buttons.push_back(Util::make_shared<Button>(4, width / 2 - 100, height / 4 + 24, u"Back to game"));
	buttons.push_back(Util::make_shared<Button>(0, width / 2 - 100, height / 4 + 96, u"Options..."));
}

void PauseScreen::buttonClicked(Button &button)
{
	if (button.id == 0)
		minecraft.setScreen(Util::make_shared<OptionsScreen>(minecraft, minecraft.screen, minecraft.options));
	if (button.id == 1)
	{
		if (minecraft.isOnline())
			minecraft.level->disconnect();

		minecraft.setLevel(nullptr);
		minecraft.setScreen(Util::make_shared<TitleScreen>(minecraft));
	}
	if (button.id == 4)
	{
		minecraft.setScreen(nullptr);
		minecraft.grabMouse();
	}
}

void PauseScreen::tick()
{
	Screen::tick();
	visibleTime++;
}

void PauseScreen::render(int_t xm, int_t ym, float a)
{
	renderBackground();

	bool isSaving = minecraft.level->pauseSave(saveStep++);
	if (isSaving || visibleTime < 20)
	{
		float col = ((visibleTime % 10) + a) / 10.0f;
		col = Mth::sin(col * Mth::PI * 2.0f) * 0.2f + 0.8f;
		int_t br = 255.0f * col;

		drawString(font, u"Saving level...", 8, height - 16, (br << 16) | (br << 8) | br);
	}

	drawCenteredString(font, u"Game menu", width / 2, 40, 0xFFFFFF);

	Screen::render(xm, ym, a);
}
