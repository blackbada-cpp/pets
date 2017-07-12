
// MFCApplication1Dlg.h : header file
//

#pragma once
#include "World.h"
#include "House.h"
#include <vector>


#define CAMERA_SPEED (4.0)
#define CAMERA_SPEED_MAX (500.0)
#define CAMERA_SPEED_MIN (0.0)
#define CAMERA_SPEED_INCREASE (4.0)

//In houses
#define DETAILIZATION_MEDIUM (25)
#define DETAILIZATION_LOW    (50)

#define HOUSE_COUNT (300)
#define BALL_COUNT (40)

#define X_MIN_PERCENT (-50.0)
#define X_INIT_PERCENT (0.0)
#define X_MAX_PERCENT (50.0)

//Upper limit of flight
//#define Y_MIN_PERCENT (-100.0)
#define Y_MIN_PERCENT (-1000.0)

#define Y_INIT_PERCENT (0.0)
#define Y_MAX_PERCENT (0.0)

#define D_PERCENT   (2.0)

//http://www.colourlovers.com/palette/264688/Grass_Green
#define COLOR_GRASSS_MEADOW         RGB(0,92,9)
#define COLOR_GRASSS_RIVER_BANK     RGB(0, 104, 10)
#define COLOR_GRASSS_HILL           RGB(0,123,12)
#define COLOR_GRASSS_FOOT_HILL      RGB(1,142,14)
#define COLOR_GRASSS_BLADE_OF_GRASS RGB(1,166,17)


class CDC3D;

class BitmapObject : public WorldObject
{
private:
   CBitmapObject & m_bitmap;

public:
   BitmapObject(CBitmapObject & m_bitmap);
   virtual void DoDraw(CDC3D & dc);
};

class  City
{
public:
   std::vector<House> m_housesLeft;
   std::vector<House> m_housesRight;
   //int m_houseCount;

   CBitmapObject m_ballBitmap;
   std::vector<BitmapObject> m_balls;

public:
   City(int houseCount);
   ~City();

   void Init(CRect & rc, double z_houseStep, double groundHeight);
   void Draw(CDC3D & dc);
   void MoveHouses(RECT * prc, double dz, int houseCount, double z_houseStep, double z_camera);
};

class Road
{
public:
   double m_zStart;
   double m_Length;
   double m_LineLength;
   double m_LineStep; // = m_LineLength + gap
   double m_LineWidth;
   double m_houseStep;
   double m_LineX;

public:
   Road(double length, double houseStep);
   ~Road();

   void Init(CRect & rc);
   void Draw(CDC3D & dc, RECT* prc);
   void MoveForward(RECT * prc, double dz, double z_camera);
};

// CAnimationWindow dialog
class CAnimationWindowStreet : public CWnd
{
// Construction
public:
   CAnimationWindowStreet(CWnd* pParent = NULL);	// standard constructor
   ~CAnimationWindowStreet();


// Implementation
protected:
	HICON m_hIcon;
   double m_z_camera;
   double m_planeWidth;
   double m_planeHeight;
   double m_cxPercent;
   double m_cyPercent;

   double m_cameraSpeed;
   const double m_z_houseStep      = 800.0; //Step between front wall of 2 houses 
   //const double m_tonnelDepthEnd   = 18000.0;
   //const double m_tonnelDepthEnd = 8000.0;
   //const double m_tonnelDepthEnd = 800.0;

   //World m_world;
   City m_city;
   Road m_road;

   enum KeyPressed
   {
      key_Nothing,
      key_Left,
      key_Right,
      key_Up,
      key_Down,
      key_Plus,
      key_Minus
   };
   KeyPressed m_pressed;

   void DrawStreet(CDC3D & dc, RECT* prc);
   static LONG GetGroundHeight(RECT* prc);

protected:
   // Generated message map functions
   afx_msg int  OnCreate(LPCREATESTRUCT);
   afx_msg void OnDestroy();
   afx_msg void OnPaint();
   afx_msg void OnTimer(UINT_PTR);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnKeyUp(UINT nChar,UINT nRepCnt,UINT nFlags);

   DECLARE_MESSAGE_MAP()
};
