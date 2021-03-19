// 04_mats_and_vecs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "gl_log.h"
#include "gl_shaders.h"
#include "gl_util.h"
#include "gl_math.h"

int main()
{
   GLFWwindow* window = gl_init();
   if (!window)
      return -1;

   // tell GL to only draw onto a pixel if the shape is closer to the viewer
   glEnable(GL_DEPTH_TEST); // enable depth-testing
   glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"

   float points[] = {
      0.0f,  0.5f,  0.0f,
      0.5f, -0.5f,  0.0f,
      -0.5f, -0.5f,  0.0f
   };
   
   float normals[] = {
      0.0f, 0.0f,  1.0f,
      0.0f, 0.0f,  1.0f,
      0.0f, 0.0f,  1.0f
   };
   
   dp::Mat4 T, R, S;
   T = dp::Mat4::Translation(0.5, 0.0, 0.0);
   R = dp::Mat4::RotationZ(0.2);
   S = dp::Mat4::Scale(1.1, 1.1, 1.1);

   GLuint points_vbo = 0;
   glGenBuffers(1, &points_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
   glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

   GLuint normals_vbo = 0;
   glGenBuffers(1, &normals_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
   glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), normals, GL_STATIC_DRAW);

   GLuint vao = 0;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
   glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);

   GLuint shader_program;
   if (!InitShaderProgram("shaders/test_vs.glsl", "shaders/test_fs.glsl", shader_program))
      return -1;
   if (!LinkShaderProgram(shader_program))
      return -1;
#define ONE_DEG_IN_RAD ( 2.0 * M_PI ) / 360.0 // 0.017444444

   int matrix_location = glGetUniformLocation(shader_program, "matrix");
   glUseProgram(shader_program);
   dp::Mat4 Model = dp::Mat4::Identity() * T * R * S;
   glUniformMatrix4fv(matrix_location, 1, GL_FALSE, Model);
   float speed = 0.5f; //1 unit per second
   float rotation_speed = 0.0f; // -8.0f; //1 unit per second
   float last_position = 0.0f;
   float last_angle = 0.0f;

   glEnable(GL_CULL_FACE); // cull face
   glCullFace(GL_BACK);    // cull back face
   glFrontFace(GL_CW);     // GL_CCW for counter clock-wise

   while (!glfwWindowShouldClose(window))
   {
      //add a timer for doing animation
      static double previous_seconds = glfwGetTime();
      double current_seconds = glfwGetTime();
      double elapsed_seconds = current_seconds - previous_seconds;
      previous_seconds = current_seconds;

      //reverse direction when going to far left or right
      if (fabs(last_position) > 1.0f) {
         speed = -speed;
         rotation_speed = -rotation_speed;
      }

      //update the matrix
      T.SetTranslation(elapsed_seconds * speed + last_position, 0.0, 0.0);
      R.SetRotationZ(last_angle);
      last_position = T.TranslationX();
      last_angle = elapsed_seconds * rotation_speed + last_angle;
      dp::Mat4 Model = dp::Mat4::Identity() * T * R * S;
      //Model = Model * T;

      glUseProgram(shader_program);
      //dp glUniformMatrix4fv(matrix_location, 1, GL_FALSE, Model.m_data);
      glUniformMatrix4fv(matrix_location, 1, GL_FALSE, Model);

      _update_fps_counter(window);
      glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, g_gl_window_width, g_gl_window_height);

      //--- todo: draw
      glPolygonMode(GL_FRONT, GL_LINE);
      glUseProgram(shader_program);
      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 3);
      //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      //---
      glfwPollEvents();
      glfwSwapBuffers(window);

      if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
         glfwSetWindowShouldClose(window, 1);
      }
   }

   glDeleteProgram(shader_program);

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
