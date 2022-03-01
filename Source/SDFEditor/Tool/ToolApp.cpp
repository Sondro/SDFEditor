
// Copyright (c) 2022 David Gallardo and SDFEditor Project

#include "ToolApp.h"

#include "imgui/imgui.h"
#include "GLFW/glfw3.h"
#include "sbx/Core/Log.h"

#include "SDFEditor/GUI/GUIStrokesEdit.h"
#include "SDFEditor/GUI/GUIDocument.h"
#include "SDFEditor/Utils/FileIO.h"

CToolApp::CToolApp()
{
    mScene.Reset(true);
}

CToolApp::~CToolApp()
{
}

void CToolApp::Init()
{
    GUI::ConfigureFileDialogsIcons();

    mScene.mDocument->SetDocStateChangeCallback([&](bool aPendingChanges) {
        UpdateTitleBar();
    });

    mRenderer.Init();
    mRenderer.ReloadShaders();

    UpdateTitleBar();
}

void CToolApp::Shutdown()
{
}

void CToolApp::Update()
{
    GUI::DrawFileDialogs(*this);

    bool lCameraMoving = false;
    if (!HandleShortcuts())
    {
        UpdateCamera(lCameraMoving);
    }

    GUI::DrawDocOptionsBar(*this);

    // TODO: Update scene with ui
    GUI::DrawStrokesPanel(mScene);
    GUI::DrawStrokesGuizmos(mScene);


    ImGuiIO& io = ImGui::GetIO();
    if (!lCameraMoving && !io.WantCaptureMouse && ImGui::IsMouseClicked(0))
    {
        GUI::RaycastSelectStroke(mScene);
    }
    
#ifdef DEBUG
    ImGui::Begin("Debug");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("Use Voxels", &mScene.mUseVoxels);
    ImGui::Checkbox("LUT Nearest Filter", &mScene.mLutNearestFilter);
    ImGui::Checkbox("Atlas Nearest Filter", &mScene.mAtlasNearestFilter);
    ImGui::DragInt("Preview Slice", &mScene.mPreviewSlice, 1, 0, 127);
    ImGui::End(); 
#endif

    mRenderer.UpdateSceneData(mScene);

    mScene.CleanDirtyFlag();
}

void CToolApp::Render()
{
    mRenderer.RenderFrame();
}

