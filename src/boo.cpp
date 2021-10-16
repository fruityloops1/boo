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

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

#define FPS_CAP 60

#define DEFAULT_FONT "res/NotoSansJP-Regular.otf"

int resources()
{
	int done = 1;
	ImGuiIO& io = ImGui::GetIO();
	if (std::filesystem::exists(DEFAULT_FONT))
	{
    	io.Fonts->AddFontFromFileTTF(DEFAULT_FONT, 16, NULL, io.Fonts->GetGlyphRangesJapanese());
		done--;
	}
	return done;
}

void run(bool StageDataSet, bool ObjectDataSet)
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
	boo::Config c;
	if (c.Load("boo.ini"))
	{
		std::cout << "No configuration found. Creating...\n";
		c.Save("boo.ini");
	}

	boo::Localization::SetLanguage(c.language);

	boo::ObjectParameterDatabase& opdb = boo::ObjectParameterDatabase::Get();
	if (std::filesystem::exists("db.opdb")) opdb.Load("db.opdb");

	run(!c.StageDataPath.empty(), !c.ObjectDataPath.empty());
	return 0;
}

/*
int main()
{
    boo::ObjectParameterDatabase opdb = boo::ObjectParameterDatabase();
	opdb.Load("db.opdb");
    std::cout << "Loading Stage file" << std::endl;
    std::ifstream in("CityWorldHomeStage2Map.szs", std::ios::in | std::ios::binary);
    std::vector<u8> id((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    boo::Stage stage;
    std::cout << "Loading Stage" << std::endl;
    stage.Load(id);
    boo::ObjectParameterDatabase opdb;
    opdb.Generate("/home/noah/fds/moonlight/StageData"m );
    opdb.Save("db.opdb");

    SetTraceLogLevel(60);
    
    InitWindow(1280, 720, "Boo Editor");

	boo::ui::UIContainer ui;

    SetTargetFPS(60);

	int scenario = 0;

    ImGui::CreateContext();
    
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplRaylib_Init();

    resources();

    while (!WindowShouldClose())
    {

		auto t1 = std::chrono::high_resolution_clock::now();

		float mwm = GetMouseWheelMove();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplRaylib_NewFrame();
        ImGui::NewFrame();
        ImGui_ImplRaylib_ProcessEvent();

        BeginDrawing();

        ClearBackground(BLACK);
        
        ui.ShowMainMenuBar();
		ui.ShowDebug();
		ui.ShowObjectView(stage.data, scenario);
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        EndDrawing();
    }

    ImGui_ImplRaylib_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();

    CloseWindow();

    return 0;
}*/

/*void resources()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("NotoSansJP-Regular.otf", 16, NULL, io.Fonts->GetGlyphRangesJapanese());

	ImGui::StyleColorsLight();

    ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text]                   = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_Border]                 = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_CheckMark]              = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Button]                 = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	colors[ImGuiCol_Header]                 = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 2.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 4.0f;
}*/