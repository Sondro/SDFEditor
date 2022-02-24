
/*
    MAIN TODO:

    - [DONE] Manage stroke list in cpu with more attributes (different layout) than in gpu

    - [DONE] Names for strokes (GUI)

    - [DONE] Separate strokes list and from properties panel (GUI)
        > [DONE] stroke selection in list
    
    - [DONE] Primitive selection menu (GUI)
	
	- [DONE]  Move up, Move Down in the list options (GUI)

    - [DONE] Subtract operation (GUI + Data + Shader)
        > [DONE] bitfield for operations
        > [DONE] operation checks in gui

    - [DONE] Implement ImGuizmo (GUI)

    - [DONE] Stroke selection with bounding box raycast
        > [PENDING] Draw bounding box (GUI)

    - [(basic test done)] Scene serializaion/deserialization + save/load (Core)
	
	- [DONE] Stroke Rotation (Data + Shader)
	
    - [DONE] ImGuizmo options panel (GUI)
        > Rotate, Translate, Scale toggle
        > World, Local
        > Bounds on/off
        > Grid on/off

    - Display properties layout per primitivie (GUI)

    - Adapt transform guizmos to primitive (GUI)

    - [DONE] Undo / Redo

    - [DONE] Copy / Paste strokes

    - [DONE] Delete key input to remove strokes

    - Scene export to json file

    - [DONE (basic)]Custom stroke list widget to replace ImGui::Selectable

    - Add extra strokes operations like mirror, pivot, etc.

    - Handle group selection and transform

    // Bugs

    - [LOW] Fix Camera Moving overalpping ImGuizmo (GUI + Camera)

    // Future
    
    - Offscreen render pipeline?
    - Half resolution? Checkerboard?
    - [WIP] Sparse voxel SDF
        > [DONE] AABB box check to discard traces out of the voxe box
        > [TODO, fixed size for now] adapt the size of the volume in the world depending on the farthest transformed bounding volume
        > [DONE] Fill atlas tiles with distance data
        > [EARLY VERSION, artifacts] Raymarch vs the atlas tiles



*/

#include "ToolApp.h"

#include "imgui/imgui.h"
#include "GLFW/glfw3.h"
#include "sbx/Core/Log.h"

#include "SDFEditor/GUI/GUIStrokesEdit.h"
#include "SDFEditor/GUI/GUIButtonBar.h"
#include "SDFEditor/Utils/FileIO.h"

CToolApp::CToolApp()
{
}

CToolApp::~CToolApp()
{
}

void CToolApp::Init()
{
    mRenderer.Init();
    mRenderer.ReloadShaders();
}

void CToolApp::Shutdown()
{
}

void CToolApp::Update()
{
    bool lCameraMoving = false;
    UpdateCamera(lCameraMoving);


    GEditor::DrawTopBar(*this);

    // TODO: Update scene with ui
    GEditor::DrawStrokesPanel(mScene);
    GEditor::DrawStrokesGuizmos(mScene);

    HandleShortcuts();

    ImGuiIO& io = ImGui::GetIO();
    if (!lCameraMoving && !io.WantCaptureMouse && ImGui::IsMouseClicked(0))
    {
        GEditor::RaycastSelectStroke(mScene);
    }
    
    ImGui::Begin("Debug");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("Use Voxels", &mScene.mUseVoxels);
    ImGui::Checkbox("LUT Nearest Filter", &mScene.mLutNearestFilter);
    ImGui::Checkbox("Atlas Nearest Filter", &mScene.mAtlasNearestFilter);
    ImGui::DragInt("Preview Slice", &mScene.mPreviewSlice, 1, 0, 127);
    ImGui::End(); 

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

    // F3 Load Scene
    if (ImGui::IsKeyPressed(292, false))
    {
        LoadScene("test.dfs");
        return true;
    }

    // Ctrl + S Save Scene
    if (io.KeyCtrl && ImGui::IsKeyPressed('S', false))
    {
        SaveScene("test.dfs");
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

void CToolApp::SaveScene(const std::string&  aFilePath)
{
    size_t lStrokesBytesSize = mScene.mStorkesArray.size() * sizeof(TStrokeInfo);
    uint32_t lNumElements = mScene.mStorkesArray.size() & 0xFFFFFFFF;

    std::vector<char> lData(lStrokesBytesSize + sizeof(lNumElements));

    ::memcpy(lData.data(), &lNumElements, sizeof(lNumElements));
    ::memcpy(lData.data() + sizeof(lNumElements), mScene.mStorkesArray.data(), lStrokesBytesSize);

    WriteFile(aFilePath, lData);
}

void CToolApp::LoadScene(const std::string& aFilePath)
{
    GEditor::ResetSelection(mScene);

    std::vector<char> lData = ReadFile(aFilePath);

    if (lData.size() > 0)
    {

        uint32_t lNumElements = 0;
        ::memcpy(&lNumElements, lData.data(), sizeof(lNumElements));

        mScene.mStorkesArray.clear();
        mScene.mSelectedItems.clear();
        mScene.mStack->Reset();
        mScene.mStorkesArray.resize(lNumElements);
        //TODO: json
        ::memcpy(mScene.mStorkesArray.data(), lData.data() + sizeof(lNumElements), lNumElements * sizeof(TStrokeInfo));
        mScene.SetDirty();
        mScene.mStack->PushState(EPushStateFlags::EPE_ALL);
    }

   
}
