#include "stdafx.h"
#include "World.h"

WorldObject::WorldObject()
{

}

WorldObject::WorldObject(POINT3D &pos, SIZE3D &size)
: m_pos(pos)
, m_size(size)
{
}

WorldObject::WorldObject(int x, int y, int z, int w, int h, int d)
: m_pos(x, y, z)
, m_size(w, h, d)
{

}


//////////////////////////////////////////////////////////////////////////
World::~World()
{
}

void World::Add(WorldObject * obj)
{
   insert(std::pair<int, WorldObject *>(obj->m_pos.z, obj));
}

void World::DoDraw(CDC3D & dc)
{
   std::multimap<int, WorldObject *>::reverse_iterator ir;
   std::multimap<int, WorldObject *>::const_reverse_iterator end;
   for (ir = rbegin(), end = rend(); ir != end; ++ir)
   {
      WorldObject * obj = (*ir).second;
      obj->DoDraw(dc);
   }
}
