#include "Player.h"

Player::Player() = default;

Player::Player(Point position, int highestFloor, int jumpedBy, int platformsTouchedCount, Facing facingDirection, bool isHoldingKey, int jumpVelocity, AbilityType currentAbility, int abilityDuration, unsigned int shootingCooldown, Asset aPlayer)
	: position(position), highestFloor(highestFloor), jumpedBy(jumpedBy), platformsTouchedCount(platformsTouchedCount), facingDirection(facingDirection), isHoldingKey(isHoldingKey), jumpVelocity(jumpVelocity), currentAbility(currentAbility), abilityDuration(abilityDuration), shootingCooldown(shootingCooldown), aPlayer(aPlayer)
{
}

Rectangle Player::getHitbox() {
	return Rectangle(Point(position.x - aPlayer.width / 4, position.y + aPlayer.height), Point(position.x + aPlayer.width / 4, position.y));
}

Point Player::getSpritePosition(Asset aPlatform, int windowHeight, int view) {
	return Point(position.x - aPlayer.width / 2, windowHeight - position.y - aPlayer.height - aPlatform.height - view);
}

Point Player::getShootingPosition() {
	return Point(position.x, position.y + aPlayer.height / 2);
}

void Player::updatePosition(int speed, int windowWidth) {

	if (isHoldingKey)
	{
		position.x += facingDirection == RIGHT ? speed : -speed;
	}
	if (position.x >= windowWidth)
	{
		position.x -= windowWidth;
	}
	if (position.x < 0)
	{
		position.x += windowWidth;
	}
	position.y += jumpVelocity;
	jumpVelocity--;
}