#include "CrashHandler.h"

#include "SDL_messagebox.h"

namespace CrashHandler
{

void Crash(const std::string &message, const std::string &stackTrace)
{
	std::string text = message + "\n\n" + stackTrace;
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Minecraft has crashed!", text.c_str(), nullptr);
}

}
