#include "java/File.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlobj_core.h>

#include <queue>
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>

static jstring FromWPath(const std::wstring &wstr)
{
	/*
	// Convert wide path to string
	int length = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (length == 0)
		throw std::runtime_error("Failed to convert wide path to string");

	std::string str(length - 1, 0);
	if (WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], length, nullptr, nullptr) == 0)
		throw std::runtime_error("Failed to convert wide path to string");

	// Remove prefix
	return str.substr(4);
	*/
	// Convert wstring to u16string
	std::u16string u16str(wstr.begin(), wstr.end());

	// Remove prefix
	return u16str.substr(4);
}

static std::wstring ToWPath(const jstring &path)
{
	/*
	// Convert path to wide string
	int length = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
	if (length == 0)
		throw std::runtime_error("Failed to convert path to wide string: " + path);

	std::wstring wpath(length, 0);
	if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, &wpath[0], length) == 0)
		throw std::runtime_error("Failed to convert path to wide string: " + path);
	*/

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
		this->wpath = ToWPath(path);
		this->path = FromWPath(this->wpath);
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
			files.push_back(std::make_unique<File_Impl>(FromWPath(child_path)));
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
		auto is = std::make_unique<std::ifstream>(wpath, std::ios::binary);
		if (!is->is_open() || !is->good())
			return nullptr;
		return is.release();
	}

	std::ostream *toStreamOut() const override
	{
		auto os = std::make_unique<std::ofstream>(wpath, std::ios::binary);
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
