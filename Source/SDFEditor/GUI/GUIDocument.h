
#pragma once

#include <functional>

class CToolApp;

namespace GUI
{
    using TYesNoDialogCallback = std::function<void(bool aYes)>;
    void ModalYesNoDialog(const char* aName, const char* aText, TYesNoDialogCallback aCallback);

    void ConfigureFileDialogsIcons();

    void RequestOpenFile(CToolApp& aToolApp);
    void RequestSaveFile(CToolApp& aToolApp);
    void RequestNewFile(CToolApp& aToolApp);

    void LaunchOpenFileDialog(CToolApp& aToolApp);
    void LaunchSaveFileDialog(CToolApp& aToolApp);

    void WantCloseDocument(CToolApp& aToolApp);

    void DrawDocOptionsBar(CToolApp & aToolApp);
    void DrawFileDialogs(CToolApp& aToolApp);
}