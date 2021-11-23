#pragma once

#include <boo/stage.h>
#include <filesystem>
#include <string>

#include <raylib.h>

namespace boo
{

class Editor
{
private:
    Camera camera;
public:
    void loadStage(std::string path);
    void saveStage(std::string path);

    void init();
    void update();
    void draw();
    
    boo::Stage stage;

    std::vector<std::string> cursel;
    std::deque<std::string> link_return;

    int currentScenario = 0;
    bool changed = false;
    std::string filter;

    std::filesystem::path lastSaved;
};

}