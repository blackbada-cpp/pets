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
#define NUM_SPHERES 4

//TODO: move all globals to engine 
static GLuint shader_programme;
static int model_mat_location = -1;
static int view_mat_location  = -1;
static int proj_mat_location  = -1;

dp::Mat4 view_mat;
dp::Mat4 proj_mat = dp::Mat4::Identity();
dp::Vec3 cam_pos(0.0f, 0.0f, 5.0f);
// a world position for each sphere in the scene
dp::Vec3 sphere_pos_wor[] = { dp::Vec3(-2.0, 0.0, 0.0), dp::Vec3(2.0, 0.0, 0.0), dp::Vec3(-2.0, 0.0, -2.0), dp::Vec3(1.5, 1.0, -1.0) };
const float sphere_radius = 1.0f;
// indicates which sphere is selected
int g_selected_sphere = -1;


/* check if a ray and a sphere intersect. if not hit, returns false. it rejects
intersections behind the ray caster's origin, and sets intersection_distance to
the closest intersection */
bool ray_sphere(dp::Vec3 ray_origin_wor, dp::Vec3 ray_direction_wor, dp::Vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {
   // work out components of quadratic
   dp::Vec3 dist_to_sphere = ray_origin_wor - sphere_centre_wor;
   //dp float b = dot(ray_direction_wor, dist_to_sphere);
   //dp float c = dot(dist_to_sphere, dist_to_sphere) - sphere_radius * sphere_radius;
   float b = ray_direction_wor.DotProduct(dist_to_sphere);
   float c = dist_to_sphere.DotProduct(dist_to_sphere) - sphere_radius * sphere_radius;
   float b_squared_minus_c = b * b - c;
   // check for "imaginary" answer. == ray completely misses sphere
   if (b_squared_minus_c < 0.0f) { return false; }
   // check for ray hitting twice (in and out of the sphere)
   if (b_squared_minus_c > 0.0f) {
      // get the 2 intersection distances along ray
      float t_a = -b + sqrt(b_squared_minus_c);
      float t_b = -b - sqrt(b_squared_minus_c);
      *intersection_distance = t_b;
      // if behind viewer, throw one or both away
      if (t_a < 0.0) {
         if (t_b < 0.0) { return false; }
      }
      else if (t_b < 0.0) {
         *intersection_distance = t_a;
      }

      return true;
   }
   // check for ray hitting once (skimming the surface)
   if (0.0f == b_squared_minus_c) {
      // if behind viewer, throw away
      float t = -b + sqrt(b_squared_minus_c);
      if (t < 0.0f) { return false; }
      *intersection_distance = t;
      return true;
   }
   // note: could also check if ray origin is inside sphere radius
   return false;
}

/* takes mouse position on screen and return ray in world coords */
dp::Vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {
   // screen space (viewport coordinates)
   float x = (2.0f * mouse_x) / g_gl_window_width - 1.0f;
   float y = 1.0f - (2.0f * mouse_y) / g_gl_window_height;
   float z = 1.0f;
   // normalised device space
   dp::Vec3 ray_nds = dp::Vec3(x, y, z);
   // clip space
   dp::Vec4 ray_clip = dp::Vec4(ray_nds.X(), ray_nds.Y(), -1.0, 1.0);
   // eye space
   dp::Vec4 ray_eye = proj_mat.Inverse() * ray_clip;
   ray_eye = dp::Vec4(ray_eye.X(), ray_eye.Y(), -1.0, 0.0);
   // world space
   dp::Vec3 ray_wor = dp::Vec3(view_mat.Inverse() * ray_eye);
   // don't forget to normalise the vector at some point
   
   ray_wor = ray_wor.Normalize();
   return ray_wor;
}

void OnUpdatePerspective(int width, int height)
{
   // update any perspective matrices used here
   glUseProgram(shader_programme);
   proj_mat = dp::Mat4::Projection(width, height);
   glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
}

/* this function is called when the mouse buttons are clicked or un-clicked */
void OnMouseClick(GLFWwindow* window, int button, int action, int mods) {
   // Note: could query if window has lost focus here
   if (GLFW_PRESS == action) {
      double xpos, ypos;
      glfwGetCursorPos(g_window, &xpos, &ypos);
      // work out ray
      dp::Vec3 ray_wor = get_ray_from_mouse((float)xpos, (float)ypos);
      // check ray against all spheres in scene
      int closest_sphere_clicked = -1;
      float closest_intersection = 0.0f;
      for (int i = 0; i < NUM_SPHERES; i++) {
         float t_dist = 0.0f;
         if (ray_sphere(cam_pos, ray_wor, sphere_pos_wor[i], sphere_radius, &t_dist)) {
            // if more than one sphere is in path of ray, only use the closest one
            if (-1 == closest_sphere_clicked || t_dist < closest_intersection) {
               closest_sphere_clicked = i;
               closest_intersection = t_dist;
            }
         }
      } // endfor
      g_selected_sphere = closest_sphere_clicked;
      printf("sphere %i was clicked\n", closest_sphere_clicked);
   }
}


int main()
{
   //////////////////////////////////////////////////////////////////////////
   // start OpenGL
   g_window = gl_init(OnUpdatePerspective);
   if (!g_window)
      return 1;

   glfwSetMouseButtonCallback(g_window, OnMouseClick);

   //////////////////////////////////////////////////////////////////////////
   // create geometry
   GLfloat* vp = NULL; // array of vertex points
   GLfloat* vn = NULL; // array of vertex normals
   GLfloat* vt = NULL; // array of texture coordinates
   int point_count = 0;
   load_obj_file(MESH_FILE, vp, vt, vn, point_count);

   // Column-major matrices to produce Model matrix:
   dp::Mat4 T, R;

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
   if (!InitShaderProgram(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE, shader_programme))
      return 1;

   if (!LinkShaderProgram(shader_programme))
      return 1;

   model_mat_location = glGetUniformLocation(shader_programme, "model");
   view_mat_location  = glGetUniformLocation(shader_programme, "view");
   proj_mat_location  = glGetUniformLocation(shader_programme, "proj");

   //////////////////////////////////////////////////////////////////////////
   // create camera
   float cam_speed = 5.0f; //1 unit per second
   float cam_heading_speed = 100.0f; // 30 degrees per second
   float cam_heading = 0.0f;   // y-rotation in degrees

   T = dp::Mat4::Translation(-cam_pos);
   
   dp::Quaternion quaternion(-cam_heading, 0.0f, 1.0f, 0.0f);

   proj_mat = dp::Mat4::Projection(g_gl_window_width, g_gl_window_height);
   R = quaternion.GetMatrix();
   view_mat = R * T;
   // keep track of some useful vectors that can be used for keyboard movement
   dp::Vec4 fwd(0.0f, 0.0f, -1.0f, 0.0f);
   dp::Vec4 rgt(1.0f, 0.0f, 0.0f, 0.0f);
   dp::Vec4 up(0.0f, 1.0f, 0.0f, 0.0f);


   //////////////////////////////////////////////////////////////////////////
   // set rendering defaults
   glUseProgram(shader_programme);
   glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
   glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat);
   // unique model matrix for each sphere
   dp::Mat4 model_mats[NUM_SPHERES];
   for (int i = 0; i < NUM_SPHERES; i++) { 
      model_mats[i] = dp::Mat4::Translation(sphere_pos_wor[i]); 
   }

   glEnable(GL_DEPTH_TEST);          // enable depth-testing
   glDepthFunc(GL_LESS);             // depth-testing interprets a smaller value as "closer"
   glEnable(GL_CULL_FACE);           // cull face
   glCullFace(GL_BACK);              // cull back face
   glFrontFace(GL_CCW);              // set counter-clock-wise vertex order to mean the front
   glClearColor(0.2, 0.2, 0.2, 1.0); // grey background to help spot mistakes
   glViewport(0, 0, g_gl_window_width, g_gl_window_height);

   while (!glfwWindowShouldClose(g_window))
   {
      // update timers
      static double previous_seconds = glfwGetTime();
      double current_seconds         = glfwGetTime();
      double elapsed_seconds         = current_seconds - previous_seconds;
      previous_seconds               = current_seconds;
      _update_fps_counter(g_window);
      
      // wipe the drawing surface clear
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, g_gl_window_width, g_gl_window_height);

      glUseProgram(shader_programme);
      for (int i = 0; i < NUM_SPHERES; i++) 
      {
         glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mats[i]);
         glDrawArrays(GL_TRIANGLES, 0, point_count); //dp 2 triangles
      }

      // update other events like input handling
      glfwPollEvents();

      // control keys
      bool cam_moved = false;
      dp::Vec3 move(0.0, 0.0, 0.0);
      float cam_yaw = 0.0f;
      float cam_pitch = 0.0f;
      float cam_roll = 0.0;

      //camera move
      if (glfwGetKey(g_window, GLFW_KEY_A)) {
         move.X() -= cam_speed * elapsed_seconds; //move left
         cam_moved = true;
      }
      if (glfwGetKey(g_window, GLFW_KEY_D)) {
         move.X() += cam_speed * elapsed_seconds; //move right
         cam_moved = true;
      }
      if (glfwGetKey(g_window, GLFW_KEY_Q)) {
         move.Y() += cam_speed * elapsed_seconds; //move forward
         cam_moved = true;
      }
      if (glfwGetKey(g_window, GLFW_KEY_E)) {
         move.Y() -= cam_speed * elapsed_seconds; //move forward
         cam_moved = true;
      }
      if (glfwGetKey(g_window, GLFW_KEY_W)) {
         move.Z() -= cam_speed * elapsed_seconds; //move forward
         cam_moved = true;
      }
      if (glfwGetKey(g_window, GLFW_KEY_S)) {
         move.Z() += cam_speed * elapsed_seconds; //move forward
         cam_moved = true;
      }
      if (glfwGetKey(g_window, GLFW_KEY_LEFT)) {
         cam_yaw += cam_heading_speed * elapsed_seconds; //move forward
         cam_moved = true;
         dp::Quaternion q_yaw(cam_yaw, up.X(), up.Y(), up.Z()); //create versor
         quaternion = quaternion * q_yaw;
         // recalc axes to suit new orientation
         R = quaternion.GetMatrix();
         fwd = R * dp::Vec4(0.0, 0.0, -1.0, 0.0);
         rgt = R * dp::Vec4(1.0, 0.0, 0.0, 0.0);
         up = R * dp::Vec4(0.0, 1.0, 0.0, 0.0);
      }
      if (glfwGetKey(g_window, GLFW_KEY_RIGHT)) {
         cam_yaw -= cam_heading_speed * elapsed_seconds; //move forward
         cam_moved = true;
         dp::Quaternion q_yaw(cam_yaw, up.X(), up.Y(), up.Z()); //create versor
         quaternion = quaternion * q_yaw;
         // recalc axes to suit new orientation
         R = quaternion.GetMatrix();
         fwd = R * dp::Vec4(0.0, 0.0, -1.0, 0.0);
         rgt = R * dp::Vec4(1.0, 0.0, 0.0, 0.0);
         up = R * dp::Vec4(0.0, 1.0, 0.0, 0.0);
      }
      if (glfwGetKey(g_window, GLFW_KEY_UP)) {
         cam_pitch += cam_heading_speed * elapsed_seconds;
         cam_moved = true;
         dp::Quaternion q_pitch(cam_pitch, rgt.X(), rgt.Y(), rgt.Z());
         quaternion = quaternion * q_pitch;
         // recalc axes to suit new orientation
         R = quaternion.GetMatrix();
         fwd = R * dp::Vec4(0.0, 0.0, -1.0, 0.0);
         rgt = R * dp::Vec4(1.0, 0.0, 0.0, 0.0);
         up = R * dp::Vec4(0.0, 1.0, 0.0, 0.0);
      }
      if (glfwGetKey(g_window, GLFW_KEY_DOWN)) {
         cam_pitch -= cam_heading_speed * elapsed_seconds;
         cam_moved = true;
         dp::Quaternion q_pitch(cam_pitch, rgt.X(), rgt.Y(), rgt.Z());
         quaternion = quaternion * q_pitch;
         // recalc axes to suit new orientation
         R = quaternion.GetMatrix();
         fwd = R * dp::Vec4(0.0, 0.0, -1.0, 0.0);
         rgt = R * dp::Vec4(1.0, 0.0, 0.0, 0.0);
         up = R * dp::Vec4(0.0, 1.0, 0.0, 0.0);
      }
      if (glfwGetKey(g_window, GLFW_KEY_Z)) {
         cam_roll -= cam_heading_speed * elapsed_seconds;
         cam_moved = true;
         dp::Quaternion q_roll(cam_roll, fwd.X(), fwd.Y(), fwd.Z());
         quaternion = quaternion * q_roll;
         R = quaternion.GetMatrix();
         fwd = R * dp::Vec4(0.0, 0.0, -1.0, 0.0);
         rgt = R * dp::Vec4(1.0, 0.0, 0.0, 0.0);
         up = R * dp::Vec4(0.0, 1.0, 0.0, 0.0);
      }
      if (glfwGetKey(g_window, GLFW_KEY_C)) {
         cam_roll += cam_heading_speed * elapsed_seconds;
         cam_moved = true;
         dp::Quaternion q_roll(cam_roll, fwd.X(), fwd.Y(), fwd.Z());
         quaternion = quaternion * q_roll;
         R = quaternion.GetMatrix();
         fwd = R * dp::Vec4(0.0, 0.0, -1.0, 0.0);
         rgt = R * dp::Vec4(1.0, 0.0, 0.0, 0.0);
         up = R * dp::Vec4(0.0, 1.0, 0.0, 0.0);
      }
      if (cam_moved)
      {
         R = quaternion.GetMatrix();
         cam_pos = cam_pos + dp::Vec3(fwd) * (-move.Z());
         cam_pos = cam_pos + dp::Vec3(up) * move.Y();
         cam_pos = cam_pos + dp::Vec3(rgt) * move.X();
         T = dp::Mat4::Translation(cam_pos);

         view_mat = R.Inverse() * T.Inverse();
         //dp glUseProgram(shader_programme);
         glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat);
      }
      if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
         glfwSetWindowShouldClose(g_window, 1); //exit
      }

      glfwSwapBuffers(g_window);
   } //while loop

   glDeleteProgram(shader_programme);

   // close GL context and any other GLFW resources
   glfwTerminate();

   return 0;
}
