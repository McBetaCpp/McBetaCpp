#include "world/level/Level.h"

#include "nbt/NbtIo.h"

#include "world/level/chunk/ChunkCache.h"
#include "world/level/material/GasMaterial.h"

#include "java/File.h"
#include "java/IOUtil.h"

#include "util/Mth.h"

#include <cmath>
#include <algorithm>

int_t Level::maxLoop = 0;

std::shared_ptr<CompoundTag> Level::getDataTagFor(File &workingDirectory, const jstring &name)
{
	std::unique_ptr<File> saves(File::open(workingDirectory, u"saves"));
	std::unique_ptr<File> world(File::open(*saves, name));
	if (!world->exists())
		return nullptr;

	std::unique_ptr<File> levelDat(File::open(*world, u"level.dat"));
	if (!levelDat->exists())
		return nullptr;

	std::unique_ptr<std::istream> is(levelDat->toStreamIn());
	std::unique_ptr<CompoundTag> tag(NbtIo::readCompressed(*is));
	return tag->getCompound(u"Data");
}

void Level::deleteLevel(File &workingDirectory, const jstring &name)
{
	std::unique_ptr<File> saves(File::open(workingDirectory, u"saves"));
	std::unique_ptr<File> world(File::open(*saves, name));
	if (!world->exists())
		return;

	auto level_files = world->listFiles();
	deleteRecursive(level_files);
	world->remove();
}

void Level::deleteRecursive(std::vector<std::unique_ptr<File>> &files)
{
	for (auto &i : files)
	{
		if (i->isDirectory())
		{
			auto i_files = i->listFiles();
			deleteRecursive(i_files);
		}
		i->remove();
	}
}

BiomeSource &Level::getBiomeSource()
{
	return *dimension->biomeSource;
}

long_t Level::getLevelSize(File &workingDirectory, const jstring &name)
{
	std::unique_ptr<File> saves(File::open(workingDirectory, u"saves"));
	std::unique_ptr<File> world(File::open(*saves, name));
	if (!world->exists())
		return 0;
	auto level_files = world->listFiles();
	return calcSize(level_files);
}

long_t Level::calcSize(std::vector<std::unique_ptr<File>> &files)
{
	long_t sizeOnDisk = 0;
	for (auto &i : files)
	{
		if (i->isDirectory())
		{
			auto i_files = i->listFiles();
			sizeOnDisk += calcSize(i_files);
		}
		else
		{
			sizeOnDisk += i->length();
		}
	}
	return sizeOnDisk;
}

Level::Level(File *workingDirectory, const jstring &name) : Level(workingDirectory, name, Random().nextLong())
{

}

Level::Level(const jstring &name, int_t dimension, long_t seed)
{
	this->name = name;
	this->seed = seed;
	this->dimension.reset(Dimension::getNew(*this, dimension));
	this->chunkSource.reset(createChunkSource(dir));
	updateSkyBrightness();
}

Level::Level(Level &level, int_t dimension)
{
	this->sessionId = level.sessionId;
	this->workDir = std::move(level.workDir);
	this->dir = std::move(level.dir);
	this->seed = level.seed;
	this->time = level.time;
	this->xSpawn = level.xSpawn;
	this->ySpawn = level.ySpawn;
	this->zSpawn = level.zSpawn;
	this->sizeOnDisk = level.sizeOnDisk;
	this->dimension.reset(Dimension::getNew(*this, dimension));
	this->chunkSource.reset(createChunkSource(dir));
	updateSkyBrightness();
}

Level::Level(File *workingDirectory, const jstring &name, long_t seed) : Level(workingDirectory, name, seed, Dimension::Id_None)
{

}

Level::Level(File *workingDirectory, const jstring &name, long_t seed, int_t dimension)
{
	this->workDir.reset(workingDirectory);
	this->name = name;

	workingDirectory->mkdirs();
	dir.reset(File::open(*workingDirectory, name));
	dir->mkdirs();

	{
		std::unique_ptr<File> session_lock(File::open(*dir, u"session.lock"));
		std::unique_ptr<std::ostream> os(session_lock->toStreamOut());
		if (!os)
			throw std::runtime_error("Failed to check session lock, aborting");
		IOUtil::writeLong(*os, sessionId);
	}

	int_t new_dimension = Dimension::Id_Normal;

	std::unique_ptr<File> fileDat(File::open(*dir, u"level.dat"));
	isNew = !fileDat->exists();

	if (fileDat->exists())
	{
		std::shared_ptr<CompoundTag> root(NbtIo::readCompressed(*std::unique_ptr<std::istream>(fileDat->toStreamIn())));
		std::shared_ptr<CompoundTag> data(root->getCompound(u"Data"));
		seed = data->getLong(u"RandomSeed");
		xSpawn = data->getInt(u"SpawnX");
		ySpawn = data->getInt(u"SpawnY");
		zSpawn = data->getInt(u"SpawnZ");
		time = data->getLong(u"Time");
		sizeOnDisk = data->getLong(u"SizeOnDisk");

		if (data->contains(u"Player"))
		{
			loadedPlayerTag = data->getCompound(u"Player");

			int_t tag_dimension = loadedPlayerTag->getInt(u"Dimension");
			if (tag_dimension == Dimension::Id_Hell)
				dimension = tag_dimension;
		}
	}

	if (dimension != Dimension::Id_None)
		new_dimension = dimension;

	bool findSpawn = false;
	if (this->seed == 0)
	{
		this->seed = seed;
		findSpawn = true;
	}

	this->dimension.reset(Dimension::getNew(*this, new_dimension));
	this->chunkSource.reset(createChunkSource(dir));

	if (findSpawn)
	{
		isFindingSpawn = true;
		xSpawn = 0;
		ySpawn = 64;
		zSpawn = 0;
		while (!this->dimension->isValidSpawn(xSpawn, zSpawn))
		{
			xSpawn += random.nextInt(64) - random.nextInt(64);
			zSpawn += random.nextInt(64) - random.nextInt(64);
		}
		isFindingSpawn = false;
	}
}

