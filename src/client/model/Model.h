#pragma once

#include "java/Type.h"

class Model
{
public:
	float attackTime = 0.0f;
	bool riding = false;

	virtual ~Model() {}

	virtual void render(float time, float r, float bob, float yRot, float xRot, float scale);
	virtual void setupAnim(float time, float r, float bob, float yRot, float xRot, float scale);
};
