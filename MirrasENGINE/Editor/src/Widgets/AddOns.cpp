#include "Widgets/AddOns.h"

#include "Core/Log.h"

#include <vector>

namespace mirras
{
    uint8 isItemSingleOrDoubleClicked(ImGuiMouseButton button)
    {
        // This workaround is meant to make single and double clicks on the same item behave
        // nicely (also avoids missing clicks when leaving the button area right after clicking)

        // Only using a vector for ease of initialization (we'd need a function in order to init a static array with a given value without writing it multiple times {(uint32)-1, ...}.
        // Also, if ImGuiMouseButton_COUNT changes in the future, we'd need to update the initializer list as well, and we don't want to keep track of such changes)
        static std::vector<ImGuiID> singleClickIDs(ImGuiMouseButton_COUNT, -1); // Uses -1 (max uint32) because 0 is a valid ID for ImGui::Text
        //static ImGuiID singleClickIDs[ImGuiMouseButton_COUNT] = {(uint32)-1}; 

        ImGuiID& singleClickID = singleClickIDs[button];
        const ImGuiID itemID = ImGui::GetItemID();
        bool isHovered = ImGui::IsItemHovered();

        if(isHovered)
        {
            if(singleClickID == itemID && ImGui::IsMouseDoubleClicked(button))
            {
                singleClickID = -1;
                return 2;
            }

            if(ImGui::IsMouseClicked(button))
                singleClickID = itemID;
        }
        else
        if(!isHovered && singleClickID == itemID && !ImGui::IsMouseDown(button))
        {
            singleClickID = -1;

            if(ImGui::IsMouseReleased(button))
                return 0;

            return 1;
        }

        if(singleClickID == itemID)
        {
            ImGuiIO& io = ImGui::GetIO();
            bool releasedWithDelay = ImGui::IsMouseReleasedWithDelay(button, io.MouseDoubleClickTime);

            if(releasedWithDelay && io.MouseClickedLastCount[button] == 1)
            {
                singleClickID = -1;
                return 1;
            }
        }

        return 0;
    }
} // namespace mirras

