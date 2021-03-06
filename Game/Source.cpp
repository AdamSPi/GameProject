// Source.cpp  
// Read readme.txt for  controls and instructions
// Unresolved issues: 10% of the time starting the game will return an error trying again will fix and some collision detection bugs
// I tried creating separate classes for entity but the window drawing wouldn't work so had to settle with globals

#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>  
#include <thread> 
#include <atomic> 
#include <list>
#include <string>
#include <atlbase.h>
#include <vector>
#include <time.h>
// Global variables  

enum Dir
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

// The main window class name.  
static TCHAR szWindowClass[] = _T("win32app");

// The string that appears in the application's title bar.  
static TCHAR szTitle[] = _T("Super Tofu Boy");

HINSTANCE hInst;
DWORD FIXED_SIZED_WINDOW = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

// Window dimensions
int WIDTH = 1000;
int HEIGHT = 800;

// Forward declarations of functions included in this code module:  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Returns true when Player is landed on anything that is collidable
BOOL LandCheck();
// Returns true when Player is against anything that is collidable
BOOL WallCheckLeft();
BOOL WallCheckRight();
BOOL WallCheckTop();

// Moves the entire world to keep the Player centered on the screen
void CameraMove(Dir d);

// Coin  rectangles with boolean flags
struct coin
{
	BOOL taken = false;
	BOOL render = true;
	RECT * body;
};
// List of collidables objects
std::vector<RECT*> entities;
std::vector<coin*> coins;

// Animation modifiers
int Squish = 0; // short and fat (used when landing or maximum jump height)
int Squash = 0; // tall and skinny (used when in mid-jump or fall)
int Squanch = 0;// walking animation
int frames = 0;

// Entities
static RECT Player;

static RECT Ground;

static RECT Wall1;
static RECT Wall2;
static RECT Wall3;
static RECT Wall4;
static RECT Wall5;
static RECT Wall6;
static RECT Wall7;
static RECT Wall9;

static RECT Land1;
static RECT Land2;
static RECT Land3;
static RECT Land4;
static RECT Land5;
static RECT Land6;
static RECT Land7;
static RECT Land8;

static RECT Vanity1;

static RECT Platform1;
static RECT Platform2;
static RECT Platform3;

static RECT Win;
// Pointer
static RECT * Pointy;

// Physics flags
BOOL collision = false;
BOOL fall = true;
BOOL jump = false;
BOOL glide = false;

// Movement flags
BOOL right = false;
BOOL left = false;
BOOL Camera = false;

// Movement speed flags
BOOL fast;
BOOL slow;
BOOL slower;
int toggle = -1;

BOOL win = false;

int Score = 0;

static RECT Coin1;
static RECT Coin2;
static RECT Coin3;
static RECT Coin4;
static RECT Coin5;

coin coin1;
coin coin2;
coin coin3;
coin coin4;
coin coin5;

time_t start;
static RECT timer;
static RECT score;

BOOL run;

