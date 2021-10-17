#include <boo/config.h>
#include <boo/localization.h>
#include <boo/ui.h>
#include <filesystem>
#include <imgui.h>
#include <raylib.h>
#include <nfd.hpp>

namespace boo::ui
{
    void boo::ui::UIContainer::ShowDebug()
    {
        if (DebugOpen && ImGui::Begin(boo::Localization::GetLocalized("debug").c_str()))
        {
            std::string fps = std::to_string(GetFPS());
            fps.append(" FPS");
            ImGui::Text("%s", fps.c_str());
            ImGui::End();
        }
    }

    void boo::ui::UIContainer::ShowMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu(boo::Localization::GetLocalized("view").c_str()))
            {
                if (ImGui::MenuItem(boo::Localization::GetLocalized("s_debug").c_str(), "", &DebugOpen)) {}
                if (ImGui::MenuItem(boo::Localization::GetLocalized("s_object_view").c_str(), "", &ObjectViewOpen)) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void boo::ui::UIContainer::ShowObjectView(boo::StageData& sd, int scenario)
    {
        if (ObjectViewOpen && ImGui::Begin(boo::Localization::GetLocalized("object_view").c_str()))
        {
            for (auto ol = sd.entries[scenario].object_lists.begin(); ol != sd.entries[scenario].object_lists.end(); ++ol)
            {
                if (ImGui::TreeNode(ol->first.c_str()))
                {
                    for (boo::Object object : ol->second.objects)
                    {
                        if (ImGui::Selectable(object.UnitConfig.ParameterConfigName.c_str(), false))
                        {

                        }
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::End();
        }
    }

    bool boo::ui::UIContainer::ShowStageDataFileSelectPopup()
    {
        ImGui::OpenPopup(boo::Localization::GetLocalized("select_stage_data").c_str());
        if (ImGui::BeginPopupModal(boo::Localization::GetLocalized("select_stage_data").c_str()))
        {
            static char temp_path[255];
            static bool exists = false;
            ImGui::Text("%s", boo::Localization::GetLocalized("stage_data_notice").c_str());
            if (ImGui::Button(boo::Localization::GetLocalized("browse").c_str()))
            {
                nfdchar_t *sdPath;
                nfdresult_t result = NFD::PickFolder(sdPath, ".");
                if (result == NFD_OKAY)
                {
                    std::string sdp(sdPath);
                    std::fill(temp_path, temp_path + 255, 0);
                    std::copy(sdp.begin(), sdp.end(), temp_path);
                    NFD::FreePath(sdPath);
                    exists = std::filesystem::exists(temp_path);
                }
            }
            ImGui::SameLine();
            if (ImGui::InputText(boo::Localization::GetLocalized("stage_data_folder").c_str(), temp_path, IM_ARRAYSIZE(temp_path)))
            {
                exists = std::filesystem::exists(temp_path);
            }
            if (!exists)
                ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::GetLocalized("not_exist").c_str());
            else if (ImGui::Button(boo::Localization::GetLocalized("ok").c_str()))
            {
                boo::Config::Get().StageDataPath = temp_path;
                boo::Config::Get().Save("boo.ini");
                return true;
            }
            ImGui::EndPopup();
        }
        return false;
    }
}