#pragma once

#include "java/Type.h"
#include "java/String.h"

#include "util/ProgressListener.h"

class LevelChunk;

class ChunkSource
{
public:
	virtual bool hasChunk(int_t x, int_t z) = 0;
	virtual std::shared_ptr<LevelChunk> getChunk(int_t x, int_t z) = 0;
	virtual void postProcess(ChunkSource &parent, int_t x, int_t z) = 0;
	virtual bool save(bool force, std::shared_ptr<ProgressListener> progressListener) = 0;
	virtual bool tick() = 0;
	virtual bool shouldSave() = 0;
	virtual jstring gatherStats() = 0;

	virtual bool isChunkCache() const { return false; }
};
