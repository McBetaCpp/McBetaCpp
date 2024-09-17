#pragma once

#include "nbt/Tag.h"

class DoubleTag : public Tag
{
public:
	double data = 0;

	DoubleTag();
	DoubleTag(double data);

	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;
};
