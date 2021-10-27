#include <boo/editor.h>

#include <fstream>

void boo::Editor::LoadStage(std::string path)
{
    std::ifstream opf(path, std::ios::binary);
    std::istreambuf_iterator<char> start(opf), end;
    std::vector<u8> sarc(start, end);
    opf.close();

    stage.Load(sarc);
}

void boo::Editor::SaveStage(std::string path)
{
    std::vector<u8> file = stage.Save();
    std::ofstream stagefile2(path, std::ios::out | std::ios::binary);
    stagefile2.write(reinterpret_cast<const char*>(&file[0]), file.size()*sizeof(u8));
    stagefile2.close();
    Changed = false;
}