void Spawn()
{
	// Set all the rect's dimensions
	Player.left = 482;
	Player.top = -135;
	Player.bottom = -100;
	Player.right = 518;

	Ground.left = 0;
	Ground.top = HEIGHT/2;
	Ground.right = WIDTH;
	Ground.bottom = Ground.top+200;

	Wall1.left = Ground.left;
	Wall1.top = Ground.top - 250;
	Wall1.right = Wall1.left + 100;
	Wall1.bottom = Ground.top + 1;

	Wall2.left = Wall1.left + 225;
	Wall2.top = Wall1.top - 250;
	Wall2.right = Wall2.left + 100;
	Wall2.bottom = Wall1.top+100;

	Wall3.left = Ground.right - 100;
	Wall3.top = Ground.top - 300;
	Wall3.right = Ground.right;
	Wall3.bottom = Ground.top + 1;
	
	Wall4.left = -100;
	Wall4.top = Wall1.top - 200;
	Wall4.right = 0;
	Wall4.bottom = (HEIGHT/2)+100;

	Wall5.left = -800;
	Wall5.top = Land2.top - 750;
	Wall5.right = Wall5.left + 100;
	Wall5.bottom = Wall4.top+1;

	Wall6.left = Wall5.right + 200;
	Wall6.top = Wall5.top-300;
	Wall6.right = Wall6.left + 100;
	Wall6.bottom = Wall4.top -150;

	Land1.left = Ground.right-1;
	Land1.top = Ground.top;
	Land1.right = Ground.right+100;
	Land1.bottom = Ground.bottom;

	Land2.left = -800;
	Land2.top = Wall4.top;
	Land2.right = Wall4.right;
	Land2.bottom = Land2.top +100;

	Land3.left = Wall3.right-1;
	Land3.top = Wall3.top;
	Land3.right = 1300;
	Land3.bottom = Land3.top + 81;

	Land4.left = Land1.right - 1;
	Land4.top = Land1.top;
	Land4.right = 1700;
	Land4.bottom = Land1.top + 100;

	Land5.left = Wall6.right-1;
	Land5.top = Wall6.top;
	Land5.right = Land5.left+500;
	Land5.bottom = Land5.top + 100;

	Land6.left = Wall5.left-300;
	Land6.top = Wall5.top;
	Land6.right = Wall5.left+1;
	Land6.bottom = Land6.top + 100;

	Wall7.left = Land6.left - 100;
	Wall7.top = Land6.top - 800;
	Wall7.right = Land6.left;
	Wall7.bottom = Land6.top + 1;

	Vanity1.top = Land4.bottom-1;
	Vanity1.left = Land1.right - 1;
	Vanity1.right = Vanity1.left + 100;
	Vanity1.bottom = Vanity1.top + 150;

	Platform1.top = Land3.bottom - 1;
	Platform1.left = Land3.right - 1;
	Platform1.right = Platform1.left + 80;
	Platform1.bottom = Platform1.top + 80;

	Platform2.top = Land5.top+10;
	Platform2.left = Land5.right + 200;
	Platform2.right = Platform2.left + 250;
	Platform2.bottom = Platform2.top + 90;

	Platform3.top = Land5.top+10;
	Platform3.left = Platform2.right + 250;
	Platform3.right = Platform3.left + 250;
	Platform3.bottom = Platform3.top + 90;

	Land7.left = Platform3.right + 200;
	Land7.top = Land5.top;
	Land7.right = Land7.left + 600;
	Land7.bottom = Land7.top + 100;

	Wall9.left = Land7.right - 100;
	Wall9.top = Land7.top-500;
	Wall9.right = Land7.right;
	Wall9.bottom = Land7.top + 1;

	Coin1.left = 38;
	Coin1.top = Wall1.top - 50;
	Coin1.right = 63;
	Coin1.bottom = Coin1.top + 25;

	Coin2.left = Wall3.right+50;
	Coin2.top = Land1.top - 50;
	Coin2.right = Coin2.left +25;
	Coin2.bottom = Coin2.top + 25;

	Coin3.left = Wall5.right + 50;
	Coin3.top = Land2.top - 50;
	Coin3.right = Coin3.left + 25;
	Coin3.bottom = Coin3.top + 25;

	Coin4.left = Platform2.left + 162;
	Coin4.top = Platform2.top - 50;
	Coin4.right = Coin4.left + 25;
	Coin4.bottom = Coin4.top + 25;

	Coin5.left = Land7.left + 238;
	Coin5.top = Land7.top - 50;
	Coin5.right = Coin5.left + 25;
	Coin5.bottom = Coin5.top + 25;
	
	Win.left = 480;
	Win.right = 520;
	Win.top = -200;
	Win.bottom = -185;

	timer.left = 0;
	timer.right = 29;
	timer.top = 0;
	timer.bottom = 15;

	score.left = 0;
	score.right = 8;
	score.top = timer.bottom+1;
	score.bottom = score.top + 14;

}



//***********************************************************************************************************\\

