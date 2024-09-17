#pragma once

#include <vector>

#include "nbt/Tag.h"

class ByteArrayTag : public Tag
{
public:
	std::vector<byte_t> data;

	ByteArrayTag();
	ByteArrayTag(std::vector<byte_t> &&data);

	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;
};
