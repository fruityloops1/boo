#ifndef EDITOR_H
#define EDITOR_H

#include <boo/stage.h>

#include <raylib.h>

namespace boo
{

class Editor
{
private:
    Camera camera;
public:

    void LoadStage(std::string path);

    void Init();
    void Update();
    void Draw();
    
    boo::Stage stage;

    u8 CurrentScenario = 0;
};

}

#endif