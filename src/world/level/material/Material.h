#pragma once

class DecorationMaterial;
class GasMaterial;
class LiquidMaterial;

class Material
{
public:
	static GasMaterial air;
	static Material dirt;
	static Material wood;
	static Material stone;

	static Material sand;

private:
	bool flammableFlag = false;

public:
	virtual ~Material() {}

	virtual bool isLiquid() const;
	virtual bool letsWaterThrough() const;
	virtual bool isSolid() const;
	virtual bool blocksLight() const;
	virtual bool blocksMotion() const;

private:
	Material &flammable();

public:
	virtual bool isFlammable();
};
