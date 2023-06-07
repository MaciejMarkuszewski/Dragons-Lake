#include "Framework.h"
#include <iostream>
#include <string>
#include<cstdlib>
#include <cmath>
#include <deque>
using namespace std;

enum AbilityType {
	NONE,
	AUTOSHOOTER
};

enum Facing {
	RIGHT,
	LEFT
};


struct Point {

	int x, y;

	Point() = default;

	Point(int x, int y)
		: x(x), y(y)
	{
	}
};

struct Rectangle {

	Point l, r;

	Rectangle() = default;

	Rectangle(Point l, Point r)
		: l(l), r(r)
	{
	}
};

class Asset {

	public:

		Sprite* pSprite;
		string path;
		int width;
		int height;

		void load(string name) {
			path = "assets/" + name + ".png";
			pSprite = createSprite(path.c_str());
			getSpriteSize(pSprite, width, height);
		}

		Asset() = default;
};

class Player {

	public:

		Point position;
		int highestFloor;
		int jumpedBy;
		int platformsTouched;
		Facing facingDirection;
		bool isHoldingKey;
		int jumpVelocity;
		AbilityType currentAbility;
		int abilityDuration;
		unsigned int shootingCooldown;
		Asset aPlayer;

		Player() = default;

		Player(Point position, int highestFloor, int jumpedBy, int platformsTouched, Facing facingDirection, bool isHoldingKey, int jumpVelocity, AbilityType currentAbility, int abilityDuration, unsigned int shootingCooldown, Asset aPlayer)
			: position(position), highestFloor(highestFloor), jumpedBy(jumpedBy), platformsTouched(platformsTouched), facingDirection(facingDirection), isHoldingKey(isHoldingKey), jumpVelocity(jumpVelocity), currentAbility(currentAbility), abilityDuration (abilityDuration), shootingCooldown(shootingCooldown), aPlayer(aPlayer)
		{
		}

		Rectangle getHitbox() {
			return Rectangle(Point(position.x - aPlayer.width / 4, position.y + aPlayer.height * 2 / 3), Point(position.x + aPlayer.width / 4, position.y));
		}

		Point getSpritePosition(Asset aPlatform, int windowHeight, int view) {
			return Point(position.x - aPlayer.width / 2, windowHeight - position.y - aPlayer.height - aPlatform.height - view);
		}

		Point getShootingPosition() {
			return Point(position.x, position.y + aPlayer.height /2);
		}
};

class Projectile {

	public:

		Point position;
		Point target;
		bool isFriendly;
		Asset aProjectile;

		Projectile() = default;

		Projectile(Point position, Point target, bool isFriendly, Asset aProjectile)
			: position(position), target(target), isFriendly(isFriendly), aProjectile(aProjectile)
		{
		}

		Rectangle getHitbox() {
			return Rectangle(Point(position.x - aProjectile.width / 2, position.y + aProjectile.height / 2), Point(position.x + aProjectile.width / 2, position.y - aProjectile.height / 2));
		}

		Point getSpritePosition(Asset aPlatform, int windowHeight, int view) {
			return Point(position.x - aProjectile.width / 2, windowHeight - position.y - aProjectile.height /2 - aPlatform.height - view);
		}
};

class StationaryObject {

	public:

		Point tile;
		Asset aObject;

		StationaryObject() = default;

		StationaryObject(Point tile, Asset aObject)
			: tile(tile), aObject(aObject)
		{
		}

		Rectangle getHitbox(int tileSize, int view) {
			return Rectangle(Point(tile.x * tileSize + (tileSize - aObject.width) / 2, tile.y * tileSize + aObject.height - view), Point(tile.x * tileSize + (tileSize + aObject.width) / 2, tile.y * tileSize - view));
		}

		Point getSpritePosition(Asset aPlatform, int windowHeight, int view, int tileSize, int jumpedBy) {
			return Point(tile.x * tileSize + (tileSize - aObject.width)/2, windowHeight - (tile.y - jumpedBy) * tileSize - aObject.height - aPlatform.height - view);
		}

