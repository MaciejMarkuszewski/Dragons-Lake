#include "Framework.h"
#include <iostream>
#include <string>
#include<cstdlib>
#include <cmath>
#include <deque>
using namespace std;


struct Point {

	int x, y;

	Point() = default;

	Point(int x, int y)
		: x(x), y(y)
	{
	}
};

struct Rectangle {

	Point L, R;

	Rectangle() = default;

	Rectangle(Point L, Point R)
		: R(R), L(L)
	{
	}
};

class Player {

public:

	Point Position;
	int Floor; // rename to isTouchingFloor, isStanding?
	int Plus; // ???
	int Points;
	bool FacingRight; // isFacingRight? or enum direction: left, right. Then var should be enum direction; or enum facingDirection; 
	bool HoldingKey; // isHoldingKey?
	int JumpVelocity;
	Rectangle Hitbox;
	Point Drawing; // u mean sprite? 

	Player() = default;

	Player(Point Position, int Floor, int Plus, int Points, bool FacingRight, bool HoldingKey, int JumpVelocity, Rectangle Hitbox, Point Drawing) 
		: Position(Position), Floor(Floor), Plus(Plus), Points(Points), FacingRight(FacingRight), HoldingKey(HoldingKey), JumpVelocity(JumpVelocity), Hitbox(Hitbox), Drawing(Drawing)
	{
	}
};

class Projectile {

public: // what about pressing tab here to make indentation? 

	Point Position;
	Point Target;
	bool Friendly; // isFriendly ? 

	Projectile() = default;

	Projectile(Point Position, Point Target, bool Friendly)
		: Position(Position), Target(Target), Friendly(Friendly)
	{
	}

};
class Asset {

public:

	Sprite* sprite;
	string path;
	int width;
	int height;

	void Load(string p) { // assetName? 
		path = "assets/" + p + ".png";
		sprite = createSprite(path.c_str());
		getSpriteSize(sprite, width, height);
	}

	Asset() = default;
};

// Returns true if two rectangles (l1, r1) and (l2, r2)
// overlap
bool doOverlap(Point l1, Point r1, Point l2, Point r2) // isOverlaping? 
{
	// if rectangle has area 0, no overlap
	if (l1.x == r1.x || l1.y == r1.y || r2.x == l2.x || l2.y == r2.y)
		return false;

	// If one rectangle is on left side of other
	if (l1.x > r2.x || l2.x > r1.x)
		return false;

	// If one rectangle is above other
	if (r1.y > l2.y || r2.y > l1.y)
		return false;

	return true;
}

void CreatePlatformRow(string* Column, int size, int platforms, int monsters) { //column lowercase or should it be uppercase cuz its a pointer? dunno, just asking
	bool failed = true;
	for (int i = 0; i < size; i++) {
		if (rand() % 100 >= platforms) {
			if (rand() % 100 >= monsters) {
				Column[i] = "2"; // Why u keep it as string if its a number? xD
			}
			else
			{

				Column[i] = "1";
			}
			failed = false;
		}
		else
		{
			Column[i] = "0";
		}
	}
	if (failed) {
		Column[rand() % 8] = "1";
	}
}


/* Test Framework realization */
class MyFramework : public Framework {

public:

	Asset GPlatform;
	Asset GPlayerL;
	Asset GPlayerR;
	Asset GPlayerJumpingL;
	Asset GPlayerJumpingR;
	Asset GBackground;
	Asset GScores;
	Asset GAdd;
	Asset GFriendly;
	Asset GMonster;
	Asset GDigits[10];
	deque <string*> Rows;
	Player player;
	int WindowWidth;
	int WindowHeight;
	int view = 0;
	int CursorX = 0;
	int CursorY = 0;
	deque <Projectile> Projectiles;
	deque <Point> Monsters;

