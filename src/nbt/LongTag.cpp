#include "nbt/LongTag.h"

#include "java/IOUtil.h"

LongTag::LongTag()
{

}

LongTag::LongTag(long_t data)
{
	this->data = data;
}

void LongTag::write(std::ostream &os)
{
	IOUtil::writeLong(os, data);
}

void LongTag::load(std::istream &is)
{
	data = IOUtil::readLong(is);
}

byte_t LongTag::getId() const
{
	return TAG_Long;
}

jstring LongTag::toString() const
{
	return String::fromUTF8(std::to_string(data));
}
