#pragma once

#include <boo/stage.h>
#include <string>

#include <raylib.h>

namespace boo
{

class Editor
{
private:
    Camera camera;
public:

    void LoadStage(std::string path);
    void SaveStage(std::string path);

    void Init();
    void Update();
    void Draw();
    
    boo::Stage stage;

    std::vector<std::string> cursel;
    std::deque<std::string> link_return;

    int CurrentScenario = 0;
    bool Changed = false;
    std::string filter;
};

}