ChunkSource *Level::createChunkSource(std::shared_ptr<File> dir)
{
	return new ChunkCache(*this, dimension->createStorage(dir), dimension->createRandomLevelSource());
}

void Level::validateSpawn()
{

}

int_t Level::getTopTile(int_t x, int_t z)
{
	byte_t y;
	for (y = SEA_LEVEL; !isEmptyTile(x, y + 1, z); y++);
	return getTile(x, y, z);
}

void Level::clearLoadedPlayerData()
{

}

void Level::loadPlayer(std::shared_ptr<Player> player)
{
	if (loadedPlayerTag != nullptr)
	{
		player->load(*loadedPlayerTag);
		loadedPlayerTag = nullptr;
	}

	if (chunkSource->isChunkCache())
	{
		ChunkCache &chunkCache = static_cast<ChunkCache &>(*chunkSource);
		int_t x = Mth::floor(static_cast<float>(static_cast<int_t>(player->x))) >> 4;
		int_t z = Mth::floor(static_cast<float>(static_cast<int_t>(player->z))) >> 4;
		chunkCache.centerOn(x, z);
	}

	addEntity(player);
}

void Level::save(bool force, std::shared_ptr<ProgressListener> progressRenderer)
{
	if (!chunkSource->shouldSave())
		return;

	if (progressRenderer != nullptr)
		progressRenderer->progressStartNoAbort(u"Saving level");
	saveLevelData();
	if (progressRenderer != nullptr)
		progressRenderer->progressStage(u"Saving chunks");
	chunkSource->save(force, progressRenderer);
}

void Level::saveLevelData()
{
	checkSession();

	std::shared_ptr<CompoundTag> data = std::make_shared<CompoundTag>();
	data->putLong(u"RandomSeed", seed);
	data->putInt(u"SpawnX", xSpawn);
	data->putInt(u"SpawnY", ySpawn);
	data->putInt(u"SpawnZ", zSpawn);
	data->putLong(u"Time", time);
	data->putLong(u"SizeOnDisk", sizeOnDisk);
	data->putLong(u"LastPlayed", System::currentTimeMillis());

	std::shared_ptr<Player> player;
	if (!players.empty())
		player = players[0];

	if (player != nullptr)
	{
		std::shared_ptr<CompoundTag> playerTag = std::make_shared<CompoundTag>();
		player->saveWithoutId(*playerTag);
		data->put(u"Player", playerTag);
	}

	std::shared_ptr<CompoundTag> root = std::make_shared<CompoundTag>();
	root->put(u"Data", data);

	std::unique_ptr<File> fileNewDat(File::open(*dir, u"level.dat_new"));
	std::unique_ptr<File> fileOldDat(File::open(*dir, u"level.dat_old"));
	std::unique_ptr<File> fileDat(File::open(*dir, u"level.dat"));

	std::unique_ptr<std::ostream> os(fileNewDat->toStreamOut());
	NbtIo::writeCompressed(*root, *os);
	os->flush();
	os.reset();

	if (fileOldDat->exists())
		fileOldDat->remove();
	fileDat->renameTo(*fileOldDat);
	if (fileDat->exists())
		fileDat->remove();
	fileNewDat->renameTo(*fileDat);
	if (fileNewDat->exists())
		fileNewDat->remove();
}

bool Level::pauseSave(int_t saveStep)
{
	if (!chunkSource->shouldSave())
		return true;
	if (saveStep == 0)
		saveLevelData();
	return chunkSource->save(false, nullptr);
}

int_t Level::getTile(int_t x, int_t y, int_t z)
{
	if (x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z > MAX_LEVEL_SIZE)
		return 0;
	if (y < 0)
		return 0;
	if (y >= DEPTH)
		return 0;
	std::shared_ptr<LevelChunk> chunk = getChunk(x >> 4, z >> 4);
	return chunk->getTile(x & 0xF, y, z & 0xF);
}

bool Level::isEmptyTile(int_t x, int_t y, int_t z)
{
	return getTile(x, y, z) == 0;
}

bool Level::hasChunkAt(int_t x, int_t y, int_t z)
{
	return y >= 0 && y < DEPTH && hasChunk(x >> 4, z >> 4);
}

bool Level::hasChunksAt(int_t x, int_t y, int_t z, int_t d)
{
	return hasChunksAt(x - d, y - d, z - d, x + d, y + d, z + d);
}

bool Level::hasChunksAt(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	if (y1 < 0 || y0 >= DEPTH)
		return false;

	x0 >>= 4;
	y0 >>= 4;
	z0 >>= 4;
	x1 >>= 4;
	y1 >>= 4;
	z1 >>= 4;

	for (int_t cx = x0; cx <= x1; cx++)
		for (int_t cz = z0; cz <= z1; cz++)
			if (!hasChunk(cx, cz))
				return false;
	return true;
}

bool Level::hasChunk(int_t xc, int_t zc)
{
	return chunkSource->hasChunk(xc, zc);
}

