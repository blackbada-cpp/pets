#include "TestTexture2D.h"

#include "GL/glew.h"
#include "Renderer.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


float positions[] = {
   -50.0f, -50.0f, 0.0f, 0.0f, //0
    50.0f, -50.0f, 1.0f, 0.0f, //1
    50.0f,  50.0f, 1.0f, 1.0f, //2
   -50.0f,  50.0f, 0.0f, 1.0f //3
};
unsigned int indices[] = {
   0, 1, 2,
   2, 3, 0
};

namespace tests
{
   TestTexture2D::TestTexture2D()
      : m_translationA(200, 200, 0 )
      , m_translationB(400, 200, 0 )
      , m_proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f))
      , m_view(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))

   {

      GLCall(glEnable(GL_BLEND));
      GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      m_VAO = std::make_unique<VertexArray>();

      m_vertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

      VertexBufferLayout layout;
      layout.Push<float>(2);
      layout.Push<float>(2);
      //layout.Push<float>(2);
      m_VAO->AddBuffer(*m_vertexBuffer, layout);
      m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

      m_proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
      m_view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

      m_shader = std::make_unique<Shader>("res/shaders/basic.shader");
      m_shader->Bind();
      m_shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

      m_texture = std::make_unique<Texture>("res\\textures\\short-circuit.png");
      //m_texture->Bind(0); //slot #0
      m_shader->SetUniform1i("u_Texture", 0); // slot #0

      // m_VAO->Unbind();
      // vb.Unbind();
      // m_IndexBuffer->Unbind();
      // m_shader->Unbind();
   }

   TestTexture2D::~TestTexture2D()
   {

   }

   void TestTexture2D::OnUpdate(float deltaTime)
   {
      
   }

   void TestTexture2D::OnRender()
   {
      GLCall(glClearColor(0.0f,0.0f,0.0f,1.0f));
      GLCall(glClear(GL_COLOR_BUFFER_BIT));

      Renderer renderer;

      m_texture->Bind();

      {
         glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationA);
         glm::mat4 mvp = m_proj * m_view * model;
         m_shader->Bind();
         m_shader->SetUniformMat4f("u_MVP", mvp);
         renderer.Draw(*m_VAO, *m_IndexBuffer, *m_shader);
      }

      {
         glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationB);
         glm::mat4 mvp = m_proj * m_view * model;
         m_shader->Bind();
         m_shader->SetUniformMat4f("u_MVP", mvp);
         renderer.Draw(*m_VAO, *m_IndexBuffer, *m_shader);
      }
   }

   void TestTexture2D::OnImGuiRender()
   {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::SliderFloat3("Translation A", &m_translationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::SliderFloat3("Translation B", &m_translationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
   }

}