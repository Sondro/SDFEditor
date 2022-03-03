// Copyright (c) 2022 David Gallardo and SDFEditor Project

#pragma once

class CScene;

namespace GUI
{
    void ResetSelection(CScene& aScene);

    void DrawMainPanel(CScene& aScene);
    void DrawStrokesGuizmos(CScene& aScene);

    void RaycastSelectStroke(CScene& aScene);
}
