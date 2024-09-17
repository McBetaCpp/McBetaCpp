#include "world/level/chunk/storage/OldChunkStorage.h"

#include "world/level/Level.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/entity/EntityIO.h"

#include "nbt/NbtIo.h"
#include "nbt/CompoundTag.h"
#include "nbt/ListTag.h"

OldChunkStorage::OldChunkStorage(std::shared_ptr<File> dir, bool create)
{
	this->dir = dir;
	this->create = create;
}

std::unique_ptr<File> OldChunkStorage::getFile(int_t x, int_t z)
{
	jstring chunkname = u"c." + String::toString(x, 36) + u"." + String::toString(z, 36) + u".dat";
	jstring xname = String::toString(x & 63, 36);
	jstring zname = String::toString(z & 63, 36);

	std::unique_ptr<File> file(File::open(*dir, xname));
	if (!file->exists())
	{
		if (create)
			file->mkdir();
		else
			return nullptr;
	}

	file.reset(File::open(*file, zname));
	if (!file->exists())
	{
		if (create)
			file->mkdir();
		else
			return nullptr;
	}

	file.reset(File::open(*file, chunkname));
	if (!file->exists())
	{
		if (!create)
			return nullptr;
	}

	return file;
}

std::shared_ptr<LevelChunk> OldChunkStorage::load(Level &level, int_t x, int_t z)
{
	std::unique_ptr<File> file = getFile(x, z);
	if (file != nullptr && file->exists())
	{
		std::unique_ptr<std::istream> is(file->toStreamIn());
		std::shared_ptr<CompoundTag> tag(NbtIo::readCompressed(*is));
		if (!tag->contains(u"Level"))
		{
			std::cout << "Chunk file at " << x << "," << z << " is missing level data, skipping\n";
			return nullptr;
		}
		if (!tag->getCompound(u"Level")->contains(u"Blocks"))
		{
			std::cout << "Chunk file at " << x << "," << z << " is missing block data, skipping\n";
			return nullptr;
		}

		std::shared_ptr<LevelChunk> chunk = load(level, *tag->getCompound(u"Level"));
		if (!chunk->isAt(x, z))
		{
			std::cout << "Chunk file at " << x << "," << z << " is in the wrong location; relocating. (Expected " << x << ", " << z << ", got " << chunk->x << ", " << chunk->z << ")\n";
			tag->putInt(u"xPos", x);
			tag->putInt(u"zPos", z);
			chunk = load(level, *tag);
		}

		return chunk;
	}

	return nullptr;
}

void OldChunkStorage::save(Level &level, LevelChunk &chunk)
{
	level.checkSession();

	std::unique_ptr<File> file = getFile(chunk.x, chunk.z);
	if (file->exists())
		level.sizeOnDisk -= file->length();

	std::unique_ptr<File> tmp_file(File::open(*dir, u"tmp_chunk.dat"));
	std::unique_ptr<std::ostream> os(tmp_file->toStreamOut());
	
	std::unique_ptr<CompoundTag> rootTag = std::make_unique<CompoundTag>();
	std::shared_ptr<CompoundTag> levelTag = std::make_shared<CompoundTag>();

	rootTag->put(u"Level", levelTag);
	save(chunk, level, *levelTag);

	NbtIo::writeCompressed(*rootTag, *os);

	levelTag.reset();
	rootTag.reset();

	os->flush();
	os.reset();

	if (file->exists())
		file->remove();
	tmp_file->renameTo(*file);

	level.sizeOnDisk += file->length();
}

