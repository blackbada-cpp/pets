#pragma once
#include <assert.h>

namespace dp
{
#define ROW_SIZE (4)
#define COL_SIZE (4)

   // Ñolumn-major vector with 4 items, represented as one vertical column
   // | #0 |
   // | #1 |
   // | #2 |
   // | #3 |
   class Vec4
   {
   public:
      float m_data[COL_SIZE];
      Vec4()
      {
         Clear();
      }
      Vec4(const Vec4 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];
      }
      Vec4 & operator = (const Vec4 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];

         return *this;
      }
      inline int Count() { return COL_SIZE; }
      const float & Element(int row) const
      {
         // row = 0..3, col=0..3
         assert(row >= 0 && row < COL_SIZE);
         return m_data[row];
      }
      float & Element(int row) 
      {
         assert(row >= 0 && row < COL_SIZE);
         return m_data[row];
      }
      void Clear()
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = 0;
      }
   };


   // Column-major matrix with 16 items indexed this way:
   // | #0 #4 #8   #12 |
   // | #1 #5 #9   #13 |
   // | #2 #6 #10  #14 |
   // | #3 #7 #11  #15 |
   class Mat4
   {
   public:
      float m_data[ROW_SIZE * COL_SIZE];

   public:
      Mat4() 
      {
         Clear();
      }
      Mat4(const Mat4 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];
      }
      Mat4 & operator = (const Mat4 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];
         
         return *this;
      }
      operator float*() 
      {
         return &m_data[0];
      }
      operator const float*() const
      {
         return &m_data[0];
      }


      //////////////////////////////////////////////////////////////////////////
   public:

      void Clear()
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = 0;
      }

      inline int Count() { return ROW_SIZE * COL_SIZE; }
      float & Element(int row, int col)
      {
         // row = 0..3, col=0..3
         assert(row >= 0 && row < COL_SIZE);
         assert(col >= 0 && col < ROW_SIZE);
         return m_data[col*COL_SIZE + row];
      }
      const float & Element(int row, int col) const 
      {
         // row = 0..3, col=0..3
         assert(row >= 0 && row < COL_SIZE);
         assert(col >= 0 && col < ROW_SIZE);
         return m_data[col*COL_SIZE + row];
      }

      // Matrix * Vector in column-major form
      // | #0 #4 #8   #12 |
      // | #1 #5 #9   #13 |
      // | #2 #6 #10  #14 |
      // | #3 #7 #11  #15 |
      // | a b c d |  | x |    |ax + by + cz + dw|
      // | e f g h |  | y |  = |ex + fy + gz + hw|
      // | i j k l |  | z |    |ix + jy + kz + lw|
      // | m n o p |  | w |    |mx + ny + oz + pw|
      Vec4 operator *(const Vec4 & other)
      {
         Vec4 res;

         for (int irow = 0; irow < COL_SIZE; irow++)
         {
            res.Element(irow) = 0.0;
            for (int icol = 0; icol < ROW_SIZE; icol++)
            {
               res.Element(irow) += Element(irow, icol) * other.Element(icol);
            }
         }
         return res; //copy
      }

      Mat4 operator *(const Mat4 & other)
      {
         Mat4 res;

         for (int irow = 0; irow < COL_SIZE; irow++) //i
         {
            for (int icol = 0; icol < ROW_SIZE; icol++) //j
            {
               res.Element(irow, icol) = 0.0;
               for (int i = 0; i < ROW_SIZE; i++) //1,2,3,4
               {
                  //from https://en.wikipedia.org/wiki/Matrix_multiplication
                  // Cij = Ai1*B1j + Ai2*B2j + Ai3*B3j + Ai4*B4j
                  res.Element(irow, icol) += Element(irow, i) * other.Element(i, icol);
               }
            }
         }
         return res; //copy
      }

   private:
      static Mat4 GetIdentity()
      {
         Mat4 res;
         for (int i = 0; i < ROW_SIZE; i++)
            res.Element(i, i) = 1.0;
         return res;
      }

   public:
      static Mat4 Identity()
      {
         static Mat4 res = GetIdentity();
         return res;
      }

      //////////////////////////////////////////////////////////////////////////
      void SetTranslation(float x, float y, float z)
      {
         TranslationX() = x;
         TranslationY() = y;
         TranslationZ() = z;
      }
      inline float & TranslationX() { return Element(0, 3); }
      inline float & TranslationY() { return Element(1, 3); }
      inline float & TranslationZ() { return Element(2, 3); }
      static Mat4 Translation(float x, float y, float z)
      {
         Mat4 res;
         res = Identity();
         res.SetTranslation(x, y, z);
         return res;
      }

      //////////////////////////////////////////////////////////////////////////
      void SetRotationX(float angle);
      void SetRotationY(float angle);
      void SetRotationZ(float angle);
      static Mat4 RotationX(float angle)
      {
         Mat4 res;
         res = Identity();
         res.SetRotationX(angle);
         return res;
      }
      static Mat4 RotationY(float angle)
      {
         Mat4 res;
         res = Identity();
         res.SetRotationY(angle);
         return res;
      }
      static Mat4 RotationZ(float angle)
      {
         Mat4 res;
         res = Identity();
         res.SetRotationZ(angle);
         return res;
      }
   };

}