#include "UI/ImGui.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Core/Application.h"

namespace mirras::imgui
{
    void init()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigDockingAnyDraggableArea = true;                    // Enable docking a window from any draggable area

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(App::getOSWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void beginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void endFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void onEvent(Event& event)
    {
        ImGuiIO& io = ImGui::GetIO();
        bool capturedByImGui = event.isInCategory(EventCategory::Mouse)    && io.WantCaptureMouse ||
                               event.isInCategory(EventCategory::Keyboard) && io.WantCaptureKeyboard;

		event.propagable = !capturedByImGui;
    }

    void shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}