#include "client/title/TitleScreen.h"

#include <chrono>

#include "SharedConstants.h"
#include "client/Minecraft.h"
#include "client/locale/Language.h"
#include "client/renderer/Tesselator.h"
#include "client/renderer/TileRenderer.h"
#include "client/gui/ScreenSizeCalculator.h"

#include "client/gui/SelectWorldScreen.h"
#include "client/gui/OptionsScreen.h"

#include "world/level/tile/StoneTile.h"

#include "client/gamemode/SurvivalMode.h"

#include "util/Mth.h"
#include "util/GLU.h"

#include "world/level/levelgen/synth/PerlinNoise.h"

#include "java/Resource.h"

Random TitleScreen::random;

TitleScreen::TitleScreen(Minecraft &minecraft) : Screen(minecraft)
{
	std::vector<std::string> splashes;

	std::unique_ptr<std::istream> is(Resource::getResource(u"/title/splashes.txt"));
	std::string line;
	while (std::getline(*is, line))
	{
		while (!line.empty() && std::isspace(line.back()))
			line.pop_back();
		while (!line.empty() && std::isspace(line.front()))
			line.erase(0, 1);

		if (!line.empty())
			splashes.push_back(line);
	}

	splash = String::fromUTF8(splashes[random.nextInt(splashes.size())]);

	for (unsigned i = 0; i < LOGO_WIDTH; i++)
		for (unsigned j = 0; j < LOGO_HEIGHT; j++)
			letterBlocks[i][j].reset(i, j);
}

void TitleScreen::tick()
{
	for (unsigned i = 0; i < LOGO_WIDTH; i++)
		for (unsigned j = 0; j < LOGO_HEIGHT; j++)
			letterBlocks[i][j].tick();
}

void TitleScreen::keyPressed(char_t eventCharacter, int_t eventKey)
{

}

void TitleScreen::init()
{
	// Get calendar date
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm *tm = std::localtime(&now_c);

	if (tm->tm_mon == 11 && tm->tm_wday == 9)
		splash = u"Happy birthday, ez!";
	else if (tm->tm_mon == 6 && tm->tm_mday == 1)
		splash = u"Happy birthday, Notch!";
	else if (tm->tm_mon == 12 && tm->tm_mday == 24)
		splash = u"Merry X-mas!";
	else if (tm->tm_mon == 1 && tm->tm_mday == 1)
		splash = u"Happy new year!";

	// Buttons
	Language &language = Language::getInstance();

	int_t y = height / 4 + 48;
	buttons.push_back(std::make_shared<Button>(1, width / 2 - 100, y, language.getElement(u"menu.singleplayer")));
	buttons.push_back(std::make_shared<Button>(2, width / 2 - 100, y + 24, language.getElement(u"menu.multiplayer")));
	buttons.push_back(std::make_shared<Button>(3, width / 2 - 100, y + 48, language.getElement(u"menu.mods")));
	if (false) // Applet
	{
		buttons.push_back(std::make_shared<Button>(0, width / 2 - 100, y + 72, language.getElement(u"menu.options")));
	}
	else
	{
		buttons.push_back(std::make_shared<Button>(0, width / 2 - 100, y + 72 + 12, 98, 20, language.getElement(u"menu.options")));
		buttons.push_back(std::make_shared<Button>(4, width / 2 + 2, y + 72 + 12, 98, 20, language.getElement(u"menu.quit")));
	}

	if (minecraft.user == nullptr)
		buttons[1]->active = false;

	// Level test
	/*
	minecraft.gameMode = std::make_shared<SurvivalMode>(minecraft);
	Level::deleteLevel(*minecraft.getWorkingDirectory(), u"World1");
	std::unique_ptr<File> saves(File::open(*minecraft.getWorkingDirectory(), u"saves"));
	std::shared_ptr<Level> level = std::make_shared<Level>(saves.release(), u"World1", 7746616078674770041LL);
	minecraft.setLevel(level);
	minecraft.setScreen(nullptr);
	*/
}

void TitleScreen::buttonClicked(Button &button)
{
	if (button.id == 0)
		minecraft.setScreen(std::make_shared<OptionsScreen>(minecraft, minecraft.screen, minecraft.options));
	if (button.id == 1)
		minecraft.setScreen(std::make_shared<SelectWorldScreen>(minecraft, minecraft.screen));
	if (button.id == 2);
	if (button.id == 3);
	if (button.id == 4)
		minecraft.stop();
}

