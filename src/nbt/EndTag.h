#pragma once

#include "nbt/Tag.h"

class EndTag : public Tag
{
public:
	EndTag();

	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;
};
