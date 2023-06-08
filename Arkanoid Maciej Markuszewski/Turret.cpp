#include "Turret.h"

Turret::Turret() = default;

Turret::Turret(Point Tile, Asset aObject, unsigned int shootingCooldown)
	: StationaryObject(Tile, aObject), shootingCooldown(shootingCooldown)
{
}