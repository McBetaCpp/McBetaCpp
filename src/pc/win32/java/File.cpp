#include "java/File.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShlObj_core.h>

#include <queue>
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>

#include "util/Memory.h"

static jstring FromWPath(const std::wstring &wstr)
{
	if (wstr.empty())
		return u"";

	// Convert wstring to u16string
	std::u16string u16str(wstr.begin(), wstr.end());

	// Remove prefix
	if (u16str.compare(0, 4, u"\\\\?\\") == 0)
		return u16str.substr(4);
	else
		return u16str;
}

static std::wstring ToWPath(const jstring &path)
{
	if (path.empty())
		return L"";

	// Convert u16string to wstring
	std::wstring wpath(path.begin(), path.end());

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
}

class File_Impl : public File
{
private:
	std::wstring wpath;

public:
	File_Impl(const jstring &path)
	{
		wpath = ToWPath(path);
		this->path = FromWPath(wpath);
	}

	virtual ~File_Impl()
	{

	}

	virtual bool createNewFile() const override
	{
		HANDLE hfile = CreateFileW(wpath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hfile == INVALID_HANDLE_VALUE)
			return false;

		CloseHandle(hfile);
		return true;
	
	}

	bool remove() const override
	{
		if (isDirectory())
			return RemoveDirectoryW(wpath.c_str()) != 0;
		else
			return DeleteFileW(wpath.c_str()) != 0;
	}

	bool renameTo(const File &dest) const override
	{
		const File_Impl &dest_impl = reinterpret_cast<const File_Impl&>(dest);
		if (MoveFileW(wpath.c_str(), dest_impl.wpath.c_str()) == 0)
			return false;
		return true;
	}

	bool exists() const override
	{
		DWORD attrib = GetFileAttributesW(wpath.c_str());
		return attrib != INVALID_FILE_ATTRIBUTES;
	}

	bool isDirectory() const override
	{
		DWORD attrib = GetFileAttributesW(wpath.c_str());
		return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	bool isFile() const override
	{
		DWORD attrib = GetFileAttributesW(wpath.c_str());
		return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}

	long_t lastModified() const override
	{
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
	}

	long_t length() const override
	{
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
	}

	std::vector<std::unique_ptr<File>> listFiles() const override
	{
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
			files.push_back(Util::make_unique<File_Impl>(FromWPath(child_path)));
		} while (FindNextFileW(hfind, &find_data) != 0);

		FindClose(hfind);
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
		return CreateDirectoryW(wpath.c_str(), nullptr) != 0;
	}

	std::istream *toStreamIn() const override
	{
		auto is = Util::make_unique<std::ifstream>(wpath, std::ios::binary);
		if (!is->is_open() || !is->good())
			return nullptr;
		return is.release();
	}

	std::ostream *toStreamOut() const override
	{
		auto os = Util::make_unique<std::ofstream>(wpath, std::ios::binary);
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
	jstring new_path = parent.path + u'\\' + child;
	return new File_Impl(new_path);
}

File *File::openResourceDirectory()
{
	// Get the path to the executable
	std::wstring path(MAX_PATH, 0);
	while (1)
	{
		DWORD length = GetModuleFileNameW(nullptr, &path.front(), static_cast<DWORD>(path.size()));
		if (length < path.size())
		{
			path.resize(length);
			break;
		}
		path.resize(path.size() * 2);
	}

	// Convert to UTF-16
	jstring u16str = FromWPath(path);

	// Remove the executable name
	size_t pos = u16str.find_last_of(u"/\\");
	if (pos == std::string::npos)
		return new File_Impl(u"");

	// Return resource directory
	return new File_Impl(u16str.substr(0, pos) + u"\\resource");
}

File *File::openWorkingDirectory(const jstring &name)
{
	// Get appdata directory
	wchar_t path[MAX_PATH] = {};
	if (SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, path) != S_OK)
		return new File_Impl(u"");

	// Convert to UTF-16
	jstring u16str = FromWPath(path);

	return new File_Impl(u16str + u"\\" + name);
}
