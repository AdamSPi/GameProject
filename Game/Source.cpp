// GT_HelloWorldWin32.cpp  
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c  

// https://msdn.microsoft.com/en-us/library/bb384843.aspx

#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>  
#include <thread> 
#include <atomic> 
#include <list>
#include <vector>
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
static TCHAR szTitle[] = _T("Bounce Box");

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
BOOL WallCheck();
// Returns pointer to object collided with
RECT * Collider();

// Moves the entire world to keep the Player centered on the screen
void WorldMove(Dir d);

// List of collidables objects
std::vector<RECT*> entities;

// Animation modifiers
int Squish = 0;
int Squash = 0;
int Squanch = 0;
int frames = 0;

// Entities
static RECT Player;
static RECT Ground;
static RECT Wall1;
static RECT Death;
static RECT Platform1;
static RECT * Pointy;

// Physics flags
BOOL collision = false;
BOOL fall = true;
BOOL jump = false;

// Movement flags
BOOL right = false;
BOOL left = false;

// Movement speed flags
BOOL fast;
BOOL slow;
int toggle = -1;


BOOL run;

void animation(HWND hWnd)
{

	while (run)
	{

		// DOWNdate game logic here 
		// Jump animation
		if (jump && collision && !fall)
		{
			collision = false;
			while (frames < 100) // jump 100 frames high
			{
				Squish = 0;
				while (Squish <= 6 && frames < 1)
				{
					Squish++;
					InvalidateRect(hWnd, NULL, TRUE);
					Sleep(10);
				}
				while (Squish != -1 && frames < 1)
				{
					Squish--;
					InvalidateRect(hWnd, NULL, TRUE);
					Sleep(6);
				}

				InvalidateRect(hWnd, NULL, TRUE); 
				Sleep(1); 
				WorldMove(DOWN);

				if (frames <= 50 )
				{
					if (Squash < 8)
					{
						Squash++;
						frames++;
						WorldMove(DOWN);
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(1);
						continue;
					}
					frames++;
					WorldMove(DOWN);
					InvalidateRect(hWnd, NULL, TRUE);
					Sleep(1);
					continue;
				}
				if (frames <= 80 && frames > 50) {
					while (Squash > 5)
					{
						Squash--;
						WorldMove(DOWN);
						frames++;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(1);
					}
					WorldMove(DOWN);
					frames++;
					InvalidateRect(hWnd, NULL, TRUE);
					Sleep(2);
				}
				else if (frames <= 100) { Sleep(1); while (Squash > 3) 
				{ Squash--; InvalidateRect(hWnd, NULL, TRUE); Sleep(1); Player.top--;Player.bottom--;}frames++;}
			}

			frames = 0; // reset
			while (Squash != 0)
			{
				Squash--;
				WorldMove(UP);
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(6);
			}
			while (Squish < 6)
			{
				Squish++;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(5);
			}
			jump = false;
			fall = true;
		}

		// Falling condition
		if (fall && !collision)
		{
			while (!collision && !WallCheck())
			{
				while (Squash < 4)
				{
					if (Squish != 0) { Squish--; Sleep(25); }
					Squash++;
					if (LandCheck())
					{
						Pointy = Collider();
						Player.bottom = Pointy->top;
						Player.top = Player.bottom - 35;
						fall = false;
						collision = true;
						slow = false;
						toggle = -1;
						frames = 0;
						while (Squash != 0)
						{
							Squash--;
							Squish++;
							InvalidateRect(hWnd, NULL, TRUE);
							Sleep(5);
						}
						Squish = 0;
						Sleep(10);
						break;
					}
					frames++;
					WorldMove(UP);
					InvalidateRect(hWnd, NULL, TRUE);
					if (frames < 20) { Sleep(6); }

				}
				if (LandCheck())
				{
					Pointy = Collider();
					Player.bottom = Pointy->top;
					Player.top = Player.bottom - 35;
					fall = false;
					collision = true;
					slow = false;
					toggle = -1;
					frames = 0;
					int offset = 0;
					while (Squash != 0)
					{
						Squash--;
						Squish++;
						WorldMove(DOWN);
						offset++;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(7);
					}
					while (Squish != 0)
					{
						Squish--;
						WorldMove(UP);
						offset--;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(5);
					}
					while (offset != 0)
					{
						WorldMove(DOWN);
						offset++;
					}
					break;
				}
				frames++;
				WorldMove(UP);
				InvalidateRect(hWnd, NULL, TRUE);
				if (frames < 20) Sleep(6);
				else if (frames < 70 && frames >= 20) {
					Sleep(1);
					while (Squash < 7) {
						Squash++;
						frames++;
						WorldMove(UP);
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(5);
					}
				}
				else  if (frames < 200 && frames >= 70)
				{
					Sleep(1);
					while (Squash < 8)
					{
						Squash++;
						frames++;
						WorldMove(UP);
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(4);
					}
				}
				else if (frames >= 400)
				{
					Sleep(1);
					while (Squash <= 9)
					{
						Squash++;
						frames++;
						WorldMove(UP);
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(1);
					}
				}
				else Sleep(1); frames++;
			}
			if (WallCheck())
			{
				//while()
			}
		}

		// Walk animation
		if (left || right && !jump && !WallCheck())
		{
			while (Squanch < 5 && !jump && !WallCheck())
			{
				Squanch++;
				fast = true;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(30);
			}
			while (Squanch >= 0 && !jump && !WallCheck())
			{
				fast = false;
				Squanch--;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(30);
			}
		}
		else { while (Squanch != -1) { Squanch--; InvalidateRect(hWnd, NULL, TRUE);Sleep(30);} }

		// draw game
		InvalidateRect(hWnd, NULL, TRUE);

		// update resolution 
		Sleep(20);

	}// end game loop
}

