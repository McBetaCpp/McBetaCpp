#pragma once

#include "client/renderer/Chunk.h"
#include "world/entity/player/Player.h"

struct DirtyChunkSorter
{
private:
	Player &player;

public:
	DirtyChunkSorter(Player &player) : player(player)
	{

	}

	bool operator()(const std::shared_ptr<Chunk> &c0, const std::shared_ptr<Chunk> &c1) const
	{
		bool i0 = c0->visible;
		bool i1 = c1->visible;

		if (i0 && !i1) return false;
		if (i1 && !i0) return true;

		double d0 = c0->distanceToSqr(player);
		double d1 = c1->distanceToSqr(player);

		if (d0 < d1) return false;
		if (d0 > d1) return true;

		return (c0->id < c1->id) ? false : true;
	}
};
