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
#include "obj_parser.h"
#include <vector>

#define MESH_FILE "models\\sphere.obj"
#define VERTEX_SHADER_FILE   "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"

//TODO: move all globals to engine 
static GLuint gl_shader_program;
static int model_mat_location = -1;
static int view_mat_location  = -1;
static int proj_mat_location  = -1;
static dp::Mat4 proj_mat = dp::Mat4::Identity();

void OnUpdatePerspective(int width, int height)
{
   // update any perspective matrices used here
   glUseProgram(gl_shader_program);
   proj_mat = dp::Mat4::Projection(width, height);
   glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
}

int main()
{
   //////////////////////////////////////////////////////////////////////////
   // start OpenGL
   GLFWwindow* window = gl_init(OnUpdatePerspective);
   if (!window)
      return 1;
   
   //////////////////////////////////////////////////////////////////////////
   // create geometry
   GLfloat* vp = NULL; // array of vertex points
   GLfloat* vn = NULL; // array of vertex normals
   GLfloat* vt = NULL; // array of texture coordinates
   int point_count = 0;
   load_obj_file(MESH_FILE, vp, vt, vn, point_count);

   // Column-major matrices to produce Model matrix:
   dp::Mat4 T, R, S;
   T = dp::Mat4::Translation(0.5, 0.0, 0.0);
   R = dp::Mat4::RotationZ(0.2);
   S = dp::Mat4::Scale(0.5, 0.5, 0.5);

   GLuint vao = 0;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   GLuint points_vbo;
   if (vp)
   {
      glGenBuffers(1, &points_vbo);
      glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
      glBufferData(GL_ARRAY_BUFFER, 3 * point_count * sizeof(GLfloat), vp, GL_STATIC_DRAW); //dp 2 triangles
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
      glEnableVertexAttribArray(0);
   }

   //////////////////////////////////////////////////////////////////////////
   // Create shader
   if (!InitShaderProgram(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE, gl_shader_program))
      return 1;

   if (!LinkShaderProgram(gl_shader_program))
      return 1;

   model_mat_location = glGetUniformLocation(gl_shader_program, "model");
   view_mat_location  = glGetUniformLocation(gl_shader_program, "view");
   proj_mat_location  = glGetUniformLocation(gl_shader_program, "proj");

   //////////////////////////////////////////////////////////////////////////
   // create camera
   glUseProgram(gl_shader_program);
   dp::Mat4 Model = dp::Mat4::Identity() * T * R * S;
   dp::Vec3 cameraPosition(0.0, 0.0, 1.0); //(0.5, 0.5, 0.5);
   float camera_yaw = 0.0f;
   dp::Mat4 ViewLookYaw = dp::Mat4::LookYaw(cameraPosition, camera_yaw);

   proj_mat = dp::Mat4::Projection(g_gl_width, g_gl_height);
   glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
   glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, ViewLookYaw);
   glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, Model);
   float camera_speed = 1.0f; //1 unit per second
   float camera_yaw_speed = 10.f; // 10 degrees per second

   float last_position = 0.0f;
   float last_angle = 0.0f;

   while (!glfwWindowShouldClose(window))
   {
      // update timers
      static double previous_seconds = glfwGetTime();
      double current_seconds = glfwGetTime();
      double elapsed_seconds = current_seconds - previous_seconds;
      previous_seconds = current_seconds;
      _update_fps_counter(window);
      
      // wipe the drawing surface clear
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      //model animation
      Model = dp::Mat4::Identity() * T * R * S;

      glUseProgram(gl_shader_program);
      glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, Model);

      glViewport(0, 0, g_gl_width, g_gl_height);

      //draw
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //both faces
      glUseProgram(gl_shader_program);
      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, point_count); //dp 2 triangles

      // update other events like input handling
      glfwPollEvents();

      //control keys
      bool cam_moved = false;
      if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
         glfwSetWindowShouldClose(window, 1); //exit
      }
      //camera move
      if (glfwGetKey(window, GLFW_KEY_A)) {
         cameraPosition.X() -= camera_speed * elapsed_seconds; //move left
         cam_moved = true;
      }
      if (glfwGetKey(window, GLFW_KEY_D)) {
         cameraPosition.X() += camera_speed * elapsed_seconds; //move right
         cam_moved = true;
      }
      if (glfwGetKey(window, GLFW_KEY_PAGE_UP)) {
         cameraPosition.Y() += camera_speed * elapsed_seconds; //move up
         cam_moved = true;
      }
      if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN)) {
         cameraPosition.Y() -= camera_speed * elapsed_seconds; //move down
         cam_moved = true;
      }
      if (glfwGetKey(window, GLFW_KEY_W)) {
         cameraPosition.Z() -= camera_speed * elapsed_seconds; //move forward
         cam_moved = true;
      }
      if (glfwGetKey(window, GLFW_KEY_S)) {
         cameraPosition.Z() += camera_speed * elapsed_seconds; //move forward
         cam_moved = true;
      }
      if (glfwGetKey(window, GLFW_KEY_LEFT)) {
         camera_yaw += camera_yaw_speed * elapsed_seconds; //move forward
         cam_moved = true;
      }
      if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
         camera_yaw -= camera_yaw_speed * elapsed_seconds; //move forward
         cam_moved = true;
      }
      if (cam_moved)
      {
         glUseProgram(gl_shader_program);
         ViewLookYaw = dp::Mat4::LookYaw(cameraPosition, camera_yaw);
         glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, ViewLookYaw);
      }

      glfwSwapBuffers(window);
   } //while loop

   glDeleteProgram(gl_shader_program);

   // close GL context and any other GLFW resources
   glfwTerminate();

   return 0;
}
