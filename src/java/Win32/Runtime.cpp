#include "java/Runtime.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>
#else
#include <sys/sysinfo.h>   // For sysinfo()
#include <sys/resource.h>  // For getrusage()
#include <unistd.h>        // For getpid()
#include <fstream>
#include <sstream>
#endif

Runtime Runtime::instance;

Runtime &Runtime::getRuntime()
{
	return instance;
}

long_t Runtime::maxMemory()
{
    #ifdef _WIN32
	// Get amount of system memory available
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	return statex.ullTotalPhys;
	#else
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return info.totalram * info.mem_unit;
    }
    return 0;
	#endif
}

long_t Runtime::totalMemory()
{
    #ifdef _WIN32
	// Get memory that the application currently has paged
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	return pmc.WorkingSetSize;
	#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss * 1024; // Convert from KB to bytes
    }
    return 0;
	#endif
}

long_t Runtime::freeMemory()
{
    #ifdef _WIN32
	// Get memory out of the total memory that is not currently used
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	return 0;
	#else
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return info.freeram * info.mem_unit;
    }
    return 0;
	#endif
}
