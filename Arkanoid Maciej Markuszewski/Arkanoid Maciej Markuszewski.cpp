#include "Framework.h"
#include "Player.h"
#include "Projectile.h"
#include "Turret.h"
#include "Ability.h"
#include "Monster.h"
#include <iostream>
#include <string>
#include<cstdlib>
#include <cmath>
#include <deque>
using namespace std;

#define GAME_TICK 20 // in ms = 1000/FPS
#define THRESHOLD 1000
#define PLATFORM_DENSITY 750 // 20% empty platforms
#define MONSTERS_DENSITY 950 // 4,5% monsters
#define TURRETS_DENSITY 995 // 0,5% turrets
#define CHECKPOINT 100
#define BASE_ABILITY_COOLDOWN 15000 // in ms
#define ADDITIONAL_ABILITY_COOLDOWN 15000 // in ms
#define WINDOW_WIDTH 1200 // in pixels
#define WINDOW_HEIGHT 800 // in pixels
#define FULLSCREEN false
#define VERTICAL_SPEED 24 // in pixels. Player will jump up to (VERTICAL_SPEED * (VERTICAL_SPEED +1) / 2) pixels
#define HORIZONTAL_SPEED 10  // in pixels
#define PROJECTILE_SPEED 10 // in pixels
#define TURRET_COOLDOWN 1000 // in ms
#define PLAYER_COOLDOWN 500 // in ms
#define PLATFORMS_ON_SCREEN 6
#define COLUMNS 10
#define ROWS 10
#define AUTO_SHOOTER_DURATION 20000 // in ms
#define SCORES_VERTICAL_OFFSET 10 // in pixels
#define SCORES_HORIZONTAL_OFFSET 100 // in pixels
#define DIGITS 10
#define ASCII 48

/* Test Framework realization */
class MyFramework : public Framework {
public:

	//Assets
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
	Asset aDigits[DIGITS];

	//Variables that don't change value after Init()
	int windowWidth;
	int windowHeight;
	int tileSize;

	//Variables and objects
	Point cursor;
	Player player;
	int view = 0;
	unsigned int previousTick = 0;
	unsigned int nextAbility;

