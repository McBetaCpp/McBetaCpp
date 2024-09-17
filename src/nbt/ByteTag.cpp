#include "nbt/ByteTag.h"

#include "java/IOUtil.h"

ByteTag::ByteTag()
{

}

ByteTag::ByteTag(byte_t data)
{
	this->data = data;
}

void ByteTag::write(std::ostream &os)
{
	IOUtil::writeByte(os, data);
}

void ByteTag::load(std::istream &is)
{
	data = IOUtil::readByte(is);
}

byte_t ByteTag::getId() const
{
	return TAG_Byte;
}

jstring ByteTag::toString() const
{
	return String::fromUTF8(std::to_string(data));
}
