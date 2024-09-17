#pragma once

#include <memory>

#include "client/MemoryTracker.h"
#include "client/renderer/Chunk.h"
#include "client/renderer/TileRenderer.h"
#include "client/renderer/Textures.h"
#include "client/renderer/OffsettedRenderList.h"
#include "client/renderer/culling/Culler.h"

#include "world/level/Level.h"
#include "world/level/LevelListener.h"
#include "world/level/tile/entity/TileEntity.h"
#include "world/item/ItemInstance.h"

#include "world/phys/AABB.h"
#include "world/phys/Vec3.h"

#include "java/Type.h"
#include "java/String.h"

#include "util/Memory.h"

class Minecraft;

class LevelRenderer : public LevelListener
{
public:
	static constexpr int_t CHUNK_SIZE = 16;
	static constexpr int_t MAX_VISIBLE_REBUILDS_PER_FRAME = 3;
	static constexpr int_t MAX_INVISIBLE_REBUILDS_PER_FRAME = 1;

	std::vector<std::shared_ptr<TileEntity>> renderableTileEntities;

private:
	std::shared_ptr<Level> level;

	Textures &textures;

	std::vector<std::shared_ptr<Chunk>> dirtyChunks;
	std::vector<std::shared_ptr<Chunk>> sortedChunks;
	std::vector<std::shared_ptr<Chunk>> chunks;

	int_t xChunks = 0, yChunks = 0, zChunks = 0;
	int_t chunkLists = 0;

	Minecraft &mc;

	std::unique_ptr<TileRenderer> tileRenderer;

	std::vector<int_t> occlusionCheckIds;
	bool occlusionCheck = false;

	int_t ticks = 0;

	int_t starList = 0, skyList = 0, darkList = 0;

	int_t xMinChunk = 0, yMinChunk = 0, zMinChunk = 0;
	int_t xMaxChunk = 0, yMaxChunk = 0, zMaxChunk = 0;

	int_t lastViewDistance = -1;

	int_t noEntityRenderFrames = 2;
	int_t totalEntities = 0;
	int_t renderedEntities = 0;
	int_t culledEntities = 0;

public:
	std::array<int_t, 50000> toRender = {};
	std::vector<int_t> resultBuffer = MemoryTracker::createIntBuffer(64);

private:
	int_t totalChunks = 0;
	int_t offscreenChunks = 0;
	int_t occludedChunks = 0;
	int_t renderedChunks = 0;
	int_t emptyChunks = 0;
	int_t chunkFixOffs = 0;

	std::vector<std::shared_ptr<Chunk>> renderChunksList;
	std::array<OffsettedRenderList, 4> renderLists = {};

public:
	int_t frame = 0;
	int_t repeatList = MemoryTracker::genLists(1);

	double xOld = -9999.0, yOld = -9999.0, zOld = -9999.0;

	float destroyProgress = 0.0f;

	int_t cullStep = 0;

	LevelRenderer(Minecraft &mc, Textures &textures);

private:
	void renderStars();

public:
	void setLevel(std::shared_ptr<Level> level);

	void allChanged() override;

	void renderEntities(Vec3 &cam, Culler &culler, float a);

	jstring gatherStats1();
	jstring gatherStats2();

private:
	void resortChunks(int_t xc, int_t yc, int_t zc);

public:
	int_t render(Player &player, int_t layer, double alpha);

private:
	void checkQueryResults(int_t from, int_t to);

	int_t renderChunks(int_t from, int_t to, int_t layer, double alpha);

public:
	void renderSameAsLast(int_t layer, double alpha);

	void tick();

	void renderSky(float alpha);
	void renderClouds(float alpha);
	void renderAdvancedClouds(float alpha);

	bool updateDirtyChunks(Player &player, bool force);

	void renderHit(Player &player, HitResult &h, int_t mode, ItemInstance *inventoryItem, float a);
	void renderHitOutline(Player &player, HitResult &h, int_t mode, ItemInstance *inventoryItem, float a);

private:
	void render(AABB &b);

public:
	void setDirty(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1);
	void tileChanged(int_t x, int_t y, int_t z) override;
	void setTilesDirty(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1) override;

	void cull(Culler &culler, float a);

	void playStreamingMusic(const jstring &name, int_t x, int_t y, int_t z) override;
	void playSound(const jstring &name, double x, double y, double z, float volume, float pitch) override;

	void addParticle(const jstring &name, double x, double y, double z, double xa, double ya, double za) override;

	void playMusic(const jstring &name, double x, double y, double z, float songOffset) override;

	void entityAdded(std::shared_ptr<Entity> entity) override;
	void entityRemoved(std::shared_ptr<Entity> entity) override;

	void skyColorChanged() override;

	void tileEntityChanged(int_t x, int_t y, int_t z, std::shared_ptr<TileEntity> tileEntity) override;
};