	//Object containers
	deque <int*> Platforms;
	deque <Projectile> Projectiles;
	deque <Monster> Monsters;
	deque <Turret> Turrets;
	deque <Ability> Abilities;

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = WINDOW_WIDTH;
		height = WINDOW_HEIGHT;
		fullscreen = FULLSCREEN;
	}

	virtual bool Init()
	{
		//Load Assets
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
		for (int i = 0; i < DIGITS; i++)
		{
			aDigits[i].load(to_string(i));
		}

		getScreenSize(windowWidth, windowHeight);
		tileSize = aPlatform.width;


		static int rows[ROWS][COLUMNS];
		for (int row = 0; row < ROWS; row++)
		{
			Platforms.push_back(rows[row]);
		}

		reset();

		return true;
	}

	//Resets game to starting state on Init() or after meeting game over conditions
	void reset()
	{
		Projectiles.clear();
		Monsters.clear();
		Turrets.clear();
		Abilities.clear();
		createPlatformRow(Platforms[0], 0, THRESHOLD, THRESHOLD); // First row has platform on every position. Do not contain monsters and turrets
		for (int row = 1; row < ROWS; row++)
		{
			createPlatformRow(Platforms[row], PLATFORM_DENSITY, THRESHOLD, THRESHOLD); // First ROWS rows do not contain monsters and turrets
		}
		player = Player(Point(windowWidth / 2, 0), 0, 0, 0, RIGHT, false, 0, NONE, 0, getTickCount(), aPlayerR);
		view = 0;
		nextAbility = getTickCount() + BASE_ABILITY_COOLDOWN + (rand() % ADDITIONAL_ABILITY_COOLDOWN);
	}

	//Creates platform row with average:
	//platforms / THRESHOLD empty spaces
	//(monsters - platforms) / THRESHOLD platforms
	//(turrets - monsters) / THRESHOLD monsters
	//(THRESHOLD - turrets) / THRESHOLD turrets
	void createPlatformRow(int* pColumn, int platforms, int monsters, int turrets)
	{
		bool failed = true;
		for (int i = 0; i < COLUMNS; i++) {
			int random = rand() % THRESHOLD;
			//no platform created
			if (random < platforms) {
				pColumn[i] = 0;
			}
			//empty platform created
			else if (random < monsters) {
				pColumn[i] = 1;
				failed = false;
			}
			//platform with monster created
			else if (random < turrets) {
				pColumn[i] = 2;
				failed = false;
			}
			//platform with turret created
			else {
				pColumn[i] = 3;
			}
		}
		//create platform if only platforms that were created had turret, so row is not empty/hard
		if (failed) {
			pColumn[rand() % COLUMNS] = 1;
		}
	}

	virtual void Close()
	{

	}

	virtual bool Tick()
	{
		if (getTickCount() >= previousTick + GAME_TICK) {
			checkHitboxes();
			player.updatePosition(HORIZONTAL_SPEED,windowWidth);
			updateView();
			updateProjectilesPosition();
			updateShooting();
			previousTick = getTickCount();
		}
		drawSprites();
		return false;
	}

	//Check hitboxes between:
	//Every ability pickup and player
	//Every friendly projectile with every monster
	//Every hostile projectile with player
	//Player with every monster
	//Player with bottom game border
	void checkHitboxes()
	{
		//Every ability pickup and player
		for (int i = 0; i < Abilities.size(); i++)
		{
			if (player.getHitbox().doOverlap(Abilities[i].getHitbox(tileSize, player.jumpedBy)))
			{
				player.currentAbility = AUTOSHOOTER;
				player.abilityDuration = getTickCount() + AUTO_SHOOTER_DURATION;
				Platforms[Abilities[i].tile.y][Abilities[i].tile.x] = 1;
				Abilities.erase(Abilities.begin() + i);
			}
		}
		for (int i = Projectiles.size() - 1; i >= 0; i--)
		{
			//Every friendly projectile with every monster
			if (Projectiles[i].isFriendly)
			{
				for (int j = 0; j < Monsters.size(); j++)
				{
					if (Projectiles[i].getHitbox().doOverlap(Monsters[j].getHitbox(tileSize, player.jumpedBy)))
					{
						Platforms[Monsters[j].tile.y][Monsters[j].tile.x] = 1;
						Monsters.erase(Monsters.begin() + j);
						Projectiles.erase(Projectiles.begin() + i);
						break;
					}
				}
			}
			//Every hostile projectile with player
			else
			{
				//Game over conditions
				if (player.getHitbox().doOverlap(Projectiles[i].getHitbox()))
				{
					reset();
					return;
				}
			}
		}
		//Player with every monster
		for (int i = 0; i < Monsters.size(); i++)
		{
			if (player.getHitbox().doOverlap(Monsters[i].getHitbox(tileSize, player.jumpedBy)))
			{
				if (player.position.y >= Monsters[i].tile.y * tileSize && player.jumpVelocity <= 0)
				{
					Platforms[Monsters[i].tile.y][Monsters[i].tile.x] = 1;
					Monsters.erase(Monsters.begin() + i);
				}
				//Game over conditions
				else
				{
					reset();
					return;
				}
			}
		}
		//Player with bottom game border. Game over conditions
		if (player.position.y < 0)
		{
			reset();
		}
	}

	//Updates offset between camera view and objects
	void updateView()
	{
		if (player.jumpVelocity > 0 && view > 0) {
			view -= player.jumpVelocity;
			//Delete bottom player.jumpedBy rows and all objects on them and create new ones on top
			if (view <= 0) {
				view = 0;
				for (int i = Monsters.size() - 1; i >= 0; i--)
				{
					Monsters[i].tile.y -= player.jumpedBy;
					if (Monsters[i].tile.y < 0)
					{
						Monsters.pop_back();
					}
				}
				for (int i = Turrets.size() - 1; i >= 0; i--)
				{
					Turrets[i].tile.y -= player.jumpedBy;
					if (Turrets[i].tile.y < 0)
					{
						Turrets.pop_back();
					}
				}
				for (int i = Abilities.size() - 1; i >= 0; i--)
				{
					Abilities[i].tile.y -= player.jumpedBy;
					if (Abilities[i].tile.y < 0)
					{
						Abilities.pop_back();
					}
				}
				for (int i = 0; i < player.jumpedBy; i++)
				{
					//Every CHECKPOINT floor has platform on every position. Do not contain monsters and turrets
					if ((player.highestFloor + Platforms.size() + i) % CHECKPOINT == 0)
					{
						createPlatformRow(Platforms[0], 0, THRESHOLD, THRESHOLD);
					}
					else
					{
						createPlatformRow(Platforms[0], PLATFORM_DENSITY, MONSTERS_DENSITY, TURRETS_DENSITY);
					}
					for (int column = 0; column < COLUMNS; column++)
					{
						if (Platforms[0][column] == 2)
						{
							Monsters.push_front(Monster(Point(column, Platforms.size() - player.jumpedBy + i), aMonster));
						}
						else if (Platforms[0][column] == 3)
						{
							Turrets.push_front(Turret(Point(column, Platforms.size() - player.jumpedBy + i), aTurret, 0));
						}
						//Add special ability on empty platform slot
						else if (Platforms[0][column] == 1 && getTickCount() >= nextAbility && nextAbility % COLUMNS == column)
						{
							Platforms[0][column] = 4;
							Abilities.push_front(Ability(Point(column, Platforms.size() - player.jumpedBy + i), aAutoShooter, AUTOSHOOTER));
							nextAbility = getTickCount() + BASE_ABILITY_COOLDOWN + (rand() % ADDITIONAL_ABILITY_COOLDOWN);
						}
					}
					Platforms.push_back(Platforms[0]);
					Platforms.pop_front();
				}
				player.highestFloor += player.jumpedBy;
				player.jumpedBy = 0;
			}
		}
		//Move camera view and projectiles
		if (player.jumpVelocity < 0)
		{
			int row = (player.position.y + aPlatform.height) / tileSize;
			if (abs(player.position.y - row * tileSize + aPlatform.height / 2) <= aPlatform.height / 2 && (Platforms[row][player.getHitbox().l.x / tileSize] != 0 || Platforms[row][player.getHitbox().r.x / tileSize] != 0))
			{
				player.jumpVelocity = VERTICAL_SPEED;
				player.jumpedBy = row;
				if ((Platforms[row][player.getHitbox().l.x / tileSize] != 0 && Platforms[row][player.getHitbox().r.x / tileSize] != 0) && player.getHitbox().l.x / tileSize != player.getHitbox().r.x / tileSize) {
					player.platformsTouchedCount++;
				}
				player.platformsTouchedCount++;
				view = row * tileSize;
				player.position.y = 0;
				for (int i = 0; i < Projectiles.size(); i++)
				{
					Projectiles[i].position.y -= player.jumpedBy * tileSize;
				}
			}
		}
	}

	//updates turrets and player special ability to automatically shoot when not on cooldown
	void updateShooting()
	{
		for (int i = Turrets.size() - 1; i >= 0; i--)
		{
			if (Turrets[i].tile.y < PLATFORMS_ON_SCREEN)
			{
				if (getTickCount() >= Turrets[i].shootingCooldown)
				{
					float distance = sqrt(pow((player.getShootingPosition().x - Turrets[i].getShootingPosition(tileSize).x), 2) + pow(player.getShootingPosition().y - Turrets[i].getShootingPosition(tileSize).y, 2));
					Projectile hostile = Projectile(Turrets[i].getShootingPosition(tileSize), Point((player.getShootingPosition().x - Turrets[i].getShootingPosition(tileSize).x) / distance * PROJECTILE_SPEED, (player.getShootingPosition().y - Turrets[i].getShootingPosition(tileSize).y) / distance * PROJECTILE_SPEED), false, aHostile);
					Projectiles.push_back(hostile);
					Turrets[i].shootingCooldown = getTickCount() + TURRET_COOLDOWN;
				}
			}
			else
			{
				break;
			}
		}
		if (player.currentAbility == AUTOSHOOTER && getTickCount() >= player.shootingCooldown)
		{
			for (int i = Monsters.size() - 1; i >= 0; i--)
			{
				if (Monsters[i].tile.y < PLATFORMS_ON_SCREEN)
				{
					float distance = sqrt(pow((Monsters[i].getShootingPosition(tileSize).x - player.getShootingPosition().x), 2) + pow(Monsters[i].getShootingPosition(tileSize).y - player.getShootingPosition().y, 2));
					Projectile friendly = Projectile(player.getShootingPosition(), Point((Monsters[i].getShootingPosition(tileSize).x - player.getShootingPosition().x) / distance * PROJECTILE_SPEED, (Monsters[i].getShootingPosition(tileSize).y - player.getShootingPosition().y) / distance * PROJECTILE_SPEED), true, aFriendly);
					Projectiles.push_back(friendly);
				}
				else
				{
					break;
				}
			}
			if (getTickCount() >= player.abilityDuration)
			{
				player.currentAbility = NONE;
				player.abilityDuration = 0;
			}
			player.shootingCooldown = getTickCount() + PLAYER_COOLDOWN;
		}
	}

	//Updates position of every projectile. Erases projectile outside the screen
	void updateProjectilesPosition()
	{
		for (int i = Projectiles.size() - 1; i >= 0; i--)
		{
			Projectiles[i].updatePosition(windowWidth);
			if (abs(Projectiles[i].position.y - windowHeight / 2) > windowHeight / 2 && view == 0)
			{
				Projectiles.erase(Projectiles.begin() + i);
				continue;
			}
		}
	}

	//Draws Sprites in order:
	//Background
	//Platforms
	//Objects on platforms
	//Projectiles
	//Player
	//Scoreboard
	//Scores
	//Active ability
	void drawSprites()
	{
		//Background
		drawSprite(aBackground.pSprite, 0, 0);
		//Platforms
		for (int row = 0; row < Platforms.size(); row++)
		{
			for (int column = 0; column < COLUMNS; column++)
			{
				if (Platforms[row][column] != 0)
				{
					drawSprite(aPlatform.pSprite, column * tileSize, windowHeight - aPlatform.height - (row - player.jumpedBy) * tileSize - view);
					//Objects on platforms
					if (Platforms[row][column] > 1)
					{
						StationaryObject object;
						switch (Platforms[row][column])
						{
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
		//Projectiles
		for (int i = 0; i < Projectiles.size(); i++)
		{
			drawSprite(Projectiles[i].isFriendly ? aFriendly.pSprite : aHostile.pSprite, Projectiles[i].getSpritePosition(aPlatform, windowHeight, view).x, Projectiles[i].getSpritePosition(aPlatform, windowHeight, view).y);
		}
		//Player
		if (player.jumpVelocity > 0 && player.jumpVelocity != VERTICAL_SPEED)
		{
			drawSprite(player.facingDirection == RIGHT ? aPlayerJumpingR.pSprite : aPlayerJumpingL.pSprite, player.getSpritePosition(aPlatform, windowHeight, view).x, player.getSpritePosition(aPlatform, windowHeight, view).y);
		}
		else
		{
			drawSprite(player.facingDirection == RIGHT ? aPlayerR.pSprite : aPlayerL.pSprite, player.getSpritePosition(aPlatform, windowHeight, view).x, player.getSpritePosition(aPlatform, windowHeight, view).y);
		}
		//Scoreboard - I wasn't sure how to interpret: "You should show both the distance the player passed and the number of platforms. Distance might be measured in pixels or abstract units.", so I coded it this way:
		drawSprite(aScores.pSprite, 0, 0);
		//Distance in (player.highestFloor + player.jumpedBy). If multiplied by tileSize = distance in pixels
		string floor = to_string(player.highestFloor);
		for (int i = 0; i < floor.length(); i++)
		{
			for (int j = 0; j < DIGITS; j++)
			{
				if (int(floor[i]) - ASCII == j)
				{
					drawSprite(aDigits[j].pSprite, SCORES_HORIZONTAL_OFFSET + i * aDigits[i].width, SCORES_VERTICAL_OFFSET);
					break;
				}
			}
		}
		if (player.jumpedBy > 0)
		{
			drawSprite(aAdd.pSprite, SCORES_HORIZONTAL_OFFSET + floor.length() * aAdd.width, SCORES_VERTICAL_OFFSET);
			for (int j = 0; j < DIGITS; j++)
			{
				if (player.jumpedBy == j)
				{
					drawSprite(aDigits[j].pSprite, SCORES_HORIZONTAL_OFFSET + (floor.length() + 1) * aDigits[j].width, SCORES_VERTICAL_OFFSET);
					break;
				}
			}
		}
		//Number of platforms player jumped on
		string points = to_string(player.platformsTouchedCount);
		for (int i = 0; i < points.length(); i++)
		{
			for (int j = 0; j < DIGITS; j++)
			{
				if (int(points[i]) - ASCII == j)
				{
					drawSprite(aDigits[j].pSprite, windowWidth - SCORES_HORIZONTAL_OFFSET - (points.length() - i) * aDigits[i].width, SCORES_VERTICAL_OFFSET);
					break;
				}
			}
		}
		//Active ability
		if (player.currentAbility == AUTOSHOOTER) {
			drawSprite(aAutoShooter.pSprite, windowWidth / 2 - aAutoShooter.width / 2, 0);
		}
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{
		cursor.x = x;
		cursor.y = y;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased)
	{
		if (button == FRMouseButton::LEFT && !isReleased && getTickCount() >= player.shootingCooldown)
		{
			float distance = sqrt(pow((cursor.x - player.position.x), 2) + pow(windowHeight - cursor.y - aPlatform.height - view - player.getShootingPosition().y, 2));
			Projectile friendly = Projectile(player.getShootingPosition(), Point((cursor.x - player.position.x) / distance * PROJECTILE_SPEED, (windowHeight - cursor.y - aPlatform.height - view - player.getShootingPosition().y) / distance * PROJECTILE_SPEED), true, aFriendly);
			Projectiles.push_back(friendly);
			player.shootingCooldown = getTickCount() + PLAYER_COOLDOWN;
		}
	}

	virtual void onKeyPressed(FRKey k)
	{
		if (k == FRKey::RIGHT) {
			player.facingDirection = RIGHT;
			player.isHoldingKey = true;
		}
		if (k == FRKey::LEFT) {
			player.facingDirection = LEFT;
			player.isHoldingKey = true;
		}
	}

	virtual void onKeyReleased(FRKey k)
	{
		if (k == FRKey::RIGHT && player.facingDirection == RIGHT)
		{
			player.isHoldingKey = false;
		}
		if (k == FRKey::LEFT && player.facingDirection == LEFT)
		{
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
