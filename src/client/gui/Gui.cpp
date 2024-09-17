#include "client/gui/Gui.h"

#include "client/Lighting.h"
#include "client/gui/ScreenSizeCalculator.h"

#include "client/Minecraft.h"
#include "world/level/Level.h"

#include "java/Runtime.h"

Gui::Gui(Minecraft &minecraft) : minecraft(minecraft)
{

}

void Gui::render(float a, bool inScreen, int_t xm, int_t ym)
{
	ScreenSizeCalculator ssc(minecraft.width, minecraft.height);
	int_t width = ssc.getWidth();
	int_t height = ssc.getHeight();

	Font &font = *minecraft.font;

	minecraft.gameRenderer.setupGuiScreen();

	glEnable(GL_BLEND);

	// if (minecraft.options.fancyGraphics)
	// 	renderVignette(minecraft.player->getBrightness(a), width, height);

	// TODO: pumpkin

	// TODO: portal

	// Inventory bar
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, minecraft.textures.loadTexture(u"/gui/gui.png"));

	blit(width / 2 - 91, height - 22, 0, 0, 182, 22);
	blit(width / 2 - 91 - 1 + 0 * 20, height - 22 - 1, 0, 22, 24, 22);

	// Cross
	glBindTexture(GL_TEXTURE_2D, minecraft.textures.loadTexture(u"/gui/icons.png"));
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
	blit(width / 2 - 7, height / 2 - 7, 0, 0, 16, 16);
	glDisable(GL_BLEND);

	// Health and armor
	bool flicker = (minecraft.player->invulnerableTime / 3) % 2 == 1;
	if (minecraft.player->invulnerableTime < 10)
		flicker = false;

	int_t nowHealth = minecraft.player->health;
	int_t lastHealth = minecraft.player->lastHealth;

	random.setSeed(tickCount * 312871);

	if (minecraft.gameMode->canHurtPlayer())
	{
		// TODO ARMOR
		int_t armor = 0;

		for (int_t x = 0; x < 10; x++)
		{
			int_t y = height - 32;
			if (armor > 0)
			{
				int_t armorX = width / 2 + 91 - x * 8 - 9;
				if (x * 2 + 1 < armor) blit(armorX, y, 34, 9, 9, 9);
				if (x * 2 + 1 == armor) blit(armorX, y, 25, 9, 9, 9);
				if (x * 2 + 1 > armor) blit(armorX, y, 16, 9, 9, 9);
			}

			int_t healthX = width / 2 - 91 + x * 8;

			if (nowHealth <= 4)
				y += random.nextInt(2);

			blit(healthX, y, 16 + flicker * 9, 0, 9, 9);
			if (flicker)
			{
				if (x * 2 + 1 < lastHealth) blit(healthX, y, 70, 0, 9, 9);
				if (x * 2 + 1 == lastHealth) blit(healthX, y, 79, 0, 9, 9);
			}
			if (x * 2 + 1 < nowHealth) blit(healthX, y, 52, 0, 9, 9);
			if (x * 2 + 1 == nowHealth) blit(healthX, y, 61, 0, 9, 9);
		}
	}

	Lighting::turnOn();
	Lighting::turnOff();

	// Debug text
	if (lwjgl::Keyboard::isKeyDown(lwjgl::Keyboard::KEY_F3))
	{
		font.drawShadow(Minecraft::VERSION_STRING + u" (" + minecraft.fpsString + u")", 2, 2, 0xFFFFFF);
		font.drawShadow(minecraft.gatherStats1(), 2, 12, 0xFFFFFF);
		font.drawShadow(minecraft.gatherStats2(), 2, 22, 0xFFFFFF);
		font.drawShadow(minecraft.gatherStats3(), 2, 32, 0xFFFFFF);
		font.drawShadow(minecraft.gatherStats4(), 2, 42, 0xFFFFFF);

		int_t maxMemory = Runtime::getRuntime().maxMemory();
		int_t totalMemory = Runtime::getRuntime().totalMemory();
		int_t freeMemory = Runtime::getRuntime().freeMemory();
		int_t usedMemory = totalMemory - freeMemory;

		jstring str = u"Used memory: " + String::toString(usedMemory * 100 / maxMemory) + u"% (" + String::toString(usedMemory / 1024 / 1024) + u"MB) of " + String::toString(maxMemory / 1024 / 1024) + u"MB";
		drawString(font, str, width - font.width(str) - 2, 2, 0xE0E0E0);
		str = u"Allocated memory: " + String::toString(totalMemory * 100 / maxMemory) + u"% (" + String::toString(totalMemory / 1024 / 1024) + u"MB)";
		drawString(font, str, width - font.width(str) - 2, 12, 0xE0E0E0);

		drawString(font, u"x: " + String::toString(minecraft.player->x), 2, 64, 0xE0E0E0);
		drawString(font, u"y: " + String::toString(minecraft.player->y), 2, 72, 0xE0E0E0);
		drawString(font, u"z: " + String::toString(minecraft.player->z), 2, 80, 0xE0E0E0);
		drawString(font, u"xRot: " + String::toString(minecraft.player->xRot), 2, 88, 0xE0E0E0);
		drawString(font, u"yRot: " + String::toString(minecraft.player->yRot), 2, 96, 0xE0E0E0);
		drawString(font, u"tilt: " + String::toString(minecraft.player->tilt), 2, 104, 0xE0E0E0);
	}
	else
	{
		font.drawShadow(Minecraft::VERSION_STRING, 2, 2, 0xFFFFFF);
	}
}

void Gui::tick()
{
	if (nowPlayingTime > 0) nowPlayingTime--;

	tickCount++;
}
