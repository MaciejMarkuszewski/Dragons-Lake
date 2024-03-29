#include "Projectile.h"

Projectile::Projectile() = default;

Projectile::Projectile(Point position, Point target, bool isFriendly, Asset aProjectile)
	: position(position), target(target), isFriendly(isFriendly), aProjectile(aProjectile)
{
}

Rectangle Projectile::getHitbox() {
	return Rectangle(Point(position.x - aProjectile.width / 2, position.y + aProjectile.height / 2), Point(position.x + aProjectile.width / 2, position.y - aProjectile.height / 2));
}

Point Projectile::getSpritePosition(Asset aPlatform, int windowHeight, int view) {
	return Point(position.x - aProjectile.width / 2, windowHeight - position.y - aProjectile.height / 2 - aPlatform.height - view);
}

void Projectile::updatePosition(int windowWidth) {
	position.x += target.x;
	position.y += target.y;
	if (position.x >= windowWidth)
	{
		position.x -= windowWidth;
	}
	if (position.x < 0)
	{
		position.x += windowWidth;
	}
}