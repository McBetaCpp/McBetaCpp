#include "client/OpenGLCapabilities.h"

#include "lwjgl/GLContext.h"

bool OpenGLCapabilities::USE_OCCLUSION_QUERY = false;

bool OpenGLCapabilities::hasOcclusionChecks()
{
	return USE_OCCLUSION_QUERY && lwjgl::GLContext::getCapabilities()["GL_ARB_occlusion_query"];
}
