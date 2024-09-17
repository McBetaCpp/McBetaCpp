#include "client/renderer/LevelRenderer.h"

#include <algorithm>

#include "client/Minecraft.h"
#include "client/renderer/DirtyChunkSorter.h"
#include "client/renderer/DistanceChunkSorter.h"
#include "client/renderer/entity/EntityRenderDispatcher.h"

#include "world/level/tile/Tile.h"
#include "world/level/tile/LeafTile.h"

#include "util/Mth.h"

LevelRenderer::LevelRenderer(Minecraft &mc, Textures &textures) : mc(mc), textures(textures)
{
	int_t maxChunksWidth = 64;
	chunkLists = MemoryTracker::genLists(maxChunksWidth * maxChunksWidth * maxChunksWidth * 3);
	occlusionCheck = false;

	starList = MemoryTracker::genLists(3);
	
	glPushMatrix();

	glNewList(starList, GL_COMPILE);
	renderStars();
	glEndList();
	
	glPopMatrix();

	Tesselator &t = Tesselator::instance;
	skyList = starList + 1;
	glNewList(skyList, GL_COMPILE);

	int_t s = 64;
	int_t d = 256 / s + 2;
	float yy = 16.0f;

	for (int_t xx = -s * d; xx <= s * d; xx += s)
	{
		for (int_t zz = -s * d; zz <= s * d; zz += s)
		{
			t.begin();
			t.vertex(xx + 0, yy, zz + 0);
			t.vertex(xx + s, yy, zz + 0);
			t.vertex(xx + s, yy, zz + s);
			t.vertex(xx + 0, yy, zz + s);
			t.end();
		}
	}

	glEndList();

	darkList = starList + 2;
	glNewList(darkList, GL_COMPILE);

	yy = -16.0f;
	t.begin();
	for (int_t xx = -s * d; xx <= s * d; xx += s)
	{
		for (int_t zz = -s * d; zz <= s * d; zz += s)
		{
			t.vertex(xx + s, yy, zz + 0);
			t.vertex(xx + 0, yy, zz + 0);
			t.vertex(xx + 0, yy, zz + s);
			t.vertex(xx + s, yy, zz + s);
		}
	}
	t.end();

	glEndList();
}

void LevelRenderer::renderStars()
{
	Random random = Random(10842L);
	Tesselator &t = Tesselator::instance;
	t.begin();

	for (int i = 0; i < 1500; i++)
	{
		double x = (random.nextFloat() * 2.0f - 1.0f);
		double y = (random.nextFloat() * 2.0f - 1.0f);
		double z = (random.nextFloat() * 2.0f - 1.0f);
		double ss = (0.25F + random.nextFloat() * 0.25F);
		double d = x * x + y * y + z * z;
		if (d < 1.0 && d > 0.01)
		{
			d = 1.0 / std::sqrt(d);
			x *= d;
			y *= d;
			z *= d;
			double xp = x * 100.0;
			double yp = y * 100.0;
			double zp = z * 100.0;
			double yRot = std::atan2(x, z);
			double ySin = std::sin(yRot);
			double yCos = std::cos(yRot);
			double xRot = std::atan2(std::sqrt(x * x + z * z), y);
			double xSin = std::sin(xRot);
			double xCos = std::cos(xRot);
			double zRot = random.nextDouble() * 3.141592653589793 * 2.0;
			double zSin = std::sin(zRot);
			double zCos = std::cos(zRot);

			for (int c = 0; c < 4; c++)
			{
				double ___xo = 0.0;
				double ___yo = ((c & 2) - 1) * ss;
				double ___zo = ((c + 1 & 2) - 1) * ss;
				double __yo = ___yo * zCos - ___zo * zSin;
				double __zo = ___zo * zCos + ___yo * zSin;
				double _yo = __yo * xSin + ___xo * xCos;
				double _xo = ___xo * xSin - __yo * xCos;
				double xo = _xo * ySin - __zo * yCos;
				double zo = __zo * ySin + _xo * yCos;
				t.vertex(xp + xo, yp + _yo, zp + zo);
			}
		}
	}

	t.end();
}

void LevelRenderer::setLevel(std::shared_ptr<Level> level)
{
	this->tileRenderer.reset();

	if (this->level != nullptr)
		this->level->removeListener(*this);

	xOld = -9999.0;
	yOld = -9999.0;
	zOld = -9999.0;

	EntityRenderDispatcher::instance.setLevel(level);

	this->level = level;
	this->tileRenderer = std::make_unique<TileRenderer>(this->level.get());

	if (level != nullptr)
	{
		level->addListener(*this);
		allChanged();
	}
}

