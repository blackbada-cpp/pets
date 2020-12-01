#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

void _update_fps_counter(GLFWwindow* window);
void glfw_error_callback(int error, const char* description);
typedef void(*UpdatePerspectiveCalback)(int, int);
GLFWwindow* gl_init(UpdatePerspectiveCalback fptr = NULL);

// keep track of window size for things like the viewport and the mouse cursor
extern int g_gl_width;
extern int g_gl_height;

// a call-back function
void glfw_window_size_callback(GLFWwindow* window, int width, int height);
