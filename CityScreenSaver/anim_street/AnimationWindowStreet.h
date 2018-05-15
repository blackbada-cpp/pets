
// MFCApplication1Dlg.h : header file
//

#pragma once
#include "World.h"
#include "House.h"
#include <vector>

#define CELL_DEPTH (800.0)
#define HOUSE_COUNT (100)
#define CITY_DEPTH (CELL_DEPTH*HOUSE_COUNT)
#define BALL_COUNT (40)

//Camera settings
#define CAMERA_SPEED (4.0)
#define CAMERA_SPEED_MAX (500.0)
#define CAMERA_SPEED_MIN (0.0)
#define CAMERA_SPEED_INCREASE (4.0)
#define Z_CAMERA_INIT (0.0)
//See only the depth of 1 city
#define Z_CAMERA_CUTOFF (CELL_DEPTH*HOUSE_COUNT)

//In houses
#define DETAILIZATION_MEDIUM (25)
#define DETAILIZATION_LOW    (50)


#define X_MIN_PERCENT (-50.0)
#define X_INIT_PERCENT (0.0)
#define X_MAX_PERCENT (50.0)

//Upper limit of flight
//#define Y_MIN_PERCENT (-100.0)
#define Y_MIN_PERCENT (-1000.0)

#define Y_INIT_PERCENT (0.0)
#define Y_MAX_PERCENT (0.0)

#define D_PERCENT   (2.0)

#define COLOR_ASPHALT RGB(0x28, 0x2B, 0x2A)

//http://www.rapidtables.com/web/color/RGB_Color.htm
//#define FRONT_COLOR_1 RGB(255, 165, 0),  //orange
//#define FRONT_COLOR_1 RGB(255, 235, 205),//
//#define FRONT_COLOR_1 RGB(107, 142, 35), //olive drab
//#define FRONT_COLOR_1 RGB(64, 224, 208), //turquoise
//
//#define SIDE_COLOR_1 RGB(255, 140, 0),  //dark orange
//#define SIDE_COLOR_1 RGB(245, 222, 179),//
//#define SIDE_COLOR_1 RGB(85, 107, 47),  //dark olive green
//#define SIDE_COLOR_1 RGB(0, 206, 209),  //dark turquoise

//http://www.colourlovers.com/palette/264688/Grass_Green
#define COLOR_GRASSS_MEADOW         RGB(0,92,9)
#define COLOR_GRASSS_RIVER_BANK     RGB(0, 104, 10)
#define COLOR_GRASSS_HILL           RGB(0,123,12)
#define COLOR_GRASSS_FOOT_HILL      RGB(1,142,14)
#define COLOR_GRASSS_BLADE_OF_GRASS RGB(1,166,17)

//http://www.colourlovers.com/palette/758733/Art_Glass_Screen
#define COLOR_WINDOW_1 RGB(254, 252, 174)
#define COLOR_WINDOW_2 RGB(17, 53, 105)
#define COLOR_WINDOW_3 RGB(175, 192, 98)
#define COLOR_WINDOW_4 RGB(85, 147, 48)
#define COLOR_WINDOW_5 RGB(2, 119, 98)


#define MOVE_CAMERA

class CDC3D;


//////////////////////////////////////////////////////////////////////////
class BitmapObject : public WorldObject
{
private:
   CBitmapObject & m_bitmap;

public:
   BitmapObject(BitmapObject & other);
   BitmapObject(CBitmapObject & m_bitmap);
   virtual void DoDraw(CDC3D & dc);
};

//////////////////////////////////////////////////////////////////////////
class Ball : public BitmapObject
{
public:
   Ball();
   Ball(Ball & other);
   virtual WorldObject * Clone();
   void GenerateBall(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth);
};

//////////////////////////////////////////////////////////////////////////
//TODO: class Road : public WorldObject
class Road : public WorldObject
{
public:
   double m_LineLength;
   double m_LineStep; // = m_LineLength + gap
   double m_LineWidth;
   double m_cellDepth;

public:
   Road();
   Road(Road & other);
   ~Road();
   virtual WorldObject * Clone();

   void Init(CRect & rc, double zPos, double groundHeight, double depth, double cellDepth);

   virtual void DoDraw(CDC3D & dc);
};

//////////////////////////////////////////////////////////////////////////
typedef std::vector<WorldObject*> WorldObjectRow;


class  City
{
public:
   POINT3D m_pos;
   int    m_cellCount;
   double m_cellDepth;

   Road m_road;
   WorldObjectRow               m_objects;
   std::vector<WorldObjectRow*> m_rows;

   std::vector<COLORREF> * m_frontColors;
   std::vector<COLORREF> * m_sideColors;

public:
   City();
   ~City();

   double GetCityDepth() const { return m_cellCount *m_cellDepth; }
   void CopyFrom(City &other);
   void Init(CRect & rc, double z, int rowCount, int cellCount, double cellDepth, double groundHeight, int maxFloorNumber, std::vector<COLORREF> & frontColors, std::vector<COLORREF> & sideColors, HouseStyle style);
   void PrepareDraw(World & world, double cameraZPos, double cameraCutOff);
   
#ifndef MOVE_CAMERA
   //Move city with camera flip
   void MoveObjects(std::vector<WorldObject*> &row, double dz, double z_houseStep, double z_camera);
   void MoveObjects(double dz, double z_houseStep, double z_camera);
#endif

   //Move city
   void MoveObjects(double dz);

protected:
   inline void PrepareDraw(World & world, WorldObject*, double cameraZPos, double cameraCutOff);
   void MoveObjects(std::vector<WorldObject*> &row, double dz);
   void CopyRow(std::vector<WorldObject*> &row, std::vector<WorldObject*> &rowFrom);
   void CreateRow(std::vector<WorldObject*> &row, int iStart, int iEnd, int housePercent, HouseStyle style);
   void DestroyRow(std::vector<WorldObject*> &row);
   void InitCellRow(std::vector<WorldObject*> &row, int cellXPos, double groundHeight, double cellWidth, double cellDepth, int maxFloorNumber);

};


//////////////////////////////////////////////////////////////////////////
// Main window
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
   double m_z_cameraCutOff;
   double m_planeWidth;
   double m_planeHeight;
   double m_cxPercent;
   double m_cyPercent;

   double m_cameraSpeed;
#ifdef MOVE_CAMERA
   double m_worldWrapDepth;
#endif
   const double m_cellDepth      = CELL_DEPTH; //Step between front wall of 2 houses 

   std::vector<City*> m_regions;
   std::vector<COLORREF>  m_front1;
   std::vector<COLORREF>  m_side1 ;
   std::vector<COLORREF>  m_front2;
   std::vector<COLORREF>  m_side2 ;
   std::vector<COLORREF>  m_front3;
   std::vector<COLORREF>  m_side3;

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
