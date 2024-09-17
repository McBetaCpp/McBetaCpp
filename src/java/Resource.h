#pragma once

#include <iostream>

#include "java/String.h"

namespace Resource
{

std::istream *getResource(const jstring &name);

}
