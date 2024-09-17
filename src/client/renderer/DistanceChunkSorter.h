#pragma once

#include "client/renderer/Chunk.h"
#include "world/entity/player/Player.h"

struct DistanceChunkSorter
{
private:
	Player &player;

public:
	DistanceChunkSorter(Player &player) : player(player)
	{

	}

	bool operator()(const std::shared_ptr<Chunk> &c0, const std::shared_ptr<Chunk> &c1) const
	{
		return c0->distanceToSqr(player) < c1->distanceToSqr(player);
	}
};
