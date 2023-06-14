#include "Turret.h"

Turret::Turret() = default;

Turret::Turret(Point tile, Asset aObject, unsigned int shootingCooldown)
	: StationaryObject(tile, aObject), shootingCooldown(shootingCooldown)
{
}