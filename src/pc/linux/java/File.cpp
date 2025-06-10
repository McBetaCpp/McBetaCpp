#include "java/File.h"

#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "util/Memory.h"

static std::string ToPath(const jstring &path)
{
	std::string u8path = String::toUTF8(path);

	static char buffer[PATH_MAX];
	if (!::realpath(u8path.c_str(), buffer))
		return u8path; // File probably doesn't exist yet, just use the path as-is
	
	return std::string(buffer);
}

static jstring FromPath(const std::string &path)
{
	return String::fromUTF8(path);
}

class File_Impl : public File
{
private:
	std::string u8path;

public:
	File_Impl(const jstring &path)
	{
		u8path = ToPath(path);
		this->path = FromPath(u8path);
		std::cout << "Open " << u8path << std::endl;
	}

	virtual ~File_Impl()
	{

	}

	virtual bool createNewFile() const override
	{
		int fd = ::open(u8path.c_str(), O_CREAT | O_RDWR, 0644);
		if (fd < 0)
			return false;
		::close(fd);
		return true;
	
	}

	bool remove() const override
	{
		if (isDirectory())
			return ::rmdir(u8path.c_str()) == 0;
		else
			return ::unlink(u8path.c_str()) == 0;
	}

	bool renameTo(const File &dest) const override
	{
		const File_Impl &dest_impl = reinterpret_cast<const File_Impl&>(dest);
		return ::rename(u8path.c_str(), dest_impl.u8path.c_str()) == 0;
	}

	bool exists() const override
	{
		struct ::stat buffer;
		return ::stat(u8path.c_str(), &buffer) == 0;
	}

	bool isDirectory() const override
	{
		struct ::stat buffer;
		if (::stat(u8path.c_str(), &buffer) != 0)
			return false;
		return S_ISDIR(buffer.st_mode);
	}

	bool isFile() const override
	{
		struct ::stat buffer;
		if (::stat(u8path.c_str(), &buffer) != 0)
			return false;
		return S_ISREG(buffer.st_mode);
	}

	long_t lastModified() const override
	{
		struct ::stat buffer;
		if (::stat(u8path.c_str(), &buffer) != 0)
			return false;
		
    /* We check that st_mtime is a macro here in order to give us confidence
     * that struct stat has a struct timespec st_mtim member. We need this
     * check because there are some platforms that claim to be POSIX 2008
     * compliant but which do not have st_mtim... */
	#if (PLATFORM_POSIX_VERSION >= 200809L) && defined(st_mtime)
        return buffer.st_mtim.tv_sec * 1000LL + buffer.st_mtim.tv_nsec / 1000000LL;
	#else
		return buffer.st_mtime * 1000LL + buffer.st_mtimespec.tv_nsec / 1000000LL;
	#endif
	}

	long_t length() const override
	{
		struct ::stat buffer;
		if (::stat(u8path.c_str(), &buffer) != 0)
			return false;
		return buffer.st_size;
	}

	std::vector<std::unique_ptr<File>> listFiles() const override
	{
		std::vector<std::unique_ptr<File>> files;

		if (!isDirectory())
			return files;

		::DIR *dir = ::opendir(u8path.c_str());
		if (!dir)
			return files;

		struct dirent *entry;
		while ((entry = readdir(dir)) != nullptr)
		{
			// Skip "." and ".."
			if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
				continue;

			std::string child_path = u8path + "/" + entry->d_name;
			files.push_back(Util::make_unique<File_Impl>(FromPath(child_path)));
		}

		::closedir(dir);
		return files;
	}

	File *getParentFile() const override
	{
		size_t npos = path.find_last_of(u"/\\");
		if (npos != std::string::npos)
			return new File_Impl(path.substr(0, npos));
		return new File_Impl(u"");
	}

	bool mkdir() const override
	{
		return ::mkdir(u8path.c_str(), 0755) == 0;
	}

	std::istream *toStreamIn() const override
	{
		auto is = Util::make_unique<std::ifstream>(u8path, std::ios::binary);
		if (!is->is_open() || !is->good())
			return nullptr;
		return is.release();
	}

	std::ostream *toStreamOut() const override
	{
		auto os = Util::make_unique<std::ofstream>(u8path, std::ios::binary);
		if (!os->is_open() || !os->good())
			return nullptr;
		return os.release();
	}

	friend File *File::open(const jstring &path);
	friend File *File::open(const File &parent, const jstring &child);
};

File *File::open(const jstring &path)
{
	return new File_Impl(path);
}

File *File::open(const File &parent, const jstring &child)
{
	jstring new_path = parent.path + u'/' + child;
	return new File_Impl(new_path);
}

File *File::openResourceDirectory()
{
	// Get the path to the executable
	char (*path) = (char*)malloc(PATH_MAX);
	uint32_t length = PATH_MAX;
	#ifdef __APPLE__
	if (_NSGetExecutablePath(path, &length) != 0)
	{
	  	// Buffer size is too small.
		length = -1;
	}
	#else
	length = ::readlink("/proc/self/exe", path, PATH_MAX - 1);
	#endif
	if (length == -1)
		return new File_Impl(u"");

	path[length] = '\0';

	// Convert to UTF-16
	jstring u16str = FromPath(path);

	// Remove the executable name
	size_t pos = u16str.find_last_of(u'/');
	if (pos == std::string::npos)
		return new File_Impl(u"");

	// Return resource directory
	File* file = new File_Impl(u16str.substr(0, pos) + u"/resource");
	free(path);
	return file;
}

File *File::openWorkingDirectory(const jstring &name)
{
	// Get the home directory environment variable
	const char *path = ::getenv("HOME");
	if (path == nullptr)
		return new File_Impl(u"");

	// Convert to UTF-16
	jstring u16str = FromPath(path);

	return new File_Impl(u16str + u"/" + name);
}
