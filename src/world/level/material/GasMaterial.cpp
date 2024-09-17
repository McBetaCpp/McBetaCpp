#include "world/level/material/GasMaterial.h"

bool GasMaterial::isSolid() const
{
	return false;
}

bool GasMaterial::blocksLight() const
{
	return false;
}

bool GasMaterial::blocksMotion() const
{
	return false;
}
