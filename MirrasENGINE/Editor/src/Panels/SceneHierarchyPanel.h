#pragma once

#include "Scene/Entity.h"

namespace mirras
{
    class Scene;

    class SceneHierarchyPanel
    {
    public:
        void drawPanel(Scene* scene);
        void setSelectedEntity(Entity entity) { selectedEntity = entity; }
        Entity getSelectedEntity() { return selectedEntity; }

    private:
        void drawNode(Entity entity, int32 treeNodeFlags);

    private:
        Entity selectedEntity;
    };
} // namespace mirras
