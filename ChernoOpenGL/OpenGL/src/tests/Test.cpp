#include "Test.h"
#include "imgui/imgui.h"

namespace tests
{

   TestMenu::TestMenu(Test *& currentTestPtr)
      : m_CurrenTest(currentTestPtr)
   {

   }


   void TestMenu::OnImGuiRender()
   {
      for(auto & test : m_Tests)
      {
         if (ImGui::Button(test.first.c_str()))
            m_CurrenTest = test.second();
;      }
   }

}