void animation(HWND hWnd)
{
	start = time(0);
	while (run)
	{
		if (Camera)
		{
		// Jump animation
		if (jump && collision && !((left && WallCheckLeft()) || (right && WallCheckRight())))
		{
			while (frames < 100 && !((left && WallCheckLeft()) || (right && WallCheckRight())))
			{
				Squish = 0;
				while (Squish <= 6 && frames < 1 && !((left && WallCheckLeft()) || (right && WallCheckRight()))) // anticipation animation
				{
					Squish++;
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(7);
				}
				while (Squish != -1 && frames < 1 && !((left && WallCheckLeft()) || (right && WallCheckRight())))
				{
					Squish--;
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(6);
				}
				Squish = 0;
				if (!((left && WallCheckLeft()) || (right && WallCheckRight())) && !WallCheckTop()) // check so that player doesn't hit anyhthing
				{
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(1);
					CameraMove(DOWN);

					if (frames <= 50) // Sleep time will get slower as the cycles go through
					{
						if (Squash < 8)
						{
							Squash++;
							frames++;
							CameraMove(DOWN);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(1);
							continue;
						}
						frames++;
						CameraMove(DOWN);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(1);
						continue;
					}
					if (frames <= 80 && frames > 50) {
						while (Squash > 5)
						{
							collision = false;
							Squash--;
							CameraMove(DOWN);
							frames++;
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(6);
						}
						CameraMove(DOWN);
						frames++;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(2);
					}
					else if (frames <= 100 && frames > 80) {
						glide = true; Sleep(4); while (Squash > 3)
						{
							Squash--; InvalidateRect(hWnd, NULL, FALSE); Sleep(7); CameraMove(DOWN);
						}frames++;
					}
				}
				else break;
			}
			while (WallCheckTop())
			{
				Squash--; InvalidateRect(hWnd, NULL, FALSE); Sleep(3); CameraMove(DOWN);
				while (Squash != 0)
				{
					if (Squash > 0)Squash--;
					else Squash++;
					Squish++;
					CameraMove(DOWN);
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(10);
				}
				while (Squish != 0)
				{
					if (Squish > 0) Squish--;
					else Squish++;
					CameraMove(UP);
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(10);
				}
				collision = false;
				break;
			}
			while ((left && WallCheckLeft()) || (right && WallCheckRight()))
			{
				Squash--; InvalidateRect(hWnd, NULL, FALSE); Sleep(3); CameraMove(DOWN);
				while (Squash != 0)
				{
					if (Squash > 0)Squash--;
					else Squash++;
					Squish++;
					CameraMove(DOWN);
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(10);
				}
				while (Squish != 0)
				{
					if (Squish > 0) Squish--;
					else Squish++;
					CameraMove(UP);
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(10);
				}
				collision = false;
				break;
			}
			frames = 0; // reset
			// resetting body modifiers when top is reached
			while (Squash != 0)
			{
				Squash--;
				CameraMove(DOWN);
				InvalidateRect(hWnd, NULL, FALSE);
				Sleep(7);
			}
			while (Squish < 6 && !(((left && WallCheckLeft()) || (right && WallCheckRight()))))
			{
				Squish++;
				InvalidateRect(hWnd, NULL, FALSE);
				Sleep(7);
			}
			jump = false;
			fall = true;
		}

		//**************************************************************************************************************************\\

		// Falling condition
		if (fall)
		{
			// Wall jumping
			if ((left || right))
			{
				
				if (left && WallCheckLeft() && !LandCheck())
				{
					Squanch = 0;
					frames = 0;
					slow = false;
					slower = false;
					collision = true;
					while (!LandCheck() && left && !jump)
					{
						while (Squash != 4 && !jump)
						{
							if (Squish != 0)
							{
								if(Squish>0)Squish--;
								else Squish++;
							}
							if(Squash<4)Squash++;
							else Squash--;
							InvalidateRect(hWnd, NULL, FALSE);
							collision = true;
							fall = false;
							CameraMove(UP);
							Sleep(10);
						}
						fall = false;
						CameraMove(UP);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(15);
						if (!WallCheckLeft()) { 
							Squash = 0;
							collision = false;
							fall = true; 
							break; }
					}
					while (Squash != 0)
					{
						Squash--;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(10);
					}
					if (!jump)
					{
						Squash = 0;
						collision = false;
						fall = true;
					}

					// holding button will not allow player to wall jump so 
					// setting it to false with jump without having to let go
					if (jump)
					{
						left = false;
					}
				}
				if (right && WallCheckRight() && !LandCheck())
				{
					Squanch = 0;
					frames = 0;
					slow = false;
					slower = false;
					collision = true;
					while (!LandCheck() && right && !jump)
					{
						while (Squash != 4 && !jump)
						{
							if (Squish != 0)
							{
								if (Squish>0)Squish--;
								else Squish++;
							}
							if (Squash<4)Squash++;
							else Squash--;
							InvalidateRect(hWnd, NULL, FALSE);
							collision = true;
							fall = false;
							CameraMove(UP);
							Sleep(10);
						}
						
						fall = false;
						CameraMove(UP);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(15);
						if (!WallCheckRight()) {
							Squash = 0;
							collision = false;
							fall = true; 
							break; }
					}
					while (Squash != 0)
					{
						Squash--;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(1);
					}
					if (!jump)
					{
						Squash = 0;
						collision = false;
						fall = true;
					}
					// holding button will not allow player to wall jump so 
					// setting it to false with jump without having to let go
					if (jump)
					{
						right = false;
					}
				}
			}

			//-----------------------------------------------------------------------------\\
			// Jump animation in case player is jump off a wall
			if (jump && collision && !((left && WallCheckLeft()) || (right && WallCheckRight())))
			{
				while (frames < 100 && !((left && WallCheckLeft()) || (right && WallCheckRight())))
				{
					Squish = 0;
					while (Squish <= 6 && frames < 1 && !((left && WallCheckLeft()) || (right && WallCheckRight())))
					{
						Squish++;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(7);
					}
					while (Squish != -1 && frames < 1 && !((left && WallCheckLeft()) || (right && WallCheckRight())))
					{
						Squish--;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(6);
					}
					if ( !((left && WallCheckLeft()) || (right && WallCheckRight())) && !WallCheckTop())
					{
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(1);
						CameraMove(DOWN);

						if (frames <= 50)
						{
							collision = false;
							if (Squash < 8)
							{
								Squash++;
								frames++;
								CameraMove(DOWN);
								InvalidateRect(hWnd, NULL, FALSE);
								Sleep(1);
								continue;
							}
							frames++;
							CameraMove(DOWN);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(1);
							continue;
						}
						if (frames <= 80 && frames > 50) {
							while (Squash > 5)
							{
								collision = false;
								Squash--;
								CameraMove(DOWN);
								frames++;
								InvalidateRect(hWnd, NULL, FALSE);
								Sleep(6);
							}
							CameraMove(DOWN);
							frames++;
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(2);
						}
						else if (frames <= 100 && frames > 80) {
							glide = true; Sleep(4); while (Squash > 3)
							{
								Squash--; InvalidateRect(hWnd, NULL, FALSE); Sleep(7); CameraMove(DOWN);
							}frames++;
						}
					}
					else break;
				}
				while (WallCheckTop() && !(WallCheckLeft() || WallCheckRight()))
				{
					Squash--; InvalidateRect(hWnd, NULL, FALSE); Sleep(3); CameraMove(DOWN);
					while (Squash != 0)
					{
						if (Squash > 0)Squash--;
						else Squash++;
						Squish++;
						CameraMove(DOWN);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(10);
					}
					while (Squish != 0)
					{
						if (Squish > 0) Squish--;
						else Squish++;
						CameraMove(UP);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(10);
					}
					collision = false;
					break;
				}
				while (((left && WallCheckLeft()) || (right && WallCheckRight())))
				{
					Squash--; InvalidateRect(hWnd, NULL, FALSE); Sleep(3); CameraMove(DOWN);
					while (Squash != 0)
					{
						if (Squash > 0)Squash--;
						else Squash++;
						Squish++;
						CameraMove(DOWN);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(10);
					}
					while (Squish != 0)
					{
						if (Squish > 0) Squish--;
						else Squish++;
						CameraMove(UP);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(10);
					}
					collision = false;
					break;
				}
				frames = 0; // reset
				while (Squash != 0)
				{
					Squash--;
					CameraMove(DOWN);
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(7);
				}
				while (Squish < 6 && !(((left && WallCheckLeft()) || (right && WallCheckRight()))))
				{
					Squish++;
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(7);
				}
				jump = false;
				fall = true;
			}

			//-----------------------------------------------------------------------------\\
			// Falling animation
			while (!jump && fall && !((left && WallCheckLeft())||(right && WallCheckRight())))
			{
				if (WallCheckLeft() || WallCheckRight())
				{
					collision = true;
				}
				else
				{
					collision = false;
				}
				while (Squash < 4 && !jump)
				{
					if (Squish != 0) { Squish--; Sleep(10); }
					Squash++;
					frames++;
					CameraMove(UP);
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(10);
					if (frames < 20) { Sleep(7); }
				}
				if (jump)
				{
					break;
				}
				// Check if player has landed
				if (LandCheck())
				{
					fall = false;
					collision = true;
					slow = false;
					slower = false;
					toggle = -1;
					frames = 0;
					int offset = 0;
					while (Squash != 0)
					{
						Squash--;
						Squish++;
						CameraMove(DOWN);
						offset++;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(7);
					}
					while (Squish != 0)
					{
						Squish--;
						CameraMove(UP);
						offset--;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(5);
					}
					while (offset != 0)
					{
						CameraMove(DOWN);
						offset++;
					}
					Player.bottom = Pointy->top;
					Player.top = Player.bottom - 36;
					Player.left = Player.right - 36;
					Player.right = Player.left + 36;
					break;
				}
				frames++;
				CameraMove(UP);
				InvalidateRect(hWnd, NULL, FALSE);
				if (frames < 20) Sleep(3);
				else if (frames < 70 && frames >= 20) {
					glide = false;
					Sleep(2);
					while (Squash < 7) {
						Squash++;
						frames++;
						CameraMove(UP);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(6);
					}
				}
				else  if (frames < 200 && frames >= 70)
				{
					Sleep(1);
					if (frames >= 190) {
						slow = true;
					}
					while (Squash < 8)
					{
						Squash++;
						frames++;
						CameraMove(UP);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(5);
					}
				}
				else if (frames <= 400 && frames >= 200)
				{
					Sleep(1);
					if (frames >= 300)
					{
						slower = true;
						slow = false;
					}
					while (Squash <= 9)
					{
						Squash++;
						frames++;
						CameraMove(UP);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(1);
					}
				}
				else
				{
					if (Ground.bottom > 500)
					{
						Sleep(1); frames++;
					}
					else if (Player.top < 1000)
					{
						while ((Player.top < 1000))
						{
							Player.top++;
							Player.bottom++;
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(1);
						}
						Spawn();
						Camera = false;
						break;
					}
				}
			}
		}

		//**************************************************************************************************************************\\

		// Walk animation
		if ((left || right) && !jump && !(WallCheckRight() || WallCheckLeft()) && !fall)
		{
			while (Squanch < 5 && !jump && !(WallCheckRight() || WallCheckLeft()) && !fall)
			{
				Squanch++;
				fast = true;
				InvalidateRect(hWnd, NULL, FALSE);
				Sleep(30);
			}
			while (Squanch >= 0 && !jump && !(WallCheckRight() || WallCheckLeft()) && !fall)
			{
				fast = false;
				Squanch--;
				InvalidateRect(hWnd, NULL, FALSE);
				Sleep(30);
			}
		}
		else { while (Squanch != -1) { Squanch--; InvalidateRect(hWnd, NULL, FALSE); Sleep(30); } }

		}
		// respawn animation (doesn't follow player) 
		else
		{
			while (!collision)
			{
				while (Squash < 4)
				{
					if (Squish != 0) { Squish--; Sleep(10); }
					Squash++;
					frames++;
					Player.top++;
					Player.bottom++;
					InvalidateRect(hWnd, NULL, FALSE);
					if (frames < 20) { Sleep(5); }

				}
				if (LandCheck())
				{
					fall = false;
					collision = true;
					slow = false;
					slower = false;
					toggle = -1;
					frames = 0;
					int offset = 0;
					while (Squash != 0)
					{
						Squash--;
						Squish++;
						CameraMove(DOWN);
						offset++;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(5);
					}
					while (Squish != 0)
					{
						Squish--;
						CameraMove(UP);
						offset--;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(5);
					}
					while (offset != 0)
					{
						CameraMove(DOWN);
						offset++;
					}
					Player.bottom = Pointy->top;
					Player.top = Player.bottom - 36;
					Player.left = Player.right - 36;
					Player.right = Player.left + 36;
					Camera = true;
					break;
				}
				frames++;
				Player.top++;
				Player.bottom++;
				InvalidateRect(hWnd, NULL, FALSE);
				if (frames < 20) Sleep(3);
				else if (frames < 70 && frames >= 20) {
					glide = false;
					Sleep(2);
					while (Squash < 7) {
						Squash++;
						frames++;
						Player.top++;
						Player.bottom++;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(2);
					}
				}
				else  if (frames < 200 && frames >= 70)
				{
					Sleep(1);
					if (frames >= 190) {
						slow = true;
					}
					while (Squash < 8)
					{
						Squash++;
						frames++;
						Player.top++;
						Player.bottom++;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(1);
					}
				}
				else if (frames <= 400 && frames >= 200)
				{
					Sleep(1);
					if (frames >= 300)
					{
						slower = true;
						slow = false;
					}
					while (Squash <= 9)
					{
						Squash++;
						frames++;
						Player.top++;
						Player.bottom++;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(1);
					}
				}
				else Sleep(1); frames++;
			}
		}

	}// end game loop
}

