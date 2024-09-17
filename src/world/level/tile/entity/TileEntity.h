#pragma once

#include <unordered_map>
#include <memory>

#include "nbt/CompoundTag.h"

#include "java/Type.h"

class Level;
class Tile;

class TileEntity : public std::enable_shared_from_this<TileEntity>
{
public:
	virtual jstring getEncodeId() const { return u""; }

	std::shared_ptr<Level> level;

	int_t x = 0, y = 0, z = 0;

	virtual void load(CompoundTag &tag);
	virtual void save(CompoundTag &tag);

	virtual void tick();

	static TileEntity *loadStatic(CompoundTag &tag);

	int_t getData();
	void setData(int_t data);
	void setChanged();

	double distanceToSqr(double x, double y, double z);

	Tile &getTile();

	// TODO
	// getUpdatePacket
};