	void reset() {
		
		Projectiles.clear();
		Monsters.clear();
		CreatePlatformRow(Rows[0], 10, 0, 100);
		for (int row = 1; row < 10; row++) {
			CreatePlatformRow(Rows[row], 10, 80, 80);
			for (int column = 0; column < 10; column++) {
				if (Rows[row][column] == "2") {
					Monsters.push_back(Point(column, row));
				}
			}
		}
		player = Player(Point(WindowWidth / 2, 0), 0, 0, 0, true, false, 0, Rectangle(Point(-GPlayerR.width / 4, GPlayerR.height * 2 / 3), Point(GPlayerR.width / 4, 0)), Point(-GPlayerR.width / 2, -GPlayerR.height));
	}

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = 1200;
		height = 800;
		fullscreen = false;
	}

	virtual bool Init() {
		getScreenSize(WindowWidth, WindowHeight);
		GPlatform.Load("Platform");
		GPlayerL.Load("PlayerL");
		GPlayerR.Load("PlayerR");
		GPlayerJumpingL.Load("PlayerJumpingL");
		GPlayerJumpingR.Load("PlayerJumpingR");
		GBackground.Load("Background");
		GScores.Load("Scores");
		GAdd.Load("Add");
		GFriendly.Load("Friendly");
		GMonster.Load("Monster");
		for (int i = 0; i < 10; i++) {
			GDigits[i].Load(to_string(i));
		}
		static string rows[10][10];
		for (int row = 0; row < 10; row++) {
			Rows.push_back(rows[row]);
		}
		reset();
		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick() {
		// you can make it work at the FPS rate you want
		// how? create timer, save time after you execute if below
		//run if not every 8 thics but when time from last succesfull tick is 1/60 of a second or smth
		// it should make this game more stable and i think this may be main issue with perf
		// also u can make to different ticks one for getting input and calculating phisics which shouldnt be related to performance and second, displaying sprites which can be related to perf

		// you have much loops running every 8 ticks, i immagine it may be some ridiculous number of ticks per second like 500 or 5k and we try to do everything here, 
		//i think we should consider moving something out of main loop
		
		// btw this funct is definetelly too long i looked at it, you should break it to smth like 7 functions

		drawSprite(GBackground.sprite, 0, 0); // do we need to draw it every time or iths there if u draw it once? what happens to old background? 
		if (getTickCount() % 8 == 0) {
			if (player.HoldingKey) {
				player.Position.x += player.FacingRight ? 10 : -10;
			}
			if (player.Position.x >= WindowWidth) {
				player.Position.x -= WindowWidth;
			}
			if (player.Position.x < 0) {
				player.Position.x += WindowWidth;
			}
			player.Position.y += player.JumpVelocity;
			player.JumpVelocity--;
			if (player.JumpVelocity > 0 && view > 0) {
				view -= player.JumpVelocity;
				if (view < 0) {
					view = 0;
					for (int i = 0; i < player.Plus; i++) {
						if ((player.Floor - player.Plus + Rows.size() - 1 + i) % 100 == 0) {
							CreatePlatformRow(Rows[0], 10, 0, 100);
						}
						else {
							CreatePlatformRow(Rows[0], 10, 80, 90);
						}
						for (int column = 0; column < 10; column++) {
							if (Rows[0][column] == "2") {
								Monsters.push_back(Point(column, Rows.size() - player.Plus + i));
							}
						}
						Rows.push_back(Rows[0]);
						Rows.pop_front();
					}
					player.Floor += player.Plus;
					player.Plus = 0;
				}
			}
			if (player.JumpVelocity < 0) {
				int row = (player.Position.y + GPlatform.height) / GPlatform.width;
				if (abs(player.Position.y - row * GPlatform.width + GPlatform.height / 2) <= GPlatform.height / 2 && (Rows[row][(player.Position.x + player.Hitbox.L.x) / GPlatform.width] != "0" || Rows[row][(player.Position.x + player.Hitbox.R.x) / GPlatform.width] != "0")) {
					player.JumpVelocity = 25;
					player.Plus = row;
					player.Points++;
					view = row * GPlatform.width;
					player.Position.y = 0;
					for (int i = 0; i < Projectiles.size(); i++) {
						Projectiles[i].Position.y -= view;
					}
					for (int i = 0; i < Monsters.size(); i++) {
						Monsters[i].y -= player.Plus;
						if (Monsters[i].y < 0) {
							Monsters.pop_front();
							i--;
						}
					}
				}
			}
			cout << Monsters.size() << endl;
			for (int i = Projectiles.size() -1; i >=0; i--) {
				Projectiles[i].Position.x += Projectiles[i].Target.x;
				Projectiles[i].Position.y += Projectiles[i].Target.y;
				if (Projectiles[i].Position.x >= WindowWidth) {
					Projectiles[i].Position.x -= WindowWidth;
				}
				if (Projectiles[i].Position.x < 0) {
					Projectiles[i].Position.x += WindowWidth;
				}
				if (abs(Projectiles[i].Position.y - WindowHeight / 2) > WindowHeight / 2 && view == 0) {
					Projectiles.erase(Projectiles.begin() + i);
					continue;
				}
				for (int j = 0; j < Monsters.size(); j++) {
					if (doOverlap(Point(Projectiles[i].Position.x - GFriendly.width/2, Projectiles[i].Position.y + GFriendly.height / 2), Point(Projectiles[i].Position.x + GFriendly.width / 2, Projectiles[i].Position.y - GFriendly.height / 2), Point(Monsters[j].x * GPlatform.width + (GPlatform.width - GMonster.width) / 2, GMonster.height + Monsters[j].y * GPlatform.width), Point((Monsters[j].x + 1) * GPlatform.width - (GPlatform.width - GMonster.width) / 2, Monsters[j].y * GPlatform.width))) {
						Rows[Monsters[j].y][Monsters[j].x] = "1";
						Monsters.erase(Monsters.begin() + j);
						Projectiles.erase(Projectiles.begin() + i);
						break;
					}
				}
			}
			for (int i = 0; i < Monsters.size(); i++) {
				if (doOverlap(Point(player.Position.x + player.Hitbox.L.x, player.Position.y + player.Hitbox.L.y), Point(player.Position.x + player.Hitbox.R.x, player.Position.y + player.Hitbox.R.y), Point(Monsters[i].x * GPlatform.width + (GPlatform.width - GMonster.width) / 2, GMonster.height + Monsters[i].y * GPlatform.width), Point((Monsters[i].x + 1) * GPlatform.width - (GPlatform.width - GMonster.width) / 2, Monsters[i].y * GPlatform.width))) {
					if (player.Position.y >= Monsters[i].y * GPlatform.width) {
						Rows[Monsters[i].y][Monsters[i].x] = "1";
						Monsters.erase(Monsters.begin() + i);
					}
					else {
						player.Position.y = -1;
					}
					break;
				}
			}
			if (player.Position.y < 0) {
				reset();
			}
		}
		for (int row = 0; row < Rows.size();row++)
		{
			for (int column = 0; column < 10; column++) {
				if (Rows[row][column] != "0") {
					drawSprite(GPlatform.sprite, column * GPlatform.width, WindowHeight - GPlatform.height - (row - player.Plus) * GPlatform.width - view);
					if (Rows[row][column] == "2") {
						drawSprite(GMonster.sprite, column * GPlatform.width + (GPlatform.width - GMonster.width) / 2, WindowHeight - GPlatform.height - GMonster.height - (row - player.Plus) * GPlatform.width - view);
					}
				}
			}
		}
		for (int i = 0; i < Projectiles.size(); i++) {
			if (Projectiles[i].Friendly) {
				drawSprite(GFriendly.sprite, Projectiles[i].Position.x - GFriendly.width/2, WindowHeight - Projectiles[i].Position.y - (GFriendly.height + GPlatform.height) / 2 - view);
			}
		}
		if (player.JumpVelocity > 0) {
			drawSprite(player.FacingRight ? GPlayerJumpingR.sprite : GPlayerJumpingL.sprite, player.Position.x + player.Drawing.x, WindowHeight - GPlatform.height  + player.Drawing.y - player.Position.y - view);
		}
		else {
			drawSprite(player.FacingRight ? GPlayerR.sprite : GPlayerL.sprite, player.Position.x + player.Drawing.x, WindowHeight - GPlatform.height + player.Drawing.y - player.Position.y - view);
		}
		drawSprite(GScores.sprite, 0, 0); // maybe we need to draw it only when it changed? 
		string floor = to_string(player.Floor);
		for (int i = 0; i < floor.length(); i++) {
			for (int j = 0; j < 10; j++) {
				if (int(floor[i]) - 48 == j) {
					drawSprite(GDigits[j].sprite, 100 + i * 36, 10);
					break;
				}
			}
		}
		if (player.Plus > 0) {
			drawSprite(GAdd.sprite, 100 + floor.length() * 36, 10);
			for (int j = 0; j < 10; j++) {
				if (player.Plus == j) {
					drawSprite(GDigits[j].sprite, 100 + (floor.length() + 1) * 36, 10);
					break;
				}
			}
		}
		floor = to_string(player.Points);
		for (int i = 0; i < floor.length(); i++) {
			for (int j = 0; j < 10; j++) {
				if (int(floor[i]) - 48 == j) {
					drawSprite(GDigits[j].sprite, 900 + i * 36, 10);
					break;
				}
			}
		}
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		CursorX = x;
		CursorY = y;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		if (button == FRMouseButton::LEFT && !isReleased) {
			float distance = sqrt(pow((CursorX - player.Position.x), 2) + pow(WindowHeight - CursorY - player.Position.y - GPlatform.height - view, 2));
			Projectile friendly = Projectile(Point(player.Position.x, player.Position.y - player.Drawing.y/2), Point((CursorX - player.Position.x) / distance * 10, (WindowHeight - CursorY - player.Position.y - GPlatform.height - view) / distance * 10), true);
			Projectiles.push_back(friendly);
		}
	}

	virtual void onKeyPressed(FRKey k) {
		if (k == FRKey::RIGHT) {
			player.FacingRight = true;
			player.HoldingKey = true;
		}
		if (k == FRKey::LEFT) {
			player.FacingRight = false;
			player.HoldingKey = true;
		}
	}

	virtual void onKeyReleased(FRKey k) {
		if (k == FRKey::RIGHT && player.FacingRight) {
			player.HoldingKey = false;
		}
		if (k == FRKey::LEFT && !player.FacingRight) {
			player.HoldingKey = false;
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
