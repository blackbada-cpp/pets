#include "Shader.h"
#include "Renderer.h"

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>


Shader::Shader(const char * filepath)
: m_filepath(filepath), m_rendererId(0)
{
   ShaderSource source = ParseShader(); // relative to $(ProjectDir)
   m_rendererId = CreateShader(source.VertexSource, source.FragmentSource);   
}


Shader::~Shader()
{
   GLCall(glDeleteProgram(m_rendererId));

}

void Shader::Bind() const
{
   GLCall(glUseProgram(m_rendererId));

}

void Shader::Unbind() const
{
   GLCall(glUseProgram(0));

}

void Shader::SetUniform1i(const std::string & name, int value)
{
   GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string & name, float value)
{
   GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string & name, const glm::vec2 & value)
{
   GLCall(glUniform2f(GetUniformLocation(name), value.x, value.y));
}

void Shader::SetUniform3f(const std::string & name, const glm::vec3 & value)
{
   GLCall(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z));
}

void Shader::SetUniform4f(const std::string & name, const glm::vec4 & value)
{
   GLint loc = GetUniformLocation(name);
   if (loc != -1)
      glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void Shader::SetUniform4f(const std::string & name, float v0, float v1, float v2, float v3)
{
   GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string & name, const glm::mat4 & matrix)
{
   GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string & name) const
{
   if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
      return m_uniformLocationCache[name];

   GLCall(int location = glGetUniformLocation(m_rendererId, name.c_str()));
   if (location == -1)
      std::cout << "Warning: Uniform '" << name << "' doesn't exist!" << std::endl;
   else
      m_uniformLocationCache[name] = location;
   return location;
}

ShaderSource Shader::ParseShader()
{
   enum class ShaderType {
      None = -1, Vertex = 0, Fragment = 1
   };
   std::ifstream stream(m_filepath);
   std::string line;
   std::stringstream vertexStream;
   std::stringstream fragmentStream;
   ShaderType type = ShaderType::None;
   while (getline(stream, line))
   {
      if (line.find("#shader") != std::string::npos)
      {
         if (line.find("vertex") != std::string::npos)
            type = ShaderType::Vertex;
         else if (line.find("fragment") != std::string::npos)
            type = ShaderType::Fragment;
      }
      else {
         //read
         if (type == ShaderType::Fragment)
            fragmentStream << line << '\n';
         else if (type == ShaderType::Vertex)
            vertexStream << line << '\n';
      }
   }

   return { vertexStream.str(), fragmentStream.str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string & source)
{
   unsigned int id = glCreateShader(type);
   const char * src = source.c_str();
   glShaderSource(id, 1, &src, nullptr);
   glCompileShader(id);

   int result;
   glGetShaderiv(id, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE) {
      int length;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
      char * message = (char*)alloca(length * sizeof(char));
      glGetShaderInfoLog(id, length, &length, message);
      std::cout << "Failed to compile " <<
         (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
         " shader!" << std::endl;
      std::cout << message << std::endl;
      glDeleteShader(id);
      return 0;
   }
   return id;
}

unsigned int Shader::CreateShader(const std::string & vertexShader, const std::string & fragmentShader)
{
   GLCall(unsigned int program = glCreateProgram());
   unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
   unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
   glAttachShader(program, vs);
   glAttachShader(program, fs);
   glLinkProgram(program);
   glValidateProgram(program);
   glDeleteShader(vs);
   glDeleteShader(fs);
   return program;
}