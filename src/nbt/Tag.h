#pragma once

#include <string>
#include <iostream>
#include <memory>

#include "java/Type.h"
#include "java/String.h"

#include "util/Memory.h"

class Tag
{
public:
	enum TAG
	{
		TAG_End = 0,
		TAG_Byte = 1,
		TAG_Short = 2,
		TAG_Int = 3,
		TAG_Long = 4,
		TAG_Float = 5,
		TAG_Double = 6,
		TAG_Byte_Array = 7,
		TAG_String = 8,
		TAG_List = 9,
		TAG_Compound = 10,
	};

private:
	jstring name;

public:
	virtual ~Tag() {}

	virtual void write(std::ostream &os) = 0;
	virtual void load(std::istream &is) = 0;

	virtual jstring toString() const = 0;
	virtual byte_t getId() const = 0;

	void print(std::ostream &os) const;
	virtual void print(const std::string &indent, std::ostream &os) const;

	jstring getName() const;
	Tag &setName(const jstring &name);

	static Tag *readNamedTag(std::istream &is);
	static void writeNamedTag(Tag &tag, std::ostream &os);

	static Tag *newTag(byte_t type);
	static jstring getTagName(byte_t type);
};
