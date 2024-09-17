#include "nbt/StringTag.h"

#include "java/IOUtil.h"

StringTag::StringTag()
{

}

StringTag::StringTag(const jstring &data)
{
	this->data = data;
}

void StringTag::write(std::ostream &os)
{
	IOUtil::writeUTF(os, data);
}

void StringTag::load(std::istream &is)
{
	data = IOUtil::readUTF(is);
}

byte_t StringTag::getId() const
{
	return TAG_String;
}

jstring StringTag::toString() const
{
	return data;
}
