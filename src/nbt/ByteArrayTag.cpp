#include "nbt/ByteArrayTag.h"

#include "java/IOUtil.h"

ByteArrayTag::ByteArrayTag()
{

}

ByteArrayTag::ByteArrayTag(std::vector<byte_t> &&data)
{
	this->data = data;
}

void ByteArrayTag::write(std::ostream &os)
{
	IOUtil::writeInt(os, data.size());
	os.write(reinterpret_cast<const char*>(data.data()), data.size());
}

void ByteArrayTag::load(std::istream &is)
{
	size_t size = IOUtil::readInt(is);
	data.resize(size);
	is.read(reinterpret_cast<char*>(data.data()), size);
}

byte_t ByteArrayTag::getId() const
{
	return TAG_Byte_Array;
}

jstring ByteArrayTag::toString() const
{
	return u"[" + String::fromUTF8(std::to_string(data.size())) + u" bytes]";
}
