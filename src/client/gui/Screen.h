#pragma once

#include <vector>
#include <memory>

#include "client/gui/GuiComponent.h"
#include "client/gui/Button.h"
#include "client/gui/Font.h"

#include "java/Type.h"
#include "java/String.h"

#include "util/Memory.h"

class Minecraft;

class Screen : public GuiComponent
{
protected:
	Minecraft &minecraft;

public:
	int_t width = 0;
	int_t height = 0;

protected:
	std::vector<std::shared_ptr<Button>> buttons;

public:
	bool passEvents = false;

protected:
	Font &font;

	Screen(Minecraft &minecraft);

public:
	virtual void render(int_t xm, int_t ym, float a);

protected:
	virtual void keyPressed(char_t eventCharacter, int_t eventKey);

public:
	jstring getClipboard();
	void setClipboard(const jstring &text);

private:
	std::shared_ptr<Button> clickedButton = nullptr;

protected:
	virtual void mouseClicked(int_t x, int_t y, int_t buttonNum);
	virtual void mouseReleased(int_t x, int_t y, int_t buttonNum);

	virtual void buttonClicked(Button &button);

public:
	void init(int_t width, int_t height);
	void setSize(int_t width, int_t height);
	virtual void init();

	void updateEvents();
	void mouseEvent();
	void keyboardEvent();

	virtual void tick();
	virtual void removed();
	void renderBackground();
	void renderBackground(int_t vo);
	void renderDirtBackground(int_t vo);

	virtual bool isPauseScreen();

	virtual void confirmResult(bool result, int_t id);
};
