#include "nbt/ShortTag.h"

#include "java/IOUtil.h"

ShortTag::ShortTag()
{

}

ShortTag::ShortTag(short_t data)
{
	this->data = data;
}

void ShortTag::write(std::ostream &os)
{
	IOUtil::writeShort(os, data);
}

void ShortTag::load(std::istream &is)
{
	data = IOUtil::readShort(is);
}

byte_t ShortTag::getId() const
{
	return TAG_Short;
}

jstring ShortTag::toString() const
{
	return String::fromUTF8(std::to_string(data));
}
