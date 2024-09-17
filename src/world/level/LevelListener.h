#pragma once

#include "java/Type.h"
#include "java/String.h"

class Entity;
class TileEntity;

class LevelListener
{
public:
	virtual void tileChanged(int_t x, int_t y, int_t z) = 0;
	virtual void setTilesDirty(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1) = 0;
	virtual void allChanged() = 0;
	virtual void playSound(const jstring &name, double x, double y, double z, float volume, float pitch) = 0;
	virtual void addParticle(const jstring &name, double x, double y, double z, double xa, double ya, double za) = 0;
	virtual void playMusic(const jstring &name, double x, double y, double z, float songOffset) = 0;
	virtual void entityAdded(std::shared_ptr<Entity> entity) = 0;
	virtual void entityRemoved(std::shared_ptr<Entity> entity) = 0;
	virtual void skyColorChanged() = 0;
	virtual void playStreamingMusic(const jstring &name, int_t x, int_t y, int_t z) = 0;
	virtual void tileEntityChanged(int_t x, int_t y, int_t z, std::shared_ptr<TileEntity> tileEntity) = 0;
};
