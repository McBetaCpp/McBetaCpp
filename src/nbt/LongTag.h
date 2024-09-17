#pragma once

#include "nbt/Tag.h"

class LongTag : public Tag
{
public:
	long_t data = 0;

	LongTag();
	LongTag(long_t data);

	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;
};
