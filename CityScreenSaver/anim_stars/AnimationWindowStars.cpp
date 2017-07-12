
// MFCApplication1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "AnimationWindowStars.h"
#include "afxdialogex.h"
#include "DC3D.h"

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


// CAnimationWindowStars dialog



CAnimationWindowStars::CAnimationWindowStars(CWnd* pParent /*=NULL*/)
: m_sky(STAR_COUNT)
, m_pressed(key_Nothing)
, m_cxPercent(50.0)
, m_cyPercent(50.0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_cameraDepth = 0;
   m_planeWidth = 100.0;
   m_planeHeight = 100.0;
}


BEGIN_MESSAGE_MAP(CAnimationWindowStars, CWnd)
   ON_WM_PAINT()
   ON_WM_TIMER()
   ON_WM_SIZE()
   ON_WM_CREATE()
   ON_WM_TIMER()
   ON_WM_DESTROY()
   ON_WM_KEYUP()
   ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAnimationWindowStars::OnPaint()
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

      CDC3D mdc(&rc, 0, m_planeWidth, m_planeHeight, m_cxPercent, m_cyPercent);
      mdc.CreateCompatibleDC(&dc);
      CBitmap* oldBitmap = mdc.SelectObject(&bmp);

      rc = dc.m_ps.rcPaint;

      DrawStars(mdc, &rc);
      
      dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(),
         &mdc, rc.left, rc.top, SRCCOPY);
      mdc.SelectObject(oldBitmap);
   }
}



int CAnimationWindowStars::OnCreate(LPCREATESTRUCT cs)
{
   UINT ret;

   ret = SetTimer(ID_TIMER, 5, NULL);
   if (ret == 0)
      MessageBox("Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);

   CRect rc;
   GetClientRect(rc);

   m_planeWidth = rc.Width();
   m_planeHeight = rc.Height();

   m_sky.Init(&rc, m_starsDepthLimit);

   return CWnd::OnCreate(cs);
}


void CAnimationWindowStars::OnDestroy()
{
   KillTimer(ID_TIMER);

   PostQuitMessage(0);
}

void CAnimationWindowStars::OnTimer(UINT_PTR)
{
   switch (m_pressed)
   {
   case CAnimationWindowStars::key_Nothing:
      break;
   case CAnimationWindowStars::key_Left:
      if (m_cxPercent > 0) m_cxPercent -= D_PERCENT;
      break;
   case CAnimationWindowStars::key_Right:
      if (m_cxPercent < MAX_PERCENT) m_cxPercent += D_PERCENT;
      break;
   case CAnimationWindowStars::key_Up:
      if (m_cyPercent > 0) m_cyPercent -= D_PERCENT;
      break;
   case CAnimationWindowStars::key_Down:
      if (m_cyPercent < MAX_PERCENT) m_cyPercent += D_PERCENT;
      break;
   default:
      break;
   }

   m_cameraDepth = (m_cameraDepth + m_starsSpeed) % m_starsDepthDelta;
   
   CRect rc;
   GetClientRect(rc);
   m_sky.MoveStars(rc, m_starsSpeed, m_starsDepthLimit);
   RECT rcClient;
   GetClientRect(&rcClient);

   Invalidate();
}

void CAnimationWindowStars::DrawStars(CDC3D & dc, RECT* prc)
{
   //draw black sky
   CBrush brush; brush.CreateSolidBrush(RGB(0, 0, 0));
   dc.FillRect(prc, &brush);

   //draw stars
   m_sky.Draw(dc, 0/*m_cameraDepth*/);
   
}


void CAnimationWindowStars::OnSize(UINT nType, int cx, int cy)
{
   CWnd::OnSize(nType, cx, cy);
   Invalidate();
}

void CAnimationWindowStars::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

   CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
   if (m_pressed == key_Nothing)
   {
      if (nChar == VK_LEFT)  m_pressed = key_Left;
      else if (nChar == VK_RIGHT) m_pressed = key_Right;
      else if (nChar == VK_UP)    m_pressed = key_Up;
      else if (nChar == VK_DOWN)  m_pressed = key_Down;
   }
}

void CAnimationWindowStars::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
   m_pressed = key_Nothing;

}

//////////////////////////////////////////////////////////////////////////
StarSky::StarSky(int starsCount)
: m_stars(new POINT3D[starsCount])
, m_starsCount(starsCount)
{
   // Seed the random-number generator with the current time so that
   // the numbers will be different every time we run.
   srand((unsigned)time(NULL));

}

StarSky::~StarSky()
{
   delete[] m_stars;
}


double RangedRand(int range_min, int range_max)
{
   // Generate random numbers in the half-closed interval
   // [range_min, range_max). In other words,
   // range_min <= random number < range_max
   double u = (double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
   return u;
}


void StarSky::Init(RECT * prc, double z_limit)
{
   for (int i = 0; i < m_starsCount; i++)
   {
      InitStar(m_stars[i], prc, z_limit);
   }
}

void StarSky::InitStar(POINT3D & star, RECT * prc, double z_limit)
{
   double width  = prc->right  - prc->left;
   double height = prc->bottom - prc->top;

   star.x = RangedRand(prc->left - width *20,  prc->right  + width*20);
   star.y = RangedRand(prc->top  - height*20, prc->bottom + height*20);
   star.z = RangedRand(0, z_limit);

}

void StarSky::Draw(CDC3D & dc, double m_cameraDepth)
{
   CPen pen; pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
   CPen * oldPen = dc.SelectObject(&pen);

   POINT3D pp[4];
   for (int i = 0; i < m_starsCount; i++)
   {
      pp[0] = m_stars[i]; pp[0].x-=2;
      pp[1] = m_stars[i]; pp[1].y-=2;
      pp[2] = m_stars[i]; pp[2].x+=2;
      pp[3] = m_stars[i]; pp[3].y+=2;
      
      dc.Polygon(pp, 4);
   }
   
   dc.SelectObject(&oldPen);
}

void StarSky::MoveStars(RECT * prc, double dz, double z_limit)
{
   for (int i = 0; i < m_starsCount; i++)
   {
      m_stars[i].z -= dz;

      if (m_stars[i].z < -100)
      {
         InitStar(m_stars[i], prc, z_limit);
         m_stars[i].z = z_limit;
      }
   }
}


