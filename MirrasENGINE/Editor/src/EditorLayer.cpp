#include "EditorLayer.h"

#include "Core/Renderer/Renderer.h"

#include "Events/WindowEvents.h"

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

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigDpiScaleFonts = true;
        io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-SemiBold.ttf");
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        auto scene1 = instantiate<Scene>("Scene1");

        auto cameraEntity = scene1->createEntity("Camera");
        auto& camera = cameraEntity.add<CameraComponent>();

        auto textEntity = scene1->createEntity("Text");
        auto& text = textEntity.add<TextComponent>();
        text.font = instantiate<Font>("Assets/Fonts/Doto_Rounded-Black.ttf");
        text.text = L"Hello World!";
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
        std::erase_if(scenes, [](const auto& scene){ return !scene.open; });

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

    void EditorLayer::drawImGui()
    {
        auto mainDockID = ImGui::DockSpaceOverViewport();

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("New Scene", "Ctrl + N"))
                    newScene();
                

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        //ImGui::SetNextWindowSizeConstraints({800, 450}, {FLT_MAX, FLT_MAX});
        ImGuizmo::BeginFrame();
        activeScene = nullptr;

        for(auto& editorScene : scenes)
        {
            if(!editorScene.open)
                continue;

            ImGui::SetNextWindowDockID(mainDockID, ImGuiCond_FirstUseEver);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
            ImGui::Begin(editorScene.scene->name.c_str(), &editorScene.open, ImGuiWindowFlags_NoCollapse);
            {
                auto [viewportX, viewportY] = ImGui::GetCursorScreenPos();
                auto [width, height] = ImGui::GetContentRegionAvail();
                editorScene.size = {width, height};

                if(ImGui::GetCurrentWindow()->Hidden)
                {
                    ImGui::End();
                    ImGui::PopStyleVar();
                    continue;
                }

                editorScene.focused = false;

                if(ImGui::IsWindowFocused())
                {
                    cameraController.setCamera(&editorScene.camera);
                    editorScene.focused = true;
                    activeScene = &editorScene;
                }

                editorScene.hovered = false;

                if(ImGui::IsWindowHovered())
                {
                    zoomController.setCamera(&editorScene.camera);
                    editorScene.hovered = true;

                    auto [mouseX, mouseY] = ImGui::GetMousePos();

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

                if(hasTransform && editorScene.gizmoType != GizmoType::None)
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
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }

        sceneHierarchy.setContext(activeScene);
        sceneHierarchy.draw();

        entityProperties.draw(activeScene ? activeScene->selectedEntity : Entity{});

        ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoTitleBar);

        ImGui::End();

        ImGui::ShowDemoWindow();
    }

    void EditorLayer::onEvent(Event& event)
    {
        if(Event::is_a<WindowClosed>(event))
            App::getInstance().stop();
    }

    void EditorLayer::newScene()
    {
        scenes.emplace_back(instantiate<Scene>("Untitled-" + std::to_string(scenes.size())));
    }
} // namespace mirras
