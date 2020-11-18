#include "pch.h"
#include "stdio.h"
#include "gl_util.h"
#include "gl_log.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

void _update_fps_counter(GLFWwindow* window) 
{
   static double previous_seconds = glfwGetTime();
   static int frame_count;
   double current_seconds = glfwGetTime();
   double elapsed_seconds = current_seconds - previous_seconds;
   if (elapsed_seconds > 0.25) {
      previous_seconds = current_seconds;
      double fps = (double)frame_count / elapsed_seconds;
      char tmp[128];
      sprintf(tmp, "opengl @ fps: %.2f", fps);
      glfwSetWindowTitle(window, tmp);
      frame_count = 0;
   }
   frame_count++;
}

void glfw_error_callback(int error, const char* description)
{
   gllog::Get().Error("GLFW ERROR: code %i msg: %s\n", error, description);
}


GLFWwindow* gl_init()
{
   // start GL context and O/S window using the GLFW helper library
   gllog::Get().Message("starting GLFW\n%s\n", glfwGetVersionString());
   // register the error call-back function that we wrote, above
   glfwSetErrorCallback(glfw_error_callback);
   if (!glfwInit()) {
      gllog::Get().Error("ERROR: could not start GLFW3\n");
      return NULL;
   }

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   glfwWindowHint(GLFW_SAMPLES, 4);

   //#define FULL_SCREEN
#ifdef FULL_SCREEN
   GLFWmonitor* mon = glfwGetPrimaryMonitor();
   const GLFWvidmode* vmode = glfwGetVideoMode(mon);
   GLFWwindow* window = glfwCreateWindow(vmode->width, vmode->height, "Extended GL Init", mon, NULL);
#else
   GLFWwindow* window = glfwCreateWindow(800, 600, "Extended GL Init", NULL, NULL);
#endif
   if (!window) {
      gllog::Get().Error("ERROR: could not open window with GLFW3\n");
      glfwTerminate();
      return NULL;
   }
   glfwMakeContextCurrent(window);
   // start GLEW extension handler
   glewExperimental = GL_TRUE;
   glewInit();

   // get version info
   const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
   const GLubyte* version = glGetString(GL_VERSION); // version as a string
   gllog::Get().Message("Renderer: %s\n", renderer);
   gllog::Get().Message("OpenGL version supported %s\n", version);

   // tell GL to only draw onto a pixel if the shape is closer to the viewer
   glEnable(GL_DEPTH_TEST); // enable depth-testing
   glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

   glfwSetWindowSizeCallback(window, glfw_window_size_callback);
   
   glEnable(GL_CULL_FACE); // cull face
   glCullFace(GL_BACK); // cull back face
   glFrontFace(GL_CW); // GL_CCW for counter clock-wise


   gllog::Get().LogParams();
   return window;
}

// keep track of window size for things like the viewport and the mouse cursor
int g_gl_width = 640;
int g_gl_height = 480;

void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
   g_gl_width = width;
   g_gl_height = height;

   /* update any perspective matrices used here */
}