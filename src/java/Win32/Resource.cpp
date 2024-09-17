#include "java/Resource.h"
#include "java/Win32/conversion.h"

#include <algorithm>
#include <fstream>
#include <cassert>
#include <stdexcept>
#include <memory>

#include "java/File.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#ifdef _WIN32
typedef std::basic_string<WCHAR> tstring;
#endif

static jstring GetResourceRoot()
{
    #ifdef _WIN32
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
	#else
    char path[PATH_MAX];

    // Get the path to the executable
    ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (length == -1) {
        return String::fromUTF8(std::string("")); // Error occurred
    }
    path[length] = '\0'; // Null-terminate the string

    // Convert the path from UTF-8 to std::wstring if needed
    std::string utf8_path(path);
    std::wstring wpath = str_to_wstr(utf8_path);

    // Remove the executable name
    std::wstring::size_type pos = wpath.find_last_of(L"/\\");
    if (pos == std::wstring::npos) {
        return String::fromUTF8(std::string("")); // No directory part found
    }

    // Extract the directory path and convert back to UTF-8
    std::wstring dir_path = wpath.substr(0, pos);
    std::string result = wstr_to_str(dir_path);
    std::cout << "Parent dir: " << result << "\n";

    return String::fromUTF8(result);
	#endif
}

static const File &ResourceFile()
{
    auto resource_path = GetResourceRoot() + u"/resource";
    std::cout << "Opening resource dir: " << String::toUTF8(resource_path) << "\n";
	static std::unique_ptr<File> resource_file(File::open(resource_path));
	return *resource_file;
}

namespace Resource
{

std::istream *getResource(const jstring &name)
{
	const File &resource_file = ResourceFile();
	std::cout << "Opening file " << String::toUTF8(name) << " at " << String::toUTF8(resource_file.path) << "\n";
	std::unique_ptr<File> file(File::open(resource_file, name));
	std::unique_ptr<std::istream> is(file->toStreamIn());
	if (!is)
		throw std::runtime_error("Failed to open resource " + String::toUTF8(name));
	return is.release();
}

}
