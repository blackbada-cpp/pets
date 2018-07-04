#ifndef Plants_h__
#define Plants_h__

#pragma once
#include "World.h"
#include <vector>

class Plant : public WorldObject
{
public:
   static double PLANT_HEIGHT;
   static double PLANT_WIDTH;

public:
   Plant() {}
   Plant(Plant & other) : WorldObject(other) {}
   ~Plant() {}
   virtual WorldObject * Clone() { return new Plant(); }
   void Generate(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth);
   virtual void DoDraw(CDC3D & dc);

protected:
private:
};



#endif Plants_h__
