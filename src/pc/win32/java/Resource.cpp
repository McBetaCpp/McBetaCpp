#include "java/Resource.h"

#include <algorithm>
#include <fstream>
#include <cassert>
#include <memory>
#include <stdexcept>

#include "java/File.h"

#include "util/Memory.h"

static const File &ResourceFile()
{
	static std::unique_ptr<File> resource_file(File::openResourceDirectory());
	return *resource_file;
}

namespace Resource
{

std::istream *getResource(const jstring &name)
{
	const File &resource_file = ResourceFile();
	std::unique_ptr<File> file(File::open(resource_file, name));
	std::unique_ptr<std::istream> is(file->toStreamIn());
	if (!is)
		throw std::runtime_error("Failed to open resource " + String::toUTF8(name));
	return is.release();
}

}
