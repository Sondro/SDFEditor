/*
 * @file    FileWatcher.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CFileWatcher.h"
#include <sbx/System/CApplication.h>
#include <sbx/System/SystemMessages.h>
#include <sbx/Messaging/CChannel.h>
#include <sbx/Library/FList.h>
#include <list>

namespace
{
    class CFileWatcherRegistry
    {
    public:
        CFileWatcherRegistry()
            : mElapsedTime(0)
        {
            sbx::CApplication::GetInstance()->GetChannel()->RegisterCallback(this, &CFileWatcherRegistry::OnAppUpdate);
        }

        void OnAppUpdate(const TMsgAppUpdate* aMsg) 
        {
            mElapsedTime += aMsg->mElapsedFrameMillis;
            if(mElapsedTime > 500)
            {
                mElapsedTime = 0;
                for(auto lWatcher : mWatchersList)
                {
                    lWatcher->Check();
                }
            }
        }

        void RegisterWatcher(sbx::CFileWatcher* aWatcher)
        {
            mWatchersList.push_back(aWatcher);
        }

        bool UnregisterWatcher(sbx::CFileWatcher* aWatcher)
        {
            auto lIt = std::find(mWatchersList.begin(), mWatchersList.end(), aWatcher);
            if(lIt != mWatchersList.end())
            {
                mWatchersList.erase(lIt);
                return (mWatchersList.size() == 0);
            }

            return false;
        }

    private:
        std::list < sbx::CFileWatcher* > mWatchersList;
        double      mElapsedTime;
    };

    CFileWatcherRegistry* gFileWatcherRegistry { nullptr };
}

namespace sbx
{
    CFileWatcher::~CFileWatcher()
    {
        End();
    }

    bool CFileWatcher::Init(TPath const & aFile, TFileWatcherCallback aCallback, void* aUserData)
    {
        End();

        mFile = aFile;
        mLastUpdateTimestamp = aFile.GetLastWriteTime();
        mCallback = aCallback;
        mUserData = aUserData;
        if(!gFileWatcherRegistry)
        {
            gFileWatcherRegistry = SBX_NEW(CFileWatcherRegistry);
        }
        gFileWatcherRegistry->RegisterWatcher(this);
        return true;
    }
    
    void CFileWatcher::End()
    {
        if(gFileWatcherRegistry && gFileWatcherRegistry->UnregisterWatcher(this))
        {
            SBX_DELETE(gFileWatcherRegistry);
            gFileWatcherRegistry = nullptr;
        }
    }

    void CFileWatcher::Check()
    {
        uint64_t lCurrentLastWrite = mFile.GetLastWriteTime();
        if(mLastUpdateTimestamp < lCurrentLastWrite)
        {
            mLastUpdateTimestamp = lCurrentLastWrite;
            if(mCallback)
            {
                mCallback(mFile, mUserData);
            }
        }
    }

} // sbx