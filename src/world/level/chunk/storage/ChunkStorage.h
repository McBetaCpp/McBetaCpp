#pragma once

#include <memory>

#include "java/Type.h"

#include "util/Memory.h"

class Level;
class LevelChunk;

class ChunkStorage
{
public:
	virtual ~ChunkStorage() {}

	virtual std::shared_ptr<LevelChunk> load(Level &level, int_t x, int_t z) = 0;
	virtual void save(Level &level, LevelChunk &chunk) = 0;
	virtual void saveEntities(Level &level, LevelChunk &chunk) = 0;
	virtual void tick() = 0;
	virtual void flush() = 0;
};