void LevelRenderer::allChanged()
{
	Tile::leaves.setFancy(mc.options.fancyGraphics);

	lastViewDistance = mc.options.viewDistance;

	for (auto &chunk : chunks)
		chunk->remove();
	chunks.clear();

	int_t dist = 64 << (3 - lastViewDistance);
	if (dist > 400) dist = 400;

	xChunks = dist / 16 + 1;
	yChunks = 8;
	zChunks = dist / 16 + 1;

	chunks.resize(xChunks * yChunks * zChunks);
	sortedChunks.resize(xChunks * yChunks * zChunks);

	int_t id = 0;
	int_t count = 0;

	xMinChunk = 0;
	yMinChunk = 0;
	zMinChunk = 0;
	xMaxChunk = xChunks;
	yMaxChunk = yChunks;
	zMaxChunk = zChunks;

	for (auto &chunk : dirtyChunks)
		chunk->dirty = false;
	dirtyChunks.clear();

	renderableTileEntities.clear();

	for (int_t x = 0; x < xChunks; x++)
	{
		for (int_t y = 0; y < yChunks; y++)
		{
			for (int_t z = 0; z < zChunks; z++)
			{
				auto chunk = std::make_shared<Chunk>(*level, renderableTileEntities, x * 16, y * 16, z * 16, 16, chunkLists + id);
				chunks[(z * yChunks + y) * xChunks + x] = chunk;

				if (occlusionCheck)
					chunk->occlusion_id = occlusionCheckIds[count];
				chunk->occlusion_querying = false;
				chunk->occlusion_visible = true;
				chunk->visible = true;
				chunk->id = count++;
				chunk->setDirty();

				sortedChunks[(z * yChunks + y) * xChunks + x] = chunk;
				dirtyChunks.push_back(chunk);

				id += 3;
			}
		}
	}

	if (level != nullptr)
	{
		auto &player = mc.player;
		if (player != nullptr)
		{
			resortChunks(Mth::floor(player->x), Mth::floor(player->y), Mth::floor(player->z));
			std::sort(sortedChunks.begin(), sortedChunks.end(), DistanceChunkSorter(*player));
		}
	}

	noEntityRenderFrames = 2;
}

void LevelRenderer::renderEntities(Vec3 &cam, Culler &culler, float a)
{
	if (noEntityRenderFrames > 0)
	{
		noEntityRenderFrames--;
		return;
	}

	EntityRenderDispatcher::instance.prepare(level, textures, *mc.font, mc.player, mc.options, a);
	totalEntities = 0;
	renderedEntities = 0;
	culledEntities = 0;

	auto &player = *mc.player;
	EntityRenderDispatcher::xOff = player.xOld + (player.x - player.xOld) * a;
	EntityRenderDispatcher::yOff = player.yOld + (player.y - player.yOld) * a;
	EntityRenderDispatcher::zOff = player.zOld + (player.z - player.zOld) * a;

	const auto &entities = level->getAllEntities();

	for (auto &entity : entities)
	{
		if (entity->shouldRender(cam) && culler.isVisible(entity->bb) && (entity != mc.player || mc.options.thirdPersonView) && level->hasChunkAt(Mth::floor(entity->x), Mth::floor(entity->y), Mth::floor(entity->z)))
		{
			renderedEntities++;
			EntityRenderDispatcher::instance.render(*entity, a);
		}
	}
}

jstring LevelRenderer::gatherStats1()
{
	return u"C: " + String::toString(renderedChunks) + u"/" + String::toString(totalChunks) + u". F: " + String::toString(offscreenChunks) + u", O: " + String::toString(occludedChunks) + u", E: " + String::toString(emptyChunks);
}

jstring LevelRenderer::gatherStats2()
{
	return u"E: " + String::toString(renderedEntities) + u"/" + String::toString(totalEntities) + u". B: " + String::toString(culledEntities) + u", I: " + String::toString(totalEntities - culledEntities - renderedEntities);
}

void LevelRenderer::resortChunks(int_t xc, int_t yc, int_t zc)
{
	xc -= 8;
	yc -= 8;
	zc -= 8;

	xMinChunk = 0x7FFFFFFF;
	yMinChunk = 0x7FFFFFFF;
	zMinChunk = 0x7FFFFFFF;
	xMaxChunk = -0x80000000;
	yMaxChunk = -0x80000000;
	zMaxChunk = -0x80000000;

	int_t s2 = xChunks * 16;
	int_t s1 = s2 / 2;

	for (int_t x = 0; x < xChunks; x++)
	{
		int_t xx = x * 16;
		int_t xOff = xx + s1 - xc;
		if (xOff < 0) xOff -= s2 - 1;

		xOff /= s2;
		xx -= xOff * s2;
		if (xx < xMinChunk) xMinChunk = xx;
		if (xx > xMaxChunk) xMaxChunk = xx;

		for (int_t z = 0; z < zChunks; z++)
		{
			int_t zz = z * 16;
			int_t zOff = zz + s1 - zc;
			if (zOff < 0) zOff -= s2 - 1;

			zOff /= s2;
			zz -= zOff * s2;
			if (zz < zMinChunk) zMinChunk = zz;
			if (zz > zMaxChunk) zMaxChunk = zz;

			for (int_t y = 0; y < yChunks; y++)
			{
				int_t yy = y * 16;

				if (yy < yMinChunk) yMinChunk = yy;
				if (yy > yMaxChunk) yMaxChunk = yy;

				auto &chunk = chunks[(z * yChunks + y) * xChunks + x];
				bool wasDirty = chunk->dirty;
				chunk->setPos(xx, yy, zz);
				if (!wasDirty && chunk->dirty)
					dirtyChunks.push_back(chunk);
			}
		}
	}
}

