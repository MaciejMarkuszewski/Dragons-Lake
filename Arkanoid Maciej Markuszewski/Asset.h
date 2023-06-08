#pragma once
#include "Framework.h"
#include <string>
using namespace std;

class Asset {
public:

	Sprite* pSprite;
	string path;
	int width;
	int height;

	void load(string name);

	Asset();
};

