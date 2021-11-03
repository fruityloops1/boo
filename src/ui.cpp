#include <boo/config.h>
#include <boo/localization.h>
#include <boo/objparamdb.h>
#include <boo/stage.h>
#include <boo/randomizer.h>
#include <boo/ui.h>

#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <nfd.hpp>
#include <raylib.h>
#include <thread>

namespace boo::ui
{

    int boo::ui::UIContainer::GetEditorSelected() {return EditorSelected;}

    void boo::ui::UIContainer::StageFileOpen()
    {
        nfdchar_t *stagePath;
        nfdfilteritem_t filter[2] = {{"SZS", "szs"}, {"SARC", "sarc"}};
        nfdresult_t result = NFD::OpenDialog(stagePath, filter, 2, boo::Config::Get().StageDataPath.c_str());
        if (result == NFD_OKAY)
        {
            std::string sdp(stagePath);
            editors.push_back(boo::Editor());
            boo::Editor& neditor = editors[editors.size() - 1];
            
            neditor.LoadStage(sdp);

            NFD::FreePath(stagePath);
        }
    }

    void boo::ui::UIContainer::StageFileSave()
    {
        nfdchar_t *stagePath;
        nfdfilteritem_t filter[2] = {{"SZS", "szs"}, {"SARC", "sarc"}};
        nfdresult_t result = NFD::SaveDialog(stagePath, filter, 2, boo::Config::Get().StageDataPath.c_str());
        if (result == NFD_OKAY)
        {
            std::string sdp(stagePath);
            boo::Editor& neditor = editors[EditorSelected];
            
            neditor.SaveStage(sdp);

            NFD::FreePath(stagePath);
        }
    }

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

    void boo::ui::UIContainer::ShowStages()
    {
        if (ImGui::Begin(boo::Localization::GetLocalized("stages").c_str()))
        {
            if (editors.size() > 0)
            {
                ImGui::BeginTabBar("#stages");
                int sel = 0;
                for (boo::Editor& editor : editors)
                {
                    auto tab = [&editor, this, &sel]() -> int
                    {
                        ImGui::SliderInt(boo::Localization::GetLocalized("scenario").c_str(), &editor.CurrentScenario, 0, 14);
                        if (editor.CurrentScenario > editor.stage.data.entries.size() - 1) editor.CurrentScenario = editor.stage.data.entries.size() - 1;
                        if (editor.CurrentScenario < 0) editor.CurrentScenario = 0;
                        if (ImGui::Button(boo::Localization::GetLocalized("close_stage").c_str()))
                        {
                            if (editor.Changed) ImGui::OpenPopup("save_sure");
                            else return EditorSelected;
                        }
                        if (ImGui::BeginPopup("save_sure"))
                        {
                            ImGui::Text("%s", boo::Localization::GetLocalized("save_sure").c_str());
                            if (ImGui::Button(boo::Localization::GetLocalized("yes").c_str())) return EditorSelected;
                            ImGui::SameLine();
                            if (ImGui::Button(boo::Localization::GetLocalized("no").c_str()))
                            {
                                ImGui::CloseCurrentPopup();
                                return -1;
                            }
                            ImGui::EndPopup();
                        }
                        ImGui::EndTabItem();
                        EditorSelected = sel;
                        return -1;
                    };
                    std::string id = editor.stage.Name.c_str();
                    id.append(std::to_string(sel));
                    ImGui::PushID(id.c_str());
                    if (editor.Changed)
                    {
                        if (ImGui::BeginTabItem(editor.stage.Name.c_str(), NULL, ImGuiTabItemFlags_UnsavedDocument))
                        {
                            int t = tab();
                            if (t != -1)
                            {
                                editors.erase(editors.begin() + EditorSelected);
                                EditorSelected = 0;
                                break;
                            }
                        }
                    } else if (ImGui::BeginTabItem(editor.stage.Name.c_str()))
                    {
                        int t = tab();
                        if (t != -1)
                        {
                            editors.erase(editors.begin() + EditorSelected);
                            EditorSelected = 0;
                            break;
                        }
                    }
                        
                    ImGui::PopID();
                    sel++;
                }
                ImGui::EndTabBar();
            }
            else ImGui::Text("%s", boo::Localization::GetLocalized("no_stages").c_str());
            ImGui::End();
        }
        
    }

