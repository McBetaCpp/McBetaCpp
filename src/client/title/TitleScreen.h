#pragma once

#include <string>

#include "client/gui/Screen.h"

#include "java/Random.h"

class TitleScreen : public Screen
{
private:
	static Random random;

	static constexpr unsigned LOGO_WIDTH = 38;
	static constexpr unsigned LOGO_HEIGHT = 5;
	char logo[LOGO_WIDTH][LOGO_WIDTH + 1] = {
		" *   * * *   * *** *** *** *** *** ***",
		" ** ** * **  * *   *   * * * * *    * ",
		" * * * * * * * **  *   **  *** **   * ",
		" *   * * *  ** *   *   * * * * *    * ",
		" *   * * *   * *** *** * * * * *    * "
	};

	class LetterBlock
	{
	public:
		double y = 0.0;
		double yO = 0.0;
		double ya = 0.0;

		void reset(int_t a, int_t b);
		void tick();
	};
	LetterBlock letterBlocks[LOGO_WIDTH][LOGO_HEIGHT];

	float vo = 0.0f;

	jstring splash = u"missingno";

public:
	TitleScreen(Minecraft &minecraft);

	void tick() override;

protected:
	void keyPressed(char_t eventCharacter, int_t eventKey) override;

public:
	void init() override;

protected:
	void buttonClicked(Button &button) override;

public:
	void render(int_t xm, int_t ym, float a) override;

private:
	void renderMinecraftLogo(float a);
};
