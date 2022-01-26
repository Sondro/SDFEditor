

#include "GUIStrokesEdit.h"

#include "SDFEditor/Tool/SceneData.h"

#include <imgui/imgui.h>

#include <algorithm>

struct TGUIState
{
    std::vector<uint32_t> mSelectedItems;
    int32_t mVisibleItemsInList{ 10 };

    bool ValidStrokeSelected(class CScene& aScene)
    {
        return (mSelectedItems.size() == 1) && (mSelectedItems[0] < aScene.mStorkesArray.size());
    }
};

TGUIState gGUIState;

void DrawStrokesPanel(class CScene& aScene)
{
    bool lDirty = false;
    if (ImGui::Begin("Strokes Panel"))
    {
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
            }
            ImGui::EndListBox();
        }
        
        if (ImGui::Button("Add New"))
        {
            uint32_t lCloneIndex = (gGUIState.mSelectedItems.size() == 1) ? gGUIState.mSelectedItems[0] : UINT32_MAX;
            aScene.AddNewStorke(lCloneIndex);
            gGUIState.mSelectedItems.clear();
            gGUIState.mSelectedItems.push_back(int32_t(aScene.mStorkesArray.size() - 1));
            lDirty = true;
        }
        
        ImGui::BeginDisabled(gGUIState.mSelectedItems.size() == 0 || aScene.mStorkesArray.size() <= 1);
        ImGui::SameLine();
        if (ImGui::Button("Move Up"))
        {
            std::sort(gGUIState.mSelectedItems.begin(), gGUIState.mSelectedItems.end());
            for (int32_t i = 0; i < gGUIState.mSelectedItems.size(); i++)
            {
                int32_t lSelectedIndex = gGUIState.mSelectedItems[i];
                if (lSelectedIndex > 0)
                {
                    std::swap(aScene.mStorkesArray[lSelectedIndex], aScene.mStorkesArray[size_t(lSelectedIndex) - 1]);
                    gGUIState.mSelectedItems[i] = lSelectedIndex - 1;
                }
                else
                {
                    break;
                }
            }
            lDirty = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Move Down"))
        {
            std::sort(gGUIState.mSelectedItems.begin(), gGUIState.mSelectedItems.end());
            for (int32_t i = int32_t(gGUIState.mSelectedItems.size()) - 1; i >= 0; i--)
            {
                int32_t lSelectedIndex = gGUIState.mSelectedItems[i];
                if (lSelectedIndex < aScene.mStorkesArray.size() - 1)
                {
                    std::swap(aScene.mStorkesArray[lSelectedIndex], aScene.mStorkesArray[size_t(lSelectedIndex) + 1]);
                    gGUIState.mSelectedItems[i] = lSelectedIndex + 1;
                }
                else
                {
                    break;
                }
            }
            lDirty = true;
        }
        ImGui::EndDisabled();

        if (gGUIState.ValidStrokeSelected(aScene))
        {
            int32_t lSelectedIndex = gGUIState.mSelectedItems[0];
            TStrokeInfo& lStrokeInfo = aScene.mStorkesArray[lSelectedIndex];
            ImGui::PushID(lSelectedIndex);

            // NAME
            ImGui::InputText("Name", lStrokeInfo.mName, TStrokeInfo::MAX_NAME_SIZE);

            // PRIMITIVE
            static const char* lPrimitiveList = "Ellipsoid\0Box\0Torus\0Capsule\0";
            lDirty |= ImGui::Combo("Primitive", &lStrokeInfo.id.x, lPrimitiveList);

            // OPERATION
            static const char* lStrokeOpList = "Add\0Substract\0Intersect\0"; //Replace\0";
            int32_t lOpIndex = lStrokeInfo.id.y & EStrokeOp::OpsMaskAll;
            lDirty |= ImGui::Combo("Operation", &lOpIndex, lStrokeOpList);
            lStrokeInfo.id.y &= ~EStrokeOp::OpsMaskAll;
            lStrokeInfo.id.y |= lOpIndex;

            // PROPERTIES
            //lDirty |= ImGui::DragInt4("shapeId", (int32_t*)&aScene.mStorkesArray[lSelectedIndex].id.x, 0.01f);


            lDirty |= ImGui::DragFloat("Blend", (float*)&lStrokeInfo.posb.w, 0.01f);
            if (lStrokeInfo.id.x == EPrimitive::PrBox)
            {
                lDirty |= ImGui::DragFloat("Round", (float*)&lStrokeInfo.param0.w, 0.01f);
            }
            lDirty |= ImGui::DragFloat4("param1", (float*)&lStrokeInfo.param1.x, 0.01f);

            lDirty |= ImGui::DragFloat3("Position", (float*)&lStrokeInfo.posb.x, 0.01f);
            glm::vec3 lEulerAngles = lStrokeInfo.quat;
            lDirty |= ImGui::DragFloat3("Rotation", (float*)&lEulerAngles.x, 0.01f);
            lStrokeInfo.quat = glm::vec4(lEulerAngles, 1.0);

            lDirty |= ImGui::DragFloat3("Size", (float*)&lStrokeInfo.param0.x, 0.01f);

            if (ImGui::Button("Remove") && aScene.mStorkesArray.size() > 0)
            {
                aScene.mStorkesArray.erase(aScene.mStorkesArray.begin() + lSelectedIndex);
                gGUIState.mSelectedItems.clear();
                lDirty = true;
            }
            ImGui::Separator();
            ImGui::PopID();
        }        
    }
    ImGui::End();
    if (lDirty)
    {
        aScene.SetDirty();
    }
}

void DrawStrokesGuizmos(CScene& aScene)
{
    if (gGUIState.ValidStrokeSelected(aScene))
    {
        int32_t lSelectedIndex = gGUIState.mSelectedItems[0];
        TStrokeInfo& lStrokeInfo = aScene.mStorkesArray[lSelectedIndex];

        //aScene.mCamera.


    }
}
