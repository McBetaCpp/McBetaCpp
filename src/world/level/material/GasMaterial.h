#include "world/level/material/Material.h"

class GasMaterial : public Material
{
public:
	bool isSolid() const override;
	bool blocksLight() const override;
	bool blocksMotion() const override;
};
