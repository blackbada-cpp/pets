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
      // float positions[] = {
      //    -0.5f, -0.5f, //0
      //     0.5f, -0.5f, //1
      //     0.5f,  0.5f, //2
      //    -0.5f,  0.5f  //3
      // };
      //dp float positions[] = {
      //dp    -0.5f, -0.5f, 0.0f, 0.0f, //0
      //dp     0.5f, -0.5f, 1.0f, 0.0f, //1
      //dp     0.5f,  0.5f, 1.0f, 1.0f, //2
      //dp    -0.5f,  0.5f, 0.0f, 1.0f //3
      //dp };
      float positions[] = {
         100.0f, 100.0f, 0.0f, 0.0f, //0
         200.0f, 100.0f, 1.0f, 0.0f, //1
         200.0f, 200.0f, 1.0f, 1.0f, //2
         100.0f, 200.0f, 0.0f, 1.0f //3
      };
      unsigned int indices[] = {
         0, 1, 2,
         2, 3, 0
      };

      GLCall(glEnable(GL_BLEND));
      GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      VertexArray va;
      VertexBuffer vb(positions, 4 * 4 * sizeof(float));

      VertexBufferLayout layout;
      layout.Push<float>(2);
      layout.Push<float>(2);
      //layout.Push<float>(2);
      va.AddBuffer(vb, layout);

      IndexBuffer ib(indices, 6);

      //dp glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
      glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
      glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));

      Shader shader("res/shaders/basic.shader");
      shader.Bind();
      shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

      Texture texture("res\\textures\\short-circuit.png");
      texture.Bind(0); //slot #0
      shader.SetUniform1i("u_Texture", 0); // slot #0

      va.Unbind();
      vb.Unbind();
      ib.Unbind();
      shader.Unbind();

      Renderer renderer;

      // Setup Dear ImGui context
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO(); (void)io;
      //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
      //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

      // Setup Dear ImGui style
      ImGui::StyleColorsDark();
      //ImGui::StyleColorsClassic();

      glm::vec3 translation(200, 200, 0);

      // Setup Platform/Renderer bindings
      ImGui_ImplGlfw_InitForOpenGL(window, true);
      ImGui_ImplOpenGL3_Init(glsl_version);


      float r = 0.0f;
      float increment = 0.05f;
      /* Loop until the user closes the window */
      while (!glfwWindowShouldClose(window))
      {
         /* Render here */
         renderer.Clear();

         ImGui_ImplOpenGL3_NewFrame();
         ImGui_ImplGlfw_NewFrame();
         ImGui::NewFrame();

         glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
         glm::mat4 mvp = proj * view * model;

         shader.Bind();
         //shader.SetUniform4f("u_Color", r, 0.3f, 0.0f, 1.0f);
         shader.SetUniform1i("u_Texture", 0); // slot #0
         shader.SetUniformMat4f("u_MVP", mvp);
         texture.Bind(0);

         //va.Bind();
         //ib.Bind();
         renderer.Draw(va, ib, shader);

         // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
         {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
         }

         // Rendering GUI
         ImGui::Render();
         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

         /* Swap front and back buffers */
         GLCall(glfwSwapBuffers(window));

         /* Poll for and process events */
         glfwPollEvents();

         r += increment;
         if (r > 1.0f)
         {
            increment = -abs(increment);
         }
         else if (r < 0.0f)
         {
            increment = abs(increment);
         }

         //dp std::this_thread::sleep_for(std::chrono::milliseconds(5));
      }

      //GLCall(glDeleteProgram(shader));
      //call VertexBuffer and IndexBuffer dtors
   }

   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwTerminate();
   return 0;
}