#include "pch.h"
#include "math.h"
#include "gl_math.h"

//     | 1   0       0    0 |
// Rx= | 0 cos(a) -sin(a) 0 |
//     | 0 sin(a)  cos(a) 0 |
//     | 0   0       0    1 |
void dp::Mat4::SetRotationX(float angle)
{
   Element(1, 1) = cos(angle);
   Element(1, 2) = -sin(angle);
   Element(2, 1) = sin(angle);
   Element(2, 2) = cos(angle);
}

//     | cos(a) 0 sin(a) 0 |
// Ry= |   0    1   0    0 |
//     |-sin(a) 0 cos(a) 0 |
//     |   0    0   0    1 |
void dp::Mat4::SetRotationY(float angle)
{
   Element(0, 0) = cos(angle);
   Element(0, 2) = sin(angle);
   Element(2, 0) = -sin(angle);
   Element(2, 2) = cos(angle);
}

//     |cos(a) -sin(a) 0 0 |
// Rz= |sin(a)  cos(a) 0 0 |
//     |  0       0    1 0 |
//     |  0       0    0 1 |
void dp::Mat4::SetRotationZ(float angle)
{
   Element(0, 0) = cos(angle);
   Element(0, 1) = -sin(angle);
   Element(1, 0) = sin(angle);
   Element(1, 1) = cos(angle);
}
