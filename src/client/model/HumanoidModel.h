#pragma once

#include "client/model/Model.h"
#include "client/model/Cube.h"

class HumanoidModel : public Model
{
public:
	Cube cloak = Cube(0, 0);
	Cube ear = Cube(24, 0);
	Cube head = Cube(0, 0);
	Cube hair = Cube(32, 0);
	Cube body = Cube(16, 16);
	Cube arm0 = Cube(40, 16);
	Cube arm1 = Cube(40, 16);
	Cube leg0 = Cube(0, 16);
	Cube leg1 = Cube(0, 16);

	bool holdingLeftHand = false;
	bool holdingRightHand = false;
	bool sneaking = false;

	HumanoidModel();
	HumanoidModel(float g);
	HumanoidModel(float g, float yOffset);

	void render(float time, float r, float bob, float yRot, float xRot, float scale) override;
	void setupAnim(float time, float r, float bob, float yRot, float xRot, float scale) override;

	void renderHair(float scale);
	void renderEars(float scale);
	void renderCloak(float scale);

	void render(HumanoidModel &model, float scale);
};
