#pragma once

#include <memory>
#include <array>

#include "world/level/chunk/ChunkSource.h"
#include "world/level/chunk/storage/ChunkStorage.h"

class Level;

class ChunkCache : public ChunkSource
{
private:
	static constexpr int_t CHUNK_CACHE_WIDTH = 32;

	std::shared_ptr<LevelChunk> emptyChunk;

	std::unique_ptr<ChunkSource> source;
	std::unique_ptr<ChunkStorage> storage;
	std::array<std::shared_ptr<LevelChunk>, CHUNK_CACHE_WIDTH * CHUNK_CACHE_WIDTH> chunks;

	Level &level;

public:
	int_t xLast = -999999999;
	int_t zLast = -999999999;

private:
	std::shared_ptr<LevelChunk> last;

	int_t xCenter = 0, yCenter = 0;

	static constexpr int_t MAX_SAVES = 2;

public:
	ChunkCache(Level &level, ChunkStorage *storage, ChunkSource *source);

	void centerOn(int_t x, int_t y);
	bool fits(int_t x, int_t y);

	bool hasChunk(int_t x, int_t z) override;
	std::shared_ptr<LevelChunk> getChunk(int_t x, int_t z) override;

	std::shared_ptr<LevelChunk> load(int_t x, int_t z);
	void saveEntities(LevelChunk &chunk);
	void save(LevelChunk &chunk);

	void postProcess(ChunkSource &parent, int_t x, int_t z) override;
	bool save(bool force, std::shared_ptr<ProgressListener> progressListener) override;
	bool tick() override;
	bool shouldSave() override;
	jstring gatherStats() override;

	virtual bool isChunkCache() const { return true; }
};
