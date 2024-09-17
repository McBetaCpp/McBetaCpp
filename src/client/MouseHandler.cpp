#include "client/MouseHandler.h"

#include "Minecraft.h"

MouseHandler::MouseHandler(Minecraft &minecraft) : minecraft(minecraft)
{

}

void MouseHandler::grab()
{
	lwjgl::Mouse::setGrabbed(true);
	xd = 0;
	yd = 0;
}

void MouseHandler::release()
{
	lwjgl::Mouse::setCursorPosition(minecraft.width / 2, minecraft.height / 2);
	lwjgl::Mouse::setGrabbed(false);
}

void MouseHandler::poll()
{
	xd = lwjgl::Mouse::getDX();
	yd = lwjgl::Mouse::getDY();
}