int_t LevelRenderer::render(Player &player, int_t layer, double alpha)
{
	for (int_t i = 0; i < 10; i++)
	{
		chunkFixOffs = (chunkFixOffs + 1) % chunks.size();
		auto &c = chunks[chunkFixOffs];
		if (c->dirty)
		{
			bool in_dirty = false;
			for (auto &chunk : dirtyChunks)
			{
				if (chunk == c)
				{
					in_dirty = true;
					break;
				}
			}

			if (!in_dirty)
				dirtyChunks.push_back(c);
		}
	}

	if (mc.options.viewDistance != lastViewDistance)
		allChanged();

	if (layer == 0)
	{
		totalChunks = 0;
		offscreenChunks = 0;
		occludedChunks = 0;
		renderedChunks = 0;
		emptyChunks = 0;
	}

	double xOff = player.xOld + (player.x - player.xOld) * alpha;
	double yOff = player.yOld + (player.y - player.yOld) * alpha;
	double zOff = player.zOld + (player.z - player.zOld) * alpha;
	double xd = player.x - xOld;
	double yd = player.y - yOld;
	double zd = player.z - zOld;
	if ((xd * xd + yd * yd + zd * zd) > (4.0 * 4.0))
	{
		xOld = player.x;
		yOld = player.y;
		zOld = player.z;
		resortChunks(Mth::floor(player.x), Mth::floor(player.y), Mth::floor(player.z));
		std::sort(sortedChunks.begin(), sortedChunks.end(), DistanceChunkSorter(player));
	}

	int_t count = 0;
	if (occlusionCheck)
	{
		// TODO
	}
	else
	{
		count += renderChunks(0, sortedChunks.size(), layer, alpha);
	}

	/*
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	for (auto &lu : level->lightUpdates)
	{
		AABB *bb = AABB::newTemp(-xOff + (float)lu.x0, -yOff + (float)lu.y0, -zOff + (float)lu.z0, -xOff + (float)lu.x1 + 1.0f, -yOff + (float)lu.y1 + 1.0f, -zOff + (float)lu.z1 + 1.0f);
		render(*bb);
	}
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	*/

	return count;
}

void LevelRenderer::checkQueryResults(int_t from, int_t to)
{
	// TODO
}

int_t LevelRenderer::renderChunks(int_t from, int_t to, int_t layer, double alpha)
{
	renderChunksList.clear();

	int_t count = 0;
	for (int_t i = from; i < to; i++)
	{
		if (layer == 0)
		{
			totalChunks++;
			if (sortedChunks[i]->empty[layer])
				emptyChunks++;
			else if (!sortedChunks[i]->visible)
				offscreenChunks++;
			else if (occlusionCheck && !sortedChunks[i]->occlusion_visible)
				occludedChunks++;
			else
				renderedChunks++;
		}

		if (!sortedChunks[i]->empty[layer] && sortedChunks[i]->visible && sortedChunks[i]->occlusion_visible)
		{
			int_t list = sortedChunks[i]->getList(layer);
			if (list >= 0)
			{
				renderChunksList.push_back(sortedChunks[i]);
				count++;
			}
		}
	}

	auto &player = mc.player;

	double xOff = player->xOld + (player->x - player->xOld) * alpha;
	double yOff = player->yOld + (player->y - player->yOld) * alpha;
	double zOff = player->zOld + (player->z - player->zOld) * alpha;

	int_t lists = 0;
	for (auto &l : renderLists)
		l.clear();

	for (int_t i = 0; i < renderChunksList.size(); i++)
	{
		auto &chunk = renderChunksList[i];
		
		int_t list = -1;
		for (int_t l = 0; l < lists; l++)
		{
			if (renderLists[l].isAt(chunk->xRender, chunk->yRender, chunk->zRender))
				list = l;
		}

		if (list < 0)
		{
			list = lists++;
			renderLists[list].init(chunk->xRender, chunk->yRender, chunk->zRender, xOff, yOff, zOff);
		}

		renderLists[list].add(chunk->getList(layer));
	}

	renderSameAsLast(layer, alpha);
	return count;
}

void LevelRenderer::renderSameAsLast(int_t layer, double alpha)
{
	for (auto &l : renderLists)
		l.render();
}

