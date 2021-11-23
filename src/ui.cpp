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

    void boo::ui::UIContainer::StatusBar::update()
    {
        if (framesUntilClose > 0)
        {
            framesUntilClose--;

            ImGui::Separator();

            if (isError)
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "%s", message.c_str());
            else
                ImGui::Text("%s", message.c_str());
        }
    }

    void boo::ui::UIContainer::StatusBar::info(std::string message)
    {
        isError = false;
        this->message = std::string(message);
        framesUntilClose = 300;
    }

    void boo::ui::UIContainer::StatusBar::error(std::string message)
    {
        isError = true;
        this->message = std::string(message);
        framesUntilClose = 300;
    }

    int boo::ui::UIContainer::getEditorSelected() {return editorSelected;}

    void boo::ui::UIContainer::stageFileOpen()
    {
        nfdchar_t *stagePath;
        nfdfilteritem_t filter[2] = {{"SZS", "szs"}, {"SARC", "sarc"}};
        nfdresult_t result = NFD::OpenDialog(stagePath, filter, 2, boo::Config::Get().stageDataPath.c_str());
        if (result == NFD_OKAY)
        {
            std::string sdp(stagePath);
            editors.push_back(boo::Editor());
            boo::Editor& neditor = editors[editors.size() - 1];
            
            neditor.loadStage(sdp);

            std::string status_string = boo::Localization::getLocalized("loaded_s");
            status_string.append(neditor.stage.name);

            statusBar.info(status_string.c_str());

            NFD::FreePath(stagePath);
        }
    }

    void boo::ui::UIContainer::stageFileSave(bool as)
    {
        boo::Editor& neditor = editors[editorSelected];

        auto infoSaved = [this, &neditor]()
        {
            std::string status_string = boo::Localization::getLocalized("saved_s");
            status_string.append(neditor.stage.name);
            statusBar.info(status_string.c_str());
        };

        if (!neditor.lastSaved.has_parent_path() || neditor.lastSaved.empty() || as)
        {
            nfdchar_t *stagePath;
            nfdfilteritem_t filter[2] = {{"SZS", "szs"}, {"SARC", "sarc"}};
            nfdresult_t result = NFD::SaveDialog(stagePath, filter, 2, boo::Config::Get().stageDataPath.c_str());
            if (result == NFD_OKAY)
            {
                std::string sdp(stagePath);
            
                neditor.saveStage(sdp);
                neditor.lastSaved = std::filesystem::path(sdp);

                infoSaved();

                NFD::FreePath(stagePath);
            }
            return;
        }

        neditor.saveStage(neditor.lastSaved);
        infoSaved();
        
    }

    bool boo::ui::UIContainer::isExit()
    {
        return exit;
    }
    
    void boo::ui::UIContainer::showDebug()
    {
        if (debugOpen && ImGui::Begin(boo::Localization::getLocalized("debug").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            std::string fps = std::to_string(GetFPS());
            fps.append(" FPS");
            ImGui::Text("%s", fps.c_str());
            ImGui::End();
        }
    }

    void boo::ui::UIContainer::showStages()
    {
        if (ImGui::Begin(boo::Localization::getLocalized("stages").c_str()))
        {
            if (editors.size() > 0)
            {
                ImGui::BeginTabBar("#stages");
                int sel = 0;
                for (boo::Editor& editor : editors)
                {
                    auto tab = [&editor, this, &sel]() -> int
                    {
                        ImGui::SliderInt(boo::Localization::getLocalized("scenario").c_str(), &editor.currentScenario, 0, 14);
                        if (editor.currentScenario > editor.stage.data.entries.size() - 1) editor.currentScenario = editor.stage.data.entries.size() - 1;
                        if (editor.currentScenario < 0) editor.currentScenario = 0;
                        if (ImGui::Button(boo::Localization::getLocalized("close_stage").c_str()))
                        {
                            if (editor.changed) ImGui::OpenPopup("save_sure");
                            else return editorSelected;
                        }
                        if (ImGui::BeginPopup("save_sure"))
                        {
                            ImGui::Text("%s", boo::Localization::getLocalized("save_sure").c_str());
                            if (ImGui::Button(boo::Localization::getLocalized("yes").c_str())) return editorSelected;
                            ImGui::SameLine();
                            if (ImGui::Button(boo::Localization::getLocalized("no").c_str()))
                            {
                                ImGui::CloseCurrentPopup();
                                return -1;
                            }
                            ImGui::EndPopup();
                        }
                        ImGui::EndTabItem();
                        editorSelected = sel;
                        return -1;
                    };
                    std::string id = editor.stage.name.c_str();
                    id.append(std::to_string(sel));
                    ImGui::PushID(id.c_str());
                    if (editor.changed)
                    {
                        if (ImGui::BeginTabItem(editor.stage.name.c_str(), NULL, ImGuiTabItemFlags_UnsavedDocument))
                        {
                            int t = tab();
                            if (t != -1)
                            {
                                std::string status_message = boo::Localization::getLocalized("closed_s");
                                status_message.append(editors[editorSelected].stage.name);
                                statusBar.info(status_message);

                                editors.erase(editors.begin() + editorSelected);
                                editorSelected = 0;
                                break;
                            }
                        }
                    } else if (ImGui::BeginTabItem(editor.stage.name.c_str()))
                    {
                        int t = tab();
                        if (t != -1)
                        {
                            std::string status_message = boo::Localization::getLocalized("closed_s");
                            status_message.append(editors[editorSelected].stage.name);
                            statusBar.info(status_message);

                            editors.erase(editors.begin() + editorSelected);
                            editorSelected = 0;
                            break;
                        }
                    }
                        
                    ImGui::PopID();
                    sel++;
                }
                ImGui::EndTabBar();
            }
            else ImGui::Text("%s", boo::Localization::getLocalized("no_stages").c_str());
            ImGui::End();
        }
        
    }

    void boo::ui::UIContainer::showMainMenuBar()
    {
        isEditorOpen = !editors.empty();
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu(boo::Localization::getLocalized("file").c_str()))
            {
                if (ImGui::MenuItem(boo::Localization::getLocalized("open").c_str(), boo::Localization::getLocalized("open_shortcut").c_str())) stageFileOpen();
                if (isEditorOpen)
                {
                    if (ImGui::MenuItem(boo::Localization::getLocalized("save").c_str(), boo::Localization::getLocalized("save_shortcut").c_str())) stageFileSave(false);
                    if (ImGui::MenuItem(boo::Localization::getLocalized("save_as").c_str(), boo::Localization::getLocalized("save_as_shortcut").c_str())) stageFileSave(true);
                }
                if (ImGui::MenuItem(boo::Localization::getLocalized("exit").c_str(), boo::Localization::getLocalized("exit_shortcut").c_str())) tryExit();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(boo::Localization::getLocalized("view").c_str()))
            {
                if (ImGui::MenuItem(boo::Localization::getLocalized("s_debug").c_str(), "", &debugOpen)) {}
                if (ImGui::MenuItem(boo::Localization::getLocalized("s_object_view").c_str(), "", &objectViewOpen)) {}
                if (ImGui::MenuItem(boo::Localization::getLocalized("s_preferences").c_str(), "", &preferencesOpen)) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(boo::Localization::getLocalized("extras").c_str()))
            {
                if (ImGui::MenuItem(boo::Localization::getLocalized("s_randomizer").c_str(), "", &randomizerOpen)) {}
                ImGui::EndMenu();
            }
            statusBar.update();
            ImGui::EndMainMenuBar();
        }
    }

    bool boo::ui::UIContainer::showObject(std::vector<boo::Object*>& vo)
    {
        bool edited = false;
        if (ImGui::Begin(boo::Localization::getLocalized("object").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            boo::Editor& editor = editors[editorSelected];
            if (vo.size() > 1)
            {
                ImGui::Text("%s", boo::Localization::getLocalized("multiple_objects").c_str());
            }
            else if (vo.size() == 1)
            {

                if (!editor.link_return.empty())
                    if (ImGui::Button(boo::Localization::getLocalized("return").c_str()))
                    {
                        editor.cursel.clear();
                        editor.cursel.push_back(editor.link_return[0]);
                        editor.link_return.pop_front();
                    }

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
                if (ImGui::InputText(boo::Localization::getLocalized("param_Id").c_str(), &vc, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    boo::Object* to = editor.stage.data.findObject(vc, editor.currentScenario);
                    if (!to)
                    {
                        edited = true;
                        auto i = std::find(editor.cursel.begin(), editor.cursel.end(), vo[0]->Id);
                        editor.cursel.erase(i);
                        editor.cursel.push_back(vc);
                        vo[0]->Id = std::string(vc);
                    }
                    else
                        statusBar.error(boo::Localization::getLocalized("object_already_exists"));
                }

                textbox(vo[0]->LayerConfigName, boo::Localization::getLocalized("param_LayerConfigName").c_str());
                textbox(vo[0]->UnitConfigName, boo::Localization::getLocalized("param_UnitConfigName").c_str());
                textbox(vo[0]->UnitConfig.ParameterConfigName, boo::Localization::getLocalized("param_ParameterConfigName").c_str());
                textbox(vo[0]->ModelName, boo::Localization::getLocalized("param_ModelName").c_str());
                textbox(vo[0]->UnitConfig.DisplayName, boo::Localization::getLocalized("param_DisplayName").c_str());
                textbox(vo[0]->comment, boo::Localization::getLocalized("param_comment").c_str());

                float pos[3];
                pos[0] = vo[0]->Translate.x;
                pos[1] = vo[0]->Translate.y;
                pos[2] = vo[0]->Translate.z;
                if (ImGui::DragFloat3(boo::Localization::getLocalized("position").c_str(), pos, 0.75))
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
                if (ImGui::DragFloat3(boo::Localization::getLocalized("rotation").c_str(), rot, 0.5, -180, 180))
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
                if (ImGui::DragFloat3(boo::Localization::getLocalized("scale").c_str(), scale, 0.01))
                {
                    for (u8 i = 0; i < 3; i++) if (scale[i] < 0) scale[i] = 0;
                    vo[0]->Scale.x = scale[0];
                    vo[0]->Scale.y = scale[1];
                    vo[0]->Scale.z = scale[2];
                    edited = true;
                }

                bool has_properties = !vo[0]->extra_params.empty();
                if (has_properties) ImGui::Separator();

                if (has_properties && ImGui::TreeNode(boo::Localization::getLocalized("properties").c_str()))
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

                bool has_links = !vo[0]->Links.empty();
                if (has_links) ImGui::Separator();

                if (has_links && ImGui::TreeNode(boo::Localization::getLocalized("links").c_str()))
                {
                    for (auto link = vo[0]->Links.begin(); link != vo[0]->Links.end(); ++link)
                    {
                        if (ImGui::TreeNode(link->first.c_str()))
                        {
                            for (boo::Object& object : link->second)
                            {
                                if (ImGui::Selectable(object.UnitConfigName.c_str()))
                                {
                                    editor.cursel.clear();
                                    editor.cursel.push_back(object.Id);
                                    editor.link_return.push_front(vo[0]->Id);
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
            }
            else
            {
                ImGui::Text("%s", boo::Localization::getLocalized("no_object").c_str());
            }
            ImGui::End();
        }
        return edited;
    }

    void boo::ui::UIContainer::showObjectView()
    {
        std::vector<boo::Object*> vo;
        if (objectViewOpen && editors.size() > 0 && ImGui::Begin(boo::Localization::getLocalized("object_view").c_str()))
        {
            ImGui::PushID(editors[editorSelected].stage.name.c_str());
            boo::Editor& editor = editors[editorSelected];
            boo::StageData& sd = editors[editorSelected].stage.data;

            ImGui::InputText(boo::Localization::getLocalized("filter").c_str(), &editor.filter);

            for (auto ol = sd.entries[editors[editorSelected].currentScenario].object_lists.begin(); ol != sd.entries[editors[editorSelected].currentScenario].object_lists.end(); ++ol)
            {
                bool expanded = ImGui::TreeNode(ol->first.c_str());
                for (boo::Object& object : ol->second.objects)
                {
                    auto show = [&editor, &expanded, &vo](boo::Object& object)
                    {
                        auto i = std::find(editor.cursel.begin(), editor.cursel.end(), object.Id);
                        if (!object.IsLinkDest) {
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
                                            editor.link_return.clear();
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
                                            editor.link_return.clear();
                                        }
                                    };
                                    if (editor.filter.empty())
                                        f();
                                    else if (object.UnitConfigName.find(editor.filter) != std::string::npos)
                                        f();
                                    ImGui::PopID();
                                }
                            }
                        }
                        if (i != editor.cursel.end())
                        {
                            if (object.IsLinkDest) vo.clear();
                            vo.push_back(&object);
                        }
                    };

                    std::function<void(boo::Object&)> hobject;
                    hobject = [&hobject, &show](boo::Object& o)
                    {
                        for (auto link = o.Links.begin(); link != o.Links.end(); ++link)
                        {
                            for (boo::Object& link_object : link->second)
                            {
                                hobject(link_object);
                            }
                        }
                        show(o);
                    };
                    hobject(object);

                }
                if (expanded) ImGui::TreePop();
            }
            ImGui::End();
            ImGui::PopID();
            if (showObject(vo)) editor.changed = true;
        }
    }

    void boo::ui::UIContainer::showPreferences()
    {
        if (preferencesOpen && ImGui::Begin(boo::Localization::getLocalized("s_preferences").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            boo::Config& config = boo::Config::Get();

            static char stagedata_path[255]; static bool stagedata_exists = false; static bool sds = false;
            static char objectdata_path[255]; static bool objectdata_exists = false; static bool ods = false;
            static bool restart_changes = false;
            if (!sds)
            {
                std::copy(config.stageDataPath.begin(), config.stageDataPath.end(), stagedata_path);
                stagedata_exists = std::filesystem::exists(stagedata_path);
                sds = true;
            }
            if (!ods)
            {
                std::copy(config.objectDataPath.begin(), config.objectDataPath.end(), objectdata_path);
                objectdata_exists = std::filesystem::exists(objectdata_path);
                ods = true;
            }

            static bool changed = false;

            if (ImGui::InputText(boo::Localization::getLocalized("stage_data_folder").c_str(), stagedata_path, IM_ARRAYSIZE(stagedata_path)))
            {
                stagedata_exists = std::filesystem::exists(stagedata_path);
                changed = true;
                restart_changes = true;
            }
            ImGui::PushID("stagebrowse");
            ImGui::SameLine();
            if (ImGui::Button(boo::Localization::getLocalized("browse").c_str()))
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
                ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::getLocalized("not_exist").c_str());
            
            if (ImGui::InputText(boo::Localization::getLocalized("object_data_folder").c_str(), objectdata_path, IM_ARRAYSIZE(objectdata_path)))
            {
                objectdata_exists = std::filesystem::exists(objectdata_path);
                changed = true;
                restart_changes = true;
            }
            ImGui::PushID("objectbrowse");
            ImGui::SameLine();
            if (ImGui::Button(boo::Localization::getLocalized("browse").c_str()))
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
                ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::getLocalized("not_exist").c_str());*/
            
            static int lang_current = config.language;
            static const char* langs[] = {"English", "German"};
            if (ImGui::Combo(boo::Localization::getLocalized("language").c_str(), &lang_current, langs, IM_ARRAYSIZE(langs)))
            {
                config.language = lang_current;
                boo::Localization::setLanguage((u8) lang_current);
                SetWindowTitle(boo::Localization::getLocalized("title").c_str());
                changed = true;
            }

            static int fontsize = config.fontsize;
            
            if (ImGui::SliderInt(boo::Localization::getLocalized("fontsize").c_str(), &fontsize, 8, 56))
            {
                changed = true;
                restart_changes = true;
            }
            
            if (changed && stagedata_exists && ImGui::Button(boo::Localization::getLocalized("save").c_str()))
            {
                config.fontsize = fontsize;
                config.language = (u8) lang_current;
                config.stageDataPath = std::string(stagedata_path);
                config.objectDataPath = std::string(objectdata_path);
                config.Save("boo.ini");
                changed = false;
            }

            if (restart_changes) ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::getLocalized("restart_changes").c_str());
            ImGui::End();
        }
    }

    void boo::ui::UIContainer::showRandomizer()
    {
        if (randomizerOpen && ImGui::Begin(boo::Localization::getLocalized("s_randomizer").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            boo::Config& config = boo::Config::Get();

            static bool ip = false;
            static char stagedata_path[255]; 
            static char output_path[255];

            static bool shines = true;
            static bool scenarios = false;
            static bool bgm = false;

            if (ip)
            {
                static std::string progress;
                static bool init = false;

                if (!init)
                {
                    std::thread randomizer_thread(boo::Randomizer::randomizeLZ, false, scenarios, std::string(stagedata_path), std::string(output_path), &progress);
                    randomizer_thread.detach();

                    if (shines)
                    {
                        nfdchar_t *shineInfoPath;
                        nfdfilteritem_t filter[1] = {{"ShineInfo.szs", "szs"}};
                        nfdresult_t result = NFD::OpenDialog(shineInfoPath, filter, 1);
                        if (result == NFD_OKAY)
                        {
                            std::string sdp(shineInfoPath);
            
                            boo::Randomizer::randomizeShines(sdp, output_path);

                            NFD::FreePath(shineInfoPath);
                        }
                    }

                    if (bgm)
                    {
                        nfdchar_t *bgmDataBasePath;
                        nfdfilteritem_t filter[1] = {{"BgmDataBase.szs", "szs"}};
                        nfdresult_t result = NFD::OpenDialog(bgmDataBasePath, filter, 1);
                        if (result == NFD_OKAY)
                        {
                            std::string sdp(bgmDataBasePath);
            
                            boo::Randomizer::randomizeBGM(sdp, output_path);

                            NFD::FreePath(bgmDataBasePath);
                        }
                    }

                    init = true;
                }

                ImGui::Text("%s", progress.c_str());

                if (progress == "Done")
                {
                    init = false;
                    ip = false;
                    progress = std::string();
                    statusBar.info(boo::Localization::getLocalized("randomizer_done"));
                }
            }
            else
            {

                ImGui::TextWrapped("%s", boo::Localization::getLocalized("randomizer_notice").c_str());

                static bool stagedata_exists = false; static bool sds = false;
                static bool output_exists = false; static bool sdos = false;
                static bool restart_changes = false;
                if (!sds)
                {
                    std::copy(config.stageDataPath.begin(), config.stageDataPath.end(), stagedata_path);
                    stagedata_exists = std::filesystem::exists(stagedata_path);
                    sds = true;
                }
                if (!sdos)
                {
                    std::copy(config.objectDataPath.begin(), config.objectDataPath.end(), output_path);
                    output_exists = std::filesystem::exists(output_path);
                    sdos = true;
                }

                static bool changed = false;

                if (ImGui::InputText(boo::Localization::getLocalized("stage_data_folder").c_str(), stagedata_path, IM_ARRAYSIZE(stagedata_path)))
                {
                    stagedata_exists = std::filesystem::exists(stagedata_path);
                    changed = true;
                    restart_changes = true;
                }
                ImGui::PushID("stagebrowse");
                ImGui::SameLine();
                if (ImGui::Button(boo::Localization::getLocalized("browse").c_str()))
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
                    ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::getLocalized("not_exist").c_str());
            
                if (ImGui::InputText(boo::Localization::getLocalized("output_folder").c_str(), output_path, IM_ARRAYSIZE(output_path)))
                {
                    output_exists = std::filesystem::exists(output_path);
                    changed = true;
                    restart_changes = true;
                }
                ImGui::PushID("stageoutputbrowse");
                ImGui::SameLine();
                if (ImGui::Button(boo::Localization::getLocalized("browse").c_str()))
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
                    ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::getLocalized("not_exist").c_str());

                ImGui::Checkbox(boo::Localization::getLocalized("randomize_shines").c_str(), &shines);
                ImGui::Checkbox(boo::Localization::getLocalized("randomize_scenarios").c_str(), &scenarios);
                ImGui::Checkbox(boo::Localization::getLocalized("randomize_bgm").c_str(), &bgm);

                ImGui::PopID();
                if (changed && stagedata_exists && output_exists && ImGui::Button(boo::Localization::getLocalized("ok").c_str()))
                    ip = true;
            }
            ImGui::End();
        }
    }

    bool boo::ui::UIContainer::showStageDataFileSelectPopup()
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
        ImGui::OpenPopup(boo::Localization::getLocalized("select_stage_data").c_str());
        if (ImGui::BeginPopupModal(boo::Localization::getLocalized("select_stage_data").c_str()))
        {
            static char temp_path[255];
            static bool exists = false;
            ImGui::TextWrapped("%s", boo::Localization::getLocalized("stage_data_notice").c_str());
            ImGui::Spacing();
            if (ImGui::Button(boo::Localization::getLocalized("browse").c_str()))
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
            if (ImGui::InputText(boo::Localization::getLocalized("stage_data_folder").c_str(), temp_path, IM_ARRAYSIZE(temp_path)))
                exists = std::filesystem::exists(temp_path);
            if (!exists)
                ImGui::TextColored(ImVec4{255, 0, 0, 255}, "%s", boo::Localization::getLocalized("not_exist").c_str());
            else if (ImGui::Button(boo::Localization::getLocalized("ok").c_str()))
            {
                boo::Config::Get().stageDataPath = temp_path;
                boo::Config::Get().Save("boo.ini");
                return true;
            }
            ImGui::EndPopup();
        }
        return false;
    }

    void boo::ui::UIContainer::showOPDBGeneratePopup()
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
        ImGui::OpenPopup(boo::Localization::getLocalized("generate_opdb").c_str());
        if (ImGui::BeginPopupModal(boo::Localization::getLocalized("generate_opdb").c_str()))
        {
            if (!running)
            {
                ImGui::TextWrapped("%s", boo::Localization::getLocalized("generate_notice").c_str());
                ImGui::Spacing();
                if (ImGui::Button(boo::Localization::getLocalized("generate").c_str()))
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
                    for (const auto& sf : std::filesystem::directory_iterator(boo::Config::Get().stageDataPath))
                    {
                        if (sf.is_directory()) continue;
                        opdb.generateFromFile(sf.path(), done_file_name);
                    }
                    opdb.loaded = true;
                    opdb.save("db.opdb");
                };
                static bool thread_started = false;
                if (!thread_started)
                {
                    static std::thread generator(thread);
                    generator.detach();
                    thread_started = true;
                }
                ImGui::Text("%s", boo::Localization::getLocalized("generating").c_str());
                ImGui::Text("%s", boo::Localization::getLocalized("generating_info").c_str());
                ImGui::Spacing();
                ImGui::Text("%s", done_file_name.c_str());
            }
            ImGui::EndPopup();
        }
    }

    void boo::ui::UIContainer::tryExit()
    {
        exit = true;
    }

    void boo::ui::UIContainer::updateCurrentEditor()
    {
        if (editors.size() > 0)
        {
            editors[editorSelected].update();
        }
    }
}