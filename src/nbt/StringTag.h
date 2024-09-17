#pragma once

#include "nbt/Tag.h"

class StringTag : public Tag
{
public:
	jstring data;

	StringTag();
	StringTag(const jstring &data);

	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;
};
