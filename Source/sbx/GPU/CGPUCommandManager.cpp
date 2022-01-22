/*
 * @file    GPUCommandManager.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CGPUCommandManager.h"

#include <sbx/System/CApplication.h>

#include <sbx/Messaging/CChannel.h>

#include <sbx/GPU/OpenGL/GLCommandAPI.h>
#include <sbx/Library/FList.h>
#include <chrono>
#include <thread>

namespace
{
    UID(kernel_max_wait_ms);

    sbx::CGPUCommandManager* sGPUCommandManager { nullptr };
}

namespace sbx
{
    void InitGPUCommandManager ()
    {
        if(!sGPUCommandManager)
        {
            sGPUCommandManager = SBX_NEW(sbx::CGPUCommandManager);
            sGPUCommandManager->Init();
        }
    }

    void EndGPUCommandManager ()
    {
        if(sGPUCommandManager)
        {
            SBX_DELETE(sGPUCommandManager);
        }
    }

    CGPUCommandManager* GetGPUCommandManager ()
    {
        return sGPUCommandManager;
    }

    struct CGPUCommandManager::TImpl
    {
        FList< TGPUCommand* > mQueue;
        float mCommandTimeThreshold {16};
    };

    CGPUCommandManager::CGPUCommandManager()
        : mImpl(SBX_NEW(TImpl))
    {
    }

    CGPUCommandManager::~CGPUCommandManager()
    {
        End();
        SBX_DELETE(mImpl);
        mImpl = 0;
    }
    
    bool CGPUCommandManager::Init()
    {
        //Register all gl create commands
        gl::GLInitCommandManager(this);

        //Register callbacks
        CChannel* lAppChannel = CApplication::GetInstance()->GetChannel();
        lAppChannel->RegisterCallback(this, &CGPUCommandManager::OnPreRender);

         mImpl->mCommandTimeThreshold = CApplication::GetInstance()->GetConfigParam<float>(uid(kernel_max_wait_ms), mImpl->mCommandTimeThreshold);

        return true;
    }

    void CGPUCommandManager::End()
    {
        CChannel* lAppChannel = CApplication::GetInstance()->GetChannel();
        lAppChannel->UnregisterCallback(this, &CGPUCommandManager::OnPreRender);
        gl::GLEndCommandManager();
    }

    void CGPUCommandManager::QueueCommand(TGPUCommand* aCommand)
    {
        //add to queue
        //TODO: check if there is a way to discard commands in a more optimized way
        if(aCommand->mOwnerId != 0)
        {
            for(auto lIt = mImpl->mQueue.begin(); lIt != mImpl->mQueue.end(); ++lIt)
            {
                if(((*lIt)->mOwnerId) == (aCommand->mOwnerId) && 
                    ((*lIt)->mSubIndex) == (aCommand->mSubIndex))
                {
                    //discard repeated commands
                    gl::GLDeleteCommand(*lIt);
                    lIt = mImpl->mQueue.erase(lIt);
                    break;
                }
            }
        }
        mImpl->mQueue.push_back(aCommand);
    }

    void CGPUCommandManager::CancelCommandQueue()
    {
        //Cancel all commands with owner
        //TODO: this funcion is a mistake, we should replace by CancelCommandsFromOwner only and cancel every node commands from it's generator destructor
        for(auto lIt = mImpl->mQueue.begin(); lIt != mImpl->mQueue.end(); )
        {
            TGPUCommand* lCommand = *lIt;
            if(lCommand->mOwnerId != 0)
            {
                gl::GLDeleteCommand(lCommand);
                lIt = mImpl->mQueue.erase(lIt);
            }
            else
            {
                lIt++;
            }
        }
        //mImpl->mQueue.clear();
    }

    void CGPUCommandManager::OnPreRender(TMsgAppPreRender const * aMsg)
    {
        double lTotalTime = 0;
        //iterate commands, but after total time threshold is reached, only execute commands without owner (non cancellable)
        for(auto lIt = mImpl->mQueue.begin(); lIt != mImpl->mQueue.end(); )
        {
            TGPUCommand* lCommand = *lIt;
            //TODO: make threshold configurable
            if((lTotalTime < mImpl->mCommandTimeThreshold) || (lCommand->mOwnerId == 0))
            {
                std::chrono::high_resolution_clock::time_point lStartTime = std::chrono::high_resolution_clock::now();
                lCommand->Execute();
                if(lCommand->mCallback)
                {
                    lCommand->mCallback();
                }
                gl::GLDeleteCommand(lCommand);
                std::chrono::high_resolution_clock::time_point lEndTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> lTimeSpan = lEndTime - lStartTime;
                lTotalTime += lTimeSpan.count();
                lIt = mImpl->mQueue.erase(lIt);
            }
            else
            {
                ++lIt;
            }
        }
    }

    size_t CGPUCommandManager::GetTextureMemoryAllocated () const
    {
        return gl::GLGetTextureMemoryAllocated();
    }
}