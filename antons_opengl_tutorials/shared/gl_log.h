#pragma once
#include <string>

namespace gllog
{
   class GLLog
   {
   private:
      std::string m_path;
   public:
      GLLog(const char * path);
      ~GLLog();

      bool Message(const char* message, ...);
      bool Error(const char* message, ...);
      bool VMessage(const char* message, va_list args);
      bool VError(const char* message, va_list args);
      bool LogParams();
   };

   GLLog & Get();
   void Message(const char* message, ...);
   void Error(const char* message, ...);
   void LogParams();
}