		Point getShootingPosition(int tileSize) {
			return Point(tile.x * tileSize + tileSize/2, tile.y * tileSize + aObject.height / 2);
		}

};

class Turret : public StationaryObject {

	public:

		unsigned int shootingCooldown;

		Turret() = default;

		Turret(Point Tile, Asset aObject, unsigned int shootingCooldown)
			: StationaryObject(Tile,aObject), shootingCooldown(shootingCooldown)
		{
		}

};

class Ability : public StationaryObject {

	public:

		AbilityType type;

		Ability() = default;

		Ability(Point Tile, Asset aObject, AbilityType type)
			: StationaryObject(Tile, aObject), type(type)
		{
		}
};


class Monster : public StationaryObject {

public:

	Monster() = default;

	Monster(Point Tile, Asset aObject)
		: StationaryObject(Tile, aObject)
	{
	}
};

// Returns true if two rectangles (l1, r1) and (l2, r2)
// overlap
bool doOverlap(Rectangle one, Rectangle two)
{
	// if rectangle has area 0, no overlap
	if (one.l.x == one.r.x || one.l.y == one.r.y || two.l.x == two.r.x || two.l.y == two.r.y)
		return false;

	// If one rectangle is on left side of other
	if (one.l.x > two.r.x || two.l.x > one.r.x)
		return false;

	// If one rectangle is above other
	if (one.r.y > two.l.y || two.r.y > one.l.y)
		return false;

	return true;
}

void createPlatformRow(int* pColumn, int size, int platforms, int monsters, int turrets) {
	bool failed = true;
	for (int i = 0; i < size; i++) {
		int random = rand() % 1000;
		if (random < platforms) {
			pColumn[i] = 0;
		}
		else if (random < monsters) {
			pColumn[i] = 1;
			failed = false;
		}
		else if (random < turrets) {
			pColumn[i] = 2;
			failed = false;
		}
		else {
			pColumn[i] = 3;
		}
	}
	if (failed) {
		pColumn[rand() % 8] = 1;
	}
}


/* Test Framework realization */
class MyFramework : public Framework {

public:

	Asset aPlatform;
	Asset aPlayerL;
	Asset aPlayerR;
	Asset aPlayerJumpingL;
	Asset aPlayerJumpingR;
	Asset aBackground;
	Asset aScores;
	Asset aAdd;
	Asset aFriendly;
	Asset aHostile;
	Asset aTurret;
	Asset aMonster;
	Asset aAutoShooter;
	Asset aDigits[10];
	deque <int*> Platforms;
	Player player;
	int windowWidth;
	int windowHeight;
	int view = 0;
	Point cursor;
	deque <Projectile> Projectiles;
	deque <Monster> Monsters;
	deque <Turret> Turrets;
	deque <Ability> Abilities;
	int previousTick = 0;
	int tileSize;
	int nextAbility;

