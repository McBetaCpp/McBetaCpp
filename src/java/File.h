#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "java/Type.h"
#include "java/String.h"

class File
{
protected:
	jstring path;

public:
	virtual ~File() {}

	static File *open(const jstring &path);
	static File *open(const File &parent, const jstring &child);

	static File *openResourceDirectory();
	static File *openWorkingDirectory(const jstring &name);
	
	virtual bool createNewFile() const = 0;
	virtual bool remove() const = 0;

	virtual bool renameTo(const File &dest) const = 0;

	virtual bool exists() const = 0;
	virtual bool isDirectory() const = 0;
	virtual bool isFile() const = 0;

	virtual long_t lastModified() const = 0;
	virtual long_t length() const = 0;

	virtual std::vector<std::unique_ptr<File>> listFiles() const = 0;
	virtual File *getParentFile() const = 0;

	virtual bool mkdir() const = 0;
	bool mkdirs() const;

	jstring getName() const;
	jstring toString() const;
	jstring toURL() const;

	virtual std::istream *toStreamIn() const = 0;
	virtual std::ostream *toStreamOut() const = 0;
};
