#pragma once

class OpenGLCapabilities
{
private:
	static bool USE_OCCLUSION_QUERY;

public:
	bool hasOcclusionChecks();
};
