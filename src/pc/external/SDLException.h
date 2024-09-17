#pragma once

#include <stdexcept>

#include "SDL_error.h"

class SDLException : public std::runtime_error
{
public:
	SDLException() : std::runtime_error(SDL_GetError())
	{
	}
};
