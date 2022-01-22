/*
 * @file    FileWatcher.h
 * @author  David Gallardo Moreno
 * @brief   Watch for file changes
 */

#ifndef __SBX_FILEWATCHER_H__
#define __SBX_FILEWATCHER_H__

#include <sbx/FileSystem/File.h>
#include <functional>

namespace sbx
{
    typedef std::function< void(TPath const & aFile, void*) > TFileWatcherCallback;

    class CFileWatcher
    {
    public:
        ~CFileWatcher();
        bool Init(TPath const & aFile, TFileWatcherCallback aCallback, void* aUserData = nullptr);
        void End();

        void Check();
    private:
        TPath mFile;
        uint64_t mLastUpdateTimestamp;
        TFileWatcherCallback mCallback;
        void* mUserData;
    };
} // sbx

#endif // __SBX_FILEWATCHER_H__