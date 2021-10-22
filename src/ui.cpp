#include <boo/config.h>
#include <boo/localization.h>
#include <boo/objparamdb.h>
#include <boo/ui.h>

#include <filesystem>
#include <imgui.h>
#include <nfd.hpp>
#include <raylib.h>
#include <thread>

namespace boo::ui
{
    bool boo::ui::UIContainer::IsExit()
    {
        return Exit;
    }
    
    void boo::ui::UIContainer::ShowDebug()
    {
        if (DebugOpen && ImGui::Begin(boo::Localization::GetLocalized("debug").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
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

            if (ImGui::BeginMenu(boo::Localization::GetLocalized("file").c_str()))
            {
                if (ImGui::MenuItem(boo::Localization::GetLocalized("exit").c_str(), boo::Localization::GetLocalized("save_shortcut").c_str())) {TryExit();}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(boo::Localization::GetLocalized("view").c_str()))
            {
                if (ImGui::MenuItem(boo::Localization::GetLocalized("s_debug").c_str(), "", &DebugOpen)) {}
                if (ImGui::MenuItem(boo::Localization::GetLocalized("s_object_view").c_str(), "", &ObjectViewOpen)) {}
                if (ImGui::MenuItem(boo::Localization::GetLocalized("s_preferences").c_str(), "", &PreferencesOpen)) {}
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

    void boo::ui::UIContainer::ShowPreferences()
    {
        if (PreferencesOpen && ImGui::Begin(boo::Localization::GetLocalized("s_preferences").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {

            boo::Config& config = boo::Config::Get();

            static char stagedata_path[255]; static bool stagedata_exists = false; static bool sds = false;
            static char objectdata_path[255]; static bool objectdata_exists = false; static bool ods = false;
            static bool restart_changes = false;
            if (!sds)
            {
                std::copy(config.StageDataPath.begin(), config.StageDataPath.end(), stagedata_path);
                stagedata_exists = std::filesystem::exists(stagedata_path);
                sds = true;
            }
            if (!ods)
            {
                std::copy(config.ObjectDataPath.begin(), config.ObjectDataPath.end(), objectdata_path);
                objectdata_exists = std::filesystem::exists(objectdata_path);
                ods = true;
            }

            static bool changed = false;

            if (ImGui::InputText(boo::Localization::GetLocalized("stage_data_folder").c_str(), stagedata_path, IM_ARRAYSIZE(stagedata_path)))
            {
                stagedata_exists = std::filesystem::exists(stagedata_path);
                changed = true;
                restart_changes = true;
            }
            ImGui::PushID("stagebrowse");
            ImGui::SameLine();
            if (ImGui::Button(boo::Localization::GetLocalized("browse").c_str()))
            {
                nfdchar_t *sdPath;
                nfdresult_t result = NFD::PickFolder(sdPath, ".");
                if (result == NFD_OKAY)
                {
                    std::string sdp(sdPath);
                    std::fill(stagedata_path, stagedata_path + 255, 0);
                    std::copy(sdp.begin(), sdp.end(), stagedata_path);
                    NFD::FreePath(sdPath);
                    stagedata_exists = std::filesystem::exists(stagedata_path);
                    changed = true;
                    restart_changes = true;
                }
            }
            ImGui::PopID();
            if (!stagedata_exists)
                ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::GetLocalized("not_exist").c_str());
            
            if (ImGui::InputText(boo::Localization::GetLocalized("object_data_folder").c_str(), objectdata_path, IM_ARRAYSIZE(objectdata_path)))
            {
                objectdata_exists = std::filesystem::exists(objectdata_path);
                changed = true;
                restart_changes = true;
            }
            ImGui::PushID("objectbrowse");
            ImGui::SameLine();
            if (ImGui::Button(boo::Localization::GetLocalized("browse").c_str()))
            {
                nfdchar_t *odPath;
                nfdresult_t result = NFD::PickFolder(odPath, ".");
                if (result == NFD_OKAY)
                {
                    std::string odp(odPath);
                    std::fill(objectdata_path, objectdata_path + 255, 0);
                    std::copy(odp.begin(), odp.end(), objectdata_path);
                    NFD::FreePath(odPath);
                    objectdata_exists = std::filesystem::exists(objectdata_path);
                    changed = true;
                    restart_changes = true;
                }
            }
            ImGui::PopID();
            /*if (!objectdata_exists)
                ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::GetLocalized("not_exist").c_str());*/
            
            static int lang_current = config.language;
            static const char* langs[] = {"English", "German"};
            if (ImGui::Combo(boo::Localization::GetLocalized("language").c_str(), &lang_current, langs, IM_ARRAYSIZE(langs)))
            {
                config.language = lang_current;
                boo::Localization::SetLanguage((u8) lang_current);
                SetWindowTitle(boo::Localization::GetLocalized("title").c_str());
                changed = true;
            }

            static int fontsize = config.fontsize;
            
            if (ImGui::SliderInt(boo::Localization::GetLocalized("fontsize").c_str(), &fontsize, 8, 56))
            {
                changed = true;
                restart_changes = true;
            }
            
            if (changed && stagedata_exists && ImGui::Button(boo::Localization::GetLocalized("save").c_str()))
            {
                config.fontsize = fontsize;
                config.language = (u8) lang_current;
                config.StageDataPath = std::string(stagedata_path);
                config.ObjectDataPath = std::string(objectdata_path);
                config.Save("boo.ini");
                changed = false;
            }

            if (restart_changes) ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::GetLocalized("restart_changes").c_str());
            ImGui::End();
        }
    }

    bool boo::ui::UIContainer::ShowStageDataFileSelectPopup()
    {
        ImGuiIO& io = ImGui::GetIO();
        static bool scale_pos = true;
        if (scale_pos)
        {
            boo::Config& c = boo::Config::Get();
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.36f * ((float) c.Get().fontsize / 16), io.DisplaySize.y * 0.15f * ((float) c.Get().fontsize / 16)));
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
            scale_pos = false;
        }
        ImGui::OpenPopup(boo::Localization::GetLocalized("select_stage_data").c_str());
        if (ImGui::BeginPopupModal(boo::Localization::GetLocalized("select_stage_data").c_str()))
        {
            static char temp_path[255];
            static bool exists = false;
            ImGui::TextWrapped("%s", boo::Localization::GetLocalized("stage_data_notice").c_str());
            ImGui::Spacing();
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
                exists = std::filesystem::exists(temp_path);
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

    void boo::ui::UIContainer::ShowOPDBGeneratePopup()
    {
        ImGuiIO& io = ImGui::GetIO();
        static bool scale_pos = true;
        static bool running = false;
        if (scale_pos)
        {
            boo::Config& c = boo::Config::Get();
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.36f * ((float) c.Get().fontsize / 16), io.DisplaySize.y * 0.15f * ((float) c.Get().fontsize / 16)));
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
            scale_pos = false;
        }
        ImGui::OpenPopup(boo::Localization::GetLocalized("generate_opdb").c_str());
        if (ImGui::BeginPopupModal(boo::Localization::GetLocalized("generate_opdb").c_str()))
        {
            if (!running)
            {
                ImGui::TextWrapped("%s", boo::Localization::GetLocalized("generate_notice").c_str());
                ImGui::Spacing();
                if (ImGui::Button(boo::Localization::GetLocalized("generate").c_str()))
                {
                    running = true;
                }
            }
            else
            {
                static std::string done_file_name;
                auto thread = []()
                {
                    boo::ObjectParameterDatabase& opdb = boo::ObjectParameterDatabase::Get();
                    for (const auto& sf : std::filesystem::directory_iterator(boo::Config::Get().StageDataPath))
                    {
                        if (sf.is_directory()) continue;
                        opdb.GenerateFromFile(sf.path(), done_file_name);
                    }
                    opdb.loaded = true;
                    opdb.Save("db.opdb");
                };
                static bool thread_started = false;
                if (!thread_started)
                {
                    static std::thread generator(thread);
                    generator.detach();
                    thread_started = true;
                }
                ImGui::Text("%s", boo::Localization::GetLocalized("generating").c_str());
                ImGui::Text("%s", boo::Localization::GetLocalized("generating_info").c_str());
                ImGui::Spacing();
                ImGui::Text("%s", done_file_name.c_str());
            }
            ImGui::EndPopup();
        }
    }

    void boo::ui::UIContainer::TryExit()
    {
        Exit = true;
    }
}