void OldChunkStorage::save(LevelChunk &chunk, Level &level, CompoundTag &tag)
{
	level.checkSession();

	tag.putInt(u"xPos", chunk.x);
	tag.putInt(u"zPos", chunk.z);
	tag.putLong(u"LastUpdate", level.time);
	tag.putByteArray(u"Blocks", std::vector<byte_t>(chunk.blocks.begin(), chunk.blocks.end()));
	tag.putByteArray(u"Data", std::vector<byte_t>(chunk.data.data.begin(), chunk.data.data.end()));
	tag.putByteArray(u"SkyLight", std::vector<byte_t>(chunk.skyLight.data.begin(), chunk.skyLight.data.end()));
	tag.putByteArray(u"BlockLight", std::vector<byte_t>(chunk.blockLight.data.begin(), chunk.blockLight.data.end()));
	tag.putByteArray(u"HeightMap", std::vector<byte_t>(chunk.heightmap.begin(), chunk.heightmap.end()));
	tag.putBoolean(u"TerrainPopulated", chunk.terrainPopulated);

	chunk.lastSaveHadEntities = false;

	std::shared_ptr<ListTag> entityTags = std::make_shared<ListTag>();
	for (auto &i : chunk.entityBlocks)
	{
		for (auto &e : i)
		{
			chunk.lastSaveHadEntities = true;

			std::shared_ptr<CompoundTag> entityTag = std::make_shared<CompoundTag>();
			if (e->save(*entityTag))
				entityTags->add(entityTag);
		}
	}
	tag.put(u"Entities", entityTags);

	std::shared_ptr<ListTag> tileEntityTags = std::make_shared<ListTag>();
	for (auto &e : chunk.tileEntities)
	{
		std::shared_ptr<CompoundTag> tileEntityTag = std::make_shared<CompoundTag>();
		e.second->save(*tileEntityTag);
		tileEntityTags->add(tileEntityTag);
	}
	tag.put(u"TileEntities", tileEntityTags);
}

std::shared_ptr<LevelChunk> OldChunkStorage::load(Level &level, CompoundTag &tag)
{
	int_t x = tag.getInt(u"xPos");
	int_t z = tag.getInt(u"zPos");

	std::shared_ptr<LevelChunk> chunk = std::make_shared<LevelChunk>(level, x, z);

	bool hasBlocks = false;
	{
		const std::vector<byte_t> &data = tag.getByteArray(u"Blocks");
		hasBlocks = data.size() == chunk->blocks.size();
		if (hasBlocks)
			std::copy(data.begin(), data.end(), chunk->blocks.begin());
	}

	bool hasData = false;
	{
		const std::vector<byte_t> &data = tag.getByteArray(u"Data");
		hasData = data.size() == chunk->data.data.size();
		if (hasData)
			std::copy(data.begin(), data.end(), chunk->data.data.begin());
	}

	bool hasSkyLight = false;
	{
		const std::vector<byte_t> &data = tag.getByteArray(u"SkyLight");
		hasSkyLight = data.size() == chunk->skyLight.data.size();
		if (hasSkyLight)
			std::copy(data.begin(), data.end(), chunk->skyLight.data.begin());
	}

	bool hasBlockLight = false;
	{
		const std::vector<byte_t> &data = tag.getByteArray(u"BlockLight");
		hasBlockLight = data.size() == chunk->blockLight.data.size();
		if (hasBlockLight)
			std::copy(data.begin(), data.end(), chunk->blockLight.data.begin());
	}

	bool hasHeightMap = false;
	{
		const std::vector<byte_t> &data = tag.getByteArray(u"HeightMap");
		hasHeightMap = data.size() == chunk->heightmap.size();
		if (hasHeightMap)
			std::copy(data.begin(), data.end(), chunk->heightmap.begin());
	}

	chunk->terrainPopulated = tag.getBoolean(u"TerrainPopulated");

	if (!hasBlocks)
		chunk->data.setAll(0);

	if (!hasHeightMap || !hasSkyLight)
	{
		chunk->heightmap.fill(0);
		chunk->skyLight.setAll(0);
		chunk->recalcHeightmap();
	}

	if (!hasBlockLight)
	{
		chunk->blockLight.setAll(0);
		chunk->recalcBlockLights();
	}

	std::shared_ptr<ListTag> entityTags = tag.getList(u"Entities");
	if (entityTags != nullptr)
	{
		for (byte_t i = 0; i < entityTags->size(); i++)
		{
			std::shared_ptr<CompoundTag> entityTag = std::static_pointer_cast<CompoundTag>(entityTags->get(i));
			std::shared_ptr<Entity> entity = EntityIO::loadStatic(*entityTag, level);
		}
	}

	return chunk;
}

void OldChunkStorage::tick()
{

}

void OldChunkStorage::flush()
{

}

void OldChunkStorage::saveEntities(Level &level, LevelChunk &chunk)
{

}