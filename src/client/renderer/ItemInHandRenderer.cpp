#include "client/renderer/ItemInHandRenderer.h"

#include "client/Minecraft.h"
#include "client/Lighting.h"
#include "client/renderer/entity/EntityRenderDispatcher.h"
#include "client/renderer/entity/PlayerRenderer.h"

#include "util/Mth.h"

ItemInHandRenderer::ItemInHandRenderer(Minecraft &mc) : mc(mc)
{

}

void ItemInHandRenderer::render(float a)
{
	float h = oHeight + (height - oHeight) * a;
	auto &localPlayer = *mc.player;

	glPushMatrix();
	glRotatef(localPlayer.xRotO + (localPlayer.xRot - localPlayer.xRotO) * a, 1.0f, 0.0f, 0.0f);
	glRotatef(localPlayer.yRotO + (localPlayer.yRot - localPlayer.yRotO) * a, 0.0f, 1.0f, 0.0f);
	Lighting::turnOn();
	glPopMatrix();

	float br = mc.level->getBrightness(Mth::floor(localPlayer.x), Mth::floor(localPlayer.y), Mth::floor(localPlayer.z));
	glColor4f(br, br, br, 1.0f);

	// TODO item check
	if (false)
	{

	}
	else
	{
		glPushMatrix();
		float d = 0.8f;

		float swing = localPlayer.getAttackAnim(a);

		float swing1 = Mth::sin(swing * Mth::PI);
		float swing2 = Mth::sin(Mth::sqrt(swing) * Mth::PI);
		glTranslatef(-swing2 * 0.3f, Mth::sin(Mth::sqrt(swing) * Mth::PI * 2.0f) * 0.4f, -swing1 * 0.4f);

		glTranslatef(0.8f * d, -0.75f * d - (1.0f - h) * 0.6f, -0.9f * d);

		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		glEnable(GL_RESCALE_NORMAL_EXT);

		swing = localPlayer.getAttackAnim(a);
		float swing3 = Mth::sin(swing * swing * Mth::PI);
		swing2 = Mth::sin(Mth::sqrt(swing) * Mth::PI);
		glRotatef(swing2 * 70.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(-swing3 * 20.0f, 0.0f, 0.0f, 1.0f);

		glBindTexture(GL_TEXTURE_2D, mc.textures.loadTexture(mc.player->getTexture()));
		glTranslatef(-1.0f, 3.6f, 3.5f);
		glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(200.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(-135.0f, 0.0f, 1.0f, 0.0f);
		glScalef(1.0f, 1.0f, 1.0f);
		glTranslatef(5.6f, 0.0f, 0.0f);

		auto &playerRenderer = EntityRenderDispatcher::playerRenderer;
		float ss = 1.0f;
		glScalef(ss, ss, ss);
		playerRenderer.renderHand();
		glPopMatrix();
	}

	glDisable(GL_RESCALE_NORMAL_EXT);
	Lighting::turnOff();
}

void ItemInHandRenderer::renderScreenEffect(float a)
{
	glDisable(GL_ALPHA_TEST);

	if (mc.player->isOnFire())
	{
		int_t id = mc.textures.loadTexture(u"/terrain.png");
		glBindTexture(GL_TEXTURE_2D, id);
		renderFire(a);
	}

	if (mc.player->isInWall())
	{
		int_t x = Mth::floor(mc.player->x);
		int_t y = Mth::floor(mc.player->y);
		int_t z = Mth::floor(mc.player->z);

		int_t id = mc.textures.loadTexture(u"/terrain.png");
		glBindTexture(GL_TEXTURE_2D, id);
		int_t tile = mc.level->getTile(x, y, z);
		if (Tile::tiles[tile] != nullptr)
			renderTex(a, Tile::tiles[tile]->getTexture(Facing::NORTH));
	}

	// TODO: water

	glEnable(GL_ALPHA_TEST);
}

void ItemInHandRenderer::renderTex(float a, int_t tex)
{
	// TODO
}

void ItemInHandRenderer::renderWater(float a)
{
	// TODO
}

void ItemInHandRenderer::renderFire(float a)
{
	// TODO
}

void ItemInHandRenderer::tick()
{
	oHeight = height;

	auto &localPlayer = *mc.player;

	// TODO item
	bool matches = true;

	float max = 0.4f;
	float tHeight = matches ? 1.0f : 0.0f;
	float dd = tHeight - height;
	if (dd < -max) dd = -max;
	if (dd > max) dd = max;

	height += dd;
	if (height < 0.1f)
	{
		// TODO
	}
}

void ItemInHandRenderer::itemPlaced()
{
	height = 0.0f;
}

void ItemInHandRenderer::itemUsed()
{
	height = 0.0f;
}
