// 01_extended_init.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <memory>
#include "gl_log.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#define GL_LOG_FILE "gl.log"

void glfw_error_callback(int error, const char* description) {
   gllog::Error("GLFW ERROR: code %i msg: %s\n", error, description);
}

// keep track of window size for things like the viewport and the mouse cursor
int g_gl_width = 640;
int g_gl_height = 480;
int g_fb_width = 640;
int g_fb_height = 480;


// a call-back function
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
   g_gl_width = width;
   g_gl_height = height;

   /* update any perspective matrices used here */
}

void glfw_framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
   g_fb_width = width;
   g_fb_height = height;
   /* later update any perspective matrices used here */
}


void _update_fps_counter(GLFWwindow* window) {
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

int main()
{
   // start GL context and O/S window using the GLFW helper library
   gllog::Message("starting GLFW\n%s\n", glfwGetVersionString());
   // register the error call-back function that we wrote, above
   glfwSetErrorCallback(glfw_error_callback);
   if (!glfwInit()) {
      gllog::Error("ERROR: could not start GLFW3\n");
      return 1;
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
      gllog::Error("ERROR: could not open window with GLFW3\n");
      glfwTerminate();
      return 1;
   }
   glfwMakeContextCurrent(window);
   // start GLEW extension handler
   glewExperimental = GL_TRUE;
   glewInit();

   // get version info
   const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
   const GLubyte* version = glGetString(GL_VERSION); // version as a string
   gllog::Message("Renderer: %s\n", renderer);
   gllog::Message("OpenGL version supported %s\n", version);

   // tell GL to only draw onto a pixel if the shape is closer to the viewer
   glEnable(GL_DEPTH_TEST); // enable depth-testing
   glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

   glfwSetWindowSizeCallback(window, glfw_window_size_callback);
   glfwSetFramebufferSizeCallback(window, glfw_framebuffer_resize_callback);
   gllog::LogParams();

   while (!glfwWindowShouldClose(window))
   {
      _update_fps_counter(window);
      glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //dp glViewport(0, 0, g_gl_width, g_gl_height);
      glViewport(0, 0, g_fb_width, g_fb_height);

      //--- todo: draw

      
      //---
      glfwPollEvents();
      glfwSwapBuffers(window);
      
      if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
         glfwSetWindowShouldClose(window, 1);
      }
   }
   // close GL context and any other GLFW resources
   glfwTerminate();
   
   return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
