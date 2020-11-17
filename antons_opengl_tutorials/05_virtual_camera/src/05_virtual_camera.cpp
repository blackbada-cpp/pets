// 05_virtual_camera.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <sstream>
#include <iostream>

#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "gl_log.h"
#include "gl_shaders.h"
#include "gl_util.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <corecrt_math_defines.h>

// camera variables
float cam_speed = 1.0f;  // 1 degree per second
float cam_yaw_speed = 10.0f; // 10 degrees per second
float cam_pos[] = { 0.0f, 0.0f, 2.0f }; //don't start at zero, or will be too close
float cam_yaw = 0.0f;

// input variables
#define ONE_DEG_IN_RAD ((2.0 * M_PI) / 360.0) // 0.0174444


float _near = 0.1f; //clipping plane
float _far = 100.0f; // clipping plane
float fov = 67 * ONE_DEG_IN_RAD; // 67 degrees to radians
float aspect = (float)g_gl_width / (float)g_gl_height;
float range = tan(fov * 0.05f) * _near;
float Sx = (2.0f * _near) / (range * aspect + range * aspect);
float Sy = _near / range;
float Sz = -(_far + _near) / (_far - _near);
float Pz = -(2.0f * _far * _near) / (_far - _near);

float proj_mat[] = {
   Sx,   0.0f, 0.0f, 0.0f,
   0.0f, Sy,   0.0f, 0.0f,
   0.0f, 0.0f, Sz,   -1.0f,
   0.0f, 0.0f, Pz,   0.0f 
};


int main()
{
   GLFWwindow* window = gl_init();
   if (!window)
      return -1;

   GLuint shader_program;
   if (!InitShaderProgram("shaders/test_vs.glsl", "shaders/test_fs.glsl", shader_program))
      return -1;
   if (!LinkShaderProgram(shader_program))
      return -1;

   float points[] = {
      0.0f,  0.5f,  0.0f,
      0.5f, -0.5f,  0.0f,
      -0.5f, -0.5f,  0.0f
   };
   float colours[] = {
      1.0f, 0.0f,  0.0f,
      0.0f, 1.0f,  0.0f,
      0.0f, 0.0f,  1.0f
   };


   GLuint points_vbo = 0;
   glGenBuffers(1, &points_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
   glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

   GLuint colours_vbo = 0;
   glGenBuffers(1, &colours_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
   glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colours, GL_STATIC_DRAW);

   GLuint vao = 0;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
   glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);

   glm::vec3 translation(-cam_pos[0], -cam_pos[1], -cam_pos[2]);
   glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);
   glm::mat4 R = glm::rotate(glm::mat4(1.0f), -cam_yaw, glm::vec3(0.0, 1.0, 0.0));
   glm::mat4 view = R * T;
   glm::mat4 model = glm::mat4(1.0f);
   //glm::mat4 proj = glm::make_mat4(proj_mat);
   glm::mat4 proj = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
   glm::mat4 mvp = proj * view * model;

   std::stringstream s;
   s << glm::to_string(proj);

   int mvp_location = glGetUniformLocation(shader_program, "mvp");
   glUseProgram(shader_program);
   glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

   //uniform mat4 mvp;

   while (!glfwWindowShouldClose(window))
   {
      _update_fps_counter(window);
      glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, g_gl_width, g_gl_height);

      //--- todo: draw
      //glPolygonMode(GL_FRONT, GL_LINE);
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
