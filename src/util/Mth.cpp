#include "util/Mth.h"

#include <cmath>
#include <iostream>

static constexpr int_t BIG_ENOUGH_INT = 1024;
static constexpr float BIG_ENOUGH_FLOAT = 1024.0f;

namespace Mth
{

float sin(float angle)
{
	// TODO
	return std::sin(angle);
}
float cos(float angle)
{
	// TODO
	return std::cos(angle);
}

float sqrt(float value)
{
	return std::sqrt(value);
}
float sqrt(double value)
{
	return std::sqrt(value);
}

int_t floor(float value)
{
	int_t i = value;
	return (value < i) ? (i - 1) : i;
}
int_t fastFloor(double value)
{
	return static_cast<int_t>(value + BIG_ENOUGH_FLOAT) - BIG_ENOUGH_INT;
}
int_t floor(double value)
{
	int_t i = value;
	return (value < i) ? (i - 1) : i;
}
int_t absFloor(double value)
{
	return (value >= 0.0) ? value : (-value + 1);
}

float abs(float value)
{
	return (value >= 0.0f) ? value : -value;
}

int_t ceil(float value)
{
	int_t i = value;
	return (value > i) ? (i + 1) : i;
}

/*
class Main
{
public:
	Main()
	{
		for (Type::byte i = -64; i <= 64; i++)
			std::cout << static_cast<int>(i) << " -> " << intFloorDiv(i, 32) << '\n';
	}
};
static Main main;
*/

double asbMax(double a, double b)
{
	if (a < 0)
		a = -a;
	if (b < 0)
		b = -b;
	return (a > b) ? a : b;
}

int_t intFloorDiv(int_t a, int_t b)
{
	return (a < 0) ? (-((-a - 1) / b) - 1) : (a / b);
}

}
