#pragma once

#include "nbt/Tag.h"

class IntTag : public Tag
{
public:
	int_t data = 0;

	IntTag();
	IntTag(int_t data);

	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;
};
