#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

struct ShaderSource
{
   std::string VertexSource;
   std::string FragmentSource;
};


class Shader
{
private:
   unsigned int m_rendererId;
   std::string m_filepath;
   //cache for uniforms
   std::unordered_map<std::string, int> m_uniformLocationCache;
public:
   Shader(const char * filepath);
   ~Shader();

   void Bind() const;
   void Unbind() const;

   //Set Uniforms
   void SetUniform1i(const std::string & name, int value);
   void SetUniform1f(const std::string & name, float value);
   void SetUniform4f(const std::string & name, float v0, float v1, float v2, float v3);
   void SetUniformMat4f(const std::string & name, const glm::mat4 &);

private:
   unsigned int CompileShader(unsigned int type, const std::string & source);
   ShaderSource ParseShader();
   unsigned int CreateShader(const std::string & vertexShader, const std::string & fragmentShader);
   int GetUniformLocation(const std::string & name);
};