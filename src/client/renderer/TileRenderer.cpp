#include "client/renderer/TileRenderer.h"

#include "client/renderer/Tesselator.h"

TileRenderer::TileRenderer(LevelSource *levelSource) : level(levelSource)
{

}

TileRenderer::TileRenderer()
{

}

void TileRenderer::tesselateInWorld(Tile &tile, int_t x, int_t y, int_t z, int_t fixedTexture)
{
	this->fixedTexture = fixedTexture;
	tesselateInWorld(tile, x, y, z);
	this->fixedTexture = -1;
}

void TileRenderer::tesselateInWorldNoCulling(Tile &tile, int_t x, int_t y, int_t z)
{
	noCulling = true;
	tesselateInWorld(tile, x, y, z);
	noCulling = false;
}

bool TileRenderer::tesselateInWorld(Tile &tt, int_t x, int_t y, int_t z)
{
	int_t shape = tt.getRenderShape();
	tt.updateShape(*level, x, y, z);

	if (shape == Tile::SHAPE_BLOCK)
		return tesselateBlockInWorld(tt, x, y, z);

	return false;
}

bool TileRenderer::tesselateBlockInWorld(Tile &tt, int_t x, int_t y, int_t z)
{
	int_t col = tt.getColor(*level, x, y, z);
	float r = ((col >> 16) & 0xFF) / 255.0f;
	float g = ((col >> 8) & 0xFF) / 255.0f;
	float b = (col & 0xFF) / 255.0f;
	return tesselateBlockInWorld(tt, x, y, z, r, g, b);
}

bool TileRenderer::tesselateBlockInWorld(Tile &tt, int_t x, int_t y, int_t z, float r, float g, float b)
{
	Tesselator &t = Tesselator::instance;

	bool changed = false;
	float c10 = 0.5f;
	float c11 = 1.0f;
	float c2 = 0.8f;
	float c3 = 0.6f;

	float r11 = c11 * r;
	float g11 = c11 * g;
	float b11 = c11 * b;

	if (&tt == reinterpret_cast<Tile*>(&Tile::grass))
	{
		b = 1.0f;
		g = 1.0f;
		r = 1.0f;
	}

	float r10 = c10 * r;
	float r2 = c2 * r;
	float r3 = c3 * r;

	float g10 = c10 * g;
	float g2 = c2 * g;
	float g3 = c3 * g;

	float b10 = c10 * b;
	float b2 = c2 * b;
	float b3 = c3 * b;

	float centerBrightness = tt.getBrightness(*level, x, y, z);

	if (noCulling || tt.shouldRenderFace(*level, x, y - 1, z, Facing::DOWN))
	{
		float br = tt.getBrightness(*level, x, y - 1, z);

		t.color(r10 * br, g10 * br, b10 * br);
		renderFaceUp(tt, x, y, z, tt.getTexture(*level, x, y, z, Facing::DOWN));
		changed = true;
	}

	if (noCulling || tt.shouldRenderFace(*level, x, y + 1, z, Facing::UP))
	{
		float br = tt.getBrightness(*level, x, y + 1, z);
		if (tt.yy1 != 1.0 && !tt.material.isLiquid()) br = centerBrightness;

		t.color(r11 * br, g11 * br, b11 * br);
		renderFaceDown(tt, x, y, z, tt.getTexture(*level, x, y, z, Facing::UP));
		changed = true;
	}

	if (noCulling || tt.shouldRenderFace(*level, x, y, z - 1, Facing::NORTH))
	{
		float br = tt.getBrightness(*level, x, y, z - 1);
		if (tt.zz0 > 0.0) br = centerBrightness;

		t.color(r2 * br, g2 * br, b2 * br);
		renderNorth(tt, x, y, z, tt.getTexture(*level, x, y, z, Facing::NORTH));
		changed = true;
	}

	if (noCulling || tt.shouldRenderFace(*level, x, y, z + 1, Facing::SOUTH))
	{
		float br = tt.getBrightness(*level, x, y, z + 1);
		if (tt.zz1 < 1.0) br = centerBrightness;

		t.color(r2 * br, g2 * br, b2 * br);
		renderSouth(tt, x, y, z, tt.getTexture(*level, x, y, z, Facing::SOUTH));
		changed = true;
	}

	if (noCulling || tt.shouldRenderFace(*level, x - 1, y, z, Facing::WEST))
	{
		float br = tt.getBrightness(*level, x - 1, y, z);
		if (tt.xx0 > 0.0) br = centerBrightness;

		t.color(r3 * br, g3 * br, b3 * br);
		renderWest(tt, x, y, z, tt.getTexture(*level, x, y, z, Facing::WEST));
		changed = true;
	}

	if (noCulling || tt.shouldRenderFace(*level, x + 1, y, z, Facing::EAST))
	{
		float br = tt.getBrightness(*level, x + 1, y, z);
		if (tt.xx1 < 1.0) br = centerBrightness;

		t.color(r3 * br, g3 * br, b3 * br);
		renderEast(tt, x, y, z, tt.getTexture(*level, x, y, z, Facing::EAST));
		changed = true;
	}

	return changed;
}

