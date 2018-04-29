
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

#define HOUSE_COUNT (100)
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


//http://www.colourlovers.com/palette/896737/City_Sunset
#define COLOR_CITY_SUNSET_1 RGB(255, 171, 3)
#define COLOR_CITY_SUNSET_2 RGB(252, 127, 3)
#define COLOR_CITY_SUNSET_3 RGB(252, 57, 3)
#define COLOR_CITY_SUNSET_4 RGB(209, 2, 78)
#define COLOR_CITY_SUNSET_5 RGB(166, 2, 108)

//http://www.colourlovers.com/palette/1256070/Sunset_slowly
#define COLOR_CITY_SUNSET_SIDE_1 RGB(245, 159, 50)
#define COLOR_CITY_SUNSET_SIDE_2 RGB(252, 143, 40)
#define COLOR_CITY_SUNSET_SIDE_3 RGB(255, 95, 45)
#define COLOR_CITY_SUNSET_SIDE_4 RGB(235, 61, 62)
#define COLOR_CITY_SUNSET_SIDE_5 RGB(196, 35, 68)

//http://www.colourlovers.com/palette/758733/Art_Glass_Screen
#define COLOR_WINDOW_1 RGB(254, 252, 174)
#define COLOR_WINDOW_2 RGB(17, 53, 105)
#define COLOR_WINDOW_3 RGB(175, 192, 98)
#define COLOR_WINDOW_4 RGB(85, 147, 48)
#define COLOR_WINDOW_5 RGB(2, 119, 98)
class CDC3D;


class BitmapObject : public WorldObject
{
private:
   CBitmapObject & m_bitmap;

public:
   BitmapObject(CBitmapObject & m_bitmap);
   virtual void DoDraw(CDC3D & dc);
};

class Ball : public BitmapObject
{
public:
   Ball();
   void GenerateBall(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth);
};

class  City
{
public:
   int m_houseCount;

   std::vector<WorldObject*> m_leftRow1;
   std::vector<WorldObject*> m_leftRow2;
   std::vector<WorldObject*> m_leftRow3;
   std::vector<WorldObject*> m_rightRow1;
   std::vector<WorldObject*> m_rightRow2;
   std::vector<WorldObject*> m_rightRow3;
   //int m_houseCount;

   std::vector<WorldObject*> m_balls;

public:
   City(int houseCount);
   ~City();

   void Init(CRect & rc, double z_houseStep, double groundHeight);
   void Draw(CDC3D & dc);
   void MoveObjects(std::vector<WorldObject*> &row, double dz, double z_houseStep, double z_camera);
   void MoveObjects(double dz, double z_houseStep, double z_camera);

protected:
   void CreateRow(std::vector<WorldObject*> &row);
   void DestroyRow(std::vector<WorldObject*> &row);
   void InitCellRow(std::vector<WorldObject*> &row, int cellXPos, double groundHeight, double cellWidth, double cellDepth);

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

extern double RangedRand(int range_min, int range_max);
extern int RangedRandInt(int range_min, int range_max);
