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
    bool PreferencesOpen = false;
    bool Exit = false;

public:
    bool IsExit();
    void ShowDebug();
    void ShowMainMenuBar();
    void ShowObjectView(boo::StageData& sd, int scenario);
    void ShowPreferences();
    bool ShowStageDataFileSelectPopup();
    void ShowOPDBGeneratePopup();
    void TryExit();
};

}

#endif