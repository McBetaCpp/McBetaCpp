#pragma once

#include <cmath>

#include "java/Type.h"

namespace Mth
{

static constexpr float PI = 3.1415927f;
static constexpr float RAD_TO_GRAD = 0.017453292f;
static constexpr float DEGRAD = 0.017453292f;
static constexpr float RADDEG = 57.295776f;

float sin(float angle);
float cos(float angle);

float sqrt(float value);
float sqrt(double value);

int_t floor(float value);
int_t fastFloor(double value);
int_t floor(double value);
int_t absFloor(double value);

float abs(float value);

int_t ceil(float value);

double asbMax(double a, double b);

int_t intFloorDiv(int_t a, int_t b);

}
