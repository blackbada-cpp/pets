#pragma once

#include <GL/glew.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearErrors();\
   x;\
   ASSERT(GlLogCall(#x, __FILE__, __LINE__))
void GLClearErrors();

bool GlLogCall(const char * fun, const char * file, int line);

class Renderer
{
public:
   void Clear() const;
   void Draw(const VertexArray & va, const IndexBuffer & ib, const Shader & shader) const;
};