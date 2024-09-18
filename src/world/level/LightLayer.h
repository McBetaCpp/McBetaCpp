#pragma once

#include "java/Type.h"

class LightLayer
{
public:
	static constexpr int_t Sky = 0;
	static constexpr int_t Block = 1;

	static constexpr int_t surrounding(int_t type)
	{
		return (type == Sky) ? 15 : 0;
	}
};