	void reset() {
		
		Projectiles.clear();
		Monsters.clear();
		Turrets.clear();
		Abilities.clear();
		createPlatformRow(Platforms[0], 10, 0, 1000, 1000);
		for (int row = 1; row < 10; row++) {
			createPlatformRow(Platforms[row], 10, 800, 1000, 1000);
		}
		player = Player(Point(windowWidth / 2, 0), 0, 0, 0, RIGHT, false, 0, NONE, 0, getTickCount(), aPlayerR);
		nextAbility = getTickCount() + 15000 + (rand() % 15000);
	}

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = 1200;
		height = 800;
		fullscreen = false;
	}

	virtual bool Init() {
		getScreenSize(windowWidth, windowHeight);
		aPlatform.load("Platform");
		aPlayerL.load("PlayerL");
		aPlayerR.load("PlayerR");
		aPlayerJumpingL.load("PlayerJumpingL");
		aPlayerJumpingR.load("PlayerJumpingR");
		aBackground.load("Background");
		aScores.load("Scores");
		aAdd.load("Add");
		aFriendly.load("Friendly");
		aHostile.load("Hostile");
		aTurret.load("Turret");
		aMonster.load("Monster");
		aAutoShooter.load("AutoShooter");
		for (int i = 0; i < 10; i++) {
			aDigits[i].load(to_string(i));
		}
		tileSize = aPlatform.width;
		static int rows[10][10];
		for (int row = 0; row < 10; row++) {
			Platforms.push_back(rows[row]);
		}
		reset();
		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick() {
		if (getTickCount() >= previousTick + 20) {
			previousTick = getTickCount();
			if (player.isHoldingKey) {
				player.position.x += player.facingDirection == RIGHT ? 10 : -10;
			}
			if (player.position.x >= windowWidth) {
				player.position.x -= windowWidth;
			}
			if (player.position.x < 0) {
				player.position.x += windowWidth;
			}
			player.position.y += player.jumpVelocity;
			player.jumpVelocity--;
			if (player.jumpVelocity > 0 && view > 0) {
				view -= player.jumpVelocity;
				if (view < 0) {
					view = 0;
					for (int i = Monsters.size() - 1; i >= 0; i--) {
						Monsters[i].tile.y -= player.jumpedBy;
						if (Monsters[i].tile.y < 0) {
							Monsters.pop_back();
						}
					}
					for (int i = Turrets.size() - 1; i >= 0; i--) {
						Turrets[i].tile.y -= player.jumpedBy;
						if (Turrets[i].tile.y < 0) {
							Turrets.pop_back();
						}
					}
					for (int i = Abilities.size() - 1; i >= 0; i--) {
						Abilities[i].tile.y -= player.jumpedBy;
						if (Abilities[i].tile.y < 0) {
							Abilities.pop_back();
							nextAbility = getTickCount() + 15000 + (rand() % 15000);
						}
					}
					for (int i = 0; i < player.jumpedBy; i++) {
						if ((player.highestFloor + Platforms.size() - player.jumpedBy + i + 2) % 100 == 0) {
							createPlatformRow(Platforms[0], 10, 0, 1000, 1000);
						}
						else {
							createPlatformRow(Platforms[0], 10, 750, 945, 995);
						}
						for (int column = 0; column < 10; column++) {
							if (Platforms[0][column] == 2) {
								Monsters.push_front(Monster(Point(column, Platforms.size() - player.jumpedBy + i),aMonster));
							}
							else if (Platforms[0][column] == 3) {
								Turrets.push_front(Turret(Point(column, Platforms.size() - player.jumpedBy + i),aTurret,0));
							}
							else if (Platforms[0][column] == 1 && getTickCount() >= nextAbility && nextAbility % 10 == column) {
								Platforms[0][column] = 4;
								Abilities.push_front(Ability(Point(column, Platforms.size() - player.jumpedBy + i),aAutoShooter, AUTOSHOOTER));
							}
						}
						Platforms.push_back(Platforms[0]);
						Platforms.pop_front();
					}
					player.highestFloor += player.jumpedBy;
					player.jumpedBy = 0;
				}
			}
			if (player.jumpVelocity < 0) {
				int row = (player.position.y + aPlatform.height) / tileSize;
				if (abs(player.position.y - row * tileSize + aPlatform.height / 2) <= aPlatform.height / 2 && (Platforms[row][(player.getHitbox().l.x) / tileSize] != 0 || Platforms[row][(player.getHitbox().r.x) / tileSize] != 0)) {
					player.jumpVelocity = 25;
					player.jumpedBy = row;
					player.platformsTouched++;
					view = row * tileSize;
					player.position.y = 0;
					for (int i = 0; i < Projectiles.size(); i++) {
						Projectiles[i].position.y -= view;
					}
				}
			}
			for (int i = Turrets.size() - 1; i >= 0; i--) {
				if (Turrets[i].tile.y <= 5) {
					if (getTickCount() >= Turrets[i].shootingCooldown) {
						float distance = sqrt(pow((player.position.x - (Turrets[i].tile.x + 0.5) * tileSize), 2) + pow(player.position.y - Turrets[i].tile.y * tileSize, 2));
						Projectile hostile = Projectile(Turrets[i].getShootingPosition(tileSize), Point((player.position.x - (Turrets[i].tile.x + 0.5) * tileSize) / distance * 10, (player.position.y - Turrets[i].tile.y * tileSize) / distance * 10), false, aHostile);
						Projectiles.push_back(hostile);
						Turrets[i].shootingCooldown = getTickCount() + 1000;
					}
				}
				else {
					break;
				}
			}
			for (int i = 0; i < Abilities.size(); i++) {
				if (doOverlap(player.getHitbox(),Abilities[i].getHitbox(tileSize,view))) {
					player.currentAbility = AUTOSHOOTER;
					player.abilityDuration = getTickCount() + 20000;
					Platforms[Abilities[i].tile.y][Abilities[i].tile.x] = 1;
					Abilities.erase(Abilities.begin() + i);
					nextAbility = getTickCount() + 15000 + (rand() % 15000);
				}
			}
			if (player.currentAbility == AUTOSHOOTER && getTickCount() >= player.shootingCooldown) {
				for (int i = Monsters.size() - 1; i >= 0 ;i--) {
					if (Monsters[i].tile.y <= 5) {
						float distance = sqrt(pow((Monsters[i].tile.x * tileSize + (tileSize - aMonster.width) / 2 - player.position.x), 2) + pow(Monsters[i].tile.y * tileSize - player.position.y, 2));
						Projectile friendly = Projectile(player.getShootingPosition(), Point((Monsters[i].tile.x * tileSize + (tileSize - aMonster.width) / 2 - player.position.x) / distance * 10, (Monsters[i].tile.y * tileSize - player.position.y) / distance * 10), true, aFriendly);
						Projectiles.push_back(friendly);
					}
					else {
						break;
					}
				}
				if (getTickCount() >= player.abilityDuration) {
					player.currentAbility = NONE;
				}
				player.shootingCooldown = getTickCount() + 500;
			}
			for (int i = Projectiles.size() -1; i >=0; i--) {
				Projectiles[i].position.x += Projectiles[i].target.x;
				Projectiles[i].position.y += Projectiles[i].target.y;
				if (Projectiles[i].position.x >= windowWidth) {
					Projectiles[i].position.x -= windowWidth;
				}
				if (Projectiles[i].position.x < 0) {
					Projectiles[i].position.x += windowWidth;
				}
				if (abs(Projectiles[i].position.y - windowHeight / 2) > windowHeight / 2 && view == 0) {
					Projectiles.erase(Projectiles.begin() + i);
					continue;
				}
				if (Projectiles[i].isFriendly) {
					for (int j = 0; j < Monsters.size(); j++) {
						if (doOverlap(Projectiles[i].getHitbox(), Monsters[j].getHitbox(tileSize,view))) {
							Platforms[Monsters[j].tile.y][Monsters[j].tile.x] = 1;
							Monsters.erase(Monsters.begin() + j);
							Projectiles.erase(Projectiles.begin() + i);
							break;
						}
					}
				}
				else {
					if (doOverlap(player.getHitbox(), Projectiles[i].getHitbox())) {
						reset();
						return false;
					}
				}
			}
			for (int i = 0; i < Monsters.size(); i++) {
				if (doOverlap(player.getHitbox(), Monsters[i].getHitbox(tileSize, view))) {
					if (player.position.y >= Monsters[i].tile.y * tileSize && player.jumpVelocity <= 0) {
						Platforms[Monsters[i].tile.y][Monsters[i].tile.x] = 1;
						Monsters.erase(Monsters.begin() + i);
					}
					else {
						reset();
						return false;
					}
					break;
				}
			}
			if (player.position.y < 0) {
				reset();
				return false;
			}
		}
		drawSprite(aBackground.pSprite, 0, 0);
		for (int row = 0; row < Platforms.size();row++)
		{
			for (int column = 0; column < 10; column++) {
				if (Platforms[row][column] != 0) {
					drawSprite(aPlatform.pSprite, column * tileSize, windowHeight - aPlatform.height - (row - player.jumpedBy) * tileSize - view);
					if (Platforms[row][column] > 1) {
						StationaryObject object;
						switch (Platforms[row][column]) {
							case 2:
								object = StationaryObject(Point(column, row), aMonster);
								break;
							case 3:
								object = StationaryObject(Point(column, row), aTurret);
								break;
							case 4:
								object = StationaryObject(Point(column, row), aAutoShooter);
								break;
						}
						drawSprite(object.aObject.pSprite, object.getSpritePosition(aPlatform, windowHeight, view, tileSize, player.jumpedBy).x, object.getSpritePosition(aPlatform, windowHeight, view, tileSize, player.jumpedBy).y);
					}
				}
			}
		}
		for (int i = 0; i < Projectiles.size(); i++) {
			drawSprite(Projectiles[i].isFriendly ? aFriendly.pSprite : aHostile.pSprite, Projectiles[i].getSpritePosition(aPlatform,windowHeight,view).x, Projectiles[i].getSpritePosition(aPlatform, windowHeight, view).y);
		}
		if (player.jumpVelocity > 0) {
			drawSprite(player.facingDirection == RIGHT ? aPlayerJumpingR.pSprite : aPlayerJumpingL.pSprite,player.getSpritePosition(aPlatform,windowHeight,view).x, player.getSpritePosition(aPlatform, windowHeight, view).y);
		}
		else {
			drawSprite(player.facingDirection == RIGHT ? aPlayerR.pSprite : aPlayerL.pSprite, player.getSpritePosition(aPlatform, windowHeight, view).x, player.getSpritePosition(aPlatform, windowHeight, view).y);
		}
		drawSprite(aScores.pSprite, 0, 0);
		string floor = to_string(player.highestFloor);
		for (int i = 0; i < floor.length(); i++) {
			for (int j = 0; j < 10; j++) {
				if (int(floor[i]) - 48 == j) {
					drawSprite(aDigits[j].pSprite, 100 + i * 36, 10);
					break;
				}
			}
		}
		if (player.jumpedBy > 0) {
			drawSprite(aAdd.pSprite, 100 + floor.length() * 36, 10);
			for (int j = 0; j < 10; j++) {
				if (player.jumpedBy == j) {
					drawSprite(aDigits[j].pSprite, 100 + (floor.length() + 1) * 36, 10);
					break;
				}
			}
		}
		floor = to_string(player.platformsTouched);
		for (int i = 0; i < floor.length(); i++) {
			for (int j = 0; j < 10; j++) {
				if (int(floor[i]) - 48 == j) {
					drawSprite(aDigits[j].pSprite, 900 + i * 36, 10);
					break;
				}
			}
		}
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		cursor.x = x;
		cursor.y = y;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		if (button == FRMouseButton::LEFT && !isReleased && getTickCount() >= player.shootingCooldown) {
			float distance = sqrt(pow((cursor.x - player.position.x), 2) + pow(windowHeight - cursor.y - player.position.y - aPlatform.height - view, 2));
			Projectile friendly = Projectile(player.getShootingPosition(), Point((cursor.x - player.position.x) / distance * 10, (windowHeight - cursor.y - player.position.y - aPlatform.height - view) / distance * 10), true, aFriendly);
			Projectiles.push_back(friendly);
			player.shootingCooldown = getTickCount() + 500;
		}
	}

	virtual void onKeyPressed(FRKey k) {
		if (k == FRKey::RIGHT) {
			player.facingDirection = RIGHT;
			player.isHoldingKey = true;
		}
		if (k == FRKey::LEFT) {
			player.facingDirection = LEFT;
			player.isHoldingKey = true;
		}
	}

	virtual void onKeyReleased(FRKey k) {
		if (k == FRKey::RIGHT && player.facingDirection == RIGHT) {
			player.isHoldingKey = false;
		}
		if (k == FRKey::LEFT && player.facingDirection == LEFT) {
			player.isHoldingKey = false;
		}
	}
	
	virtual const char* GetTitle() override
	{
		return "Arcanoid";
	}
};


int main(int argc, char *argv[])
{
	srand((unsigned)time(NULL));
	return run(new MyFramework);
}
