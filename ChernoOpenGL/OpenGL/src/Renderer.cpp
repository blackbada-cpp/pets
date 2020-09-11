#include "Renderer.h"

#include <string>
#include <iostream>


void GLClearErrors()
{
   while (glGetError() != GL_NO_ERROR);
}

bool GlLogCall(const char * fun, const char * file, int line)
{
   while (GLenum err = glGetError())
   {
      std::string str;
      switch (err) {
      case GL_INVALID_ENUM:
         str = "GL_INVALID_ENUM";
         //An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.
         break;
      case GL_INVALID_VALUE:
         str = "GL_INVALID_VALUE";
         //A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag.
         break;
      case GL_INVALID_OPERATION:
         str = "GL_INVALID_OPERATION";
         //The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag.
         break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
         str = "GL_INVALID_FRAMEBUFFER_OPERATION";
         //The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.
         break;
      case GL_OUT_OF_MEMORY:
         str = "GL_OUT_OF_MEMORY";
         //There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.
         break;
      case GL_STACK_UNDERFLOW:
         str = "GL_STACK_UNDERFLOW";
         //An attempt has been made to perform an operation that would cause an internal stack to underflow.
         break;
      case GL_STACK_OVERFLOW:
         str = "GL_STACK_OVERFLOW";
         //An attempt has been made to perform an operation that would cause an internal stack to overflow.
         break;
      }
      if (!str.empty())
         std::cout << "[OpenGL Error] (" << str << "): " << fun <<
         " " << file << ":" << line << std::endl;
      else
         std::cout << "[OpenGL Error] (" << err << "): " << fun <<
         " " << file << ":" << line << std::endl;
      return false;
   }
   return true;
}

void Renderer::Clear() const
{
   GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray & va, const IndexBuffer & ib, const Shader & shader) const
{
   shader.Bind();
   //shader.SetUniform4f("u_Color", r, 0.3f, 0.0f, 1.0f);

   va.Bind();
   ib.Bind();

   GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));


}
