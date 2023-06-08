#pragma once
#include "StationaryObject.h"
#include "AbilityType.h"

class Ability : public StationaryObject {
public:

	AbilityType type;

	Ability();

	Ability(Point Tile, Asset aObject, AbilityType type);
};

