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
// Global variables  

enum Dir
{
	UP,
	DOWN,
	LEFT,
	Right
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

// Returns true when Player is colliding with anything that is collidable
BOOL CollisionCheck();
// Returns pointer to object collided with
RECT * Collider();

// Moves the entire world to keep the Player centered on the screen
void WorldMove(Dir d);

// List of collidables objects
std::list<RECT> collidables;

// Animation modifiers
int Squish = 0;
int Squash = 0;
int Squanch = 0;
int frames = 0;

// Entities
static RECT Player;
static RECT Ground;

// Physics flags
BOOL collision = false;
BOOL wall = false;
BOOL fall = true;
BOOL jump = false;

// Movement flags
BOOL right = false;
BOOL left = false;
BOOL canjump = false;

// Movement speed flags
BOOL fast;
BOOL slow;
int toggle = -1;


BOOL run;

void animation(HWND hWnd)
{

	while (run)
	{

		// update game logic here 
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
				Player.top--; 
				Player.bottom--;

				if (frames <= 50 )
				{
					if (Squash < 8)
					{
						Squash++;
						frames++;
						Player.top--;
						Player.bottom--;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(1);
						continue;
					}
					frames++;
					Player.top--;
					Player.bottom--;
					InvalidateRect(hWnd, NULL, TRUE);
					Sleep(1);
					continue;
				}
				if (frames <= 80 && frames > 50) {
					while (Squash > 5)
					{
						Squash--;
						Player.top--;
						Player.bottom--;
						frames++;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(1);
					}
					Player.top--;
					Player.bottom--;
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
				Player.top++;
				Player.bottom++;
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
			while (!collision)
			{
				while (Squash < 4)
				{
					if (Squish != 0) { Squish--; Sleep(25); }
					Squash++;
					if (Player.bottom < Ground.top + 25 && Player.bottom > Ground.top)
					{
						canjump = true;
					}
					else if (Player.bottom > Ground.top && Player.left < Ground.right && Player.right > Ground.left && Player.top < Ground.bottom)
					{
						Player.bottom = Ground.top;
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
					Player.top++;
					Player.bottom++;
					InvalidateRect(hWnd, NULL, TRUE);
					if (frames < 20) { Sleep(6); }

				}
				if (Player.bottom < Ground.top +25 && Player.bottom > Ground.top)
				{
					canjump = true;
				}
				else if (Player.bottom > Ground.top && Player.left < Ground.right && Player.right > Ground.left && Player.top < Ground.top)
				{
					Player.bottom = Ground.top;
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
						Ground.top++;
						Ground.bottom++;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(5);
					}
					while (Squish != 0)
					{
						Squish--;
						Ground.top--;
						Ground.bottom--;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(3);
					}
					Ground.top+=2;
					Ground.bottom+=2;
					Sleep(10);
					break;
				}
				frames++;
				Player.top++;
				Player.bottom++;
				InvalidateRect(hWnd, NULL, TRUE);
				if (frames < 20) Sleep(6);
				else if (frames < 70 && frames >= 20) {
					Sleep(1);
					while (Squash < 7) {
						Squash++;
						frames++;
						Player.top++;
						Player.bottom++;
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
						Player.top++;
						Player.bottom++;
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
						Player.top++;
						Player.bottom++;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(1);
					}
				}
				else Sleep(1); frames++;
			}
		}

		// Walk animation
		if (left || right && !jump)
		{
			while (Squanch < 5 && !jump)
			{
				Squanch++;
				fast = true;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(30);
			}
			while (Squanch >= 0 && !jump)
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

	while (run)
	{

		//RIGHT
		if (right && !left && !wall)
		{
			int temp = Player.right;
			while (Player.right != (temp + 5))
			{
				Player.right++;
				Player.left++;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(15);
			}
			while (!wall && right)
			{
				if (collision && Player.left < Ground.right)
				{
					toggle = 1;
					if (!fast)
					{
						Ground.left--;
						Ground.right--;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(4);
					}
					else if(fast)
					{
						Ground.left--;
						Ground.right--;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(3);
					}
				}
				else if ((Player.bottom > Ground.top && Player.left < Ground.right && Player.right > Ground.left && Player.top < Ground.top) || (jump || fall) && !slow)
				{
					while (!collision && !slow) {
						if (toggle == 1)
						{
							Ground.left--;
							Ground.right--;
							InvalidateRect(hWnd, NULL, TRUE);
							Sleep(3);
						}
						else if (toggle == 0)
						{
							slow = true;
						}
					}
				}
				else if (!(Player.bottom > Ground.top && Player.left < Ground.right && Player.right > Ground.left && Player.top < Ground.top))
				{
					Ground.left--;
					Ground.right--;
					fall = true;
					collision = false;
					slow = true;
					while (fall && right)
					{
						Ground.left--;
						Ground.right--;
						Sleep(9);
					}
				}
				
			}
			while (Player.right != temp)
			{
				Player.right--;
				Player.left--;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(10);
			}
		}

		//LEFT
		if (left && !right && !wall)
		{
			int temp = Player.left;
			while (Player.left != (temp - 5))
			{
				Player.left--;
				Player.right--;
				InvalidateRect(hWnd, NULL, TRUE);
				Sleep(15);
			}
			while (!wall && left)
			{
				if (collision && Player.right > Ground.left)
				{
					toggle = 0;
					if (!fast)
					{
						Ground.left++;
						Ground.right++;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(4);
					}
					else if (fast)
					{
						Ground.left++;
						Ground.right++;
						InvalidateRect(hWnd, NULL, TRUE);
						Sleep(3);
					}
				}
				else if ((Player.bottom > Ground.top && Player.left < Ground.right && Player.right > Ground.left && Player.top < Ground.top) || (jump || fall) && !slow)
				{
					while (!collision && !slow) {
						if (toggle == 0)
						{
							Ground.left++;
							Ground.right++;
							InvalidateRect(hWnd, NULL, TRUE);
							Sleep(3);
						}
						else if (toggle == 1)
						{
							slow = true;
						}
					}
				}
				else if (!(Player.bottom > Ground.top && Player.left < Ground.right && Player.right > Ground.left && Player.top < Ground.top))
				{
					Ground.left++;
					Ground.right++;
					fall = true;
					collision = false;
					slow = true;
					while (fall && left)
					{
						Ground.left++;
						Ground.right++;
						Sleep(9);
					}
				}
				
			}
			while (Player.left != temp)
			{
				Player.left++;
				Player.right++;
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
			Player.top = -35;
			Player.bottom = 0;
			Player.right = 518;

			Ground.left = 0;
			Ground.top = HEIGHT - 400;
			Ground.right = WIDTH;
			Ground.bottom = HEIGHT-200;

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
                                
			EndPaint(hWnd, &ps);

			break;
		}
		case WM_KEYDOWN:

			// Key codes: https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
			// wParam contains key code
			if (wParam == VK_SPACE)
			{
				if (canjump)
				{
					jump = true;
					canjump = false;
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

BOOL CollisionCheck()
{
	return false;
}

RECT * Collider()
{
	RECT * rect = NULL;
	return rect;
}

void WorldMove(Dir d)
{
	//....
}