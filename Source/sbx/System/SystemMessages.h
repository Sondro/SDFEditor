/*
 * @file    SystemMessages.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_SYSTEM_MESSAGES_H__
#define __SBX_SYSTEM_MESSAGES_H__

#include <functional> 

struct TMsgAppPreUpdate{};
struct TMsgAppUpdate
{
    double mElapsedFrameMillis;
    TMsgAppUpdate(double aElapsedFrameMillis)
        : mElapsedFrameMillis(aElapsedFrameMillis)
    {;}
};
struct TMsgAppPostUpdate{};
struct TMsgAppPreRender{};
struct TMsgAppRender{};
struct TMsgAppPostRender{};
struct TMsgAppRenderGUI{};

struct TMsgCheckPendingChanges
{
    std::function<void(bool)> mSavePendingChangesCallback;

    TMsgCheckPendingChanges(std::function<void(bool)> const & aCallback)
        : mSavePendingChangesCallback(aCallback)
    {;}
};

#endif // __SBX_SYSTEM_MESSAGES_H__