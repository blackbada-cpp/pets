#include <crtdbg.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h> 
#include "resource.h"
#include "BallInfo.h"

const char g_szClassName[] = "myWindowClass";
const int ID_TIMER = 1;


HBITMAP g_hbmBall = NULL;
HBITMAP g_hbmMask = NULL;
HBRUSH g_hbrush = NULL;

HWND CreateButton(const HWND,const HINSTANCE,DWORD,const RECT*,const int,const char * caption);

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);

	SetBkColor(hdcMem, crTransparent);

	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}

void DrawBalls(HDC hdc, RECT* prc)
{
	int i;
   
   //Screen buffer for drawing without flicking
   HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, prc->right, prc->bottom);
	HBITMAP hbmOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmMask);

   FillRect(hdcBuffer, prc, g_hbrush /*(HBRUSH)GetStockObject(WHITE_BRUSH)*/);
   
   for(i=0; i<MAX_BALLS; i++)
   {
      //Cut black hole
      SelectObject(hdcMem, g_hbmMask);
      BitBlt(hdcBuffer, g_balls[i].x, g_balls[i].y, g_balls[i].width, g_balls[i].height, hdcMem, 0, 0, SRCAND);

	   SelectObject(hdcMem, g_hbmBall);
	   BitBlt(hdcBuffer, g_balls[i].x, g_balls[i].y, g_balls[i].width, g_balls[i].height, hdcMem, 0, 0, SRCPAINT);
   }

	//Copy screen from buffer
   BitBlt(hdc, 0, 0, prc->right, prc->bottom, hdcBuffer, 0, 0, SRCCOPY);

	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);

	SelectObject(hdcBuffer, hbmOldBuffer);
	DeleteDC(hdcBuffer);
	DeleteObject(hbmBuffer);
}

HWND st_hedit = NULL;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
         CREATESTRUCT *cs = (CREATESTRUCT*)(lParam);

         UINT ret;
			BITMAP bm;
         RECT rcClient;
         int i;
         LOGBRUSH brush;
         RECT rc={10,10,200,40};

         //////////////////////////////////////////////////////////////////////////
         //the various button types are created by simply varying the style bits
         CreateButton(hwnd,cs->hInstance,BS_DEFPUSHBUTTON,&rc,IDBC_DEFPUSHBUTTON, "DEFAULT PUSH BUTTON");

         //////////////////////////////////////////////////////////////////////////
         // Seed the random-number generator with the current time so that
         // the numbers will be different every time we run.
         srand( (unsigned)time( NULL ) );

         g_hbmBall = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BALL));
			if(g_hbmBall == NULL)
				MessageBox(hwnd, "Could not load IDB_BALL!", "Error", MB_OK | MB_ICONEXCLAMATION);

			g_hbmMask = CreateBitmapMask(g_hbmBall, RGB(0, 0, 0));
			if(g_hbmMask == NULL)
				MessageBox(hwnd, "Could not create mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

         //Create brush
         brush.lbStyle = BS_HATCHED;//BS_SOLID;
         brush.lbColor = RGB(255, 128, 1);
         brush.lbHatch = HS_CROSS;
         g_hbrush = CreateBrushIndirect(&brush);

			GetObject(g_hbmBall, sizeof(bm), &bm);

         GetClientRect(hwnd, &rcClient);
         for(i=0; i<MAX_BALLS; i++)
            InitBall(&g_balls[i], &bm, &rcClient);

			ret = SetTimer(hwnd, ID_TIMER, 50, NULL);
			if(ret == 0)
				MessageBox(hwnd, "Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_PAINT:
		{
			RECT rcClient;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rcClient);
			
         DrawBalls(hdc, &rcClient);

			EndPaint(hwnd, &ps);
		}
		break;
		case WM_TIMER:
		{
			RECT rcClient;
         int i;
			HDC hdc = GetDC(hwnd);
			GetClientRect(hwnd, &rcClient);

         CheckCollisions();
         
         for(i=0; i<MAX_BALLS; i++)
         {
            UpdateBall(&g_balls[i], &g_ballCollisions[i], &rcClient);
         }
         //InvalidateRect(hwnd, NULL, TRUE); 
         //UpdateWindow(hwnd);

         DrawBalls(hdc, &rcClient);

			ReleaseDC(hwnd, hdc);
		}
		break;
		case WM_DESTROY:
			KillTimer(hwnd, ID_TIMER);

			DeleteObject(g_hbmBall);
			DeleteObject(g_hbmMask);
         DeleteObject(g_hbrush);

			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"An Animation Program",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 700,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}


//=============================================================================
HWND CreateButton(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle,
                  const RECT* rc,const int id,const char * caption)
{
   dwStyle|=WS_CHILD|WS_VISIBLE;
   return CreateWindowEx(0,                            //extended styles
      "button",                 //control 'class' name
      caption,              //control caption
      dwStyle,                      //control style 
      rc->left,                      //position: left
      rc->top,                       //position: top
      rc->right,                     //width
      rc->bottom,                    //height
      hParent,                      //parent window handle
      //control's ID
      id,
      hInst,                        //application instance
      0);                           //user defined info
}