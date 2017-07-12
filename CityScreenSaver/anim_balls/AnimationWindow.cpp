
// MFCApplication1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "AnimationWindow.h"
#include "afxdialogex.h"
extern "C"
{
   #include "BallInfo.h"
}
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int ID_TIMER = 1;


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAnimationWindow dialog



CAnimationWindow::CAnimationWindow(CWnd* pParent /*=NULL*/)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_tonnelDepthStart = 0;
}


BEGIN_MESSAGE_MAP(CAnimationWindow, CWnd)
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_CREATE()
   ON_WM_TIMER()
   ON_WM_DESTROY()
END_MESSAGE_MAP()



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAnimationWindow::OnPaint()
{
	if (IsIconic())
	{
      CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
      //CWnd::OnPaint();
      
      CPaintDC dc(this); // device context for painting

      CRect rc;
      GetClientRect(rc);

      CBitmap bmp;
      bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());

      CDC mdc;
      mdc.CreateCompatibleDC(&dc);
      CBitmap* oldBitmap = mdc.SelectObject(&bmp);

      rc = dc.m_ps.rcPaint;
      CBrush brush; brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
      mdc.FillRect(rc, &brush);

      DrawBalls(mdc, &rc);
      
      dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(),
         &mdc, rc.left, rc.top, SRCCOPY);
      mdc.SelectObject(oldBitmap);
   }
}

//////////////////////////////////////////////////////////////////////////
void CreateBitmapMask(CBitmap & bmMask, CBitmap &bmColour, COLORREF crTransparent)
{
   CDC dcMem, dcMem2;
   HBITMAP hbmMask;
   BITMAP bm;

   bmColour.GetBitmap(&bm);
   bmMask.CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

   dcMem.CreateCompatibleDC(NULL);
   dcMem2.CreateCompatibleDC(NULL);

   dcMem.SelectObject(&bmColour);
   dcMem2.SelectObject(&bmMask);

   dcMem.SetBkColor(crTransparent);

   dcMem2.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCCOPY);

   dcMem.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcMem2, 0, 0, SRCINVERT);
}