void movement(HWND hWnd)
{
	// add entites and coins to vectors
	entities.push_back(&Ground);

	entities.push_back(&Wall1);
	entities.push_back(&Wall2);
	entities.push_back(&Wall3);
	entities.push_back(&Wall4);
	entities.push_back(&Wall5);
	entities.push_back(&Wall6);
	entities.push_back(&Wall7);
	entities.push_back(&Wall9);

	entities.push_back(&Land1);
	entities.push_back(&Land2);
	entities.push_back(&Land3);
	entities.push_back(&Land4);
	entities.push_back(&Land5);
	entities.push_back(&Land6);
	entities.push_back(&Land7);

	entities.push_back(&Vanity1);

	entities.push_back(&Platform1);
	entities.push_back(&Platform2);
	entities.push_back(&Platform3);

	coin1.body = &Coin1;
	coin2.body = &Coin2;
	coin3.body = &Coin3;
	coin4.body = &Coin4;
	coin5.body = &Coin5;

	while (run)
	{

//***********************************************************************************************************\\

		//RIGHT
		if (right && !WallCheckRight())
		{
			if (LandCheck()) {}
				while (right && !WallCheckRight())
				{
					if ((collision && Player.left < Pointy->right))
					{
						toggle = 1;
						if (!fast)
						{
							CameraMove(RIGHT);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(4);
						}
						else if (fast)
						{
							CameraMove(RIGHT);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(3);
						}
					}
					// if player is falling
					else if (((LandCheck()) || ((jump || fall) && !slow && !slower)))
					{
						while ((!collision && !slow) && !WallCheckRight()) {
							if (glide && toggle == 1)
							{
								CameraMove(RIGHT);
								InvalidateRect(hWnd, NULL, FALSE);
								Sleep(2);
							}
							else if (toggle == 1)
							{
								CameraMove(RIGHT);
								InvalidateRect(hWnd, NULL, FALSE);
								Sleep(3);
							}
							// if player is midair and switches directions
							else if (toggle == 0)
							{
								slow = true;
							}

						}
					}
					else if (!(LandCheck()))
					{
						CameraMove(RIGHT);
						fall = true;
						collision = false;
						while ((fall && right) && !WallCheckRight())
						{
							if (slow)
							{
								CameraMove(RIGHT);
								Sleep(9);
							}
							else if (slower)
							{
								CameraMove(RIGHT);
								Sleep(15);
							}
							InvalidateRect(hWnd, NULL, FALSE);
						}
					}
				}
		}

//***********************************************************************************************************\\

		//LEFT
		if (left && !WallCheckLeft())
		{
			if (LandCheck()){}
				while (left && !WallCheckLeft())
				{
					if ((collision && Player.right > Pointy->left))
					{
						toggle = 0;
						if (!fast)
						{
							CameraMove(LEFT);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(4);
						}
						else if (fast)
						{
							CameraMove(LEFT);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(3);
						}

					}
					// if player is falling
					else if (((LandCheck()) || ((jump || fall) && !slow && !slower)))
					{
						while ((!collision && !slow) && !WallCheckLeft()) {
							if (glide && toggle == 0)
							{
								CameraMove(LEFT);
								InvalidateRect(hWnd, NULL, FALSE);
								Sleep(2);
							}
							else if (toggle == 0)
							{
								CameraMove(LEFT);
								InvalidateRect(hWnd, NULL, FALSE);
								Sleep(3);
							}
							// if player is midair and switches directions
							else if (toggle == 1)
							{
								slow = true;
							}
						}
					}
					else if (!(LandCheck()))
					{
						CameraMove(LEFT);
						fall = true;
						collision = false;
						while ((fall && left) && !WallCheckLeft())
						{
							if (slow)
							{
								CameraMove(LEFT);
								Sleep(9);
							}
							else if (slower)
							{
								CameraMove(LEFT);
								Sleep(15);
							}
							InvalidateRect(hWnd, NULL, FALSE);
						}
					}
				}
		}

	

		// update resolution 
		//Sleep(20);

	}// end game loop
}
void scoring(HWND hWnd) // scoring thread
{
	BOOL a;
	BOOL b;
	int foo;
	int bar;
	int frames = 0;

	coins.push_back(&coin1);
	coins.push_back(&coin2);
	coins.push_back(&coin3);
	coins.push_back(&coin4);
	coins.push_back(&coin5);

	while (run)
	{
		for (int i = 0; i < coins.size(); i++)
		{
			if (!coins[i]->render) {
				continue;
			}
			foo = (coins[i]->body->right - coins[i]->body->left);
			bar = (coins[i]->body->left - coins[i]->body->right);
			if (foo == -25 && bar == 25) // alternate direction
			{
				Sleep(40);
				b = true;
				a = false;
			}
			else if (bar == -25 && foo == 25)
			{
				Sleep(40);
				a = true;
				b = false;
			}
			if (a)
			{
				coins[i]->body->left++;
				coins[i]->body->right--;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if (b)
			{
				coins[i]->body->left--;
				coins[i]->body->right++;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			// if player collides
			if ((Player.left <= coins[i]->body->right) && (Player.right >= coins[i]->body->left) && (Player.top <= coins[i]->body->bottom) && (Player.bottom >= coins[i]->body->top))
			{
				coins[i]->taken = true;
				Score++;
			}
			else if (coins[i]->render && !coins[i]->taken)
			{
				Sleep(5);
			}
			// run animation
			if (coins[i]->taken)
			{
				while (coins[i]->render)
				{
					foo = (coins[i]->body->right - coins[i]->body->left);
					bar = (coins[i]->body->left - coins[i]->body->right);
					if (foo == -25 && bar == 25)
					{
						b = true;
						a = false;
					}
					else if (bar == -25 && foo == 25)
					{
						a = true;
						b = false;
					}
					if (a)
					{
						coins[i]->body->left++;
						coins[i]->body->right--;
						InvalidateRect(hWnd, NULL, FALSE);
					}
					else if (b)
					{
						coins[i]->body->left--;
						coins[i]->body->right++;
						InvalidateRect(hWnd, NULL, FALSE);
					}
					coins[i]->body->top--;
					coins[i]->body->bottom--;
					Sleep(7);
					InvalidateRect(hWnd, NULL, FALSE);
					frames++;
					if (frames == 80)
					{
						while (!((coins[i]->body->bottom - coins[i]->body->top) <= 0))
						{
							if (a)
							{
								if (!(foo <= 0))
								{
									coins[i]->body->left++;
									coins[i]->body->right--;
									InvalidateRect(hWnd, NULL, FALSE);
								}
							}
							else if(b)
							{
								if (!(bar <= 0))
								{
									coins[i]->body->left--;
									coins[i]->body->right++;
									InvalidateRect(hWnd, NULL, FALSE);
								}
							}
							coins[i]->body->top++;
							coins[i]->body->bottom--;
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(15);
						}
						coins[i]->render = false;
						coins[i]->taken = false;
						frames = 0;
					}
				}
			}

			if (Score == 5)
			{
				while (Win.top != 250)
				{
					Win.top++;
					Win.bottom++;
					Sleep(1);
					InvalidateRect(hWnd, NULL, FALSE);
				}
			}
		}
		
	}
}


//***********************************************************************************************************\\

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	hInst = hInstance; // Store instance handle in our global variable  

					   // The parameters to CreateWindow explained:  
					   // szWindowClass: the name of the application  
					   // szTitle: the text that appears in the title bar  
					   // WS_OVERLAPPEDWINDOW: the type of window to create  
					   // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)  
					   // 500, 100: initial size (width, length)  
					   // NULL: the parent of this window  
					   // NULL: this application does not have a menu bar  
					   // hInstance: the first parameter from WinMain  
					   // NULL: not used in this application  
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		FIXED_SIZED_WINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WIDTH, HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:  
	// hWnd: the value returned from CreateWindow  
	// nCmdShow: the fourth parameter from WinMain  
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// start game thread
	run = true;
	std::thread t1{ animation, hWnd };
	std::thread t2{ movement, hWnd };
	std::thread t3{ scoring, hWnd };

	// Main message loop:  
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	// kill game thread
	run = false;
	t1.join();
	t2.join();
	t3.join();

	return (int)msg.wParam;
}

//***********************************************************************************************************\\

//  
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)  
//  
//  PURPOSE:  Processes messages for the main window.  
//  
//  WM_PAINT    - Paint the main window  
//  WM_DESTROY  - post a quit message and return  
//  
//  
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;                 // device context (DC) for window  
	PAINTSTRUCT ps;          // paint data for BeginPaint and EndPaint  
	double seconds_since_start = difftime(time(0), start);
	std::string time;
	std::string scr;

	HDC          hdcMem;
	HBITMAP      hbmMem;
	HANDLE       hOld;

	switch (message)
	{
	case WM_CREATE:
		{
		/*
			hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(1));

			// Create a device context (DC) to hold the bitmap.  
			// The bitmap is copied from this DC to the window's DC  
			// whenever it must be drawn.  

			hdc = GetDC(hWnd);
			SelectObject(hdc, hbmp);

			// Create a brush of the same color as the background  
			// of the client area. The brush is used later to erase  
			// the old bitmap before copying the bitmap into the  
			// target rectangle.  

			crBkgnd = GetBkColor(hdc);
			hbrBkgnd = CreateSolidBrush(crBkgnd);
			ReleaseDC(hWnd, hdc);

			// Create a dotted pen. The pen is used to draw the  
			// bitmap rectangle as the user drags it.  

			hpenDot = CreatePen(PS_DOT, 1, RGB(0, 0, 0));

			// Set the initial rectangle for the bitmap. Note that  
			// this application supports only a 32- by 32-pixel  
			// bitmap. The rectangle is slightly larger than the  
			// bitmap.  
			
			SetRect(NULL, 1, 1, 36, 36);
			SetRect(&Ground, 1, 1, 34, 34);
			*/

			Spawn();

			break;
		}
		case WM_PAINT:
		{
			
			// Get DC for window
			hdc = BeginPaint(hWnd, &ps);

			// Create an off-screen DC for double-buffering
			hdcMem = CreateCompatibleDC(hdc);
			hbmMem = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);

			hOld = SelectObject(hdcMem, hbmMem);

			// Draw into hdcMem here
			Rectangle(hdcMem,
				(Player.left + Squash - Squish - Squanch),
				(Player.top - Squash + Squish + Squanch),
				(Player.right - Squash + Squish + Squanch),
				(Player.bottom + Squash - Squish));

			Rectangle(hdcMem,
				Ground.left,
				Ground.top,
				Ground.right,
				Ground.bottom);

			Rectangle(hdcMem,
				Wall1.left,
				Wall1.top,
				Wall1.right,
				Wall1.bottom
			);

			Rectangle(hdcMem,
				Wall2.left,
				Wall2.top,
				Wall2.right,
				Wall2.bottom);

			Rectangle(hdcMem,
				Wall3.left,
				Wall3.top,
				Wall3.right,
				Wall3.bottom);
			
			Rectangle(hdcMem,
				Land1.left,
				Land1.top,
				Land1.right,
				Land1.bottom);

			Rectangle(hdcMem,
				Land2.left,
				Land2.top,
				Land2.right,
				Land2.bottom);

			Rectangle(hdcMem,
				Land3.left,
				Land3.top,
				Land3.right,
				Land3.bottom);

			Rectangle(hdcMem,
				Land4.left,
				Land4.top,
				Land4.right,
				Land4.bottom);

			Rectangle(hdcMem,
				Land5.left,
				Land5.top,
				Land5.right,
				Land5.bottom);
                    
			Rectangle(hdcMem,
				Land6.left,
				Land6.top,
				Land6.right,
				Land6.bottom);

			Rectangle(hdcMem,
				Land7.left,
				Land7.top,
				Land7.right,
				Land7.bottom);

			Rectangle(hdcMem,
				Wall4.left,
				Wall4.top,
				Wall4.right,
				Wall4.bottom);

			Rectangle(hdcMem,
				Wall5.left,
				Wall5.top,
				Wall5.right,
				Wall5.bottom);

			Rectangle(hdcMem,
				Wall6.left,
				Wall6.top,
				Wall6.right,
				Wall6.bottom);

			Rectangle(hdcMem,
				Wall7.left,
				Wall7.top,
				Wall7.right,
				Wall7.bottom);

			Rectangle(hdcMem,
				Wall9.left,
				Wall9.top,
				Wall9.right,
				Wall9.bottom);

			Rectangle(hdcMem,
				Vanity1.left,
				Vanity1.top,
				Vanity1.right,
				Vanity1.bottom);

			Rectangle(hdcMem,
				Platform1.left,
				Platform1.top,
				Platform1.right,
				Platform1.bottom);

			Rectangle(hdcMem,
				Platform2.left,
				Platform2.top,
				Platform2.right,
				Platform2.bottom);

			Rectangle(hdcMem,
				Platform3.left,
				Platform3.top,
				Platform3.right,
				Platform3.bottom);

			

			if (coin1.render)
			{
				Rectangle(hdcMem,
					Coin1.left,
					Coin1.top,
					Coin1.right,
					Coin1.bottom);
			}
			if (coin2.render)
			{
				Rectangle(hdcMem,
					Coin2.left,
					Coin2.top,
					Coin2.right,
					Coin2.bottom);
			}
			if (coin3.render)
			{
				Rectangle(hdcMem,
					Coin3.left,
					Coin3.top,
					Coin3.right,
					Coin3.bottom);
			}
			if (coin4.render)
			{
				Rectangle(hdcMem,
					Coin4.left,
					Coin4.top,
					Coin4.right,
					Coin4.bottom);
			}
			if (coin5.render)
			{
				Rectangle(hdcMem,
					Coin5.left,
					Coin5.top,
					Coin5.right,
					Coin5.bottom);
			}
			if (Score != 5)
			{
				time = std::to_string(seconds_since_start);
				CA2T wt(time.c_str());
				Rectangle(hdcMem,
					timer.left,
					timer.top,
					timer.right,
					timer.bottom);

				DrawText(hdcMem,
					wt,
					6,
					&timer,
					(DT_VCENTER));

				scr = std::to_string(Score);
				CA2T tw(scr.c_str());

				Rectangle(hdcMem,
					score.left,
					score.top,
					score.right,
					score.bottom);

				DrawText(hdcMem,
					tw,
					1,
					&score,
					(DT_VCENTER));
			}
			if (Score == 5)
			{
				Rectangle(hdcMem,
					Win.left,
					Win.top,
					Win.right,
					Win.bottom);
				DrawText(hdcMem,
					"Finish",
					6,
					&Win,
					(DT_VCENTER)
				);

			}

			// Transfer the off-screen DC to the screen
			BitBlt(hdc, 0, 0, WIDTH, HEIGHT, hdcMem, 0, 0, SRCCOPY);

			// Free-up the off-screen DC
			SelectObject(hdcMem, hOld);
			DeleteObject(hbmMem);
			DeleteDC(hdcMem);

			EndPaint(hWnd, &ps);
			return 0;
		}
		case WM_KEYDOWN:

			// Key codes: https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
			// wParam contains key code
			if (wParam == VK_UP || wParam == 0x57 || wParam == VK_SPACE)
			{
				if (collision)
				{
					for(int i=0; i <60;i++) jump = true;
				}
			}
			else if (wParam == VK_RIGHT || wParam == 0x44)
			{
				 right = true;
				 left = false;
				 toggle = 1;
				
			}
			else if (wParam == VK_LEFT|| wParam == 0x41)
			{
				left = true;
				right = false;
				toggle = 0;
			}

			break;
		case WM_KEYUP:

			if (wParam == VK_RIGHT || wParam == 0x44)
			{
				right = false;
				toggle = -1;
			}
			else if (wParam == VK_LEFT || wParam == 0x41)
			{
				left = false;
				toggle = -1;
			}
			break;

		case WM_DESTROY:

			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}

	return 0;
}