void movement(HWND hWnd)
{
	entities.push_back(&Ground);
	entities.push_back(&Wall1);
	entities.push_back(&Platform1);

	while (run)
	{
		
		//RIGHT
		if (right && !left)
		{
			if (LandCheck())
			{
				Pointy = Collider();
			}
			int temp = Player.right;
			while (Player.right != (temp + 5))
			{
				Player.right++;
				//Player.left++;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(15);
			}
			while (right)
			{
				if ((collision && Player.left < Pointy->right) && !WallCheck())
				{
					toggle = 1;
					if (!fast)
					{
						WorldMove(RIGHT);
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(4);
					}
					else if(fast)
					{
						WorldMove(RIGHT);
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(3);
					}
				}
				else if (((LandCheck()) || (jump || fall) && !slow) && !WallCheck())
				{
					while (!collision && !slow) {
						if (toggle == 1 && !WallCheck())
						{
							WorldMove(RIGHT);
							InvalidateRect(hWnd, NULL, TRUE);
							Sleep(3);
						}
						else if (toggle == 0)
						{
							slow = true;
						}
					}
				}
				else if (!(LandCheck()) && !WallCheck())
				{
					WorldMove(RIGHT);
					fall = true;
					collision = false;
					slow = true;
					while ((fall && right) && !WallCheck())
					{
						WorldMove(RIGHT);
						Sleep(9);
					}
				}
				
			}
			while (Player.right != temp)
			{
				Player.right--;
				//Player.left--;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(10);
			}
		}

		//LEFT
		if (left && !right)
		{
			if (LandCheck())
			{
				Pointy = Collider();
			}
			int temp = Player.left;
			while (Player.left != (temp - 5))
			{
				Player.left--;
				//Player.right--;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(15);
			}
			while (left)
			{
				if ((collision && Player.right > Pointy->left) && !WallCheck())
				{
					toggle = 0;
					if (!fast)
					{
						WorldMove(LEFT);
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(4);
					}
					else if (fast)
					{
						WorldMove(LEFT);
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(3);
					}
				}
				else if (((LandCheck()) || (jump || fall) && !slow) && !WallCheck())
				{
					while ((!collision && !slow) && !WallCheck()) {
						if (toggle == 0)
						{
							WorldMove(LEFT);
							InvalidateRect(hWnd, NULL, TRUE);
							Sleep(3);
						}
						else if (toggle == 1)
						{
							slow = true;
						}
					}
				}
				else if (!(LandCheck()) && !WallCheck())
				{
					WorldMove(LEFT);
					fall = true;
					collision = false;
					slow = true;
					while ((fall && left) && !WallCheck())
					{
						WorldMove(LEFT);
						Sleep(9);
					}
				}
				
			}
			while (Player.left != temp)
			{
				Player.left++;
				//Player.right++;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(10);
			}
		}

		// draw game
		InvalidateRect(hWnd, NULL, TRUE);

		// update resolution 
		Sleep(20);

	}// end game loop
}


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

	return (int)msg.wParam;
}

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
			
			SetRect(&Player, 1, 1, 36, 36);
			SetRect(&Ground, 1, 1, 34, 34);
			*/

			Player.left = 482;
			Player.top = 382;
			Player.bottom = 418;
			Player.right = 518;

			Ground.left = 0;
			Ground.top = HEIGHT;
			Ground.right = WIDTH;
			Ground.bottom = HEIGHT+200;

			Wall1.left = Ground.left;
			Wall1.top = Ground.top - 250;
			Wall1.right = Wall1.left + 100;
			Wall1.bottom = Ground.top+1;

			break;
		}
		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			// draw game here 

			Rectangle(hdc,
				(Player.left + Squash - Squish - Squanch),
				(Player.top - Squash + Squish + Squanch),	    
				(Player.right - Squash + Squish + Squanch),   
				(Player.bottom + Squash - Squish));	
                                                    
			Rectangle(hdc,            
				Ground.left,	
				Ground.top,	    
				Ground.right,   
				Ground.bottom);	

			Rectangle(hdc,
				Wall1.left,
				Wall1.top,
				Wall1.right,
				Wall1.bottom
			);
                                
			EndPaint(hWnd, &ps);

			break;
		}
		case WM_KEYDOWN:

			// Key codes: https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
			// wParam contains key code
			if (wParam == VK_SPACE)
			{
				if (collision)
				{
					jump = true;
				}
			}
			else if (wParam == VK_RIGHT)
			{
				right = true;
				left = false;
				toggle = 1;
				
			}
			else if (wParam == VK_LEFT)
			{
				left = true;
				right = false;
				toggle = 0;
			}

			break;
		case WM_KEYUP:

			if (wParam == VK_RIGHT)
			{
				right = false;
				toggle = -1;
			}
			else if (wParam == VK_LEFT)
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

BOOL LandCheck()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (Player.bottom > entities[i]->top && Player.left <= entities[i]->right && Player.right >= entities[i]->left && Player.top < entities[i]->top)
		{                                                                                                                                        
			return true;                                                                                                                         
			break;                                                                                                                               
		}                                                                                                                                        
	}                                                                                                                                            
	return false;                                                                                                                                
}

BOOL WallCheck()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (Player.bottom <= entities[i]->bottom && Player.left <= entities[i]->right && Player.right >= entities[i]->left && Player.top >= entities[i]->top)
		{
			return true;
			break;
		}
	}
	return false;
}

                                                                                                                                                 
RECT * Collider()                                                                                                                                
{                                                                                                                                                
	RECT * rect = &Death;                                                                                                                        
	for (int i = 0; i < entities.size(); i++)                                                                                                    
	{                                                                                                                                            
		if (Player.bottom > entities[i]->top && Player.left < entities[i]->right && Player.right > entities[i]->left && Player.top < entities[i]->top)
		{
			rect = entities[i];
			break;
		}
	}
	return rect;
}

void WorldMove(Dir d)
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
}