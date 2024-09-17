#pragma once

#include "client/gui/Screen.h"

class SelectWorldScreen : public Screen
{
protected:
	std::shared_ptr<Screen> lastScreen;

	jstring title = u"Select World";

private:
	bool done = false;

public:
	SelectWorldScreen(Minecraft &minecraft, std::shared_ptr<Screen> lastScreen);

	void init() override;

protected:
	jstring getWorldName(int_t i);

public:
	virtual void postInit();

protected:
	void buttonClicked(Button &button) override;

public:
	virtual void worldSelected(int_t i);

	void render(int_t xm, int_t ym, float a) override;
};
