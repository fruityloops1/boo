#include <boo/config.h>
#include <boo/localization.h>
#include <boo/objparamdb.h>
#include <boo/ui.h>

#include <filesystem>
#include <raylib.h>
#include <iostream>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_raylib.h>

#include <nfd.hpp>

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

#define FPS_CAP 90

#define DEFAULT_FONT "res/NotoSansJP-Regular.otf"

int resources()
{
	int done = 1;
	ImGuiIO& io = ImGui::GetIO();
	if (std::filesystem::exists(DEFAULT_FONT))
	{
    	io.Fonts->AddFontFromFileTTF(DEFAULT_FONT, boo::Config::Get().fontsize, NULL, io.Fonts->GetGlyphRangesJapanese());
		done--;
	}
	return done;
}

void run(bool StageDataSet)
{
	SetTraceLogLevel(25);
	InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, boo::Localization::GetLocalized("title").c_str());
	SetTargetFPS(FPS_CAP);

	boo::ui::UIContainer ui;

	SetExitKey(KEY_NULL);

	ImGui::CreateContext();
	ImGui_ImplOpenGL3_Init();
    ImGui_ImplRaylib_Init();

	resources();

	while (!WindowShouldClose())
	{
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplRaylib_NewFrame();
        ImGui::NewFrame();
        ImGui_ImplRaylib_ProcessEvent();

		BeginDrawing();

        ClearBackground(BLACK);

		if (!StageDataSet) StageDataSet = ui.ShowStageDataFileSelectPopup();
		else if (!boo::ObjectParameterDatabase::Get().loaded) {ui.ShowOPDBGeneratePopup();}
        
        ui.ShowMainMenuBar();
		ui.ShowDebug();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        EndDrawing();
	}
}

int main()
{
	std::cout << "Loading configuration\n";
	boo::Config& c = boo::Config::Get();
	if (c.Load("boo.ini"))
	{
		std::cout << "No configuration found. Creating...\n";
		c.Save("boo.ini");
	}

	boo::Localization::SetLanguage(c.language);

	boo::ObjectParameterDatabase& opdb = boo::ObjectParameterDatabase::Get();
	if (std::filesystem::exists("db.opdb")) opdb.Load("db.opdb");
	
	NFD_Init();
	run(!c.StageDataPath.empty());
	return 0;
}