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

	ImVec4* colors = ImGui::GetStyle().Colors;
  	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  	colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  	colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
  	colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
  	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
  	colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  	colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
  	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  	colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  	colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  	colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  	colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  	colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  	colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  	colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  	colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
  	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  	colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  	colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  	colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  	colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
  	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
  	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

  	ImGuiStyle& style = ImGui::GetStyle();
  	style.WindowPadding = ImVec2(8.00f, 8.00f);
  	style.FramePadding = ImVec2(5.00f, 2.00f);
  	style.CellPadding = ImVec2(6.00f, 6.00f);
  	style.ItemSpacing = ImVec2(6.00f, 6.00f);
  	style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
  	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
  	style.IndentSpacing = 25;
  	style.ScrollbarSize = 15;
  	style.GrabMinSize = 10;
  	style.WindowBorderSize = 1;
  	style.ChildBorderSize = 1;
  	style.PopupBorderSize = 1;
  	style.FrameBorderSize = 1;
  	style.TabBorderSize = 1;
  	style.WindowRounding = 3;
  	style.ChildRounding = 1;
  	style.FrameRounding = 1;
  	style.PopupRounding = 1;
  	style.ScrollbarRounding = 1;
  	style.GrabRounding = 1;
  	style.LogSliderDeadzone = 4;
  	style.TabRounding = 1;
	
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

	while (!WindowShouldClose() && !ui.IsExit())
	{

		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q)) ui.TryExit();

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
		ui.ShowPreferences();
        
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