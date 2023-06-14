#include "Monster.h"

Monster::Monster() = default;

Monster::Monster(Point tile, Asset aObject)
	: StationaryObject(tile, aObject)
{
}