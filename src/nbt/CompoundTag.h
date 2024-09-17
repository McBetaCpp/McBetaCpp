#pragma once

#include "nbt/Tag.h"

#include <unordered_map>
#include <vector>

class ListTag;

class CompoundTag : public Tag
{
private:
	std::unordered_map<jstring, std::shared_ptr<Tag>> tags;

public:
	void write(std::ostream &os) override;
	void load(std::istream &is) override;

	byte_t getId() const override;

	void put(const jstring &name, std::shared_ptr<Tag> tag);
	void putByte(const jstring &name, byte_t value);
	void putShort(const jstring &name, short_t value);
	void putInt(const jstring &name, int_t value);
	void putLong(const jstring &name, long_t value);
	void putFloat(const jstring &name, float value);
	void putDouble(const jstring &name, double value);
	void putString(const jstring &name, const jstring &value);
	void putByteArray(const jstring &name, std::vector<byte_t> &&value);
	void putCompound(const jstring &name, std::unique_ptr<CompoundTag> &&value);
	void putBoolean(const jstring &name, bool value);

	std::shared_ptr<Tag> get(const jstring &name) const;
	bool contains(const jstring &name) const;

	byte_t getByte(const jstring &name) const;
	short_t getShort(const jstring &name) const;
	int_t getInt(const jstring &name) const;
	long_t getLong(const jstring &name) const;
	float getFloat(const jstring &name) const;
	double getDouble(const jstring &name) const;
	jstring getString(const jstring &name) const;
	const std::vector<byte_t> &getByteArray(const jstring &name) const;
	std::shared_ptr<CompoundTag> getCompound(const jstring &name) const;
	std::shared_ptr<ListTag> getList(const jstring &name) const;
	bool getBoolean(const jstring &name) const;

	jstring toString() const override;

	void print(const std::string &indent, std::ostream &os) const override;

	bool isEmpty();
};
