#pragma once
#include "StationaryObject.h"

class Monster : public StationaryObject {
public:

	Monster();

	Monster(Point tile, Asset aObject);
};