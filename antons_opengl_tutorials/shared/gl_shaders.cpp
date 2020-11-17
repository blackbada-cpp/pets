#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include "gl_shaders.h"
#include "gl_log.h"
#include "GL/glew.h"

std::string LoadShader(const char * path)
{
   std::ifstream infile(path);
   std::string line;
   std::string shader;
   while (getline(infile, line))
   {
      shader += line;
      shader += "\n";
   }
   return shader;
}

void _print_shader_info_log(GLuint shader_index)
{
   int max_length = 2048;
   int actual_length = 0;
   char shader_log[2048];
   glGetShaderInfoLog(shader_index, max_length, &actual_length, shader_log);
   gllog::Get().Message("shader info log for GL index %u:\n%s\n", shader_index, shader_log);
}

bool _log_shader_error(GLuint vs)
{
   int params = -1;
   glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
   if (GL_TRUE != params) {
      gllog::Get().Error("ERROR: GL shader index %i did not compile\n", vs);
      _print_shader_info_log(vs);
      return false;
   }
   return true;
}

void _print_programme_info_log(GLuint programme)
{
   int max_length = 2048;
   int actual_length = 0;
   char program_log[2048];
   glGetProgramInfoLog(programme, max_length, &actual_length, program_log);
   gllog::Get().Message("program info log for GL index %u:\n%s", programme, program_log);
}

bool _log_programme_error(GLuint programme)
{
   // check if link was successful
   int params = -1;
   glGetProgramiv(programme, GL_LINK_STATUS, &params);
   if (GL_TRUE != params) {
      gllog::Get().Error("ERROR: could not link shader programme GL index %u\n", programme);
      _print_programme_info_log(programme);
      return false;
   }
   return true;
}

const char* GL_type_to_string(GLenum type) {
   switch (type) {
   case GL_BOOL: return "bool";
   case GL_INT: return "int";
   case GL_FLOAT: return "float";
   case GL_FLOAT_VEC2: return "vec2";
   case GL_FLOAT_VEC3: return "vec3";
   case GL_FLOAT_VEC4: return "vec4";
   case GL_FLOAT_MAT2: return "mat2";
   case GL_FLOAT_MAT3: return "mat3";
   case GL_FLOAT_MAT4: return "mat4";
   case GL_SAMPLER_2D: return "sampler2D";
   case GL_SAMPLER_3D: return "sampler3D";
   case GL_SAMPLER_CUBE: return "samplerCube";
   case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
   default: break;
   }
   return "other";
}

void _print_all(GLuint programme) 
{
   gllog::Get().Message("--------------------\nshader programme %i info:\n", programme);
   int params = -1;
   glGetProgramiv(programme, GL_LINK_STATUS, &params);
   gllog::Get().Message("GL_LINK_STATUS = %i\n", params);

   glGetProgramiv(programme, GL_ATTACHED_SHADERS, &params);
   gllog::Get().Message("GL_ATTACHED_SHADERS = %i\n", params);

   glGetProgramiv(programme, GL_ACTIVE_ATTRIBUTES, &params);
   gllog::Get().Message("GL_ACTIVE_ATTRIBUTES = %i\n", params);
   for (int i = 0; i < params; i++) {
      char name[64];
      int max_length = 64;
      int actual_length = 0;
      int size = 0;
      GLenum type;
      glGetActiveAttrib(
         programme,
         i,
         max_length,
         &actual_length,
         &size,
         &type,
         name
      );
      if (size > 1) {
         for (int j = 0; j < size; j++) {
            char long_name[64];
            sprintf(long_name, "%s[%i]", name, j);
            int location = glGetAttribLocation(programme, long_name);
            gllog::Get().Message("  %i) type:%s name:%s location:%i\n",
               i, GL_type_to_string(type), long_name, location);
         }
      }
      else {
         int location = glGetAttribLocation(programme, name);
         gllog::Get().Message("  %i) type:%s name:%s location:%i\n",
            i, GL_type_to_string(type), name, location);
      }
   }

   glGetProgramiv(programme, GL_ACTIVE_UNIFORMS, &params);
   gllog::Get().Message("GL_ACTIVE_UNIFORMS = %i\n", params);
   for (int i = 0; i < params; i++) {
      char name[64];
      int max_length = 64;
      int actual_length = 0;
      int size = 0;
      GLenum type;
      glGetActiveUniform(
         programme,
         i,
         max_length,
         &actual_length,
         &size,
         &type,
         name
      );
      if (size > 1) {
         for (int j = 0; j < size; j++) {
            char long_name[64];
            sprintf(long_name, "%s[%i]", name, j);
            int location = glGetUniformLocation(programme, long_name);
            gllog::Message("  %i) type:%s name:%s location:%i\n",
               i, GL_type_to_string(type), long_name, location);
         }
      }
      else {
         int location = glGetUniformLocation(programme, name);
         gllog::Message("  %i) type:%s name:%s location:%i\n",
            i, GL_type_to_string(type), name, location);
      }
   }

   _print_programme_info_log(programme);
}

//Validate programme
bool _is_programme_valid(GLuint programme) 
{
   glValidateProgram(programme);
   int params = -1;
   glGetProgramiv(programme, GL_VALIDATE_STATUS, &params);
   gllog::Get().Message("program %i GL_VALIDATE_STATUS = %i\n", programme, params);
   if (GL_TRUE != params) 
   {
      _print_programme_info_log(programme);
      return false;
   }
   return true;
}


bool InitShaderProgram(const char * vertex_shader_path, const char * fragment_shader_path, GLuint & shader_program)
{
   std::string vertex_shader = LoadShader(vertex_shader_path);
   const char * vertex_shader_str = vertex_shader.c_str();
   std::string fragment_shader = LoadShader(fragment_shader_path);
   const char * fragment_shader_str = fragment_shader.c_str();

   GLuint vs = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vs, 1, &vertex_shader_str, NULL);
   glCompileShader(vs);
   if (!_log_shader_error(vs))
      return false;// check for compile errors
   _print_shader_info_log(vs);

   GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fs, 1, &fragment_shader_str, NULL);
   glCompileShader(fs);
   if (!_log_shader_error(fs))
      return false;// check for compile errors
   _print_shader_info_log(fs);

   shader_program = glCreateProgram();
   glAttachShader(shader_program, fs);
   glAttachShader(shader_program, vs);
   return true;
}

bool LinkShaderProgram(GLuint shader_program)
{
   glLinkProgram(shader_program);

   if (!_log_programme_error(shader_program))
      return false;
   _print_all(shader_program);
   return true;
}
