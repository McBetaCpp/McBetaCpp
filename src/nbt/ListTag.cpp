#include "nbt/ListTag.h"

#include "java/IOUtil.h"

void ListTag::write(std::ostream &os)
{
	if (list.size() > 0)
		type = list[0]->getId();
	else
		type = TAG_Byte;

	IOUtil::writeByte(os, type);
	IOUtil::writeInt(os, list.size());
	for (const auto &tag : list)
		tag->write(os);
}

void ListTag::load(std::istream &is)
{
	type = IOUtil::readByte(is);
	int_t size = IOUtil::readInt(is);

	list.clear();
	for (int_t i = 0; i < size; i++)
	{
		std::unique_ptr<Tag> tag(Tag::newTag(type));
		tag->load(is);
		list.push_back(std::move(tag));
	}
}

byte_t ListTag::getId() const
{
	return TAG_List;
}

jstring ListTag::toString() const
{
	return String::fromUTF8(std::to_string(list.size())) + u" entries of type " + Tag::getTagName(type);
}

void ListTag::print(const std::string &indent, std::ostream &os) const
{
	Tag::print(indent, os);
	
	os << indent << "{\n";

	std::string new_indent = indent + "  ";
	for (const auto &tag : list)
	{
		tag->print(new_indent, os);
	}

	os << indent << "}\n";
}

void ListTag::add(std::shared_ptr<Tag> tag)
{
	type = tag->getId();
	list.push_back(tag);
}

std::shared_ptr<Tag> ListTag::get(int_t i) const
{
	return list.at(i);
}

int_t ListTag::size() const
{
	return list.size();
}
