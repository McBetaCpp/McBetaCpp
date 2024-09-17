#pragma once

#include "java/Type.h"

class Runtime
{
private:
	static Runtime instance;

public:
	static Runtime &getRuntime();

	long_t maxMemory();
	long_t totalMemory();
	long_t freeMemory();
};
