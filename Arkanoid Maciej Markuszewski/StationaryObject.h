#pragma once
#include "Rectangle.h"
#include "Asset.h"

class StationaryObject {
public:

	Point tile;
	Asset aObject;

	StationaryObject();

	StationaryObject(Point tile, Asset aObject);

	Rectangle getHitbox(int tileSize, int jumpedBy);

	Point getSpritePosition(Asset aPlatform, int windowHeight, int view, int tileSize, int jumpedBy);

	Point getShootingPosition(int tileSize);
};

