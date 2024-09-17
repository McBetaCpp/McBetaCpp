#include "nbt/DoubleTag.h"

#include "java/IOUtil.h"

DoubleTag::DoubleTag()
{

}

DoubleTag::DoubleTag(double data)
{
	this->data = data;
}

void DoubleTag::write(std::ostream &os)
{
	IOUtil::writeDouble(os, data);
}

void DoubleTag::load(std::istream &is)
{
	data = IOUtil::readDouble(is);
}

byte_t DoubleTag::getId() const
{
	return TAG_Double;
}

jstring DoubleTag::toString() const
{
	return String::fromUTF8(std::to_string(data));
}
