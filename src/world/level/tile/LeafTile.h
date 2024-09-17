#pragma once

#include "world/level/tile/TransparentTile.h"

class LeafTile : public TransparentTile
{
public:
	static constexpr int_t REQUIRED_WOOD_RANGE = 4;

	static constexpr int_t UPDATE_LEAF_BIT = 1 << 2;

	static constexpr int_t NORMAL_LEAF = 0;
	static constexpr int_t EVERGREEN_LEAF = 1;
	static constexpr int_t BIRCH_LEAF = 2;

	static constexpr int_t LEAF_TYPE_MASK = UPDATE_LEAF_BIT - 1;

private:
	int_t oTex = 0;

public:
	LeafTile(int_t id, int_t tex);

	int_t getColor(LevelSource &level, int_t x, int_t y, int_t z) override;

	void onRemove(Level &level, int_t x, int_t y, int_t z) override;
	void tick(Level &level, int_t x, int_t y, int_t z, Random &random) override;

private:
	void die(Level &level, int_t x, int_t y, int_t z);

public:
	int_t getResourceCount(Random &random) override;
	int_t getResource(int_t data, Random &random) override;

	bool isSolidRender() override;

	int_t getTexture(Facing face, int_t data) override;

	void setFancy(bool fancy);

	void stepOn(Level &level, int_t x, int_t y, int_t z, Entity &entity) override;
};
