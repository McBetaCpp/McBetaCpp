#include "java/File.h"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <shlobj_core.h>
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <dirent.h>
    #include <cstring>
#endif

#include <queue>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <ctime>

std::string wstr_to_str(const std::wstring& wstr) {
    std::string str(wstr.begin(), wstr.end());
    return str;
}

std::wstring str_to_wstr(const std::string& str) {
     std::wstring str2(str.length(), L' ');
     std::copy(str.begin(), str.end(), str2.begin());
     return str2;
}

static jstring FromWPath(const std::wstring &wstr)
{
	// Convert wstring to u16string
	std::u16string u16str(wstr.begin(), wstr.end());

	// Remove prefix
	return u16str;
}

static std::wstring ToWPath(const jstring &path)
{
	// Convert u16string to wstring
	std::wstring wpath(path.begin(), path.end());

    #ifdef _WIN32
	wchar_t temp[1] = {};
	size_t out_size = GetFullPathNameW(wpath.c_str(), 0, temp, nullptr);
	if (out_size == 0)
		throw std::runtime_error("Failed to convert wide string to canonical path: " + String::toUTF8(path));

	std::wstring out_path(out_size - 1, 0);
	if (GetFullPathNameW(wpath.c_str(), out_size, &out_path[0], nullptr) == 0)
		throw std::runtime_error("Failed to convert wide string to canonical path: " + String::toUTF8(path));

	out_path.resize(out_size - 1);

	// Remove trailing slashes
	while (!out_path.empty() && (out_path.back() == L'\\' || out_path.back() == L'/'))
		out_path.pop_back();

	return L"\\\\?\\" + out_path;
	#else

    std::string cpath = wstr_to_str(wpath);
    std::string resolved_path(cpath.length(), '\0');

    // Get the absolute path (canonicalized)
    char* result = realpath(cpath.c_str(), &resolved_path[0]);
    std::cout << "PATH( " << cpath << ") is the path\n";
    if (result == nullptr) {
        std::cout << "ERROR( " << strerror(errno) << ") is the error\n";
        resolved_path = cpath;
//        throw std::runtime_error("Failed to convert path to canonical path: " + cpath);
    } else {

    // Resize the string to the correct length
    resolved_path.resize(std::strlen(result));
    }

    // Remove trailing slashes
    while (!resolved_path.empty() && (resolved_path.back() == '/' || resolved_path.back() == '\\')) {
        resolved_path.pop_back();
    }

    std::wstring resolvedw(resolved_path.length(), L' '); // Make room for characters

    // Copy string to wstring.
    std::copy(resolved_path.begin(), resolved_path.end(), resolvedw.begin());

    return resolvedw;
	#endif
}

class File_Impl : public File
{
private:
	std::wstring wpath;

public:
	File_Impl(const jstring &path)
	{
		this->wpath = ToWPath(path);
		this->path = FromWPath(this->wpath);
	}

	virtual ~File_Impl()
	{

	}

	virtual bool createNewFile() const
	{
	    #ifdef _WIN32
		HANDLE hfile = CreateFileW(wpath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hfile == INVALID_HANDLE_VALUE)
			return false;

		CloseHandle(hfile);
		return true;
		#else
		// incomplete
		std::string path = wstr_to_str(wpath);
	    int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            // If the file already exists or there is another error, return false
            return false;
        }

