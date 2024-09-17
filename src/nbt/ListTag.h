#pragma once

#include "nbt/Tag.h"

#include <vector>

class ListTag : public Tag
{
private:
	std::vector<std::shared_ptr<Tag>> list;
	byte_t type = 0;

public:
	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;
	jstring toString() const override;

	void print(const std::string &indent, std::ostream &os) const override;

	void add(std::shared_ptr<Tag> tag);
	std::shared_ptr<Tag> get(int_t i) const;

	int_t size() const;
};