std::shared_ptr<LevelChunk> Level::getChunkAt(int_t x, int_t z)
{
	return getChunk(x >> 4, z >> 4);
}

std::shared_ptr<LevelChunk> Level::getChunk(int_t xc, int_t zc)
{
	return chunkSource->getChunk(xc, zc);
}

bool Level::setTileAndDataNoUpdate(int_t x, int_t y, int_t z, int_t tile, int_t data)
{
	if (x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z > MAX_LEVEL_SIZE)
		return false;
	if (y < 0)
		return false;
	if (y >= DEPTH)
		return false;
	std::shared_ptr<LevelChunk> chunk = getChunk(x >> 4, z >> 4);
	return chunk->setTileAndData(x & 0xF, y, z & 0xF, tile, data);
}

bool Level::setTileNoUpdate(int_t x, int_t y, int_t z, int_t tile)
{
	if (x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z > MAX_LEVEL_SIZE)
		return false;
	if (y < 0)
		return false;
	if (y >= DEPTH)
		return false;
	std::shared_ptr<LevelChunk> chunk = getChunk(x >> 4, z >> 4);
	return chunk->setTile(x & 0xF, y, z & 0xF, tile);
}

const Material &Level::getMaterial(int_t x, int_t y, int_t z)
{
	int_t tile = getTile(x, y, z);
	return (tile == 0) ? Material::air : Tile::tiles[tile]->material;
}

int_t Level::getData(int_t x, int_t y, int_t z)
{
	if (x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z > MAX_LEVEL_SIZE)
		return 0;
	if (y < 0)
		return 0;
	if (y >= DEPTH)
		return 0;
	std::shared_ptr<LevelChunk> chunk = getChunk(x >> 4, z >> 4);
	return chunk->getData(x & 0xF, y, z & 0xF);
}

void Level::setData(int_t x, int_t y, int_t z, int_t data)
{
	if (setDataNoUpdate(x, y, z, data))
		tileUpdated(x, y, z, getTile(x, y, z));
}

bool Level::setDataNoUpdate(int_t x, int_t y, int_t z, int_t data)
{
	if (x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z > MAX_LEVEL_SIZE)
		return false;
	if (y < 0)
		return false;
	if (y >= DEPTH)
		return false;
	std::shared_ptr<LevelChunk> chunk = getChunk(x >> 4, z >> 4);
	chunk->setData(x & 0xF, y, z & 0xF, data);
	return true;
}

bool Level::setTile(int_t x, int_t y, int_t z, int_t tile)
{
	if (setTileNoUpdate(x, y, z, tile))
	{
		tileUpdated(x, y, z, tile);
		return true;
	}
	return false;
}

bool Level::setTileAndData(int_t x, int_t y, int_t z, int_t tile, int_t data)
{
	if (setTileAndDataNoUpdate(x, y, z, tile, data))
	{
		tileUpdated(x, y, z, tile);
		return true;
	}
	return false;
}

void Level::sendTileUpdated(int_t x, int_t y, int_t z)
{
	for (auto &l : listeners)
		l->tileChanged(x, y, z);
}

void Level::tileUpdated(int_t x, int_t y, int_t z, int_t tile)
{
	sendTileUpdated(x, y, z);
	updateNeighborsAt(x, y, z, tile);
}

void Level::lightColumnChanged(int_t x, int_t z, int_t y0, int_t y1)
{
	if (y0 > y1)
	{
		int_t temp = y0;
		y0 = y1;
		y1 = temp;
	}
	setTilesDirty(x, y0, z, x, y1, z);
}

void Level::setTileDirty(int_t x, int_t y, int_t z)
{
	for (auto &l : listeners)
		l->setTilesDirty(x, y, z, x, y, z);
}

void Level::setTilesDirty(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	for (auto &l : listeners)
		l->setTilesDirty(x0, y0, z0, x1, y1, z1);
}

void Level::swap(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	int_t t0 = getTile(x0, y0, z0);
	int_t d0 = getData(x0, y0, z0);
	int_t t1 = getTile(x1, y1, z1);
	int_t d1 = getData(x1, y1, z1);

	setTileAndDataNoUpdate(x0, y0, z0, t1, d1);
	setTileAndDataNoUpdate(x1, y1, z1, t0, d0);
	updateNeighborsAt(x0, y0, z0, t1);
	updateNeighborsAt(x1, y1, z1, t0);
}

void Level::updateNeighborsAt(int_t x, int_t y, int_t z, int_t tile)
{
	neighborChanged(x - 1, y, z, tile);
	neighborChanged(x + 1, y, z, tile);
	neighborChanged(x, y - 1, z, tile);
	neighborChanged(x, y + 1, z, tile);
	neighborChanged(x, y, z - 1, tile);
	neighborChanged(x, y, z + 1, tile);
}

void Level::neighborChanged(int_t x, int_t y, int_t z, int_t tile)
{
	if (noNeighborUpdate || isOnline)
		return;
	Tile *ptile = Tile::tiles[getTile(x, y, z)];
	if (ptile != nullptr)
		ptile->neighborChanged(*this, x, y, z, tile);
}

bool Level::canSeeSky(int_t x, int_t y, int_t z)
{
	return getChunk(x >> 4, z >> 4)->isSkyLit(x & 0xF, y, z & 0xF);
}

int_t Level::getRawBrightness(int_t x, int_t y, int_t z)
{
	return getRawBrightness(x, y, z, true);
}

