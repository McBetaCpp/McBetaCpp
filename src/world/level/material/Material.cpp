#include "world/level/material/Material.h"

#include "world/level/material/GasMaterial.h"

GasMaterial Material::air;
Material Material::dirt;
Material Material::wood = Material().flammable();
Material Material::stone;

Material Material::sand;

bool Material::isLiquid() const
{
	return false;
}

bool Material::letsWaterThrough() const
{
	return (!isLiquid() && !isSolid());
}

bool Material::isSolid() const
{
	return true;
}

bool Material::blocksLight() const
{
	return true;
}

bool Material::blocksMotion() const
{
	return true;
}

Material &Material::flammable()
{
	flammableFlag = true;
	return *this;
}

bool Material::isFlammable()
{
	return flammableFlag;
}