void TitleScreen::render(int_t xm, int_t ym, float a)
{
	renderBackground();

	Tesselator &t = Tesselator::instance;

	renderMinecraftLogo(a);

	glBindTexture(GL_TEXTURE_2D, minecraft.textures.loadTexture(u"/gui/logo.png"));
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	t.color(0xFFFFFF);

	glPushMatrix();
	glTranslatef((width / 2 + 90), 70.0f, 0.0f);
	glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
	float s = 1.8f - Mth::abs(Mth::sin((float)(System::currentTimeMillis() % 1000) / 1000.0f * 3.1415927f * 2.0f) * 0.1f);
	s = s * 100.0f / (font.width(splash) + 32);
	glScalef(s, s, s);
	drawCenteredString(font, splash, 0, -8, 0xFFFF00);
	glPopMatrix();

	drawString(font, u"Minecraft " + SharedConstants::VERSION_STRING, 2, 2, 0x505050);

	jstring str = u"Copyright Mojang AB. Do not distribute.";
	drawString(font, str, width - font.width(str) - 2, height - 10, 0xFFFFFF);

	Screen::render(xm, ym, a);
}

void TitleScreen::renderMinecraftLogo(float a)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	ScreenSizeCalculator ssc(minecraft.width, minecraft.height);
	int_t h = 120 * ssc.scale;
	gluPerspective(70.0f, static_cast<float>(minecraft.width) / h, 0.05f, 100.0f);
	glViewport(0, minecraft.height - h, minecraft.width, h);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);

	TileRenderer tr;
	for (byte_t i = 0; i < 3; i++)
	{
		glPushMatrix();
		glTranslatef(0.4f, 0.6f, -13.0f);

		if (i == 0)
		{
			// Shadow pass
			glClear(GL_DEPTH_BUFFER_BIT);
			glTranslatef(0.0f, -0.4f, 0.0f);
			glScalef(0.98f, 1.0f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		if (i == 1)
		{
			glDisable(GL_BLEND);
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		if (i == 2)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_COLOR, GL_ONE);
		}

		glScalef(1.0f, -1.0f, 1.0f);
		glRotatef(15.0f, 1.0f, 0.0f, 0.0f);
		glScalef(0.89f, 1.0f, 0.4f);

		glTranslatef(-static_cast<float>(LOGO_WIDTH) * 0.5f, -static_cast<float>(LOGO_HEIGHT) * 0.5f, 0.0f);

		glBindTexture(GL_TEXTURE_2D, minecraft.textures.loadTexture(u"/terrain.png"));
		if (i == 0)
			glBindTexture(GL_TEXTURE_2D, minecraft.textures.loadTexture(u"/title/black.png"));

		for (byte_t y = 0; y < LOGO_HEIGHT; y++)
		{
			for (byte_t x = 0; x < LOGO_WIDTH; x++)
			{
				char c = logo[y][x];
				if (c != ' ')
				{
					glPushMatrix();

					LetterBlock &lb = letterBlocks[x][y];
					
					float z = lb.yO + (lb.y - lb.yO) * a;
					float scale = 1.0f;
					float alpha = 1.0f;
					float rot = 0.0f;
					if (i == 0)
					{
						scale = z * 0.04f + 1.0f;
						alpha = 1.0f / scale;
						z = 0.0f;
					}

					glTranslatef(x, y, z);
					glScalef(scale, scale, scale);
					glRotatef(rot, 0.0f, 1.0f, 1.0f);

					tr.renderCube(Tile::rock, alpha);
					
					glPopMatrix();
				}
			}
		}

		glPopMatrix();
	}

	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glViewport(0, 0, minecraft.width, minecraft.height);
	glEnable(GL_CULL_FACE);
}

void TitleScreen::LetterBlock::reset(int_t a, int_t b)
{
	y = yO = (10 + b) + TitleScreen::random.nextDouble() * 32.0 + a;
}
void TitleScreen::LetterBlock::tick()
{
	yO = y;
	if (y > 0.0)
		ya -= 0.6;
	y += ya;
	ya *= 0.9;
	if (y < 0.0)
	{
		y = 0;
		ya = 0.0;
	}
}