int_t Level::getRawBrightness(int_t x, int_t y, int_t z, bool neighbors)
{
	if (x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z > MAX_LEVEL_SIZE)
		return 15;

	if (neighbors)
	{
		int_t tile = getTile(x, y, z);
		// TODO
		// stoneSlabHalf farmland
	}

	if (y < 0)
		return 0;

	if (y >= DEPTH)
	{
		int_t l = 15 - skyDarken;
		if (l < 0)
			l = 0;
		return l;
	}

	return getChunk(x >> 4, z >> 4)->getRawBrightness(x & 0xF, y, z & 0xF, skyDarken);
}

bool Level::isSkyLit(int_t x, int_t y, int_t z)
{
	if (x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z > MAX_LEVEL_SIZE)
		return false;
	if (y < 0)
		return false;
	if (y >= DEPTH)
		return true;
	if (!hasChunk(x >> 4, z >> 4))
		return false;
	return getChunk(x >> 4, z >> 4)->isSkyLit(x & 0xF, y, z & 0xF);
}

int_t Level::getHeightmap(int_t x, int_t z)
{
	if (x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z > MAX_LEVEL_SIZE)
		return 0;
	if (!hasChunk(x >> 4, z >> 4))
		return 0;
	return getChunk(x >> 4, z >> 4)->getHeightmap(x & 0xF, z & 0xF);
}

void Level::updateLightIfOtherThan(int_t layer, int_t x, int_t y, int_t z, int_t brightness)
{
	if (dimension->hasCeiling && layer == LightLayer::Sky)
		return;
	if (!hasChunkAt(x, y, z))
		return;

	if (layer == LightLayer::Sky)
	{
		if (isSkyLit(x, y, z))
			brightness = 15;
	}
	else if (layer == LightLayer::Block)
	{
		int_t tile = getTile(x, y, z);
		if (Tile::lightEmission[tile] > brightness)
			brightness = Tile::lightEmission[tile];
	}

	if (getBrightness(layer, x, y, z) != brightness)
		updateLight(layer, x, y, z, x, y, z);
}

int_t Level::getBrightness(int_t layer, int_t x, int_t y, int_t z)
{
	if (y < 0 || y >= DEPTH || x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z >= MAX_LEVEL_SIZE)
		return LightLayer::surrounding(layer);

	int_t xc = x >> 4;
	int_t zc = z >> 4;
	if (!hasChunk(xc, zc))
		return 0;

	return getChunk(xc, zc)->getBrightness(layer, x & 0xF, y, z & 0xF);
}

void Level::setBrightness(int_t layer, int_t x, int_t y, int_t z, int_t brightness)
{
	if (x < -MAX_LEVEL_SIZE || z < -MAX_LEVEL_SIZE || x >= MAX_LEVEL_SIZE || z > MAX_LEVEL_SIZE)
		return;
	if (y < 0)
		return;
	if (y >= DEPTH)
		return;
	if (!hasChunk(x >> 4, z >> 4))
		return;

	getChunk(x >> 4, z >> 4)->setBrightness(layer, x & 0xF, y, z & 0xF, brightness);

	for (auto &l : listeners)
		l->tileChanged(x, y, z);
}

float Level::getBrightness(int_t x, int_t y, int_t z)
{
	return dimension->brightnessRamp[getRawBrightness(x, y, z)];
}

bool Level::isDay()
{
	return skyDarken < 4;
}

HitResult Level::clip(Vec3 &from, Vec3 &to)
{
	return clip(from, to, false);
}

HitResult Level::clip(Vec3 &from, Vec3 &to, bool canPickLiquid)
{
	if (std::isnan(from.x) || std::isnan(from.y) || std::isnan(from.z)) return HitResult();
	if (std::isnan(to.x) || std::isnan(to.y) || std::isnan(to.z)) return HitResult();

	int_t x0 = Mth::floor(to.x);
	int_t y0 = Mth::floor(to.y);
	int_t z0 = Mth::floor(to.z);
	int_t x1 = Mth::floor(from.x);
	int_t y1 = Mth::floor(from.y);
	int_t z1 = Mth::floor(from.z);
	int_t steps = 200;

	while (steps-- >= 0)
	{
		if (std::isnan(from.x) || std::isnan(from.y) || std::isnan(from.z)) return HitResult();
		if (x1 == x0 && y1 == y0 && z1 == z0)
			return HitResult();

		double dx = 999.0;
		double dy = 999.0;
		double dz = 999.0;

		if (x0 > x1) dx = x1 + 1.0;
		if (x0 < x1) dx = x1 + 0.0;
		if (y0 > y1) dy = y1 + 1.0;
		if (y0 < y1) dy = y1 + 0.0;
		if (z0 > z1) dz = z1 + 1.0;
		if (z0 < z1) dz = z1 + 0.0;

		double dx2 = 999.0;
		double dy2 = 999.0;
		double dz2 = 999.0;
		double ddx = to.x - from.x;
		double ddy = to.y - from.y;
		double ddz = to.z - from.z;

		if (dx != 999.0) dx2 = (dx - from.x) / ddx;
		if (dy != 999.0) dy2 = (dy - from.y) / ddy;
		if (dz != 999.0) dz2 = (dz - from.z) / ddz;

		Facing face = Facing::DOWN;
		if (dx2 < dy2 && dx2 < dz2)
		{
			if (x0 > x1)
				face = Facing::WEST;
			else
				face = Facing::EAST;

			from.x = dx;
			from.y += ddy * dx2;
			from.z += ddz * dx2;
		}
		else if (dy2 < dz2)
		{
			if (y0 > y1)
				face = Facing::DOWN;
			else
				face = Facing::UP;

			from.x += ddx * dy2;
			from.y = dy;
			from.z += ddz * dy2;
		}
		else
		{
			if (z0 > z1)
				face = Facing::NORTH;
			else
				face = Facing::SOUTH;

			from.x += ddx * dz2;
			from.y += ddy * dz2;
			from.z = dz;
		}

		Vec3 *newVec = Vec3::newTemp(from.x, from.y, from.z);

		x1 = newVec->x = Mth::floor(from.x);
		if (face == Facing::EAST)
		{
			x1--;
			newVec->x++;
		}

		y1 = newVec->y = Mth::floor(from.y);
		if (face == Facing::UP)
		{
			y1--;
			newVec->y++;
		}

		z1 = newVec->z = Mth::floor(from.z);
		if (face == Facing::SOUTH)
		{
			z1--;
			newVec->z++;
		}

		int_t tile = getTile(x1, y1, z1);
		int_t data = getData(x1, y1, z1);
		Tile &tt = *Tile::tiles[tile];
		if (tile > 0 && tt.mayPick(data, canPickLiquid))
		{
			HitResult hitResult = tt.clip(*this, x1, y1, z1, from, to);
			if (hitResult.type != HitResult::Type::NONE)
				return hitResult;
		}
	}

	return HitResult();
}