void LevelRenderer::tick()
{
	ticks++;
}

void LevelRenderer::renderSky(float alpha)
{
	if (mc.level->dimension->foggy)
		return;

	glDisable(GL_TEXTURE_2D);

	// Sky
	Vec3 *sc = level->getSkyColor(*mc.player, alpha);
	float sr = sc->x;
	float sg = sc->y;
	float sb = sc->z;
	float xp;
	float yp;
	if (mc.options.anaglyph3d)
	{
		float srr = (sr * 30.0F + sg * 59.0F + sb * 11.0F) / 100.0F;
		xp = (sr * 30.0F + sg * 70.0F) / 100.0F;
		yp = (sr * 30.0F + sb * 70.0F) / 100.0F;
		sr = srr;
		sg = xp;
		sb = yp;
	}

	glColor3f(sr, sg, sb);
	
	Tesselator &t = Tesselator::instance;

	glDepthMask(false);
	glEnable(GL_FOG);
	glColor3f(sr, sg, sb);
	
	glCallList(skyList);
	
	glDisable(GL_FOG);
	glDisable(GL_ALPHA_TEST);

	// Sunrise
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	float *c = level->dimension->getSunriseColor(level->getTimeOfDay(alpha), alpha);
	if (c != nullptr)
	{
		glDisable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);

		glPushMatrix();
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		yp = level->getTimeOfDay(alpha);
		glRotatef(yp > 0.5F ? 180 : 0, 0.0f, 0.0f, 1.0f);

		t.begin(GL_TRIANGLE_FAN);
		t.color(c[0], c[1], c[2], c[3]);
		t.vertex(0.0, 100.0, 0.0);
		t.color(c[0], c[1], c[2], 0.0F);

		int steps = 16;
		for (int i = 0; i <= steps; i++)
		{
			float a = i * 3.1415927f * 2.0f / steps;
			float sin = Mth::sin(a);
			float cos = Mth::cos(a);
			t.vertex((sin * 120.0f), (cos * 120.0f), (-cos * 40.0f * c[3]));
		}

		t.end();
		glPopMatrix();
		glShadeModel(GL_FLAT);
	}

	// Sun, moon, stars
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_ONE, GL_ONE);
	glPushMatrix();
	
	xp = 0.0f;
	yp = 0.0f;
	float zp = 0.0f;
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslatef(xp, yp, zp);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(level->getTimeOfDay(alpha) * 360.0f, 1.0f, 0.0f, 0.0f);
	
	float ss = 30.0f;

	glBindTexture(GL_TEXTURE_2D, textures.loadTexture(u"/terrain/sun.png"));
	t.begin();
	t.vertexUV(-ss, 100.0, -ss, 0.0, 0.0);
	t.vertexUV(ss, 100.0, -ss, 1.0, 0.0);
	t.vertexUV(ss, 100.0, ss, 1.0, 1.0);
	t.vertexUV(-ss, 100.0, ss, 0.0, 1.0);
	t.end();

	ss = 20.0F;
	glBindTexture(GL_TEXTURE_2D, textures.loadTexture(u"/terrain/moon.png"));
	t.begin();
	t.vertexUV(-ss, -100.0, ss, 1.0, 1.0);
	t.vertexUV(ss, -100.0, ss, 0.0, 1.0);
	t.vertexUV(ss, -100.0, -ss, 0.0, 0.0);
	t.vertexUV(-ss, -100.0, -ss, 1.0, 0.0);
	t.end();

	glDisable(GL_TEXTURE_2D);

	float a = level->getStarBrightness(alpha);
	if (a > 0.0f)
	{
		glColor4f(a, a, a, a);
		glCallList(starList);
	}

	// Dark plane
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_FOG);
	glPopMatrix();

	glColor3f(sr * 0.2f + 0.04f, sg * 0.2f + 0.04f, sb * 0.6f + 0.1f);
	glDisable(GL_TEXTURE_2D);
	glCallList(darkList);
	glEnable(GL_TEXTURE_2D);

	glDepthMask(true);
}

