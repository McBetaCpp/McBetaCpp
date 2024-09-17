#include "java/IOUtil.h"

namespace IOUtil
{

void writeBoolean(std::ostream &os, bool b)
{
	os.put(b ? 1 : 0);
}
void writeByte(std::ostream &os, int_t b)
{
	os.put(b);
}
void writeChar(std::ostream &os, int_t c)
{
	os.put(c >> 8);
	os.put(c >> 0);
}
void writeShort(std::ostream &os, int_t s)
{
	os.put(s >> 8);
	os.put(s >> 0);
}
void writeInt(std::ostream &os, int_t i)
{
	os.put(i >> 24);
	os.put(i >> 16);
	os.put(i >> 8);
	os.put(i >> 0);
}
void writeLong(std::ostream &os, long_t l)
{
	os.put(l >> 56);
	os.put(l >> 48);
	os.put(l >> 40);
	os.put(l >> 32);
	os.put(l >> 24);
	os.put(l >> 16);
	os.put(l >> 8);
	os.put(l >> 0);
}
void writeFloat(std::ostream &os, float f)
{
	int_t fi = *(reinterpret_cast<int_t*>(&f));
	writeInt(os, fi);
}
void writeDouble(std::ostream &os, double d)
{
	long_t di = *(reinterpret_cast<long_t *>(&d));
	writeLong(os, di);
}
void writeUTF(std::ostream &os, const jstring &str)
{
	std::string u8str = String::toUTF8(str);
	writeShort(os, u8str.length());
	os.write(u8str.c_str(), u8str.length());
}

bool readBoolean(std::istream &is)
{
	return is.get() != 0;
}

byte_t readByte(std::istream &is)
{
	return is.get();
}

char_t readChar(std::istream &is)
{
	return (is.get() << 8) | is.get();
}

short_t readShort(std::istream &is)
{
	return (is.get() << 8) | is.get();
}

int_t readInt(std::istream &is)
{
	return (is.get() << 24) | (is.get() << 16) | (is.get() << 8) | is.get();
}

long_t readLong(std::istream &is)
{
	return ((long_t)is.get() << 56) | ((long_t)is.get() << 48) | ((long_t)is.get() << 40) | ((long_t)is.get() << 32) | ((long_t)is.get() << 24) | ((long_t)is.get() << 16) | ((long_t)is.get() << 8) | (long_t)is.get();
}

float readFloat(std::istream &is)
{
	int_t fi = readInt(is);
	return *(reinterpret_cast<float *>(&fi));
}

double readDouble(std::istream &is)
{
	long_t di = readLong(is);
	return *(reinterpret_cast<double *>(&di));
}

jstring readUTF(std::istream &is)
{
	short_t len = readShort(is);
	std::string s(len, 0);
	is.read(&s[0], len);
	jstring str = String::fromUTF8(s);
	return str;
}

std::vector<char> readAllBytes(std::istream &is)
{
	is.seekg(0, std::ios::end);
	std::streampos size = is.tellg();
	is.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	is.read(buffer.data(), size);
	return buffer;
}

}
