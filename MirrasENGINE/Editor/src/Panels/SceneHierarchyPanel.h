#pragma once

#include "Scene/Entity.h"

namespace mirras
{
    class EditorScene;

    class SceneHierarchyPanel
    {
    public:
        void draw();
        void setContext(EditorScene* scene) { context = scene; };

    private:
        void drawNode(Entity entity, int32 treeNodeFlags, Entity& forDeletion);

    private:
        EditorScene* context = nullptr;
    };
} // namespace mirras
