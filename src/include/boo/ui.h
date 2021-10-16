#ifndef UI_H
#define UI_H

#include <boo/stage.h>

namespace boo::ui
{

class UIContainer
{

private:
    bool DebugOpen = false;

public:
    void ShowDebug();
    void ShowMainMenuBar();
};

}

#endif