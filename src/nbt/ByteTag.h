#pragma once

#include "nbt/Tag.h"

class ByteTag : public Tag
{
public:
	byte_t data = 0;

	ByteTag();
	ByteTag(byte_t data);

	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;
};
