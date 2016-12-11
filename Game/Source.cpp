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
BOOL WallCheckLeft();
BOOL WallCheckRight();
BOOL WallCheckTop();

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
static RECT Wall2;
static RECT Wall3;
static RECT Wall4;
static RECT Platform1;

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

BOOL run;

void Spawn()
{
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

	Wall2.left = Wall1.left + 200;
	Wall2.top = Wall1.top - 250;
	Wall2.right = Wall2.left + 100;
	Wall2.bottom = Wall1.top;

	Wall3.left = Ground.right - 100;
	Wall3.top = Ground.top - 250;
	Wall3.right = Ground.right;
	Wall3.bottom = Ground.top + 1;
	
	Wall4.left = -100;
	Wall4.top = Wall1.top - 250;
	Wall4.right = 0;
	Wall4.bottom = (HEIGHT/2)+100;

	Platform1.left = Ground.right-1;
	Platform1.top = Ground.top;
	Platform1.right = Ground.right+100;
	Platform1.bottom = Ground.bottom;

	

}



//***********************************************************************************************************\\

void animation(HWND hWnd)
{

	while (run)
	{
		if (Camera)
		{
		// Jump animation
		if (jump && collision)
		{

			while (frames < 100)
			{
				Squish = 0;
				while (Squish <= 6 && frames < 1)
				{
					Squish++;
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(10);
				}
				while (Squish != -1 && frames < 1)
				{
					Squish--;
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(6);
				}
				if (!WallCheckTop() || !!(WallCheckLeft() || WallCheckRight()))
				{
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(1);
					WorldMove(DOWN);

					if (frames <= 50)
					{
						collision = false;
						if (Squash < 8)
						{
							Squash++;
							frames++;
							WorldMove(DOWN);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(1);
							continue;
						}
						frames++;
						WorldMove(DOWN);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(1);
						continue;
					}
					if (frames <= 80 && frames > 50) {
						while (Squash > 5)
						{
							Squash--;
							WorldMove(DOWN);
							frames++;
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(6);
						}
						WorldMove(DOWN);
						frames++;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(2);
					}
					else if (frames <= 100 && frames > 80) {
						glide = true; Sleep(4); while (Squash > 3)
						{
							Squash--; InvalidateRect(hWnd, NULL, FALSE); Sleep(7); WorldMove(DOWN);
						}frames++;
					}
				}
				else break;
			}
			while (!(WallCheckLeft() || WallCheckRight()))
			{
				Squash--; InvalidateRect(hWnd, NULL, FALSE); Sleep(3); WorldMove(DOWN);
				while (Squash != 0)
				{
					if (Squash > 0)Squash--;
					else Squash++;
					Squish++;
					WorldMove(DOWN);
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(10);
				}
				while (Squish != 0)
				{
					if (Squish > 0) Squish--;
					else Squish++;
					WorldMove(UP);
					InvalidateRect(hWnd, NULL, FALSE);
					Sleep(10);
				}
				break;
			}
			frames = 0; // reset
			while (Squash != 0)
			{
				Squash--;
				WorldMove(DOWN);
				InvalidateRect(hWnd, NULL, FALSE);
				Sleep(7);
			}
			while (Squish < 6)
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
		if (fall && !collision)
		{
			// Wall jumping
			if ((left || right))
			{
				if (left && WallCheckLeft())
				{
					Squanch = 0;
					while (!LandCheck() && left && !jump)
					{
						while (Squash != 4)
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
							WorldMove(UP);
							Sleep(10);
						}
						collision = true;
						fall = false;
						WorldMove(UP);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(15);
						if (!WallCheckLeft()) break;
					}
					while (Squash != 0)
					{
						 Squash--;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(10);
					}
				}
				if (right && WallCheckRight())
				{
					Squanch = 0;
					while (!LandCheck() && right && !jump)
					{
						while (Squash != 4)
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
							WorldMove(UP);
							Sleep(10);
						}
						collision = true;
						fall = false;
						WorldMove(UP);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(15);
						if (!WallCheckRight()) break;
					}
					while (Squash != 0)
					{
						Squash--;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(1);
					}
				}
				if (!jump)
				{
					Squash = 0;
					collision = false;
					fall = true;
				}
			}
			//-----------------------------------------------------------------------------\\

			while (!collision && !((left&&WallCheckLeft())||(right&&WallCheckRight())))
			{
				while (Squash < 4)
				{
					if (Squish != 0) { Squish--; Sleep(10); }
					Squash++;
					frames++;
					WorldMove(UP);
					InvalidateRect(hWnd, NULL, FALSE);
					if (frames < 20) { Sleep(7); }

				}
				if (LandCheck())
				{
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
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(7);
					}
					while (Squish != 0)
					{
						Squish--;
						WorldMove(UP);
						offset--;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(5);
					}
					while (offset != 0)
					{
						WorldMove(DOWN);
						offset++;
					}
					Player.bottom = Pointy->top;
					Player.top = Player.bottom - 36;
					Player.left = Player.right - 36;
					Player.right = Player.left + 36;
					break;
				}
				frames++;
				WorldMove(UP);
				InvalidateRect(hWnd, NULL, FALSE);
				if (frames < 20) Sleep(3);
				else if (frames < 70 && frames >= 20) {
					glide = false;
					Sleep(2);
					while (Squash < 7) {
						Squash++;
						frames++;
						WorldMove(UP);
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
						WorldMove(UP);
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
						WorldMove(UP);
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

		// draw game
		InvalidateRect(hWnd, NULL, FALSE);

		// update resolution 
		//Sleep(20);top++
		}
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
					toggle = -1;
					frames = 0;
					int offset = 0;
					while (Squash != 0)
					{
						Squash--;
						Squish++;
						WorldMove(DOWN);
						offset++;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(5);
					}
					while (Squish != 0)
					{
						Squish--;
						WorldMove(UP);
						offset--;
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(5);
					}
					while (offset != 0)
					{
						WorldMove(DOWN);
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
	entities.push_back(&Ground);
	entities.push_back(&Wall1);
	entities.push_back(&Wall2);
	entities.push_back(&Wall3);
	entities.push_back(&Wall4);
	entities.push_back(&Platform1);

	while (run)
	{

//***********************************************************************************************************\\

		//RIGHT
		if (right)
		{
			Player.right++;
			if (LandCheck()) {}
			while (right)
			{
				if ((collision && Player.left < Pointy->right) && !WallCheckRight() && !jump && !fall)
				{
					toggle = 1;
					if (!fast)
					{
						WorldMove(RIGHT);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(4);
					}
					else if(fast)
					{
						WorldMove(RIGHT);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(3);
					}
				}
				else if (((LandCheck()) || ((jump || fall) && !slow)) && !WallCheckRight())
				{
					while ((!collision && !slow) && !WallCheckRight()) {
						if (glide && toggle == 1)
						{
							WorldMove(RIGHT);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(2);
						}
						else if (toggle == 1)
						{
							WorldMove(RIGHT);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(3);
						}
						else if (toggle == 0)
						{
							slow = true;
						}
					}
				}
				else if (!(LandCheck()) && !WallCheckRight())
				{
					WorldMove(RIGHT);
					fall = true;
					collision = false;
					while ((fall && right) && !WallCheckRight())
					{
						if (slow)
						{
							WorldMove(RIGHT);
							Sleep(9);
						}
						else if (slower)
						{
							WorldMove(RIGHT);
							Sleep(15);
						}
					}
				}
			}
		}

//***********************************************************************************************************\\

		//LEFT
		if (left)
		{
			Player.left--;
			if (LandCheck()){}
			while (left)
			{
				if ((collision && Player.right > Pointy->left) && !WallCheckLeft() && !jump && !fall)
				{
					toggle = 0;
					if (!fast)
					{
						WorldMove(LEFT);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(4);
					}
					else if (fast)
					{
						WorldMove(LEFT);
						InvalidateRect(hWnd, NULL, FALSE);
						Sleep(3);
					}
				}
				else if (((LandCheck()) || ((jump || fall) && !slow)) && !WallCheckLeft())
				{
					while ((!collision && !slow) && !WallCheckLeft()) {
						if (glide && toggle == 0)
						{
							WorldMove(LEFT);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(2);
						}
						else if (toggle == 0)
						{
							WorldMove(LEFT);
							InvalidateRect(hWnd, NULL, FALSE);
							Sleep(3);
						}
						else if (toggle == 1)
						{
							slow = true;
						}
					}
				}
				else if (!(LandCheck()) && !WallCheckLeft())
				{
					WorldMove(LEFT);
					fall = true;
					collision = false;
					while ((fall && left) && !WallCheckLeft())
					{
						if (slow)
						{
							WorldMove(LEFT);
							Sleep(9);
						}
						else if (slower)
						{
							WorldMove(LEFT);
							Sleep(15);
						}
					}
				}
			}
		}

		// draw game
		InvalidateRect(hWnd, NULL, FALSE);

		// update resolution 
		//Sleep(20);

	}// end game loop
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
				Platform1.left,
				Platform1.top,
				Platform1.right,
				Platform1.bottom);

			Rectangle(hdcMem,
				Wall4.left,
				Wall4.top,
				Wall4.right,
				Wall4.bottom);

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
			if (wParam == VK_UP || wParam == 0x57)
			{
				if (collision)
				{
					jump = true;
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
				Player.right--;
			}
			else if (wParam == VK_LEFT || wParam == 0x41)
			{
				left = false;
				toggle = -1;
				Player.left++;
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
		if (Player.bottom > entities[i]->top && Player.left < entities[i]->right && Player.right > entities[i]->left && Player.top < entities[i]->top)
		{
			Pointy = entities[i];
			return true;                                                                                                                         
			break;                                                                                                                               
		}                                                                                                                                        
	}                                                                                                                                            
	return false;                                                                                                                                
}

BOOL WallCheckTop()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (Player.top < entities[i]->bottom && Player.left < entities[i]->right && Player.right > entities[i]->left && Player.bottom > entities[i]->bottom)
		{
			return true;
			break;
		}
	}
	return false;
}

BOOL WallCheckLeft()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (Player.bottom < entities[i]->bottom && Player.left < entities[i]->right && Player.left > entities[i]->left && Player.top > entities[i]->top)
		{
			return true;
			break;
		}
	}
	return false;
}

BOOL WallCheckRight()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (Player.bottom < entities[i]->bottom && Player.right > entities[i]->left && Player.right < entities[i]->right && Player.top > entities[i]->top)
		{
			return true;
			break;
		}
	}
	return false;
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