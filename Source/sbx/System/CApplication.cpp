/*
 * @file    CApplication.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CApplication.h"
#include <sbx/Messaging/CChannel.h>
#include <sbx/FileSystem/File.h>
#include <sbx/FileSystem/ResourceUtils.h>
#include <sbx/System/SystemUtils.h>
#include <thread>


#include <fstream>
#include <sstream>
#include <Windows.h>

//GetThisPath.cpp
//#include <Shlwapi.h>
//#pragma comment(lib, "shlwapi.lib")

struct TConfigParam
{
    int32_t             mIntValue;
    float               mFloatValue;
    bool                mBoolValue;
    sbx::TUniqueIdCS    mUniqueIdCSValue;

    TConfigParam(){;}
    explicit TConfigParam(int32_t aValue) : mIntValue(aValue), mFloatValue(float(aValue)), mBoolValue(aValue != 0) {}
    explicit TConfigParam(float aValue) : mIntValue(int32_t(aValue)), mFloatValue(aValue), mBoolValue(aValue != 0.f) {}
    explicit TConfigParam(bool aValue) : mIntValue(aValue ? 1 : 0), mFloatValue(aValue ? 1.f : 0.f), mBoolValue(aValue) {}
    explicit TConfigParam(sbx::TUniqueIdCS aValue) : mUniqueIdCSValue(aValue), mIntValue(0), mFloatValue( 0.f), mBoolValue(false) {}
    explicit TConfigParam(const char* aValue) : mUniqueIdCSValue(sbx::TUniqueIdCS(aValue)), mIntValue(0), mFloatValue( 0.f), mBoolValue(false) {}
};

namespace sbx
{
    struct CApplication::TData
    {
        CChannel        mChannel;
        static std::thread::id sMainThreadId;
        bool            mRunning;
        HWND            hWnd;
        TWindowStatus   mMainWindowStatus;
        FMap< TUniqueId, TConfigParam > mConfigParams;
        TPath           mMainWindowCachePath;

        std::string mWorkingDir;

        //TODO: take a look at https://docs.microsoft.com/es-es/windows/desktop/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath
        std::string mDocumentsDir;
        std::string mLastUserDir;

        TData()
            : mRunning(true)
        {;}

        ~TData(){;}

        bool LoadConfig(TPath const & aConfigFile)
        {
            if(aConfigFile.IsFile())
            {
                CResource lConfigRes;
                sbxJsonFileToRes(aConfigFile, lConfigRes);

                for(uint32_t lConfigIdx = 0; lConfigIdx < lConfigRes.Count(); ++lConfigIdx)
                {
                    TUniqueIdCS lKey = lConfigRes.DictKeyAt(lConfigIdx);
                    CResource lValue = lConfigRes.DictValueAt(lConfigIdx);

                    if(lValue.GetType() == EResourceType::eBoolean)
                    {
                        mConfigParams[TUniqueId(lKey.GetStr())] = TConfigParam(lValue.AsBool());
                    }
                    else if(lValue.IsReal())
                    {
                        mConfigParams[TUniqueId(lKey.GetStr())] = TConfigParam(lValue.AsFloat());
                    }
                    else if(lValue.IsInteger())
                    {
                         mConfigParams[TUniqueId(lKey.GetStr())] = TConfigParam(lValue.AsInt32());
                    }
                    if(lValue.GetType() == EResourceType::eString)
                    {
                        mConfigParams[TUniqueId(lKey.GetStr())] = TConfigParam(lValue.AsConstStr());
                    }
                }

                return true;
            }
            return false;
        }
    };

    std::thread::id CApplication::TData::sMainThreadId;

    CApplication::CApplication()
        : mData(0)
    {
        mData = SBX_NEW(TData);
    }

    CApplication::~CApplication()
    {
        SBX_DELETE(mData);
    }

    bool CApplication::Init(void* aNativeHandle)
    {
        CApplication::TData::sMainThreadId = std::this_thread::get_id();
        mData->hWnd = (HWND)aNativeHandle;

        char lWorkingDirBuffer[2048];
        sys::GetCurrentDir(lWorkingDirBuffer, sizeof(lWorkingDirBuffer));
        mData->mWorkingDir = lWorkingDirBuffer;
        const TPath lWorkingPath(mData->mWorkingDir.c_str());

        mData->LoadConfig(lWorkingPath.Append("/tgconfig.json"));

        mData->mMainWindowCachePath = lWorkingPath.Append("/Cache/mws000400.cache");
        mData->mMainWindowStatus.LoadFromFile(mData->mMainWindowCachePath);
        
        return true;
    }

    void CApplication::End()
    {
        mData->mMainWindowStatus.SaveToFile(mData->mMainWindowCachePath);
    }

    CChannel* CApplication::GetChannel() const
    {
        return &mData->mChannel;
    }

    const char* CApplication::GetWorkingDirectory () const
    {
        return mData->mWorkingDir.c_str();
    }

    void CApplication::Update (double aElapsedFrameMillis)
    {
        mData->mChannel.BroadcastMessage(TMsgAppPreUpdate());
        mData->mChannel.BroadcastMessage(TMsgAppUpdate(aElapsedFrameMillis));
        mData->mChannel.BroadcastMessage(TMsgAppPostUpdate());
    }

    void CApplication::Render ()
    {
        mData->mChannel.BroadcastMessage(TMsgAppPreRender());
        mData->mChannel.BroadcastMessage(TMsgAppRender());
        mData->mChannel.BroadcastMessage(TMsgAppPostRender());
        mData->mChannel.BroadcastMessage(TMsgAppRenderGUI());
    }

    bool CApplication::IsMainThread ()
    {
         return std::this_thread::get_id() == CApplication::TData::sMainThreadId;
    }

    void CApplication::ShutDown()
    {
        mData->mRunning = false;
    }

    bool CApplication::IsRunning () const
    {
        return mData->mRunning;
    }

    void* CApplication::GetNativeHandle () const
    {
        return mData->hWnd;
    }

    TWindowStatus* CApplication::GetMainWindowStatus ()
    {
        return &mData->mMainWindowStatus;
    }

    bool CApplication::GetBoolConfigParam (TUniqueId const & aParam, bool aDefault) const
    {
        auto lIt = mData->mConfigParams.find(aParam);
        if(lIt != mData->mConfigParams.end())
        {
            return lIt->second.mBoolValue;
        }
        else
        {
            SBX_ERROR("Parameter %s not defined as Bool", aParam.GetStr());
        }

        return aDefault;
    }

    int32_t CApplication::GetIntegerConfigParam (TUniqueId const & aParam, int32_t aDefault) const
    {
        auto lIt = mData->mConfigParams.find(aParam);
        if(lIt != mData->mConfigParams.end())
        {
            return lIt->second.mIntValue;
        }
        else
        {
            SBX_ERROR("Parameter %s not defined as Integer", aParam.GetStr());
        }

        return aDefault;
    }

    float CApplication::GetFloatConfigParam (TUniqueId const & aParam, float aDefault) const
    {
        auto lIt = mData->mConfigParams.find(aParam);
        if(lIt != mData->mConfigParams.end())
        {
            return lIt->second.mFloatValue;
        }
        else
        {
            SBX_ERROR("Parameter %s not defined as Float", aParam.GetStr());
        }

        return aDefault;
    }

    TUniqueIdCS CApplication::GetStringConfigParam (TUniqueId const & aParam, TUniqueIdCS aDefault) const
    {
        auto lIt = mData->mConfigParams.find(aParam);
        if(lIt != mData->mConfigParams.end())
        {
            return lIt->second.mUniqueIdCSValue;
        }
        else
        {
            SBX_ERROR("Parameter %s not defined as String", aParam.GetStr());
        }

        return aDefault;
    }

    template <> bool    CApplication::GetConfigParam<bool>      (TUniqueId const & aParam, bool     aDefault) const { return GetBoolConfigParam     (aParam, aDefault); }
    template <> int32_t CApplication::GetConfigParam<int32_t>   (TUniqueId const & aParam, int32_t  aDefault) const { return GetIntegerConfigParam  (aParam, aDefault); }
    template <> float   CApplication::GetConfigParam<float>     (TUniqueId const & aParam, float    aDefault) const { return GetFloatConfigParam    (aParam, aDefault); }
    template <> TUniqueIdCS CApplication::GetConfigParam<TUniqueIdCS> (TUniqueId const & aParam, TUniqueIdCS aDefault) const { return GetStringConfigParam (aParam, aDefault); }
} // sbx