#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

void _update_fps_counter(GLFWwindow* window);
void glfw_error_callback(int error, const char* description);
typedef void(*UpdatePerspectiveCalback)(int, int);
GLFWwindow* gl_init(UpdatePerspectiveCalback fptr = NULL);

// keep track of window size for things like the viewport and the mouse cursor
extern int g_gl_window_width;
extern int g_gl_window_height;
extern GLFWwindow* g_window;

// a call-back function
void WindowSizeCallback(GLFWwindow* window, int width, int height);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