void LevelRenderer::renderClouds(float alpha)
{
	if (mc.level->dimension->foggy)
		return;

	if (mc.options.fancyGraphics)
	{
		renderAdvancedClouds(alpha);
		return;
	}

	glDisable(GL_CULL_FACE);

	float yOffs = mc.player->yOld + (mc.player->y - mc.player->yOld) * alpha;
	int_t s = 32;
	int_t d = 256 / s;

	Tesselator &t = Tesselator::instance;
	glBindTexture(GL_TEXTURE_2D, mc.textures.loadTexture(u"/environment/clouds.png"));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vec3 *cc = level->getCloudColor(alpha);
	float cr = cc->x;
	float cg = cc->y;
	float cb = cc->z;

	if (mc.options.anaglyph3d)
	{
		float crr = (cr * 30.0f + cg * 59.0f + cb * 11.0f) / 100.0f;
		float cgg = (cr * 30.0f + cg * 70.0f) / 100.0f;
		float cbb = (cr * 30.0f + cb * 70.0f) / 100.0f;
		cr = crr;
		cg = cgg;
		cb = cbb;
	}

	float scale = 1.0f / 2048.0f;
	
	double xo = mc.player->xo + (mc.player->x - mc.player->xo) * alpha + ((ticks + alpha) * 0.03f);
	double zo = mc.player->zo + (mc.player->z - mc.player->zo) * alpha;
	int_t xOffs = Mth::floor(xo / 2048.0);
	int_t zOffs = Mth::floor(zo / 2048.0);
	xo -= xOffs * 2048;
	zo -= zOffs * 2048;

	float yy = 120.0f - yOffs + 0.33f;
	float uo = xo * scale;
	float vo = zo * scale;

	t.begin();
	t.color(cr, cg, cb, 0.8f);

	for (int_t xx = -s * d; xx < s * d; xx += s) {
		for (int_t zz = -s * d; zz < s * d; zz += s) {
			t.vertexUV(xx + 0, yy, zz + s, (xx + 0) * scale + uo, (zz + s) * scale + vo);
			t.vertexUV(xx + s, yy, zz + s, (xx + s) * scale + uo, (zz + s) * scale + vo);
			t.vertexUV(xx + s, yy, zz + 0, (xx + s) * scale + uo, (zz + 0) * scale + vo);
			t.vertexUV(xx + 0, yy, zz + 0, (xx + 0) * scale + uo, (zz + 0) * scale + vo);
		}
	}

	t.end();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void LevelRenderer::renderAdvancedClouds(float alpha)
{
	glDisable(GL_CULL_FACE);

	float yOffs = mc.player->yOld + (mc.player->y - mc.player->yOld) * alpha;

	Tesselator &t = Tesselator::instance;
	float ss = 12.0f;
	float h = 4.0f;

	double xo = (mc.player->xo + (mc.player->x - mc.player->xo) * alpha + ((ticks + alpha) * 0.03f)) / ss;
	double zo = (mc.player->zo + (mc.player->z - mc.player->zo) * alpha) / ss + 0.33;

	float yy = 108.0f - yOffs + 0.33f;

	int_t xOffs = Mth::floor(xo / 2048.0);
	int_t zOffs = Mth::floor(zo / 2048.0);

	xo -= xOffs * 2048;
	zo -= zOffs * 2048;

	glBindTexture(GL_TEXTURE_2D, mc.textures.loadTexture(u"/environment/clouds.png"));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vec3 *cc = level->getCloudColor(alpha);
	float cr = cc->x;
	float cg = cc->y;
	float cb = cc->z;

	if (mc.options.anaglyph3d)
	{
		float crr = (cr * 30.0f + cg * 59.0f + cb * 11.0f) / 100.0f;
		float cgg = (cr * 30.0f + cg * 70.0f) / 100.0f;
		float cbb = (cr * 30.0f + cb * 70.0f) / 100.0f;
		cr = crr;
		cg = cgg;
		cb = cbb;
	}

	float uo = xo * 0.0;
	float vo = zo * 0.0;
	float scale = 1.0f / 256.0f;

	uo = Mth::floor(xo) * scale;
	vo = Mth::floor(zo) * scale;

	float xoffs = xo - Mth::floor(xo);
	float zoffs = zo - Mth::floor(zo);

	int_t D = 8;
	int_t radius = 3;
	float e = 1.0f / 1024.0f;

	glScalef(ss, 1.0f, ss);

	for (int_t pass = 0; pass < 2; pass++)
	{
		if (pass == 0)
			glColorMask(false, false, false, false);
		else
			glColorMask(true, true, true, true);

		for (int_t xPos = -radius + 1; xPos <= radius; xPos++)
		{
			for (int_t zPos = -radius + 1; zPos <= radius; zPos++)
			{
				t.begin();
				float xx = xPos * D;
				float zz = zPos * D;
				float xp = xx - xoffs;
				float zp = zz - zoffs;

				if (yy > -h - 1.0f)
				{
					t.color(cr * 0.7f, cg * 0.7f, cb * 0.7f, 0.8f);
					t.vertexUV((xp + 0.0F), (yy + 0.0F), (zp + D), ((xx + 0.0F) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + 0.0F), (zp + D), ((xx + D) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + 0.0F), (zp + 0.0F), ((xx + D) * scale + uo), ((zz + 0.0F) * scale + vo));
					t.vertexUV((xp + 0.0F), (yy + 0.0F), (zp + 0.0F), ((xx + 0.0F) * scale + uo), ((zz + 0.0F) * scale + vo));
				}

				if (yy <= h + 1.0F)
				{
					t.color(cr, cg, cb, 0.8F);
					t.normal(0.0F, 1.0F, 0.0F);
					t.vertexUV((xp + 0.0F), (yy + h - e), (zp + D), ((xx + 0.0F) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + h - e), (zp + D), ((xx + D) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + h - e), (zp + 0.0F), ((xx + D) * scale + uo), ((zz + 0.0F) * scale + vo));
					t.vertexUV((xp + 0.0F), (yy + h - e), (zp + 0.0F), ((xx + 0.0F) * scale + uo), ((zz + 0.0F) * scale + vo));
				}

				t.color(cr * 0.9F, cg * 0.9F, cb * 0.9F, 0.8F);
				
				if (xPos > -1)
				{
					t.normal(-1.0F, 0.0F, 0.0F);

					for (int_t i = 0; i < D; i++)
					{
						t.vertexUV((xp + i + 0.0F), (yy + 0.0F), (zp + D), ((xx + i + 0.5F) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 0.0F), (yy + h), (zp + D), ((xx + i + 0.5F) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 0.0F), (yy + h), (zp + 0.0F), ((xx + i + 0.5F) * scale + uo), ((zz + 0.0F) * scale + vo));
						t.vertexUV((xp + i + 0.0F), (yy + 0.0F), (zp + 0.0F), ((xx + i + 0.5F) * scale + uo), ((zz + 0.0F) * scale + vo));
					}
				}

				if (xPos <= 1)
				{
					t.normal(1.0F, 0.0F, 0.0F);

					for (int_t i = 0; i < D; i++)
					{
						t.vertexUV((xp + i + 1.0F - e), (yy + 0.0F), (zp + D), ((xx + i + 0.5F) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 1.0F - e), (yy + h), (zp + D), ((xx + i + 0.5F) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 1.0F - e), (yy + h), (zp + 0.0F), ((xx + i + 0.5F) * scale + uo), ((zz + 0.0F) * scale + vo));
						t.vertexUV((xp + i + 1.0F - e), (yy + 0.0F), (zp + 0.0F), ((xx + i + 0.5F) * scale + uo), ((zz + 0.0F) * scale + vo));
					}
				}

				t.color(cr * 0.8F, cg * 0.8F, cb * 0.8F, 0.8F);
				if (zPos > -1)
				{
					t.normal(0.0F, 0.0F, -1.0F);

					for (int_t i = 0; i < D; i++)
					{
						t.vertexUV((xp + 0.0F), (yy + h), (zp + i + 0.0F), ((xx + 0.0F) * scale + uo), ((zz + i + 0.5F) * scale + vo));
						t.vertexUV((xp + D), (yy + h), (zp + i + 0.0F), ((xx + D) * scale + uo), ((zz + i + 0.5F) * scale + vo));
						t.vertexUV((xp + D), (yy + 0.0F), (zp + i + 0.0F), ((xx + D) * scale + uo), ((zz + i + 0.5F) * scale + vo));
						t.vertexUV((xp + 0.0F), (yy + 0.0F), (zp + i + 0.0F), ((xx + 0.0F) * scale + uo), ((zz + i + 0.5F) * scale + vo));
					}
				}

				if (zPos <= 1)
				{
					t.normal(0.0F, 0.0F, 1.0F);

					for (int_t i = 0; i < D; i++)
					{
						t.vertexUV((xp + 0.0F), (yy + h), (zp + i + 1.0F - e), ((xx + 0.0F) * scale + uo), ((zz + i + 0.5F) * scale + vo));
						t.vertexUV((xp + D), (yy + h), (zp + i + 1.0F - e), ((xx + D) * scale + uo), ((zz + i + 0.5F) * scale + vo));
						t.vertexUV((xp + D), (yy + 0.0F), (zp + i + 1.0F - e), ((xx + D) * scale + uo), ((zz + i + 0.5F) * scale + vo));
						t.vertexUV((xp + 0.0F), (yy + 0.0F), (zp + i + 1.0F - e), ((xx + 0.0F) * scale + uo), ((zz + i + 0.5F) * scale + vo));
					}
				}

				t.end();
			}
		}
	}
}

bool LevelRenderer::updateDirtyChunks(Player &player, bool force)
{
	bool slow = false;
	if (slow)
	{
		std::sort(dirtyChunks.begin(), dirtyChunks.end(), DirtyChunkSorter(player));

		int_t s = dirtyChunks.size() - 1;
		int_t amount = dirtyChunks.size();

		for (int_t i = 0; i < amount; i++)
		{
			std::shared_ptr<Chunk> chunk = dirtyChunks[s - i];
			if (!force)
			{
				if (chunk->distanceToSqr(player) > 1024.0f)
				{
					if (chunk->visible)
					{
						if (i >= 3)
							return false;
					}
					else
					{
						if (i >= 1)
							return false;
					}
				}
			}
			else if (!chunk->visible)
			{
				continue;
			}

			chunk->rebuild();
			dirtyChunks.erase(dirtyChunks.begin() + (s - i));
			chunk->dirty = false;
		}

		if (dirtyChunks.empty())
			return true;
		else
			return false;
	}
	else
	{
		DirtyChunkSorter dirtyChunkSorter(player);

		std::array<std::shared_ptr<Chunk>, 3> toAdd = {};
		std::vector<std::shared_ptr<Chunk>> nearChunks;

		int_t pendingChunkSize = dirtyChunks.size();
		int_t pendingChunkRemoved = 0;

		for (int_t i = 0; i < pendingChunkSize; i++)
		{
			std::shared_ptr<Chunk> chunk = dirtyChunks[i];
			if (!force)
			{
				if (chunk->distanceToSqr(player) > 1024.0f)
				{
					int_t index;
					for (index = 0; index < 3 && (toAdd[index] == nullptr || dirtyChunkSorter(toAdd[index], chunk)); index++);
					index--;
					if (index > 0)
					{
						int_t x = index;
						while (--x != 0) toAdd[x - 1] = toAdd[x];
						toAdd[index] = chunk;
					}
					continue;
				}
			}
			else if (!chunk->visible)
			{
				continue;
			}

			pendingChunkRemoved++;
			nearChunks.emplace_back(chunk);
			dirtyChunks[i] = nullptr;
			continue;
		}

		if (!nearChunks.empty())
		{
			if (nearChunks.size() > 1)
				std::sort(nearChunks.begin(), nearChunks.end(), dirtyChunkSorter);
			for (int_t i = nearChunks.size() - 1; i >= 0; i--)
			{
				std::shared_ptr<Chunk> chunk = nearChunks[i];
				chunk->rebuild();
				chunk->dirty = false;
			}
		}

		int_t secondaryRemoved = 0;
		for (int_t j = 2; j >= 0; j--)
		{
			std::shared_ptr<Chunk> chunk = toAdd[j];
			if (chunk != nullptr)
			{
				if (!chunk->visible && j != 2)
				{
					toAdd[j] = nullptr;
					toAdd[0] = nullptr;
					break;
				}
				toAdd[j]->rebuild();
				toAdd[j]->dirty = false;
				secondaryRemoved++;
			}
		}

		int_t cursor = 0;
		int_t target = 0;
		int_t arraySize = dirtyChunks.size();
		while (cursor != arraySize)
		{
			std::shared_ptr<Chunk> chunk = dirtyChunks[cursor];
			if (chunk != nullptr && chunk != toAdd[0] && chunk != toAdd[1] && chunk != toAdd[2])
			{
				if (target != cursor)
					dirtyChunks[target] = chunk;
				target++;
			}
			cursor++;
		}
		while (--cursor >= target)
			dirtyChunks.erase(dirtyChunks.begin() + cursor);
	
		return pendingChunkSize == (pendingChunkRemoved + secondaryRemoved);
	}
}

void LevelRenderer::renderHit(Player &player, HitResult &h, int_t mode, ItemInstance *inventoryItem, float a)
{
	Tesselator &t = Tesselator::instance;
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor4f(1.0F, 1.0F, 1.0F, (Mth::sin(System::currentTimeMillis() / 100.0f) * 0.2f + 0.4f) * 0.5f);

	if (mode == 0)
	{
		if (destroyProgress > 0.0f)
		{
			glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
			int_t id = textures.loadTexture(u"/terrain.png");
			glBindTexture(GL_TEXTURE_2D, id);
			glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

			glPushMatrix();

			int_t tileId = level->getTile(h.x, h.y, h.z);
			Tile *tile = (tileId > 0) ? Tile::tiles[tileId] : nullptr;
			glDisable(GL_ALPHA_TEST);
			glPolygonOffset(-3.0f, -3.0f);
			glEnable(GL_POLYGON_OFFSET_FILL);

			t.begin();

			double xo = player.xOld + (player.x - player.xOld) * a;
			double yo = player.yOld + (player.y - player.yOld) * a;
			double zo = player.zOld + (player.z - player.zOld) * a;
			t.offset(-xo, -yo, -zo);
			t.noColor();

			if (tile == nullptr)
				tile = reinterpret_cast<Tile*>(&Tile::rock);

			tileRenderer->tesselateInWorld(*tile, h.x, h.y, h.z, 240 + static_cast<int_t>(destroyProgress * 10.0f));
			t.end();
			t.offset(0.0, 0.0, 0.0);

			glPolygonOffset(0.0f, 0.0f);
			glDisable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_ALPHA_TEST);
			glDepthMask(true);

			glPopMatrix();
		}
	}
}

