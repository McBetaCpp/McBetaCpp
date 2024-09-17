#include "nbt/NbtIo.h"

#include <sstream>

#include "java/IOUtil.h"

#include "gzip/decompress.hpp"
#include "gzip/compress.hpp"

class membuf : public std::basic_streambuf<char>
{
public:
	membuf(const char *p, size_t l)
	{
		setg(const_cast<char*>(p), const_cast<char *>(p), const_cast<char *>(p) + l);
	}
};

class memstream : public std::istream
{
public:
	memstream(const char *p, size_t l) : std::istream(&_buffer), _buffer(p, l)
	{
		rdbuf(&_buffer);
	}

private:
	membuf _buffer;
};

namespace NbtIo
{

CompoundTag *readCompressed(std::istream &is)
{
	std::vector<char> buffer = IOUtil::readAllBytes(is);
	std::string decompressed = gzip::decompress(buffer.data(), buffer.size());
	std::unique_ptr<memstream> ms = std::make_unique<memstream>(decompressed.data(), decompressed.size());
	return read(*ms);
}

CompoundTag *read(std::istream &is)
{
	Tag *tag = Tag::readNamedTag(is);
	if (tag->getId() != Tag::TAG_Compound)
		throw std::runtime_error("Root tag must be a named compound tag");
	return reinterpret_cast<CompoundTag *>(tag);
}

void writeCompressed(CompoundTag &tag, std::ostream &os)
{
	std::stringstream ss;
	write(tag, ss);
	std::string sstr = ss.str();
	std::string compressed = gzip::compress(sstr.data(), sstr.size());
	os.write(compressed.data(), compressed.size());
}

void write(CompoundTag &tag, std::ostream &os)
{
	Tag::writeNamedTag(tag, os);
}

}
