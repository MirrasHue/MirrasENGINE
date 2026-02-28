#include "EditorLayer.h"

#include "Core/Renderer/Renderer.h"
#include "Scene/Serializer.h"
#include "Events/WindowEvents.h"
#include "Utilities/FileDialogs.h"

#include "Reflection.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/gizmo.h>

#include <glm/gtc/type_ptr.hpp>

namespace mirras
{
    void EditorLayer::load()
    {
        reflect::registerComponentsFunctions();

        playIcon = Texture::loadFrom("Resources/Icons/play.png");
        pauseIcon = Texture::loadFrom("Resources/Icons/pause.png");
        stopIcon = Texture::loadFrom("Resources/Icons/stop.png");
        simulateIcon = Texture::loadFrom("Resources/Icons/simulate.png");

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigDpiScaleFonts = true;
        io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-SemiBold.ttf");
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        auto scene1 = instantiate<Scene>("Scene1");

        auto cameraEntity = scene1->createEntity("Camera");
        auto& camera = cameraEntity.add<CameraComponent>();

        auto textEntity = scene1->createEntity("Text");
        auto& text = textEntity.add<TextComponent>();
        text.loadFontFrom("Resources/Fonts/Doto_Rounded-Black.ttf");
        text.text = U"Hello \n World!";
        text.fontSize = 48.f;

        auto rectEntity = scene1->createEntity("Square");
        auto& rect = rectEntity.add<RectangleComponent>();
        rect.size  = {200, 200};
        rect.color = {0, 0, 1, 1};

        auto rectEntity2 = scene1->createEntity("Square2");
        auto& transform = rectEntity2.get<TransformComponent>();
        transform.position = {400, 0, 0};
        auto& rect2 = rectEntity2.add<RectangleComponent>();
        rect2.size  = {200, 200};
        rect2.color = {0, 1, 0, 1};

        scenes.emplace_back(std::move(scene1));
        scenes.back().addedEntityCount = 4;
    }

    void EditorLayer::update(float dt)
    {
        std::erase_if(scenes, [this](const auto& scene)
        {
            bool remove = !scene.open;
            if(remove && activeScene == &scene)
                activeScene = nullptr;
            return remove;
        });

        bool ctrlDown = Input::isKeyDown(Key::LeftControl) || Input::isKeyDown(Key::RightControl);
        bool shiftDown = Input::isKeyDown(Key::LeftShift) || Input::isKeyDown(Key::RightShift);

        for(auto& editorScene : scenes)
        {
            editorScene.canvas.resize(editorScene.size.x, editorScene.size.y);

            switch(editorScene.state)
            {
                case SceneState::Editing:
                {
                    if(editorScene.focused && !(ctrlDown || shiftDown))
                    {
                        cameraController.update(dt);

                        // Gizmo controls
                        if(Input::isKeyDown(Key::F))
                            editorScene.gizmoType = GizmoType::None;
                        else
                        if(Input::isKeyDown(Key::R))
                            editorScene.gizmoType = GizmoType::Rotate;
                        else
                        if(Input::isKeyDown(Key::T))
                            editorScene.gizmoType = GizmoType::Translate;
                        else
                        if(Input::isKeyDown(Key::Y))
                            editorScene.gizmoType = GizmoType::Scale;
                        else
                        if(Input::isKeyDown(Key::U))
                            editorScene.gizmoType = GizmoType::Universal;
                    }

                    if(editorScene.hovered)
                        zoomController.updateZoom();

                    break;
                }

                case SceneState::Playing:
                {
                    editorScene.scene->update(dt);
                    break;
                }
            }
        }

        // NOTE: GLFW by default doesn't provide a function to check if a key went from Down -> Up (only through callbacks).
        // We could detect this at onEvent(), but in the long run it would be better to roll our own state keeping, in order
        // to access it from anywhere. For now we'll be using ImGui for such cases, so that we don't spam actions here

        // Shortcuts
        if(ctrlDown && ImGui::IsKeyReleased(ImGuiKey_N))
            newScene();
    }

