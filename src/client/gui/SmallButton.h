#pragma once

#include "client/gui/Button.h"

#include "client/Options.h"

class SmallButton : public Button
{
private:
	Options::Option::Element *option = nullptr;

public:
	SmallButton(int_t id, int_t x, int_t y, const jstring &msg);
	SmallButton(int_t id, int_t x, int_t y, int_t width, int_t height, const jstring &msg);
	SmallButton(int_t id, int_t x, int_t y, Options::Option::Element *option, const jstring &msg);

	Options::Option::Element *getOption();

	bool isSmallButton() const override { return true; }
};