bool Level::addEntity(std::shared_ptr<Entity> entity)
{
	int_t cx = Mth::floor(entity->x / 16.0);
	int_t cz = Mth::floor(entity->z / 16.0);
	bool isPlayer = entity->isPlayer();

	if (!isPlayer && !hasChunk(cx, cz))
		return false;

	if (isPlayer)
	{
		players.push_back(std::static_pointer_cast<Player>(entity));
		std::cout << "Player count: " << players.size() << '\n';
	}

	getChunk(cx, cz)->addEntity(entity);
	entities.emplace(entity);
	entityAdded(entity);

	return true;
}

void Level::entityAdded(std::shared_ptr<Entity> entity)
{
	for (auto &l : listeners)
		l->entityAdded(entity);
}

void Level::entityRemoved(std::shared_ptr<Entity> entity)
{
	for (auto &l : listeners)
		l->entityRemoved(entity);
}

void Level::removeEntity(std::shared_ptr<Entity> entity)
{
	if (entity->rider != nullptr)
		entity->rider->ride(nullptr);
	if (entity->riding != nullptr)
		entity->ride(nullptr);
	entity->remove();
	if (entity->isPlayer())
		players.erase(std::remove(players.begin(), players.end(), std::static_pointer_cast<Player>(entity)), players.end());
}

void Level::removeEntityImmediately(std::shared_ptr<Entity> entity)
{
	entity->remove();
	if (entity->isPlayer())
		players.erase(std::remove(players.begin(), players.end(), std::static_pointer_cast<Player>(entity)), players.end());
	int_t cx = entity->xChunk;
	int_t cz = entity->zChunk;
	if (entity->inChunk && hasChunk(cx, cz))
		getChunk(cx, cz)->removeEntity(entity);
	entities.erase(entity);
	entityRemoved(entity);
}

void Level::addListener(LevelListener &listener)
{
	listeners.emplace(&listener);
}

void Level::removeListener(LevelListener &listener)
{
	listeners.erase(&listener);
}

const std::vector<AABB *> &Level::getCubes(Entity &entity, AABB &bb)
{
	boxes.clear();

	int_t x0 = Mth::floor(bb.x0);
	int_t x1 = Mth::floor(bb.x1 + 1.0);
	int_t y0 = Mth::floor(bb.y0);
	int_t y1 = Mth::floor(bb.y1 + 1.0);
	int_t z0 = Mth::floor(bb.z0);
	int_t z1 = Mth::floor(bb.z1 + 1.0);

	for (int_t x = x0; x < x1; x++)
	{
		for (int_t y = y0; y < y1; y++)
		{
			for (int_t z = z0; z < z1; z++)
			{
				Tile *tile = Tile::tiles[getTile(x, y, z)];
				if (tile != nullptr)
					tile->addAABBs(*this, x, y, z, bb, boxes);
			}
		}
	}

	double skin = 0.25;
	const auto &overlapEntities = getEntities(&entity, *bb.grow(skin, skin, skin));

	for (auto &other : overlapEntities)
	{
		AABB *ebb = other->getCollideBox();
		if (ebb != nullptr && ebb->intersects(bb))
			boxes.push_back(ebb);

		ebb = entity.getCollideAgainstBox(*other);
		if (ebb != nullptr && ebb->intersects(bb))
			boxes.push_back(ebb);
	}

	return boxes;
}

int_t Level::getSkyDarken(float a)
{
	float tod = getTimeOfDay(a);
	float curve = 1.0f - (Mth::cos(tod * Mth::PI * 2.0f) * 2.0f + 0.5f);
	if (curve < 0.0f) curve = 0.0f;
	if (curve > 1.0f) curve = 1.0f;
	return static_cast<int_t>(curve * 11.0f);
}

