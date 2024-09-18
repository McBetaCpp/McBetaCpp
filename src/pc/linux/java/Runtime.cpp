#include "java/Runtime.h"

Runtime Runtime::instance;

Runtime &Runtime::getRuntime()
{
	return instance;
}

long_t Runtime::maxMemory()
{
	return 2;
}

long_t Runtime::totalMemory()
{
	return 1;
}

long_t Runtime::freeMemory()
{
	return 0;
}
