#pragma once

#include "java/Type.h"

class LightLayer
{
public:
	static constexpr int_t Sky = 0;
	static constexpr int_t Block = 1;

	static constexpr int_t surrounding(int_t type)
	{
		if (type == Sky)
			return 15;
		if (type == Block)
			return 0;
		return 0;
	}
};
