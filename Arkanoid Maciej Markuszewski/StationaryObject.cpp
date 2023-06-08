#include "StationaryObject.h"

StationaryObject::StationaryObject() = default;

StationaryObject::StationaryObject(Point tile, Asset aObject)
	: tile(tile), aObject(aObject)
{
}

Rectangle StationaryObject::getHitbox(int tileSize, int jumpedBy) {
	return Rectangle(Point(tile.x * tileSize + (tileSize - aObject.width) / 2, (tile.y - jumpedBy) * tileSize + aObject.height), Point(tile.x * tileSize + (tileSize + aObject.width) / 2, (tile.y - jumpedBy) * tileSize));
}

Point StationaryObject::getSpritePosition(Asset aPlatform, int windowHeight, int view, int tileSize, int jumpedBy) {
	return Point(tile.x * tileSize + (tileSize - aObject.width) / 2, windowHeight - (tile.y - jumpedBy) * tileSize - aObject.height - aPlatform.height - view);
}

Point StationaryObject::getShootingPosition(int tileSize) {
	return Point(tile.x * tileSize + tileSize / 2, tile.y * tileSize + aObject.height / 2);
}