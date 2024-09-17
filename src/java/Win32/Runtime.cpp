#include "java/Runtime.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>

Runtime Runtime::instance;

Runtime &Runtime::getRuntime()
{
	return instance;
}

long_t Runtime::maxMemory()
{
	// Get amount of system memory available
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	return statex.ullTotalPhys;
}

long_t Runtime::totalMemory()
{
	// Get memory that the application currently has paged
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	return pmc.WorkingSetSize;
}

long_t Runtime::freeMemory()
{
	// Get memory out of the total memory that is not currently used
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	return 0;
}
