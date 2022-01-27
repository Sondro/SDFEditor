

#include "GUIStrokesEdit.h"

#include "SDFEditor/Tool/SceneData.h"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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


            lDirty |= ImGui::DragFloat("Blend", (float*)&lStrokeInfo.posb.w, 0.01f, 0.0f, 1.0f);
            lStrokeInfo.posb.w = glm::clamp(lStrokeInfo.posb.w, 0.0f, 1.0f);
            if (lStrokeInfo.id.x == EPrimitive::PrBox)
            {
                lDirty |= ImGui::DragFloat("Round", (float*)&lStrokeInfo.param0.w, 0.01f, 0.0f, 1.0f);
                lStrokeInfo.param0.w = glm::clamp(lStrokeInfo.param0.w, 0.0f, 1.0f);
            }
            lDirty |= ImGui::DragFloat4("param1", (float*)&lStrokeInfo.param1.x, 0.01f);

            lDirty |= ImGui::DragFloat3("Position", (float*)&lStrokeInfo.posb.x, 0.01f);

            if (ImGui::DragFloat3("Rotation", (float*)&lStrokeInfo.mEulerAngles.x, 0.01f))
            {
                lStrokeInfo.UpdateRotation();
                lDirty = true;
            }

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
    glm::mat4 lProjection = glm::perspective(aScene.mCamera.mFOV, aScene.mCamera.mAspect, 0.1f, 100.0f);
    glm::mat4 lView = aScene.mCamera.GetViewMatrix();
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetRect(ImGui::GetMainViewport()->Pos.x, ImGui::GetMainViewport()->Pos.y, ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y);
    ImGuizmo::DrawGrid(glm::value_ptr(lView), glm::value_ptr(lProjection), glm::value_ptr(glm::mat4(1.0f)), 20.f);
    
    if (gGUIState.ValidStrokeSelected(aScene))
    {
        int32_t lSelectedIndex = gGUIState.mSelectedItems[0];
        TStrokeInfo& lStrokeInfo = aScene.mStorkesArray[lSelectedIndex];

        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE | ImGuizmo::BOUNDS);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
        if (ImGui::IsKeyPressed(90))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(69))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(82)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        //if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        //    mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        //ImGui::SameLine();
        //if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        //    mCurrentGizmoOperation = ImGuizmo::ROTATE;
        //ImGui::SameLine();
        //if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        //    mCurrentGizmoOperation = ImGuizmo::SCALE;
        /*float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix.m16, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation, 3);
        ImGui::InputFloat3("Rt", matrixRotation, 3);
        ImGui::InputFloat3("Sc", matrixScale, 3);*/

        glm::mat4 lTransformationMatrix;
        ImGuizmo::RecomposeMatrixFromComponents(&lStrokeInfo.posb.x, &lStrokeInfo.mEulerAngles.x, &lStrokeInfo.param0.x, glm::value_ptr(lTransformationMatrix));

        /*if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }*/
        static bool useSnap(false);
        if (ImGui::IsKeyPressed(83))
            useSnap = !useSnap;
        //ImGui::Checkbox("", &useSnap);
        //ImGui::SameLine();
       // glm::vec3 snap;
       /* switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            snap = config.mSnapTranslation;
            ImGui::InputFloat3("Snap", &snap.x);
            break;
        case ImGuizmo::ROTATE:
            snap = config.mSnapRotation;
            ImGui::InputFloat("Angle Snap", &snap.x);
            break;
        case ImGuizmo::SCALE:
            snap = config.mSnapScale;
            ImGui::InputFloat("Scale Snap", &snap.x);
            break;
        }*/
        

        
        //ImGuizmo::DrawCubes(glm::value_ptr(lView), glm::value_ptr(lProjection), &lTransformationMatrix[0][0], 4);

        static float bounds[] = { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
        if (ImGuizmo::Manipulate(glm::value_ptr(lView), glm::value_ptr(lProjection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(lTransformationMatrix), NULL, NULL, bounds, NULL))
        {
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(lTransformationMatrix), &lStrokeInfo.posb.x, &lStrokeInfo.mEulerAngles.x, &lStrokeInfo.param0.x);
            lStrokeInfo.UpdateRotation();
            aScene.SetDirty();
        }
    }
}
