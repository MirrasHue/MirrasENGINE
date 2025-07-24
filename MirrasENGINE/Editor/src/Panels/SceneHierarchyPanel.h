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
        /*void setSelectedEntity(Entity entity) { selectedEntity = entity; }
        Entity getSelectedEntity() { return selectedEntity; }*/

    private:
        void drawNode(Entity entity, int32 treeNodeFlags);

    private:
        EditorScene* context = nullptr;
        //Entity selectedEntity;
    };
} // namespace mirras
