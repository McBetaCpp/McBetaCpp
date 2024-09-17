#include "client/renderer/Chunk.h"

#include "client/renderer/Tesselator.h"
#include "client/renderer/entity/EntityRenderer.h"
#include "client/renderer/TileRenderer.h"

#include "world/level/Region.h"

#include "util/Mth.h"

int_t Chunk::updates = 0;

Tesselator &Chunk::t = Tesselator::instance;

Chunk::Chunk(Level &level, std::vector<std::shared_ptr<TileEntity>> &globalRenderableTileEntities, int_t x, int_t y, int_t z, int_t size, int_t lists) : level(level), globalRenderableTileEntities(globalRenderableTileEntities)
{
	xs = ys = zs = size;
	radius = Mth::sqrt(static_cast<float>(xs * xs + ys * ys + zs * zs)) / 2.0f;
	this->lists = lists;

	this->x = -999;
	setPos(x, y, z);

	dirty = false;
}

void Chunk::setPos(int_t x, int_t y, int_t z)
{
	if (this->x == x && this->y == y && this->z == z) return;

	reset();
	this->x = x;
	this->y = y;
	this->z = z;
	xm = x + xs / 2;
	ym = y + ys / 2;
	zm = z + zs / 2;

	xRenderOffs = x & 0x3FF;
	yRenderOffs = y;
	zRenderOffs = z & 0x3FF;
	xRender = x - xRenderOffs;
	yRender = y - yRenderOffs;
	zRender = z - zRenderOffs;

	float g = 6.0f;
	bb.reset(AABB::newPermanent(x - g, y - g, z - g, x + xs + g, y + ys + g, z + zs + g));

	glNewList(lists + 2, GL_COMPILE);
	EntityRenderer::renderFlat(*bb);
	glEndList();

	setDirty();
}

void Chunk::translateToPos()
{
	glTranslatef(xRenderOffs, yRenderOffs, zRenderOffs);
}

void Chunk::rebuild()
{
	if (!dirty) return;
	updates++;

	int_t x0 = x;
	int_t y0 = y;
	int_t z0 = z;
	int_t x1 = x + xs;
	int_t y1 = y + ys;
	int_t z1 = z + zs;

	empty.fill(true);

	LevelChunk::touchedSky = false;

	std::unordered_set<std::shared_ptr<TileEntity>> oldTileEntities;
	oldTileEntities.insert(renderableTileEntities.begin(), renderableTileEntities.end());
	renderableTileEntities.clear();

	int_t r = 1;
	Region region(level, x0 - r, y0 - r, z0 - r, x1 + r, y1 + r, z1 + r);
	TileRenderer tileRenderer(&region);

	for (int_t i = 0; i < 2; i++)
	{
		bool renderNextLayer = false;
		bool rendered = false;

		bool started = false;

		for (int_t y = y0; y < y1; y++)
		{
			for (int_t z = z0; z < z1; z++)
			{
				for (int_t x = x0; x < x1; x++)
				{
					int_t tileId = region.getTile(x, y, z);
					if (tileId > 0)
					{
						if (!started)
						{
							started = true;

							glNewList(lists + i, GL_COMPILE);
							
							glPushMatrix();
							translateToPos();

							float ss = 1.0000001f;
							glTranslatef(-zs / 2.0f, -ys / 2.0f, -zs / 2.0f);
							glScalef(ss, ss, ss);
							glTranslatef(zs / 2.0f, ys / 2.0f, zs / 2.0f);

							t.begin();
							t.offset(-this->x, -this->y, -this->z);
						}

						if (i == 0 && Tile::isEntityTile[tileId])
						{
							// TODO
						}

						Tile *tile = Tile::tiles[tileId];
						int_t renderLayer = tile->getRenderLayer();
						if (renderLayer != i)
						{
							renderNextLayer = true;
						}
						else if (renderLayer == i)
						{
							rendered |= tileRenderer.tesselateInWorld(*tile, x, y, z);
						}
					}
				}
			}
		}

		if (started)
		{
			t.end();
			glPopMatrix();
			glEndList();
			t.offset(0.0, 0.0, 0.0);
		}
		else
		{
			rendered = false;
		}

		if (rendered) empty[i] = false;
		if (!renderNextLayer) break;
	}

	skyLit = LevelChunk::touchedSky;
	compiled = true;
}

float Chunk::distanceToSqr(Entity &player)
{
	float dx = static_cast<float>(player.x - static_cast<double>(xm));
	float dy = static_cast<float>(player.y - static_cast<double>(ym));
	float dz = static_cast<float>(player.z - static_cast<double>(zm));
	return dx * dx + dy * dy + dz * dz;
}

float Chunk::squishedDistanceToSqr(Entity &player)
{
	float dx = static_cast<float>(player.x - static_cast<double>(xm));
	float dy = static_cast<float>(player.y - static_cast<double>(ym)) * 2.0f;
	float dz = static_cast<float>(player.z - static_cast<double>(zm));
	return dx * dx + dy * dy + dz * dz;
}

void Chunk::reset()
{
	empty.fill(true);
	visible = false;
	compiled = false;
}

void Chunk::remove()
{
	reset();
}

int_t Chunk::getList(int_t layer)
{
	if (!visible) return -1;
	if (!empty[layer]) return lists + layer;
	return -1;
}
int_t Chunk::getAllLists(std::vector<int_t> displayLists, int_t p, int_t layer)
{
	if (!visible) return p;
	if (!empty[layer]) displayLists[p++] = lists + layer;
	return p;
}

void Chunk::cull(Culler &culler)
{
	visible = culler.isVisible(*bb);
}

void Chunk::renderBB()
{
	glCallList(lists + 2);
}

bool Chunk::isEmpty()
{
	if (!compiled) return false;
	return empty[0] && empty[1];
}

void Chunk::setDirty()
{
	dirty = true;
}