void LevelRenderer::renderHitOutline(Player &player, HitResult &h, int_t mode, ItemInstance *inventoryItem, float a)
{
	if (mode == 0 && h.type == HitResult::Type::TILE)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
		glLineWidth(2.0f);
		glDisable(GL_TEXTURE_2D);
		glDepthMask(false);
		
		float ss = 0.002F;
		int tileId = level->getTile(h.x, h.y, h.z);
		if (tileId > 0)
		{
			Tile::tiles[tileId]->updateShape(*level, h.x, h.y, h.z);
			double xo = player.xOld + (player.x - player.xOld) * a;
			double yo = player.yOld + (player.y - player.yOld) * a;
			double zo = player.zOld + (player.z - player.zOld) * a;
			render(*Tile::tiles[tileId]->getTileAABB(*level, h.x, h.y, h.z)->grow(ss, ss, ss)->cloneMove(-xo, -yo, -zo));
		}

		glDepthMask(true);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
}

void LevelRenderer::render(AABB &b)
{
	Tesselator &t = Tesselator::instance;
	t.begin(GL_LINE_STRIP);
	t.vertex(b.x0, b.y0, b.z0);
	t.vertex(b.x1, b.y0, b.z0);
	t.vertex(b.x1, b.y0, b.z1);
	t.vertex(b.x0, b.y0, b.z1);
	t.vertex(b.x0, b.y0, b.z0);
	t.end();
	t.begin(GL_LINE_STRIP);
	t.vertex(b.x0, b.y1, b.z0);
	t.vertex(b.x1, b.y1, b.z0);
	t.vertex(b.x1, b.y1, b.z1);
	t.vertex(b.x0, b.y1, b.z1);
	t.vertex(b.x0, b.y1, b.z0);
	t.end();
	t.begin(GL_LINES);
	t.vertex(b.x0, b.y0, b.z0);
	t.vertex(b.x0, b.y1, b.z0);
	t.vertex(b.x1, b.y0, b.z0);
	t.vertex(b.x1, b.y1, b.z0);
	t.vertex(b.x1, b.y0, b.z1);
	t.vertex(b.x1, b.y1, b.z1);
	t.vertex(b.x0, b.y0, b.z1);
	t.vertex(b.x0, b.y1, b.z1);
	t.end();
}

