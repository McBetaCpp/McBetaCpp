#pragma once

#include "world/level/chunk/storage/ChunkStorage.h"

#include "nbt/CompoundTag.h"

#include "java/File.h"

class Level;

class OldChunkStorage : public ChunkStorage
{
private:
	std::shared_ptr<File> dir;
	bool create = false;

public:
	OldChunkStorage(std::shared_ptr<File> dir, bool create);

private:
	std::unique_ptr<File> getFile(int_t x, int_t z);

public:
	std::shared_ptr<LevelChunk> load(Level &level, int_t x, int_t z) override;
	void save(Level &level, LevelChunk &chunk) override;

	void save(LevelChunk &chunk, Level &level, CompoundTag &tag);
	static std::shared_ptr<LevelChunk> load(Level &level, CompoundTag &tag);

	void tick() override;
	void flush() override;
	void saveEntities(Level &level, LevelChunk &chunk) override;
};
