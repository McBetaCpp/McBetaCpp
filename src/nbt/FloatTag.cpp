#include "nbt/FloatTag.h"

#include "java/IOUtil.h"

FloatTag::FloatTag()
{

}

FloatTag::FloatTag(float data)
{
	this->data = data;
}

void FloatTag::write(std::ostream &os)
{
	IOUtil::writeFloat(os, data);
}

void FloatTag::load(std::istream &is)
{
	data = IOUtil::readFloat(is);
}

byte_t FloatTag::getId() const
{
	return TAG_Float;
}

jstring FloatTag::toString() const
{
	return String::fromUTF8(std::to_string(data));
}
