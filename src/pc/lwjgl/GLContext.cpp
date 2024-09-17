#include "lwjgl/GLContext.h"

#include <iostream>
#include <stdexcept>

#include "external/SDLException.h"

#include "SDL.h"

namespace lwjgl
{
namespace GLContext
{

// Detail implementation
namespace detail
{

// Context singleton
class GLContext
{
private:
	SDL_Window *window = nullptr;
	SDL_GLContext gl_context = nullptr;
	GLCapabilities capabilties;

public:
	GLContext()
	{
		// Initialize SDL renderer
		SDL_InitSubSystem(SDL_INIT_VIDEO);

		// Setup SDL to use OpenGL 1.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);

		// Create SDL window
		window = SDL_CreateWindow("McBetaCpp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		if (window == nullptr)
			throw SDLException();

		// Create OpenGL context
		gl_context = SDL_GL_CreateContext(window);
		if (gl_context == nullptr)
			throw SDLException();

		if (SDL_GL_MakeCurrent(window, gl_context))
			throw SDLException();

		// Load GLAD
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
			throw std::runtime_error("Failed to load glad");

		// Disable VSync
		SDL_GL_SetSwapInterval(0);

		// Parse capabilities
		{
			const GLubyte *extensions = glGetString(GL_EXTENSIONS);
			std::string cap;

			const char *extension_p = reinterpret_cast<const char *>(extensions);
			while (*extension_p != '\0')
			{
				if (*extension_p == ' ')
				{
					if (!cap.empty())
					{
						capabilties.add(cap);
						cap.clear();
					}
					while (*extension_p == ' ')
						extension_p++;
					continue;
				}

				cap.push_back(*extension_p++);
			}
		}

		// Enable debugging
		// glEnable(GL_DEBUG_OUTPUT);
		// glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
		// 	std::cout << "OpenGL Debug: " << message << std::endl;
		// }, nullptr);
	}

	SDL_Window *getWindow() const { return window; }
	SDL_GLContext getGLContext() const { return gl_context; }
	const GLCapabilities &getCapabilities() const { return capabilties; }
};

// Context singletons
static GLContext &getContext()
{
	static GLContext context;
	return context;
}

SDL_Window *getWindow()
{
	return getContext().getWindow();
}
SDL_GLContext getGLContext()
{
	return getContext().getGLContext();
}

}

// GL capabilities
void instantiate()
{
	detail::getContext();
}

const detail::GLCapabilities &getCapabilities()
{
	return detail::getContext().getCapabilities();
}

}
}
