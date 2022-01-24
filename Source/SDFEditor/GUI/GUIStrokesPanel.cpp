

#include "GUIStrokesPanel.h"

#include "SDFEditor/Tool/SceneData.h"

#include <imgui/imgui.h>

struct TGUIState
{
    std::vector<uint32_t> mSelectedItems;
};

TGUIState gGUIState;

void DrawStrokesPanel(class CScene& aScene)
{
    bool lDirty = false;
    if (ImGui::Begin("Strokes Panel"))
    {
        for (int32_t i = 0; i < aScene.mStorkesArray.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::InputText("Name", aScene.mStorkesArray[i].mName, TStrokeInfo::MAX_NAME_SIZE);
            lDirty |= ImGui::DragInt4("shapeId", (int32_t*)&aScene.mStorkesArray[i].id.x, 0.01f);
            lDirty |= ImGui::DragFloat4("param0", (float*)&aScene.mStorkesArray[i].param0.x, 0.01f);
            lDirty |= ImGui::DragFloat4("param1", (float*)&aScene.mStorkesArray[i].param1.x, 0.01f);
            if (ImGui::Button("Remove") && aScene.mStorkesArray.size() > 0)
            {
                aScene.mStorkesArray.erase(aScene.mStorkesArray.begin() + i);
                lDirty = true;
                i++;
            }
            ImGui::Separator();
            ImGui::PopID();
        } 

        if (ImGui::Button("Add New"))
        {
            aScene.AddNewStorke();
            lDirty = true;
        }
    }
    ImGui::End();
    if (lDirty)
    {
        aScene.SetDirty();
    }
}