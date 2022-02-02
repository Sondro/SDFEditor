

#include "GUIStrokesEdit.h"

#include <SDFEditor/Tool/SceneData.h>
#include <SDFEditor/Math/Box.h>

#include <sbx/Core/Log.h>

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

#include "GUIIconGlyphs.h"

namespace GEditor
{
    struct TGUIState
    {
        std::vector<uint32_t> mSelectedItems;
        int32_t mVisibleItemsInList{ 10 };

        ImGuizmo::OPERATION mCurrentGizmoOperation{ ImGuizmo::TRANSLATE };
        ImGuizmo::MODE mCurrentGizmoMode{ ImGuizmo::MODE::LOCAL };
        bool mGridActive{ true };
        bool mBoundsActive{ true };

        bool ValidStrokeSelected(class CScene& aScene)
        {
            return (mSelectedItems.size() == 1) && (mSelectedItems[0] < aScene.mStorkesArray.size());
        }
    };

    TGUIState gGUIState;

    void ResetSelection()
    {
        gGUIState.mSelectedItems.clear();
    }

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

    template <typename T>
    bool GuizmoButtonValue(const char* aIconStr, T* aVariable, T aValue)
    {
        const static ImVec2 kGuizmoButtonSize = ImVec2(40.0f, 40.0f);
        bool lPressed = false;
        const bool lActive = *aVariable == aValue;
        if (lActive) { ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive)); }
        lPressed = ImGui::Button(aIconStr, kGuizmoButtonSize);
        if (lPressed) { *aVariable = aValue;  }
        if (lActive) { ImGui::PopStyleColor(1); }
        return lPressed;
    }

    bool GuizmoButtonToggle(const char* aIconStr, bool* aVariable)
    {
        const static ImVec2 kGuizmoButtonSize = ImVec2(40.0f, 40.0f);
        bool lPressed = false;
        const bool lActive = *aVariable;
        if (lActive) { ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive)); }
        lPressed = ImGui::Button(aIconStr, kGuizmoButtonSize);
        if (lPressed) { *aVariable = !*aVariable; }
        if (lActive) { ImGui::PopStyleColor(1); }
        return lPressed;
    }


    void DrawStrokesGuizmos(CScene& aScene)
    {
        glm::mat4 lProjection = glm::perspective(aScene.mCamera.mFOV, aScene.mCamera.mAspect, 0.1f, 100.0f);
        glm::mat4 lView = aScene.mCamera.GetViewMatrix();
        ImGuiIO& io = ImGui::GetIO();
        const ImVec2 kViewPos = ImGui::GetMainViewport()->Pos;
        const ImVec2 kViewSize = ImGui::GetMainViewport()->Size;
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetRect(kViewPos.x, kViewPos.y, kViewSize.x, kViewSize.y);
        

        // draw guizmos icons panel
        ImGuiWindowFlags lWindowFlags = 0;
        lWindowFlags |= ImGuiWindowFlags_NoTitleBar
                     |  ImGuiWindowFlags_NoResize
                     |  ImGuiWindowFlags_NoMove
                     |  ImGuiWindowFlags_NoScrollbar
                     |  ImGuiWindowFlags_NoScrollWithMouse
                     |  ImGuiWindowFlags_NoCollapse
                     |  ImGuiWindowFlags_NoSavedSettings
                     |  ImGuiWindowFlags_NoFocusOnAppearing
                     |  ImGuiWindowFlags_NoBringToFrontOnFocus
                     |  ImGuiWindowFlags_NoNavInputs
                     |  ImGuiWindowFlags_NoNavFocus
                     |  ImGuiWindowFlags_NoDocking;

        const ImVec2 kGuizmoPanelSize = ImVec2(470, 50);
        const ImVec2 kGuizmoPanelPos = ImVec2(kViewPos.x + kViewSize.x - kGuizmoPanelSize.x - 20, kViewPos.y + 20);
        ImGui::SetNextWindowSize(kGuizmoPanelSize, ImGuiCond_Always);
        ImGui::SetNextWindowPos(kGuizmoPanelPos, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x00000000);
        if (ImGui::Begin("Guizmos Control Panel", NULL, lWindowFlags))
        {
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(1);
            ImGui::SetWindowFontScale(1.25f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.10f, 0.12f, 0.70f));
            GuizmoButtonValue(" " ICON_MOUSE_POINTER, &gGUIState.mCurrentGizmoOperation, ImGuizmo::OPERATION(0));
            ImGui::SameLine(0.0f, 5.0f);
            GuizmoButtonValue(ICON_MOVE, &gGUIState.mCurrentGizmoOperation, ImGuizmo::TRANSLATE);
            ImGui::SameLine(0.0f, 5.0f);
            GuizmoButtonValue(ICON_ROTATE, &gGUIState.mCurrentGizmoOperation, ImGuizmo::ROTATE);
            ImGui::SameLine(0.0f, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
            GuizmoButtonValue(ICON_SCALE, &gGUIState.mCurrentGizmoOperation, ImGuizmo::SCALE);
            ImGui::PopStyleVar(1);
            ImGui::SameLine(0.0f, 25.0f);
            GuizmoButtonValue(ICON_WORLD, &gGUIState.mCurrentGizmoMode, ImGuizmo::WORLD);
            ImGui::SameLine(0.0f, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(9.0f, 8.0f));
            GuizmoButtonValue(ICON_BOX, &gGUIState.mCurrentGizmoMode, ImGuizmo::LOCAL);
            ImGui::PopStyleVar(1);
            ImGui::SameLine(0.0f, 25.0f);
            GuizmoButtonToggle(ICON_GRID, &gGUIState.mGridActive);
            ImGui::SameLine(0.0f, 25.0f);
            GuizmoButtonToggle(ICON_BOUNDS, &gGUIState.mBoundsActive);
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(3);

            ImGuizmo::OPERATION mCurrentGizmoOperation{ ImGuizmo::TRANSLATE };
            ImGuizmo::MODE mCurrentGizmoMode{ ImGuizmo::MODE::LOCAL };
            bool mGridActive{ true };
            bool mBoundsActive{ true };

            //font scale test helper
            //static float font_scale = 1.0f;
            //ImGui::DragFloat("scale", &font_scale, 0.001f, 0.5f, 2.0f);
            //font_scale = glm::clamp(font_scale, 0.5f, 2.0f);
            //ImGui::SetWindowFontScale(font_scale);
        }
        ImGui::End();
        
        if (gGUIState.mGridActive)
        {
            ImGuizmo::DrawGrid(glm::value_ptr(lView), glm::value_ptr(lProjection), glm::value_ptr(glm::mat4(1.0f)), 20.f);
        }

        // draw gizmos over selected stroke
        if (gGUIState.ValidStrokeSelected(aScene))
        {
            int32_t lSelectedIndex = gGUIState.mSelectedItems[0];
            TStrokeInfo& lStrokeInfo = aScene.mStorkesArray[lSelectedIndex];

            if (io.KeysDown['T'])
                gGUIState.mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            if (io.KeysDown['R'])
                gGUIState.mCurrentGizmoOperation = ImGuizmo::ROTATE;
            if (io.KeysDown['Y']) // r Key
                gGUIState.mCurrentGizmoOperation = ImGuizmo::SCALE;

            glm::mat4 lTransformationMatrix;
            ImGuizmo::RecomposeMatrixFromComponents(&lStrokeInfo.posb.x, &lStrokeInfo.mEulerAngles.x, &lStrokeInfo.param0.x, glm::value_ptr(lTransformationMatrix));

            static float bounds[] = { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
            if (ImGuizmo::Manipulate(glm::value_ptr(lView), 
                                     glm::value_ptr(lProjection), 
                                     gGUIState.mCurrentGizmoOperation, 
                                     gGUIState.mCurrentGizmoMode, 
                                     glm::value_ptr(lTransformationMatrix), 
                                     NULL, NULL, gGUIState.mBoundsActive ? bounds : NULL, NULL))
            {
                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(lTransformationMatrix), &lStrokeInfo.posb.x, &lStrokeInfo.mEulerAngles.x, &lStrokeInfo.param0.x);
                lStrokeInfo.UpdateRotation();
                aScene.SetDirty();
            }
        }
    }

    void CreateCameraRay(CScene const & aScene, glm::vec3& aRayOrigin, glm::vec3& aRayDirection)
    {
        const ImVec2 kViewPos = ImGui::GetMainViewport()->Pos;
        const ImVec2 kViewSize = ImGui::GetMainViewport()->Size;

        ImGuiIO& io = ImGui::GetIO();

        const glm::mat4 proj = aScene.mCamera.GetProjectionMatrix();
        const glm::mat4 view = aScene.mCamera.GetViewMatrix();

        const glm::mat4 invVP = glm::inverse(proj * view);

        const float mox = ((io.MousePos.x - kViewPos.x) / kViewSize.x) * 2.f - 1.f;
        const float moy = (1.f - ((io.MousePos.y - kViewPos.y) / kViewSize.y)) * 2.f - 1.f;

        const float zNear = 0.f;
        const float zFar = (1.f - FLT_EPSILON);

        glm::vec4 lOrigin = invVP * glm::vec4(mox, moy, zNear, 1.f);
        lOrigin *= 1.f / lOrigin.w;
        glm::vec4 lEnd = invVP * glm::vec4(mox, moy, zFar, 1.f);
        lEnd *= 1.f / lEnd.w;       

        aRayOrigin = glm::vec3(lOrigin.x, lOrigin.y, lOrigin.z);
        aRayDirection = glm::normalize(lEnd - lOrigin);
    }

    void RaycastSelectStroke(CScene& aScene)
    {
        gGUIState.mSelectedItems.clear();

        glm::vec3 lRayOrigin = glm::vec3(1.0);
        glm::vec3 lRayDirection = glm::vec3(1.0);
        float lPrevDistance = 1000000.0f;
        int32_t lIntersectedIndex = UINT32_MAX;

        // calculate ray based on mouse position
        CreateCameraRay(aScene, lRayOrigin, lRayDirection);

        static const glm::vec3 kUnitVec(1.0f, 1.0f, 1.0f);

        for (int32_t i = 0; i < aScene.mStorkesArray.size(); i++)
        {
            TStrokeInfo& lStrokeInfo = aScene.mStorkesArray[i];
           
            // Calculate box with stroke properties
            const glm::vec3 lScale = lStrokeInfo.GetScale();
            glm::mat4 lTransformationMatrix;
            ImGuizmo::RecomposeMatrixFromComponents(&lStrokeInfo.posb.x, &lStrokeInfo.mEulerAngles.x, &kUnitVec.x, glm::value_ptr(lTransformationMatrix));


            float lDistance = 1000000000.0f;
            bool lIntersects = SBox(lScale, lTransformationMatrix).CheckRayIntersection2(lRayOrigin, lRayDirection, &lDistance);
            if (lDistance < lPrevDistance)
            {
                lPrevDistance = lDistance;
                lIntersectedIndex = i;
            }
        }

        if (lIntersectedIndex != UINT32_MAX)
        {
            //SBX_LOG("Clicked Stroke %s", aScene.mStorkesArray[lIntersectedIndex].mName);
            gGUIState.mSelectedItems.push_back(lIntersectedIndex);
        }
        else
        {
            //SBX_LOG("Clicked Nothing");
        }
    }

}