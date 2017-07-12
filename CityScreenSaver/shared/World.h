#ifndef World_h__
#define World_h__

#pragma once

#include <map>
#include "DC3D.h"

class WorldObject
{
public:
   //Note: x and y follow screen coordinates:
   //
   // o-----> x
   // |
   // |
   // |
   // v y
   //z - is depth into the screen, distance from the screen into 3d world
   POINT3D m_pos;
   SIZE3D  m_size;

public:
   WorldObject();
   WorldObject(POINT3D &pos, SIZE3D &size);
   WorldObject(int x, int y, int z, int w, int h, int d);

   void SetPos(POINT3D &pos) { m_pos = pos; };
   void SetPos(int x, int y, int z) { m_pos.x = x; m_pos.y = y; m_pos.z = z; }
   const POINT3D & GetPos() const { return m_pos; }

   void SetSize(SIZE3D &sz) { m_size = sz; };
   void SetSize(int w, int h, int d) { m_size.w = w; m_size.h = h; m_size.d = d; }
   const SIZE3D & GetSize() const { return m_size; }
public:
   virtual void DoDraw(CDC3D & dc) = 0;
};


//////////////////////////////////////////////////////////////////////////
//First draws far-end objects, then near end objects, providing hand-made "hiding" of hidden surfaces
class World : public std::multimap<int, WorldObject *>
{
public:
   
   World() {}
   ~World(); //Destroys objects

   void Add(WorldObject *);
   void DoDraw(CDC3D & dc); // Draws objects from depth to closest space
};

#endif // World_h__