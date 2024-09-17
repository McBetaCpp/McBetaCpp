#include "client/renderer/entity/PlayerRenderer.h"

#include "world/entity/player/Player.h"

PlayerRenderer::PlayerRenderer(EntityRenderDispatcher &entityRenderDispatcher) : MobRenderer(entityRenderDispatcher, Util::make_shared<HumanoidModel>(0.0f), 0.5f)
{
	humanoidModel = std::static_pointer_cast<HumanoidModel>(model);

	armorParts1 = Util::make_shared<HumanoidModel>(1.0f);
	armorParts2 = Util::make_shared<HumanoidModel>(0.5f);
}

void PlayerRenderer::render(Entity &entity, double x, double y, double z, float rot, float a)
{
	Player &mob = reinterpret_cast<Player &>(entity);

	humanoidModel->holdingRightHand = false;
	humanoidModel->sneaking = mob.isSneaking();

	double yp = y - mob.heightOffset;
	if (mob.isSneaking())
		yp -= 0.125;

	MobRenderer::render(mob, x, yp, z, rot, a);
}

void PlayerRenderer::scale(Mob &mob, float a)
{
	float s = 0.9375f;
	glScalef(s, s, s);
}

void PlayerRenderer::renderHand()
{
	humanoidModel->attackTime = 0.0f;
	humanoidModel->setupAnim(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / 16.0f);
	humanoidModel->arm0.render(1.0f / 16.0f);
}
