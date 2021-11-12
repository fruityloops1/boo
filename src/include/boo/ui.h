#pragma once

#include <boo/editor.h>
#include <boo/stage.h>

namespace boo::ui
{

class UIContainer
{

    class StatusBar
    {
    private:
        std::string message;
        bool isError;
        short framesUntilClose = 0;
    public:
        void update();
        void info(std::string message);
        void error(std::string message);
    };

private:
    bool DebugOpen = false;
    bool ObjectViewOpen = true;
    bool PreferencesOpen = false;
    bool RandomizerOpen = false;
    bool Exit = false;

    std::vector<boo::Editor> editors;
    int EditorSelected;

    StatusBar statusBar;

    bool ShowObject(std::vector<boo::Object*>& vo);

public:

    bool isEditorOpen = false;

    int GetEditorSelected();

    bool IsExit();

    void ShowDebug();
    void ShowMainMenuBar();
    void ShowObjectView();
    void ShowPreferences();
    void ShowRandomizer();
    bool ShowStageDataFileSelectPopup();
    void ShowStages();
    void ShowOPDBGeneratePopup();

    void StageFileOpen();
    void StageFileSave();
    void TryExit();
};

}