    void boo::ui::UIContainer::ShowMainMenuBar()
    {
        isEditorOpen = !editors.empty();
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu(boo::Localization::GetLocalized("file").c_str()))
            {
                if (ImGui::MenuItem(boo::Localization::GetLocalized("open").c_str(), boo::Localization::GetLocalized("open_shortcut").c_str())) {StageFileOpen();}
                if (isEditorOpen && ImGui::MenuItem(boo::Localization::GetLocalized("save").c_str(), boo::Localization::GetLocalized("save_shortcut").c_str())) {StageFileSave();}
                if (ImGui::MenuItem(boo::Localization::GetLocalized("exit").c_str(), boo::Localization::GetLocalized("exit_shortcut").c_str())) {TryExit();}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(boo::Localization::GetLocalized("view").c_str()))
            {
                if (ImGui::MenuItem(boo::Localization::GetLocalized("s_debug").c_str(), "", &DebugOpen)) {}
                if (ImGui::MenuItem(boo::Localization::GetLocalized("s_object_view").c_str(), "", &ObjectViewOpen)) {}
                if (ImGui::MenuItem(boo::Localization::GetLocalized("s_preferences").c_str(), "", &PreferencesOpen)) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(boo::Localization::GetLocalized("extras").c_str()))
            {
                if (ImGui::MenuItem(boo::Localization::GetLocalized("s_randomizer").c_str(), "", &RandomizerOpen)) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    bool boo::ui::UIContainer::ShowObject(std::vector<boo::Object*>& vo)
    {
        bool edited = false;
        if (ImGui::Begin(boo::Localization::GetLocalized("object").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (vo.size() > 1)
            {
                ImGui::Text("%s", boo::Localization::GetLocalized("multiple_objects").c_str());
            }
            else if (vo.size() == 1)
            {

                auto textbox = [&edited](std::string& value, const char* name)
                {
                    std::string vc = std::string(value);
                    if (ImGui::InputText(name, &vc, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        edited = true;
                        value = std::string(vc);
                    }
                };

                auto textbox_fake = [](std::string& value, const char* name)
                {
                    std::string vc = std::string(value);
                    ImGui::InputText(name, &vc);
                };

                std::string vc = std::string(vo[0]->Id);
                if (ImGui::InputText(boo::Localization::GetLocalized("param_Id").c_str(), &vc, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    boo::Editor& editor = editors[EditorSelected];
                    boo::Object* to = editor.stage.data.FindObject(vc, editor.CurrentScenario);
                    if (!to)
                    {
                        edited = true;
                        auto i = std::find(editor.cursel.begin(), editor.cursel.end(), vo[0]->Id);
                        editor.cursel.erase(i);
                        editor.cursel.push_back(vc);
                        vo[0]->Id = std::string(vc);
                    }          
                }

                textbox(vo[0]->LayerConfigName, boo::Localization::GetLocalized("param_LayerConfigName").c_str());
                textbox(vo[0]->UnitConfigName, boo::Localization::GetLocalized("param_UnitConfigName").c_str());
                textbox(vo[0]->UnitConfig.ParameterConfigName, boo::Localization::GetLocalized("param_ParameterConfigName").c_str());
                textbox(vo[0]->ModelName, boo::Localization::GetLocalized("param_ModelName").c_str());
                textbox(vo[0]->UnitConfig.DisplayName, boo::Localization::GetLocalized("param_DisplayName").c_str());
                textbox(vo[0]->comment, boo::Localization::GetLocalized("param_comment").c_str());

                float pos[3];
                pos[0] = vo[0]->Translate.x;
                pos[1] = vo[0]->Translate.y;
                pos[2] = vo[0]->Translate.z;
                if (ImGui::DragFloat3(boo::Localization::GetLocalized("position").c_str(), pos, 0.75))
                {
                    vo[0]->Translate.x = pos[0];
                    vo[0]->Translate.y = pos[1];
                    vo[0]->Translate.z = pos[2];
                    edited = true;
                }

                float rot[3];
                rot[0] = vo[0]->Rotate.x;
                rot[1] = vo[0]->Rotate.y;
                rot[2] = vo[0]->Rotate.z;
                if (ImGui::DragFloat3(boo::Localization::GetLocalized("rotation").c_str(), rot, 0.5, -180, 180))
                {
                    vo[0]->Rotate.x = rot[0];
                    vo[0]->Rotate.y = rot[1];
                    vo[0]->Rotate.z = rot[2];
                    edited = true;
                }

                float scale[3];
                scale[0] = vo[0]->Scale.x;
                scale[1] = vo[0]->Scale.y;
                scale[2] = vo[0]->Scale.z;
                if (ImGui::DragFloat3(boo::Localization::GetLocalized("scale").c_str(), scale, 0.01))
                {
                    for (u8 i = 0; i < 3; i++) if (scale[i] < 0) scale[i] = 0;
                    vo[0]->Scale.x = scale[0];
                    vo[0]->Scale.y = scale[1];
                    vo[0]->Scale.z = scale[2];
                    edited = true;
                }

                bool has_properties = !vo[0]->extra_params.empty();

                if (has_properties) ImGui::Separator();

                if (has_properties && ImGui::TreeNode(boo::Localization::GetLocalized("properties").c_str()))
                {
                    for (auto property = vo[0]->extra_params.begin(); property != vo[0]->extra_params.end(); ++property)
                    {
                        if (property->second.GetType() == oead::Byml::Type::Bool)
                        {
                            bool v = property->second.GetBool();
                            if (ImGui::Checkbox(property->first.c_str(), &v))
                            {
                                property->second = oead::Byml(v);
                                edited = true;
                            }
                        }
                        else if (property->second.GetType() == oead::Byml::Type::Int)
                        {
                            int v = property->second.GetInt();
                            if (ImGui::InputInt(property->first.c_str(), &v, 0))
                            {
                                property->second = oead::Byml(oead::Number<int>(v));
                                edited = true;
                            }
                        }
                        else if (property->second.GetType() == oead::Byml::Type::Float)
                        {
                            float v = property->second.GetFloat();
                            if (ImGui::InputFloat(property->first.c_str(), &v, 0))
                            {
                                property->second = oead::Byml(oead::Number<float>(v));
                                edited = true;
                            }
                        }
                        else if (property->second.GetType() == oead::Byml::Type::String)
                        {
                            textbox(property->second.GetString(), property->first.c_str());
                        }
                    }
                    ImGui::TreePop();
                }
            }
            else
            {
                ImGui::Text("%s", boo::Localization::GetLocalized("no_object").c_str());
            }
            ImGui::End();
        }
        return edited;
    }

    void boo::ui::UIContainer::ShowObjectView()
    {
        std::vector<boo::Object*> vo;
        if (ObjectViewOpen && editors.size() > 0 && ImGui::Begin(boo::Localization::GetLocalized("object_view").c_str()))
        {
            ImGui::PushID(editors[EditorSelected].stage.Name.c_str());
            boo::Editor& editor = editors[EditorSelected];
            boo::StageData& sd = editors[EditorSelected].stage.data;

            ImGui::InputText(boo::Localization::GetLocalized("filter").c_str(), &editor.filter);

            for (auto ol = sd.entries[editors[EditorSelected].CurrentScenario].object_lists.begin(); ol != sd.entries[editors[EditorSelected].CurrentScenario].object_lists.end(); ++ol)
            {
                bool expanded = ImGui::TreeNode(ol->first.c_str());
                for (boo::Object& object : ol->second.objects)
                {
                    auto i = std::find(editor.cursel.begin(), editor.cursel.end(), object.Id);
                    if (i != editor.cursel.end())
                    {
                        if (expanded)
                        {
                            ImGui::PushID(object.Id.c_str());
                            auto f = [&object, &editor, &i]()
                            {
                                if (ImGui::Selectable(object.UnitConfigName.c_str(), true))
                                {
                                    if (!IsKeyDown(KEY_LEFT_SHIFT)) editor.cursel.clear();
                                    else editor.cursel.erase(i);
                                }
                            };
                            if (editor.filter.empty())
                                f();
                            else if (object.UnitConfigName.find(editor.filter) != std::string::npos)
                                f();
                            ImGui::PopID();
                        }
                    }
                    else
                    {
                        if (expanded)
                        {
                            ImGui::PushID(object.Id.c_str());
                            auto f = [&object, &editor, &i]()
                            {
                                if (ImGui::Selectable(object.UnitConfigName.c_str(), false))
                                {
                                    if (!IsKeyDown(KEY_LEFT_SHIFT)) editor.cursel.clear();
                                    editor.cursel.push_back(object.Id);
                                }
                            };
                            if (editor.filter.empty())
                                f();
                            else if (object.UnitConfigName.find(editor.filter) != std::string::npos)
                                f();
                            ImGui::PopID();
                        }
                    }
                    if (i != editor.cursel.end())
                        vo.push_back(&object);
                }
                if (expanded) ImGui::TreePop();
            }
            ImGui::End();
            ImGui::PopID();
            if (ShowObject(vo)) editor.Changed = true;
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

    void boo::ui::UIContainer::ShowRandomizer()
    {
        if (RandomizerOpen && ImGui::Begin(boo::Localization::GetLocalized("s_randomizer").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            boo::Config& config = boo::Config::Get();

            static bool ip = false;
            static char stagedata_path[255]; 
            static char output_path[255];

            static bool shines = true;
            static bool scenarios = false;

            if (ip)
            {
                static std::string progress;
                static bool init = false;

                if (!init)
                {
                    std::thread randomizer_thread(boo::Randomizer::RandomizeLZ, shines, scenarios, std::string(stagedata_path), std::string(output_path), &progress);
                    randomizer_thread.detach();
                    init = true;
                }

                ImGui::Text("%s", progress.c_str());

                if (progress == "Done")
                {
                    init = false;
                    ip = false;
                    progress = std::string();
                }

            }
            else
            {

                ImGui::TextWrapped("%s", boo::Localization::GetLocalized("randomizer_notice").c_str());

                static bool stagedata_exists = false; static bool sds = false;
                static bool output_exists = false; static bool sdos = false;
                static bool restart_changes = false;
                if (!sds)
                {
                    std::copy(config.StageDataPath.begin(), config.StageDataPath.end(), stagedata_path);
                    stagedata_exists = std::filesystem::exists(stagedata_path);
                    sds = true;
                }
                if (!sdos)
                {
                    std::copy(config.ObjectDataPath.begin(), config.ObjectDataPath.end(), output_path);
                    output_exists = std::filesystem::exists(output_path);
                    sdos = true;
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
            
                if (ImGui::InputText(boo::Localization::GetLocalized("output_folder").c_str(), output_path, IM_ARRAYSIZE(output_path)))
                {
                    output_exists = std::filesystem::exists(output_path);
                    changed = true;
                    restart_changes = true;
                }
                ImGui::PushID("stageoutputbrowse");
                ImGui::SameLine();
                if (ImGui::Button(boo::Localization::GetLocalized("browse").c_str()))
                {
                    nfdchar_t *sdoPath;
                    nfdresult_t result = NFD::PickFolder(sdoPath, ".");
                    if (result == NFD_OKAY)
                    {
                        std::string odp(sdoPath);
                        std::fill(output_path, output_path + 255, 0);
                        std::copy(odp.begin(), odp.end(), output_path);
                        NFD::FreePath(sdoPath);
                        output_exists = std::filesystem::exists(output_path);
                        changed = true;
                        restart_changes = true;
                    }
                }

                if (!output_exists)
                    ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::GetLocalized("not_exist").c_str());

                ImGui::Checkbox(boo::Localization::GetLocalized("randomize_shines").c_str(), &shines);
                ImGui::Checkbox(boo::Localization::GetLocalized("randomize_scenarios").c_str(), &scenarios);

                ImGui::PopID();
                if (changed && stagedata_exists && output_exists && ImGui::Button(boo::Localization::GetLocalized("ok").c_str()))
                    ip = true;
            }
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