//***********************************************************************************************************\\

BOOL LandCheck()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (Player.bottom >= entities[i]->top && Player.left < entities[i]->right-1 && Player.right > entities[i]->left+1 && Player.top < entities[i]->top)
		{
			Pointy = entities[i];
			return true;                                                                                                                          
		}                                                                                                                                        
	}                                                                                                                                            
	return false;                                                                                                                                
}

BOOL WallCheckTop()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (Player.top <= entities[i]->bottom && Player.left < entities[i]->right-1 && Player.right > entities[i]->left+1 && Player.bottom >= entities[i]->bottom)
		{
			return true;
		}
	}
	return false;
}

BOOL WallCheckLeft()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (Player.bottom >= entities[i]->top && Player.left < entities[i]->right && Player.right > entities[i]->left && Player.top < entities[i]->top) { continue; }
		else if (((Player.bottom < entities[i]->bottom && Player.left < entities[i]->right && Player.left > entities[i]->left && Player.top > entities[i]->top)
			|| (Player.bottom > entities[i]->bottom && Player.left < entities[i]->right && Player.left > entities[i]->left && Player.top < entities[i]->bottom)
			|| (Player.bottom > entities[i]->top && Player.left < entities[i]->right && Player.left > entities[i]->left && Player.top < entities[i]->top))
			)
		{
			return true;
		}
	}
	return false;
}

