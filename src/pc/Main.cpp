#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "client/Minecraft.h"

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_AUDIO);

	jstring username = u"Player" + String::fromUTF8(std::to_string(System::currentTimeMillis() % 1000));
	if (argc >= 2 && strlen(argv[1]) > 0)
		username = String::fromUTF8(argv[1]);

	jstring auth = u"-";
	if (argc >= 3 && strlen(argv[2]) > 1)
		auth = String::fromUTF8(argv[2]);

	Minecraft::start(&username, &auth);

	return 0;
}