static void HSBtoRGB(float hue, float saturation, float brightness, float &r, float &g, float &b)
{
	if (saturation == 0)
	{
		// Achromatic (grey)
		r = g = b = brightness;
	}
	else
	{
		hue /= (60.0f / 360.0f);
		int i = static_cast<int>(hue);
		float f = hue - i;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - saturation * (1.0f - f));

		switch (i) {
			case 0:
				r = brightness;
				g = t;
				b = p;
				break;
			case 1:
				r = q;
				g = brightness;
				b = p;
				break;
			case 2:
				r = p;
				g = brightness;
				b = t;
				break;
			case 3:
				r = p;
				g = q;
				b = brightness;
				break;
			case 4:
				r = t;
				g = p;
				b = brightness;
				break;
			default:
				r = brightness;
				g = p;
				b = q;
				break;
		}
	}
}

Vec3 *Level::getSkyColor(Entity &entity, float a)
{
	float tod = getTimeOfDay(a);
	float curve = Mth::cos(tod * Mth::PI * 2.0f) * 2.0f + 0.5f;
	if (curve < 0.0f) curve = 0.0f;
	if (curve > 1.0f) curve = 1.0f;

	int_t x = Mth::floor(entity.x);
	int_t z = Mth::floor(entity.z);

	float temperature = getBiomeSource().getTemperature(x, z);

	float r = 0.0f;
	float g = 0.0f;
	float b = 1.0f;

	temperature /= 3.0f;
	if (temperature < -1.0f) temperature = -1.0f;
	if (temperature > 1.0f) temperature = 1.0f;

	HSBtoRGB(0.6222222f - temperature * 0.05f, 0.5f + temperature * 0.1f, 1.0f, r, g, b);

	r *= curve;
	g *= curve;
	b *= curve;

	return Vec3::newTemp(r, g, b);
}

float Level::getTimeOfDay(float a)
{
	return dimension->getTimeOfDay(time, a);
}

float Level::getSunAngle(float a)
{
	float tod = getTimeOfDay(a);
	return tod * Mth::PI * 2.0f;
}

Vec3 *Level::getCloudColor(float a)
{
	float tod = getTimeOfDay(a);
	float curve = Mth::cos(tod * Mth::PI * 2.0f) * 2.0f + 0.5f;
	if (curve < 0.0f) curve = 0.0f;
	if (curve > 1.0f) curve = 1.0f;

	float r = ((cloudColor >> 16) & 0xFF) / 255.0f;
	float g = ((cloudColor >> 8) & 0xFF) / 255.0f;
	float b = (cloudColor & 0xFF) / 255.0f;

	r *= curve * 0.9f + 0.1f;
	g *= curve * 0.9f + 0.1f;
	b *= curve * 0.85f + 0.15f;

	return Vec3::newTemp(r, g, b);
}

Vec3 *Level::getFogColor(float a)
{
	float tod = getTimeOfDay(a);
	return dimension->getFogColor(tod, a);
}

int_t Level::getTopSolidBlock(int_t x, int_t z)
{
	std::shared_ptr<LevelChunk> chunk = getChunk(x >> 4, z >> 4);

	int_t y;
	for (y = DEPTH - 1; getMaterial(x, y, z).blocksMotion() && y > 0; y--);

	x &= 0xF;
	z &= 0xF;

	while (y > 0)
	{
		int_t tile = chunk->getTile(x, y, z);
		if (tile == 0 || (!(Tile::tiles[tile])->material.blocksMotion() && !(Tile::tiles[tile])->material.isLiquid()))
		{
			y--;
			continue;
		}
		return y + 1;
	}
	return -1;
}

int_t Level::getLightDepth(int_t x, int_t z)
{
	return getChunkAt(x, z)->getHeightmap(x & 0xF, z & 0xF);
}

float Level::getStarBrightness(float a)
{
	float tod = getTimeOfDay(a);
	float curve = 1.0f - (Mth::cos(tod * Mth::PI * 2.0f) * 2.0f + 0.75f);
	if (curve < 0.0f) curve = 0.0f;
	if (curve > 1.0f) curve = 1.0f;
	return curve * curve * 0.5f;
}

void Level::addToTickNextTick(int_t x, int_t y, int_t z, int_t delay)
{

}

void Level::tickEntities()
{
	// Remove entities queued to remove
	for (auto &entity : entitiesToRemove)
		entities.erase(entity);

	for (auto &entity : entitiesToRemove)
	{
		int_t xc = entity->xChunk;
		int_t zc = entity->zChunk;
		if (entity->inChunk && hasChunk(xc, zc))
			getChunk(xc, zc)->removeEntity(entity);
	}

	for (auto &entity : entitiesToRemove)
		entityRemoved(entity);

	entitiesToRemove.clear();

	// Tick all entities
	for (auto it = entities.begin(); it != entities.end();)
	{
		std::shared_ptr<Entity> entity = *it;

		if (entity->riding != nullptr)
		{
			if (entity->riding->removed || entity->riding->rider != entity)
			{
				entity->riding->rider = nullptr;
				entity->riding = nullptr;
			}
			else
			{
				it++;
				continue;
			}
		}

		if (!entity->removed)
			tick(entity);

		if (entity->removed)
		{
			int_t xc = entity->xChunk;
			int_t zc = entity->zChunk;
			if (entity->inChunk && hasChunk(xc, zc))
				getChunk(xc, zc)->removeEntity(entity);
			entities.erase(entity);

			it = entities.erase(it);
			entityRemoved(entity);
			continue;
		}

		it++;
		continue;
	}

	// Tick tile entities
	for (auto &tileEntity : tileEntityList)
		tileEntity->tick();
}

void Level::tick(std::shared_ptr<Entity> entity)
{
	tick(entity, true);
}

