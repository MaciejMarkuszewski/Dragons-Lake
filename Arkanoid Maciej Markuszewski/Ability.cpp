#include "Ability.h"

Ability::Ability() = default;

Ability::Ability(Point Tile, Asset aObject, AbilityType type)
	: StationaryObject(Tile, aObject), type(type)
{
}