#pragma once
#include "StationaryObject.h"

class Turret : public StationaryObject {
public:

	unsigned int shootingCooldown;

	Turret();

	Turret(Point tile, Asset aObject, unsigned int shootingCooldown);
};

