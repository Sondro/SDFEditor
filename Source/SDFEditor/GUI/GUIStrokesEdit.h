
#pragma once

class CScene;

namespace GEditor
{
    void ResetSelection(CScene& aScene);

    void DrawStrokesPanel(CScene& aScene);
    void DrawStrokesGuizmos(CScene& aScene);

    void RaycastSelectStroke(CScene& aScene);
}
