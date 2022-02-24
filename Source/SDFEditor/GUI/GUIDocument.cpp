
#include "GUIDocument.h"

#include <imgui/imgui.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui/imgui_internal.h>
#include <imgui/ImGuiFileDialog.h>


#include "GUIIconGlyphs.h"
#include <SDFEditor/Tool/ToolApp.h>
#include "GUIIconGlyphs.h"

namespace GEditor
{
    void ConfigureFileDialgosIcons()
    {
        // define style for all directories
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".strks", ImVec4(0.8f, 1.0f, 0.3f, 0.9f), ICON_DOC_TEXT);
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(0.8f, 0.8f, 0.8f, 0.9f), ICON_FOLDER);
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1.0f, 1.0f, 1.0f, 0.3f), ICON_DOC);
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeLink, "", ImVec4(1.0f, 1.0f, 1.0f, 0.3f), ICON_DOC);
    }

    static bool TopBarButton(const char* aIconStr)
    {
        const static ImVec2 kGuizmoButtonSize = ImVec2(40.0f, 40.0f);
        return ImGui::Button(aIconStr, kGuizmoButtonSize);
    }

    void DrawDocOptionsBar(CToolApp& aToolApp)
    {
        const ImVec2 lViewPos = ImGui::GetMainViewport()->Pos;
        const ImVec2 lViewSize = ImGui::GetMainViewport()->Size;

        ImGuiWindowFlags lWindowFlags = 0;
        lWindowFlags |= ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoFocusOnAppearing
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoNavInputs
            | ImGuiWindowFlags_NoNavFocus
            | ImGuiWindowFlags_NoDocking;

        const ImVec2 kGuizmoPanelSize = ImVec2(470, 50);
        const ImVec2 kGuizmoPanelPos = ImVec2(lViewPos.x + 20, lViewPos.y + 20);
        ImGui::SetNextWindowSize(kGuizmoPanelSize, ImGuiCond_Always);
        ImGui::SetNextWindowPos(kGuizmoPanelPos, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x00000000);
        if (ImGui::Begin("Top Bar", NULL, lWindowFlags))
        {
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(1);
            ImGui::SetWindowFontScale(1.25f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.10f, 0.12f, 0.70f));
            if (TopBarButton(ICON_OPEN_FOLDER_INV))
            {
                uint32_t lFlags = ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_ReadOnlyFileNameField;
                ImGuiFileDialog::Instance()->OpenModal("OpenStrokesFile", "Open Strokes File", "Strokes{.strks}", ".", 1, nullptr, lFlags);
            }
            ImGui::SameLine(0.0f, 10.0f);
            if (TopBarButton(ICON_SAVE_INV))
            {
                uint32_t lFlags = ImGuiFileDialogFlags_ConfirmOverwrite;
                ImGuiFileDialog::Instance()->OpenModal("SaveStrokesFile", "Save Strokes File", "Strokes{.strks}", ".", 1, nullptr, lFlags);
            }
            ImGui::SameLine(0.0f, 10.0f);
            if (TopBarButton(ICON_DOC_INV))
            {
                // TODO ask dialog
                aToolApp.ResetScene();
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(3);

            //font scale test helper
            //static float font_scale = 1.0f;
            //ImGui::DragFloat("scale", &font_scale, 0.001f, 0.5f, 2.0f);
            //font_scale = glm::clamp(font_scale, 0.5f, 2.0f);
            //ImGui::SetWindowFontScale(font_scale);
        }
        ImGui::End();
    }

    void DrawFileDialogs(CToolApp& aToolApp)
    {
        // File Dialogs
        
        ImGuiWindowFlags lDialogsFlags = ImGuiWindowFlags_NoCollapse | 
                                         ImGuiWindowFlags_NoSavedSettings |
                                         ImGuiWindowFlags_NoDocking;
        if (ImGuiFileDialog::Instance()->Display("OpenStrokesFile", lDialogsFlags, ImVec2(800, 600)))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                // action

                aToolApp.LoadScene(filePathName);
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGuiFileDialog::Instance()->Display("SaveStrokesFile", lDialogsFlags, ImVec2(800, 600)))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                // action

                aToolApp.SaveScene(filePathName);
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
    }
}