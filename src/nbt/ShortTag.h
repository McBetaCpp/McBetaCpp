#pragma once

#include "nbt/Tag.h"

class ShortTag : public Tag
{
public:
	short_t data = 0;

	ShortTag();
	ShortTag(short_t data);

	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;
};
