#pragma once

#include <string>

#include "java/Type.h"

typedef std::u16string jstring;

namespace String
{

jstring fromUTF8(const std::string &str);
std::string toUTF8(const jstring &str);

jstring toString(int_t v, int_t base = 10);
jstring toString(long_t v, int_t base = 10);
jstring toString(uint_t v, int_t base = 10);
jstring toString(ulong_t v, int_t base = 10);
jstring toString(float v);
jstring toString(double v);

}
