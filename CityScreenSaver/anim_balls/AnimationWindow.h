
// MFCApplication1Dlg.h : header file
//

#pragma once


#define TIMER_ELAPSE_MS (20)

// CAnimationWindow dialog
class CAnimationWindow : public CWnd
{
// Construction
public:
	CAnimationWindow(CWnd* pParent = NULL);	// standard constructor


// Implementation
protected:
	HICON m_hIcon;
   int m_tonnelDepthStart;

   CBitmap m_ball;
   CBitmap m_mask;
   CBrush  m_brush;

   const int m_tonnelSpeed = 4;
   const int m_tonnelDepthDelta = 200;
   const int m_tonnelDepthEnd   = 20000;
   void DrawTonnel(CDC & dc, RECT* prc);
   void DrawBalls(CDC & dc, RECT* prc);

protected:
   // Generated message map functions
   afx_msg int  OnCreate(LPCREATESTRUCT);
   afx_msg void OnDestroy();
   afx_msg void OnPaint();
   afx_msg void OnTimer(UINT_PTR);
   afx_msg void OnSize(UINT nType, int cx, int cy);


   DECLARE_MESSAGE_MAP()
};
