#include "client/model/HumanoidModel.h"

HumanoidModel::HumanoidModel() : HumanoidModel(0.0f)
{

}

HumanoidModel::HumanoidModel(float g) : HumanoidModel(g, 0.0f)
{

}

HumanoidModel::HumanoidModel(float g, float yOffset)
{
	cloak.addBox(-5.0f, 0.0f, -1.0f, 10, 16, 1, g);

	ear.addBox(-3.0f, -6.0f, -1.0f, 6, 6, 1, g);

	head.addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8, g);
	head.setPos(0.0f, 0.0f + yOffset, 0.0f);

	hair.addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8, g + 0.5f);
	hair.setPos(0.0f, 0.0f + yOffset, 0.0f);

	body.addBox(-4.0f, 0.0f, -2.0f, 8, 12, 4, g);
	body.setPos(0.0f, 0.0f + yOffset, 0.0f);

	arm0.addBox(-3.0f, -2.0f, -2.0f, 4, 12, 4, g);
	arm0.setPos(-5.0f, 2.0f + yOffset, 0.0f);

	arm1.mirror = true;
	arm1.addBox(-1.0f, -2.0f, -2.0f, 4, 12, 4, g);
	arm1.setPos(5.0f, 2.0f + yOffset, 0.0f);

	leg0.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4, g);
	leg0.setPos(-2.0f, 12.0f + yOffset, 0.0f);

	leg1.mirror = true;
	leg1.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4, g);
	leg1.setPos(2.0f, 12.0f + yOffset, 0.0f);
}

void HumanoidModel::render(float time, float r, float bob, float yRot, float xRot, float scale)
{
	setupAnim(time, r, bob, yRot, xRot, scale);

	head.render(scale);
	body.render(scale);
	arm0.render(scale);
	arm1.render(scale);
	leg0.render(scale);
	leg1.render(scale);
	hair.render(scale);
}

void HumanoidModel::setupAnim(float time, float r, float bob, float yRot, float xRot, float scale)
{
	head.yRot = yRot / Mth::RADDEG;
	head.xRot = xRot / Mth::RADDEG;
	hair.yRot = head.yRot;
	hair.xRot = head.xRot;

	arm0.xRot = Mth::cos(time * 0.6662f + Mth::PI) * 2.0f * r * 0.5f;
	arm1.xRot = Mth::cos(time * 0.6662f) * 2.0f * r * 0.5f;

	arm0.zRot = 0.0f;
	arm1.zRot = 0.0f;

	leg0.xRot = Mth::cos(time * 0.6662f) * 1.4f * r;
	leg1.xRot = Mth::cos(time * 0.6662f + Mth::PI) * 1.4f * r;
	leg0.yRot = 0.0f;
	leg1.yRot = 0.0f;

	if (riding)
	{
		arm0.xRot += -36.0f * Mth::DEGRAD;
		arm1.xRot += -36.0f * Mth::DEGRAD;
		leg0.xRot = 72.0f * Mth::DEGRAD;
		leg1.xRot = 72.0f * Mth::DEGRAD;
		leg0.yRot = 18.0f * Mth::DEGRAD;
		leg1.yRot = -18.0f * Mth::DEGRAD;
	}

	if (holdingLeftHand)
		arm1.xRot = arm1.xRot * 0.5f - (18.0f * Mth::DEGRAD);
	if (holdingRightHand)
		arm0.xRot = arm0.xRot * 0.5f - (18.0f * Mth::DEGRAD);

	arm0.yRot = 0.0f;
	arm1.yRot = 0.0f;

	if (attackTime > -9990.0f)
	{
		float swing = attackTime;
		body.yRot = Mth::sin(Mth::sqrt(swing) * Mth::PI * 2.0f) * 0.2f;
		arm0.z = Mth::sin(body.yRot) * 5.0f;
		arm0.x = -Mth::cos(body.yRot) * 5.0f;
		arm1.z = -Mth::sin(body.yRot) * 5.0f;
		arm1.x = Mth::cos(body.yRot) * 5.0f;
		arm0.yRot += body.yRot;
		arm1.yRot += body.yRot;
		arm1.xRot += body.yRot;

		swing = 1.0f - attackTime;
		swing *= swing;
		swing *= swing;
		swing = 1.0f - swing;

		float aa = Mth::sin(swing * Mth::PI);
		float bb = Mth::sin(attackTime * Mth::PI) * -(head.xRot - 0.7f) * 0.75f;
		arm0.xRot = arm0.xRot - (aa * 1.2 + bb);
		arm0.yRot += body.yRot * 2.0f;
		arm0.zRot = Mth::sin(attackTime * Mth::PI) * -0.4f;
	}

	if (sneaking)
	{
		body.xRot = 0.5f;
		leg0.xRot -= 0.0f;
		leg1.xRot -= 0.0f;
		arm0.xRot += 0.4f;
		arm1.xRot += 0.4f;
		leg0.z = 4.0f;
		leg1.z = 4.0f;
		leg0.y = 9.0f;
		leg1.y = 9.0f;
		head.y = 1.0f;
	}
	else
	{
		body.xRot = 0.0f;
		leg0.z = 0.0f;
		leg1.z = 0.0f;
		leg0.y = 12.0f;
		leg1.y = 12.0f;
		head.y = 0.0f;
	}

	arm0.zRot += Mth::cos(bob * 0.09f) * 0.05f + 0.05f;
	arm1.zRot -= Mth::cos(bob * 0.09f) * 0.05f + 0.05f;
	arm0.xRot += Mth::sin(bob * 0.067f) * 0.05f;
	arm1.xRot -= Mth::sin(bob * 0.067f) * 0.05f;
}

void HumanoidModel::renderHair(float scale)
{
	hair.yRot = head.yRot;
	hair.xRot = head.xRot;
	hair.render(scale);
}

void HumanoidModel::renderEars(float scale)
{
	ear.yRot = head.yRot;
	ear.xRot = head.xRot;
	ear.render(scale);
}

void HumanoidModel::renderCloak(float scale)
{
	// TODO
}

void HumanoidModel::render(HumanoidModel &model, float scale)
{
	head.yRot = model.head.yRot;
	head.y = model.head.y;
	head.xRot = model.head.xRot;
	hair.yRot = head.yRot;
	hair.xRot = head.xRot;

	arm0.xRot = model.arm0.xRot;
	arm0.yRot = model.arm0.yRot;

	arm1.xRot = model.arm1.xRot;
	arm1.yRot = model.arm1.yRot;

	leg0.xRot = model.leg0.xRot;
	leg1.xRot = model.leg1.xRot;

	head.render(scale);
	body.render(scale);
	arm0.render(scale);
	arm1.render(scale);
	leg0.render(scale);
	leg1.render(scale);
	hair.render(scale);
}
