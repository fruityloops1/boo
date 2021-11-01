#pragma once

#include <boo/editor.h>
#include <boo/stage.h>

namespace boo::ui
{

class UIContainer
{

private:
    bool DebugOpen = false;
    bool ObjectViewOpen = true;
    bool PreferencesOpen = false;
    bool Exit = false;

    std::vector<boo::Editor> editors;
    int EditorSelected;

    bool ShowObject(std::vector<boo::Object*>& vo);

public:

    bool isEditorOpen = false;

    int GetEditorSelected();

    bool IsExit();

    void ShowDebug();
    void ShowMainMenuBar();
    void ShowObjectView();
    void ShowPreferences();
    bool ShowStageDataFileSelectPopup();
    void ShowStages();
    void ShowOPDBGeneratePopup();

    void StageFileOpen();
    void StageFileSave();
    void TryExit();
};

}