    void EditorLayer::fixedUpdate(float ts)
    {
        if(activeScene && activeScene->state == SceneState::Playing)
            activeScene->scene->fixedUpdate(ts);
    }

    void EditorLayer::draw()
    {
        for(auto& editorScene : scenes)
        {
            Renderer::beginTextureDrawing(editorScene.canvas);
                Renderer::clearBackBuffers();

                editorScene.canvas.clear(Attachment::RedInteger, -1);

                switch(editorScene.state)
                {
                    case SceneState::Editing:
                        editorScene.scene->draw(editorScene.camera);
                        break;

                    case SceneState::Playing:
                        editorScene.scene->draw();
                        break;
                }

            Renderer::endTextureDrawing();
        }
    }

    static ImGuiWindow* activeSceneWindow = nullptr;

    void EditorLayer::drawImGui()
    {
        auto mainDockID = ImGui::DockSpaceOverViewport();

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                //if(ImGui::MenuItem("Open Project", "Ctrl + O"))

                if(ImGui::MenuItem("New Scene", "Ctrl + N"))
                    newScene();

                // Temp, just for testing
                if(ImGui::MenuItem("Save Scene", "Ctrl + S"))
                {
                    if(activeScene)
                        serialize(*activeScene->scene, "out.mirras");
                }

                if(ImGui::MenuItem("Open Scene", "Ctrl + S"))
                {
                    auto scene = instantiate<Scene>("other");
                    uint32 entityCount{};

                    deserialize(*scene, "out.mirras", entityCount);
                    scenes.emplace_back(std::move(scene));
                    scenes.back().addedEntityCount = entityCount;
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        //ImGui::SetNextWindowSizeConstraints({800, 450}, {FLT_MAX, FLT_MAX});
        ImGuizmo::BeginFrame();
        uint32 sceneCount = 0;
        bool playing = false;

        if(activeScene && activeScene->state == SceneState::Playing)
            playing = true;

        for(auto& editorScene : scenes)
        {
            if(!editorScene.open)
                continue;

            ImGui::SetNextWindowDockID(mainDockID, ImGuiCond_FirstUseEver);

            // ImGui::Begin doesn't take into consideration the ID stack to generate a unique ID for the viewport,
            // so PushID will not work here. That's why we're concatenating with "##" to differentiate equal names
            std::string name = editorScene.scene->name + "##" + std::to_string(sceneCount++);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{});
            ImGui::Begin(name.c_str(), &editorScene.open, ImGuiWindowFlags_NoCollapse);
            {
                auto [viewportX, viewportY] = ImGui::GetCursorScreenPos();
                auto [width, height] = ImGui::GetContentRegionAvail();
                editorScene.size = {width, height};

                if(activeScene == &editorScene)
                    activeSceneWindow = ImGui::GetCurrentWindow();

                if(ImGui::GetCurrentWindow()->Hidden)
                {
                    ImGui::End();
                    ImGui::PopStyleVar();
                    continue;
                }

                // Disable other windows when playing
                if(playing && activeScene != &editorScene)
                {
                    ImGui::InvisibleButton(name.c_str(), {width, height});
                    ImGui::BeginDisabled();
                    ImGui::SetCursorScreenPos({viewportX, viewportY});
                }

                editorScene.focused = false;

                if(!playing && ImGui::IsWindowFocused())
                {
                    cameraController.setCamera(&editorScene.camera);
                    editorScene.focused = true;
                    activeScene = &editorScene;
                }

                editorScene.hovered = false;
                auto [mouseX, mouseY] = ImGui::GetMousePos();

                if(!playing && ImGui::IsWindowHovered())
                {
                    zoomController.setCamera(&editorScene.camera);
                    editorScene.hovered = true;

                    // Convert the mouse position to be relative to the viewport bounds
                    mouseX -= viewportX;
                    mouseY = height - (mouseY - viewportY); // Flip Y axis for texture pixel reading

                    static bool clicked = false;
                    bool isOverGizmo = false;

                    // Even when the gizmo is not visible, it's still active, messing up the selection if we are over it
                    if(editorScene.selectedEntity)
                        isOverGizmo = ImGuizmo::IsOver();

                    if(!isOverGizmo && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        clicked = true;

                    if(clicked && !ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::GetMouseDragDelta() != ImVec2{0.f, 0.f})
                        clicked = false;
                    else
                    // It's necessary to check (mouseY <= height) before reading a pixel because when the window
                    // is not docked, the titlebar is also included (IsHovered == true), and reading a pixel in
                    // that region (outside the texture) causes OpenGL to return 0 (a valid ID), instead of -1
                    if(clicked && mouseY <= height && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                    {                       
                        int32 pixelData = editorScene.canvas.readPixel(Attachment::RedInteger, mouseX, mouseY);

                        editorScene.selectedEntity = (pixelData != /*clear value*/ -1) ?
                            Entity{(entt::entity)pixelData, &editorScene.scene->registry} : Entity{};

                        clicked = false;
                    }
                }

                // So that we keep showing the hierarchy panel for this scene when its tab is selected but not focused
                if(!activeScene && ImGui::GetCurrentWindow()->DockTabIsVisible)
                    activeScene = &editorScene;

                ImGui::Image(editorScene.canvas.color, {width, height}, {0, 1}, {1, 0});

                bool hasTransform = editorScene.selectedEntity ? editorScene.selectedEntity.has<TransformComponent>() : false;

                if(hasTransform && editorScene.gizmoType != GizmoType::None && !playing)
                {
                    ImGuizmo::PushID(&editorScene);
                    ImGuizmo::SetOrthographic(true);
                    ImGuizmo::SetDrawlist();
                    ImGuizmo::SetRect(viewportX, viewportY, width, height);

                    vec2i fbSize{editorScene.canvas.width, editorScene.canvas.height};

                    glm::mat4 view = editorScene.camera.getViewMatrix(fbSize, editorScene.canvas.initialSize);
                    glm::mat4 projection = glm::ortho(0.f, (float)fbSize.x, (float)fbSize.y, 0.f, 0.f, 1.f);

                    auto& transfComp = editorScene.selectedEntity.get<TransformComponent>();
                    glm::mat4 transform = transfComp.getTransformMatrix();

                    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                        (ImGuizmo::OPERATION)editorScene.gizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform));

                    if(ImGuizmo::IsUsing())
                        transfComp.decomposeTransform(transform);

                    ImGuizmo::PopID();
                }

                static bool rightClicked = false;

                if(editorScene.hovered && mouseY <= height && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                {
                    ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
                    rightClicked = true;
                }

                if(rightClicked && editorScene.hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.f))
                {
                    auto[dx, dy] = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right, 0.f);
                    editorScene.camera.position -= glm::vec2{dx, dy} * (1.f / editorScene.camera.zoom);
                }
                else
                if(rightClicked && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                    rightClicked = false;

                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);

                if(playing && activeScene != &editorScene)
                    ImGui::EndDisabled();
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }

        if(sceneCount == 1)
            activeScene = scenes.data();

        sceneHierarchy.setContext(activeScene);
        sceneHierarchy.draw();

        entityProperties.draw(activeScene ? activeScene->selectedEntity : Entity{});

        contentBrowser.draw();

        toolbar();

        ImGui::ShowDemoWindow();
    }

