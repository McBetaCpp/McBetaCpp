#pragma once

#include "nbt/Tag.h"

class FloatTag : public Tag
{
public:
	float data = 0;

	FloatTag();
	FloatTag(float data);

	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;
};
