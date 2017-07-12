
// MFCApplication1Dlg.h : header file
//

#pragma once

class CDC3D;

#define MAX_PERCENT (100.0)
#define D_PERCENT   (2.0)

// CAnimationWindow dialog
class CAnimationWindow : public CWnd
{
// Construction
public:
	CAnimationWindow(CWnd* pParent = NULL);	// standard constructor


// Implementation
protected:
	HICON m_hIcon;
   int m_cameraDepth;
   enum KeyPressed
   {
      key_Nothing,
      key_Left,
      key_Right,
      key_Up,
      key_Down
   };
   KeyPressed m_pressed;
   double m_cxPercent, m_cyPercent;

   const int m_tonnelSpeed = 4;
   const int m_tonnelDepthDelta = 200;
   const int m_tonnelDepthEnd   = 20000;
   double m_planeWidth, m_planeHeight;
   void DrawTonnel(CDC3D & dc, RECT* prc);

protected:
   // Generated message map functions
   afx_msg int  OnCreate(LPCREATESTRUCT);
   afx_msg void OnDestroy();
   afx_msg void OnPaint();
   afx_msg void OnTimer(UINT_PTR);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);


   DECLARE_MESSAGE_MAP()
};
