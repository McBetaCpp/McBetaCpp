#include "client/gui/SmallButton.h"

SmallButton::SmallButton(int_t id, int_t x, int_t y, const jstring &msg) : SmallButton(id, x, y, nullptr, msg)
{

}

SmallButton::SmallButton(int_t id, int_t x, int_t y, int_t width, int_t height, const jstring &msg) : Button(id, x, y, width, height, msg)
{
	
}

SmallButton::SmallButton(int_t id, int_t x, int_t y, Options::Option::Element *option, const jstring &msg) : Button(id, x, y, 150, 20, msg)
{
	this->option = option;
}

Options::Option::Element *SmallButton::getOption()
{
	return option;
}