void TileRenderer::renderFaceUp(Tile &tt, double x, double y, double z, int_t tex)
{
	Tesselator &t = Tesselator::instance;

	if (fixedTexture >= 0) tex = fixedTexture;

	int_t xt = (tex & 0xF) << 4;
	int_t yt = tex & 0xF0;

	double u0 = (xt + tt.xx0 * 16.0) / 256.0;
	double u1 = (xt + tt.xx1 * 16.0 - 0.01) / 256.0;
	double v0 = (yt + tt.zz0 * 16.0) / 256.0;
	double v1 = (yt + tt.zz1 * 16.0 - 0.01) / 256.0;

	if (tt.xx0 < 0.0 || tt.xx1 > 1.0)
	{
		u0 = ((xt + 0.0f) / 256.0f);
		u1 = ((xt + 15.99f) / 256.0f);
	}

	if (tt.zz0 < 0.0 || tt.zz1 > 1.0)
	{
		v0 = ((yt + 0.0f) / 256.0f);
		v1 = ((yt + 15.99f) / 256.0f);
	}

	double x0 = x + tt.xx0;
	double x1 = x + tt.xx1;
	double y0 = y + tt.yy0;
	double z0 = z + tt.zz0;
	double z1 = z + tt.zz1;

	t.vertexUV(x0, y0, z1, u0, v1);
	t.vertexUV(x0, y0, z0, u0, v0);
	t.vertexUV(x1, y0, z0, u1, v0);
	t.vertexUV(x1, y0, z1, u1, v1);
}

void TileRenderer::renderFaceDown(Tile &tt, double x, double y, double z, int_t tex)
{
	Tesselator &t = Tesselator::instance;

	if (fixedTexture >= 0) tex = fixedTexture;

	int_t xt = (tex & 0xF) << 4;
	int_t yt = tex & 0xF0;

	double u0 = (xt + tt.xx0 * 16.0) / 256.0;
	double u1 = (xt + tt.xx1 * 16.0 - 0.01) / 256.0;
	double v0 = (yt + tt.zz0 * 16.0) / 256.0;
	double v1 = (yt + tt.zz1 * 16.0 - 0.01) / 256.0;

	if (tt.xx0 < 0.0 || tt.xx1 > 1.0)
	{
		u0 = ((xt + 0.0f) / 256.0f);
		u1 = ((xt + 15.99f) / 256.0f);
	}

	if (tt.zz0 < 0.0 || tt.zz1 > 1.0)
	{
		v0 = ((yt + 0.0f) / 256.0f);
		v1 = ((yt + 15.99f) / 256.0f);
	}

	double x0 = x + tt.xx0;
	double x1 = x + tt.xx1;
	double y1 = y + tt.yy1;
	double z0 = z + tt.zz0;
	double z1 = z + tt.zz1;

	t.vertexUV(x1, y1, z1, u1, v1);
	t.vertexUV(x1, y1, z0, u1, v0);
	t.vertexUV(x0, y1, z0, u0, v0);
	t.vertexUV(x0, y1, z1, u0, v1);
}

