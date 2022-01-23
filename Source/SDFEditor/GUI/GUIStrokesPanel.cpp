

#include "GUIStrokesPanel.h"

#include "SDFEditor/Tool/SceneData.h"

#include <imgui/imgui.h>

void DrawStrokesPanel(class CScene& aScene)
{
    bool lDirty = false;
    if (ImGui::Begin("Strokes Panel"))
    {
        for (int32_t i = 0; i < aScene.mStorkesArray.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::Text("Stroke[%d]", i);
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
            stroke_t lDefaultShape = 
                aScene.mStorkesArray.size() ? 
                aScene.mStorkesArray.back() : 
                stroke_t
                {
                    {0.4f, 0.0f, -1.0f, 0.4f},
                    {0.35f, 0.35f, 0.35f, 0.0f},
                    {1, 0, 0, 0}
                };

            aScene.mStorkesArray.push_back(lDefaultShape);
            lDirty = true;
        }
    }
    ImGui::End();
    if (lDirty)
    {
        aScene.SetDirty();
    }
}