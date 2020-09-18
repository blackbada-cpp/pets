#pragma once
#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>

namespace tests
{
   class TestTexture2D : public Test
   {
   private:
      glm::vec3 m_translationA; //(200, 200, 0);
      glm::vec3 m_translationB; //(400, 200, 0);
      std::unique_ptr<VertexArray> m_VAO;
      std::unique_ptr<VertexBuffer> m_vertexBuffer;
      std::unique_ptr<IndexBuffer> m_IndexBuffer;
      std::unique_ptr<Shader>      m_shader;
      std::unique_ptr<Texture>     m_texture;
      glm::mat4 m_proj, m_view;// = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);


   public:
      TestTexture2D();
      ~TestTexture2D();
      void OnUpdate(float deltaTime) override;
      void OnRender() override;
      void OnImGuiRender() override;

   };
}