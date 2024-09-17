#include "nbt/Tag.h"

#include <memory>

#include "nbt/EndTag.h"
#include "nbt/ByteTag.h"
#include "nbt/ShortTag.h"
#include "nbt/IntTag.h"
#include "nbt/LongTag.h"
#include "nbt/FloatTag.h"
#include "nbt/DoubleTag.h"
#include "nbt/ByteArrayTag.h"
#include "nbt/StringTag.h"
#include "nbt/ListTag.h"
#include "nbt/CompoundTag.h"

#include "java/IOUtil.h"

void Tag::print(std::ostream &os) const
{
	print("", os);
}

void Tag::print(const std::string &indent, std::ostream &os) const
{
	jstring str = getName();
	os << indent;
	os << String::toUTF8(getTagName(getId()));
	if (!str.empty())
		os << "(\"" << String::toUTF8(str) << "\")";
	os << ": ";
	os << String::toUTF8(toString()) << '\n';
}

jstring Tag::getName() const
{
	return name;
}

Tag &Tag::setName(const jstring &name)
{
	this->name = name;
	return *this;
}

Tag *Tag::readNamedTag(std::istream &is)
{
	byte_t type = is.get();
	if (type == 0)
		return new EndTag();

	std::unique_ptr<Tag> tag(newTag(type));
	tag->name = IOUtil::readUTF(is);
	tag->load(is);
	return tag.release();
}

void Tag::writeNamedTag(Tag &tag, std::ostream &os)
{
	os.put(tag.getId());
	if (tag.getId() == 0)
		return;
	IOUtil::writeUTF(os, tag.getName());
	tag.write(os);
}

Tag *Tag::newTag(byte_t type)
{
	switch (type)
	{
		case TAG_End:
			return new EndTag();
		case TAG_Byte:
			return new ByteTag();
		case TAG_Short:
			return new ShortTag();
		case TAG_Int:
			return new IntTag();
		case TAG_Long:
			return new LongTag();
		case TAG_Float:
			return new FloatTag();
		case TAG_Double:
			return new DoubleTag();
		case TAG_Byte_Array:
			return new ByteArrayTag();
		case TAG_String:
			return new StringTag();
		case TAG_List:
			return new ListTag();
		case TAG_Compound:
			return new CompoundTag();
	}
	return nullptr;
}

jstring Tag::getTagName(byte_t type)
{
	switch (type)
	{
		case TAG_End:
			return u"TAG_End";
		case TAG_Byte:
			return u"TAG_Byte";
		case TAG_Short:
			return u"TAG_Short";
		case TAG_Int:
			return u"TAG_Int";
		case TAG_Long:
			return u"TAG_Long";
		case TAG_Float:
			return u"TAG_Float";
		case TAG_Double:
			return u"TAG_Double";
		case TAG_Byte_Array:
			return u"TAG_Byte_Array";
		case TAG_String:
			return u"TAG_String";
		case TAG_List:
			return u"TAG_List";
		case TAG_Compound:
			return u"TAG_Compound";
	}
	return u"UNKNOWN";
}
