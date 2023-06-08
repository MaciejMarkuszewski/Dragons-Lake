#include "Monster.h"

Monster::Monster() = default;

Monster::Monster(Point Tile, Asset aObject)
	: StationaryObject(Tile, aObject)
{
}