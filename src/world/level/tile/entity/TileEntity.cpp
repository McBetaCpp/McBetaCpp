#include "world/level/tile/entity/TileEntity.h"

#include "world/level/Level.h"
#include "world/level/tile/Tile.h"

void TileEntity::load(CompoundTag &tag)
{
	x = tag.getInt(u"x");
	y = tag.getInt(u"y");
	z = tag.getInt(u"z");
}

void TileEntity::save(CompoundTag &tag)
{
	jstring id = getEncodeId();
	if (id.empty())
		throw std::runtime_error("TileEntity is missing a mapping! This is a bug!");

	tag.putString(u"id", id);
	tag.putInt(u"x", x);
	tag.putInt(u"y", y);
	tag.putInt(u"z", z);
}

void TileEntity::tick()
{

}

TileEntity *TileEntity::loadStatic(CompoundTag &tag)
{
	// TODO
	return nullptr;
}

int_t TileEntity::getData()
{
	return level->getData(x, y, z);
}

void TileEntity::setData(int_t data)
{
	level->setData(x, y, z, data);
}

void TileEntity::setChanged()
{
	if (level != nullptr)
		level->tileEntityChanged(x, y, z, shared_from_this());
}

double TileEntity::distanceToSqr(double x, double y, double z)
{
	double dx = this->x + 0.5 - x;
	double dy = this->y + 0.5 - y;
	double dz = this->z + 0.5 - z;
	return dx * dx + dy * dy + dz * dz;
}

// TODO
// Tile &TileEntity::getTile()
// {
// 
// }
