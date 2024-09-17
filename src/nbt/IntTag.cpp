#include "nbt/IntTag.h"

#include "java/IOUtil.h"

IntTag::IntTag()
{

}

IntTag::IntTag(int_t data)
{
	this->data = data;
}

void IntTag::write(std::ostream &os)
{
	IOUtil::writeInt(os, data);
}

void IntTag::load(std::istream &is)
{
	data = IOUtil::readInt(is);
}

byte_t IntTag::getId() const
{
	return TAG_Int;
}

jstring IntTag::toString() const
{
	return String::fromUTF8(std::to_string(data));
}
