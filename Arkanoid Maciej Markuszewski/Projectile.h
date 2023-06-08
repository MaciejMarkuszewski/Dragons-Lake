#pragma once
#include "Point.h"
#include "Asset.h"
#include "Rectangle.h"

class Projectile {
public:

	Point position;
	Point target;
	bool isFriendly;
	Asset aProjectile;

	Projectile();

	Projectile(Point position, Point target, bool isFriendly, Asset aProjectile);

	Rectangle getHitbox();

	Point getSpritePosition(Asset aPlatform, int windowHeight, int view);
};