void Level::tick(std::shared_ptr<Entity> entity, bool ai)
{
	int_t xt = Mth::floor(entity->x);
	int_t zt = Mth::floor(entity->z);

	byte_t rad = 32;
	if (ai && !hasChunksAt(xt - rad, 0, zt - rad, xt + rad, DEPTH, zt + rad))
		return;

	// Interpolation setup
	entity->xOld = entity->x;
	entity->yOld = entity->y;
	entity->zOld = entity->z;
	entity->yRotO = entity->yRot;
	entity->xRotO = entity->xRot;

	if (ai && entity->inChunk)
	{
		if (entity->riding != nullptr)
			entity->rideTick();
		else
			entity->tick();
	}
	else
	{
		// TODO REMOVE
		entity->tick();
	}

	// Check if any invalid operations occured
	if (std::isnan(entity->x) || std::isinf(entity->x))
		entity->x = entity->xOld;
	if (std::isnan(entity->y) || std::isinf(entity->y))
		entity->y = entity->yOld;
	if (std::isnan(entity->z) || std::isinf(entity->z))
		entity->z = entity->zOld;
	if (std::isnan(entity->yRot) || std::isinf(entity->yRot))
		entity->yRot = entity->yRotO;
	if (std::isnan(entity->xRot) || std::isinf(entity->xRot))
		entity->xRot = entity->xRotO;

	// Update chunk
	int_t xc = Mth::floor(entity->x / 16.0);
	int_t yc = Mth::floor(entity->y / 16.0);
	int_t zc = Mth::floor(entity->z / 16.0);

	if (!entity->inChunk || entity->xChunk != xc || entity->yChunk != yc || entity->zChunk != zc)
	{
		if (entity->inChunk && hasChunk(entity->xChunk, entity->zChunk))
			getChunk(entity->xChunk, entity->zChunk)->removeEntity(entity);

		if (hasChunk(xc, zc))
		{
			entity->inChunk = true;
			getChunk(xc, zc)->addEntity(entity);
		}
		else
		{
			entity->inChunk = false;
		}
	}

	// Tick rider
	if (ai && entity->inChunk && entity->rider != nullptr)
	{
		if (entity->rider->removed || entity->rider->riding != entity)
		{
			entity->rider->riding = nullptr;
			entity->rider = nullptr;
		}
		else
		{
			tick(entity->rider);
		}
	}
}

bool Level::isUnobstructed(AABB &bb)
{
	auto &overlapEntities = getEntities(nullptr, bb);
	for (auto &entity : overlapEntities)
	{
		if (!entity->removed && entity->blocksBuilding)
			return false;
	}
	return true;
}

bool Level::containsAnyLiquid(AABB &bb)
{
	// TODO
	return false;
}

bool Level::containsFireTile(AABB &bb)
{
	// TODO
	return false;
}

jstring Level::gatherStats()
{
	return u"All: " + String::toString(entities.size());
}

jstring Level::gatherChunkSourceStats()
{
	return chunkSource->gatherStats();
}

std::shared_ptr<TileEntity> Level::getTileEntity(int_t x, int_t y, int_t z)
{
	std::shared_ptr<LevelChunk> chunk = getChunk(x >> 4, z >> 4);
	return (chunk != nullptr) ? chunk->getTileEntity(x & 0xF, y, z & 0xF) : nullptr;
}

void Level::setTileEntity(int_t x, int_t y, int_t z, std::shared_ptr<TileEntity> tileEntity)
{
	std::shared_ptr<LevelChunk> chunk = getChunk(x >> 4, z >> 4);
	if (chunk != nullptr)
		chunk->setTileEntity(x & 0xF, y, z & 0xF, tileEntity);
}

void Level::removeTileEntity(int_t x, int_t y, int_t z)
{
	std::shared_ptr<LevelChunk> chunk = getChunk(x >> 4, z >> 4);
	if (chunk != nullptr)
		chunk->removeTileEntity(x & 0xF, y, z & 0xF);
}

bool Level::isSolidTile(int_t x, int_t y, int_t z)
{
	Tile *tile = Tile::tiles[getTile(x, y, z)];
	return (tile == nullptr) ? false : tile->isSolidRender();
}
void Level::forceSave(std::shared_ptr<ProgressListener> progressRenderer)
{
	save(true, progressRenderer);
}

int_t Level::getLightsToUpdate()
{
	return lightUpdates.size();
}

bool Level::updateLights()
{
	if (maxRecurse >= 50)
		return false;
	maxRecurse++;

	int_t limit = 500;
	while (!lightUpdates.empty())
	{
		if (--limit <= 0)
		{
			maxRecurse--;
			return true;
		}

		LightUpdate update = lightUpdates.back();
		lightUpdates.pop_back();

		update.update(*this);
	}

	maxRecurse--;
	return false;
}

void Level::updateLight(int_t layer, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	updateLight(layer, x0, y0, z0, x1, y1, z1, true);
}

void Level::updateLight(int_t layer, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1, bool checkExpansion)
{
	if (dimension->hasCeiling && layer == LightLayer::Sky)
		return;

	if (++maxLoop == 50)
	{
		maxLoop--;
		return;
	}

	int_t xm = (x1 + x0) / 2;
	int_t zm = (z1 + z0) / 2;
	if (!hasChunkAt(xm, 64, zm))
	{
		maxLoop--;
		return;
	}

	if (getChunkAt(xm, zm)->isEmpty())
		return;

	int_t updates = lightUpdates.size();
	if (checkExpansion)
	{
		// Check if this light update can be handled by another nearby one
		int_t checkLimit = 5;
		if (checkLimit > updates)
			checkLimit = updates;

		for (int_t i = 0; i < checkLimit; i++)
		{
			LightUpdate &other = lightUpdates[lightUpdates.size() - i - 1];
			if (other.layer == layer && other.expandToContain(x0, y0, z0, x1, y1, z1))
			{
				maxLoop--;
				return;
			}
		}
	}

	// Create a new light update
	lightUpdates.emplace_back(layer, x0, y0, z0, x1, y1, z1);

	int_t limit = 1000000;
	if (lightUpdates.size() > limit)
	{
		std::cout << "More than " << limit << " updates, aborting lighting updates\n";
		lightUpdates.clear();
	}

	maxLoop--;
}

