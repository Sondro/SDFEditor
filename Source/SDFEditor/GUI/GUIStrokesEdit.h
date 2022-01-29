
#pragma once

class CScene;

namespace GEditor
{
    void ResetSelection();

    void DrawStrokesPanel(CScene& aScene);
    void DrawStrokesGuizmos(CScene& aScene);

    void RaycastSelectStroke(CScene& aScene);
}