    static void drawDisabledImageButton(const char* str, uint32 textureID, ImVec2 buttonSize)
    {
        ImGui::BeginDisabled();
        ImGui::ImageButton(str, textureID, buttonSize);
        ImGui::EndDisabled();
    }

    void EditorLayer::toolbar()
    {
        static ImGuiID toolbarDockID = 0;
        static const ImGuiWindow* focusedWindow = nullptr;
        const int32 itemCount = 4;
        const ImVec2 windowPadding = {10.f, 2.f};

        ImGuiWindowClass windowClass;
        windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_NoResizeY |
            ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplit;
        ImGui::SetNextWindowClass(&windowClass);

        float frameHeight = ImGui::GetFrameHeight() - windowPadding.y * 2.f;
        float itemsWidth = itemCount * frameHeight + (itemCount - 1) * ImGui::GetStyle().ItemSpacing.x;
        ImVec2 buttonSize = {frameHeight, frameHeight};

        if(toolbarDockID != 0)
        {
            auto* node = ImGui::DockBuilderGetNode(toolbarDockID);

            float nodeWidth = node->Size.x;

            if(nodeWidth < 1.f) // node->Size.x is 0 in the 1st frame
                nodeWidth = itemsWidth;

            ImGui::DockBuilderSetNodeSize(toolbarDockID, ImVec2{nodeWidth, frameHeight + windowPadding.y * 2 - 1.f});
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg));
        ImGui::Begin("toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollWithMouse);
        {
            toolbarDockID = ImGui::IsWindowDocked() ? ImGui::GetWindowDockID() : 0;

            if(ImGui::GetCurrentContext()->NavWindow != ImGui::GetCurrentWindow())
                focusedWindow = ImGui::GetCurrentContext()->NavWindow;

            ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - itemsWidth) * 0.5f + windowPadding.x);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{});
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{});

            if(!activeScene)
            {
                ImGui::BeginDisabled();
                    ImGui::ImageButton("play", playIcon->id, buttonSize);
                    ImGui::SameLine();
                    ImGui::ImageButton("pause", pauseIcon->id, buttonSize);
                    ImGui::SameLine();
                    ImGui::ImageButton("stop", stopIcon->id, buttonSize);
                    ImGui::SameLine();
                    ImGui::ImageButton("simulate", simulateIcon->id, buttonSize);
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();
                ImGui::EndDisabled();

                ImGui::End();
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();

                return;
            }

            auto& state = activeScene->state;
            bool& paused = activeScene->scene->paused;

            bool focusOnScene = false;
            bool focusOnPreviousWindow = false;

            bool playButton  = state == SceneState::Editing || (state == SceneState::Playing && paused);
            bool pauseButton = state != SceneState::Editing && !paused;
            bool stopButton  = state != SceneState::Editing;
            bool simulateButton = state == SceneState::Editing || (state == SceneState::Simulating && paused);

            if(!playButton)
                drawDisabledImageButton("play", playIcon->id, buttonSize);
            else
            if(ImGui::ImageButton("play", playIcon->id, buttonSize))
            {
                if(!paused)
                    activeScene->scene->onRuntimeStart();
            
                state = SceneState::Playing;
                paused = false;
                focusOnScene = true;
            }

            ImGui::SameLine();

            if(!pauseButton)
                drawDisabledImageButton("pause", pauseIcon->id, buttonSize);
            else
            if(ImGui::ImageButton("pause", pauseIcon->id, buttonSize))
            {
                paused = true;
                focusOnPreviousWindow = true;
            }

            ImGui::SameLine();

            if(!stopButton)
                drawDisabledImageButton("stop", stopIcon->id, buttonSize);
            else
            if(ImGui::ImageButton("stop", stopIcon->id, buttonSize))
            {
                state = SceneState::Editing;
                activeScene->scene->onRuntimeStop();
                paused = false;
                focusOnPreviousWindow = true;
            }

            ImGui::SameLine();

            if(!simulateButton)
                drawDisabledImageButton("simulate", simulateIcon->id, buttonSize);
            else
            if(ImGui::ImageButton("simulate", simulateIcon->id, buttonSize))
            {
                state = SceneState::Simulating;
                activeScene->scene->onRuntimeStart();
                paused = false;
                focusOnScene = true;
            }

            if(focusOnScene)
                ImGui::SetWindowFocus(activeSceneWindow->Name);
            else
            if(focusOnPreviousWindow && focusedWindow)
                ImGui::SetWindowFocus(focusedWindow->Name);

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    void EditorLayer::onEvent(Event& event)
    {
        if(Event::is_a<WindowClosed>(event))
            App::getInstance().stop();
    }

    void EditorLayer::newScene()
    {
        if(!activeScene)
        {
            scenes.emplace_back(instantiate<Scene>("Untitled-" + std::to_string(scenes.size())));
            return;
        }

        size_t idx = activeScene - scenes.data();
        scenes.emplace_back(instantiate<Scene>("Untitled-" + std::to_string(scenes.size())));
        activeScene = &scenes[idx];
    }
} // namespace mirras
