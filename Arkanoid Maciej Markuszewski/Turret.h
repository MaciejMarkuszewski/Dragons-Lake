#pragma once
#include "StationaryObject.h"

class Turret : public StationaryObject {
public:

	unsigned int shootingCooldown;

	Turret();

	Turret(Point Tile, Asset aObject, unsigned int shootingCooldown);
};

