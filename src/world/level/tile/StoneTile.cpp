#include "StoneTile.h"

StoneTile::StoneTile(int_t id, int_t texture) : Tile(id, texture, Material::stone)
{

}

int_t StoneTile::getResource(int_t subtype, Random &random)
{
	return id;
	// return Tile::stoneBrick.id;
}
