
// MFCApplication1Dlg.h : header file
//

#pragma once
#include "DC3D.h"

class CDC3D;

#define STAR_COUNT (10000)
#define MAX_PERCENT (100.0)
#define D_PERCENT   (2.0)

class StarSky
{
public:
   POINT3D *m_stars;
   int m_starsCount;

public:
   StarSky(int starsCount);
   ~StarSky();
   void Init(RECT * prc, double z_limit);
   inline void InitStar(POINT3D & star, RECT * prc, double z_limit);
   void Draw(CDC3D & dc, double m_cameraDepth);
   void MoveStars(RECT * prc, double dz, double z_limit);
};


// CAnimationWindowStars dialog
class CAnimationWindowStars : public CWnd
{
// Construction
public:
	CAnimationWindowStars(CWnd* pParent = NULL);	// standard constructor


// Implementation
protected:
	HICON m_hIcon;
   int m_cameraDepth;


   //const int m_starsSpeed = 4;
   //const int m_starsSpeed = 40;
   const int m_starsSpeed = 100;
   const int m_starsDepthDelta  = 200;
   //const int m_starsDepthLimit = 2000;
   const int m_starsDepthLimit = 20000;
   double m_planeWidth, m_planeHeight;
   
   enum KeyPressed
   {
      key_Nothing,
      key_Left,
      key_Right,
      key_Up,
      key_Down
   }; 
   KeyPressed m_pressed;
   double m_cxPercent;
   double m_cyPercent;

   StarSky m_sky;
   void DrawStars(CDC3D & dc, RECT* prc);

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
