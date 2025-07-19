#pragma once

#include "Scene/Entity.h"

namespace mirras
{
    class Scene;

    class SceneHierarchyPanel
    {
    public:
        void draw();
        void setContext(Scene* scene) { context = scene; };
        void setSelectedEntity(Entity entity) { selectedEntity = entity; }
        Entity getSelectedEntity() { return selectedEntity; }

    private:
        void drawNode(Entity entity, int32 treeNodeFlags);

    private:
        Scene* context = nullptr;
        Entity selectedEntity;
        uint64 currentSceneID = 0;
    };
} // namespace mirras
