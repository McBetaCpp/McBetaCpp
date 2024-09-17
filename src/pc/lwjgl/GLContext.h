#pragma once

#include <string>
#include <set>

#include "SDL.h"
#include "glad/glad.h"

namespace lwjgl
{
namespace GLContext
{

// Detail implementation
namespace detail
{

// GL capabilities
struct GLCapabilities
{
private:
	std::set<std::string> caps;

public:
	void add(const std::string &cap)
	{
		caps.insert(cap);
	}

	bool operator[](const std::string &cap) const
	{
		return caps.find(cap) != caps.end();
	}
};

// Context singletons
SDL_Window *getWindow();
SDL_GLContext getGLContext();

}

// Context functions
void instantiate();
const detail::GLCapabilities &getCapabilities();

}
}