void LevelRenderer::setDirty(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	int_t _x0 = Mth::intFloorDiv(x0, 16);
	int_t _y0 = Mth::intFloorDiv(y0, 16);
	int_t _z0 = Mth::intFloorDiv(z0, 16);
	int_t _x1 = Mth::intFloorDiv(x1, 16);
	int_t _y1 = Mth::intFloorDiv(y1, 16);
	int_t _z1 = Mth::intFloorDiv(z1, 16);

	for (int_t x = _x0; x <= _x1; x++)
	{
		int_t xx = x % xChunks;
		if (xx < 0) xx += xChunks;

		for (int_t y = _y0; y <= _y1; y++)
		{
			int_t yy = y % yChunks;
			if (yy < 0) yy += yChunks;

			for (int_t z = _z0; z <= _z1; z++)
			{
				int_t zz = z % zChunks;
				if (zz < 0) zz += zChunks;

				auto &chunk = chunks[(zz * yChunks + yy) * xChunks + xx];
				if (!chunk->dirty)
				{
					dirtyChunks.push_back(chunk);
					chunk->setDirty();
				}
			}
		}
	}
}

void LevelRenderer::tileChanged(int_t x, int_t y, int_t z)
{
	setDirty(x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}

void LevelRenderer::setTilesDirty(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	setDirty(x0 - 1, y0 - 1, z0 - 1, x1 + 1, y1 + 1, z1 + 1);
}

void LevelRenderer::cull(Culler &culler, float a)
{
	for (int_t i = 0; i < chunks.size(); i++)
	{
		auto &c = chunks[i];
		if (!c->isEmpty() && (!c->visible || ((i + cullStep) & 15) == 0))
			c->cull(culler);
	}

	cullStep++;
}

void LevelRenderer::playStreamingMusic(const jstring &name, int_t x, int_t y, int_t z)
{

}

void LevelRenderer::playSound(const jstring &name, double x, double y, double z, float volume, float pitch)
{

}

void LevelRenderer::addParticle(const jstring &name, double x, double y, double z, double xa, double ya, double za)
{

}

void LevelRenderer::playMusic(const jstring &name, double x, double y, double z, float songOffset)
{

}

void LevelRenderer::entityAdded(std::shared_ptr<Entity> entity)
{

}

void LevelRenderer::entityRemoved(std::shared_ptr<Entity> entity)
{

}

void LevelRenderer::skyColorChanged()
{
	for (auto &i : chunks)
	{
		if (i->skyLit && !i->dirty)
		{
			dirtyChunks.push_back(i);
			i->setDirty();
		}
	}
}

void LevelRenderer::tileEntityChanged(int_t x, int_t y, int_t z, std::shared_ptr<TileEntity> tileEntity)
{

}
