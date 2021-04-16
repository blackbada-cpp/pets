//09_texture_rendering


#include "pch.h"
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "gl_log.h"
#include "gl_shaders.h"
#include "gl_util.h"
#include "gl_math.h"

#include "stb_image.h"

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
dp::Vec3 model_pos = dp::Vec3(0.0, 0.0, 0.0);
dp::Mat4 model_mat;

void OnUpdatePerspective(int width, int height)
{
   // update any perspective matrices used here
   glUseProgram(shader_programme);
   proj_mat = dp::Mat4::Projection(width, height);
   glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
}

bool load_texture(const char* file_name, GLuint* tex) {
   int x, y, n;
   int force_channels = 4;
   // the following function call flips the image
   // needs to be called before each stbi_load(...);
   stbi_set_flip_vertically_on_load(true);
   unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
   if (!image_data) {
      fprintf(stderr, "ERROR: could not load %s\n", file_name);
      return false;
   }
   // NPOT check
   if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) { fprintf(stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name); }

   glGenTextures(1, tex);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, *tex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
   glGenerateMipmap(GL_TEXTURE_2D);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   GLfloat max_aniso = 0.0f;
   glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
   // set the maximum!
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
   return true;
}


//09_texture_rendering
int main()
{
   g_window = gl_init(OnUpdatePerspective);
   if (!g_window)
      return -1;

   // tell GL to only draw onto a pixel if the shape is closer to the viewer
   glEnable(GL_DEPTH_TEST); // enable depth-testing
   glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"

   // layout (location = 0) in vec3 vertex_position;
   GLfloat points[] = { 
      -0.5f, -0.5f, 0.0f, 
       0.5f, -0.5f, 0.0f, 
       0.5f,  0.5f, 0.0f, 
       0.5f,  0.5f, 0.0f, 
      -0.5f,  0.5f, 0.0f, 
      -0.5f, -0.5f, 0.0f };

   // layout (location = 1) in vec2 vt;
   GLfloat texcoords[] = { 
      0.0f, 0.0f, 
      1.0f, 0.0f, 
      1.0f, 1.0f, 
      1.0f, 1.0f, 
      0.0f, 1.0f, 
      0.0f, 0.0f };


   // Column-major matrices to produce Model matrix:
   dp::Mat4 T, R, S;

   GLuint points_vbo = 0;
   glGenBuffers(1, &points_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
   glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), points, GL_STATIC_DRAW);

   GLuint texcoords_vbo;
   glGenBuffers(1, &texcoords_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
   glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);

   GLuint vao = 0;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
   glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

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
   glUseProgram(shader_programme);
   glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
   glUseProgram(shader_programme);
   glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat);

   //Rectangle model matrix
   //T = dp::Mat4::Translation(0.5, 0.0, 0.0);
   //R = dp::Mat4::RotationZ(0.0);
   //S = dp::Mat4::Scale(1.0, 1.0, 1.0);
   model_mat = dp::Mat4::Translation(model_pos); // dp::Mat4::Identity()*T* R* S;
   glUseProgram(shader_programme);
   glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat);

   // load texture
   GLuint tex;
   (load_texture("skulluvmap.png", &tex));

   //   int tex_loc = glGetUniformLocation(shader_programme, "basic_texture");
   //   glUseProgram(shader_programme);
   //   glUniform1i(tex_loc, 0);//use active texture 0

   glEnable(GL_DEPTH_TEST);          // enable depth-testing
   glDepthFunc(GL_LESS);             // depth-testing interprets a smaller value as "closer"
   glEnable(GL_CULL_FACE); // cull face
   glCullFace(GL_BACK);    // cull back face
   //dp this will make all triangles invisible !!!!!!!! glFrontFace(GL_CW);     // GL_CCW for counter clock-wise
   glFrontFace(GL_CCW);              // set counter-clock-wise vertex order to mean the front
   glClearColor(0.2, 0.2, 0.2, 1.0); // grey background to help spot mistakes
   glViewport(0, 0, g_gl_window_width, g_gl_window_height);

   while (!glfwWindowShouldClose(g_window))
   {
      // update timers
      static double previous_seconds = glfwGetTime();
      double current_seconds = glfwGetTime();
      double elapsed_seconds = current_seconds - previous_seconds;
      previous_seconds = current_seconds;
      _update_fps_counter(g_window);

      // wipe the drawing surface clear
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, g_gl_window_width, g_gl_window_height);

      glUseProgram(shader_programme);
      glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat);

      //glPolygonMode(GL_FRONT, GL_LINE);
      glUseProgram(shader_programme);
      //glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 6);
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
