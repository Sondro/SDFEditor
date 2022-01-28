
/*
    MAIN TODO:

    - [DONE] Manage stroke list in cpu with more attributes (different layout) than in gpu

    - [DONE] Names for strokes (GUI)

    - [DONE] Separate strokes list and from properties panel (GUI)
        > [DONE] stroke selection in list
    
    - [DONE] Primitive selection menu (GUI)
	
	- [DONE]  Move up, Move Down in the list options (GUI)

    - [DONE] Substract operation (GUI + Data + Shader)
        > [DONE] bitfield for operations
        > [DONE] operation checks in gui


    - [DONE] Implement ImGuizmo (GUI)
    

    - Stroke selection with bounding box raycast
        > Draw bounding box (GUI)

    - [DONE (basic)] Scene serializaion/deserialization + save/load (Core)
	
	- [DONE, REVIEW] Stroke Rotation (Data + Shader)
	
    - ImGuizmo options panel (GUI)
        > Rotate, Translate, Scale toggle
        > World, Local
        > Bounds on/off
        > Grid on/off

    - Fix Camera Moving overalpping ImGuizmo (GUI + Camera)

    - Display properties layout per primitivie (GUI)

    - Adapt transform guizmos to primitive (GUI)

    - Copy / Paste strokes

    - Undo / Redo


    // Future
    
    - Offscreen render pipeline?
    - Half resolution? Checkerboard?
    - Sparse voxel SDF

*/

#include "ToolApp.h"

#include "imgui/imgui.h"
#include "GLFW/glfw3.h"
#include "sbx/Core/Log.h"

#include "SDFEditor/GUI/GUIStrokesEdit.h"
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
    UpdateCamera();

    // TODO: Update scene with ui
    DrawStrokesPanel(mScene);
    DrawStrokesGuizmos(mScene);

    mRenderer.UpdateSceneData(mScene);

    mScene.CleanDirtyFlag();
}

void CToolApp::Render()
{
    mRenderer.RenderFrame();
}

void CToolApp::UpdateCamera()
{
    CCamera& lCamera = mScene.mCamera;
    
    int lViewportWidth, lViewportHeight;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &lViewportWidth, &lViewportHeight);
    lCamera.UpdateAspect(float(lViewportWidth), float(lViewportHeight));

    ImGuiIO& io = ImGui::GetIO();

    float lMultiplier = (io.KeyShift) ? 4.f : 1.0f;

    const char* lDirection = "unknown";

    if (!io.WantCaptureKeyboard)
    {

        if (io.KeysDown['W'] || io.KeysDown['S'])
        {
            lDirection = io.KeysDown['S'] ? "backwards" : "forward";
            float lLambda = (io.KeysDown['S'] ? -1.f : 1.f) * io.DeltaTime * 3.0f * lMultiplier;
            lCamera.MoveFront(lLambda);
        }

        if (io.KeysDown['A'] || io.KeysDown['D'])
        {
            lDirection = io.KeysDown['A'] ? "left" : "rigth";
            float lLambda = (io.KeysDown['A'] ? -1.f : 1.f) * io.DeltaTime * 3.0f * lMultiplier;
            lCamera.MoveRight(lLambda * 1.2f);
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
        
        }

        // F5 Reload Shaders
        if (ImGui::IsKeyPressed(294, false))
        {
            mRenderer.ReloadShaders();
            mScene.SetDirty();
        }

        // F3 Load Scene
        if (ImGui::IsKeyPressed(292, false))
        {
            LoadScene("test.dfs");
        }

        // F4 Save Scene
        if (ImGui::IsKeyPressed(293, false))
        {
            SaveScene("test.dfs");
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
        /*else
        {
            double lCurrentCursorX, lCurrentCursorY;
            glfwGetCursorPos(glfwGetCurrentContext(), &lCurrentCursorX, &lCurrentCursorY);
            float lMouseDeltaX = float(lCurrentCursorX - lPrevCursorX);
            float lMouseDeltaY = float(lCurrentCursorY - lPrevCursorY);

            lCamera.Pan(-lMouseDeltaX * lModifier, -lMouseDeltaY * lModifier);

            glfwSetCursorPos(glfwGetCurrentContext(), lPrevCursorX, lPrevCursorY);
            lPrevCursorX = lCurrentCursorX;
            lPrevCursorY = lCurrentCursorY;
        }*/
    }
    else if(lWasMoving)
    {
        lWasMoving = false;
        glfwSetCursorPos(glfwGetCurrentContext(), lPrevCursorX, lPrevCursorY);
        glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
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
    std::vector<char> lData = ReadFile(aFilePath);

    if (lData.size() > 0)
    {

        uint32_t lNumElements = 0;
        ::memcpy(&lNumElements, lData.data(), sizeof(lNumElements));

        mScene.mStorkesArray.clear();
        mScene.mStorkesArray.resize(lNumElements);
        ::memcpy(mScene.mStorkesArray.data(), lData.data() + sizeof(lNumElements), lNumElements * sizeof(TStrokeInfo));
        mScene.SetDirty();
    }
}
