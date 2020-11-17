#include "pch.h"
#include "gl_log.h"
#include <memory>
#include "time.h"
#include "stdarg.h"
#include "GL/glew.h"

#define GL_LOG_FILE "gl.log"


namespace gllog
{
   GLLog & Get()
   {
      static std::shared_ptr<GLLog> p_log;
      if (p_log)
         return *p_log;
      p_log = std::make_shared<GLLog>(GL_LOG_FILE);
      return *p_log;
   }

   GLLog::GLLog(const char * path)
      :m_path(path)
   {
      FILE* file = fopen(path, "w");
      if (!file) {
         fprintf(stderr,
            "ERROR: could not open GL_LOG_FILE log file %s for writing\n",
            path);
         throw std::exception();
      }
      time_t now = time(NULL);
      char* date = ctime(&now);
      fprintf(file, "GL_LOG_FILE log. local time %s\n", date);
      fclose(file);
   }

   GLLog::~GLLog()
   {
      ;
   }

   bool GLLog::Message(const char* message, ...)
   {
      va_list argptr;
      FILE* file = fopen(m_path.c_str(), "a");
      if (!file) {
         fprintf(stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", m_path.c_str());
         return false;
      }
      va_start(argptr, message);
      vfprintf(file, message, argptr);
      va_end(argptr);
      fclose(file);
      return true;
   }
   bool GLLog::VMessage(const char* message, va_list args)
   {
      FILE* file = fopen(m_path.c_str(), "a");
      if (!file) {
         fprintf(stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", m_path.c_str());
         return false;
      }
      vfprintf(file, message, args);
      fclose(file);
      return true;
   }

   bool GLLog::Error(const char* message, ...)
   {
      va_list argptr;
      FILE* file = fopen(m_path.c_str(), "a");
      if (!file) {
         fprintf(stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", m_path.c_str());
         return false;
      }
      va_start(argptr, message);
      vfprintf(file, message, argptr);
      va_end(argptr);
      va_start(argptr, message);
      vfprintf(stderr, message, argptr);
      va_end(argptr);
      fclose(file);
      return true;
   }

   bool GLLog::VError(const char* message, va_list args)
   {
      FILE* file = fopen(m_path.c_str(), "a");
      if (!file) {
         fprintf(stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", m_path.c_str());
         return false;
      }
      vfprintf(file, message, args);
      vfprintf(stderr, message, args);
      fclose(file);
      return true;
   }

   bool GLLog::LogParams()
   {
      GLenum params[] = {
         GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
         GL_MAX_CUBE_MAP_TEXTURE_SIZE,
         GL_MAX_DRAW_BUFFERS,
         GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
         GL_MAX_TEXTURE_IMAGE_UNITS,
         GL_MAX_TEXTURE_SIZE,
         GL_MAX_VARYING_FLOATS,
         GL_MAX_VERTEX_ATTRIBS,
         GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
         GL_MAX_VERTEX_UNIFORM_COMPONENTS,
         GL_MAX_VIEWPORT_DIMS,
         GL_STEREO,
      };
      const char* names[] = {
         "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
         "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
         "GL_MAX_DRAW_BUFFERS",
         "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
         "GL_MAX_TEXTURE_IMAGE_UNITS",
         "GL_MAX_TEXTURE_SIZE",
         "GL_MAX_VARYING_FLOATS",
         "GL_MAX_VERTEX_ATTRIBS",
         "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
         "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
         "GL_MAX_VIEWPORT_DIMS",
         "GL_STEREO",
      };
      Message("GL Context Params:\n");
      //char msg[256];
      // integers - only works if the order is 0-10 integer return types
      for (int i = 0; i < 10; i++) {
         int v = 0;
         glGetIntegerv(params[i], &v);
         Message("%s %i\n", names[i], v);
      }
      // others
      int v[2];
      v[0] = v[1] = 0;
      glGetIntegerv(params[10], v);
      Message("%s %i %i\n", names[10], v[0], v[1]);
      unsigned char s = 0;
      glGetBooleanv(params[11], &s);
      Message("%s %u\n", names[11], (unsigned int)s);
      Message("-----------------------------\n");
      return true;
   }

   void Message(const char* message, ...)
   {
      va_list args;
      va_start(args, message);
      Get().VMessage(message, args);
      va_end(args);
   }
   
   void Error(const char* message, ...)
   {
      va_list args;
      va_start(args, message);
      Get().VError(message, args);
      va_end(args);
   }
   
   void LogParams()
   {
      Get().LogParams();
   }
}
