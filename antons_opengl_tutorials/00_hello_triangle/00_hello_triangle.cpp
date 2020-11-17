// 00_hello_triangle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>

#include "gl_shaders.h"



int main()
{
   std::cout << "00_hello_triangle.cpp\n"; 
   // start GL context and O/S window using the GLFW helper library
   if (!glfwInit()) {
      fprintf(stderr, "ERROR: could not start GLFW3\n");
      return 1;
   }

   GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Triangle", NULL, NULL);
   if (!window) {
      fprintf(stderr, "ERROR: could not open window with GLFW3\n");
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
   printf("Renderer: %s\n", renderer);
   printf("OpenGL version supported %s\n", version);

   // tell GL to only draw onto a pixel if the shape is closer to the viewer
   glEnable(GL_DEPTH_TEST); // enable depth-testing
   glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

   /* OTHER STUFF GOES HERE NEXT */
   float points[] = {
     -0.8f,  0.0f,  0.0f,
      0.8f,  0.8f,  0.0f,
      0.8f, -0.8f,  0.0f
   };

   GLuint vbo = 0;
   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(points) /*9 * sizeof(float)*/, points, GL_STATIC_DRAW);

   GLuint vao = 0;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);


   std::string vertex_shader = LoadShader("shaders/test_vs.glsl");
   const char * vertex_shader_str = vertex_shader.c_str();
   std::string fragment_shader = LoadShader("shaders/test_fs.glsl");
   const char * fragment_shader_str = fragment_shader.c_str();

   std::string vertex_shader2 = LoadShader("shaders/test2_vs.glsl");
   const char * vertex_shader2_str = vertex_shader.c_str();
   std::string fragment_shader2 = LoadShader("shaders/test2_fs.glsl");
   const char * fragment_shader2_str = fragment_shader.c_str();

   GLuint vs = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vs, 1, &vertex_shader_str, NULL);
   glCompileShader(vs);

   GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fs, 1, &fragment_shader_str, NULL);
   glCompileShader(fs);

   GLuint shader_program = glCreateProgram();
   glAttachShader(shader_program, fs);
   glAttachShader(shader_program, vs);
   glLinkProgram(shader_program);

   GLuint vs2 = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vs2, 1, &vertex_shader2_str, NULL);
   glCompileShader(vs2);

   GLuint fs2 = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fs2, 1, &fragment_shader2_str, NULL);
   glCompileShader(fs2);

   GLuint shader_program2 = glCreateProgram();
   glAttachShader(shader_program2, fs2);
   glAttachShader(shader_program2, vs2);
   glLinkProgram(shader_program2);

   while(!glfwWindowShouldClose(window))
   {
      glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glUseProgram(shader_program);
      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      //glDrawArrays(GL_LINE_STRIP, 0, 3);

      glfwPollEvents();
      glfwSwapBuffers(window);
   }

   glDeleteProgram(shader_program);
   glDeleteProgram(shader_program2);

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
