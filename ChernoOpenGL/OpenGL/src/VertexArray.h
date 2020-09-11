#pragma once 
#include "VertexBuffer.h"

class VertexBufferLayout;
class VertexBuffer;

class VertexArray
{
private:
   unsigned int m_rendererID;
public:
   VertexArray();
   ~VertexArray();

   void AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout);
   void Bind() const;
   void Unbind() const;
};