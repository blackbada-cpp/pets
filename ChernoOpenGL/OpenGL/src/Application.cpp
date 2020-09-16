#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "tests/Test.h"
#include "tests/TestClearColor.h"

int main(void)
{
   GLFWwindow* window;

   /* Initialize the library */
   if (!glfwInit())
      return -1;

   const char* glsl_version = "#version 130";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   /* Create a windowed mode window and its OpenGL context */
   //dp window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
   window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
   if (!window)
   {
      glfwTerminate();
      return -1;
   }

   /* Make the window's context current */
   glfwMakeContextCurrent(window);
   glfwSwapInterval(1);

   if (glewInit() != GLEW_OK) {
      std::cout << "GLUT Error" << std::endl;
      return -1;
   }



   std::cout << glGetString(GL_VERSION) << std::endl;
   {
      GLCall(glEnable(GL_BLEND));
      GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      Renderer renderer;

      // Setup Dear ImGui context
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO(); (void)io;

      // Setup Dear ImGui style
      ImGui::StyleColorsDark();

      // Setup Platform/Renderer bindings
      ImGui_ImplGlfw_InitForOpenGL(window, true);
      ImGui_ImplOpenGL3_Init(glsl_version);

      tests::Test * currentTest = nullptr;
      tests::TestMenu * testMenu = new tests::TestMenu(currentTest);
      currentTest = testMenu;

      testMenu->RegisterTest<tests::TestClearColor>("Clear Color");
      while (!glfwWindowShouldClose(window))
      {
         GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
         renderer.Clear();

         ImGui_ImplOpenGL3_NewFrame();
         ImGui_ImplGlfw_NewFrame();
         ImGui::NewFrame();
         if (currentTest) {
            currentTest->OnUpdate(0.0f);
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<-"))
            {
               delete currentTest;
               currentTest = testMenu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
         }
         
         ImGui::Render();
         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
         GLCall(glfwSwapBuffers(window));
         glfwPollEvents();

      }

      if (currentTest != testMenu)
         delete testMenu;
      delete currentTest;

      //GLCall(glDeleteProgram(shader));
      //call VertexBuffer and IndexBuffer dtors
   }

   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwTerminate();
   return 0;
}