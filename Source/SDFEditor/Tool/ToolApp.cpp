
#include "ToolApp.h"

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

    if (mScene.IsDirty())
    {
        mRenderer.UpdateSceneData(mScene);
        mScene.CleanDirtyFlag();
    }
}

void CToolApp::Render()
{
    mRenderer.RenderFrame();
}
