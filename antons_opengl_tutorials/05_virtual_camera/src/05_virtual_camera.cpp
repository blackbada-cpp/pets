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

//TODO: move all globals to engine 
static GLuint gl_shader_program;
static int gl_proj_location = -1;
static dp::Mat4 gl_Proj = dp::Mat4::Identity();

void OnUpdatePerspective(int width, int height)
{
   // update any perspective matrices used here
   glUseProgram(gl_shader_program);
   gl_Proj = dp::Mat4::Projection(width, height);
   glUniformMatrix4fv(gl_proj_location, 1, GL_FALSE, gl_Proj);
}

int main()
{
   GLFWwindow* window = gl_init(OnUpdatePerspective);
   if (!window)
      return -1;

   
   if (!InitShaderProgram("shaders/test_vs.glsl", "shaders/test_fs.glsl", gl_shader_program))
      return -1;
   
   if (!LinkShaderProgram(gl_shader_program))
      return -1;

   //dp //triangle
   //dp float points[] = {
   //dp    0.0f,  0.5f,  0.0f,
   //dp    0.5f, -0.5f,  0.0f,
   //dp    -0.5f, -0.5f,  0.0f
   //dp };
   
   //square
   float points[] = {
      -0.5f,  0.5f,  0.0f, //#1
       0.5f, -0.5f,  0.0f, //#2
      -0.5f, -0.5f,  0.0f, //#3
       0.5f,  0.5f,  0.0f, //#4
       0.5f, -0.5f,  0.0f, //#5=#2
      -0.5f,  0.5f,  0.0f, //#6=#1
   };

   float colours[] = {
      1.0f, 0.0f,  0.0f, //#1
      0.0f, 1.0f,  0.0f, //#2
      0.0f, 0.0f,  1.0f, //#3
      1.0f, 1.0f,  1.0f, //#4
      0.0f, 1.0f,  0.0f, //#5=#2
      1.0f, 0.0f,  0.0f, //#6=#1
   };
   
   // Column-major matrices to produce Model matrix:
   dp::Mat4 T, R, S;
   T = dp::Mat4::Translation(0.5, 0.0, 0.0);
   R = dp::Mat4::RotationZ(0.2);
   S = dp::Mat4::Scale(0.5, 0.5, 0.5);

   GLuint points_vbo = 0;
   glGenBuffers(1, &points_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
   glBufferData(GL_ARRAY_BUFFER, 2 * 3 * 3 * sizeof(float), points, GL_STATIC_DRAW); //dp 2 triangles

   GLuint colours_vbo = 0;
   glGenBuffers(1, &colours_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
   glBufferData(GL_ARRAY_BUFFER, 2 * 3 * 3 * sizeof(float), colours, GL_STATIC_DRAW); //dp 2 triangles

   GLuint vao = 0;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
   glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);

   //dp int matrix_location = glGetUniformLocation(shader_program, "mvp");
   int view_location = glGetUniformLocation(gl_shader_program, "view");
   gl_proj_location = glGetUniformLocation(gl_shader_program, "proj");
   int model_location = glGetUniformLocation(gl_shader_program, "model");
   glUseProgram(gl_shader_program);
   dp::Mat4 Model = dp::Mat4::Identity() * T * R * S;
   dp::Vec3 cameraPosition(-0.5, 0.0, 1.0); //(0.5, 0.5, 0.5);
   //dp dp::Mat4 View = dp::Mat4::View(dp::Vec3(0.0, 1.0, 0.0) /*Upward*/, 
   //dp                                dp::Vec3(0.0, 0.0, -1.0) /*Forward*/, 
   //dp                                dp::Vec3(1.0, 0.0, 0.0) /*Right*/, 
   //dp                                cameraPosition);
   
   dp::Mat4 View = dp::Mat4::View(dp::Vec3(-0.707, 0.707, -0.707) /*Upward*/,
                                  dp::Vec3(-0.707, -0.707, -0.707) /*Forward*/,
                                  dp::Vec3(0.707, 0.707, -0.707) /*Right*/,
                                  cameraPosition);
   dp::Mat4 ViewLookAt = dp::Mat4::LookAt(cameraPosition, 
                                    dp::Vec3(0.0, 0.0, 0.0)/*targetPos*/, 
                                    dp::Vec3(0.0, 1.0, 0.0)/*up_direction*/);
   
   gl_Proj = dp::Mat4::Projection(g_gl_width, g_gl_height);
   //dp dp::Mat4 MVP = gl_Proj * ViewLookAt * Model;
   //dp glUniformMatrix4fv(matrix_location, 1, GL_FALSE, MVP);
   glUniformMatrix4fv(gl_proj_location, 1, GL_FALSE, gl_Proj);
   glUniformMatrix4fv(view_location, 1, GL_FALSE, ViewLookAt);
   glUniformMatrix4fv(model_location, 1, GL_FALSE, Model);
   float x_speed = 0.0f; // 1 unit per second
   float rotation_speed = 1.0f; // -1.0f; //1 unit per second
   float camera_speed = 1.0f; //1 unit per second
   float camera_yaw_speed = 10.f; // 10 degrees per second

   float last_position = 0.0f;
   float last_angle = 0.0f;
   float camera_yaw = 0.0f;

   while (!glfwWindowShouldClose(window))
   {
      //add a timer for doing animation
      static double previous_seconds = glfwGetTime();
      double current_seconds = glfwGetTime();
      double elapsed_seconds = current_seconds - previous_seconds;
      previous_seconds = current_seconds;

      //reverse direction when going to far left or right
      if (fabs(last_position) > 1.0f) {
         x_speed = -x_speed;
      }
      if (fabs(last_angle) > 2*M_PI) {
         rotation_speed = -rotation_speed;
      }
      if (fabs(cameraPosition.X()) > 2.0f) {
         camera_speed = -camera_speed;
      }

      //update the matrix
      T.SetTranslation(elapsed_seconds * x_speed + last_position, 0.0, 0.0);
      R.SetRotationZ(last_angle);
      last_position = T.TranslationX();
      last_angle = elapsed_seconds * rotation_speed + last_angle;
      float last_camera_x = cameraPosition.X();
      cameraPosition.X() = elapsed_seconds * camera_speed + last_camera_x;
      Model = dp::Mat4::Identity() * T * R * S;
      //Model = Model * T;

      glUseProgram(gl_shader_program);
      //dp glUniformMatrix4fv(matrix_location, 1, GL_FALSE, Model.m_data);
      ViewLookAt = dp::Mat4::LookAt(cameraPosition, 
                                    dp::Vec3(0.0, 0.0, 0.0)/*targetPos*/, 
                                    dp::Vec3(0.0, 1.0, 0.0)/*up_direction*/);
      //dp dp::Mat4 MVP = gl_Proj * ViewLookAt * Model;
      //dp glUniformMatrix4fv(matrix_location, 1, GL_FALSE, MVP);
      glUniformMatrix4fv(gl_proj_location, 1, GL_FALSE, gl_Proj);
      glUniformMatrix4fv(view_location, 1, GL_FALSE, ViewLookAt);
      glUniformMatrix4fv(model_location, 1, GL_FALSE, Model);

      _update_fps_counter(window);
      glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, g_gl_width, g_gl_height);

      //--- todo: draw
      glPolygonMode(GL_FRONT, GL_LINE);
      glUseProgram(gl_shader_program);
      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 2 * 3); //dp 2 triangles
      //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      //---
      glfwPollEvents();
      glfwSwapBuffers(window);

      if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
         glfwSetWindowShouldClose(window, 1);
      }
   }

   glDeleteProgram(gl_shader_program);

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