void CToolApp::UpdateCamera(bool& aCameraMoving)
{
    CCamera& lCamera = mScene.mCamera;
    
    int lViewportWidth, lViewportHeight;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &lViewportWidth, &lViewportHeight);
    lCamera.UpdateAspect(float(lViewportWidth), float(lViewportHeight));

    ImGuiIO& io = ImGui::GetIO();

    float lMultiplier = (io.KeyShift) ? 4.f : 1.0f;

    const char* lDirection = "unknown";

    aCameraMoving = false;
    if (!io.WantCaptureKeyboard && !io.KeyCtrl && !io.KeyAlt)
    {

        if (io.KeysDown['W'] || io.KeysDown['S'])
        {
            lDirection = io.KeysDown['S'] ? "backwards" : "forward";
            float lLambda = (io.KeysDown['S'] ? -1.f : 1.f) * io.DeltaTime * 3.0f * lMultiplier;
            lCamera.MoveFront(lLambda);
            aCameraMoving = true;
        }

        if (io.KeysDown['A'] || io.KeysDown['D'])
        {
            lDirection = io.KeysDown['A'] ? "left" : "rigth";
            float lLambda = (io.KeysDown['A'] ? -1.f : 1.f) * io.DeltaTime * 3.0f * lMultiplier;
            lCamera.MoveRight(lLambda * 1.2f);
            aCameraMoving = true;
        }

        /* if(ImGui::IsKeyDown(io.KeyMap[ImGuiKey_Space]))
         {
             float lModifier = ImGui::GetIO().KeyShift ? -5.f : 5.0f;
             lCamera.MoveUp(io.DeltaTime * lModifier * lMultiplier);
         }*/

        if (io.KeysDown['Q'] || io.KeysDown['E'])
        {
            lDirection = io.KeysDown['E'] ? "downwards" : "upwards";
            float lModifier = io.KeysDown['E'] ? -5.f : 5.0f;
            lCamera.MoveUp(io.DeltaTime * lModifier * lMultiplier);
            aCameraMoving = true;
        }
    }

    // TODO: rotation control should be improved, this is very inconstitent with monitor edges and hovering everything
    // TODO proposal: use glfw mouse callback, use ImGui::CaptureMouseFromApp and avoid using imgui mouse state
    // TODO: keep the mouse on the same position but get a delta

    static double lPrevCursorX, lPrevCursorY;
    static bool lWasMoving = false;

    if (!io.WantCaptureMouse && io.MouseDown[1])
    {
        float lModifier = 0.0015f; // (io.KeyShift) ? 0.005f : 0.001f;
        lCamera.Pan(-io.MouseDelta.x * lModifier, -io.MouseDelta.y * lModifier);
        //glfwSetCursorPos(glfwGetCurrentContext(), lViewportWidth * 0.5f, lViewportHeight * 0.5f);
        glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (!lWasMoving)
        {
            glfwGetCursorPos(glfwGetCurrentContext(), &lPrevCursorX, &lPrevCursorY);
            lWasMoving = true;
        }
        
        aCameraMoving = true;
        
    }
    else if(lWasMoving)
    {
        lWasMoving = false;
        glfwSetCursorPos(glfwGetCurrentContext(), lPrevCursorX, lPrevCursorY);
        glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

bool CToolApp::HandleShortcuts()
{
    ImGuiIO& io = ImGui::GetIO();

    // F5 Reload Shaders
    if (ImGui::IsKeyPressed(294, false))
    {
        mRenderer.ReloadShaders();
        mScene.SetDirty();
        return true;
    }

    // Ctrl + S Save Scene
    if (io.KeyCtrl && !io.KeyShift && ImGui::IsKeyPressed('S', false))
    {
        GUI::RequestSaveFile(*this);
        return true;
    }

    if (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed('S', false))
    {
        //Save As, force the save dialog
        GUI::LaunchSaveFileDialog(*this);
        return true;
    }

    if (io.KeyCtrl && ImGui::IsKeyPressed('O', false))
    {
        //SaveScene("test.dfs");
        GUI::RequestOpenFile(*this);
        return true;
    }

    if (io.KeyCtrl && ImGui::IsKeyPressed('N', false))
    {
        //SaveScene("test.dfs");
        GUI::RequestNewFile(*this);
        return true;
    }

    // Ctrl + Z Undo (no Shift)
    if (io.KeyCtrl && !io.KeyShift && ImGui::IsKeyPressed('Z', false))
    {
        if (mScene.mStack->PopState())
        {
            mScene.SetDirty();
        }
        return true;
    }

    // Ctrl + Shift + Z Redo
    if (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed('Z', false))
    {
        if (mScene.mStack->RestorePopedState())
        {
            mScene.SetDirty();
        }
        return true;
    }

    // Ctrl + C Copy
    if (io.KeyCtrl && ImGui::IsKeyPressed('C', false))
    {
        mScene.mClipboard->CopySelectedItems();
        return true;
    }

    // Ctrl + V Paste
    if (io.KeyCtrl && ImGui::IsKeyPressed('V', false))
    {
        if (mScene.mClipboard->AddCopiedItems())
        {
            mScene.mStack->PushState(EPushStateFlags::EPE_ALL);
            mScene.SetDirty();
        }
        return true;
    }

    return false;
}

void CToolApp::ResetScene()
{
    mScene.Reset(true);
}

void CToolApp::SaveScene()
{
    mScene.mDocument->Save();
}

void CToolApp::SaveScene(const std::string& aFilePath)
{
    mScene.mDocument->SetFilePath(aFilePath);
    mScene.mDocument->Save();
}

void CToolApp::LoadScene(const std::string& aFilePath)
{
    GUI::ResetSelection(mScene);
    mScene.mDocument->SetFilePath(aFilePath);
    mScene.mDocument->Load();
}

void CToolApp::WantClose()
{
    GUI::WantCloseDocument(*this);
}

void CToolApp::Terminate()
{
    mRunning = false;
}

void CToolApp::UpdateTitleBar()
{
    mTitle = "SDFEditor - ";

    if (mScene.mDocument->HasPendingChanges())
    {
        mTitle += "[*] ";
    }

    if (mScene.mDocument->HasFilePath())
    {
        mTitle += mScene.mDocument->GetFilePath();
    }
    else
    {
        mTitle += "New Scene";
    }

    glfwSetWindowTitle(glfwGetCurrentContext(), mTitle.c_str());
}
