#pragma once
#include "Rectangle.h"
#include "Facing.h"
#include "AbilityType.h"
#include "Asset.h"

class Player {
public:

	Point position;
	int highestFloor;
	int jumpedBy;
	int platformsTouchedCount;
	Facing facingDirection;
	bool isHoldingKey;
	int jumpVelocity;
	AbilityType currentAbility;
	int abilityDuration;
	unsigned int shootingCooldown;
	Asset aPlayer;

	Player();

	Player(Point position, int highestFloor, int jumpedBy, int platformsTouched, Facing facingDirection, bool isHoldingKey, int jumpVelocity, AbilityType currentAbility, int abilityDuration, unsigned int shootingCooldown, Asset aPlayer);

	Rectangle getHitbox();

	Point getSpritePosition(Asset aPlatform, int windowHeight, int view);

	Point getShootingPosition();

	void updatePosition(int speed, int windowWidth);
};