void TileRenderer::renderNorth(Tile &tt, double x, double y, double z, int_t tex)
{
	Tesselator &t = Tesselator::instance;

	if (fixedTexture >= 0) tex = fixedTexture;
	int_t xt = (tex & 0xF) << 4;
	int_t yt = tex & 0xF0;

	double u0 = (xt + tt.xx0 * 16.0) / 256.0;
	double u1 = (xt + tt.xx1 * 16.0 - 0.01) / 256.0;
	double v0 = (yt + tt.yy0 * 16.0) / 256.0;
	double v1 = (yt + tt.yy1 * 16.0 - 0.01) / 256.0;
	if (xFlipTexture)
	{
		double tmp = u0;
		u0 = u1;
		u1 = tmp;
	}

	if (tt.xx0 < 0.0 || tt.xx1 > 1.0)
	{
		u0 = ((xt + 0.0f) / 256.0f);
		u1 = ((xt + 15.99f) / 256.0f);
	}
	if (tt.yy0 < 0.0 || tt.yy1 > 1.0)
	{
		v0 = ((yt + 0.0f) / 256.0f);
		v1 = ((yt + 15.99f) / 256.0f);
	}

	double x0 = x + tt.xx0;
	double x1 = x + tt.xx1;
	double y0 = y + tt.yy0;
	double y1 = y + tt.yy1;
	double z0 = z + tt.zz0;

	t.vertexUV(x0, y1, z0, u1, v0);
	t.vertexUV(x1, y1, z0, u0, v0);
	t.vertexUV(x1, y0, z0, u0, v1);
	t.vertexUV(x0, y0, z0, u1, v1);
}

void TileRenderer::renderSouth(Tile &tt, double x, double y, double z, int_t tex)
{
	Tesselator &t = Tesselator::instance;

	if (fixedTexture >= 0) tex = fixedTexture;
	int_t xt = (tex & 0xF) << 4;
	int_t yt = tex & 0xF0;

	double u0 = (xt + tt.xx0 * 16.0) / 256.0;
	double u1 = (xt + tt.xx1 * 16.0 - 0.01) / 256.0;
	double v0 = (yt + tt.yy0 * 16.0) / 256.0;
	double v1 = (yt + tt.yy1 * 16.0 - 0.01) / 256.0;
	if (xFlipTexture)
	{
		double tmp = u0;
		u0 = u1;
		u1 = tmp;
	}

	if (tt.xx0 < 0.0 || tt.xx1 > 1.0)
	{
		u0 = ((xt + 0.0f) / 256.0f);
		u1 = ((xt + 15.99f) / 256.0f);
	}
	if (tt.yy0 < 0.0 || tt.yy1 > 1.0)
	{
		v0 = ((yt + 0.0f) / 256.0f);
		v1 = ((yt + 15.99f) / 256.0f);
	}

	double x0 = x + tt.xx0;
	double x1 = x + tt.xx1;
	double y0 = y + tt.yy0;
	double y1 = y + tt.yy1;
	double z1 = z + tt.zz1;

	t.vertexUV(x0, y1, z1, u0, v0);
	t.vertexUV(x0, y0, z1, u0, v1);
	t.vertexUV(x1, y0, z1, u1, v1);
	t.vertexUV(x1, y1, z1, u1, v0);
}

