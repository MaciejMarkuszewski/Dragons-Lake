#include "Asset.h"

void Asset::load(string name) {
	path = "assets/" + name + ".png";
	pSprite = createSprite(path.c_str());
	getSpriteSize(pSprite, width, height);
}

Asset::Asset() = default;