int CAnimationWindow::OnCreate(LPCREATESTRUCT cs)
{
   UINT ret;
   BITMAP bm;
   RECT rcClient;
   int i;
   LOGBRUSH brush;
   RECT rc = { 10, 10, 200, 40 };

   //////////////////////////////////////////////////////////////////////////
   // Seed the random-number generator with the current time so that
   // the numbers will be different every time we run.
   srand((unsigned)time(NULL));

   m_ball.LoadBitmap(IDB_BALL);
   CreateBitmapMask(m_mask, m_ball, RGB(0, 0, 0));

   //Create brush
   brush.lbStyle = BS_HATCHED;//BS_SOLID;
   brush.lbColor = RGB(255, 128, 1);
   brush.lbHatch = HS_CROSS;
   m_brush.CreateBrushIndirect(&brush);

   m_ball.GetBitmap(&bm);

   GetClientRect(&rcClient);
   for (i = 0; i < MAX_BALLS; i++)
      InitBall(&g_balls[i], &bm, &rcClient);

   ret = SetTimer(ID_TIMER, TIMER_ELAPSE_MS, NULL);
   if (ret == 0)
      MessageBox("Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);
   
   return CWnd::OnCreate(cs);
}


void CAnimationWindow::OnDestroy()
{
   KillTimer(ID_TIMER);


   PostQuitMessage(0);
}

void CAnimationWindow::OnTimer(UINT_PTR)
{
   m_tonnelDepthStart = (m_tonnelDepthStart - m_tonnelSpeed) % m_tonnelDepthDelta;
   RECT rcClient;
   GetClientRect(&rcClient);

   //Update world
   CheckCollisions();
   for (int i = 0; i < MAX_BALLS; i++)
   {
      UpdateBall(&g_balls[i], &g_ballCollisions[i], &rcClient);
   }

   //DrawBalls(dc, &rcClient);
   
   Invalidate();
}

void CAnimationWindow::DrawTonnel(CDC & dc, RECT* prc)
{
   {
      const int dy = 10;
      for (int y = prc->top; y <= prc->bottom; y += dy)
      {
         dc.MoveTo(prc->left, y);
         dc.LineTo(prc->right, prc->bottom - y);


         //dc.MoveTo(prc->left, prc->bottom-y);
         //dc.LineTo(prc->right, y);
      }

      const int dx = 10;
      for (int x = prc->left; x <= prc->right; x += dx)
      {
         dc.MoveTo(x, prc->top);
         dc.LineTo(prc->right - x, prc->bottom);


         //dc.MoveTo(x, prc->bottom);
         //dc.LineTo(prc->right-x, prc->top);
      }
   }

   //draw perspective tonnel rectangles
   
   //Eye coordinates behind projection plane
   int cy = (prc->bottom - prc->top) / 2;
   int cx = (prc->right - prc->left) / 2;
   int focus = 2*max(cx, cy);

   //loop point (x=0, y=0, z=depth) from projection plane to some distance
   for (int depth = m_tonnelDepthStart /*- m_tonnelDepthDelta*/; depth < m_tonnelDepthEnd; depth += m_tonnelDepthDelta)
   {
      int py = cy * depth / (depth + focus); //projection of y
      int px = cx * depth / (depth + focus); //projection of x
      //dc.Rectangle(px, py, prc->right - px, prc->bottom - py);
      dc.MoveTo(px, py);
      dc.LineTo(prc->right - px, py);
      dc.LineTo(prc->right - px, prc->bottom - py);
      dc.LineTo(px, prc->bottom - py);
      dc.LineTo(px, py);
   }
}


void CAnimationWindow::DrawBalls(CDC & dc, RECT* prc)
{
   int i;

   CDC dcMem;
   dcMem.CreateCompatibleDC(&dc);
   CBitmap * oldBitmap = dcMem.SelectObject(&m_mask);
   BITMAP bm;
   m_mask.GetBitmap(&bm);

   //sort balls by depth
   std::multimap<int, BALLINFO &> ballmap;
   std::multimap<int, BALLINFO &>::reverse_iterator iball;
   for (i = 0; i < MAX_BALLS; i++)
   {
      ballmap.insert(std::pair<int, BALLINFO &>(g_balls[i].z, g_balls[i]));
   }

   //draw perspective balls

   //Eye coordinates behind projection plane
   int cy = (prc->bottom - prc->top) / 2;
   int cx = (prc->right - prc->left) / 2;
   int focus = 2 * max(cx, cy);

   for (iball = ballmap.rbegin(); iball != ballmap.rend(); ++iball)
   {
      BALLINFO & ball = (*iball).second;
      int pwidth = ball.width*focus/(focus+ball.z);//projection of width
      int pheight = ball.height*focus / (focus + ball.z);//projection of height
      
      //projection of y coord
      int y_ = cy - ball.y;
      int py_ = y_*focus / (focus + ball.z);
      int py = cy - py_;
      //projection of x coord
      int x_ = cx - ball.x;
      int px_ = x_*focus / (focus + ball.z);
      int px = cx - px_;


      //Cut black hole
      //dcMem.SelectObject(&m_mask);
      //dc.BitBlt(g_balls[i].x, g_balls[i].y, g_balls[i].width, g_balls[i].height, &dcMem, 0, 0, SRCAND);

      //dcMem.SelectObject(&m_ball);
      //dc.BitBlt(g_balls[i].x, g_balls[i].y, g_balls[i].width, g_balls[i].height, &dcMem, 0, 0, SRCPAINT);

      //Draw balls with stretching size
      //Cut black hole
      dcMem.SelectObject(&m_mask);
      dc.StretchBlt(px, py, pwidth, pheight, &dcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

      dcMem.SelectObject(&m_ball);
      dc.StretchBlt(px, py, pwidth, pheight, &dcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
   }

   //Copy screen from buffer
   dcMem.SelectObject(oldBitmap);
}

void CAnimationWindow::OnSize(UINT nType, int cx, int cy)
{
   CWnd::OnSize(nType, cx, cy);
   Invalidate();
}