void TileRenderer::renderWest(Tile &tt, double x, double y, double z, int_t tex)
{
	Tesselator &t = Tesselator::instance;

	if (fixedTexture >= 0) tex = fixedTexture;
	int_t xt = (tex & 0xF) << 4;
	int_t yt = tex & 0xF0;

	double u0 = (xt + tt.zz0 * 16.0) / 256.0;
	double u1 = (xt + tt.zz1 * 16.0 - 0.01) / 256.0;
	double v0 = (yt + tt.yy0 * 16.0) / 256.0;
	double v1 = (yt + tt.yy1 * 16.0 - 0.01) / 256.0;
	if (xFlipTexture)
	{
		double tmp = u0;
		u0 = u1;
		u1 = tmp;
	}

	if (tt.zz0 < 0.0 || tt.zz1 > 1.0)
	{
		u0 = ((xt + 0.0f) / 256.0f);
		u1 = ((xt + 15.99f) / 256.0f);
	}
	if (tt.yy0 < 0.0 || tt.yy1 > 1.0)
	{
		v0 = ((yt + 0.0f) / 256.0f);
		v1 = ((yt + 15.99f) / 256.0f);
	}

	double x0 = x + tt.xx0;
	double y0 = y + tt.yy0;
	double y1 = y + tt.yy1;
	double z0 = z + tt.zz0;
	double z1 = z + tt.zz1;

	t.vertexUV(x0, y1, z1, u1, v0);
	t.vertexUV(x0, y1, z0, u0, v0);
	t.vertexUV(x0, y0, z0, u0, v1);
	t.vertexUV(x0, y0, z1, u1, v1);
}

void TileRenderer::renderEast(Tile &tt, double x, double y, double z, int_t tex)
{
	Tesselator &t = Tesselator::instance;

	if (fixedTexture >= 0) tex = fixedTexture;
	int_t xt = (tex & 0xF) << 4;
	int_t yt = tex & 0xF0;

	double u0 = (xt + tt.zz0 * 16.0) / 256.0;
	double u1 = (xt + tt.zz1 * 16.0 - 0.01) / 256.0;
	double v0 = (yt + tt.yy0 * 16.0) / 256.0;
	double v1 = (yt + tt.yy1 * 16.0 - 0.01) / 256.0;
	if (xFlipTexture)
	{
		double tmp = u0;
		u0 = u1;
		u1 = tmp;
	}

	if (tt.zz0 < 0.0 || tt.zz1 > 1.0)
	{
		u0 = ((xt + 0.0f) / 256.0f);
		u1 = ((xt + 15.99f) / 256.0f);
	}
	if (tt.yy0 < 0.0 || tt.yy1 > 1.0)
	{
		v0 = ((yt + 0.0f) / 256.0f);
		v1 = ((yt + 15.99f) / 256.0f);
	}

	double x1 = x + tt.xx1;
	double y0 = y + tt.yy0;
	double y1 = y + tt.yy1;
	double z0 = z + tt.zz0;
	double z1 = z + tt.zz1;

	t.vertexUV(x1, y0, z1, u0, v1);
	t.vertexUV(x1, y0, z0, u1, v1);
	t.vertexUV(x1, y1, z0, u1, v0);
	t.vertexUV(x1, y1, z1, u0, v0);
}

void TileRenderer::renderCube(Tile &tile, float alpha)
{
	int shape = tile.getRenderShape();
	Tesselator &t = Tesselator::instance;
	
	if (shape == Tile::SHAPE_BLOCK)
	{
		tile.updateDefaultShape();

		glTranslatef(-0.5f, -0.5f, -0.5f);
		
		float sd = 0.5f;
		float su = 1.0f;
		float sns = 0.8f;
		float sew = 0.6f;
		
		t.begin();

		t.color(su, su, su, alpha);
		renderFaceUp(tile, 0.0, 0.0, 0.0, tile.getTexture(Facing::DOWN));
		t.color(sd, sd, sd, alpha);
		renderFaceDown(tile, 0.0, 0.0, 0.0, tile.getTexture(Facing::UP));
		t.color(sns, sns, sns, alpha);
		renderNorth(tile, 0.0, 0.0, 0.0, tile.getTexture(Facing::NORTH));
		renderSouth(tile, 0.0, 0.0, 0.0, tile.getTexture(Facing::SOUTH));
		t.color(sew, sew, sew, alpha);
		renderWest(tile, 0.0, 0.0, 0.0, tile.getTexture(Facing::WEST));
		renderEast(tile, 0.0, 0.0, 0.0, tile.getTexture(Facing::EAST));
		
		t.end();
		
		glTranslatef(0.5f, 0.5f, 0.5f);
	}
}
