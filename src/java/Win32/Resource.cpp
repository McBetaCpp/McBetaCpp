#include "java/Resource.h"

#include <algorithm>
#include <fstream>
#include <cassert>
#include <memory>
#include <stdexcept>

#include "java/File.h"

#include "util/Memory.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef std::basic_string<WCHAR> tstring;

static jstring GetResourceRoot()
{
	// Get the path to the executable
	tstring path(MAX_PATH, 0);
	while (1)
	{
		DWORD length = GetModuleFileNameW(NULL, &path.front(), static_cast<DWORD>(path.size()));
		if (length < path.size())
		{
			path.resize(length);
			break;
		}
		path.resize(path.size() * 2);
	}

	// Convert to UTF-16
	jstring u16str(path.begin(), path.end());

	// Remove the executable name
	size_t pos = u16str.find_last_of(u"/\\");
	if (pos == std::string::npos)
		return u"";

	return u16str.substr(0, pos);
}

static const File &ResourceFile()
{
	static std::unique_ptr<File> resource_file(File::open(GetResourceRoot() + u"/resource"));
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
