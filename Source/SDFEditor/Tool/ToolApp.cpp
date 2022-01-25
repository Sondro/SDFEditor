
/*
    MAIN TODO:

    - [DONE] Manage stroke list in cpu with more attributes (different layout) than in gpu

    - [DONE] Names for strokes (GUI)

    - [DONE] Separate strokes list and from properties panel (GUI)
        > [DONE] stroke selection in list
    
    - Primitive selection menu (GUI)

    - Display properties layout per primitivie (GUI)

    - Implement ImGuizmo (GUI)

    - Stroke Rotation (Data + Shader)

    - Stroke selection with bounding box raycast
        > Draw bounding box (GUI)

    - Scene serializaion/deserialization + save/load

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

#include "SDFEditor/GUI/GUIStrokesPanel.h"


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
    // TODO: Update scene with ui
    DrawStrokesPanel(mScene);

    UpdateCamera();

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

        static bool lActiveReload = false;
        if (!lActiveReload && ImGui::IsKeyPressed(294))
        {
            lActiveReload = true;
            mRenderer.ReloadShaders();
            mScene.SetDirty();
        }
        else if (ImGui::IsKeyReleased(294))
        {
            lActiveReload = false;
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
