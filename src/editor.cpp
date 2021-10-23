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