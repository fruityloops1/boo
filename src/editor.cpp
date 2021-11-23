#include <boo/editor.h>

#include <fstream>
#include <imgui.h>
#include <raylib.h>

void boo::Editor::loadStage(std::string path)
{
    std::ifstream opf(path, std::ios::binary);
    std::istreambuf_iterator<char> start(opf), end;
    std::vector<u8> sarc(start, end);
    opf.close();

    stage.load(sarc);
}

void boo::Editor::saveStage(std::string path)
{
    std::vector<u8> file;
    if (path.ends_with(".sarc"))
        file = stage.save(false);
    else file = stage.save(true);
    std::ofstream stagefile2(path, std::ios::out | std::ios::binary);
    stagefile2.write(reinterpret_cast<const char*>(&file[0]), file.size()*sizeof(u8));
    stagefile2.close();
    changed = false;
}

void boo::Editor::update()
{
    if (IsKeyPressed(KEY_DELETE) && !ImGui::GetIO().WantCaptureKeyboard && !ImGui::GetIO().WantCaptureMouse)
    {
        auto selection = cursel.begin();
        while (selection != cursel.end())
        {
            if (stage.data.deleteObject(*selection, currentScenario))
                changed = true;
            cursel.erase(selection);
        }
    }
}