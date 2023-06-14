#include "Ability.h"

Ability::Ability() = default;

Ability::Ability(Point tile, Asset aObject, AbilityType type)
	: StationaryObject(tile, aObject), type(type)
{
}