void Level::updateSkyBrightness()
{
	int_t newDarken = getSkyDarken(1.0f);
	if (newDarken != skyDarken)
		skyDarken = newDarken;
}

void Level::setSpawnSettings(bool spawnEnemies, bool spawnFriendlies)
{
	this->spawnEnemies = spawnEnemies;
	this->spawnFriendlies = spawnFriendlies;
}

void Level::tick()
{
	// TODO MobSpawner

	chunkSource->tick();

	int_t newDarken = getSkyDarken(1.0f);
	if (newDarken != skyDarken)
	{
		skyDarken = newDarken;
		for (auto &l : listeners)
			l->skyColorChanged();
	}

	time++;
	if (time % saveInterval == 0)
		save(false, nullptr);

	tickPendingTicks(false);
	tickTiles();
}

void Level::tickTiles()
{
	// TODO
}

bool Level::tickPendingTicks(bool unknown)
{
	// TODO
	return false;
}

void Level::animateTick(int_t x, int_t y, int_t z)
{
	// TODO
}

const std::vector<std::shared_ptr<Entity>> &Level::getEntities(Entity *ignore, AABB &aabb)
{
	es.clear();

	int_t x0 = Mth::floor((aabb.x0 - 2.0) / 16.0);
	int_t x1 = Mth::floor((aabb.x1 + 2.0) / 16.0);
	int_t z0 = Mth::floor((aabb.z0 - 2.0) / 16.0);
	int_t z1 = Mth::floor((aabb.z1 + 2.0) / 16.0);

	for (int_t cx = x0; cx <= x1; cx++)
		for (int_t cz = z0; cz <= z1; cz++)
			getChunk(cx, cz)->getEntities(ignore, aabb, es);

	return es;
}

const std::vector<std::shared_ptr<Entity>> &Level::getEntitiesOfCondition(bool (*condition)(Entity &), AABB &aabb)
{
	es.clear();

	int_t x0 = Mth::floor((aabb.x0 - 2.0) / 16.0);
	int_t x1 = Mth::floor((aabb.x1 + 2.0) / 16.0);
	int_t z0 = Mth::floor((aabb.z0 - 2.0) / 16.0);
	int_t z1 = Mth::floor((aabb.z1 + 2.0) / 16.0);

	for (int_t cx = x0; cx <= x1; cx++)
		for (int_t cz = z0; cz <= z1; cz++)
			getChunk(cx, cz)->getEntitiesOfCondition(condition, aabb, es);

	return es;
}

const std::unordered_set<std::shared_ptr<Entity>> &Level::getAllEntities()
{
	return entities;
}

void Level::tileEntityChanged(int_t x, int_t y, int_t z, std::shared_ptr<TileEntity> tileEntity)
{
	if (hasChunkAt(x, y, z))
		getChunkAt(x, z)->markUnsaved();
	for (auto &l : listeners)
		l->tileEntityChanged(x, y, z, tileEntity);
}

int_t Level::countConditionOf(bool (*condition)(Entity &))
{
	int_t count = 0;
	for (auto &i : entities)
		if (condition(*i))
			count++;
	return count;
}

void Level::addEntities(const std::unordered_set<std::shared_ptr<Entity>> &entities)
{
	this->entities.insert(entities.begin(), entities.end());
	for (auto &i : entities)
		entityAdded(i);
}

void Level::removeEntities(const std::unordered_set<std::shared_ptr<Entity>> &entities)
{
	entitiesToRemove.insert(entities.begin(), entities.end());
}

void Level::disconnect()
{

}

void Level::checkSession()
{
	std::unique_ptr<File> session_lock(File::open(*dir, u"session.lock"));
	std::unique_ptr<std::istream> is(session_lock->toStreamIn());
	if (!is)
		throw std::runtime_error("Failed to check session lock, aborting");

	if (IOUtil::readLong(*is) != sessionId)
		throw std::runtime_error("The save is being accessed from another location, aborting");
}

void Level::setTime(long_t time)
{
	this->time = time;
}

void Level::ensureAdded(std::shared_ptr<Entity> entity)
{
	int_t x = Mth::floor(entity->x / 16.0);
	int_t z = Mth::floor(entity->z / 16.0);
	byte_t rad = 2;

	for (int_t cx = x - rad; cx <= x + rad; cx++)
		for (int_t cz = z - rad; cz <= z + rad; cz++)
			getChunk(cx, cz);

	for (auto &i : entities)
		if (i == entity)
			return;
	entities.emplace(entity);
}

bool Level::mayInteract(std::shared_ptr<Player> player, int_t x, int_t y, int_t z)
{
	return true;
}

void Level::broadcastEntityEvent(std::shared_ptr<Entity> entity, byte_t event)
{

}

std::shared_ptr<ChunkSource> Level::getChunkSource()
{
	return chunkSource;
}