        close(fd);
        return true;
        #endif
	}

	bool remove() const override
	{
	    #ifdef _WIN32
		if (isDirectory())
			return RemoveDirectoryW(wpath.c_str()) != 0;
		else
			return DeleteFileW(wpath.c_str()) != 0;
		#else
		std::string path = wstr_to_str(wpath);
	    if (isDirectory()) {
            if (rmdir(path.c_str()) != 0) {
                return false;
            }
        } else {
            if (unlink(path.c_str()) != 0) {
                return false;
            }
        }
        return false;
		#endif
	}

	bool renameTo(const File &dest) const override
	{
	const File_Impl& dest_impl = reinterpret_cast<const File_Impl&>(dest);
    #ifdef _WIN32
        if (MoveFileW(wpath.c_str(), dest_impl.wpath.c_str()) == 0)
            return false;
        return true;
    #else
        std::string path = wstr_to_str(wpath); // Convert wide string to narrow string for Linux
        std::string dest_path = wstr_to_str(dest_impl.wpath); // Convert destination wide string to narrow string

        if (rename(path.c_str(), dest_path.c_str()) != 0) {
            // Handle errors: you can log or throw an exception if needed
            return false;
        }
        return true;
    #endif
	}

	bool exists() const override
	{
	    #ifdef _WIN32
		DWORD attrib = GetFileAttributesW(wpath.c_str());
		return attrib != INVALID_FILE_ATTRIBUTES;
		#else
	    struct stat buf;
        return stat(wstr_to_str(wpath).c_str(), &buf) == 0;
		#endif
	}

	bool isDirectory() const override
	{
	    #ifdef _WIN32
		DWORD attrib = GetFileAttributesW(wpath.c_str());
		return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
		#else
	    struct stat buf;
        if (stat(wstr_to_str(wpath).c_str(), &buf) != 0) {
            return false;
        }
        return S_ISDIR(buf.st_mode);
		#endif
	}

	bool isFile() const override
	{
	    #ifdef _WIN32
		DWORD attrib = GetFileAttributesW(wpath.c_str());
		return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY) == 0;
		#else
        struct stat buf;
        if (stat(wstr_to_str(wpath).c_str(), &buf) != 0) {
            return false;
        }
        return S_ISREG(buf.st_mode);
		#endif
	}

	long_t lastModified() const override
	{
	    #ifdef _WIN32
		HANDLE hfile = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hfile == INVALID_HANDLE_VALUE)
			return 0;

		FILETIME ftwrite;
		if (!GetFileTime(hfile, nullptr, nullptr, &ftwrite))
			return 0;

		CloseHandle(hfile);

		ULARGE_INTEGER uli;
		uli.HighPart = ftwrite.dwHighDateTime;
		uli.LowPart = ftwrite.dwLowDateTime;

		ULONGLONG ms = uli.QuadPart / 10000;
		ULONGLONG epoch = 11644473600000; // 1970-01-01

		if (ms < epoch)
			return 0;
		else
			return ms - epoch;
		#else
	    struct stat buf;
        if (stat(wstr_to_str(wpath).c_str(), &buf) != 0) {
            return 0;
        }
        // Convert to milliseconds since epoch
        auto time_since_epoch = std::chrono::system_clock::from_time_t(buf.st_mtime);
        auto duration = time_since_epoch.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		#endif
	}

	long_t length() const override
	{
	    #ifdef _WIN32
		HANDLE hfile = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hfile == INVALID_HANDLE_VALUE)
			return 0;

		LARGE_INTEGER size;
		if (!GetFileSizeEx(hfile, &size))
		{
			CloseHandle(hfile);
			return 0;
		}

		CloseHandle(hfile);
		return size.QuadPart;
		#else
	    struct stat buf;
        if (stat(wstr_to_str(wpath).c_str(), &buf) != 0) {
            return 0;
        }
        return buf.st_size;
		#endif
	}

	std::vector<std::unique_ptr<File>> listFiles() const override
	{
	    #ifdef _WIN32
		std::vector<std::unique_ptr<File>> files;

		if (!isDirectory())
			return files;

		WIN32_FIND_DATAW find_data;
		HANDLE hfind = FindFirstFileW((wpath + L"\\*").c_str(), &find_data);
		if (hfind == INVALID_HANDLE_VALUE)
			return files;

		do
		{
			if (wcscmp(find_data.cFileName, L".") == 0 || wcscmp(find_data.cFileName, L"..") == 0)
				continue;

			std::wstring child_path = wpath + L'\\' + find_data.cFileName;
			files.push_back(std::unique_ptr<File_Impl>(new File_Impl(FromWPath(child_path))));
		} while (FindNextFileW(hfind, &find_data) != 0);

		FindClose(hfind);
		return files;
		#else
	    std::vector<std::unique_ptr<File>> files;

        if (!isDirectory()) {
            return files;
        }

        DIR* dir = opendir(wstr_to_str(wpath).c_str());
        if (dir == nullptr) {
            return files;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            // Skip the special entries "." and ".."
            if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            std::wstring child_path = wpath + L'/' + std::wstring(entry->d_name, entry->d_name + std::strlen(entry->d_name));
            files.push_back(std::unique_ptr<File_Impl>(new File_Impl(FromWPath(child_path))));
        }

        closedir(dir);
        return files;
		#endif
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
	    #ifdef _WIN32
		return CreateDirectoryW(wpath.c_str(), nullptr) != 0;
		#else
	    std::string path = wstr_to_str(wpath);

        // Use mkdir to create the directory
        // S_IRWXU | S_IRWXG | S_IRWXO gives read, write, and execute permissions to user, group, and others
        // Adjust the permissions as needed
        return ::mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
		#endif
	}

	std::istream *toStreamIn() const override
	{
	    #ifdef _WIN32
		auto is = std::unique_ptr<std::ifstream>(new std::ifstream(wpath, std::ios::binary));
		#else
		std::string path = wstr_to_str(wpath);
		auto is = std::unique_ptr<std::ifstream>(new std::ifstream(path, std::ios::binary));
		#endif
		if (!is->is_open() || !is->good())
			return nullptr;
		return is.release();
	}

	std::ostream *toStreamOut() const override
	{
	    #ifdef _WIN32
		auto os = std::unique_ptr<std::ofstream>(new std::ofstream(wpath, std::ios::binary));
		#else
		std::string path = wstr_to_str(wpath);
		auto os = std::unique_ptr<std::ofstream>(new std::ofstream(path, std::ios::binary));
		#endif
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
    std::string str = String::toUTF8(child);
    if (!str.empty() && str.front() == '/') {
        str.erase(0, 1); // Remove the first character (slash)
    }
    std::cout << "File::Open at path " << String::toUTF8(parent.path) << " / " << str << "\n";
	jstring new_path = parent.path + u'/' + String::fromUTF8(str);
	return new File_Impl(new_path);
}
