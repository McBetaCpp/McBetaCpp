#pragma once

#include <iostream>
#include <vector>

#include "java/Type.h"
#include "java/String.h"

namespace IOUtil
{

void writeBoolean(std::ostream &os, bool b);
void writeByte(std::ostream &os, int_t b);
void writeChar(std::ostream &os, int_t c);
void writeShort(std::ostream &os, int_t s);
void writeInt(std::ostream &os, int_t i);
void writeLong(std::ostream &os, long_t l);
void writeFloat(std::ostream &os, float f);
void writeDouble(std::ostream &os, double d);
void writeUTF(std::ostream &os, const jstring &str);

bool readBoolean(std::istream &is);
byte_t readByte(std::istream &is);
char_t readChar(std::istream &is);
short_t readShort(std::istream &is);
int_t readInt(std::istream &is);
long_t readLong(std::istream &is);
float readFloat(std::istream &is);
double readDouble(std::istream &is);
jstring readUTF(std::istream &is);

std::vector<char> readAllBytes(std::istream &is);

}
