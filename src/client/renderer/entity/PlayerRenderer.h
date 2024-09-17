#pragma once

#include "client/renderer/entity/MobRenderer.h"

class PlayerRenderer : public MobRenderer
{
private:
	std::shared_ptr<HumanoidModel> humanoidModel;
	std::shared_ptr<HumanoidModel> armorParts1;
	std::shared_ptr<HumanoidModel> armorParts2;

public:
	PlayerRenderer(EntityRenderDispatcher &entityRenderDispatcher);

	void render(Entity &entity, double x, double y, double z, float rot, float a) override;

protected:
	void scale(Mob &mob, float a);

public:
	void renderHand();
};