BOOL WallCheckRight()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (Player.bottom >= entities[i]->top && Player.left < entities[i]->right && Player.right > entities[i]->left && Player.top < entities[i]->top) { continue; }
		else if (((Player.bottom < entities[i]->bottom && Player.right > entities[i]->left && Player.right < entities[i]->right && Player.top > entities[i]->top)
			|| (Player.bottom > entities[i]->bottom && Player.right > entities[i]->left && Player.right < entities[i]->right && Player.top < entities[i]->bottom)
			|| (Player.bottom > entities[i]->top && Player.right > entities[i]->left && Player.right < entities[i]->right && Player.top < entities[i]->top))
			)
		{
			return true;
		}
	}
	return false;
}

void CameraMove(Dir d)
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (d == UP)
		{
			entities[i]->top--;
			entities[i]->bottom--;
		}
		else if (d == DOWN)
		{
			entities[i]->top++;
			entities[i]->bottom++;
		}
		else if (d == LEFT)
		{
			entities[i]->left++;
			entities[i]->right++;
		}
		else if (d == RIGHT)
		{
			entities[i]->left--;
			entities[i]->right--;
		}
	}
	
	for (int i = 0; i < coins.size(); i++)
	{
		if (d == UP)
		{
			coins[i]->body->top--;
			coins[i]->body->bottom--;
		}
		else if (d == DOWN)
		{
			coins[i]->body->top++;
			coins[i]->body->bottom++;
		}
		else if (d == LEFT)
		{
			coins[i]->body->left++;
			coins[i]->body->right++;
		}
		else if (d == RIGHT)
		{
			coins[i]->body->left--;
			coins[i]->body->right--;
		}
	}
	
}