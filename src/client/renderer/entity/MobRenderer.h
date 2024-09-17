#pragma once

#include "client/renderer/entity/EntityRenderer.h"

#include "client/model/Model.h"

#include "world/entity/Mob.h"

class MobRenderer : public EntityRenderer
{
private:
	static constexpr int_t MAX_ARMOR_LAYERS = 4;

protected:
	std::shared_ptr<Model> model;
	std::shared_ptr<Model> armor;

public:
	MobRenderer(EntityRenderDispatcher &entityRenderDispatcher, const std::shared_ptr<Model> &model, float shadow);

	void setArmor(const std::shared_ptr<Model> &armor);

	void render(Entity &entity, double x, double y, double z, float rot, float a) override;

protected:
	void setupRotations(Mob &mob, float bob, float bodyRot, float a);
	float getAttackAnim(Mob &mob, float a);
	float getBob(Mob &mob, float a);

	virtual void additionalRendering(Mob &mob, float a);

	virtual bool prepareArmor(Mob &mob, int_t layer, float a);
	virtual float getFlipDegrees(Mob &mob);
	virtual int_t getOverlayColor(Mob &mob, float br, float b);
	virtual void scale(Mob &mob, float a);
};
