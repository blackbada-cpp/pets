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

#define ONE_DEG_IN_RAD ( 2.0 * M_PI ) / 360.0 // 0.017444444

//Engine
static GLuint shader_programme;
static int model_mat_location = -1;
static int view_mat_location = -1;
static int proj_mat_location = -1;

//Camera
dp::Mat4 view_mat;
dp::Mat4 proj_mat = dp::Mat4::Identity();
dp::Vec3 cam_pos(0.0f, 0.0f, 2.0f);

//3D object
dp::Mat4 model_mat;

void OnUpdatePerspective(int width, int height)
{
   // update any perspective matrices used here
   glUseProgram(shader_programme);
   proj_mat = dp::Mat4::Projection(width, height);
   glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
}

//08_phong
int main()
{
   g_window = gl_init(OnUpdatePerspective);
   if (!g_window)
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
   
   // Column-major matrices to produce Model matrix:
   dp::Mat4 T, R, S;

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

   //////////////////////////////////////////////////////////////////////////
   // Create shader
   if (!InitShaderProgram("shaders/test_vs.glsl", "shaders/test_fs.glsl", shader_programme))
      return -1;
   if (!LinkShaderProgram(shader_programme))
      return -1;

   glUseProgram(shader_programme);
   model_mat_location = glGetUniformLocation(shader_programme, "model");
   view_mat_location = glGetUniformLocation(shader_programme, "view");
   proj_mat_location = glGetUniformLocation(shader_programme, "proj");
   
   //////////////////////////////////////////////////////////////////////////
   // create camera
   float cam_heading = 0.0f;   // y-rotation in degrees
   T = dp::Mat4::Translation(-cam_pos);
   dp::Quaternion quaternion(-cam_heading, 0.0f, 1.0f, 0.0f);
   proj_mat = dp::Mat4::Projection(g_gl_window_width, g_gl_window_height);
   R = quaternion.GetMatrix();
   view_mat = R * T;

   //////////////////////////////////////////////////////////////////////////
   // set rendering defaults
   glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
   glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat);

   //Triangle model matrix
   T = dp::Mat4::Translation(0.5, 0.0, 0.0);
   R = dp::Mat4::RotationZ(0.2);
   S = dp::Mat4::Scale(1.1, 1.1, 1.1);
   dp::Mat4::Identity() * T * R * S;
   glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat);
   float speed = 0.5f; //1 unit per second
   float rotation_speed = 0.0f; // -8.0f; //1 unit per second
   float last_position = 0.0f;
   float last_angle = 0.0f;

   glEnable(GL_CULL_FACE); // cull face
   glCullFace(GL_BACK);    // cull back face
   glFrontFace(GL_CW);     // GL_CCW for counter clock-wise

   glViewport(0, 0, g_gl_window_width, g_gl_window_height);

   //08_phong
   while (!glfwWindowShouldClose(g_window))
   {
      // update timers
      static double previous_seconds = glfwGetTime();
      double current_seconds = glfwGetTime();
      double elapsed_seconds = current_seconds - previous_seconds;
      previous_seconds = current_seconds;
      _update_fps_counter(g_window);

      // wipe the drawing surface clear
      glViewport(0, 0, g_gl_window_width, g_gl_window_height);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
      model_mat = dp::Mat4::Identity() * T * R * S;
      //Model = Model * T;

      glUseProgram(shader_programme);
      //dp glUniformMatrix4fv(matrix_location, 1, GL_FALSE, Model.m_data);
      glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat);

      glPolygonMode(GL_FRONT, GL_LINE);
      glUseProgram(shader_programme);
      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 3);
      //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      // update other events like input handling
      glfwPollEvents();
      
      //...oops, no input handling

      //just check ESC key
      if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
         glfwSetWindowShouldClose(g_window, 1);
      }

      glfwSwapBuffers(g_window);
   }//loop

   glDeleteProgram(shader_programme);

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
