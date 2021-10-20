#ifndef UI_H
#define UI_H

#include <boo/stage.h>

namespace boo::ui
{

class UIContainer
{

private:
    bool DebugOpen = false;
    bool ObjectViewOpen = false;

public:
    void ShowDebug();
    void ShowMainMenuBar();
    void ShowObjectView(boo::StageData& sd, int scenario);
    bool ShowStageDataFileSelectPopup();
    void ShowOPDBGeneratePopup();
};

}

#endif