#include "pch.h"
#include "stdio.h"
#include "gl_util.h"
#include "gl_log.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#define DEFAULT_WINDOW_WIDTH (800)
#define DEFAULT_WINDOW_HEIGHT (800)
int g_gl_framebuffer_width = DEFAULT_WINDOW_WIDTH;
int g_gl_framebuffer_height = DEFAULT_WINDOW_HEIGHT;
int g_gl_window_width = DEFAULT_WINDOW_WIDTH;
int g_gl_window_height = DEFAULT_WINDOW_HEIGHT;
bool g_frambuffer_changed;
GLFWwindow* g_window = NULL;

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


static UpdatePerspectiveCalback gl_fptr = NULL;
GLFWwindow* gl_init(UpdatePerspectiveCalback fptr)
{
   gl_fptr = fptr;
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
   GLFWwindow* window = glfwCreateWindow(g_gl_window_width, g_gl_window_height, "Extended GL Init", NULL, NULL);
#endif
   if (!window) {
      gllog::Get().Error("ERROR: could not open window with GLFW3\n");
      glfwTerminate();
      return NULL;
   }

   // tell GL to only draw onto a pixel if the shape is closer to the viewer
   glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
   glfwSetWindowSizeCallback(window, WindowSizeCallback);
   glfwMakeContextCurrent(window);

   glfwGetWindowSize(window, &g_gl_window_width, &g_gl_window_height);
   gllog::Get().Message("initial window dims %ix%i\n", g_gl_window_width, g_gl_window_height);
   glfwGetFramebufferSize(window, &g_gl_framebuffer_width, &g_gl_framebuffer_height);
   gllog::Get().Message("initial framebuffer dims %ix%i\n", g_gl_framebuffer_width, g_gl_framebuffer_height);

   glfwWindowHint(GLFW_SAMPLES, 4);

   // start GLEW extension handler
   glewExperimental = GL_TRUE;
   glewInit();

   // get version info
   const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
   const GLubyte* version = glGetString(GL_VERSION); // version as a string
   gllog::Get().Message("Renderer: %s\n", renderer);
   gllog::Get().Message("OpenGL version supported %s\n", version);

   gllog::Get().LogParams();
   return window;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
   g_gl_framebuffer_width = width;
   g_gl_framebuffer_height = height;
   printf("framebuffer width %i height %i\n", width, height);
   g_frambuffer_changed = true;
}

// keep track of window size for things like the viewport and the mouse cursor
void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
   g_gl_window_width = width;
   g_gl_window_height = height;
   printf("window width %i height %i\n", width, height);

   /* update any perspective matrices used here */
   if (gl_fptr)
      gl_fptr(width, height);
}
