#include "client/renderer/entity/MobRenderer.h"

#include "client/renderer/entity/EntityRenderDispatcher.h"

MobRenderer::MobRenderer(EntityRenderDispatcher &entityRenderDispatcher, std::shared_ptr<Model> model, float shadow) : EntityRenderer(entityRenderDispatcher)
{
	this->model = model;
	this->shadowRadius = shadow;
}

void MobRenderer::setArmor(std::shared_ptr<Model> armor)
{
	this->armor = armor;
}

void MobRenderer::render(Entity &entity, double x, double y, double z, float rot, float a)
{
	Mob &mob = static_cast<Mob &>(entity);

	glPushMatrix();
	glDisable(GL_CULL_FACE);

	model->attackTime = getAttackAnim(mob, a);
	model->riding = mob.isRiding();
	if (armor != nullptr) armor->riding = model->riding;

	float bodyRot = mob.yBodyRotO + (mob.yBodyRot - mob.yBodyRotO) * a;
	float headRot = mob.yRotO + (mob.yRot - mob.yRotO) * a;
	float headRotx = mob.xRotO + (mob.xRot - mob.xRotO) * a;

	glTranslatef(x, y, z);

	float bob = getBob(mob, a);
	setupRotations(mob, bob, bodyRot, a);

	float scale = 1.0f / 16.0f;
	glEnable(GL_RESCALE_NORMAL_EXT);
	glScalef(-1.0f, -1.0f, 1.0f);

	this->scale(mob, a);
	glTranslatef(0.0f, -24.0f * scale - (1.0f / 128.0f), 0.0f);

	float ws = mob.walkAnimSpeedO + (mob.walkAnimSpeed - mob.walkAnimSpeedO) * a;
	float wp = mob.walkAnimPos - mob.walkAnimSpeed * (1.0f - a);

	if (ws > 1.0f) ws = 1.0f;

	bindTexture(mob.customTextureUrl, mob.getTexture());
	glEnable(GL_ALPHA_TEST);

	model->render(wp, ws, bob, headRot - bodyRot, headRotx, scale);
	for (int_t i = 0; i < MAX_ARMOR_LAYERS; i++)
	{
		if (prepareArmor(mob, i, a))
		{
			armor->render(wp, ws, bob, headRot - bodyRot, headRotx, scale);
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
		}
	}

	additionalRendering(mob, a);
	float br = mob.getBrightness(a);
	int_t overlayColor = getOverlayColor(mob, br, a);

	if (((overlayColor >> 24) & 0xFF) > 0 || mob.hurtTime > 0 || mob.deathTime > 0)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_EQUAL);

		if (mob.hurtTime > 0 || mob.deathTime > 0)
		{
			glColor4f(br, 0.0f, 0.0f, 0.4f);
			model->render(wp, ws, bob, headRot - bodyRot, headRotx, scale);
			for (int_t i = 0; i < MAX_ARMOR_LAYERS; i++)
			{
				if (prepareArmor(mob, i, a))
				{
					armor->render(wp, ws, bob, headRot - bodyRot, headRotx, scale);
					glDisable(GL_BLEND);
					glDisable(GL_ALPHA_TEST);
				}
			}
		}

		if (((overlayColor >> 24) & 0xFF) > 0)
		{
			float r = ((overlayColor >> 16) & 0xFF) / 255.0f;
			float g = ((overlayColor >> 8) & 0xFF) / 255.0f;
			float b = (overlayColor & 0xFF) / 255.0f;
			float aa = ((overlayColor >> 24) & 0xFF) / 255.0f;
			glColor4f(r, g, b, aa);
			model->render(wp, ws, bob, headRot - bodyRot, headRotx, scale);
			for (int_t i = 0; i < MAX_ARMOR_LAYERS; i++)
			{
				if (prepareArmor(mob, i, a))
				{
					armor->render(wp, ws, bob, headRot - bodyRot, headRotx, scale);
					glDisable(GL_BLEND);
					glDisable(GL_ALPHA_TEST);
				}
			}
		}

		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_TEXTURE_2D);
	}

	glDisable(GL_RESCALE_NORMAL_EXT);

	glEnable(GL_CULL_FACE);

	glPopMatrix();
}

void MobRenderer::setupRotations(Mob &mob, float bob, float bodyRot, float a)
{
	glRotatef(180.0f - bodyRot, 0.0f, 1.0f, 0.0f);
	if (mob.deathTime > 0)
	{
		float fall = (mob.deathTime + a - 1.0f) / 20.0f * 1.6f;
		fall = Mth::sqrt(fall);
		if (fall > 1.0f) fall = 1.0f;
		glRotatef(fall * getFlipDegrees(mob), 0.0f, 0.0f, 1.0f);
	}
}

float MobRenderer::getAttackAnim(Mob &mob, float a)
{
	return mob.getAttackAnim(a);
}

float MobRenderer::getBob(Mob &mob, float a)
{
	return mob.tickCount + a;
}

void MobRenderer::additionalRendering(Mob &mob, float a)
{

}

bool MobRenderer::prepareArmor(Mob &mob, int_t layer, float a)
{
	return false;
}

float MobRenderer::getFlipDegrees(Mob &mob)
{
	return 90.0f;
}

int_t MobRenderer::getOverlayColor(Mob &mob, float br, float b)
{
	return 0;
}

void MobRenderer::scale(Mob &mob, float a)
{

}
