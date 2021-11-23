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
    bool debugOpen = false;
    bool objectViewOpen = true;
    bool preferencesOpen = false;
    bool randomizerOpen = false;
    bool exit = false;

    std::vector<boo::Editor> editors;
    int editorSelected;

    StatusBar statusBar;

    bool showObject(std::vector<boo::Object*>& vo);

public:

    bool isEditorOpen = false;

    int getEditorSelected();

    bool isExit();

    void showDebug();
    void showMainMenuBar();
    void showObjectView();
    void showPreferences();
    void showRandomizer();
    bool showStageDataFileSelectPopup();
    void showStages();
    void showOPDBGeneratePopup();

    void stageFileOpen();
    void stageFileSave();
    void tryExit();

    void updateCurrentEditor();
};

}