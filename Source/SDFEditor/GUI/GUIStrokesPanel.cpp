

#include "GUIStrokesPanel.h"

#include "SDFEditor/Tool/SceneData.h"

#include <imgui/imgui.h>

struct TGUIState
{
    std::vector<uint32_t> mSelectedItems;
    int32_t mVisibleItemsInList{ 10 };
};

TGUIState gGUIState;

void DrawStrokesPanel(class CScene& aScene)
{
    bool lDirty = false;
    if (ImGui::Begin("Strokes Panel"))
    {
        /*{
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("ListChild", ImVec2(ImGui::GetContentRegionAvail().x, 260), false, 0);
            for (int32_t i = 0; i < aScene.mStorkesArray.size(); i++)
            {
                ImGui::Text(aScene.mStorkesArray[i].mName);
            }
            ImGui::EndChild();
        }*/


        {
            // Visible List Items
            ImGui::PushItemWidth(40);
            ImGui::DragInt("Visible List Items", &gGUIState.mVisibleItemsInList, 1, 5, 20);
            ImGui::PopItemWidth();
            gGUIState.mVisibleItemsInList = glm::clamp(gGUIState.mVisibleItemsInList, 5, 20);
        }

        if (ImGui::BeginListBox("StrokesListBox", ImVec2(-FLT_MIN, (float(gGUIState.mVisibleItemsInList) + 0.5f) * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int32_t i = 0; i < aScene.mStorkesArray.size(); i++)
            {
                auto lItemIt = std::find(gGUIState.mSelectedItems.begin(), gGUIState.mSelectedItems.end(), i);
                const bool lIsSelected = lItemIt != gGUIState.mSelectedItems.end();
                if (ImGui::Selectable(aScene.mStorkesArray[i].mName, lIsSelected))
                {
                    if (!ImGui::GetIO().KeyCtrl)
                    {
                        gGUIState.mSelectedItems.clear();
                        gGUIState.mSelectedItems.push_back(i);
                    }
                    else
                    {
                        if (lIsSelected)
                        {
                            gGUIState.mSelectedItems.erase(lItemIt);
                        }
                        else
                        {
                            gGUIState.mSelectedItems.push_back(i);
                        }
                    }
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                //if (is_selected)
                    //ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
        
        if (ImGui::Button("Add New"))
        {
            aScene.AddNewStorke();
            lDirty = true;
        }

        if (gGUIState.mSelectedItems.size() == 1)
        {
            int32_t lSelectedIndex = gGUIState.mSelectedItems[0];
            ImGui::PushID(lSelectedIndex);
            ImGui::InputText("Name", aScene.mStorkesArray[lSelectedIndex].mName, TStrokeInfo::MAX_NAME_SIZE);
            lDirty |= ImGui::DragInt4("shapeId", (int32_t*)&aScene.mStorkesArray[lSelectedIndex].id.x, 0.01f);
            lDirty |= ImGui::DragFloat4("param0", (float*)&aScene.mStorkesArray[lSelectedIndex].param0.x, 0.01f);
            lDirty |= ImGui::DragFloat4("param1", (float*)&aScene.mStorkesArray[lSelectedIndex].param1.x, 0.01f);
            if (ImGui::Button("Remove") && aScene.mStorkesArray.size() > 0)
            {
                aScene.mStorkesArray.erase(aScene.mStorkesArray.begin() + lSelectedIndex);
                lDirty = true;
            }
            ImGui::Separator();
            ImGui::PopID();
        }

        /*for (int32_t i = 0; i < aScene.mStorkesArray.size(); i++)
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
        } */

        
    }
    ImGui::End();
    if (lDirty)
    {
        aScene.SetDirty();
    }
}