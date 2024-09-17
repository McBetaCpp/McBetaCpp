#pragma once

#include <memory>
#include <array>

#include "world/phys/Vec3.h"

#include "java/Type.h"
#include "java/File.h"

class Level;
class BiomeSource;

class ChunkSource;
class ChunkStorage;

class Dimension
{
public:
	Level &level;
	std::unique_ptr<BiomeSource> biomeSource;

	bool foggy = false;
	bool ultraWarm = false;
	bool hasCeiling = false;

	float brightnessRamp[16] = {};

	int_t id = 0;

private:
	static constexpr long_t fogColor = 0xC0D8FF;

	float sunriseCol[4] = {};

public:
	Dimension(Level &level);

	void initLevel();

protected:
	virtual void updateLightRamp();

	virtual void init();

public:
	virtual ChunkSource *createRandomLevelSource();
	virtual ChunkStorage *createStorage(std::shared_ptr<File> dir);

	virtual bool isValidSpawn(int_t x, int_t z);

	virtual float getTimeOfDay(int_t time, float add);
	virtual float *getSunriseColor(float time, float unknown);
	virtual Vec3 *getFogColor(float time, float unknown);

	virtual bool mayRespawn();

	enum Id
	{
		Id_Hell = -1,
		Id_Normal = 0,

		// Used in place of passing a null dimension
		Id_None = 2,
	};
	static Dimension *getNew(Level &level, int_t id);
};	
