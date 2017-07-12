
// MFCApplication1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "AnimationWindow.h"
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


// CAnimationWindow dialog



CAnimationWindow::CAnimationWindow(CWnd* pParent /*=NULL*/)
: m_cxPercent(MAX_PERCENT/2.0)
, m_cyPercent(MAX_PERCENT/2.0)
, m_pressed(key_Nothing)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_cameraDepth = 0;
   m_planeWidth = 100.0;
   m_planeHeight = 100.0;
}


BEGIN_MESSAGE_MAP(CAnimationWindow, CWnd)
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_CREATE()
   ON_WM_TIMER()
   ON_WM_DESTROY()
   ON_WM_KEYDOWN()
   ON_WM_KEYUP()
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

      CDC3D mdc(&rc, 0, m_planeWidth, m_planeHeight, m_cxPercent, m_cyPercent);
      mdc.CreateCompatibleDC(&dc);
      CBitmap* oldBitmap = mdc.SelectObject(&bmp);

      rc = dc.m_ps.rcPaint;
      CBrush brush; brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
      mdc.FillRect(rc, &brush);

      DrawTonnel(mdc, &rc);
      
      dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(),
         &mdc, rc.left, rc.top, SRCCOPY);
      mdc.SelectObject(oldBitmap);
   }
}



int CAnimationWindow::OnCreate(LPCREATESTRUCT cs)
{
   UINT ret;

   ret = SetTimer(ID_TIMER, 5, NULL);
   if (ret == 0)
      MessageBox("Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);

   CRect rc;
   GetClientRect(rc);

   m_planeWidth = rc.Width();
   m_planeHeight = rc.Height();

   return CWnd::OnCreate(cs);
}


void CAnimationWindow::OnDestroy()
{
   KillTimer(ID_TIMER);


   PostQuitMessage(0);
} 

void CAnimationWindow::OnTimer(UINT_PTR)
{

   switch (m_pressed)
   {
   case CAnimationWindow::key_Nothing:
      break;
   case CAnimationWindow::key_Left:
      if (m_cxPercent > 0) m_cxPercent -= D_PERCENT;
      break;
   case CAnimationWindow::key_Right:
      if (m_cxPercent < MAX_PERCENT) m_cxPercent += D_PERCENT;
      break;
   case CAnimationWindow::key_Up:
      if (m_cyPercent > 0) m_cyPercent -= D_PERCENT;
      break;
   case CAnimationWindow::key_Down:
      if (m_cyPercent < MAX_PERCENT) m_cyPercent += D_PERCENT;
      break;
   default:
      break;
   }

   m_cameraDepth = (m_cameraDepth - m_tonnelSpeed) % m_tonnelDepthDelta;
   RECT rcClient;
   GetClientRect(&rcClient);

   Invalidate();
}

void CAnimationWindow::DrawTonnel(CDC3D & dc, RECT* prc)
{
   {
      const int dy = 40;
      for (int y = prc->top; y <= prc->bottom; y += dy)
      {
         dc.MoveTo(0, y, 0);
         dc.LineTo(0, y, m_tonnelDepthEnd);
         dc.MoveTo(prc->right, y, 0);
         dc.LineTo(prc->right, y, m_tonnelDepthEnd);
      }

      const int dx = 40;
      for (int x = prc->left; x <= prc->right; x += dx)
      {
         dc.MoveTo(x, 0, 0);
         dc.LineTo(x, 0, m_tonnelDepthEnd);
         dc.MoveTo(x, prc->bottom, 0);
         dc.LineTo(x, prc->bottom, m_tonnelDepthEnd);
      }
   }

   //draw perspective tunnel rectangles
   
   //loop point (x=0, y=0, z=depth) from projection plane to some distance
   for (int depth = m_cameraDepth /*- m_tonnelDepthDelta*/; depth < m_tonnelDepthEnd; depth += m_tonnelDepthDelta)
   {
      dc.MoveTo(0, 0, depth);
      dc.LineTo(prc->right, 0, depth);
      dc.LineTo(prc->right, prc->bottom, depth);
      dc.LineTo(0, prc->bottom, depth);
      dc.LineTo(0, 0, depth);

      //dc.MoveTo(0, 0, depth);
      //dc.LineTo(prc->right, 0, depth + m_tonnelDepthDelta/4);
      //dc.LineTo(prc->right, prc->bottom, depth + m_tonnelDepthDelta/2);
      //dc.LineTo(0, prc->bottom, depth + m_tonnelDepthDelta*3/4);
      //dc.LineTo(0, 0, depth + m_tonnelDepthDelta);
   }
}


void CAnimationWindow::OnSize(UINT nType, int cx, int cy)
{
   CWnd::OnSize(nType, cx, cy);
   Invalidate();
}

void CAnimationWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

void CAnimationWindow::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
   m_pressed = key_Nothing;

}
