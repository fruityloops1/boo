#include <boo/ui.h>
#include <imgui.h>
#include <string>

namespace boo::ui
{
    void boo::ui::UIContainer::ShowDebug()
    {
        if (DebugOpen && ImGui::Begin("Debug"))
        {
            ImGui::End();
        }
    }

    void boo::ui::UIContainer::ShowMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Show Debug Menu", "", &DebugOpen)) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

}