#pragma once

#include <string>

namespace CrashHandler
{

void Crash(const std::string &message, const std::string& stackTrace = "");

}
